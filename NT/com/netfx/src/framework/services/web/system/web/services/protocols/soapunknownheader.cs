//------------------------------------------------------------------------------
// <copyright file="SoapUnknownHeader.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Web.Services.Protocols {
    using System.Web.Services;
    using System.Xml.Serialization;
    using System;
    using System.Reflection;
    using System.Xml;
    using System.Collections;
    using System.IO;
    using System.ComponentModel;

    /// <include file='doc\SoapUnknownHeader.uex' path='docs/doc[@for="SoapUnknownHeader"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public sealed class SoapUnknownHeader : SoapHeader {
        XmlElement element;

        /// <include file='doc\SoapUnknownHeader.uex' path='docs/doc[@for="SoapUnknownHeader.Element"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        [XmlIgnore]
        public XmlElement Element {
            get {
                if (element == null) return null;

                if (version == SoapProtocolVersion.Soap12) {
                    // need to fix up mustUnderstand and actor attributes for soap 1.2
                    if (InternalMustUnderstand) {
                        element.SetAttribute(Soap.MustUnderstand, Soap12.Namespace, "1");
                    }
                    element.RemoveAttribute(Soap.MustUnderstand, Soap.Namespace);
                    string actor = InternalActor;
                    if (actor != "") {
                        element.SetAttribute(Soap12.Role, Soap12.Namespace, actor);
                    }
                    element.RemoveAttribute(Soap.Actor, Soap.Namespace);
                }
                else if (version == SoapProtocolVersion.Soap11) {
                    // need to fix up mustUnderstand and actor attributes for soap 1.1
                    if (InternalMustUnderstand) {
                        element.SetAttribute(Soap.MustUnderstand, Soap.Namespace, "1");
                    }
                    element.RemoveAttribute(Soap.MustUnderstand, Soap12.Namespace);
                    string actor = InternalActor;
                    if (actor != "") {
                        element.SetAttribute(Soap.Actor, Soap.Namespace, actor);
                    }
                    element.RemoveAttribute(Soap12.Role, Soap12.Namespace);
                }

                return element; 
            }
            set {
                if (value == null && element != null) {
                    // save off the mustUnderstand/actor values before they go away
                    base.InternalMustUnderstand = InternalMustUnderstand;
                    base.InternalActor = InternalActor;
                }
                element = value;
            }
        }

        internal override bool InternalMustUnderstand {
            get {
                if (element == null) return base.InternalMustUnderstand;

                string elementMustUnderstand = GetElementAttribute(Soap.MustUnderstand, Soap.Namespace, element);
                if (elementMustUnderstand == null) {
                    elementMustUnderstand = GetElementAttribute(Soap.MustUnderstand, Soap12.Namespace, element);
                    if (elementMustUnderstand == null)
                        return false;
                }

                switch (elementMustUnderstand) {
                    case "false":
                    case "0":
                        return false;
                    case "true":
                    case "1":
                        return true;
                    default:
                        return false;
                }
            }
            set {
                base.InternalMustUnderstand = value;
                if (element != null) {
                    if (value)
                        element.SetAttribute(Soap.MustUnderstand, Soap.Namespace, "1");
                    else
                        element.RemoveAttribute(Soap.MustUnderstand, Soap.Namespace);

                    element.RemoveAttribute(Soap.MustUnderstand, Soap12.Namespace);
                }
            }
        }

        internal override string InternalActor {
            get {
                if (element == null) return base.InternalActor;

                string elementActor = GetElementAttribute(Soap.Actor, Soap.Namespace, element);
                if (elementActor == null) {
                    elementActor = GetElementAttribute(Soap12.Role, Soap12.Namespace, element);
                    if (elementActor == null) 
                        return "";
                }
                return elementActor;
            }
            set {
                base.InternalActor = value;
                if (element != null) {
                    if (value == null || value.Length == 0)
                        element.RemoveAttribute(Soap.Actor, Soap.Namespace);
                    else
                        element.SetAttribute(Soap.Actor, Soap.Namespace, value);
                    
                    element.RemoveAttribute(Soap12.Role, Soap12.Namespace);
                }
            }
        }

        private string GetElementAttribute(string name, string ns, XmlElement element) {
            if (element == null)
                return null;
            if (element.Prefix.Length == 0 && element.NamespaceURI == ns) {
                if (element.HasAttribute(name))
                    return element.GetAttribute(name);
                else
                    return null;
            }
            else {
                if (element.HasAttribute(name, ns))
                    return element.GetAttribute(name, ns);
                else
                    return null;
            }
        }
    }
}
