//------------------------------------------------------------------------------
/// <copyright file="IVsSolutionEvents.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
// IVsSolutionEvents.cs
//---------------------------------------------------------------------------
// WARNING: this file autogenerated
//---------------------------------------------------------------------------
// Copyright (c) 1999, Microsoft Corporation   All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//---------------------------------------------------------------------------

namespace Microsoft.VisualStudio.Interop {

    using System.Runtime.InteropServices;
    using System.Diagnostics;
    using System;

    [ComImport, ComVisible(true),Guid("A8516B56-7421-4DBD-AB87-57AF7A2E85DE"), InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown), CLSCompliant(false)]
    internal interface IVsSolutionEvents {

        [PreserveSig]
            int OnAfterOpenProject(
            IVsHierarchy pHierarchy,
            int fAdded);

        [PreserveSig]
            int OnQueryCloseProject(
            IVsHierarchy pHierarchy,
            int fRemoving,
            ref bool pfCancel);

        [PreserveSig]
            int OnBeforeCloseProject(
            IVsHierarchy pHierarchy,
            int fRemoved);

        [PreserveSig]
            int OnAfterLoadProject(
            IVsHierarchy pStubHierarchy,
            IVsHierarchy pRealHierarchy);

        [PreserveSig]
            int OnQueryUnloadProject(
            IVsHierarchy pRealHierarchy,
            ref bool pfCancel);

        [PreserveSig]
            int OnBeforeUnloadProject(
            IVsHierarchy pRealHierarchy,
            IVsHierarchy pStubHierarchy);

        [PreserveSig]
            int OnAfterOpenSolution(
            [MarshalAs(UnmanagedType.Interface)] 
            object pUnkReserved,
            int fNewSolution);

        [PreserveSig]
            int OnQueryCloseSolution(
            [MarshalAs(UnmanagedType.Interface)] 
            object pUnkReserved,
            ref bool pfCancel);

        [PreserveSig]
            int OnBeforeCloseSolution(
            [MarshalAs(UnmanagedType.Interface)] 
            object pUnkReserved);

        [PreserveSig]
            int OnAfterCloseSolution(
            [MarshalAs(UnmanagedType.Interface)] 
            object pUnkReserved);

    }
}
