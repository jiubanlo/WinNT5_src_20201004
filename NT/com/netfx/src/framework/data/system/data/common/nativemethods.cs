//------------------------------------------------------------------------------
// <copyright file="NativeMethods.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

using System;
using System.Runtime.InteropServices;

namespace System.Data.Common {

    sealed internal class NativeMethods {

        [
        Guid("0C733A5E-2A1C-11CE-ADE5-00AA0044773D"),
        InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown),
        ComImport()
        ]
        internal interface ITransactionJoin {
            [PreserveSig]
            int GetOptionsObject(/*deleted parameter signature*/);

            void JoinTransaction(
                [In, MarshalAs(UnmanagedType.Interface)] object punkTransactionCoord,
                [In] Int32 isoLevel,
                [In] Int32 isoFlags,
                [In] IntPtr pOtherOptions);
        }

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        static internal extern int GetCurrentProcessId();

        // $UNDONE: whats the impact of ExactSpelling, SqlConnection used false, SqlDebugging used true
        //[DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        //static internal extern IntPtr MapViewOfFile(IntPtr hFileMappingObject, int dwDesiredAccess, int dwFileOffsetHigh, int dwFileOffsetLow, int dwNumberOfBytesToMap);

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        static internal extern IntPtr MapViewOfFile(IntPtr hFileMappingObject, int dwDesiredAccess, int dwFileOffsetHigh, int dwFileOffsetLow, int dwNumberOfBytesToMap);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        static internal extern IntPtr OpenFileMappingA(int dwDesiredAccess, bool bInheritHandle, [MarshalAs(UnmanagedType.LPStr)] string lpName);
        
        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        static internal extern IntPtr CreateFileMappingA(IntPtr hFile, IntPtr pAttr, int flProtect, int dwMaximumSizeHigh, int dwMaximumSizeLow, [MarshalAs(UnmanagedType.LPStr)] string lpName);

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        static internal extern bool UnmapViewOfFile(IntPtr lpBaseAddress);

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern bool CloseHandle(IntPtr handle);
        
        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=false)]
        static internal extern uint GetLastError();

         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern bool  AllocateAndInitializeSid(
                      IntPtr pIdentifierAuthority, // authority
                      byte nSubAuthorityCount,                        // count of subauthorities
                      int dwSubAuthority0,                          // subauthority 0
                      int dwSubAuthority1,                          // subauthority 1
                      int dwSubAuthority2,                          // subauthority 2
                      int dwSubAuthority3,                          // subauthority 3
                      int dwSubAuthority4,                          // subauthority 4
                      int dwSubAuthority5,                          // subauthority 5
                      int dwSubAuthority6,                          // subauthority 6
                      int dwSubAuthority7,                          // subauthority 7
                      ref IntPtr  pSid );                                   // SID

                      
         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern int GetLengthSid(
                    IntPtr pSid);   // SID to query

         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern bool InitializeAcl(
                    IntPtr pAcl,            // ACL
                    int nAclLength,     // size of ACL
                    int dwAclRevision );  // revision level of ACL

         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern bool AddAccessDeniedAce(
                    IntPtr pAcl,            // access control list
                    int dwAceRevision,  // ACL revision level
                    int AccessMask,     // access mask
                    IntPtr pSid  );           // security identifier

         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern bool AddAccessAllowedAce(
                    IntPtr pAcl,            // access control list
                    int dwAceRevision,  // ACL revision level
                    uint AccessMask,     // access mask
                    IntPtr pSid  );           // security identifier
                    
         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern bool InitializeSecurityDescriptor(
                    IntPtr pSecurityDescriptor, // SD
                    int dwRevision );                         // revision level
         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        static internal extern bool SetSecurityDescriptorDacl(
                    IntPtr pSecurityDescriptor, // SD
                    bool bDaclPresent,                        // DACL presence
                    IntPtr pDacl,                               // DACL
                    bool bDaclDefaulted);                       // default DACL

         [DllImport(ExternDll.Advapi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
         static internal extern void FreeSid(
                    IntPtr pSid);   // SID to free


    }
}
