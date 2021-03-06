//------------------------------------------------------------------------------
// <copyright file="ListComponentEditor.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Web.UI.Design.MobileControls
{
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Web.UI.MobileControls;
    using System.Windows.Forms.Design;

    /// <summary>
    ///    <para>
    ///       Provides a component editor for a Mobile List <see cref='System.Web.UI.MobileControls.List'/>
    ///       control.
    ///    </para>
    /// </summary>
    /// <seealso cref='System.Web.UI.MobileControls.List'/>
    /// <seealso cref='System.Web.UI.Design.MobileControls.ListDesigner'/>
    [
        System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand,
        Flags=System.Security.Permissions.SecurityPermissionFlag.UnmanagedCode)
    ]
    internal class ListComponentEditor : BaseTemplatedMobileComponentEditor 
    {

        // The set of pages used within the List ComponentEditor
        private static Type[] _editorPages = new Type[]
                                             {
                                                 typeof(ListGeneralPage),
                                                 typeof(ListItemsPage)
                                             };

        internal const int IDX_GENERAL = 0;
        internal const int IDX_ITEMS = 1;

        /// <summary>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.Web.UI.Design.MobileControls.ListComponentEditor'/>.
        ///    </para>
        /// </summary>
        public ListComponentEditor() : base(IDX_GENERAL)
        {
        }

        /// <summary>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.Web.UI.Design.MobileControls.ListComponentEditor'/>.
        ///    </para>
        /// </summary>
        /// <param name='initialPage'>
        ///    The index of the initial page.
        /// </param>
        public ListComponentEditor(int initialPage) : base(initialPage)
        {
        }

        /// <summary>
        ///    <para>
        ///       Gets the set of all pages in the <see cref='System.Web.UI.MobileControls.List'/>
        ///       .
        ///    </para>
        /// </summary>
        /// <returns>
        ///    <para>
        ///       An array consisting of the set of component editor pages.
        ///    </para>
        /// </returns>
        /// <remarks>
        ///    <note type="inheritinfo">
        ///       This method may
        ///       be overridden to change the set of pages to show.
        ///    </note>
        /// </remarks>
        protected override Type[] GetComponentEditorPages()
        {
            return _editorPages;
        }
    }
}
