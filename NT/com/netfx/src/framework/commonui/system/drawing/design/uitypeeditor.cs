//------------------------------------------------------------------------------
// <copyright file="UITypeEditor.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.Drawing.Design {
    using System.Runtime.InteropServices;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Collections;
    using Microsoft.Win32;    
    using System.ComponentModel.Design;
    using System.Drawing;

    /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor"]/*' />
    /// <devdoc>
    ///    <para>Provides a base class for editors 
    ///       that may provide users with a user interface to visually edit
    ///       the values of the supported type or types.</para>
    /// </devdoc>
    [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.InheritanceDemand, Name="FullTrust")]
    [System.Security.Permissions.PermissionSetAttribute(System.Security.Permissions.SecurityAction.LinkDemand, Name="FullTrust")]
    public class UITypeEditor {
    
        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.UITypeEditor"]/*' />
        /// <devdoc>
        ///      In this static constructor we provide default UITypeEditors to
        ///      the TypeDescriptor.
        /// </devdoc>
        static UITypeEditor() {
            Hashtable intrinsicEditors = new Hashtable();
            
            // Our set of intrinsic editors.
            intrinsicEditors[typeof(DateTime)] = "System.ComponentModel.Design.DateTimeEditor, " + AssemblyRef.SystemDesign;
            intrinsicEditors[typeof(Array)] = "System.ComponentModel.Design.ArrayEditor, " + AssemblyRef.SystemDesign;
            intrinsicEditors[typeof(IList)] = "System.ComponentModel.Design.CollectionEditor, " + AssemblyRef.SystemDesign;
            intrinsicEditors[typeof(ICollection)] = "System.ComponentModel.Design.CollectionEditor, " + AssemblyRef.SystemDesign;
            intrinsicEditors[typeof(byte[])] = "System.ComponentModel.Design.BinaryEditor, " + AssemblyRef.SystemDesign;
            intrinsicEditors[typeof(System.IO.Stream)] = "System.ComponentModel.Design.BinaryEditor, " + AssemblyRef.SystemDesign;
            intrinsicEditors[typeof(string[])] = "System.Windows.Forms.Design.StringArrayEditor, " + AssemblyRef.SystemDesign;
            
            // Add our intrinsic editors to TypeDescriptor.
            //
            TypeDescriptor.AddEditorTable(typeof(UITypeEditor), intrinsicEditors);
        }
        
        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.UITypeEditor1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes
        ///       a new instance of the <see cref='System.Drawing.Design.UITypeEditor'/> class.
        ///    </para>
        /// </devdoc>
        public UITypeEditor() {
        }
        
        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.EditValue"]/*' />
        /// <devdoc>
        ///    <para>Edits the specified value using the editor style
        ///       provided by <see cref='System.Drawing.Design.UITypeEditor.GetEditStyle'/>.</para>
        /// </devdoc>
        public object EditValue(IServiceProvider provider, object value) {
            return EditValue(null, provider, value);
        }

        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.EditValue1"]/*' />
        /// <devdoc>
        ///    <para>Edits the specified object's value using the editor style
        ///       provided by <see cref='System.Drawing.Design.UITypeEditor.GetEditStyle'/>.</para>
        /// </devdoc>
        public virtual object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value) {
            return value;
        }

        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.GetEditStyle"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets the <see cref='System.Drawing.Design.UITypeEditorEditStyle'/>
        ///       of the Edit method.
        ///    </para>
        /// </devdoc>
        public UITypeEditorEditStyle GetEditStyle() {
            return GetEditStyle(null);
        }

        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.GetPaintValueSupported"]/*' />
        /// <devdoc>
        ///    <para> Gets a value indicating whether this editor supports painting a representation
        ///       of an object's value.</para>
        /// </devdoc>
        public bool GetPaintValueSupported() {
            return GetPaintValueSupported(null);
        }

        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.GetPaintValueSupported1"]/*' />
        /// <devdoc>
        ///    <para> Gets a value indicating whether the specified context supports painting a representation
        ///       of an object's value.</para>
        /// </devdoc>
        public virtual bool GetPaintValueSupported(ITypeDescriptorContext context) {
            return false;
        }

        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.GetEditStyle1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets the editing style of the Edit method.
        ///    </para>
        /// </devdoc>
        public virtual UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context) {
            return UITypeEditorEditStyle.None;
        }

        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.PaintValue"]/*' />
        /// <devdoc>
        ///    <para>Paints a representative value of the specified object to the
        ///       specified canvas.</para>
        /// </devdoc>
        public void PaintValue(object value, Graphics canvas, Rectangle rectangle) {
            PaintValue(new PaintValueEventArgs(null, value, canvas, rectangle));
        }

        /// <include file='doc\UITypeEditor.uex' path='docs/doc[@for="UITypeEditor.PaintValue1"]/*' />
        /// <devdoc>
        ///    <para>Paints a representative value of the specified object to the
        ///       provided canvas.</para>
        /// </devdoc>
        public virtual void PaintValue(PaintValueEventArgs e) {
        }
    }
}

