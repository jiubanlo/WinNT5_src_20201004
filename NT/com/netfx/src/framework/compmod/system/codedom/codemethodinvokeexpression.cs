//------------------------------------------------------------------------------
// <copyright file="CodeMethodInvokeExpression.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.CodeDom {

    using System.Diagnostics;
    using System;
    using Microsoft.Win32;
    using System.Collections;
    using System.Runtime.InteropServices;

    /// <include file='doc\CodeMethodInvokeExpression.uex' path='docs/doc[@for="CodeMethodInvokeExpression"]/*' />
    /// <devdoc>
    ///    <para>
    ///       Represents an
    ///       expression to invoke a method, to be called on a given target.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeMethodInvokeExpression : CodeExpression {
        private CodeMethodReferenceExpression method;
        private CodeExpressionCollection parameters = new CodeExpressionCollection();

        /// <include file='doc\CodeMethodInvokeExpression.uex' path='docs/doc[@for="CodeMethodInvokeExpression.CodeMethodInvokeExpression"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodInvokeExpression'/>.
        ///    </para>
        /// </devdoc>
        public CodeMethodInvokeExpression() {
        }

        /// <include file='doc\CodeMethodInvokeExpression.uex' path='docs/doc[@for="CodeMethodInvokeExpression.CodeMethodInvokeExpression1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeMethodInvokeExpression'/> using the specified target object, method name
        ///       and parameters.
        ///    </para>
        /// </devdoc>
        public CodeMethodInvokeExpression(CodeMethodReferenceExpression method, params CodeExpression[] parameters) {
            this.method = method;
            Parameters.AddRange(parameters);
        }

        /// <include file='doc\CodeMethodInvokeExpression.uex' path='docs/doc[@for="CodeMethodInvokeExpression.CodeMethodInvokeExpression2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeMethodInvokeExpression(CodeExpression targetObject, string methodName, params CodeExpression[] parameters) {
            this.method = new CodeMethodReferenceExpression(targetObject, methodName);
            Parameters.AddRange(parameters);
        }

        /// <include file='doc\CodeMethodInvokeExpression.uex' path='docs/doc[@for="CodeMethodInvokeExpression.Method"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets the name of the method to invoke.
        ///    </para>
        /// </devdoc>
        public CodeMethodReferenceExpression Method {
            get {
                if (method == null) {
                    method = new CodeMethodReferenceExpression();
                }
                return method;
            }
            set {
                method = value;
            }
        }

        /// <include file='doc\CodeMethodInvokeExpression.uex' path='docs/doc[@for="CodeMethodInvokeExpression.Parameters"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets
        ///       the parameters to invoke the method with.
        ///    </para>
        /// </devdoc>
        public CodeExpressionCollection Parameters {
            get {
                return parameters;
            }
        }
    }
}
