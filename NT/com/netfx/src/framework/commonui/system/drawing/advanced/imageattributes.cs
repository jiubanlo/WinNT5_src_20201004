//------------------------------------------------------------------------------
// <copyright file="ImageAttributes.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   ImageAttributes.cs
*
* Abstract:
*
*   Native GDI+ ImageAttributes structure.
*
* Revision History:
*
*   9/22/1999 ericvan
*       Created it.
*
\**************************************************************************/

namespace System.Drawing.Imaging {
    using System.Runtime.InteropServices;
    using System.Diagnostics;
    using System;
    using System.Drawing;
    using System.ComponentModel;
    using Microsoft.Win32;    
    using System.Drawing.Drawing2D;
    using System.Drawing.Internal;

    // sdkinc\GDIplusImageAttributes.h

    // There are 5 possible sets of color adjustments:
    //          ColorAdjustDefault,
    //          ColorAdjustBitmap,
    //          ColorAdjustBrush,
    //          ColorAdjustPen,
    //          ColorAdjustText,

    // Bitmaps, Brushes, Pens, and Text will all use any color adjustments
    // that have been set into the default ImageAttributes until their own
    // color adjustments have been set.  So as soon as any "Set" method is
    // called for Bitmaps, Brushes, Pens, or Text, then they start from
    // scratch with only the color adjustments that have been set for them.
    // Calling Reset removes any individual color adjustments for a type
    // and makes it revert back to using all the default color adjustments
    // (if any).  The SetToIdentity method is a way to force a type to
    // have no color adjustments at all, regardless of what previous adjustments
    // have been set for the defaults or for that type.

    /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes"]/*' />
    /// <devdoc>
    ///    Contains information about how image colors
    ///    are manipulated during rendering.
    /// </devdoc>
    [StructLayout(LayoutKind.Sequential)]
    public sealed class ImageAttributes : ICloneable, IDisposable {
#if FINALIZATION_WATCH
        private string allocationSite = Graphics.GetAllocationStack();
#endif                                                         


        /*
         * Handle to native image attributes object
         */

        internal IntPtr nativeImageAttributes;

        internal void SetNativeImageAttributes(IntPtr handle) {
            if (handle == IntPtr.Zero)
                    throw new ArgumentNullException("handle");
    
            nativeImageAttributes = handle;
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ImageAttributes"]/*' />
        /// <devdoc>
        ///    Initializes a new instance of the <see cref='System.Drawing.Imaging.ImageAttributes'/> class.
        /// </devdoc>
        public ImageAttributes()
            {
                IntPtr newImageAttributes = IntPtr.Zero;

                int status = SafeNativeMethods.GdipCreateImageAttributes(out newImageAttributes);

                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);

                SetNativeImageAttributes(newImageAttributes);
            }

        internal ImageAttributes(IntPtr newNativeImageAttributes)
            {
                SetNativeImageAttributes(newNativeImageAttributes);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.Dispose"]/*' />
        /// <devdoc>
        ///    Cleans up Windows resources for this
        /// <see cref='System.Drawing.Imaging.ImageAttributes'/>.
        /// </devdoc>
        public void Dispose() {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing) {
#if FINALIZATION_WATCH
            if (!disposing && nativeImageAttributes != IntPtr.Zero)
                Debug.WriteLine("**********************\nDisposed through finalization:\n" + allocationSite);
#endif
            if (nativeImageAttributes != IntPtr.Zero) {
                int status = SafeNativeMethods.GdipDisposeImageAttributes(new HandleRef(this, nativeImageAttributes));
                nativeImageAttributes = IntPtr.Zero;
                if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
            }
       }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.Finalize"]/*' />
        /// <devdoc>
        ///    Cleans up Windows resources for this
        /// <see cref='System.Drawing.Imaging.ImageAttributes'/>.
        /// </devdoc>
        ~ImageAttributes() {
            Dispose(false);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.Clone"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Creates an exact copy of this <see cref='System.Drawing.Imaging.ImageAttributes'/>.
        ///    </para>
        /// </devdoc>
        public object Clone() {
            IntPtr clone = IntPtr.Zero;

            int status = SafeNativeMethods.GdipCloneImageAttributes(
                                    new HandleRef(this, nativeImageAttributes),
                                    out clone);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);

            return new ImageAttributes(clone);
        }

        void SetToIdentity()
        {
            SetToIdentity(ColorAdjustType.Default);
        }

        void SetToIdentity(ColorAdjustType type) 
        {
            int status = SafeNativeMethods.GdipSetImageAttributesToIdentity(new HandleRef(this, nativeImageAttributes), type);

                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);
        }

        void Reset()
        {
            Reset(ColorAdjustType.Default);
        }

        void Reset(ColorAdjustType type)
        {
            int status = SafeNativeMethods.GdipResetImageAttributes(new HandleRef(this, nativeImageAttributes), type);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorMatrix"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Sets the 5 X 5 color adjust matrix to the
        ///       specified <see cref='System.Drawing.Drawing2D.Matrix'/>.
        ///    </para>
        /// </devdoc>
        public void SetColorMatrix(ColorMatrix newColorMatrix)
        {
            SetColorMatrix(newColorMatrix, ColorMatrixFlag.Default, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorMatrix1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Sets the 5 X 5 color adjust matrix to the specified 'Matrix' with the specified 'ColorMatrixFlags'.
        ///    </para>
        /// </devdoc>
        public void SetColorMatrix(ColorMatrix newColorMatrix, ColorMatrixFlag flags)
        {
            SetColorMatrix(newColorMatrix, flags, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorMatrix2"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Sets the 5 X 5 color adjust matrix to the specified 'Matrix' with the 
        ///       specified 'ColorMatrixFlags'.
        ///    </para>
        /// </devdoc>
        public void SetColorMatrix(ColorMatrix newColorMatrix, ColorMatrixFlag mode, ColorAdjustType type)
        {
            int status = SafeNativeMethods.GdipSetImageAttributesColorMatrix(
                        new HandleRef(this, nativeImageAttributes),
                        type,
                        true,
                        newColorMatrix,
                        null,
                        mode);

                if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
            }   

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearColorMatrix"]/*' />
        /// <devdoc>
        ///    Clears the color adjust matrix to all
        ///    zeroes.
        /// </devdoc>
        public void ClearColorMatrix()
        {
            ClearColorMatrix(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearColorMatrix1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Clears the color adjust matrix.
        ///    </para>
        /// </devdoc>
        public void ClearColorMatrix(ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesColorMatrix(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    false,
                    null,
                    null,
                    ColorMatrixFlag.Default);
        
                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorMatrices"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Sets a color adjust matrix for image colors
        ///       and a separate gray scale adjust matrix for gray scale values.
        ///    </para>
        /// </devdoc>
        public void SetColorMatrices(ColorMatrix newColorMatrix, ColorMatrix grayMatrix)
        {
            SetColorMatrices(newColorMatrix, grayMatrix, ColorMatrixFlag.Default, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorMatrices1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetColorMatrices(ColorMatrix newColorMatrix, ColorMatrix grayMatrix, ColorMatrixFlag flags)
        {
            SetColorMatrices(newColorMatrix, grayMatrix, flags, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorMatrices2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetColorMatrices(ColorMatrix newColorMatrix, ColorMatrix grayMatrix, ColorMatrixFlag mode, 
                                     ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesColorMatrix(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    true,
                    newColorMatrix,
                    grayMatrix,
                    mode);

                if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
            }   

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetThreshold"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetThreshold(float threshold)
            {
            SetThreshold(threshold, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetThreshold1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetThreshold(float threshold, ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesThreshold(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    true,
                    threshold);
        
                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearThreshold"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearThreshold()
            {
            ClearThreshold(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearThreshold1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearThreshold(ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesThreshold(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    false,
                    0.0f);

                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetGamma"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetGamma(float gamma)
            {
            SetGamma(gamma, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetGamma1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetGamma(float gamma, ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesGamma(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    true,
                    gamma);

                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearGamma"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearGamma()
            {
            ClearGamma(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearGamma1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearGamma(ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesGamma(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    false,
                    0.0f);

                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetNoOp"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetNoOp()
        {
            SetNoOp(ColorAdjustType.Default);   
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetNoOp1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetNoOp(ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesNoOp(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    true);

                if (status != SafeNativeMethods.Ok)
                        throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearNoOp"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearNoOp()
            {
            ClearNoOp(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearNoOp1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearNoOp(ColorAdjustType type)
            {
                int status = SafeNativeMethods.GdipSetImageAttributesNoOp(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    false);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetColorKey(Color colorLow, Color colorHigh)
        {
            SetColorKey(colorLow, colorHigh, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetColorKey1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetColorKey(Color colorLow, Color colorHigh, ColorAdjustType type)
        {
            int lowInt = colorLow.ToArgb();
            int highInt = colorHigh.ToArgb();

            int status = SafeNativeMethods.GdipSetImageAttributesColorKeys(
                                        new HandleRef(this, nativeImageAttributes),
                                        type,
                                        true,
                                        lowInt,
                                        highInt);

            if (status != SafeNativeMethods.Ok)
                throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearColorKey"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearColorKey()
            {
            ClearColorKey(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearColorKey1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearColorKey(ColorAdjustType type)
            {
            int zero = 0;
            int status = SafeNativeMethods.GdipSetImageAttributesColorKeys(
                                        new HandleRef(this, nativeImageAttributes),
                                        type,
                                        false,
                                        zero,
                                        zero);

           if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetOutputChannel"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetOutputChannel(ColorChannelFlag flags)
        {
            SetOutputChannel(flags, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetOutputChannel1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetOutputChannel(ColorChannelFlag flags, ColorAdjustType type)
        {
                int status = SafeNativeMethods.GdipSetImageAttributesOutputChannel(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    true,
                    flags);
                            
            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearOutputChannel"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearOutputChannel()
        {
            ClearOutputChannel(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearOutputChannel1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearOutputChannel(ColorAdjustType type)
        {
                int status = SafeNativeMethods.GdipSetImageAttributesOutputChannel(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    false,
                    ColorChannelFlag.ColorChannelLast);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
        }
        
        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetOutputChannelColorProfile"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetOutputChannelColorProfile(String colorProfileFilename)
        {
            SetOutputChannelColorProfile(colorProfileFilename, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetOutputChannelColorProfile1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetOutputChannelColorProfile(String colorProfileFilename,
                                                 ColorAdjustType type)
        {
            IntSecurity.DemandReadFileIO(colorProfileFilename);

            int status = SafeNativeMethods.GdipSetImageAttributesOutputChannelColorProfile(
                                        new HandleRef(this, nativeImageAttributes),
                                        type,
                                        true,
                                        colorProfileFilename);
                            
            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearOutputChannelColorProfile"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearOutputChannelColorProfile()
        {
            ClearOutputChannel(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearOutputChannelColorProfile1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearOutputChannelColorProfile(ColorAdjustType type)
        {
                int status = SafeNativeMethods.GdipSetImageAttributesOutputChannel(
                    new HandleRef(this, nativeImageAttributes),
                    type,
                    false,
                    ColorChannelFlag.ColorChannelLast);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetRemapTable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetRemapTable(ColorMap[] map)
        {
            SetRemapTable(map, ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetRemapTable1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetRemapTable(ColorMap[] map, ColorAdjustType type)
        {
            int index = 0;
            int mapSize = map.Length;
            int size = 4; // Marshal.SizeOf(index.GetType());
            IntPtr memory = Marshal.AllocHGlobal(mapSize*size*2);

            for (index=0; index<mapSize; index++) {
                Marshal.StructureToPtr(map[index].OldColor.ToArgb(), (IntPtr)((long)memory+index*size*2), false);
                Marshal.StructureToPtr(map[index].NewColor.ToArgb(), (IntPtr)((long)memory+index*size*2+size), false);
            }

            int status = SafeNativeMethods.GdipSetImageAttributesRemapTable(
                new HandleRef(this, nativeImageAttributes),
                type,
                true,
                mapSize,
                new HandleRef(null, memory));

            Marshal.FreeHGlobal(memory);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
            }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearRemapTable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearRemapTable()
        {
            ClearRemapTable(ColorAdjustType.Default);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearRemapTable1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearRemapTable(ColorAdjustType type)
        {
                int status = SafeNativeMethods.GdipSetImageAttributesRemapTable(
                                new HandleRef(this, nativeImageAttributes),
                                type,
                                false,
                                0,
                                NativeMethods.NullHandleRef);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetBrushRemapTable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetBrushRemapTable(ColorMap[] map)
        {
            SetRemapTable(map, ColorAdjustType.Brush);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.ClearBrushRemapTable"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void ClearBrushRemapTable()
        {
            ClearRemapTable(ColorAdjustType.Brush);
        }

        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetWrapMode"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetWrapMode(WrapMode mode)
        {
            SetWrapMode(mode, new Color(), false);
        }
        
        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetWrapMode1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetWrapMode(WrapMode mode, Color color)
        {
            SetWrapMode(mode, color, false);
        }
        
        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.SetWrapMode2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void SetWrapMode(WrapMode mode, Color color, bool clamp)
        {
            int status = SafeNativeMethods.GdipSetImageAttributesWrapMode(
                            new HandleRef(this, nativeImageAttributes),
                            (int)mode,
                            color.ToArgb(),
                            clamp);

            if (status != SafeNativeMethods.Ok)
                    throw SafeNativeMethods.StatusException(status);
        }
        
        /// <include file='doc\ImageAttributes.uex' path='docs/doc[@for="ImageAttributes.GetAdjustedPalette"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public void GetAdjustedPalette(ColorPalette palette, ColorAdjustType type)
        {
            // does inplace adjustment
            IntPtr memory = palette.ConvertToMemory();
            
            int status = SafeNativeMethods.GdipGetImageAttributesAdjustedPalette(
                                new HandleRef(this, nativeImageAttributes), new HandleRef(null, memory), type);
                                            
            if (status != SafeNativeMethods.Ok)
            {
                Marshal.FreeHGlobal(memory);
                throw SafeNativeMethods.StatusException(status);
            }
            
            palette.ConvertFromMemory(memory);
            Marshal.FreeHGlobal(memory);
        }
        
    }
}
