//------------------------------------------------------------------------------
// <copyright file="GridToolTip.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.Windows.Forms.PropertyGridInternal {
    using System.Runtime.Serialization.Formatters;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;
    using System.Diagnostics;

    using System;
    using System.Collections;
    
    using System.Windows.Forms;
    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.IO;
    using System.Drawing;
    using Microsoft.Win32;
    using Message = System.Windows.Forms.Message;

    internal class GridToolTip : Control {

        Control[]    controls;
        string       toolTipText;
        NativeMethods.TOOLINFO_T[]   toolInfos;
        bool         dontShow;
        Point        lastMouseMove = Point.Empty;

        internal GridToolTip(Control[] controls) {
            this.controls = controls;
            SetStyle(ControlStyles.UserPaint, false);
            this.Font = controls[0].Font;
            this.toolInfos = new NativeMethods.TOOLINFO_T[controls.Length];
            
            for (int i = 0; i < controls.Length; i++) {
                  controls[i].HandleCreated += new EventHandler(this.OnControlCreateHandle);
                  controls[i].HandleDestroyed += new EventHandler(this.OnControlDestroyHandle);
                  
                  if (controls[i].IsHandleCreated) {
                     SetupToolTip(controls[i]);
                  }
            }
        }

        public string ToolTip{
            get {
               return toolTipText;
            }
            set {  
                  this.Reset();

                  this.toolTipText = value;

                  if (this.IsHandleCreated) {

                     bool visible = this.Visible;

                     if (visible){
                        this.Visible = false;
                     }

                     // here's a hack.  if we give
                     // the tooltip an empty string, it won't come back
                     // so we just force it hidden instead
                     //
                     if (value == null || value.Length == 0){
                        dontShow = true;
                        value = "";
                     }
                     else{
                        dontShow = false;
                     }
                     
                     for (int i = 0; i < controls.Length; i++) {
                        UnsafeNativeMethods.SendMessage(new HandleRef(this, Handle), NativeMethods.TTM_UPDATETIPTEXT, 0, GetTOOLINFO(controls[i]));
                     }

                     if (visible && !dontShow){
                        this.Visible = true;
                     }
                  
               }
            }
        }


        /// <include file='doc\GridToolTip.uex' path='docs/doc[@for="GridToolTip.CreateParams"]/*' />
        /// <devdoc>
        ///     The createParams to create the window.
        /// </devdoc>
        /// <internalonly/>
        protected override  CreateParams CreateParams {
            get {
                NativeMethods.INITCOMMONCONTROLSEX icc = new NativeMethods.INITCOMMONCONTROLSEX();
                icc.dwICC = NativeMethods.ICC_TAB_CLASSES;
                SafeNativeMethods.InitCommonControlsEx(icc);
                CreateParams cp = new CreateParams();
                cp.Parent = IntPtr.Zero;
                cp.ClassName = NativeMethods.TOOLTIPS_CLASS;
                cp.Style |= NativeMethods.TTS_ALWAYSTIP;
                cp.ExStyle = 0;
                cp.Caption = this.ToolTip;
                return cp;
            }
        }

        private NativeMethods.TOOLINFO_T GetTOOLINFO(Control c) {
        
            int index = Array.IndexOf(controls, c);
            
            Debug.Assert(index != -1, "Failed to find control in tooltip array");
            
            if (toolInfos[index] == null){
               toolInfos[index] = new NativeMethods.TOOLINFO_T();
               toolInfos[index].cbSize = Marshal.SizeOf(typeof(NativeMethods.TOOLINFO_T));
               toolInfos[index].uFlags |= NativeMethods.TTF_IDISHWND | NativeMethods.TTF_TRANSPARENT | NativeMethods.TTF_SUBCLASS;
            }
            toolInfos[index].lpszText = this.toolTipText;
            toolInfos[index].hwnd = c.Handle;
            toolInfos[index].uId = c.Handle;
            return toolInfos[index];
        }

        private bool MouseMoved(Message msg){
            bool moved = true;

            Point newMove = new Point(NativeMethods.Util.LOWORD(msg.LParam), NativeMethods.Util.HIWORD(msg.LParam));

            // check if the mouse has actually moved...
            if (lastMouseMove == newMove){
                  moved = false;
            }

            lastMouseMove = newMove;
            return moved;
        }

        private void OnControlCreateHandle(object sender, EventArgs e){
            SetupToolTip((Control)sender);
        }

        private void OnControlDestroyHandle(object sender, EventArgs e){
            if (IsHandleCreated) {
                UnsafeNativeMethods.SendMessage(new HandleRef(this, Handle), NativeMethods.TTM_DELTOOL, 0, GetTOOLINFO((Control)sender));
            }
        }
        
        protected override void OnHandleCreated(EventArgs e) {
            base.OnHandleCreated(e);
            for (int i = 0; i < controls.Length; i++) {
                  if (controls[i].IsHandleCreated) {
                     SetupToolTip(controls[i]);
                  }
            }
        }

        private void SetupToolTip(Control c) {
            
            if (this.IsHandleCreated) {
               SafeNativeMethods.SetWindowPos(new HandleRef(this, Handle), NativeMethods.HWND_TOPMOST,
                                 0, 0, 0, 0,
                                 NativeMethods.SWP_NOMOVE | NativeMethods.SWP_NOSIZE |
                                 NativeMethods.SWP_NOACTIVATE);

               if (0 == (int)UnsafeNativeMethods.SendMessage(new HandleRef(this, Handle), NativeMethods.TTM_ADDTOOL, 0, GetTOOLINFO(c))) {
                     Debug.Fail("TTM_ADDTOOL failed for " + c.GetType().Name);
               }

            // Setting the max width has the added benefit of enabling multiline
            // tool tips! subhag 66503)
            //
            UnsafeNativeMethods.SendMessage(new HandleRef(this, Handle), NativeMethods.TTM_SETMAXTIPWIDTH, 0, SystemInformation.MaxWindowTrackSize.Width);


            }
        }

        public void Reset(){

            // okay, this resets the tooltip state,
            // which can get screwed up when we leave the window
            // then reenter.  So we set the tooltip to null,
            // update the text, then it back to what it was, so the tooltip
            // thinks it's back in the regular state again
            //
            string oldText = this.ToolTip;
            this.toolTipText = "";
            for (int i = 0; i < controls.Length; i++) {
               if (0 == (int)UnsafeNativeMethods.SendMessage(new HandleRef(this, Handle), NativeMethods.TTM_UPDATETIPTEXT, 0, GetTOOLINFO(controls[i]))) {
                    //Debug.Fail("TTM_UPDATETIPTEXT failed for " + controls[i].GetType().Name);
               }
            }
            this.toolTipText = oldText;
            this.SendMessage(NativeMethods.TTM_UPDATE, 0, 0);
        }

        protected override void WndProc(ref Message msg) {
            switch (msg.Msg) {
               case NativeMethods.WM_SHOWWINDOW:
                  if ((int)msg.WParam != 0 && dontShow){
                     msg.WParam = IntPtr.Zero;
                  }
                  break;
            }
            base.WndProc(ref msg);
        }
    }
}
