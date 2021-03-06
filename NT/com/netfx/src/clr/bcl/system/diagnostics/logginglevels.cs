// ==++==
// 
//   Copyright (c) Microsoft Corporation.  All rights reserved.
// 
// ==--==
namespace System.Diagnostics {
    
	using System;
   // Constants representing the importance level of messages to be logged.
   // This level can be used to organize messages, and also to filter which
   // messages are displayed.
   //
   // An attached debugger can enable or disable which messages will
   // actually be reported to the user through the COM+ debugger
   // services API.  This info is communicated to the runtime so only
   // desired events are actually reported to the debugger.  
    // AtulC: NOTE: The following constants mirror the constants 
    // declared in the EE code (DebugDebugger.h). Any changes here will also
    // need to be made there.
    // Constants representing the importance level of messages to be logged.
    // This level can be used to organize messages, and also to filter which
    // messages are displayed.
    /// <include file='doc\LoggingLevels.uex' path='docs/doc[@for="LoggingLevels"]/*' />
	[Serializable()]
    internal enum LoggingLevels
    {
    	TraceLevel0         = 0,
    	TraceLevel1         = 1,
    	TraceLevel2         = 2,
    	TraceLevel3         = 3,
    	TraceLevel4         = 4,
    
    	StatusLevel0        = 20,
    	StatusLevel1        = 21,
    	StatusLevel2        = 22,
    	StatusLevel3        = 23,
    	StatusLevel4        = 24,
    
    	
    	WarningLevel        = 40,
    
    	/// <include file='doc\LoggingLevels.uex' path='docs/doc[@for="LoggingLevels.ErrorLevel"]/*' />
    	ErrorLevel          = 50,
    
    	PanicLevel          = 100,
    }

}
