//------------------------------------------------------------------------------
// <copyright file="ImageUrlEditor.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Web.UI.Design.MobileControls
{
    /// <include file='doc\ImageUrlEditor.uex' path='docs/doc[@for="ImageUrlEditor"]/*' />
    /// <devdoc>
    /// </devdoc>
    [
        System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand,
        Flags=System.Security.Permissions.SecurityPermissionFlag.UnmanagedCode)
    ]
    internal class ImageUrlEditor: System.Web.UI.Design.UrlEditor
    {
        /// <include file='doc\ImageUrlEditor.uex' path='docs/doc[@for="ImageUrlEditor.Caption"]/*' />
        /// <devdoc>
        /// </devdoc>
        protected override String Caption
        {
            get
            {
                return SR.GetString(SR.ImageUrlPicker_ImageCaption);
            }
        }

        /// <include file='doc\ImageUrlEditor.uex' path='docs/doc[@for="ImageUrlEditor.Filter"]/*' />
        /// <devdoc>
        /// </devdoc>
        protected override String Filter
        {
            get
            {
                return SR.GetString(SR.ImageUrlPicker_ImageFilter);
            }
        }
    }
}