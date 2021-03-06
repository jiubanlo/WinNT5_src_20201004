//------------------------------------------------------------------------------
/// <copyright file="IVsWindowPane.cs" company="Microsoft">
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

    using System;
    using System.Diagnostics;
    using System.Runtime.InteropServices;
    
    [ComImport, ComVisible(true), Guid("B0834D0F-ACFF-4EA5-809B-97CBB5D3D26B"), InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IVsWindowPane {

         void SetSite(
                [In, MarshalAs(UnmanagedType.Interface)]
                object pSP);

        [PreserveSig]
        int CreatePaneWindow(IntPtr hwndParent,int x,int y,int cx,int cy, out IntPtr pane);

        [PreserveSig]
        int GetDefaultSize(
                [Out]
                tagSIZE psize);
                 
        [PreserveSig]
        int ClosePane();

        [PreserveSig]
        int LoadViewState(
                [MarshalAs(UnmanagedType.Interface)]
                object pstream);

        [PreserveSig]
        int SaveViewState(
                [MarshalAs(UnmanagedType.Interface)]
                object pstream);

        [PreserveSig]
        int TranslateAccelerator(
                ref tagMSG lpmsg);
    }
}
