//------------------------------------------------------------------------------
// <copyright file="MDIClient.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Windows.Forms {

    using Microsoft.Win32;
    using System;
    using System.Security.Permissions;
    using System.Collections;
    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.ComponentModel.Design.Serialization;
    using System.Diagnostics;
    using System.Drawing;
    using System.Globalization;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;
    using System.Threading;
    using System.Windows.Forms;
    
    /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient"]/*' />
    /// <internalonly/>
    /// <devdoc>
    ///    <para> 
    ///       Summary to
    ///       Come</para>
    /// </devdoc>
    [
    ToolboxItem(false),
    DesignTimeVisible(false)
    ]
    public sealed class MdiClient : Control {

        // kept in add order, not ZOrder. Need to return the correct
        // array of items...
        //
        private ArrayList children = new ArrayList();


        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.MdiClient"]/*' />
        /// <devdoc>
        ///     Creates a new MdiClient.
        /// </devdoc>
        /// <internalonly/>
        public MdiClient() : base() {
            SetStyle(ControlStyles.Selectable, false);
            BackColor = SystemColors.AppWorkspace;
            Dock = DockStyle.Fill;
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.BackgroundImage"]/*' />
        /// <devdoc>
        ///      Use parent's BackgroundImage if our BackgroundImage isn't set.
        /// </devdoc>
        [
        Localizable(true)
        ]
        public override Image BackgroundImage {
            get {
                Image result = base.BackgroundImage;
                if (result == null && ParentInternal != null)
                    result = ParentInternal.BackgroundImage;
                return result;
            }
            
            set {
                base.BackgroundImage = value;
            }
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.CreateParams"]/*' />
        /// <devdoc>
        /// </devdoc>
        /// <internalonly/>
        protected override CreateParams CreateParams {
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            get {
                CreateParams cp = base.CreateParams;

                cp.ClassName = "MDICLIENT";
                cp.Style |= NativeMethods.WS_VSCROLL | NativeMethods.WS_HSCROLL;
                cp.ExStyle |= NativeMethods.WS_EX_CLIENTEDGE;
                cp.Param = new NativeMethods.CLIENTCREATESTRUCT(IntPtr.Zero, 1);
                ISite site = (ParentInternal == null) ? null : ParentInternal.Site;
                if (site != null && site.DesignMode) {
                      cp.Style |= NativeMethods.WS_DISABLED;
                      SetState(STATE_ENABLED, false);
                }
                return cp;
            }
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.MdiChildren"]/*' />
        /// <devdoc>
        ///     The list of MDI children contained. This list
        ///     will be sorted by the order in which the children were
        ///     added to the form, not the current ZOrder.
        /// </devdoc>
        public Form[] MdiChildren {
            get {
                Form[] temp = new Form[children.Count];
                children.CopyTo(temp, 0);
                return temp;
            }
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.CreateControlsInstance"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override Control.ControlCollection CreateControlsInstance() {
            return new ControlCollection(this);
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.LayoutMdi"]/*' />
        /// <devdoc>
        ///     Arranges the MDI child forms according to value, which should be a
        ///     member of the MdiLayout enum.
        /// </devdoc>
        public void LayoutMdi(MdiLayout value) {
            if (Handle == IntPtr.Zero)
                return;

            switch (value) {
                case MdiLayout.Cascade:
                    SendMessage(NativeMethods.WM_MDICASCADE, 0, 0);
                    break;
                case MdiLayout.TileVertical:
                    SendMessage(NativeMethods.WM_MDITILE, NativeMethods.MDITILE_VERTICAL, 0);
                    break;
                case MdiLayout.TileHorizontal:
                    SendMessage(NativeMethods.WM_MDITILE, NativeMethods.MDITILE_HORIZONTAL, 0);
                    break;
                case MdiLayout.ArrangeIcons:
                    SendMessage(NativeMethods.WM_MDIICONARRANGE, 0, 0);
                    break;
            }
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.OnResize"]/*' />
        /// <devdoc>
        /// </devdoc>
        /// <internalonly/>
        protected override void OnResize(EventArgs e) {
            ISite site = (ParentInternal == null) ? null : ParentInternal.Site;
            if (site != null && site.DesignMode && Handle != IntPtr.Zero) {
                SetWindowRgn();
            }
            base.OnResize(e);
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ScaleCore"]/*' />
        /// <devdoc>
        ///     Performs the work of scaling the entire control and any child controls.
        /// </devdoc>
        protected override void ScaleCore(float dx, float dy) {

            // Don't scale child forms...
            //

            SuspendLayout();
            try {
                float xAdjust;
                float yAdjust;
                Rectangle bounds = Bounds;
                if (bounds.X < 0) {
                    xAdjust = -0.5f;
                }
                else {
                    xAdjust = 0.5f;
                }
                if (bounds.Y < 0) {
                    yAdjust = -0.5f;
                }
                else {
                    yAdjust = 0.5f;
                }
                int sx = (int)(bounds.X * dx + xAdjust);
                int sy = (int)(bounds.Y * dy + yAdjust);
                int sw = (int)((bounds.X + bounds.Width) * dx + 0.5f) - sx;
                int sh = (int)((bounds.Y + bounds.Height) * dy + 0.5f) - sy;
                SetBounds(sx, sy, sw, sh, BoundsSpecified.All);
            }
            finally {
                ResumeLayout();
            }
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.SetBoundsCore"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified) {
            ISite site = (ParentInternal == null) ? null : ParentInternal.Site;
            if (IsHandleCreated && (site == null || !site.DesignMode)) {
                Rectangle oldBounds = Bounds;
                base.SetBoundsCore(x, y, width, height, specified);
                Rectangle newBounds = Bounds;

                int yDelta = oldBounds.Height - newBounds.Height;

                // NOTE: This logic is to keep minimized MDI children anchored to
                // the bottom left of the client area, normally they are anchored
                // to the top right which just looks wierd!
                //
                NativeMethods.WINDOWPLACEMENT wp = new NativeMethods.WINDOWPLACEMENT();
                wp.length = Marshal.SizeOf(typeof(NativeMethods.WINDOWPLACEMENT));

                for (int i=0; i < Controls.Count; i++) {
                    Control ctl = Controls[i];
                    if (ctl != null && ctl is Form) {
                        Form child = (Form)ctl;
                        if (child.WindowState == FormWindowState.Minimized) {
                            UnsafeNativeMethods.GetWindowPlacement(new HandleRef(child, child.Handle), ref wp);
                            wp.ptMinPosition_y -= yDelta;
                            if (wp.ptMinPosition_y == -1) {
                                if (yDelta < 0) {
                                    wp.ptMinPosition_y = 0;
                                }
                                else {
                                    wp.ptMinPosition_y = -2;
                                }
                            }
                            wp.flags = NativeMethods.WPF_SETMINPOSITION;
                            UnsafeNativeMethods.SetWindowPlacement(new HandleRef(child, child.Handle), ref wp);
                            wp.flags = 0;
                        }
                    }
                }
            }
            else {
                base.SetBoundsCore(x, y, width, height, specified);
            }

        }

        /// <devdoc>
        /// </devdoc>
        /// <internalonly/>
        private void SetWindowRgn() {
            IntPtr rgn1 = IntPtr.Zero;
            IntPtr rgn2 = IntPtr.Zero;
            NativeMethods.RECT rect = new NativeMethods.RECT();
            CreateParams cp = CreateParams;

            SafeNativeMethods.AdjustWindowRectEx(ref rect, cp.Style, false, cp.ExStyle);

            Rectangle bounds = Bounds;
            rgn1 = SafeNativeMethods.CreateRectRgn(0, 0, bounds.Width, bounds.Height);
            try {
                rgn2 = SafeNativeMethods.CreateRectRgn(-rect.left, -rect.top,
                                             bounds.Width - rect.right, bounds.Height - rect.bottom);
                try {
                    if (rgn1 == IntPtr.Zero || rgn2 == IntPtr.Zero)
                        throw new InvalidOperationException(SR.GetString(SR.ErrorSettingWindowRegion));

                    if (SafeNativeMethods.CombineRgn(new HandleRef(null, rgn1), new HandleRef(null, rgn1), new HandleRef(null, rgn2), NativeMethods.RGN_DIFF) == 0)
                        throw new InvalidOperationException(SR.GetString(SR.ErrorSettingWindowRegion));

                    if (UnsafeNativeMethods.SetWindowRgn(new HandleRef(this, Handle), new HandleRef(null, rgn1), true) == 0) {
                        throw new InvalidOperationException(SR.GetString(SR.ErrorSettingWindowRegion));
                    }
                    else {
                        // The hwnd now owns the region.
                        rgn1 = IntPtr.Zero;
                    }
                }
                finally {
                    if (rgn2 != IntPtr.Zero) {
                        SafeNativeMethods.DeleteObject(new HandleRef(null, rgn2));
                    }
                }
            }
            finally {
                if (rgn1 != IntPtr.Zero) {
                    SafeNativeMethods.DeleteObject(new HandleRef(null, rgn1)); 
                }
            }
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ShouldSerializeBackColor"]/*' />
        internal override bool ShouldSerializeBackColor() {
            return BackColor != SystemColors.AppWorkspace;
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ShouldSerializeLocation"]/*' />
        private bool ShouldSerializeLocation() {
            return false;
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ShouldSerializeSize"]/*' />
        internal override bool ShouldSerializeSize() {
            return false;
        }
        

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.WndProc"]/*' />
        /// <devdoc>
        /// </devdoc>
        /// <internalonly/>
        [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m) {
            switch (m.Msg) {
                case NativeMethods.WM_CREATE:
                    if (ParentInternal != null && ParentInternal.Site != null && ParentInternal.Site.DesignMode && Handle != IntPtr.Zero) {
                        SetWindowRgn();
                    }
                    break;
                case NativeMethods.WM_SETFOCUS:
                    InvokeGotFocus(ParentInternal, EventArgs.Empty);
                    Form childForm = null;
                    if (ParentInternal is Form) {
                        childForm = ((Form)ParentInternal).ActiveMdiChild;
                    }
                    if (childForm == null  &&  MdiChildren.Length > 0) {
                        childForm = MdiChildren[0];
                    }
                    if (childForm != null && childForm.Visible) {
                        childForm.Active = true;
                    }

                    // Do not use control's implementation of WmSetFocus
		    // as it will improperly activate this control. 
	            ImeSetFocus();
		    DefWndProc(ref m);
		    InvokeGotFocus(this, EventArgs.Empty);
		    return;
                case NativeMethods.WM_KILLFOCUS:
                    InvokeLostFocus(ParentInternal, EventArgs.Empty);
                    break;
            }
            base.WndProc(ref m);
        }

        /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ControlCollection"]/*' />
        /// <devdoc>
        ///     Collection of controls...
        /// </devdoc>
        new public class ControlCollection : Control.ControlCollection {
            private MdiClient owner;

            /*C#r: protected*/

            /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ControlCollection.ControlCollection"]/*' />
            /// <devdoc>
            ///    <para>[To be supplied.]</para>
            /// </devdoc>
            public ControlCollection(MdiClient owner)
            : base(owner) {
                this.owner = owner;
            }

            /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ControlCollection.Add"]/*' />
            /// <devdoc>
            ///     Adds a control to the MDI Container. This child must be
            ///     a Form that is marked as an MDI Child to be added to the
            ///     container. You should not call this directly, but rather
            ///     set the child form's (ctl) MDIParent property:
            /// <code>
            ///     //     wrong
            ///     Form child = new ChildForm();
            ///     this.getMdiClient().add(child);
            ///     //     right
            ///     Form child = new ChildForm();
            ///     child.setMdiParent(this);
            /// </code>
            /// </devdoc>
            public override void Add(Control value) {
                if (!(value is Form) || !((Form)value).IsMdiChild) {
                    throw new ArgumentException(SR.GetString(SR.MDIChildAddToNonMDIParent), "value");
                }
                if (owner._CreateThreadId != value._CreateThreadId) {
                    throw new ArgumentException(SR.GetString(SR.AddDifferentThreads), "value");
                }
                owner.children.Add((Form)value);
                base.Add(value);
            }

            /// <include file='doc\MDIClient.uex' path='docs/doc[@for="MdiClient.ControlCollection.Remove"]/*' />
            /// <devdoc>
            ///     Removes a child control.
            /// </devdoc>
            public override void Remove(Control value) {
                owner.children.Remove(value);
                base.Remove(value);
            }
        }
    }
}
