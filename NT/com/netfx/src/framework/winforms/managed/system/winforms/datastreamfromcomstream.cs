//------------------------------------------------------------------------------
// <copyright file="DataStreamFromComStream.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Windows.Forms {
    using System.Runtime.InteropServices;

    using System.Diagnostics;
    using System;
    using System.IO;
    

    /// <include file='doc\DataStreamFromComStream.uex' path='docs/doc[@for="DataStreamFromComStream"]/*' />
    /// <internalonly/>
    /// <devdoc>
    /// </devdoc>
    internal class DataStreamFromComStream : Stream {

        private UnsafeNativeMethods.IStream comStream;

        public DataStreamFromComStream(UnsafeNativeMethods.IStream comStream) : base() {
            this.comStream = comStream;
        }

        public override long Position {
            get {
                return Seek(0, SeekOrigin.Current);
            }

            set {
                Seek(value, SeekOrigin.Begin);
            }
        }

        public override bool CanWrite {
            get {
                return true;
            }
        }

        public override bool CanSeek {
            get {
                return true;
            }
        }

        public override bool CanRead {
            get {
                return true;
            }
        }

        public override long Length {
            get {
                long curPos = this.Position;
                long endPos = Seek(0, SeekOrigin.End);
                this.Position = curPos;
                return endPos - curPos;
            }
        }

        private void _NotImpl(string message) {
            NotSupportedException ex = new NotSupportedException(message, new ExternalException("", NativeMethods.E_NOTIMPL));
            throw ex;
        }

        private unsafe int _Read(void* handle, int bytes) {
            return comStream.Read((IntPtr)(int)handle, bytes);
        }

        private unsafe int _Write(void* handle, int bytes) {
            return comStream.Write((IntPtr)(int)handle, bytes);
        }

        public override void Flush() {
        }

        public unsafe override int Read(byte[] buffer, int index, int count) {
            int bytesRead = 0;
            if (count > 0 && index >= 0 && (count + index) <= buffer.Length) {
                fixed (byte* ch = buffer) {
                    bytesRead = _Read((void*)(ch + index), count); 
                }
            }
            return bytesRead;
        }

        public override void SetLength(long value) {
            comStream.SetSize(value);
        }

        public override long Seek(long offset, SeekOrigin origin) {
            return comStream.Seek(offset, (int)origin);
        }

        public unsafe override void Write(byte[] buffer, int index, int count) {
            int bytesWritten = 0;
            if (count > 0 && index >= 0 && (count + index) <= buffer.Length) {
                try {
                    fixed (byte* b = buffer) {
                        bytesWritten = _Write((void*)(b + index), count);
                    }
                }
                catch {
                }
            }
            if (bytesWritten < count) {
                throw new IOException(SR.GetString(SR.DataStreamWrite));
            }
        }

        public override void Close() {
            if (comStream != null) {
                try {
                    comStream.Commit(NativeMethods.STGC_DEFAULT);
                }
                catch(Exception) {
                }
                comStream = null;
            }
        }

        ~DataStreamFromComStream() {
            comStream = null;
        }
    }
}
