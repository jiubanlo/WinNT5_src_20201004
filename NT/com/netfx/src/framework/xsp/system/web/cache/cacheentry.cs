//------------------------------------------------------------------------------
// <copyright file="CacheEntry.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 * CacheEntry
 * 
 * Copyright (c) 1998-1999, Microsoft Corporation
 * 
 */

namespace System.Web.Caching {
    using System.Text;
    using System.Threading;

    using System.Web;
    using System.Web.Util;
    using System.Collections;

    internal class CacheKey {
        protected const byte BitPublic = 0x20;

        protected string    _key;   /* key to the item */
        protected byte      _bits;  /* cache lifetime state and public property */

        internal CacheKey(String key, bool isPublic) {
            if (key == null) {
                throw new ArgumentNullException("key");
            }
    
            _key = key;
            if (isPublic) {
                _bits = BitPublic;
            }
        }

        internal String Key {
            get {return _key;}
        }

        internal bool IsPublic {
            get { return (_bits & BitPublic) != 0; }
        }

#if DBG
        public override string ToString() {
            return (IsPublic ? "P:" : "I:") + _key;
        }
#endif
    }

    /*
     * An entry in the cache.
     * Overhead is 68 bytes + object header.
     */
    internal sealed class CacheEntry : CacheKey {
        static readonly DateTime    NoAbsoluteExpiration = DateTime.MaxValue;
        static readonly TimeSpan    NoSlidingExpiration = TimeSpan.Zero;
        const CacheItemPriority     CacheItemPriorityMin = CacheItemPriority.Low;
        const CacheItemPriority     CacheItemPriorityMax = CacheItemPriority.NotRemovable;
        static readonly TimeSpan    OneYear = new TimeSpan(365, 0, 0, 0);


        internal enum EntryState : byte {
            NotInCache         = 0x00,  // Created but not in hashtable
            AddingToCache      = 0x01,  // In hashtable only
            AddedToCache       = 0x02,  // In hashtable + expires + usage
            RemovingFromCache  = 0x04,  // Removed from hashtable only
            RemovedFromCache   = 0x08,  // Removed from hashtable & expires & usage
            Closed             = 0x10, 
        }

        const byte EntryStateMask   = 0x1f;
//        protected const byte BitPublic = 0x20;

        // item
        object                      _value;                 /* value */
        DateTime                    _utcCreated;            /* creation date */


        // expiration
        DateTime                    _utcExpires;            /* when this item expires */
        TimeSpan                    _slidingExpiration;     /* expiration interval */
        byte                        _expiresBucket;         /* index of the expiration list (bucket) */
        internal int                _expiresIndex;          /* index into the expiration list */

        // usage
        byte                        _usageBucket;           /* index of the usage list (== priority-1) */
        int                         _usageIndex;            /* index into the usage list */
        DateTime                    _utcLastUpdate;         /* time we last updated usage */

        // dependencies
        CacheDependency             _dependency;            /* dependencies this item has */
        object                      _onRemovedTargets;      /* targets of OnRemove notification */
        ReadWriteSpinLock           _lockTargets;           /* lock for targets */

        
            
        /*
         * ctor.
         */

        internal CacheEntry(
                   String                   key, 
                   Object                   value, 
                   CacheDependency          dependency,
                   CacheItemRemovedCallback onRemovedHandler,
                   DateTime                 utcAbsoluteExpiration,              
                   TimeSpan                 slidingExpiration,      
                   CacheItemPriority        priority,
                   bool                     isPublic) : 

                base(key, isPublic) {

            if (value == null) {
                throw new ArgumentNullException("value");
            }

            if (slidingExpiration < TimeSpan.Zero || OneYear < slidingExpiration) {
                throw new ArgumentOutOfRangeException("slidingExpiration");
            }

            if (utcAbsoluteExpiration != Cache.NoAbsoluteExpiration && slidingExpiration != Cache.NoSlidingExpiration) {
                throw new ArgumentException(HttpRuntime.FormatResourceString(SR.Invalid_expiration_combination));
            }

            if (priority < CacheItemPriorityMin || CacheItemPriorityMax < priority) {
                throw new ArgumentOutOfRangeException("priority");
            }

            _value = value;
            _dependency = dependency;
            _onRemovedTargets = onRemovedHandler;

            _utcCreated = DateTime.UtcNow;
            _slidingExpiration = slidingExpiration;
            if (_slidingExpiration > TimeSpan.Zero) {
                _utcExpires = _utcCreated + _slidingExpiration;
            }
            else {
                _utcExpires = utcAbsoluteExpiration;
            } 

            _expiresIndex = -1;
            _expiresBucket = 0xff;

            _usageIndex = -1;
            if (priority == CacheItemPriority.NotRemovable) {
                _usageBucket = 0xff;
            }
            else {
                _usageBucket = (byte) (priority - 1);
            }
        }

        internal Object Value {
            get {return _value;}
        }

        internal DateTime UtcCreated {
            get {return _utcCreated;}
        }

        internal EntryState State {
            get { return (EntryState) (_bits & EntryStateMask); }
            set { _bits = (byte) (((uint) _bits & ~(uint)EntryStateMask) | (uint) value); }
        }

        internal DateTime UtcExpires {
            get {return _utcExpires;}
            set {_utcExpires = value;}
        }

        internal TimeSpan SlidingExpiration {
            get {return _slidingExpiration;}
        }

        internal byte ExpiresBucket {
            get {return _expiresBucket;}
            set {_expiresBucket = value;}
        }

        internal int ExpiresIndex {
            get {return _expiresIndex;}
            set {_expiresIndex = value;}
        }

        internal bool HasExpiration() {
            return _utcExpires < DateTime.MaxValue;
        }

        internal bool InExpires() {
            return _expiresIndex != -1;
        }

        internal byte UsageBucket {
            get {return _usageBucket;}
            set {_usageBucket = value;}
        }

        internal int UsageIndex {
            get {return _usageIndex;}
            set {_usageIndex = value;}
        }

        internal DateTime UtcLastUsageUpdate {
            get {return _utcLastUpdate;}
            set {_utcLastUpdate = value;}
        }

        internal bool HasUsage() {
            return _usageBucket != 0xff;
        }

        internal bool InUsage() {
            return _usageIndex != -1;
        }

        internal CacheDependency Dependency {
            get {return _dependency;}
        }

        internal void MonitorDependencyChanges() {
            // need to protect against the item being closed
            CacheDependency dependency = _dependency;
            if (dependency != null && State == EntryState.AddedToCache) {
                if (!dependency.Use()) {
                    throw new InvalidOperationException(SR.Cache_dependency_used_more_that_once);
                }

                dependency.AddCacheEntryNotify(this);
            }
        }

        /*
         * The entry has changed, so remove ourselves from the cache.
         */
        internal void OnChanged(Object sender, EventArgs e) {
            if (State == EntryState.AddedToCache) {
                HttpRuntime.CacheInternal.Remove(this, CacheItemRemovedReason.DependencyChanged);
            }
        }

        /*
         * Helper to call the on-remove callback
         */

        private void CallCacheItemRemovedCallback(CacheItemRemovedCallback callback, CacheItemRemovedReason reason) {
            if (IsPublic) {
                try {
                    // for public need to impersonate if called outside of request context
                    HttpContext.ImpersonationData impersonation = null;
                    if (HttpContext.Current == null) {
                        impersonation = HttpContext.GetAppLevelImpersonation();
                        impersonation.Start(true /*forGlobalCode*/, false /*throwOnError*/);
                    }

                    try {
                        callback(_key, _value, reason);
                    }
                    finally {
                        if (impersonation != null)
                            impersonation.Stop();
                    }
                }
                catch (Exception e) {
                    // for public need to report application error
                    HttpApplicationFactory.RaiseError(e);
                }
            }
            else {
                // for private items just make the call and eat any exceptions
                try {
                    callback(_key, _value, reason);
                }
                catch {
                }
            }
        }

        /*
         * Close the item to complete its removal from cache.
         * 
         * @param reason The reason the item is removed.
         */
        internal void Close(CacheItemRemovedReason reason) {
            Debug.Assert(State == EntryState.RemovedFromCache, "State == EntryState.RemovedFromCache");
            State = EntryState.Closed;

            object      onRemovedTargets = null;
            object[]    targets = null;

            _lockTargets.AcquireWriterLock();
            try {
                if (_onRemovedTargets != null) {
                    onRemovedTargets = _onRemovedTargets;
                    if (onRemovedTargets is Hashtable) {
                        ICollection col = ((Hashtable) onRemovedTargets).Keys;
                        targets = new object[col.Count];
                        col.CopyTo(targets, 0);
                    }
                }
            }
            finally {
                _lockTargets.ReleaseWriterLock();
            }

            if (onRemovedTargets != null) {
                if (targets != null) {
                    foreach (object target in targets) {
                        if (target is CacheDependency) {
                            ((CacheDependency)target).ItemRemoved();
                        }
                        else {
                            CallCacheItemRemovedCallback((CacheItemRemovedCallback) target, reason);
                        }
                    }
                }
                else if (onRemovedTargets is CacheItemRemovedCallback) {
                    CallCacheItemRemovedCallback((CacheItemRemovedCallback) onRemovedTargets, reason);
                }
                else {
                    ((CacheDependency) onRemovedTargets).ItemRemoved();
                }
            }

            if (_dependency != null) {
                _dependency.DisposeInternal();
            }
        }

#if DBG
        internal /*public*/ string DebugDescription(string indent) {
            StringBuilder sb = new StringBuilder();
            String      nlindent = "\n" + indent + "    ";

            sb.Append(indent + "CacheItem");
            sb.Append(nlindent); sb.Append("_key=");        sb.Append(_key);
            sb.Append(nlindent); sb.Append("_value=");      sb.Append(Debug.GetDescription(_value, indent));
            sb.Append(nlindent); sb.Append("_utcExpires="); sb.Append(DateTimeUtil.ConvertToLocalTime(_utcExpires));
            sb.Append(nlindent); sb.Append("_bits=0x");     sb.Append(((int)_bits).ToString("x"));
            sb.Append("\n");

            return sb.ToString();
        }
#endif

        internal void AddCacheDependencyNotify(CacheDependency dependency) {
            _lockTargets.AcquireWriterLock();
            try {
                if (_onRemovedTargets == null) {
                    _onRemovedTargets = dependency;
                }
                else if (_onRemovedTargets is Hashtable) {
                    Hashtable h = (Hashtable) _onRemovedTargets;
                    h[dependency] = dependency;
                }
                else {
                    Hashtable h = new Hashtable(2);
                    h[_onRemovedTargets] = _onRemovedTargets;
                    h[dependency] = dependency;
                    _onRemovedTargets = h;
                }
            }
            finally {
                _lockTargets.ReleaseWriterLock();
            }
            
        }

        internal void RemoveCacheDependencyNotify(CacheDependency dependency) {
            _lockTargets.AcquireWriterLock();
            try {
                if (_onRemovedTargets != null) {
                    if (_onRemovedTargets == dependency) {
                        _onRemovedTargets = null;
                    }
                    else {
                        // We assume the dependency must exist, so we don't need
                        // to test for a cast.
                        Hashtable h = (Hashtable) _onRemovedTargets;
                        h.Remove(dependency);
                        if (h.Count == 0) {
                            _onRemovedTargets = null;
                        }
                    }
                }
            }
            finally {
                _lockTargets.ReleaseWriterLock();
            }
        }
    }
}
