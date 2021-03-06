//------------------------------------------------------------------------------
// <copyright file="ImageIndexConverter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

/*
 */
namespace System.Windows.Forms {

    using Microsoft.Win32;
    using System.Collections;
    using System.ComponentModel;
    using System.Drawing;
    using System.Diagnostics;
    using System.Globalization;
    using System.Reflection;

    /// <include file='doc\ImageIndexConverter.uex' path='docs/doc[@for="ImageIndexConverter"]/*' />
    /// <devdoc>
    ///      ImageIndexConverter is a class that can be used to convert
    ///      image index values one data type to another.
    /// </devdoc>
    public class ImageIndexConverter : Int32Converter {

        /// <include file='doc\ImageIndexConverter.uex' path='docs/doc[@for="ImageIndexConverter.IncludeNoneAsStandardValue"]/*' />
        protected virtual bool IncludeNoneAsStandardValue {
            get {
                return true;
            }
        }                                
                                
        /// <include file='doc\ImageIndexConverter.uex' path='docs/doc[@for="ImageIndexConverter.ConvertFrom"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Converts the given value object to a 32-bit signed integer object.
        ///    </para>
        /// </devdoc>
        public override object ConvertFrom(ITypeDescriptorContext context, CultureInfo culture, object value) {
            if (value is string && String.Compare((string) value, SR.GetString(SR.toStringNone), true, culture) == 0) {
                return -1;
            }

            return base.ConvertFrom(context, culture, value);
        }


        /// <include file='doc\ImageIndexConverter.uex' path='docs/doc[@for="ImageIndexConverter.ConvertTo"]/*' />
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

            if (destinationType == typeof(string) && value is int && ((int)value) == -1) {
                return SR.GetString(SR.toStringNone);
            }

            return base.ConvertTo(context, culture, value, destinationType);
        }

        /// <include file='doc\ImageIndexConverter.uex' path='docs/doc[@for="ImageIndexConverter.GetStandardValues"]/*' />
        /// <devdoc>
        ///      Retrieves a collection containing a set of standard values
        ///      for the data type this validator is designed for.  This
        ///      will return null if the data type does not support a
        ///      standard set of values.
        /// </devdoc>
        public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context) {
            if (context != null && context.Instance != null) {
                object instance = context.Instance;
                PropertyDescriptor imageListProp = null;

                while (instance != null && imageListProp == null) {
                    PropertyDescriptorCollection props = TypeDescriptor.GetProperties(instance);

                    foreach (PropertyDescriptor prop in props) {
                        if (typeof(ImageList).IsAssignableFrom(prop.PropertyType)) {
                            imageListProp = prop;
                            break;
                        }
                    }

                    if (imageListProp == null) {

                        // We didn't find the image list in this component.  See if the 
                        // component has a "parent" property.  If so, walk the tree...
                        //
                        PropertyDescriptor parentProp = props["Parent"];
                        if (parentProp != null) {
                            instance = parentProp.GetValue(instance);
                        }
                        else {
                            // Stick a fork in us, we're done.
                            //
                            instance = null;
                        }
                    }
                }

                if (imageListProp != null) {
                    ImageList imageList = (ImageList)imageListProp.GetValue(instance);

                    if (imageList != null) {
                        
                        // Create array to contain standard values
                        //
                        object[] values;
                        int nImages = imageList.Images.Count;
                        if (IncludeNoneAsStandardValue) {
                            values = new object[nImages + 1];
                            values[nImages] = -1;
                        }
                        else {
                            values = new object[nImages];
                        }
                        
                        
                        // Fill in the array
                        //
                        for (int i = 0; i < nImages; i++) {
                            values[i] = i;
                        }
                        
                        return new StandardValuesCollection(values);
                    }
                }
            }

            if (IncludeNoneAsStandardValue) {
                return new StandardValuesCollection(new object[] {-1});
            }
            else {
                return new StandardValuesCollection(new object[0]);
            }
        }

        /// <include file='doc\ImageIndexConverter.uex' path='docs/doc[@for="ImageIndexConverter.GetStandardValuesExclusive"]/*' />
        /// <devdoc>
        ///      Determines if the list of standard values returned from
        ///      GetStandardValues is an exclusive list.  If the list
        ///      is exclusive, then no other values are valid, such as
        ///      in an enum data type.  If the list is not exclusive,
        ///      then there are other valid values besides the list of
        ///      standard values GetStandardValues provides.
        /// </devdoc>
        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context) {
            return false;
        }

        /// <include file='doc\ImageIndexConverter.uex' path='docs/doc[@for="ImageIndexConverter.GetStandardValuesSupported"]/*' />
        /// <devdoc>
        ///      Determines if this object supports a standard set of values
        ///      that can be picked from a list.
        /// </devdoc>
        public override bool GetStandardValuesSupported(ITypeDescriptorContext context) {
            return true;
        }
    }
}

