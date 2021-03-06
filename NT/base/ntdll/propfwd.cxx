//+============================================================================
//
//  propfwd.cxx
//
//  This file provides (slow) forwarders of the NT4 property APIs
//  from NTDLL to OLE32.  At one time, these APIs were used in both
//  kernel mode and user mode.  They're now only used in user mode,
//  so all the property code has been consolidated into ole32.  Older
//  copies of Index Server (CI), however, still link to NTDLL, thus
//  the need for these forwarders.
//
//+============================================================================

#include <pch.cxx>
#include <windows.h>
#include <ddeml.h>      // for CP_WINUNICODE
#include <objidl.h>
#include <propidl.h>
extern "C"
{
#include <propapi.h>
}
#include <stgprop.h>
class PMemoryAllocator;
#include <propstm.hxx>
#include <align.hxx>
#include <sstream.hxx>
#include "propmac.hxx"

//+----------------------------------------------------------------------------
//
//  Function:   LoadOle32Export
//
//  Synopsis:   Load ole32.dll and get one of its exports.
//              Raises on error.
//
//+----------------------------------------------------------------------------

PVOID
LoadOle32Export( PVOID* Ole32, const PCHAR ProcedureName )
{
    NTSTATUS Status;
    const static UNICODE_STRING Ole32DllName_U = RTL_CONSTANT_STRING(L"ole32.dll");
    STRING ProcedureNameString;
    PVOID ProcedureAddress = NULL;

    Status = LdrLoadDll( NULL, NULL, &Ole32DllName_U, Ole32 );
    if( !NT_SUCCESS(Status) )
        RtlRaiseStatus( Status );

    RtlInitString( &ProcedureNameString, ProcedureName );
    Status = LdrGetProcedureAddress(
                    *Ole32,
                    &ProcedureNameString,
                    0,
                    (PVOID*) &ProcedureAddress
                    );
    if( !NT_SUCCESS(Status) )
        RtlRaiseStatus(Status);

    return( ProcedureAddress );
}


//+----------------------------------------------------------------------------
//
//  Function:   RtlConvertVariantToProperty
//
//  Synopsis:   Serialize a variant.
//
//+----------------------------------------------------------------------------

typedef SERIALIZEDPROPERTYVALUE* (*PFNStgConvertVariantToProperty) (
                                        IN PROPVARIANT const *pvar,
                                        IN USHORT CodePage,
                                        OUT SERIALIZEDPROPERTYVALUE *pprop,
                                        IN OUT ULONG *pcb,
                                        IN PROPID pid,
                                        IN BOOLEAN fVariantVector,
                                        OPTIONAL OUT ULONG *pcIndirect);


SERIALIZEDPROPERTYVALUE * PROPSYSAPI PROPAPI
RtlConvertVariantToProperty(
    IN PROPVARIANT const *pvar,
    IN USHORT CodePage,
    OPTIONAL OUT SERIALIZEDPROPERTYVALUE *pprop,
    IN OUT ULONG *pcb,
    IN PROPID pid,
    IN BOOLEAN fVariantVector,
    OPTIONAL OUT ULONG *pcIndirect)
{
    PVOID Ole32 = NULL;
    PFNStgConvertVariantToProperty ProcedureAddress;
    SERIALIZEDPROPERTYVALUE *ppropRet = NULL;

    __try
    {
        ProcedureAddress = (PFNStgConvertVariantToProperty)
                           LoadOle32Export( &Ole32, "StgConvertVariantToProperty" );

        ppropRet = ProcedureAddress( pvar,
                                     CodePage,
                                     pprop,
                                     pcb,
                                     pid,
                                     fVariantVector,
                                     pcIndirect );  // Raises on error
    }
    __finally
    {
        if( NULL != Ole32 )
            LdrUnloadDll( Ole32 );
    }

    return (ppropRet );

}




//+----------------------------------------------------------------------------
//
//  Function:   RtlConvertPropertyToVariant
//
//  Synopsis:   De-serialize a variant.
//
//+----------------------------------------------------------------------------

typedef BOOLEAN (* PFNStgConvertPropertyToVariant) (
    IN SERIALIZEDPROPERTYVALUE const *pprop,
    IN USHORT CodePage,
    OUT PROPVARIANT *pvar,
    IN PMemoryAllocator *pma);

BOOLEAN PROPSYSAPI PROPAPI
RtlConvertPropertyToVariant(
    IN SERIALIZEDPROPERTYVALUE const *pprop,
    IN USHORT CodePage,
    OUT PROPVARIANT *pvar,
    IN PMemoryAllocator *pma)
{
    BOOLEAN Ret = FALSE;
    PVOID Ole32 = NULL;
    PFNStgConvertPropertyToVariant ProcedureAddress;

    __try
    {
        ProcedureAddress = (PFNStgConvertPropertyToVariant)
                           LoadOle32Export( &Ole32, "StgConvertPropertyToVariant" );
    
        Ret = ProcedureAddress( pprop, CodePage, pvar, pma );  // Raises on error
    }
    __finally
    {
        if( NULL != Ole32 )
            LdrUnloadDll( Ole32 );
    }

    return (Ret);

}




//+----------------------------------------------------------------------------
//
//  Function:   PropertyLengthAsVariant
//
//  Synopsis:   Returns the amount of external memory will need to be
//              allocated for this variant when RtlPropertyToVariant is called.
//
//+----------------------------------------------------------------------------

typedef ULONG (*PFNStgPropertyLengthAsVariant)(
        IN SERIALIZEDPROPERTYVALUE const *pprop,
        IN ULONG cbprop,
        IN USHORT CodePage,
        IN BYTE flags);

ULONG PROPSYSAPI PROPAPI
PropertyLengthAsVariant(
    IN SERIALIZEDPROPERTYVALUE const *pprop,
    IN ULONG cbprop,
    IN USHORT CodePage,
    IN BYTE flags)
{
    ULONG Length = 0;
    PVOID Ole32 = NULL;
    PFNStgPropertyLengthAsVariant ProcedureAddress;

    __try
    {
        ProcedureAddress = (PFNStgPropertyLengthAsVariant)
                           LoadOle32Export( &Ole32, "StgPropertyLengthAsVariant" );

        Length = ProcedureAddress( pprop, cbprop, CodePage, flags );  // Raises on error
    }
    __finally
    {
        if( NULL != Ole32 )
            LdrUnloadDll( Ole32 );
    }

    return( Length);
}

//+---------------------------------------------------------------------------
//
//  Function:   RtlSetUnicodeCallouts, public
//
//  Synopsis:   Set the Unicode conversion function pointers, used by
//              RtlConvertVarianttoProperty, RtlConvertPropertyToVariant,
//              and PropertyLengthAsVariant.
//
//              These functions are no longer settable (they're defaulted by
//              ole32).
//
//---------------------------------------------------------------------------

VOID PROPSYSAPI PROPAPI
RtlSetUnicodeCallouts(
    IN UNICODECALLOUTS *pUnicodeCallouts)
{
    return;
}
