/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    rebase.c

Abstract:

    Source file for the REBASE utility that takes a group of image files and
    rebases them so they are packed as closely together in the virtual address
    space as possible.

Author:

    Mark Lucovsky (markl) 30-Apr-1993

Revision History:

--*/

#include <private.h>

//
// byte swapping macros (LE/BE) used for IA64 relocations
// source != destination
//

#define SWAP_SHORT(_dst,_src)                                                  \
   ((((unsigned char *)_dst)[1] = ((unsigned char *)_src)[0]),                 \
    (((unsigned char *)_dst)[0] = ((unsigned char *)_src)[1]))

#define SWAP_INT(_dst,_src)                                                    \
   ((((unsigned char *)_dst)[3] = ((unsigned char *)_src)[0]),                 \
    (((unsigned char *)_dst)[2] = ((unsigned char *)_src)[1]),                 \
    (((unsigned char *)_dst)[1] = ((unsigned char *)_src)[2]),                 \
    (((unsigned char *)_dst)[0] = ((unsigned char *)_src)[3]))

#define SWAP_LONG_LONG(_dst,_src)                                              \
   ((((unsigned char *)_dst)[7] = ((unsigned char *)_src)[0]),                 \
    (((unsigned char *)_dst)[6] = ((unsigned char *)_src)[1]),                 \
    (((unsigned char *)_dst)[5] = ((unsigned char *)_src)[2]),                 \
    (((unsigned char *)_dst)[4] = ((unsigned char *)_src)[3]),                 \
    (((unsigned char *)_dst)[3] = ((unsigned char *)_src)[4]),                 \
    (((unsigned char *)_dst)[2] = ((unsigned char *)_src)[5]),                 \
    (((unsigned char *)_dst)[1] = ((unsigned char *)_src)[6]),                 \
    (((unsigned char *)_dst)[0] = ((unsigned char *)_src)[7]))


#define REBASE_ERR 99
#define REBASE_OK  0

static
PVOID
RvaToVa(
    ULONG Rva,
    PLOADED_IMAGE Image
    );

typedef
PIMAGE_BASE_RELOCATION
(WINAPI *LPRELOCATE_ROUTINE)(
    IN ULONG_PTR VA,
    IN ULONG SizeOfBlock,
    IN PUSHORT NextOffset,
    IN LONG_PTR Diff
    );

typedef
PIMAGE_BASE_RELOCATION
(WINAPI *LPRELOCATE_ROUTINE64)(
    IN ULONG_PTR VA,
    IN ULONG SizeOfBlock,
    IN PUSHORT NextOffset,
    IN LONGLONG Diff
    );


static LPRELOCATE_ROUTINE RelocRoutineNative;
static LPRELOCATE_ROUTINE64 RelocRoutine64;


#include <ldrreloc_rebase.c>        // P/u ldrreloc from ntos\rtl

#define x256MEG (256*(1024*1024))

#define x256MEGSHIFT 28

#define ROUND_UP( Size, Amount ) (((ULONG)(Size) + ((Amount) - 1)) & ~((Amount) - 1))

VOID
AdjImageBaseSize(
    PULONG  pImageBase,
    PULONG  ImageSize,
    BOOL    fGoingDown
    );


BOOL
RelocateImage(
    PLOADED_IMAGE LoadedImage,
    ULONG64 NewBase,
    ULONG64 *Diff,
    ULONG tstamp
    );

BOOL
ReBaseImage(
    IN     LPSTR CurrentImageName,
    IN     LPSTR SymbolPath,        // Symbol path (if
    IN     BOOL  fReBase,           // TRUE if actually rebasing, false if only summing
    IN     BOOL  fRebaseSysfileOk,  // TRUE is system images s/b rebased
    IN     BOOL  fGoingDown,        // TRUE if the image s/b rebased below the given base
    IN     ULONG CheckImageSize,    // Max size allowed  (0 if don't care)
    OUT    ULONG *OldImageSize,     // Returned from the header
    OUT    ULONG_PTR *OldImageBase, // Returned from the header
    OUT    ULONG *NewImageSize,     // Image size rounded to next separation boundary
    IN OUT ULONG_PTR *NewImageBase, // (in) Desired new address.
                                    // (out) Next new address (above/below this one)
    IN     ULONG tstamp             // new timestamp for image
    )
{
    ULONG64 xOldImageBase = *OldImageBase;
    ULONG64 xNewImageBase = *NewImageBase;
    BOOL rc;

    rc = ReBaseImage64(
        CurrentImageName,
        SymbolPath,
        fReBase,
        fRebaseSysfileOk,
        fGoingDown,
        CheckImageSize,
        OldImageSize,
        &xOldImageBase,
        NewImageSize,
        &xNewImageBase,
        tstamp);

    *OldImageBase = (ULONG_PTR)xOldImageBase;
    *NewImageBase = (ULONG_PTR)xNewImageBase;
    return rc;
}

BOOL
ReBaseImage64(
    IN     LPSTR CurrentImageName,
    IN     LPSTR SymbolPath,       // Symbol path (if
    IN     BOOL  fReBase,          // TRUE if actually rebasing, false if only summing
    IN     BOOL  fRebaseSysfileOk, // TRUE is system images s/b rebased
    IN     BOOL  fGoingDown,       // TRUE if the image s/b rebased below the given base
    IN     ULONG CheckImageSize,   // Max size allowed  (0 if don't care)
    OUT    ULONG *OldImageSize,    // Returned from the header
    OUT    ULONG64 *OldImageBase,  // Returned from the header
    OUT    ULONG *NewImageSize,    // Image size rounded to next separation boundary
    IN OUT ULONG64 *NewImageBase,  // (in) Desired new address.
                                   // (out) Next new address (above/below this one)
    IN     ULONG tstamp            // new timestamp for image
    )
{
    BOOL  fSymbolsAlreadySplit = FALSE;
    CHAR  DebugFileName[ MAX_PATH+1 ];
    CHAR  DebugFilePath[ MAX_PATH+1 ];
    ULONG CurrentImageSize;
    ULONG64 DesiredImageBase;
    ULONG OldChecksum;
    ULONG64 Diff = 0;
    ULONG UpdateSymbolsError = 0;
    LOADED_IMAGE CurrentImage = {0};

    BOOL rc = TRUE;

    if (fReBase && (*NewImageBase & 0x0000FFFF) != 0) {
        rc = FALSE;
        UpdateSymbolsError = ERROR_INVALID_ADDRESS;
        goto Exit;
    }

    // Map and load the current image

    if ( MapAndLoad( CurrentImageName, NULL, &CurrentImage, FALSE, fReBase ? FALSE : TRUE ) ) {
        PVOID pData;
        DWORD dwDataSize;
        pData = ImageDirectoryEntryToData(
                                          CurrentImage.MappedAddress,
                                          FALSE,
                                          IMAGE_DIRECTORY_ENTRY_SECURITY,
                                          &dwDataSize
                                          );

        if (pData || dwDataSize) {
            // Certificates in the image, can't rebase
            UpdateSymbolsError = ERROR_EXE_CANNOT_MODIFY_SIGNED_BINARY;
            rc = FALSE;
            goto CleanupAndExit;
        }

        pData = ImageDirectoryEntryToData(
                                          CurrentImage.MappedAddress,
                                          FALSE,
                                          IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR,
                                          &dwDataSize
                                          );

        if (pData || dwDataSize) {
            // COR header found - see if it's strong signed
            if (((IMAGE_COR20_HEADER *)pData)->Flags & COMIMAGE_FLAGS_STRONGNAMESIGNED)
            {
                UpdateSymbolsError = ERROR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY;
                rc = FALSE;
                goto CleanupAndExit;
            }
        }

        if (!(!fRebaseSysfileOk && CurrentImage.fSystemImage)) {
            fSymbolsAlreadySplit = CurrentImage.Characteristics & IMAGE_FILE_DEBUG_STRIPPED ? TRUE : FALSE;
            if ( fSymbolsAlreadySplit ) {

                // Find DebugFileName for later use.

                PIMAGE_DEBUG_DIRECTORY DebugDirectories;
                ULONG DebugDirectoriesSize;
                PIMAGE_DEBUG_MISC MiscDebug;

                strcpy( DebugFileName, CurrentImageName );

                DebugDirectories = (PIMAGE_DEBUG_DIRECTORY)ImageDirectoryEntryToData(
                                                        CurrentImage.MappedAddress,
                                                        FALSE,
                                                        IMAGE_DIRECTORY_ENTRY_DEBUG,
                                                        &DebugDirectoriesSize
                                                        );
                if (DebugDirectoryIsUseful(DebugDirectories, DebugDirectoriesSize)) {
                    while (DebugDirectoriesSize != 0) {
                        if (DebugDirectories->Type == IMAGE_DEBUG_TYPE_MISC) {
                            MiscDebug = (PIMAGE_DEBUG_MISC)
                                ((PCHAR)CurrentImage.MappedAddress +
                                 DebugDirectories->PointerToRawData
                                );
                            strcpy( DebugFileName, (PCHAR) MiscDebug->Data );
                            break;
                        }
                        else {
                            DebugDirectories += 1;
                            DebugDirectoriesSize -= sizeof( *DebugDirectories );
                        }
                    }
                }
            }

            if (CurrentImage.FileHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
                CurrentImageSize = ((PIMAGE_NT_HEADERS32)CurrentImage.FileHeader)->OptionalHeader.SizeOfImage;
                *OldImageBase = ((PIMAGE_NT_HEADERS32)CurrentImage.FileHeader)->OptionalHeader.ImageBase;
            } else {
                CurrentImageSize = ((PIMAGE_NT_HEADERS64)CurrentImage.FileHeader)->OptionalHeader.SizeOfImage;
                *OldImageBase = ((PIMAGE_NT_HEADERS64)CurrentImage.FileHeader)->OptionalHeader.ImageBase;
            }

            // Save the current settings for the caller.

            *OldImageSize = CurrentImageSize;
            *NewImageSize = ROUND_UP( CurrentImageSize, IMAGE_SEPARATION );

            if (CheckImageSize) {
                // The user asked for a max size test.

                if ( *NewImageSize > ROUND_UP(CheckImageSize, IMAGE_SEPARATION) ) {
                    *NewImageBase = 0;
                    rc = FALSE;
                    goto CleanupAndExit;
                }
            }

            DesiredImageBase = *NewImageBase;

            // So long as we're not basing to zero or rebasing to the same address,
            // go for it.

            if (fReBase) {
                BOOL fAdjust;
                if ((CurrentImage.FileHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) &&
                    (CurrentImage.FileHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_ALPHA) &&
                    (CurrentImage.FileHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_ALPHA64) &&
                    (CurrentImage.FileHeader->FileHeader.Machine != IMAGE_FILE_MACHINE_IA64))
                {
                    fAdjust = TRUE;
                } else {
                    fAdjust = FALSE;
                }

                if (fGoingDown) {
                    DesiredImageBase -= *NewImageSize;
                    if (fAdjust) {
                        AdjImageBaseSize( (PULONG)&DesiredImageBase, &CurrentImageSize, fGoingDown );
                    }
                }

                if ((DesiredImageBase) &&
                    (DesiredImageBase != *OldImageBase)
                   ) {

                    if (CurrentImage.FileHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
                        OldChecksum = ((PIMAGE_NT_HEADERS32)CurrentImage.FileHeader)->OptionalHeader.CheckSum;
                    } else {
                        OldChecksum = ((PIMAGE_NT_HEADERS64)CurrentImage.FileHeader)->OptionalHeader.CheckSum;
                    }
                    if ( !RelocateImage( &CurrentImage, DesiredImageBase, &Diff, tstamp ) ) {
                        UpdateSymbolsError = GetLastError();
                        rc = FALSE;
                        goto CleanupAndExit;
                    }

                    if ( fSymbolsAlreadySplit && Diff ) {
                        if ( UpdateDebugInfoFileEx(CurrentImageName,
                                                   SymbolPath,
                                                   DebugFilePath,
                                                   (PIMAGE_NT_HEADERS32)(CurrentImage.FileHeader),
                                                   OldChecksum )) {
                            UpdateSymbolsError = GetLastError();
                        } else {
                            UpdateSymbolsError = 0;
                        }
                    }
                } else {
                    //
                    // Should this be -1??  shouldn't it be 0 instead? - kentf
                    //
                    Diff = (ULONG) -1;
                }

                if (!fGoingDown && Diff) {
                    DesiredImageBase += *NewImageSize;
                    if (fAdjust) {
                        AdjImageBaseSize( (PULONG)&DesiredImageBase, &CurrentImageSize, fGoingDown );
                    }
                }

            }
        }

        if (fReBase) {
            if (Diff) {
                *NewImageBase = DesiredImageBase;
            } else {
                UpdateSymbolsError = ERROR_INVALID_ADDRESS;
                rc = FALSE;
                goto CleanupAndExit;
            }
        }
    } else {
        if (CurrentImage.fDOSImage == TRUE) {
            UpdateSymbolsError = ERROR_BAD_EXE_FORMAT;
        } else {
            UpdateSymbolsError = GetLastError();
        }
        rc = FALSE;
        goto Exit;
    }

CleanupAndExit:
    UnmapViewOfFile( CurrentImage.MappedAddress );
    if ( CurrentImage.hFile != INVALID_HANDLE_VALUE ) {
        CloseHandle( CurrentImage.hFile );
    }
    ZeroMemory( &CurrentImage, sizeof( CurrentImage ) );

Exit:

    SetLastError(UpdateSymbolsError);

    return(rc);
}


VOID
AdjImageBaseSize (
    PULONG pulImageBase,
    PULONG pulImageSize,
    BOOL   fGoingDown
    )
{

    DWORD Meg1, Meg2, Delta;

    //
    // ImageBase is the base for the current image. Make sure that
    // the image does not span a 256Mb boundry. This is due to an r4000
    // chip bug that has problems computing the correct address for absolute
    // jumps that occur in the last few instructions of a 256mb region
    //

    Meg1 = *pulImageBase >> x256MEGSHIFT;
    Meg2 = ( *pulImageBase + ROUND_UP( *pulImageSize, IMAGE_SEPARATION ) ) >> x256MEGSHIFT;

    if ( Meg1 != Meg2 ) {

        //
        // If we are going down, then subtract the overlap from ThisBase
        //

        if ( fGoingDown ) {

            Delta = ( *pulImageBase + ROUND_UP( *pulImageSize, IMAGE_SEPARATION ) ) -
                    ( Meg2 << x256MEGSHIFT );
            Delta += IMAGE_SEPARATION;
            *pulImageBase = *pulImageBase - Delta;
            *pulImageSize += Delta;
            }
        else {
            Delta = ( Meg2 << x256MEGSHIFT ) - *pulImageBase;
            *pulImageBase += Delta;
            *pulImageSize += Delta;
            }
        }
}

BOOL
RelocateImage(
    PLOADED_IMAGE LoadedImage,
    ULONG64 NewBase,
    ULONG64 *Diff,
    ULONG tstamp
    )
{
    ULONG_PTR VA;
    ULONG64 OldBase;
    ULONG SizeOfBlock;
    PUSHORT NextOffset;
    PIMAGE_NT_HEADERS NtHeaders;
    PIMAGE_BASE_RELOCATION NextBlock;
    ULONG CheckSum;
    ULONG HeaderSum;
    PIMAGE_FILE_HEADER FileHeader;
    BOOL rc = TRUE;
    ULONG TotalCountBytes = 0;

    static BOOL  fInit = FALSE;

    if (!fInit) {

        RelocRoutineNative = (LPRELOCATE_ROUTINE)GetProcAddress(GetModuleHandle("ntdll"), "LdrProcessRelocationBlock");

#ifdef _WIN64
        RelocRoutine64 = RelocRoutineNative;
#else
        RelocRoutine64 = xxLdrProcessRelocationBlock64;
#endif
    }

    __try {
        if (LoadedImage->FileHeader->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) {
            // Relocations stripped.  Nothing to do.
            __leave;
        }

        NtHeaders = LoadedImage->FileHeader;
        FileHeader = &NtHeaders->FileHeader;
        if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
            OldBase = ((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.ImageBase;
        } else {
            OldBase = ((PIMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.ImageBase;
        }

        //
        // Locate the relocation section.
        //

        NextBlock = (PIMAGE_BASE_RELOCATION)ImageDirectoryEntryToData(
                                                LoadedImage->MappedAddress,
                                                FALSE,
                                                IMAGE_DIRECTORY_ENTRY_BASERELOC,
                                                &TotalCountBytes
                                                );

        *Diff = NewBase - OldBase;

        //
        // If the image has a relocation table, then apply the specified fixup
        // information to the image.
        //

        while (TotalCountBytes) {
            SizeOfBlock = NextBlock->SizeOfBlock;
            TotalCountBytes -= SizeOfBlock;
            SizeOfBlock -= sizeof(IMAGE_BASE_RELOCATION);
            SizeOfBlock /= sizeof(USHORT);
            NextOffset = (PUSHORT)(NextBlock + 1);

            //
            // Compute the address and value for the fixup.
            //

            if ( SizeOfBlock ) {
                VA = (ULONG_PTR)RvaToVa(NextBlock->VirtualAddress,LoadedImage);
                if ( !VA ) {
                    NtHeaders->Signature = (ULONG)-1;
                    rc = FALSE;
                    __leave;
                    }

                if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
                    if ( !(NextBlock = (RelocRoutine64)(VA,SizeOfBlock,NextOffset,*Diff)) ) {
                        NtHeaders->Signature = (ULONG)-1;
                        rc = FALSE;
                        __leave;
                    }
                } else {
                    if ( !(NextBlock = (RelocRoutineNative)(VA,SizeOfBlock,NextOffset,(LONG_PTR)*Diff)) ) {
                        NtHeaders->Signature = (ULONG)-1;
                        rc = FALSE;
                        __leave;
                        }
                    }
                }
            else {
                NextBlock++;
                }
            }

        if (tstamp) {
            FileHeader->TimeDateStamp = tstamp;
        } else {
            FileHeader->TimeDateStamp++;
        }

        if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
            ((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.ImageBase = (ULONG)NewBase;
            if ( LoadedImage->hFile != INVALID_HANDLE_VALUE ) {

                ((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.CheckSum = 0;

                CheckSumMappedFile(
                            (PVOID)LoadedImage->MappedAddress,
                            GetFileSize(LoadedImage->hFile, NULL),
                            &HeaderSum,
                            &CheckSum
                            );
                ((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.CheckSum = CheckSum;
            }
        } else {
            ((PIMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.ImageBase = NewBase;
            if ( LoadedImage->hFile != INVALID_HANDLE_VALUE ) {
                ((PIMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.CheckSum = 0;

                CheckSumMappedFile(
                            (PVOID)LoadedImage->MappedAddress,
                            GetFileSize(LoadedImage->hFile, NULL),
                            &HeaderSum,
                            &CheckSum
                            );

                ((PIMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.CheckSum = CheckSum;
            }
        }

        FlushViewOfFile(LoadedImage->MappedAddress,0);
        TouchFileTimes(LoadedImage->hFile,NULL);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        rc = FALSE;
    }

    return rc;
}


PVOID
RvaToVa(
    ULONG Rva,
    PLOADED_IMAGE Image
    )
{

    PIMAGE_SECTION_HEADER Section;
    ULONG i;
    PVOID Va;

    Va = NULL;
    Section = Image->LastRvaSection;
    if (Rva == 0) {
        // a NULL Rva will be sent if there are relocs before the first page
        //  (ie: we're relocating a system image)

        Va = Image->MappedAddress;

    } else {
        if ( Rva >= Section->VirtualAddress &&
             Rva < (Section->VirtualAddress + Section->SizeOfRawData) ) {
            Va = (PVOID)(Rva - Section->VirtualAddress + Section->PointerToRawData + Image->MappedAddress);
        } else {
            for(Section = Image->Sections,i=0; i<Image->NumberOfSections; i++,Section++) {
                if ( Rva >= Section->VirtualAddress &&
                     Rva < (Section->VirtualAddress + Section->SizeOfRawData) ) {
                    Va = (PVOID)(Rva - Section->VirtualAddress + Section->PointerToRawData + Image->MappedAddress);
                    Image->LastRvaSection = Section;
                    break;
                }
            }
        }
    }

    return Va;
}

#ifndef STANDALONE_REBASE
// Dummy stub so the rebase.exe that shipped with VC5/VC6 will load.
VOID
RemoveRelocations(
    PCHAR ImageName
    )
{
    return;
}
#endif
