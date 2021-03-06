//------------------------------------------------------------------------------
// <copyright file="PersistenceTypeAttribute.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Web.UI {
    using System.Runtime.InteropServices;

    using System;
    using System.ComponentModel;
    using System.Security.Permissions;

    /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute"]/*' />
    /// <devdoc>
    ///     LiteralContentAttribute indicates whether the contents within a tag representing
    ///     a custom/web control should be treated by Trident as a "literal/text" content.
    ///     Web controls supporting complex properties (like Templates, etc.) typically
    ///     mark themselves as "literals", thereby letting the designer infra-structure
    ///     and Trident deal with the persistence of those attributes.
    ///
    ///     If LiteralContentAttribute.No is present or no LiteralContentAttribute marking
    ///     exists, then the tag corresponding to the web control is not treated as a literal
    ///     content tag.
    ///     If LiteralContentAttribute.Yes is present, then the tag corresponding to the web
    ///     control is treated as a literal content tag.
    /// </devdoc>
    [AttributeUsage(AttributeTargets.All)]
    [AspNetHostingPermission(SecurityAction.LinkDemand, Level=AspNetHostingPermissionLevel.Minimal)]
    public sealed class PersistenceModeAttribute : Attribute {

        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.Attribute"]/*' />
        /// <devdoc>
        ///     This marks a property or event as persistable in the HTML tag as an attribute.
        /// </devdoc>
        public static readonly PersistenceModeAttribute Attribute = new PersistenceModeAttribute(PersistenceMode.Attribute);

        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.InnerProperty"]/*' />
        /// <devdoc>
        ///     This marks a property or event as persistable within the HTML tag as a nested tag.
        /// </devdoc>
        public static readonly PersistenceModeAttribute InnerProperty = new PersistenceModeAttribute(PersistenceMode.InnerProperty);

        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.InnerDefaultProperty"]/*' />
        /// <devdoc>
        ///     This marks a property or event as persistable within the HTML tag as a child.
        /// </devdoc>
        public static readonly PersistenceModeAttribute InnerDefaultProperty = new PersistenceModeAttribute(PersistenceMode.InnerDefaultProperty);

        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.EncodedInnerDefaultProperty"]/*' />
        /// <devdoc>
        ///     This marks a property or event as persistable within the HTML tag as a child.
        /// </devdoc>
        public static readonly PersistenceModeAttribute EncodedInnerDefaultProperty = new PersistenceModeAttribute(PersistenceMode.EncodedInnerDefaultProperty);

    
        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.Default"]/*' />
        /// <devdoc>
        /// </devdoc>
        public static readonly PersistenceModeAttribute Default = Attribute;

        private PersistenceMode mode = PersistenceMode.Attribute;


        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.PersistenceModeAttribute"]/*' />
        /// <internalonly/>
        public PersistenceModeAttribute(PersistenceMode mode) {
            if (mode < PersistenceMode.Attribute || mode > PersistenceMode.EncodedInnerDefaultProperty) {
                throw new ArgumentOutOfRangeException("mode");
            }
            this.mode = mode;
        }


        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.Mode"]/*' />
        /// <devdoc>
        /// </devdoc>
        public PersistenceMode Mode {
            get {
                return mode;
            }
        }

        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.GetHashCode"]/*' />
        /// <internalonly/>
        public override int GetHashCode() {
            return base.GetHashCode();
        }

        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.Equals"]/*' />
        /// <devdoc>
        /// </devdoc>
        /// <internalonly/>
        public override bool Equals(object obj) {
            if (obj == this) {
                return true;
            }
            if ((obj != null) && (obj is PersistenceModeAttribute)) {
                return((PersistenceModeAttribute)obj).Mode == mode;
            }

            return false;
        }

        /// <include file='doc\PersistenceTypeAttribute.uex' path='docs/doc[@for="PersistenceModeAttribute.IsDefaultAttribute"]/*' />
        /// <devdoc>
        /// </devdoc>
        /// <internalonly/>
        public override bool IsDefaultAttribute() {
            return this.Equals(Default);
        }
    }
}
