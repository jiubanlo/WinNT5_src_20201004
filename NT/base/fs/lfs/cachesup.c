/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    CacheSup.c

Abstract:

    This module provides an interface with the cache manager.

Author:

    Brian Andrew    [BrianAn]   20-June-1991

Revision History:

--*/

#include "lfsprocs.h"
#include <ntdddisk.h>
#include <NtIoLogc.h>
#include <elfmsg.h>

//
//  The debug trace level
//

#define Dbg                             (DEBUG_TRACE_CACHE_SUP)

//
//  Following is used to generate a sequence number when the cache manager
//  gives us a page of zeroes.  Otherwise all of the sequence numbers will
//  be 1.
//

USHORT LfsUsaSeqNumber;

LARGE_INTEGER LiMinus1 = {(ULONG)-1,-1};


BOOLEAN
LfsIsRestartPageHeaderValid (
    IN LONGLONG FileOffset,
    IN PLFS_RESTART_PAGE_HEADER PageHeader,
    OUT PBOOLEAN LogPacked
    );

BOOLEAN
LfsIsRestartAreaValid (
    IN PLFS_RESTART_PAGE_HEADER PageHeader,
    IN BOOLEAN LogPacked
    );

BOOLEAN
LfsIsClientAreaValid (
    IN PLFS_RESTART_PAGE_HEADER PageHeader,
    IN BOOLEAN LogPacked,
    IN BOOLEAN UsaError
    );

VOID
LfsFindFirstIo (
    IN PLFCB Lfcb,
    IN LSN TargetLsn,
    IN BOOLEAN RestartLsn,
    IN PLBCB FirstLbcb,
    OUT PLBCB *NextLbcb,
    OUT PLONGLONG FileOffset,
    OUT PBOOLEAN ContainsLastEntry,
    OUT PBOOLEAN LfsRestart,
    OUT PBOOLEAN UseTailCopy,
    OUT PULONG IoBlocks
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, LfsCopyReadLogRecord)
#pragma alloc_text(PAGE, LfsFindFirstIo)
#pragma alloc_text(PAGE, LfsIsClientAreaValid)
#pragma alloc_text(PAGE, LfsIsRestartAreaValid)
#pragma alloc_text(PAGE, LfsIsRestartPageHeaderValid)
#pragma alloc_text(PAGE, LfsPinOrMapData)
#pragma alloc_text(PAGE, LfsPinOrMapLogRecordHeader)
#pragma alloc_text(PAGE, LfsReadRestart)
#endif


NTSTATUS
LfsPinOrMapData (
    IN PLFCB Lfcb,
    IN LONGLONG FileOffset,
    IN ULONG Length,
    IN BOOLEAN PinData,
    IN BOOLEAN AllowErrors,
    IN BOOLEAN IgnoreUsaErrors,
    OUT PBOOLEAN UsaError,
    OUT PVOID *Buffer,
    OUT PBCB *Bcb
    )

/*++

Routine Description:

    This routine will pin or map a portion of the log file.

Arguments:

    Lfcb - This is the file control block for the log file.

    FileOffset - This is the offset of the log page to pin.

    Length - This is the length of the data to access.

    PinData - Boolean indicating if we are to pin or map this data.

    AllowErrors - This boolean indicates whether we should raise on an
        I/O error or return on an I/O error.

    IgnoreUsaErrors - Boolean indicating whether we will raise on Usa
        errors.

    UsaError - Address to store whether the Usa had an error.

    Buffer - This is the address to store the address of the data.

    Bcb - This is the Bcb for this operation.

Return Value:

    NTSTATUS - The result of the I/O.

--*/

{
    volatile NTSTATUS Status;
    ULONG Signature;
    BOOLEAN Result = FALSE;

    Status = STATUS_SUCCESS;

    PAGED_CODE();

    DebugTrace( +1, Dbg, "LfsPinReadLogPage:  Entered\n", 0 );
    DebugTrace(  0, Dbg, "Lfcb              -> %08lx\n", Lfcb );
    DebugTrace(  0, Dbg, "FileOffset (Low)  -> %08lx\n", FileOffset.HighPart );
    DebugTrace(  0, Dbg, "FileOffset (High) -> %08lx\n", FileOffset.LowPart );
    DebugTrace(  0, Dbg, "Length            -> %08lx\n", Length );
    DebugTrace(  0, Dbg, "PinData           -> %04x\n", PinData );
    DebugTrace(  0, Dbg, "AllowErrors       -> %08x\n", AllowErrors );
    DebugTrace(  0, Dbg, "IgnoreUsaErrors   -> %04x\n", IgnoreUsaErrors );

    if (FileOffset + Length > Lfcb->FileSize) {
        ExRaiseStatus( STATUS_DISK_CORRUPT_ERROR );
    }

    //
    //  Use a try-finally to facilitate cleanup.
    //

    try {

        //
        //  Use a try-except to catch cache manager errors.
        //

        try {

            //
            //  We call the cache to perform the work.
            //

            if (PinData) {

                Result = CcPinRead( Lfcb->FileObject,
                                    (PLARGE_INTEGER)&FileOffset,
                                    Length,
                                    TRUE,
                                    Bcb,
                                    Buffer );

            } else {

                Result = CcMapData( Lfcb->FileObject,
                                    (PLARGE_INTEGER)&FileOffset,
                                    Length,
                                    TRUE,
                                    Bcb,
                                    Buffer );
            }

            //
            //  Capture the signature now while we are within the
            //  exception filter.
            //

            Signature = *((PULONG) *Buffer);

        } except( LfsExceptionFilter( GetExceptionInformation() )) {

            Status = GetExceptionCode();
            if (Result) {
                CcUnpinData( *Bcb );
                *Bcb = NULL;
            }
        }

        *UsaError = FALSE;

        //
        //  If an error occurred, we raise the status.
        //

        if (!NT_SUCCESS( Status )) {

            if (!AllowErrors) {

                DebugTrace( 0, Dbg, "Read on log page failed -> %08lx\n", Status );
                ExRaiseStatus( Status );
            }

            //
            //  Check that the update sequence array for this
            //  page is valid.
            //

        } else if (Signature == LFS_SIGNATURE_BAD_USA_ULONG) {

            //
            //  If we don't allow errors, raise an error status.
            //

            if (!IgnoreUsaErrors) {

                DebugTrace( 0, Dbg, "Usa error on log page\n", 0 );
                ExRaiseStatus( STATUS_DISK_CORRUPT_ERROR );
            }

            *UsaError = TRUE;
        }

    } finally {

        DebugUnwind( LfsPinOrMapData );

        DebugTrace(  0, Dbg, "Buffer    -> %08lx\n", *Buffer );
        DebugTrace(  0, Dbg, "Bcb       -> %08lx\n", *Bcb );

        DebugTrace( -1, Dbg, "LfsPinOrMapData:  Exit -> %08lx\n", Status );
    }

    return Status;
}


VOID
LfsPinOrMapLogRecordHeader (
    IN PLFCB Lfcb,
    IN LSN Lsn,
    IN BOOLEAN PinData,
    IN BOOLEAN IgnoreUsaErrors,
    OUT PBOOLEAN UsaError,
    OUT PLFS_RECORD_HEADER *RecordHeader,
    OUT PBCB *Bcb
    )

/*++

Routine Description:

    This routine will pin or map a log record for read access.

Arguments:

    Lfcb - This is the file control block for the log file.

    Lsn - This is the Lsn whose header should be pinned.

    PinData - Boolean indicating if we are to pin or map this data.

    IgnoreUsaErrors - Boolean indicating whether we will raise on Usa
        errors.

    UsaError - Address to store whether the Usa had an error.

    RecordHeader - This is the address to store the address of the pinned data.

    Bcb - This is the Bcb for this pin operation.

Return Value:

    None.

--*/

{
    PLFS_RECORD_PAGE_HEADER LogPageHeader;
    LONGLONG LogPage;
    ULONG PageOffset;

    PAGED_CODE();

    DebugTrace( +1, Dbg, "LfsPinOrMapLogRecordHeader:  Entered\n", 0 );
    DebugTrace(  0, Dbg, "Lfcb       -> %08lx\n", Lfcb );
    DebugTrace(  0, Dbg, "Lsn (Low)  -> %08lx\n", Lsn.HighPart );
    DebugTrace(  0, Dbg, "Lsn (High) -> %08lx\n", Lsn.LowPart );
    DebugTrace(  0, Dbg, "PinData           -> %04x\n", PinData );
    DebugTrace(  0, Dbg, "IgnoreUsaErrors   -> %04x\n", IgnoreUsaErrors );

    //
    //  Compute the log page and the offset of the log record header
    //  in the log page.
    //

    LfsTruncateLsnToLogPage( Lfcb, Lsn, &LogPage );
    PageOffset = LfsLsnToPageOffset( Lfcb, Lsn );

    //
    //  Call the cache manager to pin the page.
    //

    LfsPinOrMapData( Lfcb,
                     LogPage,
                     (ULONG)Lfcb->LogPageSize,
                     PinData,
                     FALSE,
                     IgnoreUsaErrors,
                     UsaError,
                     (PVOID *) &LogPageHeader,
                     Bcb );

    //
    //  The actual offset we need is at PageOffset from the start of the page.
    //

    *RecordHeader = Add2Ptr( LogPageHeader, PageOffset, PLFS_RECORD_HEADER );

    DebugTrace(  0, Dbg, "Record Header -> %08lx\n", *RecordHeader );
    DebugTrace(  0, Dbg, "Bcb           -> %08lx\n", *Bcb );

    DebugTrace( -1, Dbg, "LfsPinOrMapLogRecordHeader:  Exit\n", 0 );

    return;
}


VOID
LfsCopyReadLogRecord (
    IN PLFCB Lfcb,
    IN PLFS_RECORD_HEADER RecordHeader,
    OUT PVOID Buffer
    )

/*++

Routine Description:

    This routines copies a log record from the file to a buffer.  The log
    record may span several log pages and may even wrap in the file.

Arguments:

    Lfcb - A pointer to the control block for the log file.

    RecordHeader - Pointer to the log record header for this log record.

    Buffer - Pointer to the buffer to store the log record.

Return Value:

    None.

--*/

{
    PBCB Bcb = NULL;
    BOOLEAN UsaError;

    PLFS_RECORD_PAGE_HEADER PageHeader;

    LONGLONG LogPageFileOffset;
    ULONG LogPageOffset;

    ULONG RemainingTransferBytes;

    PAGED_CODE();

    DebugTrace( +1, Dbg, "LfsCopyReadLogRecord:  Entered\n", 0 );
    DebugTrace(  0, Dbg, "Lfcb           -> %08lx\n", Lfcb );
    DebugTrace(  0, Dbg, "RecordHeader   -> %08lx\n", RecordHeader );
    DebugTrace(  0, Dbg, "Buffer         -> %08lx\n", Buffer );

    //
    //  We find the file offset of the log page containing the start of
    //  this log record, the offset within the page to start the transfer from,
    //  the number of bytes to transfer on this page and the starting
    //  position in the buffer to begin the transfer to.
    //

    LfsTruncateLsnToLogPage( Lfcb, RecordHeader->ThisLsn, &LogPageFileOffset );
    LogPageOffset = LfsLsnToPageOffset( Lfcb, RecordHeader->ThisLsn ) + Lfcb->RecordHeaderLength;

    RemainingTransferBytes = RecordHeader->ClientDataLength;

    //
    //  Use a try-finally to facilitate cleanup.
    //

    try {

        //
        //  While there are more bytes to transfer, we continue to attempt to
        //  perform the read.
        //

        while (TRUE) {

            ULONG RemainingPageBytes;

            BOOLEAN Wrapped;

            RemainingPageBytes = (ULONG)Lfcb->LogPageSize - LogPageOffset;

            //
            //  We compute the number of bytes to read from this log page and
            //  call the cache package to perform the transfer.
            //

            if (RemainingTransferBytes <= RemainingPageBytes) {

                RemainingPageBytes = RemainingTransferBytes;
            }

            RemainingTransferBytes -= RemainingPageBytes;

            //
            //  Unpin any previous buffer.
            //

            if (Bcb != NULL) {

                CcUnpinData( Bcb );
                Bcb = NULL;
            }

            LfsPinOrMapData( Lfcb,
                             LogPageFileOffset,
                             (ULONG)Lfcb->LogPageSize,
                             FALSE,
                             FALSE,
                             TRUE,
                             &UsaError,
                             (PVOID *) &PageHeader,
                             &Bcb );

            //
            //  The last Lsn on this page better be greater or equal to the Lsn we
            //  are copying.
            //

            if ( PageHeader->Copy.LastLsn.QuadPart < RecordHeader->ThisLsn.QuadPart ) {

                ExRaiseStatus( STATUS_DISK_CORRUPT_ERROR );
            }

            RtlCopyMemory( Buffer,
                           Add2Ptr( PageHeader, LogPageOffset, PVOID ),
                           RemainingPageBytes );

            //
            //  If there are no more bytes to transfer, we exit the loop.
            //

            if (RemainingTransferBytes == 0) {

                //
                //  Our log record better not span this page.
                //

                if (!FlagOn( PageHeader->Flags, LOG_PAGE_LOG_RECORD_END )

                    || (FlagOn( Lfcb->Flags, LFCB_PACK_LOG )
                        && ( RecordHeader->ThisLsn.QuadPart > PageHeader->Header.Packed.LastEndLsn.QuadPart ))) {

                    ExRaiseStatus( STATUS_DISK_CORRUPT_ERROR );
                }

                break;
            }

            //
            //  If the page header indicates that the log record ended on this page,
            //  this is a disk corrupt condition.  For a packed page it means
            //  that the last Lsn and the last Ending Lsn are the same.
            //

            if (FlagOn( Lfcb->Flags, LFCB_PACK_LOG )) {

                //
                //  If there is no spanning log record this is an error.
                //

                if (( PageHeader->Copy.LastLsn.QuadPart == PageHeader->Header.Packed.LastEndLsn.QuadPart )

                    || ( RecordHeader->ThisLsn.QuadPart > PageHeader->Copy.LastLsn.QuadPart )) {

                    ExRaiseStatus( STATUS_DISK_CORRUPT_ERROR );
                }

                //
                //  For an unpacked page it simply means that the page
                //  contains the end of a log record.
                //

            } else if (FlagOn( PageHeader->Flags, LOG_PAGE_LOG_RECORD_END )) {

                ExRaiseStatus( STATUS_DISK_CORRUPT_ERROR );
            }

            //
            //  We find the start of the next log page and the offset within
            //  that page to start transferring bytes.
            //

            LfsNextLogPageOffset( Lfcb,
                                  LogPageFileOffset,
                                  &LogPageFileOffset,
                                  &Wrapped );

            LogPageOffset = (ULONG)Lfcb->LogPageDataOffset;

            //
            //  We also adjust our pointer in the user's buffer to transfer
            //  the next block to.
            //

            Buffer = Add2Ptr( Buffer, RemainingPageBytes, PVOID );
        }

    } finally {

        //
        //  Unpin any previous buffer.
        //

        if (Bcb != NULL) {

            CcUnpinData( Bcb );
            Bcb = NULL;
        }

        DebugTrace( -1, Dbg, "LfsCopyReadLogRecord:  Exit\n", 0 );
    }

    return;
}


VOID
LfsFlushLfcb (
    IN PLFCB Lfcb,
    IN LSN TargetLsn,
    IN BOOLEAN RestartLsn
    )

/*++

Routine Description:

    This routine is called to flush the current Lbcbs in on the Lfcb
    work queue.  It will flush up to the I/O which contains the desired
    TargetLsn.  It should be called with LfsIoState already set - this will be cleared
    on finish. At periodic points the sync event will be pulsed to wake up waiter threads

Arguments:

    Lfcb - This is the file control block for the log file.

    TargetLsn - This is the Lsn which is needed to be flushed to disk.
                if it is greater than the current lsn everything will get flushed
                
    Restart - if true the target lsn is an lfs restart pseudo lsn                

Return Value:

    None.

--*/

{
    PLBCB FirstLbcb;
    PLBCB ThisLbcb;
    PLBCB NextLbcb;

    PLBCB TargetLbcb;
    PULONG Signature;

    LONGLONG FileOffset;
    ULONG Length;

    BOOLEAN ValidLastLsn = FALSE;

    BOOLEAN ContainsLastEntry = FALSE;
    BOOLEAN LfsRestart;
    BOOLEAN UseTailCopy;
    
    ULONG IoBlocks;
    ULONG NewLfcbFlags = 0;

    PBCB MapPageBcb = NULL;

    LSN LastLsn;

    IO_STATUS_BLOCK Iosb;

    PBCB PageBcb = NULL;
    NTSTATUS FailedFlushStatus = STATUS_SUCCESS;
    LONGLONG FailedFlushOffset;

    KEVENT Event;

    PLFS_WAITER LfsWaiter;

    BOOLEAN OwnedExclusive;

    DebugTrace( +1, Dbg, "LfsFlushLfcb:  Entered\n", 0 );
    DebugTrace(  0, Dbg, "Lfcb          -> %08lx\n", Lfcb );

    //
    //  We'd absolutely hate for this to happen on a read only volume.
    //

    ASSERT(!(BooleanFlagOn( Lfcb->Flags, LFCB_READ_ONLY )));

    //
    //  Use a try-finally to facilitate cleanup.
    //

    OwnedExclusive = ExIsResourceAcquiredExclusiveLite( &Lfcb->Sync->Resource );

    try {

        //
        //  If there are no elements on the list, we are done.
        //

        if (IsListEmpty( &Lfcb->LbcbWorkque )) {
            leave;
        }

        KeInitializeEvent( &Event, SynchronizationEvent, FALSE );

        //
        //  Convert max lsn to the last lsn currently on the list - current lsn will
        //  not change since we hold the lfcb at least shared at this point and writers
        //  need it exclusive
        //  

        if (TargetLsn.QuadPart > Lfcb->RestartArea->CurrentLsn.QuadPart) {

            ThisLbcb = CONTAINING_RECORD( Lfcb->LbcbWorkque.Blink,
                                          LBCB,
                                          WorkqueLinks );

            TargetLsn.QuadPart = ThisLbcb->LastLsn.QuadPart;
            RestartLsn = (BOOLEAN) LfsLbcbIsRestart( ThisLbcb );
        }


        //
        //  Remember the first Lbcb in the list.
        //

        FirstLbcb = CONTAINING_RECORD( Lfcb->LbcbWorkque.Flink,
                                       LBCB,
                                       WorkqueLinks );

        ASSERT( FirstLbcb != NULL );

        //
        //  We continue looping and performing I/o for as long as possible.
        //

        while (!ContainsLastEntry) {

            ASSERT( FirstLbcb != NULL );


            //
            //  Find the block of Lbcb's that make up the first I/O, remembering
            //  how many there are.  Also remember if this I/O contains the
            //  last element on the list when we were called.
            //

            LfsFindFirstIo( Lfcb,
                            TargetLsn,
                            RestartLsn,
                            FirstLbcb,
                            &NextLbcb,
                            &FileOffset,
                            &ContainsLastEntry,
                            &LfsRestart,
                            &UseTailCopy,
                            &IoBlocks );

            Length = IoBlocks * (ULONG) Lfcb->LogPageSize;
            if (UseTailCopy) {

                TargetLbcb = Lfcb->ActiveTail;
                Lfcb->ActiveTail = Lfcb->PrevTail;
                Lfcb->PrevTail = TargetLbcb;

                FileOffset = TargetLbcb->FileOffset;

            } else {

                TargetLbcb = FirstLbcb;
            }

            //
            //  Give up the Lfcb unless we are looking at an active page.
            //

            if (!UseTailCopy) {

                LfsReleaseLfcb( Lfcb );
            }

            //
            //  If this I/O involves the Lfs restart area, write it to the
            //  cache pages.
            //

            if (LfsRestart) {

                PLFS_RESTART_PAGE_HEADER RestartPage;

                ASSERT( !UseTailCopy && IoBlocks == 1);

                //
                //  Build the partial mdl to describe this lfs restart page from the permanently
                //  mapped piece of the log
                //

                RestartPage = Add2Ptr( Lfcb->LogHeadBuffer, FileOffset, PLFS_RESTART_PAGE_HEADER );
                IoBuildPartialMdl( Lfcb->LogHeadMdl, Lfcb->LogHeadPartialMdl, RestartPage, (ULONG)Lfcb->LogPageSize );

                //
                //  Initialize the restart page header.
                //

                Signature = (PULONG) &RestartPage->MultiSectorHeader.Signature;

                *Signature = LFS_SIGNATURE_RESTART_PAGE_ULONG;
                RestartPage->ChkDskLsn = LfsLi0;

                RestartPage->MultiSectorHeader.UpdateSequenceArrayOffset
                = Lfcb->RestartUsaOffset;

                RestartPage->MultiSectorHeader.UpdateSequenceArraySize
                = Lfcb->UsaArraySize;

                //
                //  Maintain the illusion that all systems have log page == system page
                //  on disk so we can migrate disks between different platforms
                //

                RestartPage->SystemPageSize = (ULONG)Lfcb->LogPageSize;
                RestartPage->LogPageSize = (ULONG)Lfcb->LogPageSize;

                RestartPage->RestartOffset = (USHORT) Lfcb->RestartDataOffset;
                RestartPage->MajorVersion = Lfcb->MajorVersion;
                RestartPage->MinorVersion = Lfcb->MinorVersion;

                //
                //  If the Lfcb indicates that the file has wrapped, then clear the
                //  first pass flag in the restart area.
                //

                if (FlagOn( Lfcb->Flags, LFCB_LOG_WRAPPED )) {

                    ClearFlag( ((PLFS_RESTART_AREA) FirstLbcb->PageHeader)->Flags, RESTART_SINGLE_PAGE_IO );
                    SetFlag( Lfcb->Flags, LFCB_MULTIPLE_PAGE_IO );
                }

                //
                //  Write the page header into the page and mark the page dirty.
                //

                RtlCopyMemory( Add2Ptr( RestartPage, Lfcb->RestartDataOffset, PVOID ),
                               FirstLbcb->PageHeader,
                               (ULONG)FirstLbcb->Length );

                LastLsn = FirstLbcb->LastLsn;
                ValidLastLsn = TRUE;

#ifdef LFS_CLUSTER_CHECK
                //
                //  Update the Lsn range on the disk.
                //

                *(Add2Ptr( RestartPage, 0xe00 - sizeof( ULONG ), PULONG )) = Lfcb->LsnRangeIndex + 1;
                *(Add2Ptr( RestartPage, 0xe00 + (sizeof( LSN ) * Lfcb->LsnRangeIndex * 2), PLSN )) = Lfcb->LsnAtMount;
                *(Add2Ptr( RestartPage, 0xe00 + (sizeof( LSN ) * (Lfcb->LsnRangeIndex * 2 + 1)), PLSN )) = Lfcb->LastFlushedLsn;

#endif
                //
                //  Use a system page size as the length we need to flush.
                //

                Length = (ULONG)Lfcb->LogPageSize;

                //
                //  Otherwise these are log record pages
                //

            } else {

                PLFS_RECORD_PAGE_HEADER RecordPageHeader;
                ULONG Count;

                //
                //  Mark the last Lsn fields for the page headers and each
                //  page's position in the transfer.  Also unpin all of the
                //  log pages.
                //


                ASSERT( UseTailCopy || FirstLbcb->FileOffset == FileOffset );

                ThisLbcb = FirstLbcb;

                for (Count=1; Count <= IoBlocks; Count++) {

                    if (UseTailCopy) {

                        //
                        //  Build the partial mdl to describe the tail (pin/pong) page
                        //  from the permanently mapped section of the log
                        //

                        RecordPageHeader = Add2Ptr( Lfcb->LogHeadBuffer, TargetLbcb->FileOffset, PLFS_RECORD_PAGE_HEADER );
                        IoBuildPartialMdl( Lfcb->LogHeadMdl, Lfcb->LogHeadPartialMdl, RecordPageHeader, (ULONG)Lfcb->LogPageSize );

                        //
                        //  Store the file offset of the real page in the header.
                        //  Also set the flag indicating the page is a tail copy.
                        //

                        RtlCopyMemory( RecordPageHeader,
                                       ThisLbcb->PageHeader,
                                       (ULONG)Lfcb->LogPageSize );

                        RecordPageHeader->Copy.FileOffset = ThisLbcb->FileOffset;

                    } else {

                        PUSHORT SeqNumber;

                        RecordPageHeader = (PLFS_RECORD_PAGE_HEADER) ThisLbcb->PageHeader;

                        //
                        //  If the sequence number is zero then this is probably a
                        //  page of zeroes produced by the cache manager.  In order
                        //  to insure that we don't have the same sequence number
                        //  on each page we will seed the sequence number.
                        //

                        SeqNumber = Add2Ptr( RecordPageHeader,
                                             Lfcb->LogRecordUsaOffset,
                                             PUSHORT );

                        if (*SeqNumber == 0) {

                            *SeqNumber = LfsUsaSeqNumber;
                            LfsUsaSeqNumber += 1;
                        }
                    }

                    //
                    //  We update all of fields as yet not updated.
                    //

                    RecordPageHeader->PagePosition = (USHORT) Count;
                    RecordPageHeader->PageCount = (USHORT) IoBlocks;

                    //
                    //  We set up the update sequence array for this structure.
                    //

                    Signature = (PULONG) &RecordPageHeader->MultiSectorHeader.Signature;
                    *Signature = LFS_SIGNATURE_RECORD_PAGE_ULONG;

                    RecordPageHeader->MultiSectorHeader.UpdateSequenceArrayOffset = Lfcb->LogRecordUsaOffset;
                    RecordPageHeader->MultiSectorHeader.UpdateSequenceArraySize = Lfcb->UsaArraySize;

                    //
                    //  Make sure the modified bit gets set in the pfn database.  The
                    //  cache manager should do this even for files we told him not to
                    //  lazy write.
                    //

                    if (!UseTailCopy) {

                        CcSetDirtyPinnedData( ThisLbcb->LogPageBcb, NULL );

                        //
                        //  We unpin any buffers pinned on this page.
                        //

                        CcUnpinDataForThread( ThisLbcb->LogPageBcb, ThisLbcb->ResourceThread );
                        ThisLbcb->LogPageBcb = NULL;
                    }

                    //
                    //  Remember the last lsn and its length if this is the final
                    //  page of an Lsn.
                    //

                    if (FlagOn( ThisLbcb->Flags, LOG_PAGE_LOG_RECORD_END )) {

                        LastLsn = ThisLbcb->LastEndLsn;
                        ValidLastLsn = TRUE;
                    }

                    //
                    //  Otherwise move to the next entry.
                    //

                    ThisLbcb = CONTAINING_RECORD( ThisLbcb->WorkqueLinks.Flink,
                                                  LBCB,
                                                  WorkqueLinks );
                }
            }

            //
            //  Remember the range we are flushing and find the second half of a page
            //  if necessary.
            //

            Lfcb->UserWriteData->FileOffset = FileOffset;
            Lfcb->UserWriteData->Length = Length;

            //
            //  For the loghead pages (2 lfs restart pages and 2 ping pong pages
            //  explicitly flush them down using the partial mdl we built
            //  The regular cc logic w/ UserWriteData  that pares the write down
            //  to the correct offsets works here as well
            //

            if (LfsRestart || UseTailCopy) {

                NTSTATUS Status;

                ASSERT( IoBlocks == 1 );

                //
                //  We can release the lfcb now that we've finished using the active page
                //  

                if (UseTailCopy) {
                    LfsReleaseLfcb( Lfcb );
                }

                Status = IoSynchronousPageWrite( Lfcb->FileObject,
                                                 Lfcb->LogHeadPartialMdl,
                                                 (PLARGE_INTEGER)&FileOffset,
                                                 &Event,
                                                 &Iosb );

                if (Status == STATUS_PENDING) {
                    Status = KeWaitForSingleObject( &Event, Executive, KernelMode, FALSE, NULL );
                }

                if (!NT_SUCCESS( Status ) || !NT_SUCCESS( Iosb.Status)) {

                    //
                    //  Record status if we haven't failed already and continue on
                    //

                    if (NT_SUCCESS( FailedFlushStatus )) {
                        if (!NT_SUCCESS( Status )) {
                            FailedFlushStatus = Status;
                        } else if (!NT_SUCCESS( Iosb.Status )) {
                            FailedFlushStatus = Iosb.Status;
                        }
                        FailedFlushOffset = FileOffset;

#ifdef LFS_CLUSTER_CHECK
                        //
                        //  Remember this to figure out naggin cluster problems.
                        //

                        if ((Status == STATUS_DEVICE_OFF_LINE) ||
                            (Iosb.Status == STATUS_DEVICE_OFF_LINE)) {

                            SetFlag( Lfcb->Flags, LFCB_DEVICE_OFFLINE_SEEN );
                        }

                        //
                        //  Remember all errors.
                        //

                        SetFlag( Lfcb->Flags, LFCB_FLUSH_FAILED );
#endif
                    }

#ifdef LFS_CLUSTER_CHECK
                } else if (Iosb.Information != 0) {

                    //
                    //  Once OFFLINE, always OFFLINE.
                    //

                    ASSERT( !FlagOn( Lfcb->Flags, LFCB_DEVICE_OFFLINE_SEEN ));

                    //
                    //  Catch the first write after a failed IO.
                    //

                    if (LfsTestBreakOnAnyError &&
                        FlagOn( Lfcb->Flags, LFCB_FLUSH_FAILED )) {

                        ASSERT( !LfsTestBreakOnAnyError ||
                                !FlagOn( Lfcb->Flags, LFCB_FLUSH_FAILED ));

                        ClearFlag( Lfcb->Flags, LFCB_FLUSH_FAILED );
                    }
#endif
                }

            } else {

                //
                //  This is a normal log page so flush through the cache
                //

                CcFlushCache( Lfcb->FileObject->SectionObjectPointer,
                              (PLARGE_INTEGER)&FileOffset,
                              Length,
                              &Iosb );

                if (!NT_SUCCESS( Iosb.Status )) {

                    LONG BytesRemaining = (LONG) Length;

                    //
                    //  If we get an error then try each individual page.
                    //

                    while (BytesRemaining > 0) {

                        //
                        //  Remember the range we are flushing and find the second half of a page
                        //  if necessary.
                        //

                        ASSERT( Length >= Lfcb->LogPageSize );

                        Lfcb->UserWriteData->FileOffset = FileOffset;
                        Lfcb->UserWriteData->Length = (ULONG)Lfcb->LogPageSize;

                        CcFlushCache( Lfcb->FileObject->SectionObjectPointer,
                                      (PLARGE_INTEGER)&FileOffset,
                                      (ULONG)Lfcb->LogPageSize,
                                      &Iosb );

                        if (!NT_SUCCESS( Iosb.Status )) {

                            if (NT_SUCCESS( FailedFlushStatus )) {
                                FailedFlushStatus = Iosb.Status;
                                FailedFlushOffset = FileOffset;
#ifdef LFS_CLUSTER_CHECK
                                //
                                //  Remember this to figure out naggin cluster problems.
                                //

                                if (FailedFlushStatus == STATUS_DEVICE_OFF_LINE) {

                                    SetFlag( Lfcb->Flags, LFCB_DEVICE_OFFLINE_SEEN );
                                }

                                //
                                //  Remember all errors.
                                //

                                SetFlag( Lfcb->Flags, LFCB_FLUSH_FAILED );
#endif
                            }
#ifdef LFS_CLUSTER_CHECK
                        } else if (Iosb.Information != 0) {

                            //
                            //  Once OFFLINE, always OFFLINE.
                            //

                            ASSERT( !FlagOn( Lfcb->Flags, LFCB_DEVICE_OFFLINE_SEEN ));

                            //
                            //  Catch the first write after a failed IO.
                            //

                            if (LfsTestBreakOnAnyError &&
                                FlagOn( Lfcb->Flags, LFCB_FLUSH_FAILED )) {

                                ASSERT( !LfsTestBreakOnAnyError ||
                                        !FlagOn( Lfcb->Flags, LFCB_FLUSH_FAILED ));

                                ClearFlag( Lfcb->Flags, LFCB_FLUSH_FAILED );
                            }
#endif
                        }
                        BytesRemaining -= (LONG)Lfcb->LogPageSize;
                        FileOffset = FileOffset + Lfcb->LogPageSize;
                    }
                }
            }

            //
            //  Reacquire the Lfcb at the original state to modify fields within it 
            //
            
            if (OwnedExclusive) {
                LfsAcquireLfcbExclusive( Lfcb );
            } else {
                LfsAcquireLfcbShared( Lfcb );
            }

            //
            //  Update the last flushed Lsn value if its valid
            //

            if (ValidLastLsn) {

                //
                //  Acquire synchronization to change the field
                //  

                ExAcquireFastMutexUnsafe( &Lfcb->Sync->Mutex );
                if (LfsRestart) {
                    Lfcb->LastFlushedRestartLsn = LastLsn;
                } else {
                    Lfcb->LastFlushedLsn = LastLsn;
                }

                //
                //   And also wake any waiters who have been satisfied
                //  

                LfsWaiter = (PLFS_WAITER)Lfcb->WaiterList.Flink;

                while ((PVOID)LfsWaiter != &Lfcb->WaiterList) {

                    if (LastLsn.QuadPart > LfsWaiter->Lsn.QuadPart ) {

                        RemoveEntryList( &LfsWaiter->Waiters );
                        KeSetEvent( &LfsWaiter->Event, 0, FALSE );

                        LfsWaiter = (PLFS_WAITER)Lfcb->WaiterList.Flink;
                    } else {
                        break;
                    }
                }
                ExReleaseFastMutexUnsafe( &Lfcb->Sync->Mutex );
            }

            if (LfsRestart) {

                //
                //  Clear any neccessary flags on a successful operation.
                //

                if (NT_SUCCESS( FailedFlushStatus )) {

                    ClearFlag( Lfcb->Flags, NewLfcbFlags );
                    NewLfcbFlags = 0;
                }

                //
                //  If this is the first write of a restart area and we have
                //  updated the LogOpenCount then update the field in the Lfcb.
                //

                if (NT_SUCCESS( Iosb.Status ) &&
                    (Lfcb->CurrentOpenLogCount != ((PLFS_RESTART_AREA) FirstLbcb->PageHeader)->RestartOpenLogCount)) {

                    Lfcb->CurrentOpenLogCount = ((PLFS_RESTART_AREA) FirstLbcb->PageHeader)->RestartOpenLogCount;
                }
            }

            //
            //  Walk through all the Lbcb's we flushed, deallocating the Lbcbs.
            //

            if (!UseTailCopy) {

                PLBCB TempLbcb;

                for (ThisLbcb = FirstLbcb; IoBlocks > 0; IoBlocks -= 1) {

                    //
                    //  Remember the next entry on the list.
                    //

                    TempLbcb = CONTAINING_RECORD( ThisLbcb->WorkqueLinks.Flink,
                                                  LBCB,
                                                  WorkqueLinks );

                    //
                    //  Remove it from the LbcbWorkque queue.
                    //

                    RemoveEntryList( &ThisLbcb->WorkqueLinks );

                    //
                    //  Deallocate the structure.
                    //

                    LfsDeallocateLbcb( Lfcb, ThisLbcb );
                    ThisLbcb = TempLbcb;
                }
            }

            //
            //  Remember the starting Lbcb for the next I/O.
            //

            FirstLbcb = NextLbcb;
        }

    } finally {

        PLFCB_SYNC Sync = Lfcb->Sync;

        DebugUnwind( LfsFlushLfcb );

        //
        //  I expect that we must at least own the lfcb shared at this point to
        //  modify its fields
        // 

        ASSERT( ExIsResourceAcquiredSharedLite( &Sync->Resource ) );

        //
        //  Show that there is no Io in progress. Preset the event but with wait == true sinc
        //  ownership of the event is indicated but the LfsIoState.
        //  This leaves us with the dispatcher db locked so the whole operation is atomic 
        //  until we call delay execution thread
        //

        ExAcquireFastMutexUnsafe(  &Lfcb->Sync->Mutex );
        
        Lfcb->LfsIoThread = 0;
        Sync->LfsIoState = LfsNoIoInProgress;

        //
        //  Wake up any waiters that have been satified + 1 additional if there is one
        //  who can continue flushing
        //  

        LfsWaiter = (PLFS_WAITER)Lfcb->WaiterList.Flink;

        while ((PVOID)LfsWaiter != &Lfcb->WaiterList ) {

            LastLsn.QuadPart = max( Lfcb->LastFlushedLsn.QuadPart, Lfcb->LastFlushedRestartLsn.QuadPart );
            
            if (LastLsn.QuadPart >= LfsWaiter->Lsn.QuadPart) {

                RemoveEntryList( &LfsWaiter->Waiters );
                KeSetEvent( &LfsWaiter->Event, 0, FALSE );
            
            } else {
                
                RemoveEntryList( &LfsWaiter->Waiters );
                KeSetEvent( &LfsWaiter->Event, 0, FALSE );
                break;
            }

            LfsWaiter = (PLFS_WAITER)Lfcb->WaiterList.Flink;
        }

        ExReleaseFastMutexUnsafe(  &Lfcb->Sync->Mutex );

        //
        //  Make sure we didn't leave any pages pinned.
        //

        if (PageBcb != NULL) {

            CcUnpinData( PageBcb );
        }

        DebugTrace( -1, Dbg, "LfsFlushLfcb:  Exit\n", 0 );
    }

    //
    //  If the Io failed at some point, we log the error in the eventlog if possible
    //  and note it in the lfs restart area
    //

    if (!NT_SUCCESS( FailedFlushStatus )) {

        PIO_ERROR_LOG_PACKET ErrorLogEntry;

        //
        //  Note failure in restart area - acquire synchronization to access lastflushedlsn
        //

        ExAcquireFastMutexUnsafe(  &Lfcb->Sync->Mutex );

        Lfcb->RestartArea->LastFailedFlushOffset = FailedFlushOffset;
        Lfcb->RestartArea->LastFailedFlushStatus = FailedFlushStatus;
        Lfcb->RestartArea->LastFailedFlushLsn = Lfcb->LastFlushedLsn;

        ExReleaseFastMutexUnsafe(  &Lfcb->Sync->Mutex );

        if (Lfcb->ErrorLogPacket != NULL) {
            ErrorLogEntry = Lfcb->ErrorLogPacket;
            Lfcb->ErrorLogPacket = NULL;
        } else {
            ErrorLogEntry = IoAllocateErrorLogEntry( Lfcb->FileObject->DeviceObject, ERROR_LOG_MAXIMUM_SIZE );
        }

        if (ErrorLogEntry != NULL) {
            ErrorLogEntry->EventCategory = ELF_CATEGORY_DISK;
            ErrorLogEntry->ErrorCode = IO_WARNING_LOG_FLUSH_FAILED;
            ErrorLogEntry->FinalStatus = FailedFlushStatus;

            IoWriteErrorLogEntry( ErrorLogEntry );
        }
    }

    //
    //  Try to preallocate another log packet if we don't have one already
    //

    if (Lfcb->ErrorLogPacket == NULL) {
        Lfcb->ErrorLogPacket = IoAllocateErrorLogEntry( Lfcb->FileObject->DeviceObject, ERROR_LOG_MAXIMUM_SIZE );
    }

    return;
}


BOOLEAN
LfsReadRestart (
    IN PLFCB Lfcb,
    IN LONGLONG FileSize,
    IN BOOLEAN FirstRestart,
    OUT PLONGLONG RestartPageOffset,
    OUT PLFS_RESTART_PAGE_HEADER *RestartPage,
    OUT PBCB *RestartPageBcb,
    OUT PBOOLEAN ChkdskWasRun,
    OUT PBOOLEAN ValidPage,
    OUT PBOOLEAN UninitializedFile,
    OUT PBOOLEAN LogPacked,
    OUT PLSN LastLsn
    )

/*++

Routine Description:

    This routine will walk through 512 blocks of the file looking for a
    valid restart page header.  It will stop the first time we find
    a valid page header.

Arguments:

    Lfcb - This is the Lfcb for the log file.

    FileSize - Size in bytes for the log file.

    FirstRestart - Indicates if we are looking for the first valid
        restart area.

    RestartPageOffset - This is the location to store the offset in the
        file where the log page was found.

    RestartPage - This is the location to store the address of the
        pinned restart page.

    RestartPageBcb - This is the location to store the Bcb for this
        cache pin operation.

    ChkdskWasRun - Address to store whether checkdisk was run on this volume.

    ValidPage - Address to store whether there was valid data on this page.

    UninitializedFile - Address to store whether this is an uninitialized
        log file.  Return value only valid if for the first restart area.

    LogPacked - Address to store whether the log file is packed.

    LastLsn - Address to store the last Lsn for this restart page.  It will be the
        chkdsk value if checkdisk was run.  Otherwise it is the LastFlushedLsn
        for this restart page.

Return Value:

    BOOLEAN - TRUE if a restart area was found, FALSE otherwise.

--*/

{
    ULONG FileOffsetIncrement;
    LONGLONG FileOffset;

    PLFS_RESTART_AREA RestartArea;

    NTSTATUS Status;

    PLFS_RESTART_PAGE_HEADER ThisPage;
    PBCB ThisPageBcb = NULL;

    BOOLEAN FoundRestart = FALSE;

    PAGED_CODE();

    DebugTrace( +1, Dbg, "LfsReadRestart:  Entered\n", 0 );
    DebugTrace(  0, Dbg, "Lfcb   -> %08lx\n", Lfcb );

    //
    //  Use a try-finally to facilitate cleanup.
    //

    *UninitializedFile = TRUE;
    *ValidPage = FALSE;
    *ChkdskWasRun = FALSE;
    *LogPacked = FALSE;

    try {

        //
        //  Determine which restart area we are looking for.
        //

        if (FirstRestart) {

            FileOffset = 0;
            FileOffsetIncrement = SEQUENCE_NUMBER_STRIDE;

        } else {

            FileOffset = SEQUENCE_NUMBER_STRIDE;
            FileOffsetIncrement = 0;
        }

        //
        //  We loop up to 16 pages  until we succeed, pin a log record page
        //  or exhaust the number of possible tries.
        //

        while ( FileOffset < min( FileSize, 16 * PAGE_SIZE )) {

            ULONG Signature;
            BOOLEAN UsaError;

            if (ThisPageBcb != NULL) {

                CcUnpinData( ThisPageBcb );
                ThisPageBcb = NULL;
            }

            //
            //  Attempt to pin a page header at the current offset.
            //

            Status = LfsPinOrMapData( Lfcb,
                                      FileOffset,
                                      SEQUENCE_NUMBER_STRIDE,
                                      TRUE,
                                      TRUE,
                                      TRUE,
                                      &UsaError,
                                      (PVOID *)&ThisPage,
                                      &ThisPageBcb );

            //
            //
            //  If we succeeded, we look at the 4 byte signature.
            //

            if (NT_SUCCESS( Status )) {

                Signature = *((PULONG) &ThisPage->MultiSectorHeader.Signature);

                //
                //  If the signature is a log record page, we will exit.
                //

                if (Signature == LFS_SIGNATURE_RECORD_PAGE_ULONG) {

                    *UninitializedFile = FALSE;
                    break;
                }

                //
                //  Continue analyzing the page if the signature is chkdsk or
                //  a restart page.
                //

                if (Signature == LFS_SIGNATURE_MODIFIED_ULONG || 
                    Signature == LFS_SIGNATURE_RESTART_PAGE_ULONG) {

                    *UninitializedFile = FALSE;

                    //
                    //  Remember where we found this page.
                    //

                    *RestartPageOffset = FileOffset;

                    //
                    //  Let's check the restart area if this is a valid page.
                    //

                    if (LfsIsRestartPageHeaderValid( FileOffset,
                                                     ThisPage,
                                                     LogPacked )

                        && LfsIsRestartAreaValid( ThisPage, *LogPacked )) {

                        //
                        //  We have a valid restart page header and restart area.
                        //  If chkdsk was run or we have no clients then
                        //  we have no more checking to do.
                        //

                        RestartArea = Add2Ptr( ThisPage,
                                               ThisPage->RestartOffset,
                                               PLFS_RESTART_AREA );

                        if (Signature == LFS_SIGNATURE_RESTART_PAGE_ULONG
                            && RestartArea->ClientInUseList != LFS_NO_CLIENT) {

                            //
                            //  Pin the entire restart area if we didn't have an earlier
                            //

                            CcUnpinData( ThisPageBcb );
                            ThisPageBcb = NULL;

                            Status = LfsPinOrMapData( Lfcb,
                                                      FileOffset,
                                                      ThisPage->SystemPageSize,
                                                      TRUE,
                                                      TRUE,
                                                      TRUE,
                                                      &UsaError,
                                                      (PVOID *)&ThisPage,
                                                      &ThisPageBcb );

                            if (NT_SUCCESS( Status )
                                && LfsIsClientAreaValid( ThisPage, *LogPacked, UsaError )) {

                                *ValidPage = TRUE;

                                RestartArea = Add2Ptr( ThisPage,
                                                       ThisPage->RestartOffset,
                                                       PLFS_RESTART_AREA );
                            }

                        } else {

                            *ValidPage = TRUE;
                        }
                    }

                    //
                    //  If chkdsk was run then update the caller's values and return.
                    //

                    if (Signature == LFS_SIGNATURE_MODIFIED_ULONG) {

                        *ChkdskWasRun = TRUE;

                        *LastLsn = ThisPage->ChkDskLsn;

                        FoundRestart = TRUE;

                        *RestartPageBcb = ThisPageBcb;
                        *RestartPage = ThisPage;

                        ThisPageBcb = NULL;
                        break;
                    }

                    //
                    //  If we have a valid page then copy the values we need from it.
                    //

                    if (*ValidPage) {

                        *LastLsn = RestartArea->CurrentLsn;

                        FoundRestart = TRUE;

                        *RestartPageBcb = ThisPageBcb;
                        *RestartPage = ThisPage;

                        ThisPageBcb = NULL;
                        break;
                    }

                    //
                    //  Remember if the signature does not indicate uninitialized file.
                    //

                } else if (Signature != LFS_SIGNATURE_UNINITIALIZED_ULONG) {

                    *UninitializedFile = FALSE;
                }
            }

            //
            //  Move to the next possible log page.
            //

            FileOffset = FileOffset << 1;

            (ULONG)FileOffset += FileOffsetIncrement;

            FileOffsetIncrement = 0;
        }

    } finally {

        DebugUnwind( LfsReadRestart );

        //
        //  Unpin the log pages if pinned.
        //

        if (ThisPageBcb != NULL) {

            CcUnpinData( ThisPageBcb );
        }

        DebugTrace(  0, Dbg, "RestartPageAddress (Low)  -> %08lx\n", RestartPageAddress->LowPart );
        DebugTrace(  0, Dbg, "RestartPageAddress (High) -> %08lx\n", RestartPageAddress->HighPart );
        DebugTrace(  0, Dbg, "FirstRestartPage          -> %08lx\n", *FirstRestartPage );
        DebugTrace( -1, Dbg, "LfsReadRestart:  Exit\n", 0 );
    }

    return FoundRestart;
}


//
//  Local support routine
//

BOOLEAN
LfsIsRestartPageHeaderValid (
    IN LONGLONG FileOffset,
    IN PLFS_RESTART_PAGE_HEADER PageHeader,
    OUT PBOOLEAN LogPacked
    )

/*++

Routine Description:

    This routine is called to verify that the candidate for a restart page
    has no corrupt values in the page header.  It verifies that the restart and
    system page size have only one bit set and are at least the value of
    the update sequence array stride.

Arguments:

    FileOffset - This is the offset in the file of the restart area to examine.
        If this offset is not 0, then it should match the system page size.

    PageHeader - This is the page to examine.

    LogPacked - Address to store whether the log file is packed.

Return Value:

    BOOLEAN - TRUE if there is no corruption in the pool header values.
              FALSE otherwise.

--*/

{
    ULONG SystemPage;
    ULONG LogPageSize;
    ULONG Mask;
    ULONG BitCount;

    USHORT EndOfUsa;

    PAGED_CODE();

    *LogPacked = FALSE;

    //
    //  Copy the values from the page header into the local variables.
    //

    SystemPage = PageHeader->SystemPageSize;
    LogPageSize = PageHeader->LogPageSize;

    //
    //  The system page and log page sizes must be greater or equal to the
    //  update sequence stride.
    //

    if (SystemPage < SEQUENCE_NUMBER_STRIDE
        || LogPageSize < SEQUENCE_NUMBER_STRIDE) {

        return FALSE;
    }

    //
    //  Now we check that the Log page and system page are multiples of two.
    //  They should only have a single bit set.
    //

    for (Mask = 1, BitCount = 0; Mask != 0; Mask = Mask << 1) {

        if (Mask & LogPageSize) {

            BitCount += 1;
        }
    }

    //
    //  If the bit count isn't 1, return false.
    //

    if (BitCount != 1) {

        return FALSE;
    }

    //
    //  Now do the system page size.
    //

    for (Mask = 1, BitCount = 0; Mask != 0; Mask = Mask << 1) {

        if (Mask & SystemPage) {

            BitCount += 1;
        }
    }

    //
    //  If the bit count isn't 1, return false.
    //

    if (BitCount != 1) {

        return FALSE;
    }

    //
    //  Check that if the file offset isn't 0, it is the system page size.
    //

    if (( FileOffset != 0 )
        && ((ULONG)FileOffset != SystemPage)) {

        return FALSE;
    }

    //
    //  We only support major version numbers 0.x and 1.x
    //
    //  Version number beyond 1.0 mean the log file is packed.
    //

    if (PageHeader->MajorVersion != 0
        && PageHeader->MajorVersion != 1) {

        return FALSE;
    }

    //
    //  Check that the restart area offset is within the system page and that
    //  the restart length field will fit within the system page size.
    //

    if (QuadAlign( PageHeader->RestartOffset ) != PageHeader->RestartOffset
        || PageHeader->RestartOffset > (USHORT) PageHeader->SystemPageSize) {

        return FALSE;
    }

    //
    //  Check that the restart offset will lie beyond the Usa Array for this page.
    //

    EndOfUsa = (USHORT) (UpdateSequenceArraySize( PageHeader->SystemPageSize )
                         * sizeof( UPDATE_SEQUENCE_NUMBER ));

    EndOfUsa += PageHeader->MultiSectorHeader.UpdateSequenceArrayOffset;

    if (PageHeader->RestartOffset < EndOfUsa) {

        return FALSE;
    }

    //
    //  Check if the log pages are packed.
    //

    if (PageHeader->MajorVersion == 1
        && PageHeader->MinorVersion > 0) {

        *LogPacked = TRUE;
    }

    //
    //  Otherwise the page header is valid.
    //

    return TRUE;
}


//
//  Local support routine
//

BOOLEAN
LfsIsRestartAreaValid (
    IN PLFS_RESTART_PAGE_HEADER PageHeader,
    IN BOOLEAN LogPacked
    )

/*++

Routine Description:

    This routine is called to verify that the restart area attached to the
    log page header is valid.  The restart values must be contained within
    the first Usa stride of the file.  This is so we can restart successfully
    after chkdsk.

Arguments:

    PageHeader - This is the page to examine.

    LogPacked - Indicates if the log file is packed.

Return Value:

    BOOLEAN - TRUE if there is no corruption in the restart area values.
              FALSE otherwise.

--*/

{
    PLFS_RESTART_AREA RestartArea;
    ULONG OffsetInRestart;
    ULONG SeqNumberBits;

    LONGLONG FileSize;

    PAGED_CODE();

    //
    //  The basic part of the restart area must fit into the first stride of
    //  the page.  This will allow chkdsk to work even if there are Usa errors.
    //

    OffsetInRestart = FIELD_OFFSET( LFS_RESTART_AREA, FileSize );

    if ((PageHeader->RestartOffset + OffsetInRestart) > FIRST_STRIDE) {

        return FALSE;
    }

    RestartArea = Add2Ptr( PageHeader, PageHeader->RestartOffset, PLFS_RESTART_AREA );

    //
    //  Everything in the restart area except the actual client array must also
    //  be in the first stride.  If the structure is packed, then we can use
    //  a field in the restart area for the client offset.
    //

    if (LogPacked) {

        OffsetInRestart = RestartArea->ClientArrayOffset;

    } else {

        //
        //  We shouldn't see any of the older disks now.
        //

        OffsetInRestart = FIELD_OFFSET( LFS_OLD_RESTART_AREA, LogClientArray );
    }

    if (QuadAlign( OffsetInRestart ) != OffsetInRestart
        || (PageHeader->RestartOffset + OffsetInRestart) > FIRST_STRIDE) {

        return FALSE;
    }

    //
    //  The full size of the restart area must fit in the system page specified by
    //  the page header.  We compute the size of the restart area by calculating
    //  the space needed by all clients.  We also check the given size of the
    //  restart area.
    //

    OffsetInRestart += (RestartArea->LogClients * sizeof( LFS_CLIENT_RECORD ));

    if (OffsetInRestart > PageHeader->SystemPageSize ) {

        return FALSE;
    }

    //
    //  If the log is packed, then check the restart length field and whether
    //  the entire restart area is contained in that length.
    //

    if (LogPacked
        && ((ULONG) (PageHeader->RestartOffset + RestartArea->RestartAreaLength) > PageHeader->SystemPageSize
            || OffsetInRestart > RestartArea->RestartAreaLength)) {

        return FALSE;
    }

    //
    //  As a final check make sure that the in use list and the free list are either
    //  empty or point to a valid client.
    //

    if ((RestartArea->ClientFreeList != LFS_NO_CLIENT
         && RestartArea->ClientFreeList >= RestartArea->LogClients)

        || (RestartArea->ClientInUseList != LFS_NO_CLIENT
            && RestartArea->ClientInUseList >= RestartArea->LogClients)) {

        return FALSE;
    }

    //
    //  Make sure the sequence number bits match the log file size.
    //

    FileSize = RestartArea->FileSize;

    for (SeqNumberBits = 0;
        ( FileSize != 0 );
        SeqNumberBits += 1,
        FileSize = ((ULONGLONG)(FileSize)) >> 1 ) {
    }

    SeqNumberBits = (sizeof( LSN ) * 8) + 3 - SeqNumberBits;

    if (SeqNumberBits != RestartArea->SeqNumberBits) {

        return FALSE;
    }

    //
    //  We will check the fields that apply only to a packed log file.
    //

    if (LogPacked) {

        //
        //  The log page data offset and record header length must be
        //  quad-aligned.
        //

        if ((QuadAlign( RestartArea->LogPageDataOffset ) != RestartArea->LogPageDataOffset ) ||
            (QuadAlign( RestartArea->RecordHeaderLength ) != RestartArea->RecordHeaderLength )) {

            return FALSE;
        }
    }

    return TRUE;
}


//
//  Local support routine
//

BOOLEAN
LfsIsClientAreaValid (
    IN PLFS_RESTART_PAGE_HEADER PageHeader,
    IN BOOLEAN LogPacked,
    IN BOOLEAN UsaError
    )

/*++

Routine Description:

    This routine is called to verify that the client array is valid.  We test
    if the client lists are correctly chained.  If the entire restart area is
    within the first Usa stride, we will ignore any Usa errors.

Arguments:

    PageHeader - This is the page to examine.

    LogPacked - Indicates if the log file is packed.

    UsaError - There was a Usa error in reading the full page.

Return Value:

    BOOLEAN - TRUE if there is no corruption in client array values.
              FALSE otherwise.

--*/

{
    PLFS_RESTART_AREA RestartArea;
    USHORT ThisClientIndex;
    USHORT ClientCount;

    PLFS_CLIENT_RECORD ClientArray;
    PLFS_CLIENT_RECORD ThisClient;

    ULONG LoopCount;

    PAGED_CODE();

    RestartArea = Add2Ptr( PageHeader, PageHeader->RestartOffset, PLFS_RESTART_AREA );

    //
    //  If there was a Usa error and the restart area isn't contained in the
    //  first Usa stride, then we have an error.
    //

    if (UsaError
        && (RestartArea->RestartAreaLength + PageHeader->RestartOffset) > FIRST_STRIDE) {

        return FALSE;
    }

    //
    //  Find the start of the client array.
    //

    if (LogPacked) {

        ClientArray = Add2Ptr( RestartArea,
                               RestartArea->ClientArrayOffset,
                               PLFS_CLIENT_RECORD );

    } else {

        //
        //  Handle the case where the offset of the client array is fixed.
        //

        ClientArray = Add2Ptr( RestartArea,
                               FIELD_OFFSET( LFS_OLD_RESTART_AREA,
                                             LogClientArray ),
                               PLFS_CLIENT_RECORD );
    }

    //
    //  Start with the free list.  Check that all the clients are valid and
    //  that there isn't a cycle.  Do the in-use list on the second pass.
    //

    ThisClientIndex = RestartArea->ClientFreeList;

    LoopCount = 2;

    do {

        BOOLEAN FirstClient;

        FirstClient = TRUE;

        ClientCount = RestartArea->LogClients;

        while (ThisClientIndex != LFS_NO_CLIENT) {

            //
            //  If the client count is zero then we must have hit a loop.
            //  If the client index is greater or equal to the log client
            //  count then the list is corrupt.
            //

            if (ClientCount == 0
                || ThisClientIndex >= RestartArea->LogClients) {

                return FALSE;
            }

            ClientCount -= 1;

            ThisClient = ClientArray + ThisClientIndex;
            ThisClientIndex = ThisClient->NextClient;

            //
            //  If this is the first client, then the previous value
            //  should indicate no client.
            //

            if (FirstClient) {

                FirstClient = FALSE;

                if (ThisClient->PrevClient != LFS_NO_CLIENT) {

                    return FALSE;
                }
            }
        }

        ThisClientIndex = RestartArea->ClientInUseList;

    } while (--LoopCount);

    //
    //  The client list is valid.
    //

    return TRUE;
}


//
//  Local support routine.
//

VOID
LfsFindFirstIo (
    IN PLFCB Lfcb,
    IN LSN TargetLsn,
    IN BOOLEAN RestartLsn,
    IN PLBCB FirstLbcb,
    OUT PLBCB *NextLbcb,
    OUT PLONGLONG FileOffset,
    OUT PBOOLEAN ContainsLastEntry,
    OUT PBOOLEAN LfsRestart,
    OUT PBOOLEAN UseTailCopy,
    OUT PULONG IoBlocks
    )

/*++

Routine Description:

    This routine walks through the linked Lbcb's for a Lfcb and groups
    as many of them as can be grouped into a single I/O transfer.
    It updates pointers to indicate the file offset and length of the
    transfer, whether the I/O includes a particular Lbcb, whether the
    transfer is a restart area or a log record page and the number of
    Lbcb's included in the transfer.  We only flush a single log page
    if we are passing through the file for the first time.

Arguments:

    Lfcb - This is the file control block for the log file.

    TargetLsn - This is the Lsn that the caller wants to have included in
        the transfer.
        
    RestartLsn - whether the target lsn is a restart lsn        

    FirstLbcb - This is the first Lbcb to look at in the list.

    NextLbcb - This is the Lbcb to look at first on the next call to this
        routine.

    FileOffset - Supplies the address where we store the offset in the
        log file of this transfer.

    ContainsLastEntry - Supplies the address where we store whether this
        I/O includes the 'LastEntry' Lbcb.

    LfsRestart - Supplies the address where we store whether this transfer
        is a Lfs restart area.

    UseTailCopy - Supplies the address where we store whether we should
        use of page for a copy of the end of the log file.

    IoBlocks - Supplies the address where we store the number of Lbcb's
        for this transfer.

Return Value:

    None.

--*/

{

    ULONG MaxFlushCount = LFS_MAX_FLUSH_COUNT;

    PAGED_CODE();

    DebugTrace( +1, Dbg, "LfsFindFirstIo:  Entered\n", 0 );
    DebugTrace(  0, Dbg, "Lfcb          -> %08lx\n", Lfcb );

    //
    //  Initialize the file offset, length and io blocks values.
    //  Also assume the last entry is not contained here.
    //  Also assume we have no next Lbcb.
    //

    *FileOffset = FirstLbcb->FileOffset;
    *IoBlocks = 1;

    *LfsRestart = FALSE;
    *UseTailCopy = FALSE;

    *NextLbcb = NULL;

    //
    //  Check if we have found the desired Lsn in a non restart page.  We reject the match
    //  if the Lbcb indicates that we should flush the copy first. Also if this is a restart
    //  page the target the target should be one and vice versa
    //

    if ((FirstLbcb->LastEndLsn.QuadPart >= TargetLsn.QuadPart) &&
        !FlagOn( FirstLbcb->LbcbFlags, LBCB_FLUSH_COPY ) &&

        ((!RestartLsn && !LfsLbcbIsRestart( FirstLbcb ) &&
         (FlagOn( FirstLbcb->Flags, LOG_PAGE_LOG_RECORD_END ))) ||

         (RestartLsn && LfsLbcbIsRestart( FirstLbcb )))) {

        *ContainsLastEntry = TRUE;

    } else {

        *ContainsLastEntry = FALSE;
    }

    //
    //  Check if this is a restart block or if we are passing through the log
    //  file for the first time or if this Lbcb is still in the active queue.
    //  If not, then group as many of the Lbcb's as can be part of a single Io.
    //

    if (LfsLbcbIsRestart( FirstLbcb )) {

        *LfsRestart = TRUE;

#ifdef BENL_DBG
        //
        //  if one is ever on the active queue will use the code at the bottom to remove it
        //

        ASSERT( !FlagOn( FirstLbcb->LbcbFlags, LBCB_ON_ACTIVE_QUEUE ) );
#endif

    } else if (FlagOn( FirstLbcb->LbcbFlags, LBCB_FLUSH_COPY)) {
        
        //
        //  Only packed logs - reuse the tail which cause us to need to flush a copy
        //  

        ASSERT( FlagOn( Lfcb->Flags, LFCB_PACK_LOG ) );

        //
        //  This is going to be a tail copy and we will restart with same lbcb
        //  and remove the flag so its normal next time
        //  

        *UseTailCopy = TRUE;
        *NextLbcb = FirstLbcb;
        ClearFlag( FirstLbcb->LbcbFlags, LBCB_FLUSH_COPY );

    } else if (FlagOn( FirstLbcb->LbcbFlags, LBCB_ON_ACTIVE_QUEUE ) &&
               FlagOn( Lfcb->Flags, LFCB_PACK_LOG )) {

        //
        //   This is a normal tail copy since its still active
        //  

        *UseTailCopy = TRUE;


    } else if (FlagOn( Lfcb->Flags, LFCB_MULTIPLE_PAGE_IO )) {

        PLBCB EndOfPageLbcb = NULL;
        ULONG EndOfPageIoBlocks;

        //
        //  If we are not supporting a packed log file and this Lbcb is from
        //  the active queue, we need to check that losing the tail of the page
        //  will not swallow up any of our reserved space. 
        //
        
        if (!FlagOn( Lfcb->Flags, LFCB_PACK_LOG )) {
        
            LONGLONG CurrentAvail;
            LONGLONG UnusedBytes;

            //
            //  ISSUE: old code only removed the selected element from the active queue
            //  we'll remove any we hit before the selected lsn
            //  
        
            //
            //  Find the unused bytes.
            //
        
            UnusedBytes = 0;
        
            LfsCurrentAvailSpace( Lfcb,
                                  &CurrentAvail,
                                  (PULONG)&UnusedBytes );
        
            CurrentAvail = CurrentAvail - Lfcb->TotalUndoCommitment;
        
            if (UnusedBytes > CurrentAvail) {
        
                DebugTrace( -1, Dbg, "Have to preserve these bytes for possible aborts\n", 0 );
        
                ExRaiseStatus( STATUS_LOG_FILE_FULL );
            }
        
            //
            //  We want to make sure we don't write any more data into this
            //  page.  Remove this from the active queue.
            //
        
            RemoveEntryList( &FirstLbcb->ActiveLinks );
            ClearFlag( FirstLbcb->LbcbFlags, LBCB_ON_ACTIVE_QUEUE );
        } 

        //
        //  We loop until there are no more blocks or they aren't
        //  contiguous in the file or we have found an entry on the
        //  active queue or we found an entry where we want to explicitly
        //  flush a copy first.
        //

        while ((FirstLbcb->WorkqueLinks.Flink != &Lfcb->LbcbWorkque) &&
               !FlagOn( FirstLbcb->LbcbFlags, LBCB_ON_ACTIVE_QUEUE ) &&
               *IoBlocks < MaxFlushCount) {


            LONGLONG ExpectedFileOffset;
            PLBCB TempLbcb;

            //
            //  Get the next Lbcb.
            //

            TempLbcb = CONTAINING_RECORD( FirstLbcb->WorkqueLinks.Flink,
                                          LBCB,
                                          WorkqueLinks );

            //
            //  Break out of the loop if the file offset is not the
            //  expected value or the next entry is on the active queue.
            //

            ExpectedFileOffset = FirstLbcb->FileOffset + Lfcb->LogPageSize;

            //
            //  We want to stop at this point if the next Lbcb is not
            //  the expected offset or we are packing the log file and
            //  the next Lbcb is on the active queue or we want to write
            //  a copy of the data before this page goes out.
            //

            if ((TempLbcb->FileOffset != ExpectedFileOffset) ||
                (FlagOn( Lfcb->Flags, LFCB_PACK_LOG ) &&
                 FlagOn( TempLbcb->LbcbFlags, LBCB_FLUSH_COPY | LBCB_ON_ACTIVE_QUEUE))) {

                //
                //  Use the Lbcb at the end of a page if possible.
                //

                if (EndOfPageLbcb != NULL) {

                    FirstLbcb = EndOfPageLbcb;
                    *IoBlocks = EndOfPageIoBlocks;
                }

                break;
            }

            //
            //  We can add this to our I/o.  Increment the Io blocks
            //  and length of the transfer.  Also check if this entry
            //  contains the Last Entry specified by the caller.
            //

            *IoBlocks += 1;

            if (FlagOn( TempLbcb->Flags, LOG_PAGE_LOG_RECORD_END ) &&
                (TempLbcb->LastEndLsn.QuadPart >= TargetLsn.QuadPart) &&
                !RestartLsn) {

                *ContainsLastEntry = TRUE;
            }

            //
            //  Check if this Lbcb is at the end of a system page.
            //

            if (*ContainsLastEntry &&
                (PAGE_SIZE != (ULONG) Lfcb->LogPageSize) &&
                !FlagOn( ((ULONG) TempLbcb->FileOffset + (ULONG) Lfcb->LogPageSize),
                         PAGE_SIZE - 1 )) {

                EndOfPageLbcb = TempLbcb;
                EndOfPageIoBlocks = *IoBlocks;
            }

            //
            //  Use this entry as the current entry.
            //

            FirstLbcb = TempLbcb;
        }
    }

    //
    //  If the current Lbcb is on the active queue and we aren't using
    //  a tail copy, then remove this from the active queue.  If this
    //  not our target and removing this will cause us to swallow up
    //  part of our reserved quota then back up one Lbcb.
    //

    if (!(*UseTailCopy) && FlagOn( FirstLbcb->LbcbFlags, LBCB_ON_ACTIVE_QUEUE )) {

        if (Lfcb->CurrentAvailable < Lfcb->TotalUndoCommitment) {

            //
            //  Move back one file record.
            //

            *IoBlocks -= 1;
            *NextLbcb = FirstLbcb;

            //
            //  Otherwise remove it from the active queue.
            //

        } else {

            ClearFlag( FirstLbcb->LbcbFlags, LBCB_ON_ACTIVE_QUEUE );
            RemoveEntryList( &FirstLbcb->ActiveLinks );
        }
    }

    //
    //  If we haven't found the Lbcb to restart from we will just use the
    //  next Lbcb after the last one found.
    //

    if ((*NextLbcb == NULL) && (FirstLbcb->WorkqueLinks.Flink != &Lfcb->LbcbWorkque)) {

        *NextLbcb = CONTAINING_RECORD( FirstLbcb->WorkqueLinks.Flink,
                                       LBCB,
                                       WorkqueLinks );
    }

    ASSERT( *ContainsLastEntry || (*NextLbcb != NULL) );

    DebugTrace(  0, Dbg, "File Offset (Low)     -> %08lx\n", FileOffset->LowPart );
    DebugTrace(  0, Dbg, "File Offset (High)    -> %08lx\n", FileOffset->HighPart );
    DebugTrace(  0, Dbg, "Contains Last Entry   -> %08x\n", *ContainsLastEntry );
    DebugTrace(  0, Dbg, "LfsRestart            -> %08x\n", *LfsRestart );
    DebugTrace(  0, Dbg, "IoBlocks              -> %08lx\n", *IoBlocks );
    DebugTrace( -1, Dbg, "LfsFindFirstIo:  Exit\n", 0 );

    return;
}

