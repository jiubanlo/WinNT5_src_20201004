/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    input.cxx

Abstract:

    This file contains the routines to get user input.

Author:

    Jason Hartman (JasonHa) 2001-05-22

Environment:

    User Mode

--*/

#include "precomp.hxx"



HRESULT
GetYNInput(
    PDEBUG_CONTROL Control,
    PCSTR Prompt
    )
{
    HRESULT hr;
    CHAR    Response[4];
    ULONG   ResponseLen;

    Control->Output(DEBUG_OUTPUT_NORMAL, "%s yn\n", Prompt);

    do
    {
        hr = Control->Input(Response, sizeof(Response), &ResponseLen);

        if (hr == E_ABORT) break;

        if (hr == S_OK)
        {
            if (ResponseLen != 2)
            {
                hr = E_INVALIDARG;
            }
            else
            {
                Response[0] = (CHAR)tolower(Response[0]);

                if (Response[0] != 'y' && Response[0] != 'n')
                {
                    hr = E_INVALIDARG;
                }
            }
        }

        if (hr != S_OK)
        {
            Control->Output(DEBUG_OUTPUT_WARNING, " Please answer y or n.\n");
        }
    } while (hr != S_OK);

    if (hr == S_OK && Response[0] != 'y')
    {
        hr = S_FALSE;
    }

    return hr;
}

