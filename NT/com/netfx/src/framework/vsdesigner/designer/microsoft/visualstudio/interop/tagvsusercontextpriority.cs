//------------------------------------------------------------------------------
/// <copyright file="tagVsUserContextPriority.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
// tagVsUserContextPriority.cs
//---------------------------------------------------------------------------
// WARNING: this file autogenerated
//---------------------------------------------------------------------------
// Copyright (c) 1999, Microsoft Corporation   All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//---------------------------------------------------------------------------

namespace Microsoft.VisualStudio.Interop {
    

    using System.Diagnostics;
    using System;
    
    using UnmanagedType = System.Runtime.InteropServices.UnmanagedType;

    internal class tagVsUserContextPriority {
    
       public const   int VSUC_Priority_None        = 0;      // typical/default
       public const   int VSUC_Priority_Ambient     = 100;    // ambient keyword
       public const   int VSUC_Priority_State       = 200;    // cmd ui ctx
       public const   int VSUC_Priority_Project     = 300;    // hierarchy
       public const   int VSUC_Priority_ProjectItem = 400;    // hierarchy item
       public const   int VSUC_Priority_Window      = 500;    // SEID_UserContext
       public const   int VSUC_Priority_Selection   = 600;    // selection container
       public const   int VSUC_Priority_MarkerSel   = 700;    // selection container
       public const   int VSUC_Priority_Enterprise  = 800;    // keyword for enterprise templates to target
       public const   int VSUC_Priority_WindowFrame = 900;    // to be used by tool windows that do not want to overwrite 
                                                               // SEID_UserContext, but want their context to appear at the 
                                                               // top of the RL window   //V7-49265
       public const   int VSUC_Priority_ToolWndSel  = 1000;    // for selection in a toolwindow
       public const   int VSUC_Priority_Highest     = 1100;     // just a placeholder, make sure it's the highest
    }
}
