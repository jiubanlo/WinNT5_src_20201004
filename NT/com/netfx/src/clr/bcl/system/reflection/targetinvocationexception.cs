// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// TargetInvocationException is used to report an exception that was thrown
//	by the target of an invocation.
//
// Author: darylo
// Date: March 98
//
namespace System.Reflection {
    
    
	using System;
	using System.Runtime.Serialization;
    /// <include file='doc\TargetInvocationException.uex' path='docs/doc[@for="TargetInvocationException"]/*' />
	[Serializable()] 
    public sealed class TargetInvocationException : ApplicationException {

		// This exception is not creatable without specifying the
		//	inner exception.
    	private TargetInvocationException()
	        : base(Environment.GetResourceString("Arg_TargetInvocationException")) {
    		SetErrorCode(__HResults.COR_E_TARGETINVOCATION);
    	}

		// This is called from within the runtime.
        private TargetInvocationException(String message) : base(message) {
    		SetErrorCode(__HResults.COR_E_TARGETINVOCATION);
        }   	
    	
        /// <include file='doc\TargetInvocationException.uex' path='docs/doc[@for="TargetInvocationException.TargetInvocationException"]/*' />
        public TargetInvocationException(System.Exception inner) 
			: base(Environment.GetResourceString("Arg_TargetInvocationException"), inner) {
    		SetErrorCode(__HResults.COR_E_TARGETINVOCATION);
        }
    
        /// <include file='doc\TargetInvocationException.uex' path='docs/doc[@for="TargetInvocationException.TargetInvocationException1"]/*' />
        public TargetInvocationException(String message, Exception inner) : base(message, inner) {
    		SetErrorCode(__HResults.COR_E_TARGETINVOCATION);
        }

        internal TargetInvocationException(SerializationInfo info, StreamingContext context) : base (info, context) {
        }
    }
}
