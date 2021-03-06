//------------------------------------------------------------------------------
// <copyright file="CalendarDataBindingHandler.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.Web.UI.Design.MobileControls
{
    using System;
    using System.ComponentModel;
    using System.ComponentModel.Design;
    using System.Diagnostics;
    using System.Reflection;
    using System.Web.UI;
    using System.Web.UI.Design;
    using System.Web.UI.MobileControls;

    [
        System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand,
        Flags=System.Security.Permissions.SecurityPermissionFlag.UnmanagedCode)
    ]
    internal class CalendarDataBindingHandler : DataBindingHandler
    {
        public override void DataBindControl(IDesignerHost designerHost, Control control)
        {
            Debug.Assert(control is Calendar, "Expected a Calendar");
            Calendar calendar = (Calendar)control;

            DataBinding dateBinding = ((IDataBindingsAccessor)calendar).DataBindings["SelectedDate"];
            if (dateBinding != null) {
                calendar.SelectedDate = DateTime.Today;
            }
        }
    }
}

