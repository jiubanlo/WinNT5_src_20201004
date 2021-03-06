//------------------------------------------------------------------------------
// <copyright file="ImageCodecInfo.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/**************************************************************************\
*
* Copyright (c) 1998-1999, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   ImageCodecInfo.cs
*
* Abstract:
*
*   Native GDI+ ImageCodecInfo structure.
*
* Revision History:
*
*   9/22/1999 ericvan
*       Created it.
*
\**************************************************************************/

namespace System.Drawing.Imaging {
    using System.Text;
    using System.Runtime.InteropServices;
    using System.Diagnostics;
    using System;    
    using System.Drawing;
    using System.Drawing.Internal;

    // sdkinc\imaging.h
    /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    [
    System.Runtime.InteropServices.ComVisible(false)
    ]
    public sealed class ImageCodecInfo {
         Guid clsid;
         Guid formatID;
         string codecName;
         string dllName;
         string formatDescription;
         string filenameExtension;
         string mimeType;
         ImageCodecFlags flags;
         int version;
         byte[][] signaturePatterns;
         byte[][] signatureMasks;

         internal ImageCodecInfo() {
         }
         
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.Clsid"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public Guid Clsid {
             get { return clsid; }
             set { clsid = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.FormatID"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public Guid FormatID {
             get { return formatID; }
             set { formatID = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.CodecName"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public string CodecName {
             get { return codecName; }
             set { codecName = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.DllName"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public string DllName {
             get { return dllName; }
             set { dllName = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.FormatDescription"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public string FormatDescription {
             get { return formatDescription; }
             set { formatDescription = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.FilenameExtension"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public string FilenameExtension {
             get { return filenameExtension; }
             set { filenameExtension = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.MimeType"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public string MimeType {
             get { return mimeType; }
             set { mimeType = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.Flags"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public ImageCodecFlags Flags {
             get { return flags; }
             set { flags = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.Version"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public int Version {
             get { return version; }
             set { version = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.SignaturePatterns"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         [CLSCompliant(false)]
         public byte[][] SignaturePatterns {
             get { return signaturePatterns; }
             set { signaturePatterns = value; }
         }
         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.SignatureMasks"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         [CLSCompliant(false)]
         public byte[][] SignatureMasks {
             get { return signatureMasks; }
             set { signatureMasks = value; }
         }
         
         // Encoder/Decoder selection APIs

         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.GetImageDecoders"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public static ImageCodecInfo[] GetImageDecoders() {
             int numDecoders;
             int size;

             int status = SafeNativeMethods.GdipGetImageDecodersSize(out numDecoders, out size);

             if (status != SafeNativeMethods.Ok)
                 throw SafeNativeMethods.StatusException(status);

             IntPtr memory = Marshal.AllocHGlobal(size);

             status = SafeNativeMethods.GdipGetImageDecoders(numDecoders, size, memory);

             if (status != SafeNativeMethods.Ok) {
                 Marshal.FreeHGlobal(memory);
                 throw SafeNativeMethods.StatusException(status);
             }

             ImageCodecInfo[] imageCodecs = ImageCodecInfo.ConvertFromMemory(memory, numDecoders);

             Marshal.FreeHGlobal(memory);
                          
             return imageCodecs;
         }

         /// <include file='doc\ImageCodecInfo.uex' path='docs/doc[@for="ImageCodecInfo.GetImageEncoders"]/*' />
         /// <devdoc>
         ///    <para>[To be supplied.]</para>
         /// </devdoc>
         public static ImageCodecInfo[] GetImageEncoders() {
             int numEncoders;
             int size;

             int status = SafeNativeMethods.GdipGetImageEncodersSize(out numEncoders, out size);

             if (status != SafeNativeMethods.Ok)
                 throw SafeNativeMethods.StatusException(status);

             IntPtr memory = Marshal.AllocHGlobal(size);

             status = SafeNativeMethods.GdipGetImageEncoders(numEncoders, size, memory);

             if (status != SafeNativeMethods.Ok) {
                 Marshal.FreeHGlobal(memory);
                 throw SafeNativeMethods.StatusException(status);
             }

             ImageCodecInfo[] imageCodecs = ImageCodecInfo.ConvertFromMemory(memory, numEncoders);

             Marshal.FreeHGlobal(memory);

             return imageCodecs;
         }
         
         internal static ImageCodecInfoPrivate ConvertToMemory(ImageCodecInfo imagecs)
         {
             ImageCodecInfoPrivate imagecsp = new ImageCodecInfoPrivate();
             
             imagecsp.Clsid = imagecs.Clsid;
             imagecsp.FormatID = imagecs.FormatID;
             
             imagecsp.CodecName = Marshal.StringToHGlobalUni(imagecs.CodecName);
             imagecsp.DllName = Marshal.StringToHGlobalUni(imagecs.DllName);
             imagecsp.FormatDescription = Marshal.StringToHGlobalUni(imagecs.FormatDescription);
             imagecsp.FilenameExtension = Marshal.StringToHGlobalUni(imagecs.FilenameExtension);
             imagecsp.MimeType = Marshal.StringToHGlobalUni(imagecs.MimeType);
             
             imagecsp.Flags = (int)imagecs.Flags;
             imagecsp.Version = (int)imagecs.Version;
             imagecsp.SigCount = imagecs.SignaturePatterns.Length;
             imagecsp.SigSize = imagecs.SignaturePatterns[0].Length;
             
             imagecsp.SigPattern = Marshal.AllocHGlobal(imagecsp.SigCount*imagecsp.SigSize);
             imagecsp.SigMask = Marshal.AllocHGlobal(imagecsp.SigCount*imagecsp.SigSize);
             
             for (int i=0; i<imagecsp.SigCount; i++)
             {
                 Marshal.Copy(imagecs.SignaturePatterns[i], 
                              0, 
                              (IntPtr)((long)imagecsp.SigPattern + i*imagecsp.SigSize), 
                              imagecsp.SigSize);
                                  
                 Marshal.Copy(imagecs.SignatureMasks[i], 
                              0, 
                              (IntPtr)((long)imagecsp.SigMask + i*imagecsp.SigSize), 
                              imagecsp.SigSize);
             }
                          
             return imagecsp;
         }

         internal static void FreeMemory(ImageCodecInfoPrivate imagecodecp)
         {
             Marshal.FreeHGlobal(imagecodecp.CodecName);
             Marshal.FreeHGlobal(imagecodecp.FormatDescription);
             Marshal.FreeHGlobal(imagecodecp.FilenameExtension);
             Marshal.FreeHGlobal(imagecodecp.MimeType);
             Marshal.FreeHGlobal(imagecodecp.SigPattern);
             Marshal.FreeHGlobal(imagecodecp.SigMask);
         }
                          
         internal static ImageCodecInfo[] ConvertFromMemory(IntPtr memoryStart, int numCodecs)
         {
             ImageCodecInfo[] codecs = new ImageCodecInfo[numCodecs];

             int index;

             for (index=0; index<numCodecs; index++)
             {
                 IntPtr curcodec = (IntPtr)((long)memoryStart + (int)Marshal.SizeOf(typeof(ImageCodecInfoPrivate))*index);
                 ImageCodecInfoPrivate codecp = new ImageCodecInfoPrivate();
                 UnsafeNativeMethods.PtrToStructure(curcodec, codecp);
                 
                 codecs[index] = new ImageCodecInfo();
                 codecs[index].Clsid = codecp.Clsid;
                 codecs[index].FormatID = codecp.FormatID;
                 codecs[index].CodecName = Marshal.PtrToStringUni(codecp.CodecName);
                 codecs[index].DllName = Marshal.PtrToStringUni(codecp.DllName);
                 codecs[index].FormatDescription = Marshal.PtrToStringUni(codecp.FormatDescription);
                 codecs[index].FilenameExtension = Marshal.PtrToStringUni(codecp.FilenameExtension);
                 codecs[index].MimeType = Marshal.PtrToStringUni(codecp.MimeType);
                 
                 codecs[index].Flags = (ImageCodecFlags)codecp.Flags;
                 codecs[index].Version = (int)codecp.Version;
                                  
                 codecs[index].SignaturePatterns = new byte[codecp.SigCount][];
                 codecs[index].SignatureMasks = new byte[codecp.SigCount][];
                 
                 for (int j=0; j<codecp.SigCount; j++)
                 {
                     codecs[index].SignaturePatterns[j] = new byte[codecp.SigSize];
                     codecs[index].SignatureMasks[j] = new byte[codecp.SigSize];

                     Marshal.Copy((IntPtr)((long)codecp.SigMask + j*codecp.SigSize), codecs[index].SignatureMasks[j], 0, codecp.SigSize);
                     Marshal.Copy((IntPtr)((long)codecp.SigPattern + j*codecp.SigSize), codecs[index].SignaturePatterns[j], 0, codecp.SigSize);
                 }
             }

             return codecs;
         }

    }

}
