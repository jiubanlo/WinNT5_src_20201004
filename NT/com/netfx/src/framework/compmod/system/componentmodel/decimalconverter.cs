//------------------------------------------------------------------------------
// <copyright file="DecimalConverter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.ComponentModel {
    using System.Runtime.Serialization.Formatters;
    using System.Globalization;
    using System.Runtime.Remoting;
    using System.Runtime.InteropServices;
    using System.ComponentModel.Design.Serialization;
    using System.Reflection;

    using System.Diagnostics;

    using Microsoft.Win32;

    /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter"]/*' />
    /// <devdoc>
    /// <para>Provides a type converter to convert <see cref='System.Decimal'/>
    /// objects to and from various
    /// other representations.</para>
    /// </devdoc>
    public class DecimalConverter : BaseNumberConverter {
    
          
        /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.AllowHex"]/*' />
        /// <devdoc>
        /// Determines whether this editor will attempt to convert hex (0x or #) strings
        /// </devdoc>
        internal override bool AllowHex {
                get {
                     return false;
                }
        }
    
         /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.TargetType"]/*' />
         /// <devdoc>
        /// The Type this converter is targeting (e.g. Int16, UInt32, etc.)
        /// </devdoc>
        internal override Type TargetType {
                get {
                    return typeof(Decimal);
                }
        }
        
        /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.CanConvertTo"]/*' />
        /// <devdoc>
        ///    <para>Gets a value indicating whether this converter can
        ///       convert an object to the given destination type using the context.</para>
        /// </devdoc>
        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType) {
            if (destinationType == typeof(InstanceDescriptor)) {
                return true;
            }
            return base.CanConvertTo(context, destinationType);
        }
        
        /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.ConvertTo"]/*' />
        /// <devdoc>
        ///      Converts the given object to another type.  The most common types to convert
        ///      are to and from a string object.  The default implementation will make a call
        ///      to ToString on the object if the object is valid and if the destination
        ///      type is string.  If this cannot convert to the desitnation type, this will
        ///      throw a NotSupportedException.
        /// </devdoc>
        public override object ConvertTo(ITypeDescriptorContext context, CultureInfo culture, object value, Type destinationType) {
            if (destinationType == null) {
                throw new ArgumentNullException("destinationType");
            }
            
            if (destinationType == typeof(InstanceDescriptor) && value is Decimal) {
            
                object[] args = new object[] { Decimal.GetBits((Decimal)value) };
                MemberInfo member = typeof(Decimal).GetConstructor(new Type[] {typeof(Int32[])});
                
                Debug.Assert(member != null, "Could not convert decimal to member.  Did someone change method name / signature and not update DecimalConverter?");
                if (member != null) {
                    return new InstanceDescriptor(member, args);
                }
                else {
                    return null;
                }
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }

        /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.FromString"]/*' />
        /// <devdoc>
        /// Convert the given value to a string using the given radix
        /// </devdoc>
        internal override object FromString(string value, int radix) {
                return Convert.ToDecimal(value);
        }
        
        /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.FromString1"]/*' />
        /// <devdoc>
        /// Convert the given value to a string using the given formatInfo
        /// </devdoc>
        internal override object FromString(string value, NumberFormatInfo formatInfo) {
                return Decimal.Parse(value, NumberStyles.Float, formatInfo);
        }
        
        
        /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.FromString2"]/*' />
        /// <devdoc>
        /// Convert the given value to a string using the given CultureInfo
        /// </devdoc>
        internal override object FromString(string value, CultureInfo culture){
                 return Decimal.Parse(value, culture);
        }
        
        /// <include file='doc\DecimalConverter.uex' path='docs/doc[@for="DecimalConverter.ToString"]/*' />
        /// <devdoc>
        /// Convert the given value from a string using the given formatInfo
        /// </devdoc>
        internal override string ToString(object value, NumberFormatInfo formatInfo) {
                return ((Decimal)value).ToString("G", formatInfo);
        }
    }
}

