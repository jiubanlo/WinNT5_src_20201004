/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    NameP.h

Abstract:

    This module defines private part of the File System Rtl component, used by
        name.c.

Author:

    Gary Kimura     [GaryKi]    30-Jul-1990

Revision History:

        [t-HeathH]  17-Jul-1994 Moved this header file into the ftphelp porject, to
        maintain a single source base for Chicago and NT.

--*/

#ifndef _NAMEP_H_INCLUDED_
#define _NAMEP_H_INCLUDED_

#if defined(__cplusplus)
extern "C" {
#endif

//#include "ftp.h"
#include <name.h>

//
//  The global MyFsRtl debug level variable, its values are:
//
//      0x00000000      Always gets printed (used when about to bug check)
//
//      0x00000001      Error conditions
//      0x00000002      Debug hooks
//      0x00000004
//      0x00000008
//
//      0x00000010
//      0x00000020
//      0x00000040
//      0x00000080
//
//      0x00000100
//      0x00000200
//      0x00000400
//      0x00000800
//
//      0x00001000
//      0x00002000
//      0x00004000
//      0x00008000
//
//      0x00010000
//      0x00020000
//      0x00040000
//      0x00080000
//
//      0x00100000
//      0x00200000
//      0x00400000
//      0x00800000
//
//      0x01000000
//      0x02000000
//      0x04000000      NotifyChange routines
//      0x08000000      Oplock routines
//
//      0x10000000      Name routines
//      0x20000000      FileLock routines
//      0x40000000      Vmcb routines
//      0x80000000      Mcb routines
//

//
//  Debug trace support
//

#ifdef FSRTLDBG

extern LONG MyFsRtlDebugTraceLevel;
extern LONG MyFsRtlDebugTraceIndent;

#define DebugTrace(INDENT,LEVEL,X,Y) {                        \
    LONG _i;                                                  \
    if (((LEVEL) == 0) || (MyFsRtlDebugTraceLevel & (LEVEL))) { \
        _i = (ULONG)PsGetCurrentThread();                     \
        DbgPrint("%08lx:",_i);                                 \
        if ((INDENT) < 0) {                                   \
            MyFsRtlDebugTraceIndent += (INDENT);                \
        }                                                     \
        if (MyFsRtlDebugTraceIndent < 0) {                      \
            MyFsRtlDebugTraceIndent = 0;                        \
        }                                                     \
        for (_i=0; _i<MyFsRtlDebugTraceIndent; _i+=1) {         \
            DbgPrint(" ");                                     \
        }                                                     \
        DbgPrint(X,Y);                                         \
        if ((INDENT) > 0) {                                   \
            MyFsRtlDebugTraceIndent += (INDENT);                \
        }                                                     \
    }                                                         \
}

#define DebugDump(STR,LEVEL,PTR) {                            \
    ULONG _i;                                                 \
    VOID MyFsRtlDump();                                         \
    if (((LEVEL) == 0) || (MyFsRtlDebugTraceLevel & (LEVEL))) { \
        _i = (ULONG)PsGetCurrentThread();                     \
        DbgPrint("%08lx:",_i);                                 \
        DbgPrint(STR);                                         \
        if (PTR != NULL) {MyFsRtlDump(PTR);}                    \
        DbgBreakPoint();                                      \
    }                                                         \
}

#else

#define DebugTrace(INDENT,LEVEL,X,Y)     {NOTHING;}

#define DebugDump(STR,LEVEL,PTR)         {NOTHING;}

#endif // FSRTLDBG

//
//  This macro returns TRUE if a flag in a set of flags is on and FALSE
//  otherwise
//

#define FlagOn(Flags,SingleFlag)        ((Flags) & (SingleFlag))

#define BooleanFlagOn(Flags,SingleFlag) ((BOOLEAN)(((Flags) & (SingleFlag)) != 0))

//
//  This macro takes a pointer (or ulong) and returns its rounded up word
//  value
//

#define WordAlign(Ptr) (                \
    ((((ULONG)(Ptr)) + 1) & 0xfffffffe) \
    )

//
//  This macro takes a pointer (or ulong) and returns its rounded up longword
//  value
//

#define LongAlign(Ptr) (                \
    ((((ULONG)(Ptr)) + 3) & 0xfffffffc) \
    )

//
//  This macro takes a pointer (or ulong) and returns its rounded up quadword
//  value
//

#define QuadAlign(Ptr) (                \
    ((((ULONG)(Ptr)) + 7) & 0xfffffff8) \
    )

//
//  This macro takes a ulong and returns its value rounded up to a sector
//  boundary
//

#define SectorAlign(Ptr) (                \
    ((((ULONG)(Ptr)) + 511) & 0xfffffe00) \
    )

//
//  This macro takes a number of bytes and returns the number of sectors
//  required to contain that many bytes, i.e., it sector aligns and divides
//  by the size of a sector.
//

#define SectorsFromBytes(bytes) ( \
    ((bytes) + 511) / 512         \
    )

//
//  This macro takes a number of sectors and returns the number of bytes
//  contained in that many sectors.
//

#define BytesFromSectors(sectors) ( \
    (sectors) * 512                 \
    )

//
//  The following types and macros are used to help unpack the packed and
//  misaligned fields found in the Bios parameter block
//

typedef union _UCHAR1 {
    UCHAR  Uchar[1];
    UCHAR  ForceAlignment;
} UCHAR1, *PUCHAR1;

typedef union _UCHAR2 {
    UCHAR  Uchar[2];
    USHORT ForceAlignment;
} UCHAR2, *PUCHAR2;

typedef union _UCHAR4 {
    UCHAR  Uchar[4];
    ULONG  ForceAlignment;
} UCHAR4, *PUCHAR4;

//
//  This macro copies an unaligned src byte to an aligned dst byte
//

#define CopyUchar1(Dst,Src) {                                \
    *((UCHAR1 *)(Dst)) = *((UNALIGNED UCHAR1 *)(Src)); \
    }

//
//  This macro copies an unaligned src word to an aligned dst word
//

#define CopyUchar2(Dst,Src) {                                \
    *((UCHAR2 *)(Dst)) = *((UNALIGNED UCHAR2 *)(Src)); \
    }

//
//  This macro copies an unaligned src longword to an aligned dsr longword
//

#define CopyUchar4(Dst,Src) {                                \
    *((UCHAR4 *)(Dst)) = *((UNALIGNED UCHAR4 *)(Src)); \
    }


//
//  The following macros are used to establish the semantics needed
//  to do a return from within a try-finally clause.  As a rule every
//  try clause must end with a label call try_exit.  For example,
//
//      try {
//              :
//              :
//
//      try_exit: NOTHING;
//      } finally {
//
//              :
//              :
//      }
//
//  Every return statement executed inside of a try clause should use the
//  try_return macro.  If the compiler fully supports the try-finally construct
//  then the macro should be
//
//      #define try_return(S)  { return(S); }
//
//  If the compiler does not support the try-finally construct then the macro
//  should be
//
//      #define try_return(S)  { S; goto try_exit; }
//

#define try_return(S) { S; goto try_exit; }

#if defined(__cplusplus)
}
#endif

#endif // _FSRTLP_
