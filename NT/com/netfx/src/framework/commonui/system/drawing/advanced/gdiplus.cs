//------------------------------------------------------------------------------
// <copyright file="Gdiplus.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   gdiplus.cs
*
* Abstract:
*
*   Wrapper for flat GDI+ APIs exported by gdiplus.dll
*
* Revision History:
*
*   12/14/1998 davidx
*       Created it.
*
\**************************************************************************/

namespace System.Drawing {
    using System.Text;
    using System.Collections;
    using System.Runtime.InteropServices;
    using System.ComponentModel;
    using System.Diagnostics;
    using System;    
    using System.IO;
    using Microsoft.Win32;
    using System.Drawing;
    using System.Drawing.Internal;
    using System.Drawing.Imaging;
    using System.Drawing.Text;
    using System.Drawing.Drawing2D;
    using System.Threading;
    using System.Security.Permissions;
    using System.Security;

    [System.Runtime.InteropServices.ComVisible(false)]
    [System.Security.SuppressUnmanagedCodeSecurityAttribute()]
    internal class SafeNativeMethods {
        private static readonly TraceSwitch GdiPlusInitialization = new TraceSwitch("GdiPlusInitialization", "Tracks GDI+ initialization and teardown");
        private static readonly BooleanSwitch GdiPlusIgnoreAtom = new BooleanSwitch("GdiPlusIgnoreAtom", "Ignores the use of global atoms for startup/shutdown");

        private static bool isShutdown = true; //true if GDI+ isn't initialized or has been shutdown
        private static IntPtr initToken;
        private static IntPtr hookToken;
        private const string ThreadDataSlotName = "system.drawing.threaddata";
        private static string atomName = null;
        private static IntPtr hAtom = IntPtr.Zero;

        static SafeNativeMethods() {
            Initialize();
        }

        /// <devdoc>
        ///      Determines if GDI+ is already initialized, we do this by checking
        ///      a global atom that we create - once per process.
        /// </devdoc>
        static bool AlreadyInitialized {
            get {
                if (atomName == null) {
                    atomName = "GDI+Atom" + GetCurrentProcessId().ToString();
                }
                if (FindAtom(atomName) != IntPtr.Zero) {
                    return true;
                }
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Creating our global atom: " + atomName);
                hAtom = AddAtom(atomName);
                return false;  
            }
        }
        
        /// <devdoc>
        ///      Returns true if we've shutdown GDI+
        /// </devdoc>
        private static bool IsShutdown {
            get {
                return isShutdown;
            }
            set {
                isShutdown = value;
            }
        }

        /// <devdoc>
        ///      This property will give us back a hashtable we can use to store
        ///      all of our static brushes and pens on a per-thread basis.  This way   
        ///      we can avoid 'object in use' crashes when differnt threads are
        ///      referencing the same drawing object.
        /// </devdoc>
        internal static IDictionary ThreadData {
            get {
                LocalDataStoreSlot slot = Thread.GetNamedDataSlot(ThreadDataSlotName);
                IDictionary threadData = (IDictionary)Thread.GetData(slot);
                if (threadData == null) {
                    threadData = new Hashtable();
                    Thread.SetData(slot, threadData);
                }
                return threadData;
            }
        }
        
        /// <devdoc>
        ///      If we've created a valid atom when we started gdi+
        ///      we'll tear it down here.
        /// </devdoc>
        static void DestroyAtom() {
            if (hAtom != IntPtr.Zero) {
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Deleting our global atom");
                DeleteAtom(new HandleRef(null, hAtom));
            }
        }

        /// <devdoc>
        ///      Initializes GDI+
        /// </devdoc>
        static void Initialize() {
            bool isUserInteractive = Environment.UserInteractive;
            bool alreadyInitialized;

            // We only want to initialize GDI+ once per process
            // so we'll bail out if we can find our atom.
            // 
#if DEBUG 
            alreadyInitialized = (!GdiPlusIgnoreAtom.Enabled && AlreadyInitialized);
#else
            alreadyInitialized = AlreadyInitialized;
#endif

            if (!alreadyInitialized) {
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Initialize GDI+ [" + AppDomain.CurrentDomain.FriendlyName + "]");
                Debug.Indent();

                // We'll try to load the gdiplus.dll from the framework's install directory.
                // Since we weak bind - on a down level system (something < WindowsXP) 
                // the dll will get loaded here.  On a WindowsXP OS, this will fail and fusion
                // will load the gdiplus.dll from the System32 directory.
                //
                IntPtr dllModule;
                string version = null;
                int result = LoadLibraryShim("Gdiplus.dll", version, (IntPtr)0, out dllModule);

                StartupInput input = StartupInput.GetDefault();
                StartupOutput output;

                if (isUserInteractive) {
                    input.SuppressBackgroundThread = true;
                    Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Process is user interactive, suppress GDI+ background thread...");
                }

                int status = GdiplusStartup(out initToken, ref input, out output);

                if (status != SafeNativeMethods.Ok) {
                    throw SafeNativeMethods.StatusException(status);
                }

                // NOTE:  The secondary check here is to prevent system events thread from starting up 
                // under asp.net.  If this thread starts up, it kills their performance because
                // there is one of these threads per app-domain.
                if (isUserInteractive &&  Thread.GetDomain().GetData(".appDomain") == null) {
                    Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Process is user interactive, and not running inside ASP.NET...");
                    SystemEvents.InvokeOnEventsThread(new EventHandler(SystemEventThreadCallback));
                }
                
                Debug.Unindent();
            }
            else {
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "GDI+ was already initialized, so we won't attempt to do it again.");
            }

            IsShutdown = false;
            AppDomain.CurrentDomain.ProcessExit += new EventHandler(SafeNativeMethods.OnProcessExit);
            
            // NOTE:  The secondary check here is to prevent system events thread from starting up 
            // under asp.net.  If this thread starts up, it kills their performance because
            // there is one of these threads per app-domain.
            if (isUserInteractive &&  Thread.GetDomain().GetData(".appDomain") == null) {
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Process is user interactive, and not running inside ASP.NET...");
                SystemEvents.EventsThreadShutdown += new EventHandler(OnSystemEventThreadShutdown);
            }
        }

        /// <devdoc>
        ///      Shutsdown GDI+
        /// </devdoc>
        private static void Shutdown() {
            Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Shutdown GDI+ [" + AppDomain.CurrentDomain.FriendlyName + "]");
            Debug.Indent();

#if DEBUG
            if (!GdiPlusIgnoreAtom.Enabled) {
                DestroyAtom();
            }
#else
            DestroyAtom();
#endif
            
            if (!IsShutdown) {
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Not already shutdown");

                // Nuke thread data
                //
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Releasing TLS data");
                LocalDataStoreSlot slot = Thread.GetNamedDataSlot(ThreadDataSlotName);
                Thread.SetData(slot, null);

                // Let any thread data collect and finalize before
                // we tear down GDI+
                //
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Running garbage collector");
                GC.Collect();
                GC.WaitForPendingFinalizers();

                // Shutdown GDI+
                //
                Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Instruct GDI+ to shutdown");

                if (initToken != IntPtr.Zero) {
                    GdiplusShutdown(new HandleRef(null, initToken));
                }

                // Mark the AppDomain as having GDI+ unloaded
                //
                IsShutdown = true;
            }
            Debug.Unindent();
        }

        // Called on the SystemEventThread so that GDI+ will use this
        // thread to listen to system events.
        //
        private static void SystemEventThreadCallback(object sender, EventArgs e) {
            int status = GdiplusNotificationHook(out hookToken);
        }

        // Called on the SystemEventThread so that GDI+ will stop
        // listening to system events on this thread.
        //
        private static void OnSystemEventThreadShutdown(object sender, EventArgs e) {
            if (hookToken != IntPtr.Zero) {
                GdiplusNotificationUnhook(new HandleRef(null, hookToken));
            }
        }

        // When we get notification that the process is terminating, we will
        // try to shutdown GDI+ if we haven't already.
        //
        private static void OnProcessExit(object sender, EventArgs e) {
            Debug.WriteLineIf(GdiPlusInitialization.TraceVerbose, "Process exited");
            Shutdown();
        }


        //-------------------------------------------------------------------------------------------
        // Global atom APIs - used to determine when to startup/shutdown GDI+ on a per-process basis
        //-------------------------------------------------------------------------------------------

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        private static extern int GetCurrentProcessId();

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        private static extern IntPtr AddAtom(string lpString);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        private static extern IntPtr DeleteAtom(HandleRef hAtom);

        [DllImport(ExternDll.Kernel32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        private static extern IntPtr FindAtom(string lpString);


        //----------------------------------------------------------------------------------------                                                           
        // LoadLibrary method - used for weak binding to the gdiplus.dll on a down-level system
        //----------------------------------------------------------------------------------------
        
        [DllImport(ExternDll.Mscoree, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int LoadLibraryShim(string dllName, string version, IntPtr reserved, out IntPtr dllModule);


        //----------------------------------------------------------------------------------------                                                           
        // Initialization methods (GdiplusInit.h)
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int GdiplusNotificationHook(out IntPtr token);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern void GdiplusNotificationUnhook(HandleRef token);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int GdiplusStartup(out IntPtr token, ref StartupInput input, out StartupOutput output);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern void GdiplusShutdown(HandleRef token);

        [StructLayout(LayoutKind.Sequential)]
        private struct StartupInput {
            public int GdiplusVersion;             // Must be 1

            // public DebugEventProc DebugEventCallback; // Ignored on free builds
            public IntPtr DebugEventCallback;

            public bool SuppressBackgroundThread;     // FALSE unless you're prepared to call 
            // the hook/unhook functions properly

            public bool SuppressExternalCodecs;       // FALSE unless you want GDI+ only to use
            // its internal image codecs.

            public static StartupInput GetDefault() {
                StartupInput result = new StartupInput();
                result.GdiplusVersion = 1;
                // result.DebugEventCallback = null;
                result.SuppressBackgroundThread = false;
                result.SuppressExternalCodecs  = false;
                return result;
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct StartupOutput
        {
            // The following 2 fields won't be used.  They were originally intended 
            // for getting GDI+ to run on our thread - however there are marshalling
            // dealing with function *'s and what not - so we make explicit calls
            // to gdi+ after the fact, via the GdiplusNotificationHook and 
            // GdiplusNotificationUnhook methods.
            public IntPtr hook;//not used
            public IntPtr unhook;//not used.
        }

        private enum DebugEventLevel {
            Fatal,
            Warning,
        }


        // private delegate void DebugEventProc(DebugEventLevel level, /* char* */ string message);

        // returns GdiplusStatus
        private delegate int NotificationHookProc(out IntPtr token);
        private delegate void NotificationUnhookProc(IntPtr token);

        //----------------------------------------------------------------------------------------                                                           
        // Path methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePath(int brushMode, out IntPtr path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePath2(HandleRef points, HandleRef types, int count, int brushMode, out IntPtr path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePath2I(HandleRef points, HandleRef types, int count, int brushMode, out IntPtr path);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipClonePath(HandleRef path, out IntPtr clonepath);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeletePath", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeletePath(HandleRef path);
        internal static int GdipDeletePath(HandleRef path) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeletePath(path);
            return result;
        }


        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetPath(HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPointCount(HandleRef path, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathTypes(HandleRef path, byte[] types, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathPoints(HandleRef path, HandleRef points, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathFillMode(HandleRef path, out int fillmode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathFillMode(HandleRef path, int fillmode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathData(HandleRef path, IntPtr pathData);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipStartPathFigure(HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipClosePathFigure(HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipClosePathFigures(HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathMarker(HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipClearPathMarkers(HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipReversePath(HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathLastPoint(HandleRef path, GPPOINTF lastPoint);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathLine(HandleRef path, float x1, float y1, float x2,
                                                   float y2);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathLine2(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathArc(HandleRef path, float x, float y, float width,
                                                  float height, float startAngle,
                                                  float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathBezier(HandleRef path, float x1, float y1, float x2,
                                                     float y2, float x3, float y3, float x4,
                                                     float y4);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathBeziers(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathCurve(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathCurve2(HandleRef path, HandleRef memorypts, int count,
                                                     float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathCurve3(HandleRef path, HandleRef memorypts, int count,
                                                     int offset, int numberOfSegments,
                                                     float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathClosedCurve(HandleRef path, HandleRef memorypts,
                                                          int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathClosedCurve2(HandleRef path, HandleRef memorypts,
                                                           int count, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathRectangle(HandleRef path, float x, float y, float width,
                                                        float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathRectangles(HandleRef path, HandleRef rects, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathEllipse(HandleRef path, float x, float y,
                                                      float width, float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathPie(HandleRef path, float x, float y, float width,
                                                  float height, float startAngle,
                                                  float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathPolygon(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathPath(HandleRef path, HandleRef addingPath, bool connect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathString(HandleRef path, string s, int length,
                                                     HandleRef fontFamily, int style, float emSize,
                                                     ref GPRECTF layoutRect, HandleRef format);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathStringI(HandleRef path, string s, int length,
                                                      HandleRef fontFamily, int style, float emSize,
                                                      ref GPRECT layoutRect, HandleRef format);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathLineI(HandleRef path, int x1, int y1, int x2,
                                                    int y2);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathLine2I(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathArcI(HandleRef path, int x, int y, int width,
                                                   int height, float startAngle,
                                                   float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathBezierI(HandleRef path, int x1, int y1, int x2,
                                                      int y2, int x3, int y3, int x4,
                                                      int y4);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathBeziersI(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathCurveI(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathCurve2I(HandleRef path, HandleRef memorypts, int count,
                                                      float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathCurve3I(HandleRef path, HandleRef memorypts, int count,
                                                      int offset, int numberOfSegments,
                                                      float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathClosedCurveI(HandleRef path, HandleRef memorypts,
                                                           int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathClosedCurve2I(HandleRef path, HandleRef memorypts,
                                                            int count, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathRectangleI(HandleRef path, int x, int y, int width,
                                                         int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathRectanglesI(HandleRef path, HandleRef rects, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathEllipseI(HandleRef path, int x, int y,
                                                       int width, int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathPieI(HandleRef path, int x, int y, int width,
                                                   int height, float startAngle,
                                                   float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipAddPathPolygonI(HandleRef path, HandleRef memorypts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFlattenPath(HandleRef path, HandleRef matrixfloat, float flatness);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipWidenPath(HandleRef path, HandleRef pen, HandleRef matrix, float flatness);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipWindingModeOutline(HandleRef path, HandleRef matrix, float flatness);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipWarpPath(HandleRef path, HandleRef matrix, HandleRef points, int count,
                                                float srcX, float srcY, float srcWidth, float srcHeight,
                                                WarpMode warpMode, float flatness);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTransformPath(HandleRef path, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathWorldBounds(HandleRef path, ref GPRECTF gprectf, HandleRef matrix, HandleRef pen);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathWorldBoundsI(HandleRef path, ref GPRECT gprect, HandleRef matrix, HandleRef pen);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisiblePathPoint(HandleRef path, float x, float y,
                                                          HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisiblePathPointI(HandleRef path, int x, int y,
                                                           HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsOutlineVisiblePathPoint(HandleRef path, float x, float y, HandleRef pen,
                                                                 HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsOutlineVisiblePathPointI(HandleRef path, int x, int y, HandleRef pen,
                                                                  HandleRef graphics, out int boolean);

        //----------------------------------------------------------------------------------------                                                           
        // GraphicsPath Enumeration methods
        //----------------------------------------------------------------------------------------
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePathIter(out IntPtr pathIter, HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeletePathIter", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeletePathIter(HandleRef pathIter);
        internal static int GdipDeletePathIter(HandleRef pathIter) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeletePathIter(pathIter);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterNextSubpath(HandleRef pathIter, out int resultCount,
                                                           out int startIndex, out int endIndex, out bool isClosed);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterNextSubpathPath(HandleRef pathIter, out int resultCount,
                                                               HandleRef path, out bool isClosed);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterNextPathType(HandleRef pathIter, out int resultCount,
                                                            out byte pathType, out int startIndex, 
                                                            out int endIndex);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterNextMarker(HandleRef pathIter, out int resultCount,
                                                          out int startIndex, out int endIndex);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterNextMarkerPath(HandleRef pathIter, out int resultCount,
                                                              HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterGetCount(HandleRef pathIter, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterGetSubpathCount(HandleRef pathIter, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterIsValid(HandleRef pathIter, out bool valid);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterHasCurve(HandleRef pathIter, out bool hasCurve);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterRewind(HandleRef pathIter);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterEnumerate(HandleRef pathIter, out int resultCount,
                                                         IntPtr memoryPts, [In, Out] byte[] types, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPathIterCopyData(HandleRef pathIter, out int resultCount,
                                                        IntPtr memoryPts, [In, Out] byte[] types, int startIndex,
                                                        int endIndex);

        //----------------------------------------------------------------------------------------                                                           
        // Matrix methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMatrix(out IntPtr matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMatrix2(float m11, float m12,
                                                     float m21, float m22, float dx,
                                                     float dy, out IntPtr matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMatrix3(ref GPRECTF rect, HandleRef dstplg, out IntPtr matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMatrix3I(ref GPRECT rect, HandleRef dstplg, out IntPtr matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneMatrix(HandleRef matrix, out IntPtr cloneMatrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteMatrix",  CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteMatrix(HandleRef matrix);
        internal static int GdipDeleteMatrix(HandleRef matrix) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteMatrix(matrix);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetMatrixElements(HandleRef matrix, float m11,
                                                         float m12, float m21,
                                                         float m22, float dx, float dy);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMultiplyMatrix(HandleRef matrix, HandleRef matrix2, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateMatrix(HandleRef matrix, float offsetX,
                                                       float offsetY, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipScaleMatrix(HandleRef matrix, float scaleX, float scaleY, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRotateMatrix(HandleRef matrix, float angle, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipShearMatrix(HandleRef matrix, float shearX, float shearY, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipInvertMatrix(HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTransformMatrixPoints(HandleRef matrix, HandleRef pts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTransformMatrixPointsI(HandleRef matrix, HandleRef pts, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipVectorTransformMatrixPoints(HandleRef matrix, HandleRef pts,
                                                                   int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipVectorTransformMatrixPointsI(HandleRef matrix, HandleRef pts,
                                                                    int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetMatrixElements(HandleRef matrix, IntPtr m);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsMatrixInvertible(HandleRef matrix, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsMatrixIdentity(HandleRef matrix, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsMatrixEqual(HandleRef matrix, HandleRef matrix2,
                                                     out int boolean);

        //----------------------------------------------------------------------------------------                                                           
        // Region methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateRegion(out IntPtr region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateRegionRect(ref GPRECTF gprectf, out IntPtr region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateRegionRectI(ref GPRECT gprect, out IntPtr region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateRegionPath(HandleRef path, out IntPtr region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateRegionRgnData(byte[] rgndata, int size, out IntPtr region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateRegionHrgn(HandleRef hRgn, out IntPtr region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneRegion(HandleRef region, out IntPtr cloneregion);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteRegion", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteRegion(HandleRef region);
        internal static int GdipDeleteRegion(HandleRef region) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteRegion(region);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetInfinite(HandleRef region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetEmpty(HandleRef region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCombineRegionRect(HandleRef region, ref GPRECTF gprectf, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCombineRegionRectI(HandleRef region, ref GPRECT gprect, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCombineRegionPath(HandleRef region, HandleRef path, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCombineRegionRegion(HandleRef region, HandleRef region2, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateRegion(HandleRef region, float dx, float dy);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateRegionI(HandleRef region, int dx, int dy);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTransformRegion(HandleRef region, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionBounds(HandleRef region, HandleRef graphics, ref GPRECTF gprectf);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionBoundsI(HandleRef region, HandleRef graphics, ref GPRECT gprect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionHRgn(HandleRef region, HandleRef graphics, out IntPtr hrgn);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsEmptyRegion(HandleRef region, HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsInfiniteRegion(HandleRef region, HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsEqualRegion(HandleRef region, HandleRef region2, HandleRef graphics,
                                                     out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionDataSize(HandleRef region, out int bufferSize);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionData(HandleRef region, 
                                                     byte[] regionData,
                                                     int bufferSize,
                                                     out int sizeFilled);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisibleRegionPoint(HandleRef region, float X, float Y,
                                                            HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisibleRegionPointI(HandleRef region, int X, int Y,
                                                             HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisibleRegionRect(HandleRef region, float X, float Y,
                                                           float width, float height,
                                                           HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisibleRegionRectI(HandleRef region, int X, int Y,
                                                            int width, int height,
                                                            HandleRef graphics, out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionScansCount(HandleRef region, out int count, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionScans(HandleRef region, IntPtr rects, out int count, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRegionScansI(HandleRef region, IntPtr rects, out int count, HandleRef matrix);

        //----------------------------------------------------------------------------------------                                                           
        // Brush methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneBrush(HandleRef brush, out IntPtr clonebrush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteBrush", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteBrush(HandleRef brush);
        internal static int GdipDeleteBrush(HandleRef brush) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteBrush(brush);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetBrushType(HandleRef brush, out int type);

        //----------------------------------------------------------------------------------------                                                           
        // Hatch Brush
        //----------------------------------------------------------------------------------------
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateHatchBrush(int hatchstyle, int forecol, int backcol, out IntPtr brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetHatchStyle(HandleRef brush, out int hatchstyle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetHatchForegroundColor(HandleRef brush, out int forecol);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetHatchBackgroundColor(HandleRef brush, out int backcol);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateTexture(HandleRef bitmap, int wrapmode, out IntPtr texture);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateTexture2(HandleRef bitmap, int wrapmode, float x,
                                                      float y, float width, float height,
                                                      out IntPtr texture);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateTextureIA(HandleRef bitmap, HandleRef imageAttrib,
                                                       float x, float y, float width, float height,
                                                       out IntPtr texture);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateTexture2I(HandleRef bitmap, int wrapmode, int x,
                                                       int y, int width, int height,
                                                       out IntPtr texture);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateTextureIAI(HandleRef bitmap, HandleRef imageAttrib,
                                                        int x, int y, int width, int height,
                                                        out IntPtr texture);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetTextureTransform(HandleRef brush, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetTextureTransform(HandleRef brush, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetTextureTransform(HandleRef brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMultiplyTextureTransform(HandleRef brush, 
                                                                HandleRef matrix, 
                                                                MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateTextureTransform(HandleRef brush, 
                                                                 float dx,
                                                                 float dy,
                                                                 MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipScaleTextureTransform(HandleRef brush, 
                                                             float sx, 
                                                             float sy,
                                                             MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRotateTextureTransform(HandleRef brush, 
                                                              float angle,
                                                              MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetTextureWrapMode(HandleRef brush, int wrapMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetTextureWrapMode(HandleRef brush, out int wrapMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetTextureImage(HandleRef brush, out IntPtr image);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateSolidFill(int color, out IntPtr brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetSolidFillColor(HandleRef brush, int color);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetSolidFillColor(HandleRef brush, out int color);

        //----------------------------------------------------------------------------------------                                                           
        // Linear Gradient Brush
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateLineBrush(GPPOINTF point1, GPPOINTF point2, int color1, int color2, int wrapMode, out IntPtr lineGradient);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateLineBrushI(GPPOINT point1, GPPOINT point2, int color1, int color2, int wrapMode, out IntPtr lineGradient);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateLineBrushFromRect(ref GPRECTF rect, int color1, int color2, int lineGradientMode, int wrapMode,
                                                                 out IntPtr lineGradient);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateLineBrushFromRectI(ref GPRECT rect, int color1, int color2, int lineGradientMode, int wrapMode,
                                                                  out IntPtr lineGradient);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateLineBrushFromRectWithAngle(ref GPRECTF rect, int color1, int color2, float angle, bool isAngleScaleable,
                                                                          int wrapMode, out IntPtr lineGradient);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateLineBrushFromRectWithAngleI(ref GPRECT rect, int color1, int color2, float angle, bool isAngleScaleable,
                                                                           int wrapMode, out IntPtr lineGradient);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLinePoints(HandleRef brush, GPPOINTF point1, GPPOINTF point2);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLinePointsI(HandleRef brush, GPPOINT point1, GPPOINT point2);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLinePoints(HandleRef brush, HandleRef points);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLinePointsI(HandleRef brush, HandleRef points);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLineColors(HandleRef brush, int color1, int color2);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineColors(HandleRef brush, int[] colors);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineRect(HandleRef brush, ref GPRECTF gprectf);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineRectI(HandleRef brush, ref GPRECT gprect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineGammaCorrection(HandleRef brush, out bool useGammaCorrection);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLineGammaCorrection(HandleRef brush, bool useGammaCorrection);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLineSigmaBlend(HandleRef brush, float focus, float scale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLineLinearBlend(HandleRef brush, float focus, float scale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineBlendCount(HandleRef brush, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineBlend(HandleRef brush, IntPtr blend, IntPtr positions, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLineBlend(HandleRef brush, HandleRef blend, HandleRef positions, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLinePresetBlendCount(HandleRef brush, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLinePresetBlend(HandleRef brush, IntPtr blend, IntPtr positions, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLinePresetBlend(HandleRef brush, HandleRef blend, HandleRef positions, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLineWrapMode(HandleRef brush, int wrapMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineWrapMode(HandleRef brush, out int wrapMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetLineTransform(HandleRef brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMultiplyLineTransform(HandleRef brush, HandleRef matrix, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineTransform(HandleRef brush, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetLineTransform(HandleRef brush, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateLineTransform(HandleRef brush, float dx, float dy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipScaleLineTransform(HandleRef brush, float sx, float sy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRotateLineTransform(HandleRef brush, float angle, MatrixOrder order);

        //----------------------------------------------------------------------------------------                                                           
        // Path Gradient Brush
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePathGradient(HandleRef points, int count, int wrapMode, out IntPtr brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePathGradientI(HandleRef points, int count, int wrapMode, out IntPtr brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePathGradientFromPath(HandleRef path, out IntPtr brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientCenterColor(HandleRef brush,
                                                                  out int color);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientCenterColor(HandleRef brush,
                                                                  int color);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientSurroundColorsWithCount(HandleRef brush,
                                                                              int[] color,
                                                                              ref int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientSurroundColorsWithCount(HandleRef brush,
                                                                              int[] argb,
                                                                              ref int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientCenterPoint(HandleRef brush,
                                                                  GPPOINTF point);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientCenterPointI(HandleRef brush,
                                                                   GPPOINT point);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientCenterPoint(HandleRef brush,
                                                                  GPPOINTF point);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientCenterPointI(HandleRef brush,
                                                                   GPPOINT point);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientRect(HandleRef brush,
                                                           ref GPRECTF gprectf);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientRectI(HandleRef brush,
                                                            ref GPRECT gprect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientPointCount(HandleRef brush, 
                                                                 out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientSurroundColorCount(HandleRef brush, 
                                                                         out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientBlendCount(HandleRef brush,
                                                                 out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientBlend(HandleRef brush,
                                                            IntPtr blend,
                                                            IntPtr positions,
                                                            int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientBlend(HandleRef brush,
                                                            HandleRef blend,
                                                            HandleRef positions,
                                                            int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientPresetBlendCount(HandleRef brush, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientPresetBlend(HandleRef brush, IntPtr blend,
                                                                  IntPtr positions, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientPresetBlend(HandleRef brush, HandleRef blend,
                                                                  HandleRef positions, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientSigmaBlend(HandleRef brush,
                                                                 float focus,
                                                                 float scale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientLinearBlend(HandleRef brush,
                                                                  float focus,
                                                                  float scale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientWrapMode(HandleRef brush,
                                                               int wrapmode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientWrapMode(HandleRef brush,
                                                               out int wrapmode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientTransform(HandleRef brush,
                                                                HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientTransform(HandleRef brush,
                                                                HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetPathGradientTransform(HandleRef brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMultiplyPathGradientTransform(HandleRef brush, HandleRef matrix, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslatePathGradientTransform(HandleRef brush, float dx, float dy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipScalePathGradientTransform(HandleRef brush, float sx, float sy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRotatePathGradientTransform(HandleRef brush, float angle, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPathGradientFocusScales(HandleRef brush,
                                                                  float[] xScale,
                                                                  float[] yScale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPathGradientFocusScales(HandleRef brush,
                                                                  float xScale,
                                                                  float yScale);

        //----------------------------------------------------------------------------------------                                                           
        // Pen methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePen1(int argb, float width, int unit,
                                                  out IntPtr pen);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreatePen2(HandleRef brush, float width, int unit,
                                                  out IntPtr pen);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipClonePen(HandleRef pen, out IntPtr clonepen);
  
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeletePen", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeletePen(HandleRef Pen);
        internal static int GdipDeletePen(HandleRef pen) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeletePen(pen);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenMode(HandleRef pen, PenAlignment penAlign);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenMode(HandleRef pen, out PenAlignment penAlign);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenWidth(HandleRef pen, float width);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenWidth(HandleRef pen, float[] width);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenUnit(HandleRef pen, int unit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenUnit(HandleRef pen, out int unit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenLineCap197819(HandleRef pen, int startCap, int endCap, int dashCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenStartCap(HandleRef pen, int startCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenEndCap(HandleRef pen, int endCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenStartCap(HandleRef pen, out int startCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenEndCap(HandleRef pen, out int endCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenDashCap197819(HandleRef pen, out int dashCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenDashCap(HandleRef pen, out int dashCap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenDashCap197819(HandleRef pen, int dashCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenLineJoin(HandleRef pen, int lineJoin);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenLineJoin(HandleRef pen, out int lineJoin);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenCustomStartCap(HandleRef pen, HandleRef customCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenCustomStartCap(HandleRef pen, out IntPtr customCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenCustomEndCap(HandleRef pen, HandleRef customCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenCustomEndCap(HandleRef pen, out IntPtr customCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenMiterLimit(HandleRef pen, float miterLimit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenMiterLimit(HandleRef pen, float[] miterLimit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenTransform(HandleRef pen, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenTransform(HandleRef pen, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetPenTransform(HandleRef brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMultiplyPenTransform(HandleRef brush, HandleRef matrix, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslatePenTransform(HandleRef brush, float dx, float dy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipScalePenTransform(HandleRef brush, float sx, float sy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRotatePenTransform(HandleRef brush, float angle, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenColor(HandleRef pen, int argb);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenColor(HandleRef pen, out int argb);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenBrushFill(HandleRef pen, HandleRef brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenBrushFill(HandleRef pen, out IntPtr brush);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenFillType(HandleRef pen, out int pentype);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenDashStyle(HandleRef pen, out int dashstyle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenDashStyle(HandleRef pen, int dashstyle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenDashArray(HandleRef pen, HandleRef memorydash, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenDashOffset(HandleRef pen, float[] dashoffset);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenDashOffset(HandleRef pen, float dashoffset);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenDashCount(HandleRef pen, out int dashcount);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenDashArray(HandleRef pen, IntPtr memorydash, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenCompoundCount(HandleRef pen, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPenCompoundArray(HandleRef pen, float[] array, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPenCompoundArray(HandleRef pen, float[] array, int count);

        //----------------------------------------------------------------------------------------                                                           
        // CustomLineCap methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateCustomLineCap(HandleRef fillpath, HandleRef strokepath, LineCap baseCap, float baseInset, out IntPtr customCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteCustomLineCap", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteCustomLineCap(HandleRef customCap);
        internal static int GdipDeleteCustomLineCap(HandleRef customCap) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteCustomLineCap(customCap);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneCustomLineCap(HandleRef customCap, out IntPtr clonedCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCustomLineCapType(HandleRef customCap, 
                                                            out CustomLineCapType capType);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetCustomLineCapStrokeCaps(HandleRef customCap,
                                                                  LineCap startCap,
                                                                  LineCap endCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCustomLineCapStrokeCaps(HandleRef customCap,
                                                                  out LineCap startCap,
                                                                  out LineCap endCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetCustomLineCapStrokeJoin(HandleRef customCap,
                                                                  LineJoin lineJoin);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCustomLineCapStrokeJoin(HandleRef customCap,
                                                                  out LineJoin lineJoin);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetCustomLineCapBaseCap(HandleRef customCap,
                                                               LineCap baseCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCustomLineCapBaseCap(HandleRef customCap,
                                                               out LineCap baseCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetCustomLineCapBaseInset(HandleRef customCap,
                                                                 float inset);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCustomLineCapBaseInset(HandleRef customCap,
                                                                 out float inset);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetCustomLineCapWidthScale(HandleRef customCap,
                                                                  float widthScale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCustomLineCapWidthScale(HandleRef customCap,
                                                                  out float widthScale);

        //----------------------------------------------------------------------------------------                                                           
        // AdjustableArrowCap methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateAdjustableArrowCap(float height, float width, bool isFilled, out IntPtr adjustableArrowCap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetAdjustableArrowCapHeight(HandleRef adjustableArrowCap,
                                                                   float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetAdjustableArrowCapHeight(HandleRef adjustableArrowCap,
                                                                   out float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetAdjustableArrowCapWidth(HandleRef adjustableArrowCap,
                                                                  float width);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetAdjustableArrowCapWidth(HandleRef adjustableArrowCap,
                                                                  out float width);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetAdjustableArrowCapMiddleInset(HandleRef adjustableArrowCap,
                                                                        float middleInset);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetAdjustableArrowCapMiddleInset(HandleRef adjustableArrowCap,
                                                                        out float middleInset);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetAdjustableArrowCapFillState(HandleRef adjustableArrowCap,
                                                                      bool fillState);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetAdjustableArrowCapFillState(HandleRef adjustableArrowCap,
                                                                      out bool fillState);

        //----------------------------------------------------------------------------------------                                                           
        // Image methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipLoadImageFromStream(UnsafeNativeMethods.IStream stream, out IntPtr image);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipLoadImageFromFile(string filename, out IntPtr image);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipLoadImageFromStreamICM(UnsafeNativeMethods.IStream stream, out IntPtr image);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipLoadImageFromFileICM(string filename, out IntPtr image);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneImage(HandleRef image, out IntPtr cloneimage);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDisposeImage", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDisposeImage(HandleRef image);
        internal static int GdipDisposeImage(HandleRef image) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result =  IntGdipDisposeImage(image);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSaveImageToFile(HandleRef image, string filename,
                                                       ref Guid classId, HandleRef encoderParams);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSaveImageToStream(HandleRef image, UnsafeNativeMethods.IStream stream,
                                                         ref Guid classId, HandleRef encoderParams);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSaveAdd(HandleRef image, HandleRef encoderParams);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSaveAddImage(HandleRef image, HandleRef newImage, HandleRef encoderParams);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageGraphicsContext(HandleRef image, out IntPtr graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageBounds(HandleRef image, ref GPRECTF gprectf, out GraphicsUnit unit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageDimension(HandleRef image, out float width, out float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageType(HandleRef image, out int type);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageWidth(HandleRef image, out int width);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageHeight(HandleRef image, out int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageHorizontalResolution(HandleRef image, out float horzRes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageVerticalResolution(HandleRef image, out float vertRes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageFlags(HandleRef image, out int flags);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageRawFormat(HandleRef image, ref Guid format);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImagePixelFormat(HandleRef image, out int format);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageThumbnail(HandleRef image, int thumbWidth, int thumbHeight, 
                                                         out IntPtr thumbImage, 
                                                         Image.GetThumbnailImageAbort callback, 
                                                         IntPtr callbackdata);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetEncoderParameterListSize(HandleRef image, ref Guid clsid,
                                                                   out int size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetEncoderParameterList(HandleRef image, ref Guid clsid, int size,
                                                               IntPtr buffer);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipImageGetFrameDimensionsCount(HandleRef image, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipImageGetFrameDimensionsList(HandleRef image, 
                                                                   IntPtr buffer,
                                                                   int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipImageGetFrameCount(HandleRef image, ref Guid dimensionID, int[] count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipImageSelectActiveFrame(HandleRef image, ref Guid dimensionID, int frameIndex);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipImageRotateFlip(HandleRef image, int rotateFlipType);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImagePalette(HandleRef image, IntPtr palette, int size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImagePalette(HandleRef image, IntPtr palette);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImagePaletteSize(HandleRef image, out int size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPropertyCount(HandleRef image, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPropertyIdList(HandleRef image, int count, int[] list);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPropertyItemSize(HandleRef image, int propid, out int size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPropertyItem(HandleRef image, int propid, int size, IntPtr buffer);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPropertySize(HandleRef image, out int totalSize, ref int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetAllPropertyItems(HandleRef image, int totalSize, int count, IntPtr buffer);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRemovePropertyItem(HandleRef image, int propid);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPropertyItem(HandleRef image, PropertyItemInternal propitem);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipImageForceValidation(HandleRef image);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageDecodersSize(out int numDecoders, out int size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageDecoders(int numDecoders, int size, IntPtr decoders);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageEncodersSize(out int numEncoders, out int size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageEncoders(int numEncoders, int size, IntPtr encoders);

        //----------------------------------------------------------------------------------------                                                           
        // Bitmap methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromStream(UnsafeNativeMethods.IStream stream, out IntPtr bitmap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromFile(string filename, out IntPtr bitmap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromStreamICM(UnsafeNativeMethods.IStream stream, out IntPtr bitmap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromFileICM(string filename, out IntPtr bitmap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromScan0(int width, int height, int stride, int format, HandleRef scan0, out IntPtr bitmap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromGraphics(int width, int height, HandleRef graphics, out IntPtr bitmap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromHBITMAP(HandleRef hbitmap, HandleRef hpalette, out IntPtr bitmap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromHICON(HandleRef hicon, out IntPtr bitmap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateBitmapFromResource(HandleRef hresource, HandleRef name, out IntPtr bitmap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateHBITMAPFromBitmap(HandleRef nativeBitmap, out IntPtr hbitmap, int argbBackground);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateHICONFromBitmap(HandleRef nativeBitmap, out IntPtr hicon);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneBitmapArea(float x, float y, float width, float height, int format, HandleRef srcbitmap, out IntPtr dstbitmap);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneBitmapAreaI(int x, int y, int width, int height, int format, HandleRef srcbitmap, out IntPtr dstbitmap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBitmapLockBits(HandleRef bitmap, 
                                                      ref GPRECT rect,
                                                      ImageLockMode flags, // ImageLockMode
                                                      PixelFormat format, // PixelFormat
                                                      [In, Out] BitmapData lockedBitmapData);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBitmapUnlockBits(HandleRef bitmap,
                                                        BitmapData lockedBitmapData);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBitmapGetPixel(HandleRef bitmap, int x, int y, out int argb);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBitmapSetPixel(HandleRef bitmap, int x, int y, int argb);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBitmapSetResolution(HandleRef bitmap, float dpix, float dpiy);

        //----------------------------------------------------------------------------------------                                                           
        // ImageAttributes methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateImageAttributes(out IntPtr imageattr);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneImageAttributes(HandleRef imageattr, out IntPtr cloneImageattr);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDisposeImageAttributes", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDisposeImageAttributes(HandleRef imageattr);
        internal static int GdipDisposeImageAttributes(HandleRef imageattr) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDisposeImageAttributes(imageattr);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesToIdentity(HandleRef imageattr,
                                                                    ColorAdjustType type);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetImageAttributes(HandleRef imageattr,
                                                            ColorAdjustType type);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesColorMatrix(HandleRef imageattr,
                                                                     ColorAdjustType type,
                                                                     bool enableFlag,
                                                                     ColorMatrix colorMatrix,
                                                                     ColorMatrix grayMatrix,
                                                                     ColorMatrixFlag flags);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesThreshold(HandleRef imageattr,
                                                                   ColorAdjustType type,
                                                                   bool enableFlag,
                                                                   float threshold);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesGamma(HandleRef imageattr,
                                                               ColorAdjustType type,
                                                               bool enableFlag,
                                                               float gamma);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesNoOp(HandleRef imageattr,
                                                              ColorAdjustType type,
                                                              bool enableFlag);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesColorKeys(HandleRef imageattr,
                                                                   ColorAdjustType type,
                                                                   bool enableFlag,
                                                                   int colorLow, // yes, ref, not out
                                                                   int colorHigh);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesOutputChannel(HandleRef imageattr,
                                                                       ColorAdjustType type,
                                                                       bool enableFlag,
                                                                       ColorChannelFlag flags);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesOutputChannelColorProfile(
                                                                                  HandleRef imageattr,
                                                                                  ColorAdjustType type,
                                                                                  bool enableFlag,
                                                                                  string colorProfileFilename);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesRemapTable(HandleRef imageattr,
                                                                    ColorAdjustType type,
                                                                    bool enableFlag,
                                                                    int mapSize,
                                                                    HandleRef map);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetImageAttributesWrapMode(HandleRef imageattr,
                                                                  int wrapmode,
                                                                  int argb,
                                                                  bool clamp);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetImageAttributesAdjustedPalette(HandleRef imageattr,
                                                                         HandleRef palette,
                                                                         ColorAdjustType type);

        //----------------------------------------------------------------------------------------                                                           
        // Graphics methods
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFlush(HandleRef graphics, FlushIntention intention);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFromHDC(HandleRef hdc, out IntPtr graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFromHDC2(HandleRef hdc, HandleRef hdevice, out IntPtr graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFromHWND(HandleRef hwnd, out IntPtr graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFromHWNDICM(HandleRef hwnd, out IntPtr graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteGraphics", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteGraphics(HandleRef graphics);
        internal static int GdipDeleteGraphics(HandleRef graphics) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteGraphics(graphics);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetDC(HandleRef graphics, out IntPtr hdc);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipReleaseDC", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipReleaseDC(HandleRef graphics, HandleRef hdc);
        internal static int GdipReleaseDC(HandleRef graphics, HandleRef hdc) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipReleaseDC(graphics, hdc);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetCompositingMode(HandleRef graphics, int compositeMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetTextRenderingHint(HandleRef graphics, TextRenderingHint textRenderingHint);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetTextContrast(HandleRef graphics, int textContrast);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetInterpolationMode(HandleRef graphics, int mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCompositingMode(HandleRef graphics, out int compositeMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetRenderingOrigin(HandleRef graphics, int x, int y);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetRenderingOrigin(HandleRef graphics, out int x, out int y);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetCompositingQuality(HandleRef graphics, CompositingQuality quality);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCompositingQuality(HandleRef graphics, out CompositingQuality quality);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetSmoothingMode(HandleRef graphics, SmoothingMode smoothingMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetSmoothingMode(HandleRef graphics, out SmoothingMode smoothingMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPixelOffsetMode(HandleRef graphics, PixelOffsetMode pixelOffsetMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPixelOffsetMode(HandleRef graphics, out PixelOffsetMode pixelOffsetMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetTextRenderingHint(HandleRef graphics, out TextRenderingHint textRenderingHint);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetTextContrast(HandleRef graphics, out int textContrast);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetInterpolationMode(HandleRef graphics, out int mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetWorldTransform(HandleRef graphics, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetWorldTransform(HandleRef graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMultiplyWorldTransform(HandleRef graphics, HandleRef matrix, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateWorldTransform(HandleRef graphics, float dx, float dy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipScaleWorldTransform(HandleRef graphics, float sx, float sy, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRotateWorldTransform(HandleRef graphics, float angle, MatrixOrder order);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetWorldTransform(HandleRef graphics, HandleRef matrix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetPageTransform(HandleRef graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPageUnit(HandleRef graphics, out int unit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetPageScale(HandleRef graphics, float[] scale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPageUnit(HandleRef graphics, int unit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetPageScale(HandleRef graphics, float scale);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetDpiX(HandleRef graphics, float[] dpi);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetDpiY(HandleRef graphics, float[] dpi);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTransformPoints(HandleRef graphics, int destSpace,
                                                       int srcSpace, IntPtr points, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTransformPointsI(HandleRef graphics, int destSpace,
                                                        int srcSpace, IntPtr points, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetNearestColor(HandleRef graphics, ref int color);

        // Create the Win9x Halftone Palette (even on NT) with correct Desktop colors
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern IntPtr GdipCreateHalftonePalette();

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawLine(HandleRef graphics, HandleRef pen, float x1, float y1,
                                                float x2, float y2);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawLineI(HandleRef graphics, HandleRef pen, int x1, int y1,
                                                 int x2, int y2);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawLines(HandleRef graphics, HandleRef pen, HandleRef points,
                                                 int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawLinesI(HandleRef graphics, HandleRef pen, HandleRef points,
                                                  int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawArc(HandleRef graphics, HandleRef pen, float x, float y,
                                               float width, float height, float startAngle,
                                               float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawArcI(HandleRef graphics, HandleRef pen, int x, int y,
                                                int width, int height, float startAngle,
                                                float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawBezier(HandleRef graphics, HandleRef pen, float x1, float y1,
                                                  float x2, float y2, float x3, float y3,
                                                  float x4, float y4);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawBezierI(HandleRef graphics, HandleRef pen, int x1, int y1,
                                                   int x2, int y2, int x3, int y3,
                                                   int x4, int y4);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawBeziers(HandleRef graphics, HandleRef pen, HandleRef points,
                                                   int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawBeziersI(HandleRef graphics, HandleRef pen, HandleRef points,
                                                    int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawRectangle(HandleRef graphics, HandleRef pen, float x, float y,
                                                     float width, float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawRectangleI(HandleRef graphics, HandleRef pen, int x, int y,
                                                      int width, int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawRectangles(HandleRef graphics, HandleRef pen, HandleRef rects,
                                                      int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawRectanglesI(HandleRef graphics, HandleRef pen, HandleRef rects,
                                                       int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawEllipse(HandleRef graphics, HandleRef pen, float x, float y,
                                                   float width, float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawEllipseI(HandleRef graphics, HandleRef pen, int x, int y,
                                                    int width, int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawPie(HandleRef graphics, HandleRef pen, float x, float y,
                                               float width, float height, float startAngle,
                                               float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawPieI(HandleRef graphics, HandleRef pen, int x, int y,
                                                int width, int height, float startAngle,
                                                float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawPolygon(HandleRef graphics, HandleRef pen, HandleRef points,
                                                   int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawPolygonI(HandleRef graphics, HandleRef pen, HandleRef points,
                                                    int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawPath(HandleRef graphics, HandleRef pen, HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawCurve(HandleRef graphics, HandleRef pen, HandleRef points,
                                                 int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawCurveI(HandleRef graphics, HandleRef pen, HandleRef points,
                                                  int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawCurve2(HandleRef graphics, HandleRef pen, HandleRef points,
                                                  int count, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawCurve2I(HandleRef graphics, HandleRef pen, HandleRef points,
                                                   int count, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawCurve3(HandleRef graphics, HandleRef pen, HandleRef points,
                                                  int count, int offset,
                                                  int numberOfSegments, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawCurve3I(HandleRef graphics, HandleRef pen, HandleRef points,
                                                   int count, int offset,
                                                   int numberOfSegments, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawClosedCurve(HandleRef graphics, HandleRef pen, HandleRef points,
                                                       int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawClosedCurveI(HandleRef graphics, HandleRef pen, HandleRef points,
                                                        int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawClosedCurve2(HandleRef graphics, HandleRef pen, HandleRef points,
                                                        int count, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawClosedCurve2I(HandleRef graphics, HandleRef pen, HandleRef points,
                                                         int count, float tension);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGraphicsClear(HandleRef graphics, int argb);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillRectangle(HandleRef graphics, HandleRef brush, float x, float y,
                                                     float width, float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillRectangleI(HandleRef graphics, HandleRef brush, int x, int y,
                                                      int width, int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillRectangles(HandleRef graphics, HandleRef brush, HandleRef rects,
                                                      int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillRectanglesI(HandleRef graphics, HandleRef brush, HandleRef rects,
                                                       int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillPolygon(HandleRef graphics, HandleRef brush, HandleRef points,
                                                   int count, int brushMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillPolygonI(HandleRef graphics, HandleRef brush, HandleRef points,
                                                    int count, int brushMode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillPolygon2(HandleRef graphics, HandleRef brush, HandleRef points,
                                                    int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillPolygon2I(HandleRef graphics, HandleRef brush, HandleRef points,
                                                     int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillEllipse(HandleRef graphics, HandleRef brush, float x, float y,
                                                   float width, float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillEllipseI(HandleRef graphics, HandleRef brush, int x, int y,
                                                    int width, int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillPie(HandleRef graphics, HandleRef brush, float x, float y,
                                               float width, float height, float startAngle,
                                               float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillPieI(HandleRef graphics, HandleRef brush, int x, int y,
                                                int width, int height, float startAngle,
                                                float sweepAngle);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillPath(HandleRef graphics, HandleRef brush, HandleRef path);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillClosedCurve(HandleRef graphics, HandleRef brush, HandleRef points,
                                                       int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillClosedCurveI(HandleRef graphics, HandleRef brush, HandleRef points,
                                                        int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillClosedCurve2(HandleRef graphics, HandleRef brush, HandleRef points,
                                                        int count, float tension,
                                                        int mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillClosedCurve2I(HandleRef graphics, HandleRef brush, HandleRef points,
                                                         int count, float tension,
                                                         int mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipFillRegion(HandleRef graphics, HandleRef brush, HandleRef region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImage(HandleRef graphics, HandleRef image, float x, float y);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImageI(HandleRef graphics, HandleRef image, int x, int y);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImageRect(HandleRef graphics, HandleRef image, float x,
                                                     float y, float width, float height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImageRectI(HandleRef graphics, HandleRef image, int x,
                                                      int y, int width, int height);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImagePoints(HandleRef graphics, HandleRef image,
                                                       HandleRef points, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImagePointsI(HandleRef graphics, HandleRef image,
                                                        HandleRef points, int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImagePointRect(HandleRef graphics, HandleRef image, float x,
                                                          float y, float srcx, float srcy,
                                                          float srcwidth, float srcheight,
                                                          int srcunit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImagePointRectI(HandleRef graphics, HandleRef image, int x,
                                                           int y, int srcx, int srcy,
                                                           int srcwidth, int srcheight,
                                                           int srcunit);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImageRectRect(HandleRef graphics, HandleRef image,
                                                         float dstx, float dsty,
                                                         float dstwidth, float dstheight,
                                                         float srcx, float srcy,
                                                         float srcwidth, float srcheight,
                                                         int srcunit, HandleRef imageAttributes,
                                                         Graphics.DrawImageAbort callback, HandleRef callbackdata);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImageRectRectI(HandleRef graphics, HandleRef image,
                                                          int dstx, int dsty,
                                                          int dstwidth, int dstheight,
                                                          int srcx, int srcy,
                                                          int srcwidth, int srcheight,
                                                          int srcunit, HandleRef imageAttributes,
                                                          Graphics.DrawImageAbort callback, HandleRef callbackdata);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImagePointsRect(HandleRef graphics, HandleRef image,
                                                           HandleRef points, int count, float srcx,
                                                           float srcy, float srcwidth,
                                                           float srcheight, int srcunit,
                                                           HandleRef imageAttributes,
                                                           Graphics.DrawImageAbort callback, HandleRef callbackdata);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawImagePointsRectI(HandleRef graphics, HandleRef image,
                                                            HandleRef points, int count, int srcx,
                                                            int srcy, int srcwidth,
                                                            int srcheight, int srcunit,
                                                            HandleRef imageAttributes,
                                                            Graphics.DrawImageAbort callback, HandleRef callbackdata);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileDestPoint(HandleRef graphics,
                                                                  HandleRef metafile,
                                                                  GPPOINTF destPoint,
                                                                  Graphics.EnumerateMetafileProc callback,
                                                                  HandleRef callbackdata,
                                                                  HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileDestPointI(HandleRef graphics,
                                                                   HandleRef metafile,
                                                                   GPPOINT destPoint,
                                                                   Graphics.EnumerateMetafileProc callback,
                                                                   HandleRef callbackdata,
                                                                   HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileDestRect(HandleRef graphics,
                                                                 HandleRef metafile,
                                                                 ref GPRECTF destRect,
                                                                 Graphics.EnumerateMetafileProc callback,
                                                                 HandleRef callbackdata,
                                                                 HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileDestRectI(HandleRef graphics,
                                                                  HandleRef metafile,
                                                                  ref GPRECT destRect,
                                                                  Graphics.EnumerateMetafileProc callback,
                                                                  HandleRef callbackdata,
                                                                  HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileDestPoints(HandleRef graphics,
                                                                   HandleRef metafile,
                                                                   IntPtr destPoints,
                                                                   int count,
                                                                   Graphics.EnumerateMetafileProc callback,
                                                                   HandleRef callbackdata,
                                                                   HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileDestPointsI(HandleRef graphics,
                                                                    HandleRef metafile,
                                                                    IntPtr destPoints,
                                                                    int count,
                                                                    Graphics.EnumerateMetafileProc callback,
                                                                    HandleRef callbackdata,
                                                                    HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileSrcRectDestPoint(HandleRef graphics,
                                                                         HandleRef metafile,
                                                                         GPPOINTF destPoint,
                                                                         ref GPRECTF srcRect,
                                                                         int pageUnit,
                                                                         Graphics.EnumerateMetafileProc callback,
                                                                         HandleRef callbackdata,
                                                                         HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileSrcRectDestPointI(HandleRef graphics,
                                                                          HandleRef metafile,
                                                                          GPPOINT destPoint,
                                                                          ref GPRECT srcRect,
                                                                          int pageUnit,
                                                                          Graphics.EnumerateMetafileProc callback,
                                                                          HandleRef callbackdata,
                                                                          HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileSrcRectDestRect(HandleRef graphics,
                                                                        HandleRef metafile,
                                                                        ref GPRECTF destRect,
                                                                        ref GPRECTF srcRect,
                                                                        int pageUnit,
                                                                        Graphics.EnumerateMetafileProc callback,
                                                                        HandleRef callbackdata,
                                                                        HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileSrcRectDestRectI(HandleRef graphics,
                                                                         HandleRef metafile,
                                                                         ref GPRECT destRect,
                                                                         ref GPRECT srcRect,
                                                                         int pageUnit,
                                                                         Graphics.EnumerateMetafileProc callback,
                                                                         HandleRef callbackdata,
                                                                         HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileSrcRectDestPoints(HandleRef graphics,
                                                                          HandleRef metafile,
                                                                          IntPtr destPoints,
                                                                          int count,
                                                                          ref GPRECTF srcRect,
                                                                          int pageUnit,
                                                                          Graphics.EnumerateMetafileProc callback,
                                                                          HandleRef callbackdata,
                                                                          HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEnumerateMetafileSrcRectDestPointsI(HandleRef graphics,
                                                                           HandleRef metafile,
                                                                           IntPtr destPoints,
                                                                           int count,
                                                                           ref GPRECT srcRect,
                                                                           int pageUnit,
                                                                           Graphics.EnumerateMetafileProc callback,
                                                                           HandleRef callbackdata,
                                                                           HandleRef imageattributes);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPlayMetafileRecord(HandleRef graphics, 
                                                          EmfPlusRecordType recordType, 
                                                          int flags, 
                                                          int dataSize, 
                                                          byte[] data);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetClipGraphics(HandleRef graphics, HandleRef srcgraphics, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetClipRect(HandleRef graphics, float x, float y,
                                                   float width, float height, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetClipRectI(HandleRef graphics, int x, int y,
                                                    int width, int height, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetClipPath(HandleRef graphics, HandleRef path, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetClipRegion(HandleRef graphics, HandleRef region, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetClipHrgn(HandleRef graphics, HandleRef hRgn, CombineMode mode);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipResetClip(HandleRef graphics);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateClip(HandleRef graphics, float dx, float dy);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipTranslateClipI(HandleRef graphics, int dx, int dy);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetClip(HandleRef graphics, HandleRef region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetClipBounds(HandleRef graphics, ref GPRECTF rect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetClipBoundsI(HandleRef graphics, ref GPRECT rect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsClipEmpty(HandleRef graphics,
                                                   out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetVisibleClipBounds(HandleRef graphics, ref GPRECTF rect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetVisibleClipBoundsI(HandleRef graphics, ref GPRECT rect);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisibleClipEmpty(HandleRef graphics,
                                                          out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisiblePoint(HandleRef graphics, float x, float y,
                                                      out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisiblePointI(HandleRef graphics, int x, int y,
                                                       out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisibleRect(HandleRef graphics, float x, float y,
                                                     float width, float height,
                                                     out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsVisibleRectI(HandleRef graphics, int x, int y,
                                                      int width, int height,
                                                      out int boolean);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSaveGraphics(HandleRef graphics, out int state);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRestoreGraphics(HandleRef graphics, int state);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBeginContainer(HandleRef graphics, ref GPRECTF dstRect,
                                                      ref GPRECTF srcRect, int unit,
                                                      out int state);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBeginContainer2(HandleRef graphics, out int state);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipBeginContainerI(HandleRef graphics, ref GPRECT dstRect,
                                                       ref GPRECT srcRect, int unit,
                                                       out int state);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEndContainer(HandleRef graphics, int state);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetMetafileHeaderFromWmf(HandleRef hMetafile,      // WMF
                                                                WmfPlaceableFileHeader wmfplaceable,
                                                                [In, Out] MetafileHeaderWmf metafileHeaderWmf);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetMetafileHeaderFromEmf(HandleRef hEnhMetafile,   // EMF
                                                                [In, Out] MetafileHeaderEmf metafileHeaderEmf);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetMetafileHeaderFromFile(string filename,
                                                                 IntPtr header);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetMetafileHeaderFromStream(UnsafeNativeMethods.IStream stream,
                                                                   IntPtr header);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetMetafileHeaderFromMetafile(HandleRef metafile,
                                                                     IntPtr header);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetHemfFromMetafile(HandleRef metafile,
                                                           out IntPtr hEnhMetafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateStreamOnFile(string file, int access, UnsafeNativeMethods.IStream[] stream);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMetafileFromWmf(HandleRef hMetafile, WmfPlaceableFileHeader wmfplacealbeHeader,bool deleteWmf, out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMetafileFromEmf(HandleRef hEnhMetafile, bool deleteEmf, out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMetafileFromFile(string file, out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateMetafileFromWmfFile(string file, WmfPlaceableFileHeader wmfplaceableFileHeader, 
                                                                 out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode        
        internal static extern int GdipCreateMetafileFromStream(UnsafeNativeMethods.IStream stream, out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafile(HandleRef referenceHdc,
                                                      int emfType,
                                                      ref GPRECTF frameRect,
                                                      int frameUnit,
                                                      string description,
                                                      out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafile(HandleRef referenceHdc,
                                                      int emfType,
                                                      HandleRef pframeRect,
                                                      int frameUnit,
                                                      string description,
                                                      out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafileI(HandleRef referenceHdc,
                                                       int emfType,
                                                       ref GPRECT frameRect,
                                                       int frameUnit,
                                                       string description,
                                                       out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafileFileName(string fileName,
                                                              HandleRef referenceHdc,
                                                              int emfType,
                                                              ref GPRECTF frameRect,
                                                              int frameUnit,
                                                              string description,
                                                              out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafileFileName(string fileName,
                                                              HandleRef referenceHdc,
                                                              int emfType,
                                                              HandleRef pframeRect,
                                                              int frameUnit,
                                                              string description,
                                                              out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafileFileNameI(string fileName,
                                                               HandleRef referenceHdc,
                                                               int emfType,
                                                               ref GPRECT frameRect,
                                                               int frameUnit,
                                                               string description,
                                                               out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafileStream(UnsafeNativeMethods.IStream stream,
                                                            HandleRef referenceHdc,
                                                            int emfType,
                                                            ref GPRECTF frameRect,
                                                            int frameUnit,
                                                            string description,
                                                            out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafileStream(UnsafeNativeMethods.IStream stream,
                                                            HandleRef referenceHdc,
                                                            int emfType,
                                                            HandleRef pframeRect,
                                                            int frameUnit,
                                                            string description,
                                                            out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipRecordMetafileStreamI(UnsafeNativeMethods.IStream stream,
                                                             HandleRef referenceHdc,
                                                             int emfType,
                                                             ref GPRECT frameRect,
                                                             int frameUnit,
                                                             string description,
                                                             out IntPtr metafile);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetMetafileDownLevelRasterizationLimit(
                                                             HandleRef metafile,
                                                             int rasterizationLimit);
                                                                     
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetMetafileDownLevelRasterizationLimit(
                                                             HandleRef metafile,
                                                             out int rasterizationLimit);
                                                                     
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipEmfToWmfBits(int hEnhMetafile,
                                                    int cbData16,
                                                    byte[] pData16,
                                                    int mapMode,
                                                    int eFlags);
        
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipComment(HandleRef graphics, int sizeData, byte[] data);

        //----------------------------------------------------------------------------------------
        // Font Collection
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipNewInstalledFontCollection(out IntPtr fontCollection);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipNewPrivateFontCollection(out IntPtr fontCollection);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeletePrivateFontCollection", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeletePrivateFontCollection(out IntPtr fontCollection);
        internal static int GdipDeletePrivateFontCollection(out IntPtr fontCollection) {
            if (IsShutdown) {
                fontCollection = IntPtr.Zero;
                return SafeNativeMethods.Ok;
            }
            int result = IntGdipDeletePrivateFontCollection(out fontCollection);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFontCollectionFamilyCount(HandleRef fontCollection, out int numFound);

        // should be IntPtr
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFontCollectionFamilyList(HandleRef fontCollection, int numSought, IntPtr[] gpfamilies,
                                                                   out int numFound);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPrivateAddFontFile(HandleRef fontCollection, string filename);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipPrivateAddMemoryFont(HandleRef fontCollection, HandleRef memory, int length);

        //----------------------------------------------------------------------------------------                                                           
        // FontFamily
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFontFamilyFromName(string name, HandleRef fontCollection, out IntPtr FontFamily);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetGenericFontFamilySansSerif(out IntPtr fontfamily);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetGenericFontFamilySerif(out IntPtr fontfamily);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetGenericFontFamilyMonospace(out IntPtr fontfamily);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteFontFamily", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteFontFamily(HandleRef fontFamily);
        internal static int GdipDeleteFontFamily(HandleRef fontFamily) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteFontFamily(fontFamily);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneFontFamily(HandleRef fontfamily, out IntPtr clonefontfamily);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFamilyName(HandleRef family, StringBuilder name, int language);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipIsStyleAvailable(HandleRef family, FontStyle style, out int isStyleAvailable);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetEmHeight(HandleRef family, FontStyle style, out int EmHeight);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCellAscent(HandleRef family, FontStyle style, out int CellAscent);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetCellDescent(HandleRef family, FontStyle style, out int CellDescent);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLineSpacing(HandleRef family, FontStyle style, out int LineSpaceing);
        //----------------------------------------------------------------------------------------                                                           
        // Font      
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFontFromDC(HandleRef hdc, ref IntPtr font);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Ansi)] // DIFFERENT: ANSI, not Unicode
        internal static extern int GdipCreateFontFromLogfontA(HandleRef hdc, [In, Out, MarshalAs(UnmanagedType.AsAny)] object lf, out IntPtr font);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFontFromLogfontW(HandleRef hdc, [In, Out, MarshalAs(UnmanagedType.AsAny)] object lf, out IntPtr font);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateFont(HandleRef fontFamily, float emSize, FontStyle style, GraphicsUnit unit, out IntPtr font);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetLogFontW(HandleRef font, HandleRef graphics, [In, Out, MarshalAs(UnmanagedType.AsAny)] object lf);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Ansi)] // 3 = Unicode
        internal static extern int GdipGetLogFontA(HandleRef font, HandleRef graphics, [In, Out, MarshalAs(UnmanagedType.AsAny)] object lf);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneFont(HandleRef font, out IntPtr cloneFont);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteFont", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteFont(HandleRef font);
        internal static int GdipDeleteFont(HandleRef font) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteFont(font);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFamily(HandleRef font, out IntPtr family);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFontStyle(HandleRef font, out FontStyle style);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFontSize(HandleRef font, out float size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFontHeight(HandleRef font, HandleRef graphics, out float size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFontHeightGivenDPI(HandleRef font, float dpi, out float size);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetFontUnit(HandleRef font, out GraphicsUnit unit);

        // Text

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawString(HandleRef graphics, string textString, int length, HandleRef font, ref GPRECTF layoutRect,
                                                  HandleRef stringFormat, HandleRef brush);
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMeasureString(HandleRef graphics, string textString, int length, HandleRef font, ref GPRECTF layoutRect,
                                                     HandleRef stringFormat, [In, Out] ref GPRECTF boundingBox, out int codepointsFitted, out int linesFilled);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipMeasureCharacterRanges(HandleRef graphics, string textString, int length, HandleRef font, ref GPRECTF layoutRect, HandleRef stringFormat, 
                                                           int characterCount, [In, Out] int[] region);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatMeasurableCharacterRanges(HandleRef format, int rangeCount, [In, Out] CharacterRange[] range);
                                                                      
        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateStringFormat(StringFormatFlags options, int language, out IntPtr format);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipStringFormatGetGenericDefault(out IntPtr format);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipStringFormatGetGenericTypographic(out IntPtr format);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteStringFormat", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteStringFormat(HandleRef format);
        internal static int GdipDeleteStringFormat(HandleRef format) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteStringFormat(format);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCloneStringFormat(HandleRef format, out IntPtr newFormat);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatFlags(HandleRef format, StringFormatFlags options);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatFlags(HandleRef format, out StringFormatFlags result);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatLineSpacingAmount(HandleRef format, out float amount);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatAlign(HandleRef format, StringAlignment align);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatAlign(HandleRef format, out StringAlignment align);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatLineAlign(HandleRef format, StringAlignment align);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatLineAlign(HandleRef format, out StringAlignment align);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatHotkeyPrefix(HandleRef format, HotkeyPrefix hotkeyPrefix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatHotkeyPrefix(HandleRef format, out HotkeyPrefix hotkeyPrefix);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatTabStops(HandleRef format, float firstTabOffset, int count, float[] tabStops);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatTabStops(HandleRef format, int count, out float firstTabOffset, [In, Out]float[] tabStops);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatTabStopCount(HandleRef format, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatMeasurableCharacterRangeCount(HandleRef format, out int count);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatTrimming(HandleRef format, StringTrimming  trimming);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatTrimming(HandleRef format, out StringTrimming trimming);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipSetStringFormatDigitSubstitution(HandleRef format, int langID, StringDigitSubstitute sds);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipGetStringFormatDigitSubstitution(HandleRef format, out int langID, out StringDigitSubstitute sds);

        //----------------------------------------------------------------------------------------                                                           
        // Cached Bitmap APIs
        //----------------------------------------------------------------------------------------

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipCreateCachedBitmap(HandleRef bitmap, HandleRef graphics, out IntPtr cachedBitmap);

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, EntryPoint="GdipDeleteCachedBitmap", CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        private static extern int IntGdipDeleteCachedBitmap(HandleRef cachedBitmap);
        internal static int GdipDeleteCachedBitmap(HandleRef cachedBitmap) {
            if (IsShutdown) return SafeNativeMethods.Ok;
            int result = IntGdipDeleteCachedBitmap(cachedBitmap);
            return result;
        }

        [DllImport(ExternDll.Gdiplus, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)] // 3 = Unicode
        internal static extern int GdipDrawCachedBitmap(HandleRef graphics, HandleRef cachedbitmap, int x, int y);

        //----------------------------------------------------------------------------------------                                                           
        // Status codes
        //----------------------------------------------------------------------------------------
        internal const int Ok = 0;
        internal const int GenericError = 1;
        internal const int InvalidParameter = 2;
        internal const int OutOfMemory = 3;
        internal const int ObjectBusy = 4;
        internal const int InsufficientBuffer = 5;
        internal const int NotImplemented = 6;
        internal const int Win32Error = 7;
        internal const int WrongState = 8;
        internal const int Aborted = 9;
        internal const int FileNotFound = 10; 
        internal const int ValueOverflow = 11;
        internal const int AccessDenied = 12;
        internal const int UnknownImageFormat = 13;
        internal const int FontFamilyNotFound = 14;
        internal const int FontStyleNotFound = 15;
        internal const int NotTrueTypeFont = 16;
        internal const int UnsupportedGdiplusVersion = 17;
        internal const int GdiplusNotInitialized = 18;
        internal const int PropertyNotFound = 19;
        internal const int PropertyNotSupported = 20;

        internal static Exception StatusException(int status) {

            Debug.Assert(status != Ok, "Throwing an exception for an 'Ok' return code");

            switch (status) {
                case GenericError:       return new ExternalException(SR.GetString(SR.GdiplusGenericError), E_FAIL);
                case InvalidParameter:   return new ArgumentException(SR.GetString(SR.GdiplusInvalidParameter));
                case OutOfMemory:        return new OutOfMemoryException(SR.GetString(SR.GdiplusOutOfMemory));
                case ObjectBusy:         return new InvalidOperationException(SR.GetString(SR.GdiplusObjectBusy));
                case InsufficientBuffer: return new OutOfMemoryException(SR.GetString(SR.GdiplusInsufficientBuffer));
                case NotImplemented:     return new NotImplementedException(SR.GetString(SR.GdiplusNotImplemented));
                case Win32Error:         return new ExternalException(SR.GetString(SR.GdiplusGenericError), E_FAIL);
                case WrongState:         return new InvalidOperationException(SR.GetString(SR.GdiplusWrongState));
                case Aborted:            return new ExternalException(SR.GetString(SR.GdiplusAborted), E_ABORT);
                case FileNotFound:       return new FileNotFoundException(SR.GetString(SR.GdiplusFileNotFound));
                case ValueOverflow:      return new OverflowException(SR.GetString(SR.GdiplusOverflow));
                case AccessDenied:       return new ExternalException(SR.GetString(SR.GdiplusAccessDenied), E_ACCESSDENIED);
                case UnknownImageFormat: return new ArgumentException(SR.GetString(SR.GdiplusUnknownImageFormat));
                case PropertyNotFound:   return new ArgumentException(SR.GetString(SR.GdiplusPropertyNotFoundError));
                case PropertyNotSupported:return new ArgumentException(SR.GetString(SR.GdiplusPropertyNotSupportedError));


                case FontFamilyNotFound:
                    Debug.Fail("We should be special casing FontFamilyNotFound so we can provide the font name");
                    return new ArgumentException(SR.GetString(SR.GdiplusFontFamilyNotFound, "?"));

                case FontStyleNotFound:
                    Debug.Fail("We should be special casing FontStyleNotFound so we can provide the font name");
                    return new ArgumentException(SR.GetString(SR.GdiplusFontStyleNotFound, "?", "?"));

                case NotTrueTypeFont:
                    Debug.Fail("We should be special casing NotTrueTypeFont so we can provide the font name");
                    return new ArgumentException(SR.GetString(SR.GdiplusNotTrueTypeFont_NoName));

                case UnsupportedGdiplusVersion:
                    return new ExternalException(SR.GetString(SR.GdiplusUnsupportedGdiplusVersion), E_FAIL);

                case GdiplusNotInitialized:
                    return new ExternalException(SR.GetString(SR.GdiplusNotInitialized), E_FAIL);
            }

            return new ExternalException(SR.GetString(SR.GdiplusUnknown), E_UNEXPECTED);
        }

        //----------------------------------------------------------------------------------------                                                           
        // Helper function:  Convert GpPointF* memory block to PointF[]
        //----------------------------------------------------------------------------------------
        internal static PointF[] ConvertGPPOINTFArrayF(IntPtr memory, int count) {
            if (memory == IntPtr.Zero)
                throw new ArgumentNullException("temporary memory allocation");

            PointF[] points = new PointF[count];

            int index;
            GPPOINTF pt = new GPPOINTF();

            int size = (int)Marshal.SizeOf(pt.GetType());

            for (index=0; index < count; index++) {
                pt = (GPPOINTF)  UnsafeNativeMethods.PtrToStructure((IntPtr)((long)memory+index*size), pt.GetType());
                points[index] = new PointF(pt.X, pt.Y);
            }

            return points;
        }

        //----------------------------------------------------------------------------------------                                                           
        // Helper function:  Convert GpPoint* memory block to Point[]
        //----------------------------------------------------------------------------------------
        internal static Point[] ConvertGPPOINTArray(IntPtr memory, int count) {
            if (memory == IntPtr.Zero)
                throw new ArgumentNullException("temporary memory allocation");

            Point[] points = new Point[count];

            int index;
            GPPOINT pt = new GPPOINT();

            int size = (int)Marshal.SizeOf(pt.GetType());

            for (index=0; index < count; index++) {
                pt = (GPPOINT)  UnsafeNativeMethods.PtrToStructure((IntPtr)((long)memory+index*size), pt.GetType());
                points[index] = new Point( (int)pt.X, (int) pt.Y);
            }

            return points;
        }

        //----------------------------------------------------------------------------------------                                                           
        // Helper function:  Convert PointF[] to native memory block GpPointF*
        //----------------------------------------------------------------------------------------
        internal static IntPtr ConvertPointToMemory(PointF[] points) {
            if (points == null)
                throw new ArgumentNullException("points");

            int index;

            GPPOINTF pt = new GPPOINTF();

            int size =  (int)Marshal.SizeOf(pt.GetType());
            int count = points.Length;

            IntPtr memory =  Marshal.AllocHGlobal(count*size);

            for (index=0; index<count; index++) {
                Marshal.StructureToPtr(new GPPOINTF(points[index]), (IntPtr)((long)memory+index*size), false);
            }

            return memory;
        }

        //----------------------------------------------------------------------------------------                                                           
        // Helper function:  Convert Point[] to native memory block GpPoint*
        //----------------------------------------------------------------------------------------
        internal static IntPtr ConvertPointToMemory(Point[] points) {
            if (points == null)
                throw new ArgumentNullException("points");

            int index;

            GPPOINT pt = new GPPOINT();

            int size =  (int)Marshal.SizeOf(pt.GetType());
            int count = points.Length;

            IntPtr memory =  Marshal.AllocHGlobal(count*size);

            for (index=0; index<count; index++) {
                Marshal.StructureToPtr(new GPPOINT(points[index]), (IntPtr)((long)memory+index*size), false);
            }

            return memory;
        }

        //----------------------------------------------------------------------------------------                                                           
        // Helper function:  Convert RectangleF[] to native memory block GpRectF*
        //----------------------------------------------------------------------------------------
        internal static IntPtr ConvertRectangleToMemory(RectangleF[] rect) {
            if (rect == null)
                throw new ArgumentNullException("rectangle");

            int index;

            GPRECTF rt = new GPRECTF();

            int size =  (int)Marshal.SizeOf(rt.GetType());
            int count = rect.Length;

            IntPtr memory =  Marshal.AllocHGlobal(count*size);

            for (index=0; index<count; index++) {
                Marshal.StructureToPtr(new GPRECTF(rect[index]), (IntPtr)((long)memory+index*size), false);
            }

            return memory;
        }

        //----------------------------------------------------------------------------------------                                                           
        // Helper function:  Convert Rectangle[] to native memory block GpRect*
        //----------------------------------------------------------------------------------------
        internal static IntPtr ConvertRectangleToMemory(Rectangle[] rect) {
            if (rect == null)
                throw new ArgumentNullException("rectangle");

            int index;

            GPRECT rt = new GPRECT();

            int size =  (int)Marshal.SizeOf(rt.GetType());
            int count = rect.Length;

            IntPtr memory =  Marshal.AllocHGlobal(count*size);

            for (index=0; index<count; index++) {
                Marshal.StructureToPtr(new GPRECT(rect[index]), (IntPtr)((long)memory+index*size), false);
            }

            return memory;
        }
        
        public static IntPtr InvalidIntPtr = ((IntPtr)((int)(-1)));
        
        [StructLayout(LayoutKind.Sequential)]
        public class USEROBJECTFLAGS {
            public int fInherit = 0;
            public int fReserved = 0;
            public int dwFlags = 0;
        }

        public const int 
        UOI_FLAGS = 1,
        WSF_VISIBLE = 0x0001,
        E_UNEXPECTED = unchecked((int)0x8000FFFF),
        E_NOTIMPL = unchecked((int)0x80004001),
        E_OUTOFMEMORY = unchecked((int)0x8007000E),
        E_INVALIDARG = unchecked((int)0x80070057),
        E_NOINTERFACE = unchecked((int)0x80004002),
        E_POINTER = unchecked((int)0x80004003),
        E_HANDLE = unchecked((int)0x80070006),
        E_ABORT = unchecked((int)0x80004004),
        E_FAIL = unchecked((int)0x80004005),
        E_ACCESSDENIED = unchecked((int)0x80070005),
        PM_NOREMOVE = 0x0000,
        PM_REMOVE = 0x0001,
        PM_NOYIELD = 0x0002,
        GMEM_FIXED = 0x0000,
        GMEM_MOVEABLE = 0x0002,
        GMEM_NOCOMPACT = 0x0010,
        GMEM_NODISCARD = 0x0020,
        GMEM_ZEROINIT = 0x0040,
        GMEM_MODIFY = 0x0080,
        GMEM_DISCARDABLE = 0x0100,
        GMEM_NOT_BANKED = 0x1000,
        GMEM_SHARE = 0x2000,
        GMEM_DDESHARE = 0x2000,
        GMEM_NOTIFY = 0x4000,
        GMEM_LOWER = 0x1000,
        GMEM_VALID_FLAGS = 0x7F72,
        GMEM_INVALID_HANDLE = unchecked((int)0x8000),
        DM_UPDATE = 1,
        DM_COPY = 2,
        DM_PROMPT = 4,
        DM_MODIFY = 8,
        DM_IN_BUFFER = 8,
        DM_IN_PROMPT = 4,
        DM_OUT_BUFFER = 2,
        DM_OUT_DEFAULT = 1,
        DT_PLOTTER = 0,
        DT_RASDISPLAY = 1,
        DT_RASPRINTER = 2,
        DT_RASCAMERA = 3,
        DT_CHARSTREAM = 4,
        DT_METAFILE = 5,
        DT_DISPFILE = 6,
        TECHNOLOGY = 2,
        DC_FIELDS = 1,
        DC_PAPERS = 2,
        DC_PAPERSIZE = 3,
        DC_MINEXTENT = 4,
        DC_MAXEXTENT = 5,
        DC_BINS = 6,
        DC_DUPLEX = 7,
        DC_SIZE = 8,
        DC_EXTRA = 9,
        DC_VERSION = 10,
        DC_DRIVER = 11,
        DC_BINNAMES = 12,
        DC_ENUMRESOLUTIONS = 13,
        DC_FILEDEPENDENCIES = 14,
        DC_TRUETYPE = 15,
        DC_PAPERNAMES = 16,
        DC_ORIENTATION = 17,
        DC_COPIES = 18,
        PD_ALLPAGES = 0x00000000,
        PD_SELECTION = 0x00000001,
        PD_PAGENUMS = 0x00000002,
        PD_CURRENTPAGE = 0x00400000,
        PD_NOSELECTION = 0x00000004,
        PD_NOPAGENUMS = 0x00000008,
        PD_NOCURRENTPAGE = 0x00800000,
        PD_COLLATE = 0x00000010,
        PD_PRINTTOFILE = 0x00000020,
        PD_PRINTSETUP = 0x00000040,
        PD_NOWARNING = 0x00000080,
        PD_RETURNDC = 0x00000100,
        PD_RETURNIC = 0x00000200,
        PD_RETURNDEFAULT = 0x00000400,
        PD_SHOWHELP = 0x00000800,
        PD_ENABLEPRINTHOOK = 0x00001000,
        PD_ENABLESETUPHOOK = 0x00002000,
        PD_ENABLEPRINTTEMPLATE = 0x00004000,
        PD_ENABLESETUPTEMPLATE = 0x00008000,
        PD_ENABLEPRINTTEMPLATEHANDLE = 0x00010000,
        PD_ENABLESETUPTEMPLATEHANDLE = 0x00020000,
        PD_USEDEVMODECOPIES = 0x00040000,
        PD_USEDEVMODECOPIESANDCOLLATE = 0x00040000,
        PD_DISABLEPRINTTOFILE = 0x00080000,
        PD_HIDEPRINTTOFILE = 0x00100000,
        PD_NONETWORKBUTTON = 0x00200000,
        DI_MASK = 0x0001,
        DI_IMAGE = 0x0002,
        DI_NORMAL = 0x0003,
        DI_COMPAT = 0x0004,
        DI_DEFAULTSIZE = 0x0008,
        IDC_ARROW = 32512,
        IDC_IBEAM = 32513,
        IDC_WAIT = 32514,
        IDC_CROSS = 32515,
        IDC_UPARROW = 32516,
        IDC_SIZE = 32640,
        IDC_ICON = 32641,
        IDC_SIZENWSE = 32642,
        IDC_SIZENESW = 32643,
        IDC_SIZEWE = 32644,
        IDC_SIZENS = 32645,
        IDC_SIZEALL = 32646,
        IDC_NO = 32648,
        IDC_APPSTARTING = 32650,
        IDC_HELP = 32651,
        IMAGE_BITMAP = 0,
        IMAGE_ICON = 1,
        IMAGE_CURSOR = 2,
        IMAGE_ENHMETAFILE = 3,
        IDI_APPLICATION = 32512,
        IDI_HAND = 32513,
        IDI_QUESTION = 32514,
        IDI_EXCLAMATION = 32515,
        IDI_ASTERISK = 32516,
        IDI_WINLOGO = 32517,
        IDI_WARNING = 32515,
        IDI_ERROR = 32513,
        IDI_INFORMATION = 32516,
        SRCCOPY = 0x00CC0020,
        PLANES = 14,
        PS_SOLID = 0,
        PS_DASH = 1,
        PS_DOT = 2,
        PS_DASHDOT = 3,
        PS_DASHDOTDOT = 4,
        PS_NULL = 5,
        PS_INSIDEFRAME = 6,
        PS_USERSTYLE = 7,
        PS_ALTERNATE = 8,
        PS_STYLE_MASK = 0x0000000F,
        PS_ENDCAP_ROUND = 0x00000000,
        PS_ENDCAP_SQUARE = 0x00000100,
        PS_ENDCAP_FLAT = 0x00000200,
        PS_ENDCAP_MASK = 0x00000F00,
        PS_JOIN_ROUND = 0x00000000,
        PS_JOIN_BEVEL = 0x00001000,
        PS_JOIN_MITER = 0x00002000,
        PS_JOIN_MASK = 0x0000F000,
        PS_COSMETIC = 0x00000000,
        PS_GEOMETRIC = 0x00010000,
        PS_TYPE_MASK = 0x000F0000,
        BITSPIXEL = 12,
        ALTERNATE = 1,
        LOGPIXELSX = 88,
        LOGPIXELSY = 90,
        PHYSICALWIDTH = 110,
        PHYSICALHEIGHT = 111,
        PHYSICALOFFSETX = 112,
        PHYSICALOFFSETY = 113,
        WINDING = 2,
        VERTRES = 10,
        HORZRES = 8,
        DM_SPECVERSION = 0x0401,
        DM_ORIENTATION = 0x00000001,
        DM_PAPERSIZE = 0x00000002,
        DM_PAPERLENGTH = 0x00000004,
        DM_PAPERWIDTH = 0x00000008,
        DM_SCALE = 0x00000010,
        DM_COPIES = 0x00000100,
        DM_DEFAULTSOURCE = 0x00000200,
        DM_PRINTQUALITY = 0x00000400,
        DM_COLOR = 0x00000800,
        DM_DUPLEX = 0x00001000,
        DM_YRESOLUTION = 0x00002000,
        DM_TTOPTION = 0x00004000,
        DM_COLLATE = 0x00008000,
        DM_FORMNAME = 0x00010000,
        DM_LOGPIXELS = 0x00020000,
        DM_BITSPERPEL = 0x00040000,
        DM_PELSWIDTH = 0x00080000,
        DM_PELSHEIGHT = 0x00100000,
        DM_DISPLAYFLAGS = 0x00200000,
        DM_DISPLAYFREQUENCY = 0x00400000,
        DM_PANNINGWIDTH = 0x00800000,
        DM_PANNINGHEIGHT = 0x01000000,
        DM_ICMMETHOD = 0x02000000,
        DM_ICMINTENT = 0x04000000,
        DM_MEDIATYPE = 0x08000000,
        DM_DITHERTYPE = 0x10000000,
        DM_ICCMANUFACTURER = 0x20000000,
        DM_ICCMODEL = 0x40000000,
        DMORIENT_PORTRAIT = 1,
        DMORIENT_LANDSCAPE = 2,
        DMPAPER_LETTER = 1,
        DMPAPER_LETTERSMALL = 2,
        DMPAPER_TABLOID = 3,
        DMPAPER_LEDGER = 4,
        DMPAPER_LEGAL = 5,
        DMPAPER_STATEMENT = 6,
        DMPAPER_EXECUTIVE = 7,
        DMPAPER_A3 = 8,
        DMPAPER_A4 = 9,
        DMPAPER_A4SMALL = 10,
        DMPAPER_A5 = 11,
        DMPAPER_B4 = 12,
        DMPAPER_B5 = 13,
        DMPAPER_FOLIO = 14,
        DMPAPER_QUARTO = 15,
        DMPAPER_10X14 = 16,
        DMPAPER_11X17 = 17,
        DMPAPER_NOTE = 18,
        DMPAPER_ENV_9 = 19,
        DMPAPER_ENV_10 = 20,
        DMPAPER_ENV_11 = 21,
        DMPAPER_ENV_12 = 22,
        DMPAPER_ENV_14 = 23,
        DMPAPER_CSHEET = 24,
        DMPAPER_DSHEET = 25,
        DMPAPER_ESHEET = 26,
        DMPAPER_ENV_DL = 27,
        DMPAPER_ENV_C5 = 28,
        DMPAPER_ENV_C3 = 29,
        DMPAPER_ENV_C4 = 30,
        DMPAPER_ENV_C6 = 31,
        DMPAPER_ENV_C65 = 32,
        DMPAPER_ENV_B4 = 33,
        DMPAPER_ENV_B5 = 34,
        DMPAPER_ENV_B6 = 35,
        DMPAPER_ENV_ITALY = 36,
        DMPAPER_ENV_MONARCH = 37,
        DMPAPER_ENV_PERSONAL = 38,
        DMPAPER_FANFOLD_US = 39,
        DMPAPER_FANFOLD_STD_GERMAN = 40,
        DMPAPER_FANFOLD_LGL_GERMAN = 41,
        DMPAPER_ISO_B4 = 42,
        DMPAPER_JAPANESE_POSTCARD = 43,
        DMPAPER_9X11 = 44,
        DMPAPER_10X11 = 45,
        DMPAPER_15X11 = 46,
        DMPAPER_ENV_INVITE = 47,
        DMPAPER_RESERVED_48 = 48,
        DMPAPER_RESERVED_49 = 49,
        DMPAPER_LETTER_EXTRA = 50,
        DMPAPER_LEGAL_EXTRA = 51,
        DMPAPER_TABLOID_EXTRA = 52,
        DMPAPER_A4_EXTRA = 53,
        DMPAPER_LETTER_TRANSVERSE = 54,
        DMPAPER_A4_TRANSVERSE = 55,
        DMPAPER_LETTER_EXTRA_TRANSVERSE = 56,
        DMPAPER_A_PLUS = 57,
        DMPAPER_B_PLUS = 58,
        DMPAPER_LETTER_PLUS = 59,
        DMPAPER_A4_PLUS = 60,
        DMPAPER_A5_TRANSVERSE = 61,
        DMPAPER_B5_TRANSVERSE = 62,
        DMPAPER_A3_EXTRA = 63,
        DMPAPER_A5_EXTRA = 64,
        DMPAPER_B5_EXTRA = 65,
        DMPAPER_A2 = 66,
        DMPAPER_A3_TRANSVERSE = 67,
        DMPAPER_A3_EXTRA_TRANSVERSE = 68,

        // WINVER >= 0x0500
        DMPAPER_DBL_JAPANESE_POSTCARD = 69, /* Japanese Double Postcard 200 x 148 mm */
        DMPAPER_A6 =                  70,  /* A6 105 x 148 mm                 */
        DMPAPER_JENV_KAKU2 =          71,  /* Japanese Envelope Kaku #2       */
        DMPAPER_JENV_KAKU3 =          72,  /* Japanese Envelope Kaku #3       */
        DMPAPER_JENV_CHOU3 =          73,  /* Japanese Envelope Chou #3       */
        DMPAPER_JENV_CHOU4 =          74,  /* Japanese Envelope Chou #4       */
        DMPAPER_LETTER_ROTATED =      75,  /* Letter Rotated 11 x 8 1/2 11 in */
        DMPAPER_A3_ROTATED =          76,  /* A3 Rotated 420 x 297 mm         */
        DMPAPER_A4_ROTATED =          77,  /* A4 Rotated 297 x 210 mm         */
        DMPAPER_A5_ROTATED =          78,  /* A5 Rotated 210 x 148 mm         */
        DMPAPER_B4_JIS_ROTATED =      79,  /* B4 (JIS) Rotated 364 x 257 mm   */
        DMPAPER_B5_JIS_ROTATED =      80,  /* B5 (JIS) Rotated 257 x 182 mm   */
        DMPAPER_JAPANESE_POSTCARD_ROTATED = 81, /* Japanese Postcard Rotated 148 x 100 mm */
        DMPAPER_DBL_JAPANESE_POSTCARD_ROTATED = 82, /* Double Japanese Postcard Rotated 148 x 200 mm */
        DMPAPER_A6_ROTATED =          83,  /* A6 Rotated 148 x 105 mm         */
        DMPAPER_JENV_KAKU2_ROTATED =  84,  /* Japanese Envelope Kaku #2 Rotated */
        DMPAPER_JENV_KAKU3_ROTATED =  85,  /* Japanese Envelope Kaku #3 Rotated */
        DMPAPER_JENV_CHOU3_ROTATED =  86,  /* Japanese Envelope Chou #3 Rotated */
        DMPAPER_JENV_CHOU4_ROTATED =  87,  /* Japanese Envelope Chou #4 Rotated */
        DMPAPER_B6_JIS =              88,  /* B6 (JIS) 128 x 182 mm           */
        DMPAPER_B6_JIS_ROTATED =      89,  /* B6 (JIS) Rotated 182 x 128 mm   */
        DMPAPER_12X11 =               90,  /* 12 x 11 in                      */
        DMPAPER_JENV_YOU4 =           91,  /* Japanese Envelope You #4        */
        DMPAPER_JENV_YOU4_ROTATED =   92,  /* Japanese Envelope You #4 Rotated*/
        DMPAPER_P16K =                93,  /* PRC 16K 146 x 215 mm            */
        DMPAPER_P32K =                94,  /* PRC 32K 97 x 151 mm             */
        DMPAPER_P32KBIG =             95,  /* PRC 32K(Big) 97 x 151 mm        */
        DMPAPER_PENV_1 =              96,  /* PRC Envelope #1 102 x 165 mm    */
        DMPAPER_PENV_2 =              97,  /* PRC Envelope #2 102 x 176 mm    */
        DMPAPER_PENV_3 =              98,  /* PRC Envelope #3 125 x 176 mm    */
        DMPAPER_PENV_4 =              99,  /* PRC Envelope #4 110 x 208 mm    */
        DMPAPER_PENV_5 =              100, /* PRC Envelope #5 110 x 220 mm    */
        DMPAPER_PENV_6 =              101, /* PRC Envelope #6 120 x 230 mm    */
        DMPAPER_PENV_7 =              102, /* PRC Envelope #7 160 x 230 mm    */
        DMPAPER_PENV_8 =              103, /* PRC Envelope #8 120 x 309 mm    */
        DMPAPER_PENV_9 =              104, /* PRC Envelope #9 229 x 324 mm    */
        DMPAPER_PENV_10 =             105, /* PRC Envelope #10 324 x 458 mm   */
        DMPAPER_P16K_ROTATED =        106, /* PRC 16K Rotated                 */
        DMPAPER_P32K_ROTATED =        107, /* PRC 32K Rotated                 */
        DMPAPER_P32KBIG_ROTATED =     108, /* PRC 32K(Big) Rotated            */
        DMPAPER_PENV_1_ROTATED =      109, /* PRC Envelope #1 Rotated 165 x 102 mm */
        DMPAPER_PENV_2_ROTATED =      110, /* PRC Envelope #2 Rotated 176 x 102 mm */
        DMPAPER_PENV_3_ROTATED =      111, /* PRC Envelope #3 Rotated 176 x 125 mm */
        DMPAPER_PENV_4_ROTATED =      112, /* PRC Envelope #4 Rotated 208 x 110 mm */
        DMPAPER_PENV_5_ROTATED =      113, /* PRC Envelope #5 Rotated 220 x 110 mm */
        DMPAPER_PENV_6_ROTATED =      114, /* PRC Envelope #6 Rotated 230 x 120 mm */
        DMPAPER_PENV_7_ROTATED =      115, /* PRC Envelope #7 Rotated 230 x 160 mm */
        DMPAPER_PENV_8_ROTATED =      116, /* PRC Envelope #8 Rotated 309 x 120 mm */
        DMPAPER_PENV_9_ROTATED =      117, /* PRC Envelope #9 Rotated 324 x 229 mm */
        DMPAPER_PENV_10_ROTATED =     118, /* PRC Envelope #10 Rotated 458 x 324 mm */

        DMPAPER_LAST = DMPAPER_PENV_10_ROTATED,
        DMPAPER_USER = 256,

        DMBIN_UPPER = 1,
        DMBIN_ONLYONE = 1,
        DMBIN_LOWER = 2,
        DMBIN_MIDDLE = 3,
        DMBIN_MANUAL = 4,
        DMBIN_ENVELOPE = 5,
        DMBIN_ENVMANUAL = 6,
        DMBIN_AUTO = 7,
        DMBIN_TRACTOR = 8,
        DMBIN_SMALLFMT = 9,
        DMBIN_LARGEFMT = 10,
        DMBIN_LARGECAPACITY = 11,
        DMBIN_CASSETTE = 14,
        DMBIN_FORMSOURCE = 15,
        DMBIN_LAST = 15,
        DMBIN_USER = 256,
        DMRES_DRAFT = -1,
        DMRES_LOW = -2,
        DMRES_MEDIUM = -3,
        DMRES_HIGH = -4,
        DMCOLOR_MONOCHROME = 1,
        DMCOLOR_COLOR = 2,
        DMDUP_SIMPLEX = 1,
        DMDUP_VERTICAL = 2,
        DMDUP_HORIZONTAL = 3,
        DMTT_BITMAP = 1,
        DMTT_DOWNLOAD = 2,
        DMTT_SUBDEV = 3,
        DMTT_DOWNLOAD_OUTLINE = 4,
        DMCOLLATE_FALSE = 0,
        DMCOLLATE_TRUE = 1,
        DMDISPLAYFLAGS_TEXTMODE = 0x00000004,
        DMICMMETHOD_NONE = 1,
        DMICMMETHOD_SYSTEM = 2,
        DMICMMETHOD_DRIVER = 3,
        DMICMMETHOD_DEVICE = 4,
        DMICMMETHOD_USER = 256,
        DMICM_SATURATE = 1,
        DMICM_CONTRAST = 2,
        DMICM_COLORMETRIC = 3,
        DMICM_USER = 256,
        DMMEDIA_STANDARD = 1,
        DMMEDIA_TRANSPARENCY = 2,
        DMMEDIA_GLOSSY = 3,
        DMMEDIA_USER = 256,
        DMDITHER_NONE = 1,
        DMDITHER_COARSE = 2,
        DMDITHER_FINE = 3,
        DMDITHER_LINEART = 4,
        DMDITHER_GRAYSCALE = 5,
        DMDITHER_USER = 256,
        PRINTER_ENUM_DEFAULT = 0x00000001,
        PRINTER_ENUM_LOCAL = 0x00000002,
        PRINTER_ENUM_CONNECTIONS = 0x00000004,
        PRINTER_ENUM_FAVORITE = 0x00000004,
        PRINTER_ENUM_NAME = 0x00000008,
        PRINTER_ENUM_REMOTE = 0x00000010,
        PRINTER_ENUM_SHARED = 0x00000020,
        PRINTER_ENUM_NETWORK = 0x00000040,
        PRINTER_ENUM_EXPAND = 0x00004000,
        PRINTER_ENUM_CONTAINER = 0x00008000,
        PRINTER_ENUM_ICONMASK = 0x00ff0000,
        PRINTER_ENUM_ICON1 = 0x00010000,
        PRINTER_ENUM_ICON2 = 0x00020000,
        PRINTER_ENUM_ICON3 = 0x00040000,
        PRINTER_ENUM_ICON4 = 0x00080000,
        PRINTER_ENUM_ICON5 = 0x00100000,
        PRINTER_ENUM_ICON6 = 0x00200000,
        PRINTER_ENUM_ICON7 = 0x00400000,
        PRINTER_ENUM_ICON8 = 0x00800000,
        DC_BINADJUST = 19,
        DC_EMF_COMPLIANT = 20,
        DC_DATATYPE_PRODUCED = 21,
        DC_COLLATE = 22,
        DCTT_BITMAP = 0x0000001,
        DCTT_DOWNLOAD = 0x0000002,
        DCTT_SUBDEV = 0x0000004,
        DCTT_DOWNLOAD_OUTLINE = 0x0000008,
        DCBA_FACEUPNONE = 0x0000,
        DCBA_FACEUPCENTER = 0x0001,
        DCBA_FACEUPLEFT = 0x0002,
        DCBA_FACEUPRIGHT = 0x0003,
        DCBA_FACEDOWNNONE = 0x0100,
        DCBA_FACEDOWNCENTER = 0x0101,
        DCBA_FACEDOWNLEFT = 0x0102,
        DCBA_FACEDOWNRIGHT = 0x0103,
        SM_CXSCREEN = 0,
        SM_CYSCREEN = 1,
        SM_CXVSCROLL = 2,
        SM_CYHSCROLL = 3,
        SM_CYCAPTION = 4,
        SM_CXBORDER = 5,
        SM_CYBORDER = 6,
        SM_CXDLGFRAME = 7,
        SM_CYDLGFRAME = 8,
        SM_CYVTHUMB = 9,
        SM_CXHTHUMB = 10,
        SM_CXICON = 11,
        SM_CYICON = 12,
        SM_CXCURSOR = 13,
        SM_CYCURSOR = 14,
        SM_CYMENU = 15,
        SM_CXFULLSCREEN = 16,
        SM_CYFULLSCREEN = 17,
        SM_CYKANJIWINDOW = 18,
        SM_MOUSEPRESENT = 19,
        SM_CYVSCROLL = 20,
        SM_CXHSCROLL = 21,
        SM_DEBUG = 22,
        SM_SWAPBUTTON = 23,
        SM_RESERVED1 = 24,
        SM_RESERVED2 = 25,
        SM_RESERVED3 = 26,
        SM_RESERVED4 = 27,
        SM_CXMIN = 28,
        SM_CYMIN = 29,
        SM_CXSIZE = 30,
        SM_CYSIZE = 31,
        SM_CXFRAME = 32,
        SM_CYFRAME = 33,
        SM_CXMINTRACK = 34,
        SM_CYMINTRACK = 35,
        SM_CXDOUBLECLK = 36,
        SM_CYDOUBLECLK = 37,
        SM_CXICONSPACING = 38,
        SM_CYICONSPACING = 39,
        SM_MENUDROPALIGNMENT = 40,
        SM_PENWINDOWS = 41,
        SM_DBCSENABLED = 42,
        SM_CMOUSEBUTTONS = 43,
        SM_CXFIXEDFRAME = 7,
        SM_CYFIXEDFRAME = 8,
        SM_CXSIZEFRAME = 32,
        SM_CYSIZEFRAME = 33,
        SM_SECURE = 44,
        SM_CXEDGE = 45,
        SM_CYEDGE = 46,
        SM_CXMINSPACING = 47,
        SM_CYMINSPACING = 48,
        SM_CXSMICON = 49,
        SM_CYSMICON = 50,
        SM_CYSMCAPTION = 51,
        SM_CXSMSIZE = 52,
        SM_CYSMSIZE = 53,
        SM_CXMENUSIZE = 54,
        SM_CYMENUSIZE = 55,
        SM_ARRANGE = 56,
        SM_CXMINIMIZED = 57,
        SM_CYMINIMIZED = 58,
        SM_CXMAXTRACK = 59,
        SM_CYMAXTRACK = 60,
        SM_CXMAXIMIZED = 61,
        SM_CYMAXIMIZED = 62,
        SM_NETWORK = 63,
        SM_CLEANBOOT = 67,
        SM_CXDRAG = 68,
        SM_CYDRAG = 69,
        SM_SHOWSOUNDS = 70,
        SM_CXMENUCHECK = 71,
        SM_CYMENUCHECK = 72,
        SM_SLOWMACHINE = 73,
        SM_MIDEASTENABLED = 74,
        SM_MOUSEWHEELPRESENT = 75,
        SM_XVIRTUALSCREEN = 76,
        SM_YVIRTUALSCREEN = 77,
        SM_CXVIRTUALSCREEN = 78,
        SM_CYVIRTUALSCREEN = 79,
        SM_CMONITORS = 80,
        SM_SAMEDISPLAYFORMAT = 81,
        SM_CMETRICS = 83;

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern IntPtr GlobalFree(HandleRef handle);
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int StartDoc(HandleRef hDC, DOCINFO lpDocInfo);
        [DllImport(ExternDll.Gdi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int StartPage(HandleRef hDC);
        [DllImport(ExternDll.Gdi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int EndPage(HandleRef hDC);
        //      public static extern int SetAbortProc(m_hDC, (ABORTPROC)lpfn);
        [DllImport(ExternDll.Gdi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int AbortDoc(HandleRef hDC);
        [DllImport(ExternDll.Gdi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int EndDoc(HandleRef hDC);

        [DllImport(ExternDll.Comdlg32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern bool PrintDlg([In, Out] PRINTDLG lppd);
        
        [DllImport(ExternDll.Comdlg32, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int PageSetupDlg([In, Out] PAGESETUPDLG lppsd);

        [DllImport(ExternDll.Winspool, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int DeviceCapabilities(string pDevice, string pPort, short fwCapabilities, IntPtr pOutput, IntPtr /*DEVMODE*/ pDevMode);

        [DllImport(ExternDll.Winspool, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int DocumentProperties(HandleRef hwnd, HandleRef hPrinter, string pDeviceName, IntPtr /*DEVMODE*/ pDevModeOutput, HandleRef /*DEVMODE*/ pDevModeInput, int fMode);

        [DllImport(ExternDll.Winspool, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int GetPrinter(HandleRef hPrinter, int level, HandleRef pPrinter, int cbBuf, int[] pcbNeeded);

        [DllImport(ExternDll.Winspool, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern bool OpenPrinter(string pPrinterName, HandleRef [] phPrinter, HandleRef pDefault);
        [DllImport(ExternDll.Winspool, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int ClosePrinter(HandleRef hPrinter);

        [DllImport(ExternDll.Winspool, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public static extern int EnumPrinters(int flags, string name, int level, IntPtr pPrinterEnum/*buffer*/,
                                              int cbBuf, out int pcbNeeded, out int pcReturned);

        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern IntPtr GlobalLock(HandleRef handle);
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern IntPtr /*HDC*/ ResetDC(HandleRef hDC, HandleRef /*DEVMODE*/ lpDevMode);
        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern bool GlobalUnlock(HandleRef handle);
        [DllImport(ExternDll.Gdi32, ExactSpelling=true, EntryPoint="DeleteDC", CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        private static extern bool IntDeleteDC(HandleRef hDC);
        public static bool DeleteDC(HandleRef hDC) {
            // GDI or HDC handle?  If from bitmap, GDI, but if printer, HDC?
            HandleCollector.Remove((IntPtr)hDC, CommonHandles.GDI);
            return IntDeleteDC(hDC);
        }

        [DllImport(ExternDll.Gdi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int IntersectClipRect(HandleRef hDC, int x1, int y1, int x2, int y2);
        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern IntPtr GlobalAlloc(int uFlags, int dwBytes);
        [DllImport(ExternDll.Kernel32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public extern static int GetLastError();
        
        public const int ERROR_ACCESS_DENIED = 5;
        public const int ERROR_PROC_NOT_FOUND = 127;


        [System.Runtime.InteropServices.ComVisible(false), System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
        public class ENHMETAHEADER {
            public int  iType;
            public int  nSize = 40; // ndirect.DllLib.sizeOf( this )
            // rclBounds was a by-value RECTL structure
            public int  rclBounds_left;
            public int  rclBounds_top;
            public int  rclBounds_right;
            public int  rclBounds_bottom;
            // rclFrame was a by-value RECTL structure
            public int  rclFrame_left;
            public int  rclFrame_top;
            public int  rclFrame_right;
            public int  rclFrame_bottom;
            public int  dSignature;
            public int  nVersion;
            public int  nBytes;
            public int  nRecords;
            public short nHandles;
            public short sReserved;
            public int  nDescription;
            public int  offDescription;
            public int  nPalEntries;
            // szlDevice was a by-value SIZE structure
            public int  szlDevice_cx;
            public int  szlDevice_cy;
            // szlMillimeters was a by-value SIZE structure
            public int  szlMillimeters_cx;
            public int  szlMillimeters_cy;
            public int  cbPixelFormat;
            public int  offPixelFormat;
            public int  bOpenGL;
        }

        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
        public class DOCINFO {
                public int cbSize = 20; //ndirect.DllLib.sizeOf(this);
                public String lpszDocName;
                public String lpszOutput;
                public String lpszDatatype;
                public int fwType;
        }

        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto, Pack=1)]
        public class PRINTDLG {
            public   int lStructSize;
            public   IntPtr hwndOwner;
            public   IntPtr hDevMode;
            public   IntPtr hDevNames;
            public   IntPtr hDC;
            public   int Flags;
            public   short nFromPage;
            public   short nToPage;
            public   short nMinPage;
            public   short nMaxPage;
            public   short nCopies;
            public   IntPtr hInstance;
            public   IntPtr lCustData;
            public   WndProc lpfnPrintHook;
            public   WndProc lpfnSetupHook;
            public   string lpPrintTemplateName;
            public   string lpSetupTemplateName;
            public   IntPtr hPrintTemplate;
            public   IntPtr hSetupTemplate;
        }

        public delegate int WndProc(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam);
        
        [System.Runtime.InteropServices.ComVisible(false)]
        public enum StructFormat {
            Ansi = 1,
            Unicode = 2,
            Auto = 3,
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential)]
        public class PAGESETUPDLG {
            public int      lStructSize; 
            public IntPtr   hwndOwner; 
            public IntPtr   hDevMode; 
            public IntPtr   hDevNames; 
            public int      Flags; 

            //POINT           ptPaperSize; 
            public int      paperSizeX;
            public int      paperSizeY;

            // RECT            rtMinMargin; 
            public int minMarginLeft;
            public int minMarginTop;
            public int minMarginRight;
            public int minMarginBottom;

            // RECT            rtMargin; 
            public int marginLeft;
            public int marginTop;
            public int marginRight;
            public int marginBottom;

            public IntPtr   hInstance; 
            public IntPtr   lCustData; 
            public WndProc  lpfnPageSetupHook; 
            public WndProc  lpfnPagePaintHook; 
            public string   lpPageSetupTemplateName; 
            public IntPtr   hPageSetupTemplate; 
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct RECT {
            public int left;
            public int top;
            public int right;
            public int bottom;

            public RECT(int left, int top, int right, int bottom) {
                this.left = left;
                this.top = top;
                this.right = right;
                this.bottom = bottom;
            }

            public static RECT FromXYWH(int x, int y, int width, int height) {
                return new RECT(x,
                                y,
                                x + width,
                                y + height);
            }
        }
        [StructLayout(LayoutKind.Sequential)]
        public class COMRECT {
            public int left;
            public int top;
            public int right;
            public int bottom;

            public COMRECT() {
            }

            public COMRECT(int left, int top, int right, int bottom) {
                this.left = left;
                this.top = top;
                this.right = right;
                this.bottom = bottom;
            }

            public static COMRECT FromXYWH(int x, int y, int width, int height) {
                return new COMRECT(x,
                                y,
                                x + width,
                                y + height);
            }
        }
        [StructLayout(LayoutKind.Sequential)]
        public class POINT {
            public int x;
            public int y;

            public POINT() {
            }

            public POINT(int x, int y) {
                this.x = x;
                this.y = y;
            }
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct MSG {
            public IntPtr   hwnd;
            public int      message;
            public IntPtr   wParam;
            public IntPtr   lParam;
            public int      time;
            // pt was a by-value POINT structure
            public int      pt_x;
            public int      pt_y;
        }
        [StructLayout(LayoutKind.Sequential)]
        public class ICONINFO {
                public int fIcon;
                public int xHotspot;
                public int yHotspot;
                public IntPtr hbmMask;
                public IntPtr hbmColor;
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential)]
        public class BITMAP {
                public int bmType;
                public int bmWidth;
                public int bmHeight;
                public int bmWidthBytes;
                public short bmPlanes;
                public short bmBitsPixel;
                public IntPtr bmBits;
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential)]
        public class DIBSECTION {
            public BITMAP dsBm;
            public BITMAPINFOHEADER dsBmih;
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValArray, SizeConst=3)]
            public int[] dsBitfields;
            public IntPtr dshSection;
            public int dsOffset;
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential)]
        public class BITMAPINFOHEADER {
                public int      biSize = 40;    // ndirect.DllLib.sizeOf( this );
                public int      biWidth;
                public int      biHeight;
                public short    biPlanes;
                public short    biBitCount;
                public int      biCompression;
                public int      biSizeImage;
                public int      biXPelsPerMeter;
                public int      biYPelsPerMeter;
                public int      biClrUsed;
                public int      biClrImportant;
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential)]
        public class LOGPEN {
            public int  lopnStyle;
            // lopnWidth was a by-value POINT structure
            public int  lopnWidth_x;
            public int  lopnWidth_y;
            public int  lopnColor;
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential)]
        public class LOGBRUSH {
                public int lbStyle;
                public int lbColor;
                public int lbHatch;
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
        public class LOGFONT {
                public int lfHeight;
                public int lfWidth;
                public int lfEscapement;
                public int lfOrientation;
                public int lfWeight;
                public byte lfItalic;
                public byte lfUnderline;
                public byte lfStrikeOut;
                public byte lfCharSet;
                public byte lfOutPrecision;
                public byte lfClipPrecision;
                public byte lfQuality;
                public byte lfPitchAndFamily;
                [MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst=32)]
                public string   lfFaceName;

                public override string ToString() {
                    return 
                        "lfHeight=" + lfHeight + ", " +
                        "lfWidth=" + lfWidth + ", " +
                        "lfEscapement=" + lfEscapement + ", " +
                        "lfOrientation=" + lfOrientation + ", " +
                        "lfWeight=" + lfWeight + ", " +
                        "lfItalic=" + lfItalic + ", " +
                        "lfUnderline=" + lfUnderline + ", " +
                        "lfStrikeOut=" + lfStrikeOut + ", " +
                        "lfCharSet=" + lfCharSet + ", " +
                        "lfOutPrecision=" + lfOutPrecision + ", " +
                        "lfClipPrecision=" + lfClipPrecision + ", " +
                        "lfQuality=" + lfQuality + ", " +
                        "lfPitchAndFamily=" + lfPitchAndFamily + ", " +
                          "lfFaceName=" +   lfFaceName;
                }
        }
        
        [StructLayout(LayoutKind.Sequential, Pack=2)]
        public struct ICONDIR {
            public short idReserved;
            public short idType;
            public short idCount;
            public ICONDIRENTRY idEntries;
        }
        
        [StructLayout(LayoutKind.Sequential)]
        public struct ICONDIRENTRY {
            public byte bWidth;
            public byte bHeight;
            public byte bColorCount;
            public byte bReserved;
            public short wPlanes;
            public short wBitCount;
            public int dwBytesInRes;
            public int dwImageOffset;
        }
        
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential)]
        public class PICTDESC
        {
            internal int cbSizeOfStruct;
            public int picType;
            internal IntPtr union1;
            internal int union2;
            internal int union3;

            public static PICTDESC CreateBitmapPICTDESC(IntPtr hbitmap, IntPtr hpal) {
                PICTDESC pictdesc = new PICTDESC();
                pictdesc.cbSizeOfStruct = 16;
                pictdesc.picType = Ole.PICTYPE_BITMAP;
                pictdesc.union1 = hbitmap;
                pictdesc.union2 = (int)(((long)hpal) & 0xffffffff);
                pictdesc.union3 = (int)(((long)hpal) >> 32);
                return pictdesc;
            }

            public static PICTDESC CreateIconPICTDESC(IntPtr hicon) {
                PICTDESC pictdesc = new PICTDESC();
                pictdesc.cbSizeOfStruct = 12;
                pictdesc.picType = Ole.PICTYPE_ICON;
                pictdesc.union1 = hicon;
                return pictdesc;
            }

            public static PICTDESC CreateEnhMetafilePICTDESC(IntPtr hEMF) {
                PICTDESC pictdesc = new PICTDESC();
                pictdesc.cbSizeOfStruct = 12;
                pictdesc.picType = Ole.PICTYPE_ENHMETAFILE;
                pictdesc.union1 = hEMF;
                return pictdesc;
            }

            public static PICTDESC CreateWinMetafilePICTDESC(IntPtr hmetafile, int x, int y) {
                PICTDESC pictdesc = new PICTDESC();
                pictdesc.cbSizeOfStruct = 20;
                pictdesc.picType = Ole.PICTYPE_METAFILE;
                pictdesc.union1 = hmetafile;
                pictdesc.union2 = x;
                pictdesc.union3 = y;
                return pictdesc;
            }

            public virtual IntPtr GetHandle() {
                return union1;
            }

            public virtual IntPtr GetHPal() {
                Debug.Assert((union2 >= 0) && (union3 >= 0));

                long u2 = union2;
                long u3 = union3;
                if (picType == Ole.PICTYPE_BITMAP)
                    return (IntPtr)(u2 | (u3 << 32));
                    // REVIEW: This is what it used to be. Had to change it since the compiler did not like
                    //         the casts and the bitwise OR.
                    // return (IntPtr)(union2 | (((long)union3) << 32));
                else
                    return IntPtr.Zero;
            }
        }
        [System.Runtime.InteropServices.ComVisible(false)]
        public class Ole {
            /*
             * Pictypes
             */
            public const int PICTYPE_UNINITIALIZED = -1;
            public const int PICTYPE_NONE          =  0;
            public const int PICTYPE_BITMAP        =  1;
            public const int PICTYPE_METAFILE      =  2;
            public const int PICTYPE_ICON          =  3;
            public const int PICTYPE_ENHMETAFILE   =  4;

            public const int STATFLAG_DEFAULT = 0;
            public const int STATFLAG_NONAME = 1;
        }
        [System.Runtime.InteropServices.ComVisible(false), StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
        public class DEVMODE {
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst=32)]
            public string dmDeviceName;
            public short dmSpecVersion;
            public short dmDriverVersion;
            public short dmSize;
            public short dmDriverExtra;
            public int dmFields;
            public short dmOrientation;
            public short dmPaperSize;
            public short dmPaperLength;
            public short dmPaperWidth;
            public short dmScale;
            public short dmCopies;
            public short dmDefaultSource;
            public short dmPrintQuality;
            public short dmColor;
            public short dmDuplex;
            public short dmYResolution;
            public short dmTTOption;
            public short dmCollate;
            [MarshalAs(System.Runtime.InteropServices.UnmanagedType.ByValTStr, SizeConst=32)]
            public string dmFormName;
            public short dmLogPixels;
            public int dmBitsPerPel;
            public int dmPelsWidth;
            public int dmPelsHeight;
            public int dmDisplayFlags;
            public int dmDisplayFrequency;
            public int dmICMMethod;
            public int dmICMIntent;
            public int dmMediaType;
            public int dmDitherType;
            public int dmICCManufacturer;
            public int dmICCModel;
            public int dmPanningWidth;
            public int dmPanningHeight;


            public override string ToString() {
                return "[DEVMODE: "
                + "dmDeviceName=" + dmDeviceName
                + ", dmSpecVersion=" + dmSpecVersion
                + ", dmDriverVersion=" + dmDriverVersion
                + ", dmSize=" + dmSize
                + ", dmDriverExtra=" + dmDriverExtra
                + ", dmFields=" + dmFields
                + ", dmOrientation=" + dmOrientation
                + ", dmPaperSize=" + dmPaperSize
                + ", dmPaperLength=" + dmPaperLength
                + ", dmPaperWidth=" + dmPaperWidth
                + ", dmScale=" + dmScale
                + ", dmCopies=" + dmCopies
                + ", dmDefaultSource=" + dmDefaultSource
                + ", dmPrintQuality=" + dmPrintQuality
                + ", dmColor=" + dmColor
                + ", dmDuplex=" + dmDuplex
                + ", dmYResolution=" + dmYResolution
                + ", dmTTOption=" + dmTTOption
                + ", dmCollate=" + dmCollate
                + ", dmFormName=" + dmFormName
                + ", dmLogPixels=" + dmLogPixels
                + ", dmBitsPerPel=" + dmBitsPerPel
                + ", dmPelsWidth=" + dmPelsWidth
                + ", dmPelsHeight=" + dmPelsHeight
                + ", dmDisplayFlags=" + dmDisplayFlags
                + ", dmDisplayFrequency=" + dmDisplayFrequency
                + ", dmICMMethod=" + dmICMMethod
                + ", dmICMIntent=" + dmICMIntent
                + ", dmMediaType=" + dmMediaType
                + ", dmDitherType=" + dmDitherType
                + ", dmICCManufacturer=" + dmICCManufacturer
                + ", dmICCModel=" + dmICCModel
                + ", dmPanningWidth=" + dmPanningWidth
                + ", dmPanningHeight=" + dmPanningHeight
                + "]";
            }
        }

        public sealed class CommonHandles {
            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.Accelerator"]/*' />
            /// <devdoc>
            ///     Handle type for accelerator tables.
            /// </devdoc>
            public static readonly int Accelerator  = HandleCollector.RegisterType("Accelerator", 80, 50);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.Cursor"]/*' />
            /// <devdoc>
            ///     handle type for cursors.
            /// </devdoc>
            public static readonly int Cursor       = HandleCollector.RegisterType("Cursor", 20, 500);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.EMF"]/*' />
            /// <devdoc>
            ///     Handle type for enhanced metafiles.
            /// </devdoc>
            public static readonly int EMF          = HandleCollector.RegisterType("EnhancedMetaFile", 20, 500);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.Find"]/*' />
            /// <devdoc>
            ///     Handle type for file find handles.
            /// </devdoc>
            public static readonly int Find         = HandleCollector.RegisterType("Find", 0, 1000);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.GDI"]/*' />
            /// <devdoc>
            ///     Handle type for GDI objects.
            /// </devdoc>
            public static readonly int GDI          = HandleCollector.RegisterType("GDI", 90, 50);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.HDC"]/*' />
            /// <devdoc>
            ///     Handle type for HDC's that count against the Win98 limit of five DC's.  HDC's
            ///     which are not scarce, such as HDC's for bitmaps, are counted as GDIHANDLE's.
            /// </devdoc>
            public static readonly int HDC          = HandleCollector.RegisterType("HDC", 100, 2); // wait for 2 dc's before collecting

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.Icon"]/*' />
            /// <devdoc>
            ///     Handle type for icons.
            /// </devdoc>
            public static readonly int Icon         = HandleCollector.RegisterType("Icon", 20, 500);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.Kernel"]/*' />
            /// <devdoc>
            ///     Handle type for kernel objects.
            /// </devdoc>
            public static readonly int Kernel       = HandleCollector.RegisterType("Kernel", 0, 1000);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.Menu"]/*' />
            /// <devdoc>
            ///     Handle type for files.
            /// </devdoc>
            public static readonly int Menu         = HandleCollector.RegisterType("Menu", 30, 1000);

            /// <include file='doc\SafeNativeMethods.uex' path='docs/doc[@for="SafeNativeMethods.CommonHandles.Window"]/*' />
            /// <devdoc>
            ///     Handle type for windows.
            /// </devdoc>
            public static readonly int Window       = HandleCollector.RegisterType("Window", 5, 1000);
        }
        
        [System.Runtime.InteropServices.ComVisible(false)]
        public class StreamConsts {
            public const   int LOCK_WRITE = 0x1;
            public const   int LOCK_EXCLUSIVE = 0x2;
            public const   int LOCK_ONLYONCE = 0x4;
            public const   int STATFLAG_DEFAULT = 0x0;
            public const   int STATFLAG_NONAME = 0x1;
            public const   int STATFLAG_NOOPEN = 0x2;
            public const   int STGC_DEFAULT = 0x0;
            public const   int STGC_OVERWRITE = 0x1;
            public const   int STGC_ONLYIFCURRENT = 0x2;
            public const   int STGC_DANGEROUSLYCOMMITMERELYTODISKCACHE = 0x4;
            public const   int STREAM_SEEK_SET = 0x0;
            public const   int STREAM_SEEK_CUR = 0x1;
            public const   int STREAM_SEEK_END = 0x2;
        }
        [DllImport(ExternDll.Gdi32)]
        public static extern int DeleteObject(HandleRef hObject);

        [DllImport(ExternDll.User32, EntryPoint="CreateIconIndirect")]
        private static extern IntPtr IntCreateIconIndirect(SafeNativeMethods.ICONINFO piconinfo);
        public static IntPtr CreateIconIndirect(SafeNativeMethods.ICONINFO piconinfo) {
            return HandleCollector.Add(IntCreateIconIndirect(piconinfo), SafeNativeMethods.CommonHandles.Icon);
        }
        
        [DllImport(ExternDll.User32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto, SetLastError=true)]
        public unsafe static extern IntPtr CreateIconFromResourceEx(byte * pbIconBits, int cbIconBits, bool fIcon, int dwVersion, int csDesired, int cyDesired, int flags);
        
        [DllImport(ExternDll.User32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern IntPtr LoadIcon(HandleRef hInst, int iconId);
        [DllImport(ExternDll.User32, ExactSpelling=true, EntryPoint="DestroyIcon", CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        private static extern bool IntDestroyIcon(HandleRef hIcon);
        public static bool DestroyIcon(HandleRef hIcon) {
            HandleCollector.Remove((IntPtr)hIcon, SafeNativeMethods.CommonHandles.Icon);
            return IntDestroyIcon(hIcon);
        }
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern IntPtr CreateFontIndirect(SafeNativeMethods.LOGFONT lf);
        [DllImport(ExternDll.User32, ExactSpelling=true, EntryPoint="CopyImage", CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        private static extern IntPtr IntCopyImage(HandleRef hImage, int uType, int cxDesired, int cyDesired, int fuFlags);
        public static IntPtr CopyImage(HandleRef hImage, int uType, int cxDesired, int cyDesired, int fuFlags) {
            return HandleCollector.Add(IntCopyImage(hImage, uType, cxDesired, cyDesired, fuFlags), SafeNativeMethods.CommonHandles.GDI);
        }
        
        // GetObject stuff
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetObject(HandleRef hObject, int nSize, [In, Out] SafeNativeMethods.BITMAP bm);
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetObject(HandleRef hObject, int nSize, [In, Out] SafeNativeMethods.DIBSECTION ds);
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetObject(HandleRef hObject, int nSize, [In, Out] SafeNativeMethods.LOGPEN lp);
        public static int GetObject(HandleRef hObject, SafeNativeMethods.LOGPEN lp) {
            return GetObject(hObject, System.Runtime.InteropServices.Marshal.SizeOf(typeof(SafeNativeMethods.LOGPEN)), lp);
        }
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetObject(HandleRef hObject, int nSize, [In, Out] SafeNativeMethods.LOGBRUSH lb);
        public static int GetObject(HandleRef hObject, SafeNativeMethods.LOGBRUSH lb) {
            return GetObject(hObject, System.Runtime.InteropServices.Marshal.SizeOf(typeof(SafeNativeMethods.LOGBRUSH)), lb);
        }
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetObject(HandleRef hObject, int nSize, [In, Out] SafeNativeMethods.LOGFONT lf);
        public static int GetObject(HandleRef hObject, SafeNativeMethods.LOGFONT lp) {
            return GetObject(hObject, System.Runtime.InteropServices.Marshal.SizeOf(typeof(SafeNativeMethods.LOGFONT)), lp);
        }
        //HPALETTE
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetObject(HandleRef hObject, int nSize, ref int nEntries);
        [DllImport(ExternDll.Gdi32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetObject(HandleRef hObject, int nSize, int[] nEntries);
        [DllImport(ExternDll.User32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern bool GetIconInfo(HandleRef hIcon, [In, Out] SafeNativeMethods.ICONINFO info);
        [DllImport(ExternDll.User32, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int GetSysColor(int nIndex);
        [DllImport(ExternDll.User32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern bool DrawIcon(HandleRef hDC, int x, int y, HandleRef hIcon);
        [DllImport(ExternDll.Gdi32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern int SafeIntersectClipRect(HandleRef hDC, int x1, int y1, int x2, int y2);
        [DllImport(ExternDll.User32, ExactSpelling=true, CharSet=System.Runtime.InteropServices.CharSet.Auto)]
        public static extern bool DrawIconEx(HandleRef hDC, int x, int y, HandleRef hIcon, int width, int height, int iStepIfAniCursor, HandleRef hBrushFlickerFree, int diFlags);
        [DllImport(ExternDll.Oleaut32, PreserveSig=false)]
        public static extern IPicture OleLoadPicture(UnsafeNativeMethods.IStream pStream, int lSize, bool fRunmode, ref Guid refiid);
        
        [DllImport(ExternDll.Oleaut32, PreserveSig=false)]
        public static extern IPicture OleLoadPictureEx(UnsafeNativeMethods.IStream pStream, int lSize, bool fRunmode, ref Guid refiid, int width, int height, int dwFlags);
        
        
        #if CUSTOM_MARSHALING_ISTREAM
        [DllImport(ExternDll.Oleaut32, PreserveSig=false)]
        public static extern IPicture OleLoadPictureEx(
                                                        [return: MarshalAs(UnmanagedType.CustomMarshaler,MarshalType="StreamToIStreamMarshaler")] Stream pStream, 
                                                        int lSize, bool fRunmode, ref Guid refiid, int width, int height, int dwFlags);
                                                        
                                                        
        #endif
        [DllImport(ExternDll.Oleaut32, PreserveSig=false)]
        public static extern IPicture OleCreatePictureIndirect(SafeNativeMethods.PICTDESC pictdesc, [In]ref Guid refiid, bool fOwn);
    
        [
            ComImport(), 
            Guid("7BF80980-BF32-101A-8BBB-00AA00300CAB"), 
            InterfaceType(ComInterfaceType.InterfaceIsIUnknown),
            
        ]
        public interface IPicture {

            [SuppressUnmanagedCodeSecurity()]
             IntPtr GetHandle();

            [SuppressUnmanagedCodeSecurity()]
             IntPtr GetHPal();

            [return: MarshalAs(UnmanagedType.I2)]
            [SuppressUnmanagedCodeSecurity()]
             short GetPictureType();

            [SuppressUnmanagedCodeSecurity()]
             int GetWidth();

            [SuppressUnmanagedCodeSecurity()]
             int GetHeight();

            [SuppressUnmanagedCodeSecurity()]
             void Render();

            [SuppressUnmanagedCodeSecurity()]
             void SetHPal(
                    [In] 
                     IntPtr phpal);

            [SuppressUnmanagedCodeSecurity()]
             IntPtr GetCurDC();

            [SuppressUnmanagedCodeSecurity()]
             void SelectPicture(
                    [In] 
                     IntPtr hdcIn,
                    [Out, MarshalAs(UnmanagedType.LPArray)] 
                     int[] phdcOut,
                    [Out, MarshalAs(UnmanagedType.LPArray)] 
                     int[] phbmpOut);

            [return: MarshalAs(UnmanagedType.Bool)]
            [SuppressUnmanagedCodeSecurity()]
             bool GetKeepOriginalFormat();

            [SuppressUnmanagedCodeSecurity()]
             void SetKeepOriginalFormat(
                    [In, MarshalAs(UnmanagedType.Bool)] 
                     bool pfkeep);

            [SuppressUnmanagedCodeSecurity()]
             void PictureChanged();

            [SuppressUnmanagedCodeSecurity()]
             [System.Runtime.InteropServices.PreserveSig]
             int SaveAsFile(
                    [In, MarshalAs(UnmanagedType.Interface)] 
                     UnsafeNativeMethods.IStream pstm,
                    [In] 
                     int fSaveMemCopy,
                    [Out]
                     out int pcbSize);

            [SuppressUnmanagedCodeSecurity()]
             int GetAttributes();

            [SuppressUnmanagedCodeSecurity()]
             void SetHdc(
                    [In] 
                     IntPtr hdc);


        }

    }
}

