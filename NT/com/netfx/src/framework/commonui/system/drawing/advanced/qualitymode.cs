//------------------------------------------------------------------------------
// <copyright file="QualityMode.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   QualityMode.cs  
*
* Abstract:
*
*   Quality mode constants
*
* Revision History:
*
*   05/01/2000 ericvan
*       Created it.
*
\**************************************************************************/

namespace System.Drawing.Drawing2D {

    using System.Diagnostics;

    using System.Drawing;
    using System;

    /// <include file='doc\QualityMode.uex' path='docs/doc[@for="QualityMode"]/*' />
    /// <devdoc>
    ///    <para>
    ///       Specifies the overall quality of rendering
    ///       of graphics shapes.
    ///    </para>
    /// </devdoc>
    public enum QualityMode
    {
        /// <include file='doc\QualityMode.uex' path='docs/doc[@for="QualityMode.Invalid"]/*' />
        /// <devdoc>
        ///    Specifies an invalid mode.
        /// </devdoc>
        Invalid = -1,
        /// <include file='doc\QualityMode.uex' path='docs/doc[@for="QualityMode.Default"]/*' />
        /// <devdoc>
        ///    Specifies the default mode.
        /// </devdoc>
        Default = 0,
        /// <include file='doc\QualityMode.uex' path='docs/doc[@for="QualityMode.Low"]/*' />
        /// <devdoc>
        ///    Specifies low quality, high performance
        ///    rendering.
        /// </devdoc>
        Low = 1,             // for apps that need the best performance
        /// <include file='doc\QualityMode.uex' path='docs/doc[@for="QualityMode.High"]/*' />
        /// <devdoc>
        ///    Specifies high quality, lower performance
        ///    rendering.
        /// </devdoc>
        High = 2             // for apps that need the best rendering quality                                          
    }
}
