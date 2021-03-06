//------------------------------------------------------------------------------
// <copyright file="LinkedDataMemberFieldEditor.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Windows.Forms.Design {

    using System;
    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.Drawing;
    using System.Drawing.Design;
    
    [System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand, Flags=System.Security.Permissions.SecurityPermissionFlag.UnmanagedCode)]
    internal class LinkedDataMemberFieldEditor : UITypeEditor {

        private DesignBindingPicker designBindingPicker;
        
        public override object EditValue(ITypeDescriptorContext context,  IServiceProvider  provider, object value) {
            if (provider != null) {
                IWindowsFormsEditorService edSvc = (IWindowsFormsEditorService)provider.GetService(typeof(IWindowsFormsEditorService));

                if (edSvc != null && context.Instance != null) {
                    if (designBindingPicker == null) {
                        designBindingPicker = new DesignBindingPicker(context, /*multiple DataSources*/ false, /*select lists*/ false);
                    }
                    PropertyDescriptor dataSourceProperty = TypeDescriptor.GetProperties(context.Instance)["LinkedDataSource"];
                    if (dataSourceProperty != null) {
                        object dataSource = dataSourceProperty.GetValue(context.Instance);
                        if (dataSource != null) {
                            designBindingPicker.Start(context, edSvc, dataSource, new DesignBinding(null, (string)value));
                            edSvc.DropDownControl(designBindingPicker);
                            if (designBindingPicker.SelectedItem != null) {
                                value = designBindingPicker.SelectedItem.DataMember;
                            }
                            designBindingPicker.End();
                        }
                    }
                }
            }

            return value;
        }

        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context) {
            return UITypeEditorEditStyle.DropDown;
        }
    }
}
