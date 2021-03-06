//------------------------------------------------------------------------------
// <copyright file="Command.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.Windows.Forms {

    using System.Diagnostics;

    using System;
    using System.ComponentModel;
    using System.Windows.Forms;
    using System.Drawing;
    
    using Microsoft.Win32;

    /// <include file='doc\Command.uex' path='docs/doc[@for="Command"]/*' />
    /// <internalonly/>
    internal class Command : WeakReference {

        private static Command[] cmds;
        private static int icmdTry;
        private const int idMin = 0x00100;
        private const int idLim = 0x10000;

        internal int id;

        public Command(ICommandExecutor target)
            : base(target, false) {
            AssignID(this);
        }

        public virtual int ID {
            get {
                return id;
            }
        }

        protected static void AssignID(Command cmd) {
            lock(typeof(Command)) {
                int icmd;

                if (null == cmds) {
                    cmds = new Command[20];
                    icmd = 0;
                }
                else {
                    Debug.Assert(cmds.Length > 0, "why is cmds.Length zero?");
                    Debug.Assert(icmdTry >= 0, "why is icmdTry negative?");

                    int icmdLim = cmds.Length;

                    if (icmdTry >= icmdLim)
                        icmdTry = 0;

                    // First look for an empty slot (starting at icmdTry).
                    for (icmd = icmdTry; icmd < icmdLim; icmd++)
                        if (null == cmds[icmd]) goto FindSlotComplete;
                    for (icmd = 0; icmd < icmdTry; icmd++)
                        if (null == cmds[icmd]) goto FindSlotComplete;

                    // All slots have Command objects in them. Look for a command
                    // with a null referent.
                    for (icmd = 0; icmd < icmdLim; icmd++)
                        if (null == cmds[icmd].Target) goto FindSlotComplete;

                    // Grow the array.
                    icmd = cmds.Length;
                    icmdLim = Math.Min(idLim - idMin, 2 * icmd);

                    if (icmdLim <= icmd) {
                        // Already at maximal size. Do a garbage collect and look again.
                        GC.Collect();
                        for (icmd = 0; icmd < icmdLim; icmd++) {
                            if (null == cmds[icmd] || null == cmds[icmd].Target)
                                goto FindSlotComplete;
                        }
                        throw new ArgumentException(SR.GetString(SR.CommandIdNotAllocated));
                    }
                    else {
                        Command[] newCmds = new Command[icmdLim];
                        Array.Copy(cmds, 0, newCmds, 0, icmd);
                        cmds = newCmds;
                    }
                }

FindSlotComplete:

                cmd.id = icmd + idMin;
                Debug.Assert(cmd.id >= idMin && cmd.id < idLim, "generated command id out of range");

                cmds[icmd] = cmd;
                icmdTry = icmd + 1;
            }
        }

        public static bool DispatchID(int id) {
            Command cmd = GetCommandFromID(id);
            if (null == cmd)
                return false;
            return cmd.Invoke();
        }

        protected static void Dispose(Command cmd) {
            lock(typeof(Command)) {
                if (cmd.id >= idMin) {
                    cmd.Target = null;
                    if (cmds[cmd.id - idMin] == cmd)
                        cmds[cmd.id - idMin] = null;
                    cmd.id = 0;
                }
            }
        }

        public virtual void Dispose() {
            if (id >= idMin)
                Dispose(this);
        }

        public static Command GetCommandFromID(int id) {
            lock(typeof(Command)) {
                if (null == cmds)
                    return null;
                int i = id - idMin;
                if (i < 0 || i >= cmds.Length)
                    return null;
                return cmds[i];
            }
        }

        public virtual bool Invoke() {
            object target = Target;
            if (!(target is ICommandExecutor))
                return false;
            ((ICommandExecutor)target).Execute();
            return true;
        }
    }
}
