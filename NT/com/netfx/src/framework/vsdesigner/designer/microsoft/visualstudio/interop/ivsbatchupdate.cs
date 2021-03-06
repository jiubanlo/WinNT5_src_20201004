//------------------------------------------------------------------------------
/// <copyright file="IVsBatchUpdate.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
// IVsWindowPane.cs
//---------------------------------------------------------------------------
// WARNING: this file autogenerated
//---------------------------------------------------------------------------
// Copyright (c) 1999, Microsoft Corporation   All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//---------------------------------------------------------------------------

namespace Microsoft.VisualStudio.Interop {
    
    using System.Runtime.InteropServices;

    //---------------------------------------------------------------------------
    // IVsBatchUpdate
    // Any editor view or buffer that needs to process updates in a batch delayed manor can
    // implement this interface. Example: The VS text buffer will implement this interface and
    // any compiler or build system which requires the buffer be up to date will QI the buffer
    // for this interface and call FlushPendingUpdates. The text buffer will then QI all of
    // it's registered independent views for this interface and FW this request to them
    //---------------------------------------------------------------------------
    [
    ComImport, 
    ComVisible(true), 
    Guid("A2D3286E-B5AE-4981-8D32-E9053FCF997D"),
    InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)
    ]
    internal interface IVsBatchUpdate {
        void FlushPendingUpdates(int dwReserved);
    }
}

