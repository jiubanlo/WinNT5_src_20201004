//------------------------------------------------------------------------------
// <copyright file="PrintPreviewControl.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Windows.Forms {
    using System.Runtime.Serialization.Formatters;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;
    using System.Diagnostics;
    using System;
    using System.Security.Permissions;
    using System.Drawing;
    using Microsoft.Win32;
    using System.ComponentModel;
    using System.Drawing.Printing;
    using CodeAccessPermission = System.Security.CodeAccessPermission;

    /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl"]/*' />
    /// <devdoc>
    ///    <para>
    ///       The raw "preview" part of print previewing, without any dialogs or buttons.
    ///       Most PrintPreviewControl's are found on PrintPreviewDialog's,
    ///       but they don't have to be.
    ///    </para>
    /// </devdoc>
    [DefaultProperty("Document")]
    public class PrintPreviewControl : Control {
        Size virtualSize = new Size(1,1);
        Point position = new Point(0,0);
        Point lastOffset;
        bool antiAlias;

        private const int SCROLL_PAGE = 100;
        private const int SCROLL_LINE = 5;

        private const int border = 10; // spacing per page, in mm

        private PrintDocument document;
        private PreviewPageInfo[] pageInfo; // null if needs refreshing
        private int startPage = 0;  // 0-based
        private int rows = 1;
        private int columns = 1;
        private bool autoZoom = true;

        // The following are all computed by ComputeLayout
        private bool layoutOk = false;
        private Size imageSize = System.Drawing.Size.Empty; // 100ths of inch, not pixels
        private Point screendpi = Point.Empty;
        private double zoom = 0.3;
        bool pageInfoCalcPending = false;
        bool exceptionPrinting = false;

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.PrintPreviewControl"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.Windows.Forms.PrintPreviewControl'/> class.
        ///    </para>
        /// </devdoc>
        public PrintPreviewControl() {
            ResetBackColor();
            ResetForeColor();
            Size = new Size(100, 100);
            SetStyle(ControlStyles.ResizeRedraw, false);
            SetStyle(ControlStyles.Opaque, true);
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.UseAntiAlias"]/*' />
        [
        SRCategory(SR.CatBehavior), 
        DefaultValue(false),
        SRDescription(SR.PrintPreviewAntiAliasDescr)
        ]
        public bool UseAntiAlias {
            get {
                return antiAlias;
            }
            set {
                antiAlias = value;
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.AutoZoom"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets a value If true (the default), resizing the control or changing the number of pages shown
        ///       will automatically adjust Zoom to make everything visible.
        ///    </para>
        /// </devdoc>
        [
        SRCategory(SR.CatBehavior),
        DefaultValue(true),
        SRDescription(SR.PrintPreviewAutoZoomDescr)
        ]
        public bool AutoZoom {
            get { return autoZoom;}
            set {
                autoZoom = value;
                InvalidateLayout();
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.Document"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets a value indicating the document to preview.
        ///       
        ///    </para>
        /// </devdoc>
        [
        SRCategory(SR.CatBehavior),
        DefaultValue(null),
        SRDescription(SR.PrintPreviewDocumentDescr)
        ]
        public PrintDocument Document {
            get { return document;}
            set {
                document = value;
                InvalidatePreview();
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.Columns"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets the number of pages
        ///       displayed horizontally across the screen.
        ///    </para>
        /// </devdoc>
        [
        DefaultValue(1),
        SRCategory(SR.CatLayout),
        SRDescription(SR.PrintPreviewColumnsDescr)
        ]
        public int Columns {
            get { return columns;}
            set {
                columns = value;
                InvalidateLayout();
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.CreateParams"]/*' />
        /// <internalonly/>
        /// <devdoc>
        ///    <para>
        ///       Gets the CreateParams used to create the window.
        ///       If a subclass overrides this function, it must call the base implementation.
        ///       
        ///    </para>
        /// </devdoc>
        protected override CreateParams CreateParams {
            [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
            get {
                CreateParams cp = base.CreateParams;
                cp.Style |= NativeMethods.WS_HSCROLL;
                cp.Style |= NativeMethods.WS_VSCROLL;
                return cp;
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.Position"]/*' />
        /// <devdoc>
        ///     The virtual coordinate of the upper left visible pixel.
        /// </devdoc>

        [
        SRCategory(SR.CatLayout),
        Browsable(false),
        DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),
        SRDescription(SR.ControlWithScrollbarsPositionDescr)
        ]
        private Point Position {
            get { return position;}
            set {
                SetPositionNoInvalidate(value);
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.Rows"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets the number of pages
        ///       displayed vertically down the screen.
        ///    </para>
        /// </devdoc>
        [
        DefaultValue(1),
        SRDescription(SR.PrintPreviewRowsDescr),
        SRCategory(SR.CatBehavior)
        ]
        public int Rows {
            get { return rows;}
            set {
                rows = value;
                InvalidateLayout();
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.Text"]/*' />
        [
        Browsable(false), EditorBrowsable(EditorBrowsableState.Never), 
        Bindable(false), 
        DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)
        ]                
        public override string Text {
            get {
                return base.Text;
            }
            set {
                base.Text = value;
            }
        }
        
        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.TextChanged"]/*' />
        /// <internalonly/>
        [Browsable(false), EditorBrowsable(EditorBrowsableState.Never)]
        new public event EventHandler TextChanged {
            add {
                base.TextChanged += value;
            }
            remove {
                base.TextChanged -= value;
            }
        }
        
        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.StartPage"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets the page number of the upper left page.
        ///       
        ///    </para>
        /// </devdoc>
        [
        DefaultValue(0),
        SRDescription(SR.PrintPreviewStartPageDescr),
        SRCategory(SR.CatBehavior)
        ]
        public int StartPage {
            get { 
                int value = startPage;
                if (pageInfo != null) {
                    value = Math.Min(value, pageInfo.Length - (rows * columns));
                }
                value = Math.Max(value, 0);

                return value;
            }
            set {
                int oldValue = StartPage;
                startPage = value;

                if (oldValue != startPage) {
                    InvalidateLayout();
                    OnStartPageChanged(EventArgs.Empty);
                }
            }
        }

        private static readonly object EVENT_STARTPAGECHANGED = new object();

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.StartPageChanged"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [SRCategory(SR.CatPropertyChanged), SRDescription(SR.RadioButtonOnStartPageChangedDescr)]
        public event EventHandler StartPageChanged {
            add {
                Events.AddHandler(EVENT_STARTPAGECHANGED, value);
            }
            remove {
                Events.RemoveHandler(EVENT_STARTPAGECHANGED, value);
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.VirtualSize"]/*' />
        /// <devdoc>
        ///     How big the control would be if the screen was infinitely large.
        /// </devdoc>
        [
        SRCategory(SR.CatLayout),
        Browsable(false), EditorBrowsable(EditorBrowsableState.Never),
        DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden),
        SRDescription(SR.ControlWithScrollbarsVirtualSizeDescr)
        ]
        private Size VirtualSize {
            get { return virtualSize;}
            set {
                SetVirtualSizeNoInvalidate(value);
                Invalidate();
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.Zoom"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets a value indicating how large the pages will appear.
        ///    </para>
        /// </devdoc>
        [
        SRCategory(SR.CatBehavior),
        SRDescription(SR.PrintPreviewZoomDescr)
        ]
        public double Zoom {
            get { return zoom;}
            set {
                if (value <= 0)
                    throw new ArgumentException(SR.GetString(SR.PrintPreviewControlZoomNegative));
                autoZoom = false;
                zoom = value;
                InvalidateLayout();
            }
        }

        private int AdjustScroll(Message m, int pos, int maxPos) {
            switch (NativeMethods.Util.LOWORD(m.WParam)) {
                case NativeMethods.SB_THUMBPOSITION:
                case NativeMethods.SB_THUMBTRACK:
                    pos = NativeMethods.Util.HIWORD(m.WParam);
                    break;
                case NativeMethods.SB_LINEUP:
                    if (pos > SCROLL_LINE) {
                        pos-=SCROLL_LINE;
                    }
                    else {
                        pos = 0;
                    }
                    break;
                case NativeMethods.SB_LINEDOWN:
                    if (pos < maxPos-SCROLL_LINE) {
                        pos+=SCROLL_LINE;
                    }
                    else {
                        pos = maxPos;
                    }
                    break;
                case NativeMethods.SB_PAGEUP:
                    if (pos > SCROLL_PAGE) {
                        pos-=SCROLL_PAGE;
                    }
                    else {
                        pos = 0;
                    }
                    break;
                case NativeMethods.SB_PAGEDOWN:
                    if (pos < maxPos-SCROLL_PAGE) {
                        pos+=SCROLL_PAGE;
                    }
                    else {
                        pos = maxPos;
                    }
                    break;
            }
            return pos;
        }


        // This function computes everything in terms of physical size (millimeters), not pixels
        // CONSIDER: In multi-page mode, assumes all pages are same size
        private void ComputeLayout() {
            Debug.Assert(pageInfo != null, "Must call ComputePreview first");
            layoutOk = true;
            if (pageInfo.Length == 0) {
                ClientSize = Size;
                return;
            }

            Graphics tempGraphics = CreateGraphicsInternal();
            IntPtr dc = tempGraphics.GetHdc();
            screendpi = new Point(UnsafeNativeMethods.GetDeviceCaps(new HandleRef(tempGraphics, dc), NativeMethods.LOGPIXELSX),
                                  UnsafeNativeMethods.GetDeviceCaps(new HandleRef(tempGraphics, dc), NativeMethods.LOGPIXELSY));
            tempGraphics.ReleaseHdcInternal(dc);
            tempGraphics.Dispose();

            Size pageSize = pageInfo[StartPage].PhysicalSize;
            Size controlPhysicalSize = new Size(PixelsToPhysical(new Point(Size), screendpi));

            if (autoZoom) {
                double zoomX = ((double) controlPhysicalSize.Width - border*(columns + 1)) / (columns*pageSize.Width);
                double zoomY = ((double) controlPhysicalSize.Height - border*(rows + 1)) / (rows*pageSize.Height);
                zoom = Math.Min(zoomX, zoomY);
            }

            imageSize = new Size((int) (zoom*pageSize.Width), (int) (zoom*pageSize.Height));
            int virtualX = (imageSize.Width * columns) + border * (columns +1);
            int virtualY = (imageSize.Height * rows) + border * (rows +1);
            SetVirtualSizeNoInvalidate(new Size(PhysicalToPixels(new Point(virtualX, virtualY), screendpi)));
        }

        // "Prints" the document to memory
        private void ComputePreview() {
            int oldStart = StartPage;

            if (document == null)
                pageInfo = new PreviewPageInfo[0];
            else {
                IntSecurity.SafePrinting.Demand(); 

                PrintController oldController = document.PrintController;
                PreviewPrintController previewController = new PreviewPrintController();
                previewController.UseAntiAlias = UseAntiAlias;
                document.PrintController = new PrintControllerWithStatusDialog(previewController, 
                                                                               SR.GetString(SR.PrintControllerWithStatusDialog_DialogTitlePreview));

                // Want to make sure we've reverted any security asserts before we call Print -- that calls into user code
                document.Print();
                pageInfo = previewController.GetPreviewPageInfo();
                Debug.Assert(pageInfo != null, "ReviewPrintController did not give us preview info");

                document.PrintController = oldController;
            }

            if (oldStart != StartPage) {
                OnStartPageChanged(EventArgs.Empty);
            }
        }

        // Recomputes the sizes and positions of pages without forcing a new "preview print"
        private void InvalidateLayout() {
            layoutOk = false;        
            Invalidate();
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.InvalidatePreview"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Refreshes the preview of the document.
        ///    </para>
        /// </devdoc>
        public void InvalidatePreview() {
            pageInfo = null;
            InvalidateLayout();
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.OnResize"]/*' />
        /// <internalonly/>
        /// <devdoc>
        ///    <para>
        ///       Invalidate the layout, if necessary.
        ///    </para>
        /// </devdoc>
        protected override void OnResize(EventArgs eventargs) {
            if (autoZoom) {
                InvalidateLayout();
            }
            else {
                Point imagePixels = PhysicalToPixels(new Point(imageSize), screendpi);
                Point virtualPixels = new Point(VirtualSize);

                // center pages on screen if small enough
                Point offset = new Point(Math.Max(0, (Size.Width - virtualPixels.X) / 2),
                                         Math.Max(0, (Size.Height - virtualPixels.Y) / 2));
                offset.X -= Position.X;
                offset.Y -= Position.Y;

                if (lastOffset.X != offset.X || lastOffset.Y != offset.Y) {
                    Invalidate();
                }
            }
            base.OnResize(eventargs);
        }

        void CalculatePageInfo() {
            if (pageInfoCalcPending) {
                return;
            }

            pageInfoCalcPending = true;
            try {
                if (pageInfo == null) {
                    try {
                        ComputePreview();
                    }
                    catch (Exception e) {
                        exceptionPrinting = true;
                        throw e;
                    }
                    finally {
                        Invalidate();
                    }
                }
            }
            finally {
                pageInfoCalcPending = false;
            }
        }
            

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.OnPaint"]/*' />
        /// <internalonly/>
        /// <devdoc>
        ///    <para>
        ///       Paints the control.
        ///    </para>
        /// </devdoc>
        protected override void OnPaint(PaintEventArgs pevent) {
            Brush backBrush = new SolidBrush(BackColor);

            try {
                if (pageInfo == null || pageInfo.Length == 0) {
                    pevent.Graphics.FillRectangle(backBrush, ClientRectangle);

                    if (pageInfo != null || exceptionPrinting) {
                        // Calculate formats
                        StringFormat format = new StringFormat();
                        format.Alignment = ControlPaint.TranslateAlignment(ContentAlignment.MiddleCenter);
                        format.LineAlignment = ControlPaint.TranslateLineAlignment(ContentAlignment.MiddleCenter);

                        // Do actual drawing
                        SolidBrush brush = new SolidBrush(ForeColor);
                        try {
                            if (exceptionPrinting) {
                                pevent.Graphics.DrawString(SR.GetString(SR.PrintPreviewExceptionPrinting), Font, brush, ClientRectangle, format);
                            }
                            else {
                                pevent.Graphics.DrawString(SR.GetString(SR.PrintPreviewNoPages), Font, brush, ClientRectangle, format);
                            }
                        }
                        finally {
                            brush.Dispose();
                            format.Dispose();
                        }
                    }
                    else {
                        BeginInvoke(new MethodInvoker(CalculatePageInfo));
                    }
                }
                else {
                    if (!layoutOk)
                        ComputeLayout();

                    Point imagePixels = PhysicalToPixels(new Point(imageSize), screendpi);
                    Point virtualPixels = new Point(VirtualSize);

                    // center pages on screen if small enough
                    Point offset = new Point(Math.Max(0, (Size.Width - virtualPixels.X) / 2),
                                             Math.Max(0, (Size.Height - virtualPixels.Y) / 2));
                    offset.X -= Position.X;
                    offset.Y -= Position.Y;
                    lastOffset = offset;

                    int borderPixelsX = PhysicalToPixels(border, screendpi.X);
                    int borderPixelsY = PhysicalToPixels(border, screendpi.Y);

                    Region originalClip = pevent.Graphics.Clip;
                    Rectangle[] pageRenderArea = new Rectangle[rows * columns];

                    try {
                        for (int row = 0; row < rows; row++) {
                            for (int column = 0; column < columns; column++) {
                                int imageIndex = StartPage + column + row*columns;
                                if (imageIndex < pageInfo.Length) {
                                    int x = offset.X + borderPixelsX * (column + 1) + imagePixels.X * column;
                                    int y = offset.Y + borderPixelsY * (row + 1) + imagePixels.Y * row;

                                    pageRenderArea[imageIndex - StartPage] = new Rectangle(x, y, imagePixels.X, imagePixels.Y);
                                    pevent.Graphics.ExcludeClip(pageRenderArea[imageIndex - StartPage]);
                                }
                            }
                        }

                        pevent.Graphics.FillRectangle(backBrush, ClientRectangle);
                    }
                    finally {
                        pevent.Graphics.Clip = originalClip;
                    }

                    for (int i=0; i<pageRenderArea.Length; i++) {
                        if (i + StartPage < pageInfo.Length) {
                            Rectangle box = pageRenderArea[i];
                            pevent.Graphics.DrawRectangle(Pens.Black, box);
                            box.Inflate(-1, -1);
                            pevent.Graphics.FillRectangle(new SolidBrush(ForeColor), box);
                            if (pageInfo[i + StartPage].Image != null) {
                                pevent.Graphics.DrawImage(pageInfo[i + StartPage].Image, box);
                            }
                            box.Width --;
                            box.Height--;
                            pevent.Graphics.DrawRectangle(Pens.Black, box);
                        }
                    }
                }
            }
            finally {
                backBrush.Dispose();
            }

            base.OnPaint(pevent); // raise paint event
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.OnStartPageChanged"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        protected virtual void OnStartPageChanged(EventArgs e) {
            EventHandler eh = Events[EVENT_STARTPAGECHANGED] as EventHandler;
            if (eh != null) {
                eh(this, e);
            }
        }

        private static int PhysicalToPixels(int physicalSize, int dpi) {
            return(int) (physicalSize * dpi / 100.0);
        }

        private static Point PhysicalToPixels(Point physical, Point dpi) {
            return new Point(PhysicalToPixels(physical.X, dpi.X),
                             PhysicalToPixels(physical.Y, dpi.Y));
        }

        private static Size PhysicalToPixels(Size physicalSize, Point dpi) {
            return new Size(PhysicalToPixels(physicalSize.Width, dpi.X),
                            PhysicalToPixels(physicalSize.Height, dpi.Y));
        }

        private static int PixelsToPhysical(int pixels, int dpi) {
            return(int) (pixels * 100.0 / dpi);
        }

        private static Point PixelsToPhysical(Point pixels, Point dpi) {
            return new Point(PixelsToPhysical(pixels.X, dpi.X),
                             PixelsToPhysical(pixels.Y, dpi.Y));
        }

        private static Size PixelsToPhysical(Size pixels, Point dpi) {
            return new Size(PixelsToPhysical(pixels.Width, dpi.X),
                            PixelsToPhysical(pixels.Height, dpi.Y));
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.ResetBackColor"]/*' />
        /// <devdoc>
        ///     Resets the back color to the defaults for the PrintPreviewControl.
        /// </devdoc>
        [EditorBrowsable(EditorBrowsableState.Never)]
        public override void ResetBackColor() {
            BackColor = SystemColors.AppWorkspace;
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.ResetForeColor"]/*' />
        /// <devdoc>
        ///     Resets the back color to the defaults for the PrintPreviewControl.
        /// </devdoc>
        [EditorBrowsable(EditorBrowsableState.Never)]
        public override void ResetForeColor() {
            ForeColor = Color.White;
        }

        /// <devdoc>
        ///     WM_HSCROLL handler
        /// </devdoc>
        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.WmHScroll"]/*' />
        /// <internalonly/>

        private void WmHScroll(ref Message m) {

            // The lparam is handle of the sending scrollbar, or NULL when
            // the scrollbar sending the message is the "form" scrollbar...
            //
            if (m.LParam != IntPtr.Zero) {
                base.WndProc(ref m);
                return;
            }

            Point locPos = position;
            int pos = locPos.X;
            int maxPos = Math.Max(Width, virtualSize.Width /*- Width*/);

            locPos.X = AdjustScroll(m, pos, maxPos);
            Position = locPos;
        }

        private void SetPositionNoInvalidate(Point value) {
            Point current = position;

            position = value;
            position.X = Math.Min(position.X, virtualSize.Width - Width);
            position.Y = Math.Min(position.Y, virtualSize.Height - Height);
            if (position.X < 0) position.X = 0;
            if (position.Y < 0) position.Y = 0;

            Rectangle rect = ClientRectangle;
            NativeMethods.RECT scroll = NativeMethods.RECT.FromXYWH(rect.X, rect.Y, rect.Width, rect.Height);
            SafeNativeMethods.ScrollWindow(new HandleRef(this, Handle),
                                 current.X - position.X,
                                 current.Y - position.Y,
                                 ref scroll,
                                 ref scroll);

            UnsafeNativeMethods.SetScrollPos(new HandleRef(this, Handle), NativeMethods.SB_HORZ, position.X, true);
            UnsafeNativeMethods.SetScrollPos(new HandleRef(this, Handle), NativeMethods.SB_VERT, position.Y, true);
        }

        internal void SetVirtualSizeNoInvalidate(Size value) {
            virtualSize = value;
            SetPositionNoInvalidate(position); // Make sure it's within range

            NativeMethods.SCROLLINFO info = new NativeMethods.SCROLLINFO();
            info.fMask = NativeMethods.SIF_RANGE | NativeMethods.SIF_PAGE;
            info.nMin = 0;
            info.nMax = Math.Max(Height, virtualSize.Height) - 1;
            info.nPage = Height;
            UnsafeNativeMethods.SetScrollInfo(new HandleRef(this, Handle), NativeMethods.SB_VERT, info, true);

            info.fMask = NativeMethods.SIF_RANGE | NativeMethods.SIF_PAGE;
            info.nMin = 0;
            info.nMax = Math.Max(Width, virtualSize.Width) - 1;
            info.nPage = Width;
            UnsafeNativeMethods.SetScrollInfo(new HandleRef(this, Handle), NativeMethods.SB_HORZ, info, true);
        }

        /// <devdoc>
        ///     WM_VSCROLL handler
        /// </devdoc>
        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.WmVScroll"]/*' />
        /// <internalonly/>

        private void WmVScroll(ref Message m) {

            // The lparam is handle of the sending scrollbar, or NULL when
            // the scrollbar sending the message is the "form" scrollbar...
            //
            if (m.LParam != IntPtr.Zero) {
                base.WndProc(ref m);
                return;
            }

            Point locPos = Position;
            int pos = locPos.Y;
            int maxPos = Math.Max(Height, virtualSize.Height/* - Height*/);

            locPos.Y = AdjustScroll(m, pos, maxPos);
            Position = locPos;
        }

        /// <include file='doc\Control.uex' path='docs/doc[@for="Control.WmKeyChar"]/*' />
        /// <devdoc>
        ///     Handles the WM_KEYDOWN message.
        /// </devdoc>
        /// <internalonly/>
        //added to handle keyboard events(71009)subhag
        //
        private void WmKeyDown(ref Message msg) {

            Keys keyData = (Keys)((int)msg.WParam | (int)ModifierKeys);

            switch (keyData & Keys.KeyCode) {
                case Keys.PageUp:
                    StartPage--;
                    break;
                case Keys.PageDown:
                    StartPage++;
                    break;
                case Keys.Home:
                    if ((keyData & Keys.Modifiers) == Keys.Control)      
                         StartPage=0;
                    break;
                case Keys.End:
                    if ((keyData & Keys.Modifiers) == Keys.Control)
                         StartPage=pageInfo.Length;
                    break;
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.WndProc"]/*' />
        /// <internalonly/>
        [SecurityPermission(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.UnmanagedCode)]
        protected override void WndProc(ref Message m) {
            switch (m.Msg) {
                case NativeMethods.WM_VSCROLL:
                    WmVScroll(ref m);
                    break;
                case NativeMethods.WM_HSCROLL:
                    WmHScroll(ref m);
                    break;
                //added case to handle keyboard events(71009)subhag
                //
                case NativeMethods.WM_KEYDOWN:
                    WmKeyDown(ref m);
                    break;
                default:
                    base.WndProc(ref m);
                    break;
            }
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.ShouldSerializeBackColor"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Indicates whether the <see cref='System.Windows.Forms.Control.BackColor'/> property should be
        ///       persisted.
        ///    </para>
        /// </devdoc>
        internal override bool ShouldSerializeBackColor() {
            return !BackColor.Equals(SystemColors.AppWorkspace);
        }

        /// <include file='doc\PrintPreviewControl.uex' path='docs/doc[@for="PrintPreviewControl.ShouldSerializeForeColor"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Indicates whether the <see cref='System.Windows.Forms.Control.ForeColor'/> property should be
        ///       persisted.
        ///    </para>
        /// </devdoc>
        internal override bool ShouldSerializeForeColor() {
            return !ForeColor.Equals(Color.White);
        }
    }
}

