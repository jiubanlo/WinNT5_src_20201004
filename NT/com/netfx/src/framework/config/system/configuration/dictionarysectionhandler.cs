//------------------------------------------------------------------------------
// <copyright file="DictionarySectionHandler.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

#if !LIB

namespace System.Configuration {
    using System.Collections;
    using System.Collections.Specialized;
    using System.Xml;
    using System.Globalization;
    
    /// <include file='doc\DictionarySectionHandler.uex' path='docs/doc[@for="DictionarySectionHandler"]/*' />
    /// <devdoc>
    /// Simple dictionary config factory
    /// config is a dictionary mapping key-&gt;value
    /// 
    ///     &lt;add key="name" value="text"&gt;  sets key=text 
    ///     &lt;remove key="name"&gt;            removes the definition of key
    ///     &lt;clear&gt;                        removes all definitions
    /// 
    /// </devdoc>
    public class DictionarySectionHandler : IConfigurationSectionHandler {

        /// <include file='doc\DictionarySectionHandler.uex' path='docs/doc[@for="DictionarySectionHandler.Create"]/*' />
        /// <devdoc>
        /// Given a partially composed config object (possibly null)
        /// and some input from the config system, return a
        /// further partially composed config object
        /// </devdoc>
        public virtual object Create(Object parent, Object context, XmlNode section) {
            Hashtable res;

            // start res off as a shallow clone of the parent

            if (parent == null)
                res = new Hashtable(new CaseInsensitiveHashCodeProvider(CultureInfo.InvariantCulture), new CaseInsensitiveComparer(CultureInfo.InvariantCulture));
            else
                res = (Hashtable)((Hashtable)parent).Clone();

            // process XML

            HandlerBase.CheckForUnrecognizedAttributes(section);

            foreach (XmlNode child in section.ChildNodes) {

                // skip whitespace and comments, throws if non-element
                if (HandlerBase.IsIgnorableAlsoCheckForNonElement(child))
                    continue;

                // handle <add>, <remove>, <clear> tags
                if (child.Name == "add") {
                    HandlerBase.CheckForChildNodes(child);
                    String key = HandlerBase.RemoveRequiredAttribute(child, KeyAttributeName);
                    String value;
                    if (ValueRequired)
                        value = HandlerBase.RemoveRequiredAttribute(child, ValueAttributeName);
                    else
                        value = HandlerBase.RemoveAttribute(child, ValueAttributeName);
                    HandlerBase.CheckForUnrecognizedAttributes(child);

                    if (value == null)
                        value = "";

                    res[key] = value;
                }
                else if (child.Name == "remove") {
                    HandlerBase.CheckForChildNodes(child);
                    String key = HandlerBase.RemoveRequiredAttribute(child, KeyAttributeName);
                    HandlerBase.CheckForUnrecognizedAttributes(child);

                    res.Remove(key);
                }
                else if (child.Name.Equals("clear")) {
                    HandlerBase.CheckForChildNodes(child);
                    HandlerBase.CheckForUnrecognizedAttributes(child);
                    res.Clear();
                }
                else {
                    HandlerBase.ThrowUnrecognizedElement(child);
                }
            }

            return res;
        }

        /// <include file='doc\DictionarySectionHandler.uex' path='docs/doc[@for="DictionarySectionHandler.KeyAttributeName"]/*' />
        /// <devdoc>
        ///    Make the name of the key attribute configurable by derived classes.
        /// </devdoc>
        protected virtual string KeyAttributeName {
            get { return "key";}
        }

        /// <include file='doc\DictionarySectionHandler.uex' path='docs/doc[@for="DictionarySectionHandler.ValueAttributeName"]/*' />
        /// <devdoc>
        ///    Make the name of the value attribute configurable by derived classes.
        /// </devdoc>
        protected virtual string ValueAttributeName {
            get { return "value";}
        }

        // REVIEW: (davidgut) maybe we should make this public in the future?
        internal virtual bool ValueRequired {
            get { return false; }
        }
    }
}

#endif

