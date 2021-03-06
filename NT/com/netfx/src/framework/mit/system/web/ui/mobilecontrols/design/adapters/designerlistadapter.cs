//------------------------------------------------------------------------------
// <copyright file="DesignerListAdapter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

using System.Diagnostics;
using System.Drawing;
using System.Web.Mobile;
using System.Web.UI.MobileControls;
using System.Web.UI.MobileControls.Adapters;

namespace System.Web.UI.Design.MobileControls.Adapters
{
    [
        System.Security.Permissions.SecurityPermission(System.Security.Permissions.SecurityAction.Demand,
        Flags=System.Security.Permissions.SecurityPermissionFlag.UnmanagedCode)
    ]
    internal class DesignerListAdapter : HtmlListAdapter
    {
        public override MobileCapabilities Device
        {
            get
            {
                return DesignerCapabilities.Instance;
            }
        }

        public override void Render(HtmlMobileTextWriter writer)
        {
            writer.WriteBeginTag("div");
            ((DesignerTextWriter)writer).WriteDesignerStyleAttributes(Control, Style);
            writer.Write("\">");

            base.Render(writer);

            writer.WriteEndTag("div");
        }    
    }
}
