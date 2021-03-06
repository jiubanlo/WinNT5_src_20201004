//------------------------------------------------------------------------------
// <copyright file="LinkLabelLinkClickedEvent.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Windows.Forms {

    using System.Diagnostics;

    using System;
    using System.ComponentModel;
    using System.Drawing;
    using Microsoft.Win32;


    /// <include file='doc\LinkLabelLinkClickedEvent.uex' path='docs/doc[@for="LinkLabelLinkClickedEventArgs"]/*' />
    /// <devdoc>
    ///    <para>
    ///       Provides data for the <see cref='System.Windows.Forms.LinkLabel.OnLinkClicked'/> event.
    ///    </para>
    /// </devdoc>
    [System.Runtime.InteropServices.ComVisible(true)]
    public class LinkLabelLinkClickedEventArgs : EventArgs {
        private readonly LinkLabel.Link link;
        /// <include file='doc\LinkLabelLinkClickedEvent.uex' path='docs/doc[@for="LinkLabelLinkClickedEventArgs.LinkLabelLinkClickedEventArgs"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.Windows.Forms.LinkLabelLinkClickedEventArgs'/> class, given the link.
        ///    </para>
        /// </devdoc>
        public LinkLabelLinkClickedEventArgs(LinkLabel.Link link) {
            this.link = link;
        }

        /// <include file='doc\LinkLabelLinkClickedEvent.uex' path='docs/doc[@for="LinkLabelLinkClickedEventArgs.Link"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets the <see cref='System.Windows.Forms.LinkLabel.Link'/> that was clicked.
        ///    </para>
        /// </devdoc>
        public LinkLabel.Link Link {
            get {
                return link;
            }
        }
    }
}
