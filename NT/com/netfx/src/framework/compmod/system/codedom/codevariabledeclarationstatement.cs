//------------------------------------------------------------------------------
// <copyright file="CodeVariableDeclarationStatement.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>                                                                
//------------------------------------------------------------------------------

namespace System.CodeDom {

    using System.Diagnostics;
    using System;
    using Microsoft.Win32;
    using System.Collections;
    using System.Runtime.InteropServices;

    /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement"]/*' />
    /// <devdoc>
    ///    <para>
    ///       Represents a local variable declaration.
    ///    </para>
    /// </devdoc>
    [
        ClassInterface(ClassInterfaceType.AutoDispatch),
        ComVisible(true),
        Serializable,
    ]
    public class CodeVariableDeclarationStatement : CodeStatement {
        private CodeTypeReference type;
        private string name;
        private CodeExpression initExpression;

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.CodeVariableDeclarationStatement"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeVariableDeclarationStatement'/>.
        ///    </para>
        /// </devdoc>
        public CodeVariableDeclarationStatement() {
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.CodeVariableDeclarationStatement1"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeVariableDeclarationStatement'/> using the specified type and name.
        ///    </para>
        /// </devdoc>
        public CodeVariableDeclarationStatement(CodeTypeReference type, string name) {
            Type = type;
            Name = name;
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.CodeVariableDeclarationStatement2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeVariableDeclarationStatement(string type, string name) {
            Type = new CodeTypeReference(type);
            Name = name;
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.CodeVariableDeclarationStatement3"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeVariableDeclarationStatement(Type type, string name) {
            Type = new CodeTypeReference(type);
            Name = name;
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.CodeVariableDeclarationStatement4"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of <see cref='System.CodeDom.CodeVariableDeclarationStatement'/> using the specified type, name and
        ///       initialization expression.
        ///    </para>
        /// </devdoc>
        public CodeVariableDeclarationStatement(CodeTypeReference type, string name, CodeExpression initExpression) {
            Type = type;
            Name = name;
            InitExpression = initExpression;
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.CodeVariableDeclarationStatement5"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeVariableDeclarationStatement(string type, string name, CodeExpression initExpression) {
            Type = new CodeTypeReference(type);
            Name = name;
            InitExpression = initExpression;
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.CodeVariableDeclarationStatement6"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public CodeVariableDeclarationStatement(Type type, string name, CodeExpression initExpression) {
            Type = new CodeTypeReference(type);
            Name = name;
            InitExpression = initExpression;
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.InitExpression"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets the initialization expression for the variable.
        ///    </para>
        /// </devdoc>
        public CodeExpression InitExpression {
            get {
                return initExpression;
            }
            set {
                initExpression = value;
            }
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.Name"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets the name of the variable.
        ///    </para>
        /// </devdoc>
        public string Name {
            get {
                return (name == null) ? string.Empty : name;
            }
            set {
                name = value;
            }
        }

        /// <include file='doc\CodeVariableDeclarationStatement.uex' path='docs/doc[@for="CodeVariableDeclarationStatement.Type"]/*' />
        /// <devdoc>
        ///    <para>
        ///       Gets or sets the type of the variable.
        ///    </para>
        /// </devdoc>
        public CodeTypeReference Type {
            get {
                if (type == null) {
                    type = new CodeTypeReference("");
                }
                return type;
            }
            set {
                type = value;
            }
        }
    }
}
