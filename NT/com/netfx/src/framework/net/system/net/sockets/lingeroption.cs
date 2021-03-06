//------------------------------------------------------------------------------
// <copyright file="LingerOption.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------


namespace System.Net.Sockets {
    using System;
    
    
    /// <include file='doc\LingerOption.uex' path='docs/doc[@for="LingerOption"]/*' />
    /// <devdoc>
    ///    <para>Contains information for a socket's linger time, the amount of time it will
    ///       remain after closing if data remains to be sent.</para>
    /// </devdoc>
    public class LingerOption {
        bool enabled;
        int lingerTime;

        /// <include file='doc\LingerOption.uex' path='docs/doc[@for="LingerOption.LingerOption"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='Sockets.LingerOption'/> class.
        ///    </para>
        /// </devdoc>
        public LingerOption(bool enable, int seconds) {
            Enabled = enable;
            LingerTime = seconds;
        }
        
        /// <include file='doc\LingerOption.uex' path='docs/doc[@for="LingerOption.Enabled"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Enables or disables lingering after
        ///       close.
        ///    </para>
        /// </devdoc>
        public bool Enabled {
            get {
                return enabled;
            }
            set {
                enabled = value;
            }
        }

        /// <include file='doc\LingerOption.uex' path='docs/doc[@for="LingerOption.LingerTime"]/*' />
        /// <devdoc>
        ///    <para>
        ///       The amount of time, in seconds, to remain connected after a close.
        ///    </para>
        /// </devdoc>
        public int LingerTime {
            get {
                return lingerTime;
            }
            set {
                lingerTime = value;
            }
        }

    } // class LingerOption
} // namespace System.Net.Sockets
