//------------------------------------------------------------------------------
// <copyright file="DockStyle.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Windows.Forms {

    using System.Diagnostics;

    using System;
    using System.ComponentModel;
    using System.Drawing.Design;
    using Microsoft.Win32;


    /*
     * Copyright (c) 1997, Microsoft Corporation. All Rights Reserved.
     * Information Contained Herein is Proprietary and Confidential.
     */



    /// <include file='doc\DockStyle.uex' path='docs/doc[@for="DockStyle"]/*' />
    /// <devdoc>
    ///     Control Dock values.
    ///
    ///     When a control is docked to an edge of it's container it will
    ///     always be positioned flush against that edge while the container
    ///     resizes. If more than one control is docked to an edge, the controls
    ///     will not be placed on top of each other.
    /// </devdoc>
    [
    Editor("System.Windows.Forms.Design.DockEditor, " + AssemblyRef.SystemDesign, typeof(UITypeEditor))
    ]
    public enum DockStyle {
        /// <include file='doc\DockStyle.uex' path='docs/doc[@for="DockStyle.None"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        None   = 0,
        /// <include file='doc\DockStyle.uex' path='docs/doc[@for="DockStyle.Top"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Top    = 1,
        /// <include file='doc\DockStyle.uex' path='docs/doc[@for="DockStyle.Bottom"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Bottom = 2,
        /// <include file='doc\DockStyle.uex' path='docs/doc[@for="DockStyle.Left"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Left   = 3,
        /// <include file='doc\DockStyle.uex' path='docs/doc[@for="DockStyle.Right"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Right  = 4,
        /// <include file='doc\DockStyle.uex' path='docs/doc[@for="DockStyle.Fill"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        Fill   = 5,
    }
}
