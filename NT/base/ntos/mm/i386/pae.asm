        title  "pae"
;++
;
; Copyright (c) 1989, 2000  Microsoft Corporation
;
; Module Name:
;
;    pae.asm
;
; Abstract:
;
;    This module implements the code necessary to swap PTEs on a PAE system.
;
; Author:
;
;    Landy Wang (landyw)  15-Nov-1998
;
; Environment:
;
;    Kernel mode only.
;
; Revision History:
;
;--

.586p
        .xlist
include callconv.inc
FPOFRAME macro a, b
.FPO ( a, b, 0, 0, 0, 0 )
endm
        .list

_TEXT$00   SEGMENT PARA PUBLIC 'CODE'
        ASSUME  DS:FLAT, ES:FLAT, SS:NOTHING, FS:NOTHING, GS:NOTHING

        page , 132
        subttl "Interlocked Swap PTE"

;++
;
; VOID
; InterlockedExchangePte (
;     IN OUT PMMPTE Destination,
;     IN ULONGLONG Exchange
;     )
;
; Routine Description:
;
;     This function performs an interlocked swap of a PTE.  This is only needed
;     for the PAE architecture where the PTE width is larger than the register
;     width.
;
;     Both PTEs must be valid or a careful write would have been done instead.
;
; Arguments:
;
;     PtePointer - Address of PTE to update with new value.
;
;     NewPteContents - The new value to put in the PTE.
;
; Return Value:
;
;     None.
;
;--

cPublicProc _InterlockedExchangePte ,3

    push    ebx
    push    esi

    mov     ebx, [esp] + 16         ; ebx = NewPteContents lowpart
    mov     ecx, [esp] + 20         ; ebx = NewPteContents highpart

    mov     esi, [esp] + 12         ; esi = PtePointer

    mov     edx, [esi] + 4
    mov     eax, [esi]              ; edx:eax = target pte contents

swapagain:

    ;
    ; cmpxchg loads edx:eax with the new contents of the target quadword
    ; in the event of failure
    ;

    lock cmpxchg8b qword ptr [esi]  ; compare and exchange

    jnz     short swapagain         ; if z clear, exchange failed

    pop     esi
    pop     ebx

    stdRET   _InterlockedExchangePte
stdENDP _InterlockedExchangePte

_TEXT$00   ends

        end
