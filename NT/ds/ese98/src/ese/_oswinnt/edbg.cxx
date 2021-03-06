#include "osstd.hxx"


#include <imagehlp.h>
#include <wdbgexts.h>


#include "std.hxx"
#include "_bf.hxx"
#include "_osslv.hxx"
#include "_tls.hxx"


#ifdef DEBUGGER_EXTENSION

#define EDBGAddGlobal( x )		{ #x, (VOID *)&x }

//	forward references
extern SIZE_T		cEDBGGlobals;
extern DWORD		cAllocHeap;
extern DWORD		cFreeHeap;
extern DWORD		cbAllocHeap;
extern DWORD_PTR	cbReservePage;
extern DWORD_PTR	cbCommitPage;
extern CRES *		g_pcresVERPool;

EDBGGLOBALVAR		rgEDBGGlobals[]	=
						{
						EDBGAddGlobal( cEDBGGlobals ),		//	must be first entry
						EDBGAddGlobal( g_cbPage ),
						EDBGAddGlobal( rgfmp ),
						EDBGAddGlobal( ifmpMax ),
						EDBGAddGlobal( g_rgpinst ),
						EDBGAddGlobal( ipinstMax ),
						EDBGAddGlobal( cbfChunk ),
						EDBGAddGlobal( rgpbfChunk ),
						EDBGAddGlobal( cbfCache ),
						EDBGAddGlobal( cpgChunk ),
						EDBGAddGlobal( rgpvChunk ),
						EDBGAddGlobal( cAllocHeap ),
						EDBGAddGlobal( cFreeHeap ),
						EDBGAddGlobal( cbAllocHeap ),
						EDBGAddGlobal( cbReservePage ),
						EDBGAddGlobal( cbCommitPage ),
						EDBGAddGlobal( CPAGE::cbHintCache ),
						EDBGAddGlobal( CPAGE::maskHintCache ),
						EDBGAddGlobal( g_pcresVERPool )
						};

SIZE_T				cEDBGGlobals			= sizeof(rgEDBGGlobals) / sizeof(EDBGGLOBALVAR);

//	debugger's copy of the globals table
EDBGGLOBALVAR *		rgEDBGGlobalsDebugger	= NULL;


//  ****************************************************************
//  STRUCTURES AND CLASSES
//  ****************************************************************


//  ================================================================
typedef VOID (*EDBGFUNC)(
//  ================================================================
	const HANDLE hCurrentProcess,
	const HANDLE hCurrentThread,
	const DWORD dwCurrentPc,
    const PWINDBG_EXTENSION_APIS lpExtensionApis,
    const INT argc,
    const CHAR * const argv[]
    );


//  ================================================================
class CPRINTFWDBG : public CPRINTF
//  ================================================================
	{
	public:
		VOID __cdecl operator()( const char * szFormat, ... ) const;
		static CPRINTF * PcprintfInstance();

		~CPRINTFWDBG() {}

	private:
		CPRINTFWDBG() {}
		static CHAR szBuf[1024];	//  WARNING: not multi-threaded safe!
	};


//  ================================================================
class CDUMP
//  ================================================================
	{
	public:
		CDUMP() {}
		virtual ~CDUMP() {}

		virtual VOID Dump( HANDLE, HANDLE, DWORD, PWINDBG_EXTENSION_APIS, INT, const CHAR * const [] ) = 0;
	};


//  ================================================================
template< class _STRUCT>
class CDUMPA : public CDUMP
//  ================================================================
	{
	public:
		VOID Dump(
			    HANDLE hCurrentProcess,
			    HANDLE hCurrentThread,
			    DWORD dwCurrentPc,
			    PWINDBG_EXTENSION_APIS lpExtensionApis,
			    INT argc,
			    const CHAR * const argv[] );
		static CDUMPA	instance;
	};


//  ================================================================
struct EDBGFUNCMAP
//  ================================================================
	{
	const char * 	szCommand;
	EDBGFUNC		function;
	const char * 	szHelp;
	};


//  ================================================================
struct CDUMPMAP
//  ================================================================
	{
	const char * 	szCommand;
	CDUMP 	   *	pcdump;
	const char * 	szHelp;
	};


//	HACK: declare dummy instances of these classes to allow dumping them
//
typedef CDynamicHashTable<DWORD,DWORD>			CDynamicHashTableEDBG;
typedef CApproximateIndex<DWORD,DWORD,0>		CApproximateIndexEDBG;
typedef CInvasiveList<DWORD,0>					CInvasiveListEDBG;


//  ****************************************************************
//  PROTOTYPES
//  ****************************************************************


#define VariableNameToString( var )	#var

#define DUMPA(_struct)	{ #_struct, &(CDUMPA<_struct>::instance), #_struct " <address>" }

#define DEBUG_EXT( name )					\
	LOCAL VOID name(						\
		const HANDLE hCurrentProcess,		\
		const HANDLE hCurrentThread,		\
		const DWORD dwCurrentPc,			\
	    const PWINDBG_EXTENSION_APIS lpExtensionApis,	\
	    const INT argc,						\
	    const CHAR * const argv[]  )

DEBUG_EXT( EDBGBFOB0FindOldest );
DEBUG_EXT( EDBGCacheFind );
DEBUG_EXT( EDBGCacheFindOldest );
DEBUG_EXT( EDBGCacheMap );
DEBUG_EXT( EDBGChecksum );
DEBUG_EXT( EDBGDebug );
DEBUG_EXT( EDBGDump );
DEBUG_EXT( EDBGDumpCacheMap );
DEBUG_EXT( EDBGDumpInvasiveList );
DEBUG_EXT( EDBGDumpLinkedList );
DEBUG_EXT( EDBGDumpMetaData );
DEBUG_EXT( EDBGDumpLR );
DEBUG_EXT( EDBGErr );
DEBUG_EXT( EDBGFCBFind );
DEBUG_EXT( EDBGFindRes );
DEBUG_EXT( EDBGGlobals );
DEBUG_EXT( EDBGHash );
DEBUG_EXT( EDBGHelp );
DEBUG_EXT( EDBGHelpDump );
DEBUG_EXT( EDBGLoad );
DEBUG_EXT( EDBGMemory );
DEBUG_EXT( EDBGSeek );
DEBUG_EXT( EDBGSync );
DEBUG_EXT( EDBGTableFind );
DEBUG_EXT( EDBGTest );
DEBUG_EXT( EDBGTid2PIB );
DEBUG_EXT( EDBGUnload );
DEBUG_EXT( EDBGVersion );
DEBUG_EXT( EDBGVerStore );
DEBUG_EXT( EDBGDumpAllFMPs );
DEBUG_EXT( EDBGDumpAllINSTs );
DEBUG_EXT( EDBGSympath );



extern VOID DBUTLDumpRec( const VOID * const pv, const INT cb, CPRINTF * pcprintf, const INT cbWidth );
extern VOID JetErrorToString( JET_ERR err, const char **szError, const char **szErrorText );
extern UINT g_wAssertAction;



//  ****************************************************************
//  GLOBALS
//  ****************************************************************



LOCAL WINDBG_EXTENSION_APIS ExtensionApis;

LOCAL BOOL		fDebugMode		 	= fFalse;	//	enable exceptions and assert dialogs
LOCAL BOOL		fInit 				= fFalse;	//	debugger extensions have been initialized
LOCAL ULONG		cSymInitFail		= 0;		//	number of times attempts to initialise the symbols sub-system failed
const ULONG		cSymInitAttemptsMax	= 3;		//	max attempts to initialise the symbols sub-system

LOCAL HINSTANCE	hLibrary			= NULL;		//	if we load outselves

CHAR CPRINTFWDBG::szBuf[1024];

template< class _STRUCT>
CDUMPA<_STRUCT> CDUMPA<_STRUCT>::instance;


//  ================================================================
LOCAL EDBGFUNCMAP rgfuncmap[] = {
//  ================================================================

	{
		"HELP",				EDBGHelp,
		"HELP                               - Print this help message"
	},
	{
		"BFOB0FINDOLDEST",	EDBGBFOB0FindOldest,
		"BFOB0FINDOLDEST <ifmp> [<gen>]     - Finds the BFOB0 entry of the given ifmp containing the oldest lgposBegin0"
	},
	{
		"CACHEFIND",		EDBGCacheFind,
		"CACHEFIND <ifmp> <pgno>            - Finds the BF containing the given ifmp:pgno"
	},
	{
		"CACHEFINDOLDEST",	EDBGCacheFindOldest,
		"CACHEFINDOLDEST <ifmp> [<gen>]     - Finds the BF of the given ifmp containing the oldest lgposBegin0"
	},
	{
		"CACHEMAP",			EDBGCacheMap,
		"CACHEMAP <address>                 - Performs pv => pbf or pbf => pv mapping"
	},
	{
		"CHECKSUM",			EDBGChecksum,
		"CHECKSUM <address> [<length>]      - Checksum a <g_cbPage> range of memory"
	},
	{
		"DEBUG",			EDBGDebug,
		"DEBUG                              - Toggle debug mode"
	},
	{
		"DUMP",				EDBGDump,
		"DUMP <class> <address>             - Dump an ESE structure at the given address"
	},
	{
		"DUMPCACHEMAP",		EDBGDumpCacheMap,
		"DUMPCACHEMAP                       - Dump the database cache memory map"
	},
	{
		"DUMPFMPS",			EDBGDumpAllFMPs,
		"DUMPFMPS [<rgfmp> <ifmpMax>] [*]   - Dump all used FMPs ([*] - also dump unused FMPs)"
	},
	{
		"DUMPINSTS",		EDBGDumpAllINSTs,
		"DUMPINSTS [<g_rgpinst>]            - Dump all instances"
	},
	{
		"DUMPINVASIVELIST",	EDBGDumpInvasiveList,
		"DUMPINVASIVELIST <address> [+/-]<offset> [<dwords to display> <max elements>]\n\t"
		"                                   - Dump elements of an invasive list"
	},
	{
		"DUMPLINKEDLIST",	EDBGDumpLinkedList,
		"DUMPLINKEDLIST <address> <offset> [<dwords to display> <max elements>]\n\t"
		"                                   - Dump elements of a linked list"
	},
	{
		"DUMPLR",			EDBGDumpLR,
		"DUMPLR                             - Dump a log record"
	},
	{
		"DUMPMETADATA",		EDBGDumpMetaData,
		"DUMPMETADATA <pfcb>                - Dumps meta-data for the specified FCB"
	},
	{
		"GLOBALS",			EDBGGlobals,
		"GLOBALS [<rgEDBGGlobals>]          - Load debuggee's table of globals (use when symbol mapping does not work)."
	},
	{
		"ERR",				EDBGErr,
		"ERR <error>                        - Turn an error number into a string"
	},
	{
		"FCBFIND",			EDBGFCBFind,
		"FCBFIND <FDP> [<instance>]         - Finds all FCB's with an objidFDP or pgnoFDP matching <FDP>"
	},
	{
		"FINDRES",			EDBGFindRes,
		"FINDRES <cres> <address> <length>  - Find pointers to memory in the CRES in the address range"
	},
	{
		"HASH",				EDBGHash,
		"HASH <ifmp> <pgnoFDP> <pbPrefix> <cbPrefix> <pbSuffix> <cbSuffix> <pbData> <cbData>\n\t"
		"                                   - Generate the version store hash for a given key"
	},
	{
		"LOAD",				EDBGLoad,
		"LOAD                               - Load the DLL"
	},
	{
		"MEMORY",			EDBGMemory,
		"MEMORY                             - Dump memory usage information"
	},
	{
		"SEEK",				EDBGSeek,
		"SEEK <ifmp> <pgnoRoot> <pbPrefix> <cbPrefix> [<pbSuffix> <cbSuffix> [<pbData> <cbData>]]\n\t"
		"                                   - Seeks to the specified bookmark starting from the specified page"
	},
	{
		"SYNC",				EDBGSync,
		"SYNC ...                           - Synchronization Library Debugger Extensions"
	},
	{
		"SYMPATH", 			EDBGSympath,
		"SYMPATH [<pathname>]               - Set symbol path"
	},
	{
		"TABLEFIND",		EDBGTableFind,
		"TABLEFIND <szTable> [<instance>]   - Finds the FCB for the specified table"
	},
	{
		"TEST",				EDBGTest,
		"TEST                               - Test function"
	},
	{
		"TID2PIB",			EDBGTid2PIB,
		"TID2PIB <tid> [<g_rgpinst>]        - Find the PIB for a given TID"
	},
	{
		"UNLOAD",			EDBGUnload,
		"UNLOAD                             - Unload the DLL"
	},
	{
		"VERSION",			EDBGVersion,
		"VERSION                            - Version info for ESE.DLL"
	},
	{
		"VERSTORE",			EDBGVerStore,
		"VERSTORE <instance id>             - Dump version store usage information for the specified instance"
	},
	};

const int cfuncmap = sizeof( rgfuncmap ) / sizeof( EDBGFUNCMAP );



//  ================================================================
LOCAL CDUMPMAP rgcdumpmap[] = {
//  ================================================================

	DUMPA( BF ),
	DUMPA( RCE ),
	DUMPA( PIB ),
	DUMPA( FUCB ),
	DUMPA( CSR ),
	DUMPA( REC ),
	DUMPA( FCB ),
	DUMPA( IDB ),
	DUMPA( TDB ),
	DUMPA( CRES ),
	DUMPA( INST ),
	DUMPA( FMP ),
	DUMPA( LOG ),
	DUMPA( VER ),
	{
		"MEMPOOL", 	&(CDUMPA<MEMPOOL>::instance),
 		"MEMPOOL <address> [<itag>|*]       - <itag>=specified tag only, *=all tags"
	},
	DUMPA( SPLIT ),
	DUMPA( SPLITPATH ),
	DUMPA( MERGE ),
	DUMPA( MERGEPATH ),
	DUMPA( DBFILEHDR ),
	{
		"CDynamicHashTable", &(CDUMPA<CDynamicHashTableEDBG>::instance),
		"CDynamicHashTable <address>"
	},
	{
		"CApproximateIndex", &(CDUMPA<CApproximateIndexEDBG>::instance),
		"CApproximateIndex <address>"
	},
	DUMPA( COSFile ),
	DUMPA( COSFileFind ),
	DUMPA( COSFileLayout ),
	DUMPA( COSFileSystem ),
	{
		"PAGE", 	&(CDUMPA<CPAGE>::instance),
		"PAGE [a|h|t|*|2|4|8] <address>     - a=alloc map, h=header, t=tags, *=all, 2/4/8=pagesize"
	},

#ifdef DISABLE_SLV
#else
	DUMPA( CSLVBackingFile ),
	DUMPA( CSLVFileInfo ),
	DUMPA( CSLVFileTable ),
	DUMPA( CSLVInfo ),
	DUMPA( _SLVROOT ),
#endif
	};

const int ccdumpmap = sizeof( rgcdumpmap ) / sizeof( CDUMPMAP );



//  ****************************************************************
//  FUNCTIONS
//  ****************************************************************



//  ================================================================
inline VOID __cdecl CPRINTFWDBG::operator()( const char * szFormat, ... ) const
//  ================================================================
	{
	va_list arg_ptr;
	va_start( arg_ptr, szFormat );
	_vsnprintf( szBuf, sizeof( szBuf ), szFormat, arg_ptr );
	va_end( arg_ptr );
	szBuf[ sizeof( szBuf ) - 1 ] = 0;
	(ExtensionApis.lpOutputRoutine)( "%s", szBuf );
	}


//  ================================================================
CPRINTF * CPRINTFWDBG::PcprintfInstance()
//  ================================================================
	{
	static CPRINTFWDBG CPrintfWdbg;
	return &CPrintfWdbg;
	}


//  ================================================================
LOCAL INT SzToRgsz( CHAR * rgsz[], CHAR * const sz )
//  ================================================================
	{
	INT irgsz = 0;
	CHAR * szT = sz;
	while( NULL != ( rgsz[irgsz] = strtok( szT, " \t\n" ) ) )
		{
		++irgsz;
		szT = NULL;
		}
	return irgsz;
	}


//  ================================================================
LOCAL BOOL FArgumentMatch( const CHAR * const sz, const CHAR * const szCommand )
//  ================================================================
	{
	const BOOL fMatch = ( ( strlen( sz ) == strlen( szCommand ) )
			&& !( _strnicmp( sz, szCommand, strlen( szCommand ) ) ) );
	return fMatch;
	}


namespace OSSYM {

#include <imagehlp.h>
#include <psapi.h>


typedef DWORD IMAGEAPI WINAPI PFNUnDecorateSymbolName( PCSTR, PSTR, DWORD, DWORD );
typedef DWORD IMAGEAPI PFNSymSetOptions( DWORD );
typedef BOOL IMAGEAPI PFNSymCleanup( HANDLE );
typedef BOOL IMAGEAPI PFNSymInitialize( HANDLE, PSTR, BOOL );
typedef BOOL IMAGEAPI PFNSymGetSymFromAddr( HANDLE, DWORD_PTR, DWORD_PTR*, PIMAGEHLP_SYMBOL );
typedef BOOL IMAGEAPI PFNSymGetSymFromName( HANDLE, PSTR, PIMAGEHLP_SYMBOL );
typedef BOOL IMAGEAPI PFNSymGetSearchPath( HANDLE, PSTR, DWORD );
typedef BOOL IMAGEAPI PFNSymSetSearchPath( HANDLE, PSTR );
typedef BOOL IMAGEAPI PFNSymGetModuleInfo( HANDLE, DWORD_PTR, PIMAGEHLP_MODULE );
typedef DWORD IMAGEAPI PFNSymLoadModule( HANDLE, HANDLE, PSTR, PSTR, DWORD_PTR, DWORD );
typedef DWORD IMAGEAPI PFNSymUnloadModule( HANDLE, DWORD_PTR );
typedef PIMAGE_NT_HEADERS IMAGEAPI PFNImageNtHeader( PVOID );

PFNUnDecorateSymbolName*	pfnUnDecorateSymbolName;
PFNSymSetOptions*			pfnSymSetOptions;
PFNSymCleanup*				pfnSymCleanup;
PFNSymInitialize*			pfnSymInitialize;
PFNSymGetSymFromAddr*		pfnSymGetSymFromAddr;
PFNSymGetSymFromName*		pfnSymGetSymFromName;
PFNSymGetSearchPath*		pfnSymGetSearchPath;
PFNSymSetSearchPath*		pfnSymSetSearchPath;
PFNSymGetModuleInfo*		pfnSymGetModuleInfo;
PFNSymLoadModule*			pfnSymLoadModule;
PFNSymUnloadModule*			pfnSymUnloadModule;
PFNImageNtHeader*			pfnImageNtHeader;

HMODULE hmodImagehlp;

typedef BOOL WINAPI PFNEnumProcessModules( HANDLE, HMODULE*, DWORD, LPDWORD );
typedef DWORD WINAPI PFNGetModuleFileNameExA( HANDLE, HMODULE, LPSTR, DWORD );
typedef DWORD WINAPI PFNGetModuleBaseNameA( HANDLE, HMODULE, LPSTR, DWORD );
typedef BOOL WINAPI PFNGetModuleInformation( HANDLE, HMODULE, LPMODULEINFO, DWORD );

PFNEnumProcessModules*		pfnEnumProcessModules;
PFNGetModuleFileNameExA*	pfnGetModuleFileNameExA;
PFNGetModuleBaseNameA*		pfnGetModuleBaseNameA;
PFNGetModuleInformation*	pfnGetModuleInformation;

HMODULE hmodPsapi;

LOCAL const DWORD symopt =	SYMOPT_CASE_INSENSITIVE |
							SYMOPT_UNDNAME |
							SYMOPT_OMAP_FIND_NEAREST |
							SYMOPT_DEFERRED_LOADS;
LOCAL CHAR szParentImageName[_MAX_FNAME];
LOCAL HANDLE ghDbgProcess;


//  ================================================================
LOCAL BOOL SymLoadAllModules( HANDLE hProcess, BOOL fReload = fFalse )
//  ================================================================
	{
	HMODULE* rghmodDebuggee = NULL;

	//  fetch all modules in the debuggee process and manually load their symbols.
	//  we do this because telling imagehlp to invade the debuggee process doesn't
	//  work when we are already running in the context of a debugger

	DWORD cbNeeded;
	if ( !pfnEnumProcessModules( hProcess, NULL, 0, &cbNeeded ) )
		{
		goto HandleError;
		}

	DWORD cbActual;
	do	{
		cbActual = cbNeeded;
		rghmodDebuggee = (HMODULE*)LocalAlloc( 0, cbActual );
		if ( NULL == rghmodDebuggee )
			{
			goto HandleError;
			}

		if ( !pfnEnumProcessModules( hProcess, rghmodDebuggee, cbActual, &cbNeeded ) )
			{
			goto HandleError;
			}
		}
	while ( cbNeeded > cbActual );

	SIZE_T ihmod;
	SIZE_T ihmodLim;

	ihmodLim = cbNeeded / sizeof( HMODULE );
	for ( ihmod = 0; ihmod < ihmodLim; ihmod++ )
		{
		char szModuleImageName[ _MAX_PATH ];
		if ( !pfnGetModuleFileNameExA( hProcess, rghmodDebuggee[ ihmod ], szModuleImageName, _MAX_PATH ) )
			{
			goto HandleError;
			}

		char szModuleBaseName[ _MAX_FNAME ];
		if ( !pfnGetModuleBaseNameA( hProcess, rghmodDebuggee[ ihmod ], szModuleBaseName, _MAX_FNAME ) )
			{
			goto HandleError;
			}

		MODULEINFO mi;
		if ( !pfnGetModuleInformation( hProcess, rghmodDebuggee[ ihmod ], &mi, sizeof( mi ) ) )
			{
			goto HandleError;
			}

		if ( fReload )
			{
			pfnSymUnloadModule( hProcess, DWORD_PTR( mi.lpBaseOfDll ) );
			}

		if ( !pfnSymLoadModule(	hProcess,
								NULL,
								szModuleImageName,
								szModuleBaseName,
								DWORD_PTR( mi.lpBaseOfDll ),
								mi.SizeOfImage ) )
			{
			goto HandleError;
			}

		IMAGEHLP_MODULE im;
		im.SizeOfStruct = sizeof( IMAGEHLP_MODULE );

		if ( !pfnSymGetModuleInfo( hProcess, DWORD_PTR( mi.lpBaseOfDll ), &im ) )
			{
			goto HandleError;
			}
		}

	return fTrue;

HandleError:
	if ( NULL != (void *)rghmodDebuggee )
		{
		LocalFree( (void*)rghmodDebuggee );
		}
	return fFalse;
	}

//  ================================================================
LOCAL BOOL SymInitializeEx(	HANDLE hProcess, HANDLE* phProcess )
//  ================================================================
	{
	//  init our out param

	*phProcess = NULL;

	//  duplicate the given debuggee process handle so that we have our own
	//  sandbox in imagehlp

	if ( !DuplicateHandle(	GetCurrentProcess(),
							hProcess,
							GetCurrentProcess(),
							phProcess,
							0,
							FALSE,
							DUPLICATE_SAME_ACCESS ) )
		{
		goto HandleError;
		}
	SetHandleInformation( *phProcess, HANDLE_FLAG_PROTECT_FROM_CLOSE, HANDLE_FLAG_PROTECT_FROM_CLOSE );

	//  init imagehlp for the debuggee process

	if ( !pfnSymInitialize( *phProcess, NULL, FALSE ) )
		{
		goto HandleError;
		}

	//  we're done

	return fTrue;

HandleError:
	if ( *phProcess )
		{
		SetHandleInformation( *phProcess, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0 );
		CloseHandle( *phProcess );
		*phProcess = NULL;
		}
	return fFalse;
	}

//  ================================================================
LOCAL void SymTerm()
//  ================================================================
	{
	//  shut down imagehlp

	if ( pfnSymCleanup )
		{
		if ( ghDbgProcess )
			{
			pfnSymCleanup( ghDbgProcess );
			}
		pfnSymCleanup = NULL;
		}

	//  free psapi

	if ( hmodPsapi )
		{
		FreeLibrary( hmodPsapi );
		hmodPsapi = NULL;
		}

	//  free imagehlp

	if ( hmodImagehlp )
		{
		FreeLibrary( hmodImagehlp );
		hmodImagehlp = NULL;
		}

	//  close our process handle

	if ( ghDbgProcess )
		{
		SetHandleInformation( ghDbgProcess, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0 );
		CloseHandle( ghDbgProcess );
		ghDbgProcess = NULL;
		}
	}

//  ================================================================
LOCAL BOOL FSymInit( HANDLE hProc )
//  ================================================================
	{
	HANDLE hThisProcess = NULL;

	//  reset all pointers

	ghDbgProcess	= NULL;
	hmodImagehlp	= NULL;
	pfnSymCleanup	= NULL;
	hmodPsapi		= NULL;

	//  load all calls in imagehlp

	if ( !( hmodImagehlp = LoadLibrary( "imagehlp.dll" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymGetSymFromAddr = (PFNSymGetSymFromAddr*)GetProcAddress( hmodImagehlp, "SymGetSymFromAddr" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymGetSymFromName = (PFNSymGetSymFromName*)GetProcAddress( hmodImagehlp, "SymGetSymFromName" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymInitialize = (PFNSymInitialize*)GetProcAddress( hmodImagehlp, "SymInitialize" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymCleanup = (PFNSymCleanup*)GetProcAddress( hmodImagehlp, "SymCleanup" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymSetOptions = (PFNSymSetOptions*)GetProcAddress( hmodImagehlp, "SymSetOptions" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnUnDecorateSymbolName = (PFNUnDecorateSymbolName*)GetProcAddress( hmodImagehlp, "UnDecorateSymbolName" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymGetSearchPath = (PFNSymGetSearchPath*)GetProcAddress( hmodImagehlp, "SymGetSearchPath" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymSetSearchPath = (PFNSymSetSearchPath*)GetProcAddress( hmodImagehlp, "SymSetSearchPath" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymGetModuleInfo = (PFNSymGetModuleInfo*)GetProcAddress( hmodImagehlp, "SymGetModuleInfo" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnSymLoadModule = (PFNSymLoadModule*)GetProcAddress( hmodImagehlp, "SymLoadModule" ) ) )
		{
		goto HandleError;
		}

	if ( !( pfnSymUnloadModule = (PFNSymUnloadModule*)GetProcAddress( hmodImagehlp, "SymUnloadModule" ) ) )
		{
		goto HandleError;
		}

	if ( !( pfnImageNtHeader = (PFNImageNtHeader*)GetProcAddress( hmodImagehlp, "ImageNtHeader" ) ) )
		{
		goto HandleError;
		}

	//  load all calls in psapi

	if ( !( hmodPsapi = LoadLibrary( "psapi.dll" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnEnumProcessModules = (PFNEnumProcessModules*)GetProcAddress( hmodPsapi, "EnumProcessModules" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnGetModuleFileNameExA = (PFNGetModuleFileNameExA*)GetProcAddress( hmodPsapi, "GetModuleFileNameExA" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnGetModuleBaseNameA = (PFNGetModuleBaseNameA*)GetProcAddress( hmodPsapi, "GetModuleBaseNameA" ) ) )
		{
		goto HandleError;
		}
	if ( !( pfnGetModuleInformation = (PFNGetModuleInformation*)GetProcAddress( hmodPsapi, "GetModuleInformation" ) ) )
		{
		goto HandleError;
		}

	//  get the name of our parent image in THIS process.  we need this name so
	//  that we can prefix symbols with the default module name and so that we
	//  can add the image path to the symbol path

	MEMORY_BASIC_INFORMATION mbi;
	if ( !VirtualQueryEx( GetCurrentProcess(), FSymInit, &mbi, sizeof( mbi ) ) )
		{
		goto HandleError;
		}
	char szImage[_MAX_PATH];
	if ( !GetModuleFileNameA( HINSTANCE( mbi.AllocationBase ), szImage, sizeof( szImage ) ) )
		{
		goto HandleError;
		}
	_splitpath( (const _TCHAR *)szImage, NULL, NULL, szParentImageName, NULL );

	//  init imagehlp for the debuggee process

	if ( !SymInitializeEx( hProc, &ghDbgProcess ) )
		{
		goto HandleError;
		}

	//  set our symbol path to include the path of this image and the process
	//  executable

	char szOldPath[ 4 * _MAX_PATH ];
	if ( pfnSymGetSearchPath( ghDbgProcess, szOldPath, sizeof( szOldPath ) ) )
		{
		char szNewPath[ 6 * _MAX_PATH ];
		char szDrive[ _MAX_DRIVE ];
		char szDir[ _MAX_DIR ];
		char szPath[ _MAX_PATH ];

		szNewPath[ 0 ] = 0;

		strcat( szNewPath, szOldPath );
		strcat( szNewPath, ";" );

		HMODULE hImage = GetModuleHandle( szParentImageName );
		GetModuleFileName( hImage, szPath, _MAX_PATH );
		_splitpath( szPath, szDrive, szDir, NULL, NULL );
		_makepath( szPath, szDrive, szDir, NULL, NULL );
		strcat( szNewPath, szPath );
		strcat( szNewPath, ";" );

		GetModuleFileName( NULL, szPath, _MAX_PATH );
		_splitpath( szPath, szDrive, szDir, NULL, NULL );
		_makepath( szPath, szDrive, szDir, NULL, NULL );
		strcat( szNewPath, szPath );

		pfnSymSetSearchPath( ghDbgProcess, szNewPath );
		}

	//  set our default symbol options

	pfnSymSetOptions( symopt );

	//  prepare symbols for the debuggee process

	if ( !SymLoadAllModules( ghDbgProcess ) )
		{
		goto HandleError;
		}

	return fTrue;

HandleError:
	if ( hThisProcess )
		{
		SetHandleInformation( hThisProcess, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0 );
		CloseHandle( hThisProcess );
		}
	SymTerm();
	return fFalse;
	}

//  ================================================================
LOCAL BOOL FUlFromSz( const char* const sz, ULONG* const pul, const int base = 16 )
//  ================================================================
	{
	if( sz && *sz )
		{
		char* pchEnd;
		*pul = strtoul( sz, &pchEnd, base );
		return !( *pchEnd );
		}
	return fFalse;
	}

//  ================================================================
template< class T >
LOCAL BOOL FAddressFromSz( const char* const sz, T** const ppt )
//  ================================================================
	{
	if ( sz && *sz )
		{
		int		n;
		QWORD	first;
		DWORD	second;
		int		cchRead;
		BOOL	f;

		n = sscanf( sz, "%I64x%n%*[` ]%8lx%n", &first, &cchRead, &second, &cchRead );

		switch ( n )
			{
			case 2:
				*ppt = (T*)( ( first << 32 ) | second );
				f = fTrue;
				break;

			case 1:
				*ppt = (T*)( first );
				f = fTrue;
				break;

			default:
				f = fFalse;
				break;
			};
		if ( cchRead != int( strlen( sz ) ) )
			{
			f = fFalse;
			}

		return f;
		}
	return fFalse;
	}

//  ================================================================
template< class T >
LOCAL BOOL FHintAddressFromGlobal( const char* const szGlobal, T** const ppt )
//  ================================================================
	{
	if ( NULL != rgEDBGGlobalsDebugger )
		{
		//	search in the table for the particular global name
		for ( SIZE_T i = 1; i < (SIZE_T)rgEDBGGlobalsDebugger[0].pvAddress; i++ )
			{
			if ( 0 == _stricmp( rgEDBGGlobalsDebugger[i].szName, szGlobal ) )
				{
				*ppt = (T*)rgEDBGGlobalsDebugger[i].pvAddress;
				return fTrue;
				}
			}
		}
	return fFalse;
	}

//  ================================================================
template< class T >
LOCAL BOOL FAddressFromGlobal( const char* const szGlobal, T** const ppt )
//  ================================================================
	{
	if ( FHintAddressFromGlobal( szGlobal, ppt ) )
		{
		return fTrue;
		}
	else if ( !fInit )
		{
		return fFalse;
		}

	//  add the module prefix to the global name to form the symbol

	SIZE_T	cchSymbol	= strlen( szParentImageName ) + 1 + strlen( szGlobal );
	char*	szSymbol	= (char*)LocalAlloc( 0, ( cchSymbol + 1 ) * sizeof( char ) );
	if ( !szSymbol )
		{
		return fFalse;
		}
	szSymbol[ 0 ] = 0;
	if ( !strchr( szGlobal, '!' ) )
		{
		strcat( szSymbol, szParentImageName );
		strcat( szSymbol, "!" );
		}
	strcat( szSymbol, szGlobal );

	//  try forever until we manage to retrieve the entire undecorated symbol
	//  and address corresponding to this symbol

	SIZE_T cbBuf = 1024;
	BYTE* rgbBuf = (BYTE*)LocalAlloc( 0, cbBuf );
	if ( !rgbBuf )
		{
		LocalFree( (void*)szSymbol );
		return fFalse;
		}


	IMAGEHLP_SYMBOL* pis;
	do	{
		pis							= (IMAGEHLP_SYMBOL*)rgbBuf;
		pis->SizeOfStruct			= sizeof( IMAGEHLP_SYMBOL );
		pis->MaxNameLength			= DWORD( ( cbBuf - sizeof( IMAGEHLP_SYMBOL ) ) / sizeof( char ) );

		DWORD	symoptOld	= pfnSymSetOptions( symopt );
		BOOL	fSuccess	= pfnSymGetSymFromName( ghDbgProcess, PSTR( szSymbol ), pis );
		DWORD	symoptNew	= pfnSymSetOptions( symoptOld );

		if ( !fSuccess )
			{
			LocalFree( (void*)szSymbol );
			LocalFree( (void*)rgbBuf );
			return fFalse;
			}

		if ( strlen( pis->Name ) == cbBuf - 1 )
			{
			LocalFree( (void*)rgbBuf );
			cbBuf *= 2;
			if ( !( rgbBuf = (BYTE*)LocalAlloc( 0, cbBuf ) ) )
				{
				LocalFree( (void*)szSymbol );
				return fFalse;
				}
			}
		}
	while ( strlen( pis->Name ) == cbBuf - 1 );

	//  validate the symbols for the image containing this address

	IMAGEHLP_MODULE		im	= { sizeof( IMAGEHLP_MODULE ) };
	IMAGE_NT_HEADERS*	pnh;

	if (	!pfnSymGetModuleInfo( ghDbgProcess, pis->Address, &im ) ||
			!( pnh = pfnImageNtHeader( (void*)im.BaseOfImage ) ) ||
			pnh->FileHeader.TimeDateStamp != im.TimeDateStamp ||
			pnh->FileHeader.SizeOfOptionalHeader >= IMAGE_SIZEOF_NT_OPTIONAL_HEADER &&
			pnh->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC &&
			pnh->OptionalHeader.CheckSum != im.CheckSum &&
			(	pnh->FileHeader.TimeDateStamp != im.TimeDateStamp ||
				_stricmp( im.ModuleName, "kernel32" ) &&
				_stricmp( im.ModuleName, "ntdll" ) ) )
		{
		LocalFree( (void*)szSymbol );
		LocalFree( (void*)rgbBuf );
		return fFalse;
		}

	//  return the address of the symbol

	*ppt = (T*)pis->Address;

	LocalFree( (void*)szSymbol );
	LocalFree( (void*)rgbBuf );
	return fTrue;
	}

//  ================================================================
template< class T >
LOCAL BOOL FGlobalFromAddress( T* const pt, char* szGlobal, const SIZE_T cchMax, DWORD_PTR* const pdwOffset = NULL )
//  ================================================================
	{
	//  validate the symbols for the image containing this address

	IMAGEHLP_MODULE		im	= { sizeof( IMAGEHLP_MODULE ) };
	IMAGE_NT_HEADERS*	pnh;

	if (	!pfnSymGetModuleInfo( ghDbgProcess, DWORD_PTR( pt ), &im ) ||
			!( pnh = pfnImageNtHeader( (void*)im.BaseOfImage ) ) ||
			pnh->FileHeader.TimeDateStamp != im.TimeDateStamp ||
			pnh->FileHeader.SizeOfOptionalHeader >= IMAGE_SIZEOF_NT_OPTIONAL_HEADER &&
			pnh->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC &&
			pnh->OptionalHeader.CheckSum != im.CheckSum &&
			(	pnh->FileHeader.TimeDateStamp != im.TimeDateStamp ||
				_stricmp( im.ModuleName, "kernel32" ) &&
				_stricmp( im.ModuleName, "ntdll" ) ) )
		{
		return fFalse;
		}

	//  try forever until we manage to retrieve the entire undecorated symbol
	//  corresponding to this address

	SIZE_T cbBuf = 1024;
	BYTE* rgbBuf = (BYTE*)LocalAlloc( 0, cbBuf );
	if ( !rgbBuf )
		{
		return fFalse;
		}

	IMAGEHLP_SYMBOL* pis;
	do	{
		DWORD_PTR	dwT;
		DWORD_PTR*	pdwDisp	= pdwOffset ? pdwOffset : &dwT;

		pis							= (IMAGEHLP_SYMBOL*)rgbBuf;
		pis->SizeOfStruct			= sizeof( IMAGEHLP_SYMBOL );
		pis->MaxNameLength			= DWORD( cbBuf - sizeof( IMAGEHLP_SYMBOL ) );

		DWORD	symoptOld	= pfnSymSetOptions( symopt );
		BOOL	fSuccess	= pfnSymGetSymFromAddr( ghDbgProcess, DWORD_PTR( pt ), pdwDisp, pis );
		DWORD	symoptNew	= pfnSymSetOptions( symoptOld );

		if ( !fSuccess )
			{
			LocalFree( (void*)rgbBuf );
			return fFalse;
			}

		if ( strlen( pis->Name ) == cbBuf - 1 )
			{
			LocalFree( (void*)rgbBuf );
			cbBuf *= 2;
			if ( !( rgbBuf = (BYTE*)LocalAlloc( 0, cbBuf ) ) )
				{
				return fFalse;
				}
			}
		}
	while ( strlen( pis->Name ) == cbBuf - 1 );

	//  undecorate the symbol (if possible).  if not, use the decorated symbol

	char* szSymbol = (char*)LocalAlloc( 0, cchMax );
	if ( !szSymbol )
		{
		LocalFree( (void*)rgbBuf );
		return fFalse;
		}

	if ( !pfnUnDecorateSymbolName( pis->Name, szSymbol, DWORD( cchMax ), UNDNAME_COMPLETE ) )
		{
		strncpy( szSymbol, pis->Name, size_t( cchMax ) );
		szGlobal[ cchMax - 1 ] = 0;
		}

	//  write the module!symbol into the user's buffer

	_snprintf( szGlobal, size_t( cchMax ), "%s!%s", im.ModuleName, szSymbol );

	LocalFree( (void*)szSymbol );
	LocalFree( (void*)rgbBuf );
	return fTrue;
	}

//  ================================================================
template< class T >
INLINE BOOL FReadVariable( T* const rgtDebuggee, T* const rgt, const SIZE_T ct = 1 )
//  ================================================================
	{
	return ExtensionApis.lpReadProcessMemoryRoutine(
				(ULONG_PTR)rgtDebuggee,
				(VOID *)rgt,
				DWORD( sizeof( T ) * ct ),
				NULL );
	}

//  ================================================================
template< class T >
LOCAL BOOL FFetchVariable( T* const rgtDebuggee, T** const prgt, SIZE_T ct = 1 )
//  ================================================================
	{
	//  allocate enough storage to retrieve the requested type array

	if ( !( *prgt = (T*)LocalAlloc( 0, sizeof( T ) * ct ) ) )
		{
		return fFalse;
		}

	//  retrieve the requested type array

	if ( !FReadVariable( rgtDebuggee, *prgt, ct ) )
		{
		LocalFree( (VOID *)*prgt );
		return fFalse;
		}

	return fTrue;
	}

//  ================================================================
template< class T >
LOCAL BOOL FReadGlobal( const CHAR * const szGlobal, T* const rgt, const SIZE_T ct = 1 )
//  ================================================================
	{
	//  get the address of the global in the debuggee and fetch it

	T*	rgtDebuggee;

	if ( FAddressFromGlobal( szGlobal, &rgtDebuggee )
		&& FReadVariable( rgtDebuggee, rgt, ct ) )
		{
		return fTrue;
		}
	else
		{
		dprintf( "Error: Could not read global variable '%s'.\n", szGlobal );
		return fFalse;
		}
	}

//  ================================================================
template< class T >
LOCAL BOOL FReadGlobalAndFetchVariable( const CHAR * const szGlobal, T** const prgt, const SIZE_T ct = 1 )
//  ================================================================
	{
	//  get the address of the global in the debuggee and fetch its contents

	T*	rgtDebuggee;

	if ( FReadGlobal( szGlobal, &rgtDebuggee ) )
		{
		if ( FFetchVariable( rgtDebuggee, prgt, ct ) )
			return fTrue;
		else
			dprintf( "Error: Could not fetch global variable '%s'.\n", szGlobal );
		}

	return fFalse;
	}

//  ================================================================
template< class T >
LOCAL BOOL FFetchGlobal( const CHAR * const szGlobal, T** const prgt, SIZE_T ct = 1 )
//  ================================================================
	{
	//  get the address of the global in the debuggee and fetch it

	T*	rgtDebuggee;

	if ( FAddressFromGlobal( szGlobal, &rgtDebuggee )
		&& FFetchVariable( rgtDebuggee, prgt, ct ) )
		{
		return fTrue;
		}
	else
		{
		dprintf( "Error: Could not fetch global variable '%s'.\n", szGlobal );
		return fFalse;
		}
	}

//  ================================================================
template< class T >
LOCAL BOOL FFetchSz( T* const szDebuggee, T** const psz )
//  ================================================================
	{
	//  scan for the null terminator in the debuggee starting at the given
	//  address to get the size of the string

	const SIZE_T	ctScan				= 256;
	const SIZE_T	cbScan				= ctScan * sizeof( T );
	BYTE			rgbScan[ cbScan ];
	T*				rgtScan				= (T*)rgbScan;  //  because T can be const
	SIZE_T			itScan				= -1;
	SIZE_T			itScanLim			= 0;

	do	{
		if ( !( ++itScan % ctScan ) )
			{
			ULONG	cbRead;
			ExtensionApis.lpReadProcessMemoryRoutine(
								ULONG_PTR( szDebuggee + itScan ),
								(void*)rgbScan,
								cbScan,
								&cbRead );

			itScanLim = itScan + cbRead / sizeof( T );
			}
		}
	while ( itScan < itScanLim && rgtScan[ itScan % ctScan ] );

	//  we found a null terminator

	if ( itScan < itScanLim )
		{
		//  fetch the string using the determined string length

		return FFetchVariable( szDebuggee, psz, itScan + 1 );
		}

	//  we did not find a null terminator

	else
		{
		//  fail the operation

		return fFalse;
		}
	}

//  ================================================================
template< class T >
LOCAL void Unfetch( T* const rgt )
//  ================================================================
	{
	LocalFree( (void*)rgt );
	}

template <class T>
class FetchWrap
	{
	private:
		T m_t;
		FetchWrap &operator=( FetchWrap const & ); // forbidden

	public:
		FetchWrap() { m_t = NULL; }
		~FetchWrap() { Unfetch(); }
		BOOL FVariable( T const rgtDebuggee, SIZE_T ct = 1 ) { Unfetch(); return FFetchVariable( rgtDebuggee, &m_t, ct ); }
		BOOL FGlobal( const char * const szGlobal, SIZE_T ct = 1 ) { Unfetch(); return FFetchGlobal( szGlobal, &m_t, ct ); }
		BOOL FSz( T const szDebuggee ) { Unfetch(); return FFetchSz( szDebuggee, &m_t ); }
		VOID Unfetch() { OSSYM::Unfetch( m_t ); }
		T Release() { T t = m_t; m_t = NULL; return t; }	//		dereference the pointer, so the user will take care to Unfetch

		operator T() { return m_t; }
		T operator->() { return m_t; }
	};

}  //  namespace OSSYM


#define FCall( x, szError ) { if ( !x ) { dprintf szError; goto HandleError; } }
#define FCallR( x, szError ) { if ( !x ) { dprintf szError; return; } }

using namespace OSSYM;


//  ================================================================
BOOL FDuplicateHandle( const HANDLE hSourceProcess, const HANDLE hSource, HANDLE * const phDest )
//  ================================================================
	{
	const HANDLE hDestinationProcess	= GetCurrentProcess();
	const DWORD dwDesiredAccess			= GENERIC_READ;
	const BOOL bInheritHandle			= FALSE;
	const DWORD dwOptions				= 0;

	const BOOL fSuccess = DuplicateHandle(
			hSourceProcess,
			hSource,
			hDestinationProcess,
			phDest,
			dwDesiredAccess,
			bInheritHandle,
			dwOptions );

	if( !fSuccess )
		{
		dprintf( "DuplicateHandle failed with error %d\n", GetLastError() );
		}
	return fSuccess;
	}


//  ================================================================
const CHAR * SzEDBGHexDump( const VOID * const pv, const INT cb )
//  ================================================================
//
//	WARNING: not multi-threaded safe
//
	{
	static CHAR rgchBuf[1024];
	rgchBuf[0] = '\n';
	rgchBuf[1] = '\0';

	if( NULL == pv )
		{
		return rgchBuf;
		}

	DBUTLSprintHex(
		rgchBuf,
		(BYTE *)pv,
		cb,
		cb + 1,
		4,
		0,
		0 );

	return rgchBuf;
	}


//  ================================================================
DEBUG_EXT( EDBGVersion )
//  ================================================================
	{
	dprintf(	"%s version %d.%02d.%04d.%04d (%s)\n",
				SzUtilImageVersionName(),
				DwUtilImageVersionMajor(),
				DwUtilImageVersionMinor(),
				DwUtilImageBuildNumberMajor(),
				DwUtilImageBuildNumberMinor(),
				SzUtilImageBuildClass() );
	dprintf(	"\tDAE version		= 0x%x.%x\n",
				ulDAEVersion,
				ulDAEUpdate );
	dprintf(	"\tLog version		= %d.%04d.%02d\n",
				ulLGVersionMajor,
				ulLGVersionMinor,
				ulLGVersionUpdate );
	dprintf(	"\t     cbPage		= 0x%x, %d\n",
				g_cbPage,
				g_cbPage );

#ifdef MEM_CHECK
	if ( g_fMemCheck )
		{
		dprintf(	"\tMemory allocation tracking is Enabled\n" );
		}
	else
		{
		dprintf(	"\tMemory allocation tracking is Disabled\n" );
		}
#endif	//	MEM_CHECK
	}


//  ================================================================
DEBUG_EXT( EDBGDebug )
//  ================================================================
	{
	if( fDebugMode )
		{
		dprintf( "changing to non-debug mode\n" );
		fDebugMode = fFalse;
		}
	else
		{
		dprintf( "changing to debug mode\n" );
		fDebugMode = fTrue;
		}
	}


//  ================================================================
DEBUG_EXT( EDBGTest )
//  ================================================================
	{
	dprintf( "================================================================\n" );
	dprintf( "fDebugMode = %d\n", fDebugMode );
	dprintf( "fInit = %d\n", fInit );
	dprintf( "ExtensionApis.nSize = %d\n", ExtensionApis.nSize );
	dprintf( "\n" );

	dprintf( "sizeof PIB = %d (0x%04x)\n", sizeof(PIB), sizeof(PIB) );
	dprintf( "sizeof FCB = %d (0x%04x)\n", sizeof(FCB), sizeof(FCB) );
	dprintf( "sizeof TDB = %d (0x%04x)\n", sizeof(TDB), sizeof(TDB) );
	dprintf( "sizeof IDB = %d (0x%04x)\n", sizeof(IDB), sizeof(IDB) );
	dprintf( "sizeof SCB = %d (0x%04x)\n", sizeof(SCB), sizeof(SCB) );
	dprintf( "sizeof RCE = %d (0x%04x)\n", sizeof(RCE), sizeof(RCE) );
	dprintf( "sizeof VER = %d (0x%04x)\n", sizeof(VER), sizeof(VER) );
	dprintf( "sizeof LOG = %d (0x%04x)\n", sizeof(LOG), sizeof(LOG) );
	dprintf( "sizeof FMP = %d (0x%04x)\n", sizeof(FMP), sizeof(FMP) );
	dprintf( "sizeof CSR = %d (0x%04x)\n", sizeof(CSR), sizeof(CSR) );
	dprintf( "sizeof FUCB = %d (0x%04x)\n", sizeof(FUCB), sizeof(FUCB) );
	dprintf( "sizeof INST = %d (0x%04x)\n", sizeof(INST), sizeof(INST) );
	dprintf( "\n" );

	LONG 	cbPage;
	if ( FReadGlobal( "g_cbPage", &cbPage ) )
		{
		dprintf( "g_cbPage = %d\n", cbPage );
		}
	else
		{
		dprintf( "g_cbPage = [Error: cannot determine the page size of debuggee.]\n" );
		}
	dprintf( "\n" );

	if( NULL != rgEDBGGlobalsDebugger )
		{
		dprintf( "Globals table has been loaded internally.\n" );
		}
	else
		{
		dprintf( "Globals table has NOT been loaded internally.\n" );
		}
	dprintf( "\n" );

	if( NULL != hLibrary )
		{
		dprintf( "Library has been loaded internally.\n" );
		}
	else
		{
		dprintf( "Library has NOT been loaded internally.\n" );
		}
	dprintf( "\n" );

	if ( argc >= 1 )
		{
		void* pv;
		if ( FAddressFromGlobal( argv[ 0 ], &pv ) )
			{
			dprintf( "The address of %s is 0x%p.\n", argv[0], pv );
			}
		else
			{
			dprintf( "Could not find the symbol.\n" );
			}
		}
	if ( argc >= 2 )
		{
		void* pv;
		if ( FAddressFromSz( argv[ 1 ], &pv ) )
			{
			char		szGlobal[ 1024 ];
			DWORD_PTR	dwOffset;
			if ( FGlobalFromAddress( pv, szGlobal, sizeof( szGlobal ), &dwOffset ) )
				{
				dprintf(	"The symbol closest to 0x%p is %s+0x%I64X.\n",
							pv,
							szGlobal,
							QWORD( dwOffset ) );
				}
			else
				{
				dprintf( "Could not map this address to a symbol.\n" );
				}
			}
		else
			{
			dprintf( "That is not a valid address.\n" );
			}
		}

	dprintf( "================================================================\n" );
	dprintf( "\n" );
	}


LOCAL VOID EDBGUnloadGlobals()
	{
	if ( NULL != rgEDBGGlobalsDebugger )
		{
		for ( SIZE_T i = 0; i < (SIZE_T)rgEDBGGlobalsDebugger[0].pvAddress; i++ )
			{
			Unfetch( rgEDBGGlobalsDebugger[i].szName );
			}
		Unfetch( rgEDBGGlobalsDebugger );
		rgEDBGGlobalsDebugger = NULL;
		}
	}

//  ================================================================
DEBUG_EXT( EDBGGlobals )
//  ================================================================
	{
	EDBGGLOBALVAR *		rgEDBGGlobalsDebuggee	= NULL;

	dprintf( "\n" );

	if ( argc > 1
		|| ( 1 == argc && !FAddressFromSz( argv[0], &rgEDBGGlobalsDebuggee ) )
		|| ( 0 == argc && NULL == rgEDBGGlobalsDebugger ) )
		{
		if ( NULL == rgEDBGGlobalsDebugger )
			dprintf( "Globals table not currently loaded.\n\n" );

		dprintf( "Usage: GLOBALS [<rgEDBGGlobals>]\n" );
		dprintf( "\n" );
		dprintf( "       Loads the debuggee's table of globals (for use when symbols are not present or inaccurate).\n" );
		dprintf( "       If <rgEDBGGlobals> is inaccessible, it may also be found in each INST as m_rgEDBGGlobals.\n" );
		dprintf( "       If <rgEDBGGlobals> is not specified, the current globals table is dumped.\n\n" );
		return;
		}

	if ( 1 == argc )
		{
		EDBGGLOBALVAR		globalvar;
		SIZE_T				cGlobals;
		CHAR *				szName					= NULL;

		//	free any previous copy of the globals table
		EDBGUnloadGlobals();

		if ( FReadVariable( rgEDBGGlobalsDebuggee, &globalvar )
			&& FReadVariable( (SIZE_T *)globalvar.pvAddress, &cGlobals )
			&& FFetchSz( (CHAR *)globalvar.szName, &szName )
			&& 0 == _stricmp( szName, "cEDBGGlobals" )
			&& FFetchVariable( rgEDBGGlobalsDebuggee, &rgEDBGGlobalsDebugger, cGlobals ) )
			{
			//	SPECIAL-CASE: for first entry, store count of globals
			rgEDBGGlobalsDebugger[0].szName = szName;
			rgEDBGGlobalsDebugger[0].pvAddress = (VOID *)cGlobals;

			for ( SIZE_T i = 1; i < cGlobals; i++ )
				{
				if ( !FFetchSz( (CHAR *)rgEDBGGlobalsDebugger[i].szName, &szName ) )
					{
					dprintf( "Error: Failed loading globals table.\n" );

					//	free only the entries we've already updated
					rgEDBGGlobalsDebugger[0].pvAddress = (VOID *)i;
					EDBGUnloadGlobals();
					break;
					}

				//	update the debugger's copy of the globals table entry
				//	to store its own copy of the variable name
				rgEDBGGlobalsDebugger[i].szName = szName;
				}

			if ( NULL != rgEDBGGlobalsDebugger )
				dprintf( "Successfully loaded Globals table from 0x%p\n", rgEDBGGlobalsDebuggee );
			}
		else
			{
			dprintf( "Error: Failed loading globals table.\n" );
			Unfetch( szName );
			}
		}

	else
		{
		dprintf( "Globals table: 0x%p\n", rgEDBGGlobalsDebugger );
		for ( SIZE_T i = 0; i < (SIZE_T)rgEDBGGlobalsDebugger[0].pvAddress; i++ )
			{
			dprintf( "    %-24s: 0x%p\n", rgEDBGGlobalsDebugger[i].szName, rgEDBGGlobalsDebugger[i].pvAddress );
			}
		}

	dprintf( "\n--------------------\n\n" );

	}


//  ================================================================
DEBUG_EXT( EDBGLoad )
//  ================================================================
	{
	hLibrary = LoadLibrary( SzUtilImagePath() );
	if( NULL == hLibrary )
		{
		dprintf( "Unable to load %s!\n", SzUtilImagePath() );
		}
	else
		{
		//	free any previous copy of the globals table
		EDBGUnloadGlobals();
		dprintf( "Library successfully loaded internally: %s\n", SzUtilImagePath() );
		}
	}


//  ================================================================
DEBUG_EXT( EDBGUnload )
//  ================================================================
	{
	if( NULL != hLibrary )
		{
		EDBGUnloadGlobals();
		FreeLibrary( hLibrary );
		}
	else
		{
		dprintf( "%s not loaded!", SzUtilImagePath() );
		}
	}


//  ================================================================
DEBUG_EXT( EDBGErr )
//  ================================================================
	{
	LONG lErr;
	if( 1 == argc
		&& FUlFromSz( argv[0], (ULONG *)&lErr ) )
		{
		const CHAR * szError;
		const CHAR * szErrorText;
		JetErrorToString( lErr, &szError, &szErrorText );
		dprintf( "0x%x, %d: %s (%s)\n", lErr, lErr, szError, szErrorText );
		}
	else
		{
		dprintf( "Usage: ERR <error>\n" );
		}
	}


//  ================================================================
DEBUG_EXT( EDBGHelp )
//  ================================================================
	{
	INT ifuncmap;
	for( ifuncmap = 0; ifuncmap < cfuncmap; ifuncmap++ )
		{
		dprintf( "\t%s\n", rgfuncmap[ifuncmap].szHelp );
		}
	dprintf( "\n--------------------\n\n" );
	}


//  ================================================================
DEBUG_EXT( EDBGFindRes )
//  ================================================================
	{
	CRES*	pcresDebuggee	= NULL;
	CRES*	pcres			= NULL;
	VOID**	rgpvDebuggee	= NULL;
	VOID**	rgpv			= NULL;
	DWORD	cpv;

	if (	argc != 3 ||
			!FAddressFromSz( argv[0], &pcresDebuggee ) ||
			!FAddressFromSz( argv[1], &rgpvDebuggee ) ||
			!FUlFromSz( argv[2], &cpv ) )
		{
		dprintf( "Usage: DUMP <cres> <address> <length>\n" );
		return;
		}

	if ( FFetchVariable( pcresDebuggee, &pcres )
		&& FFetchVariable( rgpvDebuggee, &rgpv, cpv ) )
		{
		dprintf(	"pbMin = 0x%p, pbMax = 0x%p, cbBlock = 0x%x\n",
					pcres->PbMin(),
					pcres->PbMax(),
					pcres->CbBlock() );

		for ( SIZE_T ipv = 0; ipv < cpv; ipv++ )
			{
			if (	(BYTE*)rgpv[ ipv ] >= pcres->PbMin() &&
					(BYTE*)rgpv[ ipv ] < pcres->PbMax() )
				{
				//  this is possibly a pointer to something in the CRES

				dprintf( "0x%p  0x%p ", rgpvDebuggee + ipv, rgpv[ipv] );

				void* pvAligned = (void*)( pcres->PbMin() + ( ( (BYTE*)rgpv[ ipv ] - pcres->PbMin() ) / pcres->CbBlock() ) * pcres->CbBlock() );
				if ( pvAligned == rgpv[ ipv ] )
					{
					dprintf( "(aligned)\n" );
					}
				else
					{
					dprintf( "(not-aligned, possibly 0x%p)\n", pvAligned );
					}
				}
			}
		}

	Unfetch( rgpv );
	Unfetch( pcres );
	}


namespace OSSYNC
	{
	VOID OSSYNCAPI OSSyncDebuggerExtension(
	    HANDLE hCurrentProcess,
	    HANDLE hCurrentThread,
	    DWORD dwCurrentPc,
	    PWINDBG_EXTENSION_APIS lpExtensionApis,
		const INT argc,
	    const CHAR * const argv[] );
	};


//  ================================================================
DEBUG_EXT( EDBGSync )
//  ================================================================
	{
	OSSyncDebuggerExtension(	hCurrentProcess,
								hCurrentThread,
								dwCurrentPc,
								lpExtensionApis,
								argc,
								argv );
	}


//  ================================================================
DEBUG_EXT( EDBGCacheFind )
//  ================================================================
	{
	ULONG		ifmp;
	ULONG		pgno;
	PBF			pbf					= pbfNil;
	BF **		rgpbfChunkDebuggee	= NULL;
	BF **		rgpbfChunkT			= NULL;
	ULONG		cbfChunkDebuggee;
	LONG_PTR	cbfChunkT;
	ULONG		cbfCacheDebuggee;
	LONG_PTR	cbfCacheT;
	BOOL		fValidUsage;

	switch ( argc )
		{
		case 2:
			fValidUsage = ( FUlFromSz( argv[0], &ifmp )
							&& FUlFromSz( argv[1], &pgno )
							&& pgno > 0 );
			break;

		case 5:
			fValidUsage = ( FUlFromSz( argv[0], &ifmp )
							&& FUlFromSz( argv[1], &pgno )
							&& pgno > 0
							&& FAddressFromSz( argv[2], &rgpbfChunkDebuggee )
							&& FUlFromSz( argv[3], &cbfChunkDebuggee )
							&& FUlFromSz( argv[4], &cbfCacheDebuggee ) );
			break;

		default:
			fValidUsage = fFalse;
			break;
		}

	dprintf( "\n" );

	if ( !fValidUsage )
		{
		dprintf( "Usage: CACHEFIND <ifmp> <pgno> [<rgpbfChunk> <cbfChunk> <cbfCache>]\n\n" );
		dprintf( "    <ifmp> is the index to the FMP entry for the desired database file\n" );
		dprintf( "    <pgno> is the desired page number from this FMP\n\n" );
		return;
		}

	if ( NULL == rgpbfChunkDebuggee )
		{
		if ( !FReadGlobal( "cbfChunk", &cbfChunkT )
			|| !FReadGlobal( "cbfCache", &cbfCacheT )
			|| !FReadGlobal( "rgpbfChunk", &rgpbfChunkDebuggee ) )
			{
			dprintf( "Error: Could not load BF parameters.\n\n" );
			return;
			}
		}
	else
		{
		cbfChunkT = cbfChunkDebuggee;
		cbfCacheT = cbfCacheDebuggee;
		}

	if ( !FFetchVariable( rgpbfChunkDebuggee, &rgpbfChunkT, cCacheChunkMax ) )
		{
		dprintf( "Error: Could not load BF parameters.\n\n" );
		return;
		}

	pbf = (PBF)LocalAlloc( 0, sizeof(BF) );
	if ( NULL == pbf )
		{
		dprintf( "Error: Could not allocate BF buffer.\n" );
		Unfetch( rgpbfChunkT );
		return;
		}

	//  scan all valid BFs looking for this IFMP / PGNO

	BOOL fFoundBF = fFalse;
	for ( LONG_PTR ibf = 0; ibf < cbfCacheT; ibf++ )
		{
		//  compute the address of the target BF

		PBF pbfDebuggee = rgpbfChunkT[ ibf / cbfChunkT ] + ibf % cbfChunkT;

		//  we failed to read this BF

		if ( !FReadVariable( pbfDebuggee, pbf ) )
			{
			dprintf( "Error: Could not read BF at 0x%p. Aborting\n", pbfDebuggee );
			fFoundBF = fTrue;
			break;
			}

		//  this BF contains this IFMP / PGNO

		if (	pbf->ifmp == IFMP( ifmp ) &&
				pbf->pgno == PGNO( pgno ) &&
				( pbf->fCurrentVersion || pbf->fOlderVersion ) )
			{
			dprintf(	"%X:%08X is cached in BF 0x%p (CurrentVersion==%s, pv==0x%p).\n",
						ifmp,
						pgno,
						pbfDebuggee,
						pbf->fCurrentVersion ? "TRUE" : "FALSE",
						pbf->pv );
			fFoundBF = fTrue;
			}
		}

	//  we did not find the IFMP / PGNO

	if ( fFoundBF )
		{
		dprintf( "\n" );
		}
	else
		{
		dprintf( "%X:%08X is not cached.\n\n", ifmp, pgno );
		}

	//  unload BF parameters

	LocalFree( pbf );
	Unfetch( rgpbfChunkT );
	}

//  ================================================================
DEBUG_EXT( EDBGCacheFindOldest )
//  ================================================================
	{
	ULONG		ifmp;
	ULONG		lgen				= 0;
	PBF			pbf					= pbfNil;
	BF **		rgpbfChunkDebuggee	= NULL;
	BF **		rgpbfChunkT			= NULL;
	ULONG		cbfChunkDebuggee;
	LONG_PTR	cbfChunkT;
	ULONG		cbfCacheDebuggee;
	LONG_PTR	cbfCacheT;
	BOOL		fValidUsage;
	PBF			pbfOldestBegin0		= pbfNil;
	LGPOS		lgposOldestBegin0	= lgposMax;
	BOOL		fFoundTargetGen		= fFalse;

	switch ( argc )
		{
		case 1:
			fValidUsage = ( FUlFromSz( argv[0], &ifmp ) );
			break;

		case 2:
			fValidUsage = ( FUlFromSz( argv[0], &ifmp )
							&& FUlFromSz( argv[1], &lgen )
							&& lgen > 0 );
			break;

		case 4:
			fValidUsage = ( FUlFromSz( argv[0], &ifmp )
							&& FAddressFromSz( argv[2], &rgpbfChunkDebuggee )
							&& FUlFromSz( argv[3], &cbfChunkDebuggee )
							&& FUlFromSz( argv[4], &cbfCacheDebuggee ) );
			break;

		case 5:
			fValidUsage = ( FUlFromSz( argv[0], &ifmp )
							&& FUlFromSz( argv[1], &lgen )
							&& lgen > 0
							&& FAddressFromSz( argv[2], &rgpbfChunkDebuggee )
							&& FUlFromSz( argv[3], &cbfChunkDebuggee )
							&& FUlFromSz( argv[4], &cbfCacheDebuggee ) );
			break;

		default:
			fValidUsage = fFalse;
			break;
		}

	dprintf( "\n" );

	if ( !fValidUsage )
		{
		dprintf( "Usage: CACHEFINDOLDEST <ifmp> [<gen>] [<rgpbfChunk> <cbfChunk> <cbfCache>]\n" );
		dprintf( "\n" );
		dprintf( "    <ifmp> - index to the FMP entry for the desired database file\n" );
		dprintf( "    <gen>  - find all BF's with lgposBegin0 less than or equal to\n" );
		dprintf( "             specified log generation\n" );
		return;
		}

	if ( NULL == rgpbfChunkDebuggee )
		{
		if ( !FReadGlobal( "cbfChunk", &cbfChunkT )
			|| !FReadGlobal( "cbfCache", &cbfCacheT )
			|| !FReadGlobal( "rgpbfChunk", &rgpbfChunkDebuggee ) )
			{
			dprintf( "Error: Could not load BF parameters.\n" );
			return;
			}
		}
	else
		{
		cbfChunkT = cbfChunkDebuggee;
		cbfCacheT = cbfCacheDebuggee;
		}

	if ( !FFetchVariable( rgpbfChunkDebuggee, &rgpbfChunkT, cCacheChunkMax ) )
		{
		dprintf( "Error: Could not load BF parameters.\n" );
		return;
		}

	pbf = (PBF)LocalAlloc( 0, sizeof(BF) );
	if ( NULL == pbf )
		{
		dprintf( "Error: Could not allocate BF buffer.\n" );
		goto HandleError;
		}

	//  scan all valid BFs looking for the oldest lgposBegin0 of this IFMP

	if ( lgen > 0 )
		{
		dprintf(
			"Scanning all BF's for ifmp 0x%x with lgposBegin0 less than or equal to generation %d (0x%x):\n",
			ifmp,
			lgen,
			lgen );
		}
	else
		{
		dprintf( "Scanning all BF's...\n" );
		}

	for ( LONG_PTR ibf = 0; ibf < cbfCacheT; ibf++ )
		{
		//  compute the address of the target BF

		PBF pbfDebuggee = rgpbfChunkT[ ibf / cbfChunkT ] + ibf % cbfChunkT;

		//  we failed to read this BF

		if ( !FReadVariable( pbfDebuggee, pbf ) )
			{
			dprintf( "Error: Could not read BF at 0x%p. Aborting.\n", pbfDebuggee );
			goto HandleError;
			}

		//  compare lgposBegin0 of this IFMP / PGNO

		if ( pbf->ifmp == IFMP( ifmp )
			&& ( pbf->fCurrentVersion || pbf->fOlderVersion ) )
			{
			if ( lgen > 0 )
				{
				if ( pbf->lgposOldestBegin0.lGeneration <= lgen )
					{
					dprintf(
						"    BF 0x%p: gen %d (0x%x)\n",
						pbfDebuggee,
						pbf->lgposOldestBegin0.lGeneration,
						pbf->lgposOldestBegin0.lGeneration );
					fFoundTargetGen = fTrue;
					}
				}

			if ( CmpLgpos( &pbf->lgposOldestBegin0, &lgposOldestBegin0 ) < 0 )
				{
				lgposOldestBegin0 = pbf->lgposOldestBegin0;
				pbfOldestBegin0 = pbfDebuggee;
 				}
			}
		}

	//	report BF with oldest Begin0

	if ( lgen > 0 && !fFoundTargetGen )
		{
		//	didn't find any BF for this ifmp
		//
		dprintf( "    <none>\n" );
		}

	if ( pbfNil != pbfOldestBegin0 )
		{
		dprintf(
			"Oldest lgposBegin0 (generation 0x%x) for ifmp 0x%x is cached in BF 0x%p.\n",
			lgposOldestBegin0.lGeneration,
			ifmp,
			pbfOldestBegin0 );
		}
	else
		{
		dprintf( "No pages cached for ifmp 0x%x.\n", ifmp );
		}

	//  unload BF parameters
HandleError:
	dprintf( "\n" );

	LocalFree( pbf );
	Unfetch( rgpbfChunkT );
	}


//  ================================================================
LOCAL BOOL FEDBGFetchTableMetaData(
	FCB * const	pfcbTableDebuggee,
	FCB **		ppfcbTable )
//  ================================================================
	{
	FCB *		pfcbTable				= pfcbNil;
	TDB *		ptdb					= ptdbNil;
	FCB *		pfcbTemplate			= pfcbNil;
	TDB *		ptdbTemplate			= ptdbNil;
	FCB *		pfcbBuffer				= pfcbNil;
	ULONG		cFieldsInitial			= 0;
	ULONG		cTemplateFieldsInitial	= 0;
	ULONG		cbAlloc					= sizeof(FCB) + sizeof(TDB);
	BOOL		fResult 				= fTrue;

	if ( !FFetchVariable( pfcbTableDebuggee, &pfcbTable ) )
		{
		dprintf( "Error: Could not fetch FCB at 0x%p.\n", pfcbTableDebuggee );
		fResult = fFalse;
		goto HandleError;
		}

	if ( !pfcbTable->FTypeTable() )
		{
		//	not actually a table FCB, so just return the FCB
		//
		*ppfcbTable = pfcbTable;
		return fTrue;
		}

	if ( !FFetchVariable( pfcbTable->Ptdb(), &ptdb ) )
		{
		dprintf( "Error: Could not fetch TDB at 0x%p.\n", pfcbTable->Ptdb() );
		fResult = fFalse;
		goto HandleError;
		}

	//	adjust buffer by size of MEMPOOL buffer
	//
	cbAlloc += ptdb->MemPool().CbBufSize();

	//	adjust buffer for initial fields
	//

	cFieldsInitial = ( ptdb->FidFixedLastInitial() + 1 - ptdb->FidFixedFirst() );
	cFieldsInitial += ( ptdb->FidVarLastInitial() + 1 - ptdb->FidVarFirst() );
	cFieldsInitial += ( ptdb->FidTaggedLastInitial() + 1 - ptdb->FidTaggedFirst() );
	cbAlloc += ( cFieldsInitial * sizeof(FIELD) );

	if ( pfcbNil != ptdb->PfcbTemplateTable() )
		{
		if ( !FFetchVariable( ptdb->PfcbTemplateTable(), &pfcbTemplate ) )
			{
			dprintf( "Error: Could not fetch FCB at 0x%p.\n", ptdb->PfcbTemplateTable() );
			fResult = fFalse;
			goto HandleError;
			}

		if ( !FFetchVariable( pfcbTemplate->Ptdb(), &ptdbTemplate ) )
			{
			dprintf( "Error: Could not fetch TDB at 0x%p.\n", pfcbTemplate->Ptdb() );
			fResult = fFalse;
			goto HandleError;
			}

		//	adjust buffer by size of template table FCB, TDB, and MEMPOOL buffer
		//
		cbAlloc += sizeof(FCB) + sizeof(TDB) + ptdbTemplate->MemPool().CbBufSize();

		//	adjust buffer for initial fields
		//
		cTemplateFieldsInitial = ( ptdbTemplate->FidFixedLastInitial() + 1 - ptdbTemplate->FidFixedFirst() );
		cTemplateFieldsInitial += ( ptdbTemplate->FidVarLastInitial() + 1 - ptdbTemplate->FidVarFirst() );
		cTemplateFieldsInitial += ( ptdbTemplate->FidTaggedLastInitial() + 1 - ptdbTemplate->FidTaggedFirst() );
		cbAlloc += ( cTemplateFieldsInitial * sizeof(FIELD) );
		}

	//	now allocate one big buffer for the table (and template table) FCB, TDB, and MEMPOOL buffer
	//
	pfcbBuffer = (FCB *)LocalAlloc( 0, cbAlloc );
	if ( NULL == pfcbBuffer )
		{
		dprintf( "Error: Could not allocate %d-byte buffer for meta-data.\n", cbAlloc );
		fResult = fFalse;
		goto HandleError;
		}

	//	copy everything to final buffer
	//
	memcpy( pfcbBuffer, pfcbTable, sizeof(FCB) );

	//	TDB allocated after the FCB
	//
	pfcbBuffer->SetPtdb( (TDB *)( pfcbBuffer + 1 ) );
	memcpy( pfcbBuffer->Ptdb(), ptdb, sizeof(TDB) );

	//	MEMPOOL buffer allocated after the TDB
	//
	pfcbBuffer->Ptdb()->MemPool().SetPbuf( (BYTE *)( pfcbBuffer->Ptdb() + 1 ) );
	if ( !FReadVariable( ptdb->MemPool().Pbuf(), pfcbBuffer->Ptdb()->MemPool().Pbuf(), ptdb->MemPool().CbBufSize() ) )
		{
		dprintf(
			"Error: Could not read %d-byte MEMPOOL buffer at 0x%p.\n",
			ptdb->MemPool().CbBufSize(),
			ptdb->MemPool().Pbuf() );
		fResult = fFalse;
		goto HandleError;
		}

	//	buffer for initial fields allocated after MEMPOOL
	//
	pfcbBuffer->Ptdb()->SetPfieldInitial( (FIELD *)( pfcbBuffer->Ptdb()->MemPool().Pbuf() + pfcbBuffer->Ptdb()->MemPool().CbBufSize() ) );
	if ( !FReadVariable( ptdb->PfieldsInitial(), pfcbBuffer->Ptdb()->PfieldsInitial(), cFieldsInitial ) )
		{
		dprintf(
			"Error: Could not read %d-byte FIELD buffer at 0x%p.\n",
			cFieldsInitial * sizeof(FIELD),
			ptdb->PfieldsInitial() );
		fResult = fFalse;
		goto HandleError;
		}

	if ( pfcbNil != pfcbTemplate )
		{
		//	template table follows derived table
		//
		FCB *	pfcbT	= (FCB *)( pfcbBuffer->Ptdb()->PfieldsInitial() + cFieldsInitial );

		pfcbBuffer->Ptdb()->SetPfcbTemplateTable( pfcbT );
		memcpy( pfcbT, pfcbTemplate, sizeof(FCB) );

		pfcbT->SetPtdb( (TDB *)( pfcbT + 1 ) );
		memcpy( pfcbT->Ptdb(), ptdbTemplate, sizeof(TDB) );

		pfcbT->Ptdb()->MemPool().SetPbuf( (BYTE *)( pfcbT->Ptdb() + 1 ) );
		if ( !FReadVariable( ptdbTemplate->MemPool().Pbuf(), pfcbT->Ptdb()->MemPool().Pbuf(), ptdbTemplate->MemPool().CbBufSize() ) )
			{
			dprintf(
				"Error: Could not read %d-byte MEMPOOL buffer at 0x%p.\n",
				ptdbTemplate->MemPool().CbBufSize(),
				ptdbTemplate->MemPool().Pbuf() );
			fResult = fFalse;
			goto HandleError;
			}

		pfcbT->Ptdb()->SetPfieldInitial( (FIELD *)( pfcbT->Ptdb()->MemPool().Pbuf() + pfcbT->Ptdb()->MemPool().CbBufSize() ) );
		if ( !FReadVariable( ptdbTemplate->PfieldsInitial(), pfcbT->Ptdb()->PfieldsInitial(), cTemplateFieldsInitial ) )
			{
			dprintf(
				"Error: Could not read %d-byte FIELD buffer at 0x%p.\n",
				cTemplateFieldsInitial * sizeof(FIELD),
				ptdbTemplate->PfieldsInitial() );
			fResult = fFalse;
			goto HandleError;
			}
		}

	//	return the buffer
	//
	*ppfcbTable = pfcbBuffer;

	//	since we're returning the buffer, ensure it doesn't get freed
	//
	pfcbBuffer = pfcbNil;


HandleError:
	LocalFree( pfcbBuffer );
	Unfetch( ptdbTemplate );
	Unfetch( pfcbTemplate );
	Unfetch( ptdb );
	Unfetch( pfcbTable );

	return fResult;
	}

//  ================================================================
LOCAL VOID EDBGDumpIndexMetaData(
	const FCB * const	pfcbIndex,
	const FCB * const	pfcbTable,
	const FCB * const	pfcbIndexDebuggee )
//  ================================================================
	{
	const TDB * const	ptdb		= pfcbTable->Ptdb();
	IDB *				pidb		= pidbNil;
	const IDXSEG *		rgidxseg;

	if ( !FFetchVariable( pfcbIndex->Pidb(), &pidb ) )
		{
		dprintf( "Error: Could not fetch IDB at 0x%p.\n", pfcbIndex->Pidb() );
		goto HandleError;
		}

	dprintf( "Index: \"%s\"", ptdb->SzIndexName( pidb->ItagIndexName(), pfcbIndex->FDerivedIndex() ) );

	if ( pfcbIndex->FDerivedIndex() )
		dprintf( " (derived)" );

	dprintf( "\n" );		
	dprintf( "    pfcb=0x%p\n", pfcbIndexDebuggee );
	dprintf( "    pidb=0x%p\n", pfcbIndex->Pidb() );
	dprintf( "    ifmp=0x%x\n", pfcbIndex->Ifmp() );
	dprintf( "    objidFDP=0x%x\n", pfcbIndex->ObjidFDP() );
	dprintf( "    pgnoFDP=0x%x\n", pfcbIndex->PgnoFDP() );
	dprintf( "    flags=0x%04x\n", pidb->FPersistedFlags() );

	dprintf( "    key segments (%d):\n", pidb->Cidxseg() );
	rgidxseg = PidxsegIDBGetIdxSeg( pidb, ptdb );
	for ( ULONG i = 0; i < pidb->Cidxseg(); i++ )
		{
		const BOOL			fDerived	= ( rgidxseg[i].FTemplateColumn() && !ptdb->FTemplateTable() );
		const FIELD * const	pfield		= ptdb->Pfield( rgidxseg[i].Columnid() );
		dprintf(
			"        %c%s (0x%08x)\n",
			( rgidxseg[i].FDescending() ? '-' : '+' ),
			ptdb->SzFieldName( pfield->itagFieldName, fDerived ),
			rgidxseg[i].Columnid() );
		}

	if ( pidb->CidxsegConditional() > 0 )
		{
		dprintf( "    conditional columns (%d):\n", pidb->CidxsegConditional() );
		rgidxseg = PidxsegIDBGetIdxSegConditional( pidb, ptdb );
		for ( ULONG i = 0; i < pidb->CidxsegConditional(); i++ )
			{
			const BOOL			fDerived	= ( rgidxseg[i].FTemplateColumn() && !ptdb->FTemplateTable() );
			const FIELD * const	pfield		= ptdb->Pfield( rgidxseg[i].Columnid() );
			dprintf(
				"        %s (0x%08x, %s)\n",
				ptdb->SzFieldName( pfield->itagFieldName, fDerived ),
				rgidxseg[i].Columnid(),
				( rgidxseg[i].FMustBeNull() ? "MustBeNull" : "MustBeNonNull" ) );
			}
		}

	dprintf( "\n" );

HandleError:
	Unfetch( pidb );
	}

//  ================================================================
const INLINE CHAR * SzColumnType( const JET_COLTYP coltyp )
//  ================================================================
	{
	CHAR * szType;

	switch ( coltyp )
		{
		case JET_coltypBit:
			szType = "Bit";
			break;
			
		case JET_coltypUnsignedByte:
			szType = "UnsignedByte";
			break;

		case JET_coltypShort:
			szType = "Short";
			break;

		case JET_coltypLong:
			szType = "Long";
			break;
			
		case JET_coltypCurrency:
			szType = "Currency";
			break;

		case JET_coltypIEEESingle:
			szType = "IEEESingle";
			break;
			
		case JET_coltypIEEEDouble:
			szType = "IEEEDouble";
			break;
			
		case JET_coltypDateTime:
			szType = "DateTime";
			break;
			
		case JET_coltypBinary:
			szType = "Binary";
			break;

		case JET_coltypText:
			szType = "Text";
			break;

		case JET_coltypLongBinary:
			szType = "LongBinary";
			break;

		case JET_coltypLongText:
			szType = "LongText";
			break;

		case JET_coltypSLV:
			szType = "SLV";
			break;

		case JET_coltypNil:
			szType = "<deleted>";
			break;
			
		default:
			szType = "<unknown>";
			break;
		}

	return szType;
	}

//  ================================================================
LOCAL VOID EDBGDumpColumnMetaData(
	const TDB * const		ptdb,
	const COLUMNID			columnid )
//  ================================================================
	{
	const FIELD * const		pfield		= ptdb->Pfield( columnid );
	const BOOL				fDeleted	= ( 0 == pfield->itagFieldName );
	const CHAR * const		szType		= ( fDeleted ? "<deleted>" : SzColumnType( pfield->coltyp ) );

	dprintf( "    0x%04x - ", FidOfColumnid( columnid ) );

	if ( fDeleted )
		{
		dprintf( "<deleted>" );
		}
	else
		{
		dprintf( "\"%s\"", ptdb->SzFieldName( pfield->itagFieldName, fFalse ) );
		}

	dprintf(
		"  [%s%s, ",
		szType,
		( FRECTextColumn( pfield->coltyp ) && usUniCodePage == pfield->cp ? " (Unicode)" : "" ) );

	if ( FFixedFid( FidOfColumnid( columnid ) ) )
		{
		dprintf( "offset=0x%04x, ", pfield->ibRecordOffset );
		}

	dprintf( "flags=0x%04x]\n", pfield->ffield );
	}

//  ================================================================
LOCAL VOID EDBGDumpTableMetaData(
	FCB * const			pfcbDebuggee,
	const TDB * const	ptdbDebuggee )
//  ================================================================
	{
	FCB *				pfcbTable	= pfcbNil;
	FCB *				pfcbIndex	= pfcbNil;

 	if ( FEDBGFetchTableMetaData( pfcbDebuggee, &pfcbTable ) )
		{
		const TDB * const	ptdb	= pfcbTable->Ptdb();

 		dprintf( "Table: \"%s\"", ptdb->SzTableName() );

		if ( pfcbTable->FDerivedTable() )
			dprintf( " (derived)" );

		dprintf( "\n" );		
		dprintf( "    pfcb=0x%p\n", pfcbDebuggee );
		dprintf( "    ptdb=0x%p\n", ptdbDebuggee );
		dprintf( "    ifmp=0x%x\n", pfcbTable->Ifmp() );
		dprintf( "    objidFDP=0x%x\n", pfcbTable->ObjidFDP() );
		dprintf( "    pgnoFDP=0x%x\n", pfcbTable->PgnoFDP() );
		dprintf( "    pfcbLV=0x%p\n", ptdb->PfcbLV() );
		dprintf( "\n" );

		//	if primary index exists, dump it
		//
		if ( pidbNil != pfcbTable->Pidb() )
			{
			EDBGDumpIndexMetaData( pfcbTable, pfcbTable, pfcbDebuggee ); 
			}

		pfcbIndex = (FCB *)LocalAlloc( 0, sizeof(FCB) );
		if ( pfcbNil == pfcbIndex )
			{
			dprintf( "Error: Could not allocate FCB buffer.\n" );
			goto HandleError;
			}

		//	dump secondary indexes
		//
		for ( FCB * pfcbT = pfcbTable->PfcbNextIndex();
			pfcbNil != pfcbT;
			pfcbT = pfcbIndex->PfcbNextIndex() )
			{
			if ( !FReadVariable( pfcbT, pfcbIndex ) )
				{
				dprintf( "Error: Could not read FCB at 0x%p.\n", pfcbT );
				goto HandleError;
				}
		
			EDBGDumpIndexMetaData( pfcbIndex, pfcbTable, pfcbT );
			}

		//	dump columns
		//
		dprintf( "Columns:\n" );

		for ( FID fid = ptdb->FidFixedFirst(); fid <= ptdb->FidFixedLast(); fid++ )
			{
			EDBGDumpColumnMetaData( ptdb, ColumnidOfFid( fid, ptdb->FTemplateTable() ) );
			}
		for ( FID fid = ptdb->FidVarFirst(); fid <= ptdb->FidVarLast(); fid++ )
			{
			EDBGDumpColumnMetaData( ptdb, ColumnidOfFid( fid, ptdb->FTemplateTable() ) );
			}
		for ( FID fid = ptdb->FidTaggedFirst(); fid <= ptdb->FidTaggedLast(); fid++ )
			{
			EDBGDumpColumnMetaData( ptdb, ColumnidOfFid( fid, ptdb->FTemplateTable() ) );
			}

		dprintf( "\n" );
		}


HandleError:
	LocalFree( pfcbIndex );
	Unfetch( pfcbTable );
	}

//  ================================================================
DEBUG_EXT( EDBGDumpMetaData )
//  ================================================================
	{
	FCB *		pfcbDebuggee		= pfcbNil;
	FCB *		pfcb				= pfcbNil;
	const BOOL	fValidUsage			= ( 1 == argc && FAddressFromSz( argv[0], &pfcbDebuggee ) );

	dprintf( "\n" );

	if ( !fValidUsage )
		{
		dprintf( "Usage: DUMPMETADATE <pfcb>\n\n" );
		dprintf( "    <pfcb> - address of FCB for which meta-data is to be dumped\n" );
		goto HandleError;
		}

	if ( !FFetchVariable( pfcbDebuggee, &pfcb ) )
		{
		dprintf( "Error: Could not fetch FCB at 0x%p.\n", pfcbDebuggee );
		goto HandleError;
		}

	if ( pfcb->FTypeTable() )
		{
		EDBGDumpTableMetaData( pfcbDebuggee, pfcb->Ptdb() );
		}
	else if ( pfcb->FTypeSecondaryIndex() )
		{
		FCB *	pfcbTable;

	 	if ( FEDBGFetchTableMetaData( pfcb->PfcbTable(), &pfcbTable ) )
	 		{
			EDBGDumpIndexMetaData( pfcb, pfcbTable, pfcbDebuggee );
	 		}
		}
	else
		{
		CHAR *	szBtree;

		if ( pfcb->FTypeDatabase() )
			{
			szBtree = "Database";
			}
		else if ( pfcb->FTypeTemporaryTable() )
			{
			szBtree = "TempTable";
			}
		else if ( pfcb->FTypeSort() )
			{
			szBtree = "Sort";
			}
		else if ( pfcb->FTypeSentinel() )
			{
			szBtree = "Sentinel";
			}
		else if ( pfcb->FTypeLV() )
			{
			szBtree = "Long-Value";
			}
		else if ( pfcb->FTypeSLVAvail() )
			{
			szBtree = "SLV Avail";
			}
		else if ( pfcb->FTypeSLVOwnerMap() )
			{
			szBtree = "SLV Owner Map";
			}
		else
			{
			szBtree = "UNKNOWN";
			}

		dprintf(
			"Btree: <%s>  [pfcb=0x%p, ifmp=0x%x, objidFDP=0x%x, pgnoFDP=0x%x]\n",
			szBtree,
			pfcbDebuggee,
			pfcb->Ifmp(),
			pfcb->ObjidFDP(),
			pfcb->PgnoFDP() );
		}

HandleError:

	//  unload parameters
	//
	dprintf( "\n--------------------\n\n" );

	Unfetch( pfcb );
	}


//  ================================================================
LOCAL BOOL FEDBGReadAndCheckFCB(
	FCB * const		pfcbDebuggee,
	FCB * const		pfcb,
	const ULONG		ulFDP,
	BOOL * const	pfFoundFCB )
//  ================================================================
	{
	const BOOL		fResult		= FReadVariable( pfcbDebuggee, pfcb );

	if ( fResult )
		{
		if ( pfcb->ObjidFDP() == ulFDP || pfcb->PgnoFDP() == ulFDP )
			{
			dprintf(
				"    0x%p [ifmp:0x%x, objidFDP:0x%x, pgnoFDP:0x%x]\n",
				pfcbDebuggee,
				pfcb->Ifmp(),
				pfcb->ObjidFDP(),
				pfcb->PgnoFDP() );
			*pfFoundFCB = fTrue;
			}
		}
	else
		{
		dprintf( "    Error: Could not read FCB at 0x%p. Aborting.\n", pfcbDebuggee );
		}

	return fResult;
	}

//  ================================================================
DEBUG_EXT( EDBGFCBFind )
//  ================================================================
	{
	ULONG		ulFDP;
	INST *		pinstTarget			= NULL;
	ULONG		cNumInstances		= 0;
	INST **		rgpinstDebuggee		= NULL;
	INST **		rgpinst				= NULL;
	INST *		pinst				= pinstNil;
	FCB *		pfcb				= pfcbNil;
	TDB *		ptdb				= ptdbNil;
	BOOL		fFoundFCB			= fFalse;
	BOOL		fValidUsage			= fTrue;

	switch ( argc )
		{
		case 2:
			fValidUsage = ( fValidUsage && FAddressFromSz( argv[1], &pinstTarget ) );
			//	FALL THROUGH to validate rest of args

		case 1:
			fValidUsage = ( fValidUsage 
							&& FUlFromSz( argv[0], &ulFDP )
							&& ulFDP > 0 );
			break;

		default:
			fValidUsage = fFalse;
		}

	if ( !fValidUsage )
		{
		dprintf( "\nUsage: FCBFIND <FDP> [<instance>]\n\n" );
		dprintf( "    <FDP> - the objidFDP or pgnoFDP to search for\n" );
		dprintf( "    <instance> - an optional parameter specifying the pointer\n" );
		dprintf( "                 to the instance for which to limit the scan\n" );
		goto HandleError;
		}

	if ( NULL != pinstTarget )
		{
		//	scan just the specified instance
		//
		cNumInstances = 1;
		}
	else
		{
		//	scan all instances (we will need to fetch g_rgpinst to do this)
		//
		cNumInstances = cMaxInstances;

		if ( !FAddressFromGlobal( "g_rgpinst", &rgpinstDebuggee )
			|| !FFetchVariable( rgpinstDebuggee, &rgpinst, cNumInstances ) )
			{
			dprintf( "\nError: Could not fetch instance table.\n" );
			goto HandleError;
			}
		}

	pinst = (INST *)LocalAlloc( 0, sizeof(INST) );
	if ( pinstNil == pinst )
		{
		dprintf( "\nError: Could not allocate INST buffer.\n" );
		goto HandleError;
		}

	pfcb = (FCB *)LocalAlloc( 0, sizeof(FCB) );
	if ( pfcbNil == pfcb )
		{
		dprintf( "\nError: Could not allocate FCB buffer.\n" );
		goto HandleError;
		}

	ptdb = (TDB *)LocalAlloc( 0, sizeof(TDB) );
	if ( ptdbNil == ptdb )
		{
		dprintf( "\nError: Could not allocate TDB buffer.\n" );
		goto HandleError;
		}

	//  scan FCB list of all INST's
	//
	for ( SIZE_T ipinst = 0; ipinst < cNumInstances; ipinst++ )
		{
		INST * pinstDebuggee	= ( NULL != pinstTarget ? pinstTarget : rgpinst[ipinst] );

		if ( pinstNil == pinstDebuggee )
			{
			continue;
			}

		dprintf( "\nScanning all FCB's of instance 0x%p...\n", pinstDebuggee );
		if ( FReadVariable( pinstDebuggee, pinst ) )
			{
			FCB *	pfcbDebuggee	= pinst->m_pfcbList;
			FCB *	pfcbNextInList	= pfcbNil;
			
			for ( pfcbDebuggee = pinst->m_pfcbList;
				pfcbNil != pfcbDebuggee;
				pfcbDebuggee = pfcbNextInList )
				{
				//	check main FCB
				//
				if ( !FEDBGReadAndCheckFCB( pfcbDebuggee, pfcb, ulFDP, &fFoundFCB ) )
					{
					goto HandleError;
					}

				//	pfcb variable will be re-used, so save
				//	off pointer to next FCB in global list
				//
				pfcbNextInList = pfcb->PfcbNextList();

				//	check possible index FCB's
				//
				for ( pfcbDebuggee = pfcb->PfcbNextIndex();
					pfcbNil != pfcbDebuggee;
					pfcbDebuggee = pfcb->PfcbNextIndex() )
					{
					if ( !FEDBGReadAndCheckFCB( pfcbDebuggee, pfcb, ulFDP, &fFoundFCB ) )
						{
						goto HandleError;
						}
					}

				//	check possible LV FCB
				//
				if ( NULL != pfcb->Ptdb() )
					{
					if ( !FReadVariable( pfcb->Ptdb(), ptdb ) )
						{
						dprintf( "    Error: Could not read TDB at 0x%p. Aborting.\n", pfcb->Ptdb() );
						goto HandleError;
						}

					if ( NULL != ptdb->PfcbLV()
						&& !FEDBGReadAndCheckFCB( ptdb->PfcbLV(), pfcb, ulFDP, &fFoundFCB ) )
						{
						goto HandleError;
						}
					}
				}
			}
		else
			{
			if ( NULL != pinstTarget )
				{
				dprintf( "    Error: Could not read INST at 0x%p. Aborting.\n", pinstTarget );
				}
			else
				{
				dprintf(
					"    g_rgpinst[0x%x]  Error: Could not read INST at 0x%p. Aborting.\n",
					ipinst,
					rgpinst[ipinst] );
				}
			goto HandleError;
			}
		}

	if ( !fFoundFCB )
		{
		//	we did not find the FCB
		//
		dprintf( "\nCould not find any FCB's with an objidFDP or pgnoFDP of %d (0x%x).\n", ulFDP, ulFDP );
		}


HandleError:

	//  unload parameters
	//
	dprintf( "\n--------------------\n\n" );

	LocalFree( ptdb );
	LocalFree( pfcb );
	LocalFree( pinst );
	Unfetch( rgpinst );
	}


//  ================================================================
DEBUG_EXT( EDBGTableFind )
//  ================================================================
	{
	const CHAR *	szTableName;
	INST *			pinstTarget			= NULL;
	ULONG			cNumInstances		= 0;
	INST **			rgpinstDebuggee		= NULL;
	INST **			rgpinst				= NULL;
	INST *			pinst				= pinstNil;
	BOOL			fFoundFCB			= fFalse;
	BOOL			fValidUsage			= fTrue;

	switch ( argc )
		{
		case 2:
			fValidUsage = ( fValidUsage && FAddressFromSz( argv[1], &pinstTarget ) );
			//	FALL THROUGH to validate rest of args

		case 1:
			szTableName = argv[0];
			break;

		default:
			fValidUsage = fFalse;
		}

	if ( !fValidUsage )
		{
		dprintf( "\nUsage: TABLEFIND <szTable> [<instance>]\n\n" );
		dprintf( "    <szTable> - name of table to search for\n" );
		dprintf( "    <instance> - an optional parameter specifying the pointer\n" );
		dprintf( "                 to the instance for which to limit the scan\n" );
		goto HandleError;
		}

	if ( NULL != pinstTarget )
		{
		//	scan just the specified instance
		//
		cNumInstances = 1;
		}
	else
		{
		//	scan all instances (we will need to fetch g_rgpinst to do this)
		//
		cNumInstances = cMaxInstances;

		if ( !FAddressFromGlobal( "g_rgpinst", &rgpinstDebuggee )
			|| !FFetchVariable( rgpinstDebuggee, &rgpinst, cNumInstances ) )
			{
			dprintf( "\nError: Could not fetch instance table.\n" );
			goto HandleError;
			}
		}

	pinst = (INST *)LocalAlloc( 0, sizeof(INST) );
	if ( pinstNil == pinst )
		{
		dprintf( "\nError: Could not allocate INST buffer.\n" );
		goto HandleError;
		}

	//  scan FCB list of all INST's
	//
	for ( SIZE_T ipinst = 0; ipinst < cNumInstances; ipinst++ )
		{
		INST * pinstDebuggee	= ( NULL != pinstTarget ? pinstTarget : rgpinst[ipinst] );

		if ( pinstNil == pinstDebuggee )
			{
			continue;
			}

		dprintf( "\nScanning all FCB's of instance 0x%p...\n", pinstDebuggee );
		if ( FReadVariable( pinstDebuggee, pinst ) )
			{
			FCB *	pfcbDebuggee	= pinst->m_pfcbList;

			while ( pfcbNil != pfcbDebuggee )
				{
				FCB *	pfcb;

				//	retrieve meta-data
				//
				if ( !FEDBGFetchTableMetaData( pfcbDebuggee, &pfcb ) )
					{
					goto HandleError;
					}

				//	see if this FCB is a match
				//
				if ( pfcb->FTypeTable()
					&& 0 == UtilCmpName( szTableName, pfcb->Ptdb()->SzTableName() ) )
					{
					dprintf(
						"    0x%p [ifmp:0x%x, objidFDP:0x%x, pgnoFDP:0x%x]\n",
						pfcbDebuggee,
						pfcb->Ifmp(),
						pfcb->ObjidFDP(),
						pfcb->PgnoFDP() );
					fFoundFCB = fTrue;
					}

				pfcbDebuggee = pfcb->PfcbNextList();
				Unfetch( pfcb );
				}
			}
		else
			{
			if ( NULL != pinstTarget )
				{
				dprintf( "    Error: Could not read INST at 0x%p. Aborting.\n", pinstTarget );
				}
			else
				{
				dprintf(
					"    g_rgpinst[0x%x]  Error: Could not read INST at 0x%p. Aborting.\n",
					ipinst,
					rgpinst[ipinst] );
				}
			goto HandleError;
			}
		}

	if ( !fFoundFCB )
		{
		//	we did not find the FCB
		//
		dprintf( "\nCould not find any FCB's with table name \"%s\".\n", szTableName );
		}


HandleError:

	//  unload parameters
	//
	dprintf( "\n--------------------\n\n" );

	LocalFree( pinst );
	Unfetch( rgpinst );
	}


//  ================================================================
DEBUG_EXT( EDBGCacheMap )
//  ================================================================
	{
	void *pvOffset;
	if ( argc == 1 && FAddressFromSz( argv[ 0 ], &pvOffset ) )
		{
		//  load BF parameters

		LONG_PTR	cbfChunkT;
		BF **		rgpbfChunkT				= NULL;
		LONG_PTR	cpgChunkT;
		VOID **		rgpvChunkT			= NULL;
		LONG		cbPageT;

		if ( !FReadGlobal( "cbfChunk", &cbfChunkT )
			|| !FReadGlobalAndFetchVariable( "rgpbfChunk", &rgpbfChunkT, cCacheChunkMax )
			|| !FReadGlobal( "cpgChunk", &cpgChunkT )
			|| !FReadGlobalAndFetchVariable( "rgpvChunk", &rgpvChunkT, cCacheChunkMax )
			|| !FReadGlobal( "g_cbPage", &cbPageT ) )
			{
			dprintf( "Error: Could not load BF parameters.\n" );
			}

		else
			{
			//  lookup this offset in both tables

			IBF ibf = ibfNil;
			for ( LONG_PTR ibfChunk = 0; ibfChunk < cCacheChunkMax; ibfChunk++ )
				{
				if (	rgpbfChunkT[ ibfChunk ] &&
						rgpbfChunkT[ ibfChunk ] <= PBF( pvOffset ) &&
						PBF( pvOffset ) < rgpbfChunkT[ ibfChunk ] + cbfChunkT  )
					{
					ibf = ibfChunk * cbfChunkT + PBF( pvOffset ) - rgpbfChunkT[ ibfChunk ];
					}
				}

			IPG ipg = ipgNil;
			for ( LONG_PTR ipgChunk = 0; ipgChunk < cCacheChunkMax; ipgChunk++ )
				{
				if (	rgpvChunkT[ ipgChunk ] &&
						rgpvChunkT[ ipgChunk ] <= pvOffset &&
						pvOffset < (BYTE*)rgpvChunkT[ ipgChunk ] + cpgChunkT * cbPageT )
					{
					ipg = ipgChunk * cpgChunkT + ( (BYTE*)pvOffset - (BYTE*)rgpvChunkT[ ipgChunk ] ) / cbPageT;
					}
				}

			//  this is a BF

			if ( ibf != ibfNil )
				{
				PBF		pbfDebuggee	= rgpbfChunkT[ ibf / cbfChunkT ] + ibf % cbfChunkT;
				void*	pvDebuggee	= (BYTE*)rgpvChunkT[ ibf / cpgChunkT ] + ( ibf % cpgChunkT ) * cbPageT;

				dprintf(	"0x%0*I64X detected as BF 0x%p\n",
							sizeof( void* ) * 2,
							QWORD( pvOffset ),
							pbfDebuggee );
				dprintf(	"\tPage Image is at 0x%p\n",
							pvDebuggee );
				}

			//  this is a page pointer

			else if ( ipg != ipgNil )
				{
				PBF		pbfDebuggee	= rgpbfChunkT[ ipg / cbfChunkT ] + ipg % cbfChunkT;
				void*	pvDebuggee	= (BYTE*)rgpvChunkT[ ipg / cpgChunkT ] + ( ipg % cpgChunkT ) * cbPageT;

				dprintf(	"0x%0*I64X detected as Page Image 0x%p\n",
							sizeof( void* ) * 2,
							QWORD( pvOffset ),
							pvDebuggee );
				dprintf(	"\tBF is at 0x%p\n",
							pbfDebuggee );
				}

			//  this is an unknown pointer

			else
				{
				dprintf(	"0x%0*I64X is not part of the cache.\n",
							sizeof( void* ) * 2,
							QWORD( pvOffset ) );
				}
			}

		//  unload BF parameters

		Unfetch( rgpbfChunkT );
		Unfetch( rgpvChunkT );
		}
	else
		{
		dprintf( "Usage: CACHEMAP <address>\n" );
		dprintf( "\n" );
		dprintf( "    <address> can be any address within a valid BF or page image\n" );
		}
	}


//  ================================================================
DEBUG_EXT( EDBGDumpCacheMap )
//  ================================================================
	{
	//  load BF parameters

	LONG_PTR	cbfChunkT;
	BF **		rgpbfChunkT				= NULL;
	LONG_PTR	cpgChunkT;
	VOID **		rgpvChunkT			= NULL;
	LONG		cbPageT;

	if ( !FReadGlobal( "cbfChunk", &cbfChunkT )
		|| !FReadGlobalAndFetchVariable( "rgpbfChunk", &rgpbfChunkT, cCacheChunkMax )
		|| !FReadGlobal( "cpgChunk", &cpgChunkT )
		|| !FReadGlobalAndFetchVariable( "rgpvChunk", &rgpvChunkT, cCacheChunkMax )
		|| !FReadGlobal( "g_cbPage", &cbPageT ) )
		{
		dprintf( "Error: Could not load BF parameters.\n" );
		}

	else
		{
		dprintf( "BF:\n" );
		for ( LONG_PTR ibfChunk = 0; ibfChunk < cCacheChunkMax && rgpbfChunkT[ ibfChunk ]; ibfChunk++ )
			{
			dprintf(	"\t[0x%0*I64X, 0x%0*I64X)\n",
						sizeof( PBF ) * 2,
						QWORD( rgpbfChunkT[ ibfChunk ] ),
						sizeof( PBF ) * 2,
						QWORD( rgpbfChunkT[ ibfChunk ] + cbfChunkT ) );
			}
		dprintf( "\n" );

		dprintf( "Page Image:\n" );
		for ( LONG_PTR ipvChunk = 0; ipvChunk < cCacheChunkMax && rgpvChunkT[ ipvChunk ]; ipvChunk++ )
			{
			dprintf(	"\t[0x%0*I64X, 0x%0*I64X)\n",
						sizeof( void* ) * 2,
						QWORD( rgpvChunkT[ ipvChunk ] ),
						sizeof( void* ) * 2,
						QWORD( (BYTE*)rgpvChunkT[ ipvChunk ] + cpgChunkT * cbPageT ) );
			}
		dprintf( "\n" );
		}

	//  unload BF parameters

	Unfetch( rgpbfChunkT );
	Unfetch( rgpvChunkT );
	}


//  ================================================================
DEBUG_EXT( EDBGTid2PIB )
//  ================================================================
	{
	ULONG		ulTid				= 0;
	INST **		rgpinstDebuggee		= NULL;
	INST **		rgpinst				= NULL;
	BOOL		fFoundPIB			= fFalse;
	BOOL		fValidUsage;

	switch ( argc )
		{
		case 1:
			fValidUsage = FUlFromSz( argv[0], &ulTid );
			break;
		case 2:
			fValidUsage = ( FAddressFromSz( argv[1], &rgpinstDebuggee )
							&& FUlFromSz( argv[0], &ulTid ) );
			break;
		default:
			fValidUsage = fFalse;
			break;
		}

	if ( !fValidUsage )
		{
		dprintf( "Usage: TID2PIB <tid> [<g_rgpinst>]\n" );
		return;
		}

	if ( ( NULL == rgpinstDebuggee && !FAddressFromGlobal( "g_rgpinst", &rgpinstDebuggee ) )
		|| !FFetchVariable( rgpinstDebuggee, &rgpinst, cMaxInstances ) )
		{
		dprintf( "Error: Could not fetch instance table.\n" );
		return;
		}

	dprintf( "\nScanning 0x%X INST's starting at 0x%p...\n", cMaxInstances, rgpinstDebuggee );

	for ( SIZE_T ipinst = 0; ipinst < cMaxInstances; ipinst++ )
		{
		if ( rgpinst[ipinst] != pinstNil )
			{
			INST* pinst;
			if ( !FFetchVariable( rgpinst[ipinst], &pinst ) )
				{
				dprintf( "Error: Could not fetch instance definition at 0x%p.\n", rgpinst[ipinst] );
				Unfetch( rgpinst );
				return;
				}

			PIB* ppibDebuggee = pinst->m_ppibGlobal;

			while ( ppibDebuggee != ppibNil )
				{
				PIB* ppib;
				if ( !FFetchVariable( ppibDebuggee, &ppib ) )
					{
					dprintf( "Error: Could not fetch PIB at 0x%p.\n", ppibDebuggee );
					Unfetch( pinst );
					Unfetch( rgpinst );
					return;
					}

				_TLS* ptlsDebuggee = (_TLS*)ppib->ptls;

				if ( ptlsDebuggee )
					{
					_TLS* ptls;
					if ( !FFetchVariable( ptlsDebuggee, &ptls ) )
						{
						dprintf( "Error: Could not fetch TLS at 0x%p.\n", ptlsDebuggee );
						Unfetch( ppib );
						Unfetch( pinst );
						Unfetch( rgpinst );
						return;
						}

					if ( ptls->dwThreadId == ulTid )
						{
						dprintf( "TID %d (0x%x) was the last user of PIB 0x%p.\n",
									ptls->dwThreadId,
									ptls->dwThreadId,
									ppibDebuggee );

						fFoundPIB = fTrue;
						}

					Unfetch( ptls );
					}

				ppibDebuggee = ppib->ppibNext;

				Unfetch( ppib );
				}

			Unfetch( pinst );
			}
		}

	if ( !fFoundPIB )
		{
		dprintf( "This thread was not the last user of any PIBs.\n" );
		}

	Unfetch( rgpinst );
	}


//  ================================================================
DEBUG_EXT( EDBGChecksum )
//  ================================================================
	{
	BYTE*	rgbDebuggee;
	ULONG	cb;

	if (	argc < 1 ||
			argc > 2 ||
			!FAddressFromSz( argv[ 0 ], &rgbDebuggee ) ||
			argc == 2 && !FUlFromSz( argv[ 1 ], &cb ) )
		{
		dprintf( "Usage: CHECKSUM <address> [<length>]\n" );
		return;
		}

	LONG	cbPage;
	if ( argc == 1 && !FReadGlobal( "g_cbPage", &cbPage ) )
		{
		dprintf( "Error: cannot determine page size of debuggee.\n" );
		return;
		}

	cb = ( argc == 1 ? cbPage : cb );

	BYTE*	rgb;
	if ( !FFetchVariable( rgbDebuggee, &rgb, cb ) )
		{
		dprintf( "Error: Could not retrieve data to checksum.\n" );
		return;
		}

	const ULONG ulChecksum = UlUtilChecksum( rgb, cb );
	dprintf(	"Checksum of 0x%X bytes starting at 0x%p is:  %u (0x%08X).\n",
				cb,
				rgbDebuggee,
				ulChecksum,
				ulChecksum );

	Unfetch( rgb );
	}


//  ================================================================
DEBUG_EXT( EDBGDumpLinkedList )
//  ================================================================
	{
	ULONG			ulOffset				= 0;
	ULONG			cDwordsToDisplay		= 8;
	ULONG			cbToFetch				= 0;
	ULONG			cMaxElements			= 32;
	ULONG			cElements				= 0;
	const ULONG		cDwordsPerLine			= 4;
	const ULONG		cDwordsToDisplayMax		= 0x1000;
	const ULONG		cMaxElementsAbsolute	= 0x8000;
	BYTE *			pbDebuggee				= NULL;
	BYTE *			rgbBuf					= NULL;
	BOOL			fValidUsage				= fTrue;

	switch ( argc )
		{
		case 4:
			fValidUsage = ( fValidUsage && FUlFromSz( argv[3], &cMaxElements ) );
			//	FALL THROUGH to validate rest of args

		case 3:
			fValidUsage = ( fValidUsage && FUlFromSz( argv[2], &cDwordsToDisplay ) );
			//	FALL THROUGH to validate rest of args

		case 2:
			fValidUsage = ( fValidUsage
							&& FAddressFromSz( argv[0], &pbDebuggee )
							&& FUlFromSz( argv[1], &ulOffset ) );
			break;

		default:
			fValidUsage = fFalse;
			break;
		}

	dprintf( "\n" );

	if ( !fValidUsage )
		{
		dprintf( "Usage: DUMPLINKEDLIST <address> <offset> [<dwords to display> <max elements>]\n" );
		dprintf( "\n" );
		dprintf( "    <address> - address of initial linked list element\n" );
		dprintf( "    <offset>  - offset (in bytes) of pointer to next linked list element\n" );
		dprintf( "    <dwords to display> - number of dwords to display per element\n" );
		dprintf( "    <max elements> - maximum linked list elements to traverse\n" );
		dprintf( "\n" );
		return;
		}

	//	for each element, we need to ensure we fetch enough
	//	to read the next pointer and to display the requested
	//	number of dwords
	//
	cbToFetch = max( ulOffset + sizeof(VOID *), cDwordsToDisplay * sizeof(DWORD) );
	rgbBuf = (BYTE *)LocalAlloc( 0, cbToFetch );
	if ( NULL == rgbBuf )
		{
		dprintf( "Error: Could not allocate %d-byte buffer for linked list elements.\n", cbToFetch );
		goto HandleError;
		}

	//	cap maximum dwords to display per element
	//	to ensure we don't dump too much
	//
	cDwordsToDisplay = min( cDwordsToDisplay, cDwordsToDisplayMax );

	//	cap maximum elements to traverse to ensure
	//	we don't loop forever
	//
	cMaxElements = min( cMaxElements, cMaxElementsAbsolute );

	//	traverse linked list and dump each element
	//
	while ( NULL != pbDebuggee
		&& cElements < cMaxElements )
		{
		if ( !FReadVariable( pbDebuggee, rgbBuf, cbToFetch ) )
			{
			dprintf( "Error: Could not fetch linked list element at 0x%p.\n", pbDebuggee );
			goto HandleError;
			}

        dprintf( "0x%p%s\n", pbDebuggee, ( cDwordsToDisplay > 0 ? ":" : ""  ) );
        for ( ULONG ib = 0; ib < cDwordsToDisplay * sizeof(DWORD); NULL )
        	{
			dprintf( "\t" );
			for ( ULONG ibMaxThisLine = min( ib + ( sizeof(DWORD) * cDwordsPerLine ), cDwordsToDisplay * sizeof(DWORD) );
				ib < ibMaxThisLine;
				ib += sizeof(DWORD) )
				{
				dprintf( "%08x ", *(ULONG *)( rgbBuf + ib ) );
				}
			dprintf( "\n" );
        	}
		cElements++;

		if ( pbDebuggee == *(BYTE **)( rgbBuf + ulOffset ) )
			{
			//	SPECIAL-CASE: next pointer points back to this element
			//	(we terminate some linked lists in this fashion)
			//
			break;
			}

		pbDebuggee = *(BYTE **)( rgbBuf + ulOffset );
		}

	dprintf( "\nElements traversed: %d\n", cElements );
	if ( NULL != pbDebuggee )
		{
		dprintf( "(End of list was not reached. Next element: 0x%p)\n", pbDebuggee );
		}

HandleError:
	dprintf( "\n" );

	LocalFree( rgbBuf );
	}


//  ================================================================
DEBUG_EXT( EDBGDumpInvasiveList )
//  ================================================================
	{
	ULONG			ulOffset				= 0;
	ULONG			cDwordsToDisplay		= 8;
	ULONG			cbToFetch				= 0;
	ULONG			cMaxElements			= 32;
	ULONG			cElements				= 0;
	BOOL			fFollowPrevPointer		= fFalse;
	const ULONG		cDwordsPerLine			= 4;
	const ULONG		cDwordsToDisplayMax		= 0x1000;
	const ULONG		cMaxElementsAbsolute	= 0x8000;
	BYTE *			pbDebuggee				= NULL;
	BYTE *			rgbBuf					= NULL;
	BOOL			fValidUsage				= fTrue;

	switch ( argc )
		{
		case 4:
			fValidUsage = ( fValidUsage && FUlFromSz( argv[3], &cMaxElements ) );
			//	FALL THROUGH to validate rest of args

		case 3:
			fValidUsage = ( fValidUsage && FUlFromSz( argv[2], &cDwordsToDisplay ) );
			//	FALL THROUGH to validate rest of args

		case 2:
			fFollowPrevPointer = ( '-' == argv[1][0] );
			fValidUsage = ( fValidUsage
							&& FAddressFromSz( argv[0], &pbDebuggee )
							&& FUlFromSz( argv[1] + ( fFollowPrevPointer ? 1 : 0 ), &ulOffset ) );
			break;

		default:
			fValidUsage = fFalse;
			break;
		}

	dprintf( "\n" );

	if ( !fValidUsage )
		{
		dprintf( "Usage: DUMPINVASIVELIST <address> [<+/->]<offset> [<dwords to display> <max elements>]\n" );
		dprintf( "\n" );
		dprintf( "    <address> - address of initial invasive list element (NOT address of invasive list head)\n" );
		dprintf( "    <+/-> - specify '+' to follow \"next\" pointer, '-' to follow \"prev\" pointer\n" );
		dprintf( "    <offset>  - offset (in bytes) of invasive list within the element\n" );
		dprintf( "    <dwords to display> - number of dwords to display per element\n" );
		dprintf( "    <max elements> - maximum invasive list elements to traverse\n" );
		dprintf( "\n" );
		return;
		}

	//	for each element, we need to ensure we fetch enough
	//	to read the next pointer and to display the requested
	//	number of dwords
	//
	cbToFetch = max( ulOffset + sizeof(CInvasiveListEDBG::CElement), cDwordsToDisplay * sizeof(DWORD) );
	rgbBuf = (BYTE *)LocalAlloc( 0, cbToFetch );
	if ( NULL == rgbBuf )
		{
		dprintf( "Error: Could not allocate %d-byte buffer for linked list elements.\n", cbToFetch );
		goto HandleError;
		}

	//	cap maximum dwords to display per element
	//	to ensure we don't dump too much
	//
	cDwordsToDisplay = min( cDwordsToDisplay, cDwordsToDisplayMax );

	//	cap maximum elements to traverse to ensure
	//	we don't loop forever
	//
	cMaxElements = min( cMaxElements, cMaxElementsAbsolute );

	//	traverse linked list and dump each element
	//
	while ( NULL != pbDebuggee
		&& cElements < cMaxElements )
		{
		if ( !FReadVariable( pbDebuggee, rgbBuf, cbToFetch ) )
			{
			dprintf( "Error: Could not fetch invasive list element at 0x%p.\n", pbDebuggee );
			goto HandleError;
			}

        dprintf( "0x%p%s\n", pbDebuggee, ( cDwordsToDisplay > 0 ? ":" : ""  ) );
        for ( ULONG ib = 0; ib < cDwordsToDisplay * sizeof(DWORD); NULL )
        	{
			dprintf( "\t" );
			for ( ULONG ibMaxThisLine = min( ib + ( sizeof(DWORD) * cDwordsPerLine ), cDwordsToDisplay * sizeof(DWORD) );
				ib < ibMaxThisLine;
				ib += sizeof(DWORD) )
				{
				dprintf( "%08x ", *(ULONG *)( rgbBuf + ib ) );
				}
			dprintf( "\n" );
        	}
		cElements++;

		pbDebuggee = *(BYTE **)(
							rgbBuf
							+ ulOffset
							+ ( fFollowPrevPointer ?
									0 :					//	OffsetOf( CInvasiveList::CElement, m_pilePrev )
									sizeof(VOID *) ) );	//	OffsetOf( CInvasiveList::CElement, m_pileNext )

		if ( NULL == pbDebuggee				//	should be impossible, but just in case
			|| (VOID *)-1 == pbDebuggee )	//	this element is not actually in an invasive list
			{
			break;
			}

		pbDebuggee -= ulOffset;
		}

	dprintf( "\nElements traversed: %d\n", cElements );
	if ( NULL != pbDebuggee )
		{
		dprintf( "(End of list was not reached. Next element: 0x%p)\n", pbDebuggee );
		}

HandleError:
	dprintf( "\n" );

	LocalFree( rgbBuf );
	}


//  ================================================================
DEBUG_EXT( EDBGDumpLR )
//  ================================================================
	{
	LR* plrDebuggee;
	if (	argc != 1 ||
			!FAddressFromSz( argv[ 0 ], &plrDebuggee ) )
		{
		dprintf( "Usage: DUMPLR <address>\n" );
		return;
		}

	//  get the fixed size of this log record

	LR	lr;
	if ( !FReadVariable( plrDebuggee, &lr ) )
		{
		dprintf( "Error: Could not fetch the log record type.\n" );
		return;
		}
	const SIZE_T	cbLRFixed	= CbLGFixedSizeOfRec( &lr );

	//  get the full size of this log record

	LR*	plrFixed;
	if ( !FFetchVariable( (BYTE*)plrDebuggee, (BYTE**)&plrFixed, cbLRFixed ) )
		{
		dprintf( "Error: Could not fetch the fixed-sized part of this log record.\n" );
		return;
		}
	const SIZE_T	cbLR		= CbLGSizeOfRec( plrFixed );

	Unfetch( (BYTE*)plrFixed );

	//  get the full log record

	LR*	plr;
	if ( !FFetchVariable( (BYTE*)plrDebuggee, (BYTE**)&plr, cbLR ) )
		{
		dprintf( "Error: Could not fetch the entire log record.\n" );
		return;
		}

	//  dump the log record

	dprintf(	"0x%0*I64X bytes @ 0x%p\n",
				sizeof( SIZE_T ) * 2,
				QWORD( cbLR ),
				plr );

#ifdef DEBUG

	CHAR szBuf[ 2048 ];
	LrToSz( plr, szBuf, NULL );
	dprintf( "%s\n", szBuf );

#endif	//	DEBUG

	Unfetch( (BYTE*)plr );
	}


//  ================================================================
DEBUG_EXT( EDBGHash )
//  ================================================================
	{
	ULONG	ifmp;
	ULONG	pgnoFDP;
	BYTE*	rgbPrefixDebuggee;
	ULONG	cbPrefix;
	BYTE*	rgbSuffixDebuggee;
	ULONG	cbSuffix;
	BYTE*	rgbDataDebuggee;
	ULONG	cbData;

	dprintf( "\n" );

	if (	argc != 8 ||
			!FUlFromSz( argv[ 0 ], &ifmp ) ||
			!FUlFromSz( argv[ 1 ], &pgnoFDP ) ||
			!FAddressFromSz( argv[ 2 ], &rgbPrefixDebuggee ) ||
			!FUlFromSz( argv[ 3 ], &cbPrefix ) ||
			!FAddressFromSz( argv[ 4 ], &rgbSuffixDebuggee ) ||
			!FUlFromSz( argv[ 5 ], &cbSuffix ) ||
			!FAddressFromSz( argv[ 6 ], &rgbDataDebuggee ) ||
			!FUlFromSz( argv[ 7 ], &cbData ) )
		{
		dprintf( "Usage: HASH <ifmp> <pgnoFDP> <pbPrefix> <cbPrefix> <pbSuffix> <cbSuffix> <pbData> <cbData>\n\n" );
		return;
		}

	BYTE*	rgbPrefix	= NULL;
	BYTE*	rgbSuffix	= NULL;
	BYTE*	rgbData		= NULL;
	if ( !FFetchVariable( rgbPrefixDebuggee, &rgbPrefix, cbPrefix )
		|| !FFetchVariable( rgbSuffixDebuggee, &rgbSuffix, cbSuffix )
		|| !FFetchVariable( rgbDataDebuggee, &rgbData, cbData ) )
		{
		dprintf( "Error: Couldn't fetch the prefix / suffix / data from the debuggee.\n" );
		}
	else
		{
		BOOKMARK bookmark;
		bookmark.key.prefix.SetPv( rgbPrefix );
		bookmark.key.prefix.SetCb( cbPrefix );
		bookmark.key.suffix.SetPv( rgbSuffix );
		bookmark.key.suffix.SetCb( cbSuffix );
		bookmark.data.SetPv( rgbData );
		bookmark.data.SetCb( cbData );

		const ULONG ulVERChecksum = UiVERHash( IFMP( ifmp ), PGNO( pgnoFDP ), bookmark );

		dprintf(	"VER checksum is:  %u (0x%08X)\n",
					ulVERChecksum,
					ulVERChecksum );
		}

	dprintf( "\n" );

	Unfetch( rgbPrefix );
	Unfetch( rgbSuffix );
	Unfetch( rgbData );
	}


//  ================================================================
LOCAL BOOL FEDBGLoadPage_(
	const IFMP	ifmp,
	const PGNO	pgno,
	BYTE *		rgbPageBuf,
	PBF			pbfBuf )
//  ================================================================
	{
	//  scan all valid BFs looking for this IFMP / PGNO
	//
	for ( LONG_PTR ibf = 0; ibf < cbfCache; ibf++ )
		{
		//  compute the address of the target BF
		//
		PBF	pbfDebuggee	= rgpbfChunk[ ibf / cbfChunk ] + ibf % cbfChunk;

		//  we failed to read this BF
		//
		if ( !FReadVariable( pbfDebuggee, pbfBuf ) )
			{
			dprintf( "Error: Could not read BF at 0x%p.\n", pbfDebuggee );
			return fFalse;
			}

		//	see if this BF contains this IFMP / PGNO
		//
		if ( pbfBuf->ifmp == IFMP( ifmp )
			&& pbfBuf->pgno == PGNO( pgno )
			&& pbfBuf->fCurrentVersion )
			{
			if ( FReadVariable( (BYTE *)pbfBuf->pv, rgbPageBuf, g_cbPage ) )
				{
				return fTrue;
				}
			else
				{
				dprintf( "Error: Could not read %d-byte PAGE at 0x%p.\n", g_cbPage, pbfBuf->pv );
				return fFalse;
				}
			}
		}

	dprintf( "Error: [%d:%d] is not cached.\n", ifmp, pgno );
	return fFalse;
	}

//  ================================================================
LOCAL VOID EDBGSeek_(
	const IFMP	ifmp,
	const PGNO	pgnoRoot,
	BOOKMARK&	bm )
//  ================================================================
	{
	CPAGE		cpage;
	INT			compare;
	INT			iline;
	BYTE *		rgbPageBuf;
	PBF			pbfBuf;

	rgbPageBuf = (BYTE *)LocalAlloc( 0, g_cbPage );
	if ( NULL == rgbPageBuf )
		{
		dprintf( "Error: Could not allocate PAGE buffer.\n" );
		return;
		}

	pbfBuf = (PBF)LocalAlloc( 0, sizeof(BF) );
	if ( NULL == pbfBuf )
		{
		dprintf( "Error: Could not allocate BF buffer.\n" );
		LocalFree( rgbPageBuf );
		return;
		}

	cpage.LoadPage( rgbPageBuf );

	dprintf( "Path to bookmark:\n" );

	if ( !FEDBGLoadPage_( ifmp, pgnoRoot, rgbPageBuf, pbfBuf ) )
		goto HandleError;

	//	now seek down the btree for the bookmark
	//
	while ( !cpage.FLeafPage() )
		{
		iline = IlineNDISeekGEQInternal( cpage, bm, &compare );

		if ( 0 == compare )
			{
			//	see ErrNDISeekInternalPage() for an
			//	explanation of why we increment by 1 here
			//
			iline++;
			}

		dprintf( "\t[%d:%d:%d]  (", ifmp, cpage.Pgno(), iline );
		if ( cpage.FRootPage() )
			dprintf( "root," );
		if ( cpage.FParentOfLeaf() )
			dprintf( "parent-of-leaf," );
		else
			dprintf( "internal," );
		dprintf( "pv==0x%p)\n", pbfBuf->pv );

		KEYDATAFLAGS	kdf;
		NDIGetKeydataflags( cpage, iline, &kdf );

		if ( sizeof(PGNO) != kdf.data.Cb() )
			{
			dprintf( "Error: bad parent page pointer.\n" );
			goto HandleError;
			}

		const PGNO	pgnoChild	= *(UnalignedLittleEndian< PGNO > *)kdf.data.Pv();

		if ( !FEDBGLoadPage_( ifmp, pgnoChild, rgbPageBuf, pbfBuf ) )
			goto HandleError;
		}

	//	find the first node in the leaf page that's
	//	greater than or equal to the specified bookmark
	//
	iline = IlineNDISeekGEQ( cpage, bm, !cpage.FNonUniqueKeys(), &compare );
	Assert( iline < cpage.Clines( ) );
	if ( iline < 0 )
		{
		//	all nodes in page are less than key
		//
		iline = cpage.Clines( ) - 1;
		}

	dprintf(
		"\t[%d:%d:%d]  (%sleaf,pv==0x%p)\n",
		ifmp,
		cpage.Pgno(),
		iline,
		( cpage.FRootPage() ? "root," : "" ),
		pbfBuf->pv );

HandleError:
	cpage.UnloadPage();
	LocalFree( pbfBuf );
	LocalFree( rgbPageBuf );
	}

//  ================================================================
DEBUG_EXT( EDBGSeek )
//  ================================================================
	{
	ULONG		ifmp;
	ULONG		pgnoRoot;
	BYTE *		rgbPrefixDebuggee	= NULL;
	BYTE *		rgbPrefix			= NULL;
	ULONG		cbPrefix			= 0;
	BYTE *		rgbSuffixDebuggee	= NULL;
	BYTE *		rgbSuffix			= NULL;
	ULONG		cbSuffix			= 0;
	BYTE *		rgbDataDebuggee		= NULL;
	BYTE *		rgbData				= NULL;
	ULONG		cbData				= 0;
	ULONG		cbPage				= 0;
	BF **		rgpbfChunkDebuggee	= NULL;
	BF **		rgpbfChunkT			= NULL;
	LONG_PTR	cbfChunkT			= 0;
	LONG_PTR	cbfCacheT			= 0;
	BOOL		fBadUsage			= fFalse;

	dprintf( "\n" );

	switch ( argc )
		{
		case 8:
			if ( !FAddressFromSz( argv[ 6 ], &rgbDataDebuggee )
				|| !FUlFromSz( argv[ 7 ], &cbData ) )
				{
				fBadUsage = fTrue;
				break;
				}
			//	FALL THROUGH
		case 6:
			if ( !FAddressFromSz( argv[ 4 ], &rgbSuffixDebuggee )
				|| !FUlFromSz( argv[ 5 ], &cbSuffix ) )
				{
				fBadUsage = fTrue;
				break;
				}
			//	FALL THROUGH
		case 4:
			if ( FUlFromSz( argv[ 0 ], &ifmp )
				&& FUlFromSz( argv[ 1 ], &pgnoRoot )
				&& FAddressFromSz( argv[ 2 ], &rgbPrefixDebuggee )
				&& FUlFromSz( argv[ 3 ], &cbPrefix ) )
				{
				break;
				}
			//	FALL THROUGH
		default:
			fBadUsage = fTrue;
		}

	if ( fBadUsage )
		{
		dprintf( "Usage: SEEK <ifmp> <pgnoRoot> <pbPrefix> <cbPrefix> [<pbSuffix> <cbSuffix> [<pbData> <cbData>]]\n" );
		}
	else if ( ( cbPrefix > 0 && !FFetchVariable( rgbPrefixDebuggee, &rgbPrefix, cbPrefix ) )
		|| ( cbSuffix > 0 && !FFetchVariable( rgbSuffixDebuggee, &rgbSuffix, cbSuffix ) )
		|| ( cbData > 0 && !FFetchVariable( rgbDataDebuggee, &rgbData, cbData ) ) )
		{
		dprintf( "Error: Couldn't fetch the bookmark from the debuggee.\n" );
		}
	else if ( !FReadGlobal( "g_cbPage", &cbPage ) )
		{
		dprintf( "Error: Could not retrieve g_cbPage from the debuggee.\n" );
		}
	else if ( !FReadGlobal( "cbfChunk", &cbfChunkT )
		|| !FReadGlobal( "cbfCache", &cbfCacheT )
		|| !FReadGlobal( "rgpbfChunk", &rgpbfChunkDebuggee )
		|| !FFetchVariable( rgpbfChunkDebuggee, &rgpbfChunkT, cCacheChunkMax ) )
		{
		dprintf( "Error: Could not load BF parameters.\n" );
		}
	else
		{
		//	set up debugger to mirror debuggee
		//
		g_cbPage = cbPage;
		SetCbPageRelated();

		cbfChunk = cbfChunkT;
		cbfCache = cbfCacheT;
		rgpbfChunk = rgpbfChunkT;

		//	perform seek to bookmark
		//
		BOOKMARK bm;
		bm.key.prefix.SetPv( rgbPrefix );
		bm.key.prefix.SetCb( cbPrefix );
		bm.key.suffix.SetPv( rgbSuffix );
		bm.key.suffix.SetCb( cbSuffix );
		bm.data.SetPv( rgbData );
		bm.data.SetCb( cbData );

		EDBGSeek_( IFMP( ifmp ), PGNO( pgnoRoot ), bm );
		}

	dprintf( "\n" );

	Unfetch( rgpbfChunkT );
	Unfetch( rgbPrefix );
	Unfetch( rgbSuffix );
	Unfetch( rgbData );
	}


//  ================================================================
DEBUG_EXT( EDBGVerStore )
//  ================================================================
	{
	INST *		pinstDebuggee			= NULL;
	INST *		pinst					= NULL;
	INST::PLS *	rgpls					= NULL;
	PIB *		ppibTrxOldestDebuggee	= NULL;
	PIB *		ppibTrxOldest			= NULL;
	VER	*		pver					= NULL;
	CRES *		pcresVER				= NULL;

	dprintf( "\n" );

	if ( argc != 1
		|| !FAddressFromSz( argv[0], &pinstDebuggee ) )
		{
		dprintf( "Usage: VERSTORE <instance id>\n" );
		dprintf( "\n" );
		dprintf( "       <instance id> is the JET_INSTANCEID of the instance\n" );
		dprintf( "       for which to dump version store information. If you\n" );
		dprintf( "       do not know the instance id, you may be able to find\n" );
		dprintf( "       it by manually scanning the instance id array, which\n" );
		dprintf( "       begins at '%s!%s' and contains '%s!%s'\n",
						SzUtilImageName(),
						VariableNameToString( g_rgpinst ),
						SzUtilImageName(),
						VariableNameToString( ipinstMac ) );
		dprintf( "       entries.\n" );
		return;
		}

	if ( !FFetchVariable( pinstDebuggee, &pinst )
		|| !FFetchVariable( pinst->m_rgpls, &rgpls, pinst->m_cpls ) )
		{
		dprintf( "Error: Could not fetch instance and/or processor-local-storage variables.\n" );
		goto HandleError;
		}
	for ( size_t iProc = 0; iProc < pinst->m_cpls; iProc++ )
		{
		PIB *	ppibTrxOldestCandidateDebuggee	= rgpls[ iProc ].m_ilTrxOldest.PrevMost();

		if ( NULL != ppibTrxOldestCandidateDebuggee )
			{
			PIB *	ppibTrxOldestCandidate;

			if ( !FFetchVariable( ppibTrxOldestCandidateDebuggee, &ppibTrxOldestCandidate ) )
				{
				dprintf( "Error: Could not fetch ppibTrxOldestCandidate from processor-local-storage.\n" );
				goto HandleError;
				}
			if ( !ppibTrxOldest
				|| TrxCmp( ppibTrxOldestCandidate->trxBegin0, ppibTrxOldest->trxBegin0 ) < 0 )
				{
				Unfetch( ppibTrxOldest );
				ppibTrxOldestDebuggee = ppibTrxOldestCandidateDebuggee;
				ppibTrxOldest = ppibTrxOldestCandidate;
				}
			else
				{
				Unfetch( ppibTrxOldestCandidate );
				}
			}
		}

	if ( FFetchVariable( pinst->m_pver, &pver )
		&& FFetchVariable( pver->m_pcresVERPool, &pcresVER ) )
		{
		SIZE_T		dwOffset;

		//	dump version store-related members of various structs

		dwOffset = (BYTE *)pinstDebuggee - (BYTE *)pinst;
		dprintf( "INSTANCE: 0x%p\n", pinstDebuggee );
		dprintf( FORMAT_POINTER( INST, pinst, m_pver, dwOffset ) );
		dprintf( FORMAT_INT( INST, pinst, m_lVerPagesMax, dwOffset ) );
		dprintf( FORMAT_INT( INST, pinst, m_lVerPagesPreferredMax, dwOffset ) );
		dprintf( FORMAT_BOOL( INST, pinst, m_fPreferredSetByUser, dwOffset ) );
		dprintf( FORMAT_INT( INST, pinst, m_trxNewest, dwOffset ) );
		dprintf( "\n" );

		if ( NULL != ppibTrxOldest )
			{
			dwOffset = (BYTE *)ppibTrxOldestDebuggee - (BYTE *)ppibTrxOldest;
			dprintf( "OLDEST TRANSACTION: 0x%p\n", ppibTrxOldestDebuggee );
			dprintf( FORMAT_UINT( PIB, ppibTrxOldest, dwTrxContext, dwOffset ) );
			dprintf( FORMAT_UINT( PIB, ppibTrxOldest, trxBegin0, dwOffset ) );
			dprintf( FORMAT_UINT( PIB, ppibTrxOldest, trxCommit0, dwOffset ) );
			dprintf( FORMAT_INT( PIB, ppibTrxOldest, level, dwOffset ) );
			dprintf( FORMAT_INT( PIB, ppibTrxOldest, levelRollback, dwOffset ) );
			dprintf( FORMAT_INT( PIB, ppibTrxOldest, levelBegin, dwOffset ) );
			dprintf( FORMAT_INT( PIB, ppibTrxOldest, clevelsDeferBegin, dwOffset ) );
			dprintf( FORMAT_UINT( PIB, ppibTrxOldest, dwSessionContext, dwOffset ) );
			dprintf( FORMAT_UINT( PIB, ppibTrxOldest, dwSessionContextThreadId, dwOffset ) );
			}
		else
			{
			dprintf( "OLDEST TRANSACTION: <none>\n" );
			}
		dprintf( "\n" );

		dwOffset = (BYTE *)pinst->m_pver - (BYTE *)pver;
		dprintf( "VER: 0x%p\n", pinst->m_pver );
		dprintf( FORMAT_POINTER( VER, pver, m_pcresVERPool, dwOffset ) );
		dprintf( FORMAT_POINTER( VER, pver, m_pbucketGlobalHead, dwOffset ) );
		dprintf( FORMAT_POINTER( VER, pver, m_pbucketGlobalTail, dwOffset ) );

#ifdef MOVEABLE_RCE
		dprintf( FORMAT_POINTER( VER, pver, m_pbucketGlobalLastDelete, dwOffset ) );
		dprintf( FORMAT_INT( VER, pver, m_cbucketGlobalAllocDelete, dwOffset ) );
#endif

		dprintf( FORMAT_INT( VER, pver, m_cbucketGlobalAlloc, dwOffset ) );
		dprintf( FORMAT_UINT( VER, pver, m_cbucketDynamicAlloc, dwOffset ) );
		dprintf( FORMAT_INT( VER, pver, m_cbucketGlobalAllocMost, dwOffset ) );
		dprintf( FORMAT_INT( VER, pver, m_cbucketGlobalAllocPreferred, dwOffset ) );
		dprintf( FORMAT_UINT( VER, pver, m_ulVERTasksPostMax, dwOffset ) );
		dprintf( FORMAT_UINT( VER, pver, m_trxBegin0LastLongRunningTransaction, dwOffset ) );
		dprintf( FORMAT_POINTER( VER, pver, m_ppibTrxOldestLastLongRunningTransaction, dwOffset ) );
		dprintf( FORMAT_UINT( VER, pver, m_dwTrxContextLastLongRunningTransaction, dwOffset ) );
		dprintf( "\n" );

		dprintf( "STATISTICS:\n" );
		dprintf( "-----------\n" );
		dprintf( "Bucket size (in bytes): %d (0x%08x)\n", pcresVER->CbBlock(), pcresVER->CbBlock() );
		dprintf( "Global reserved buckets: %d (0x%08x)\n", pcresVER->CBlocksAllocated(), pcresVER->CBlocksAllocated() );
		dprintf( "Global reserved buckets start address: 0x%0*I64x\n", sizeof(BYTE*) * 2, QWORD( pcresVER->PbBlocksAllocated() ) );
		dprintf( "Global reserved buckets committed: %d (0x%08x)\n", pcresVER->CBlockCommit(), pcresVER->CBlockCommit() );
		dprintf( "\n" );

		dprintf( "Scanning buckets for this instance...\n" );
		ULONG_PTR	cBuckets					= 0;
		ULONG_PTR	cBucketsReserved			= 0;
		ULONG_PTR	cBucketsDynamic				= 0;
		ULONG_PTR	cRCETotal					= 0;
		ULONG_PTR	cbRCESizeTotal				= 0;
		ULONG_PTR	cbRCESizeMax				= 0;
		ULONG_PTR	cRCEMoved					= 0;
		ULONG_PTR	cbRCEMoved					= 0;
		ULONG_PTR	cRCEProxy					= 0;
		ULONG_PTR	cbRCEProxy					= 0;
		ULONG_PTR	cRCERolledBack				= 0;
		ULONG_PTR	cbRCERolledBack				= 0;
		ULONG_PTR	cRCENullified				= 0;
		ULONG_PTR	cbRCENullified				= 0;
		ULONG_PTR	cRCEUncommitted				= 0;
		ULONG_PTR	cbRCEUncommitted			= 0;
		ULONG_PTR	cRCECleanable				= 0;
		ULONG_PTR	cbRCECleanable				= 0;
		ULONG_PTR	cRCECleanableFlagDelete		= 0;
		ULONG_PTR	cRCECleanableDelta			= 0;
		ULONG_PTR	cRCECleanableSLVSpace		= 0;
		ULONG_PTR	cRCEUncleanable				= 0;
		ULONG_PTR	cbRCEUncleanable			= 0;
		ULONG_PTR	cRCEUncleanableFlagDelete	= 0;
		ULONG_PTR	cRCEUncleanableDelta		= 0;
		ULONG_PTR	cRCEUncleanableSLVSpace		= 0;

		BUCKET *	pbucket;
		BUCKET *	pbucketDebuggee;
		for ( pbucketDebuggee = pver->m_pbucketGlobalTail;
			NULL != pbucketDebuggee;
			pbucketDebuggee = pbucket->hdr.pbucketNext )
			{

			if ( !FFetchVariable( pbucketDebuggee, &pbucket ) )
				{
				dprintf( "Error: Could not read BUCKET memory. Aborting.\n\n" );
				break;
				}

			//	determine whether bucket was pre-reserved or
			//	dynamically allocated and increment appropriate
			//	counter
			cBuckets++;
			if ( pcresVER->FContains( (BYTE *)pbucketDebuggee ) )
				{
				cBucketsReserved++;
				}
			else
				{
				cBucketsDynamic++;
				}

			//	scan RCE's in this bucket
			const RCE *			prce			= (RCE *)pbucket->rgb;
			const RCE * const	prceNextNew		= (RCE *)( (BYTE *)pbucket->hdr.prceNextNew - (BYTE *)pbucketDebuggee + (BYTE *)pbucket );
			const BYTE * const	pbLastDelete	= pbucket->hdr.pbLastDelete - (BYTE *)pbucketDebuggee + (BYTE *)pbucket;

			while (	prceNextNew != prce )
				{
				const ULONG		cbRCE		= prce->CbRceEDBG();
				const BYTE *	pbNextRce	= reinterpret_cast<BYTE *>( PvAlignForAllPlatforms( (BYTE *)prce + cbRCE ) );

				//	increment total and size counters
				cRCETotal++;
				cbRCESizeTotal += cbRCE;
				cbRCESizeMax = max( cbRCESizeMax, cbRCE );

				//	was RCE moved?
				if ( prce->FMoved() )
					{
					cRCEMoved++;
					cbRCEMoved += cbRCE;
					}

				//	was RCE created by proxy?
				if ( prce->FProxy() )
					{
					cRCEProxy++;
					cbRCEProxy += cbRCE;
					}

				//	was RCE rolled back?
				if ( prce->FRolledBackEDBG() )
					{
					cRCERolledBack++;
					cbRCERolledBack += cbRCE;
					}

				//	determine the state of the RCE and increment
				//	appropriate counters
				if ( prce->FOperNull() )
					{
					cRCENullified++;
					cbRCENullified += cbRCE;
					}
				else if ( trxMax == prce->TrxCommittedEDBG() )
					{
					cRCEUncommitted++;
					cbRCEUncommitted += cbRCE;
					}
				else if ( NULL == ppibTrxOldest
					|| TrxCmp( prce->TrxCommittedEDBG(), ppibTrxOldest->trxBegin0 ) < 0 )
					{
					cRCECleanable++;
					cbRCECleanable += cbRCE;

					switch ( prce->Oper() )
						{
						case operFlagDelete:
							cRCECleanableFlagDelete++;
							break;
						case operDelta:
							cRCECleanableDelta++;
							break;
						case operSLVSpace:
							cRCECleanableSLVSpace++;
							break;
						default:
							break;
						}
					}
				else
					{
					cRCEUncleanable++;
					cbRCEUncleanable += cbRCE;

					switch ( prce->Oper() )
						{
						case operFlagDelete:
							cRCEUncleanableFlagDelete++;
							break;
						case operDelta:
							cRCEUncleanableDelta++;
							break;
						case operSLVSpace:
							cRCEUncleanableSLVSpace++;
							break;
						default:
							break;
						}
					}

				//	move to next RCE, being careful to account
				//	for case where we are on the last moved RCE
				if ( pbNextRce != pbLastDelete )
					{
					prce = (RCE *)pbNextRce;
					}
				else
					{
					prce = (RCE *)( (BYTE *)pbucket->hdr.prceOldest - (BYTE *)pbucketDebuggee + (BYTE *)pbucket );
					}

				}
			}

		dprintf( "  Total buckets in use: %d (0x%0*I64x)\n", cBucketsReserved+cBucketsDynamic, sizeof(ULONG_PTR)*2, QWORD( cBucketsReserved+cBucketsDynamic ) );
		dprintf( "  Reserved buckets: %d (0x%0*I64x)\n", cBucketsReserved, sizeof(ULONG_PTR)*2, QWORD( cBucketsReserved ) );
		dprintf( "  Dynamic buckets: %d (0x%0*I64x)\n", cBucketsDynamic, sizeof(ULONG_PTR)*2, QWORD( cBucketsDynamic ) );
		dprintf( "\n" );

		dprintf( "  Total version store entries: %d (0x%0*I64x)\n", cRCETotal, sizeof(ULONG_PTR)*2, QWORD( cRCETotal ) );
		dprintf( "  Total size of all entries (in bytes): %d (0x%0*I64x)\n", cbRCESizeTotal, sizeof(ULONG_PTR)*2, QWORD( cbRCESizeTotal ) );
		dprintf( "  Max. entry size (in bytes): %d (0x%0*I64x)\n", cbRCESizeMax, sizeof(ULONG_PTR)*2, QWORD( cbRCESizeMax ) );
		dprintf( "  Average entry size (in bytes): %d (0x%0*I64x)\n", ( 0 != cRCETotal ? cbRCESizeTotal/cRCETotal : 0 ), sizeof(ULONG_PTR)*2, QWORD( 0 != cRCETotal ? cbRCESizeTotal/cRCETotal : 0 ) );
		dprintf( "\n" );

		dprintf( "  Entries moved: %d (0x%0*I64x)\n", cRCEMoved, sizeof(ULONG_PTR)*2, QWORD( cRCEMoved ) );
		dprintf( "  Total size of moved entries (in bytes): %d (0x%0*I64x)\n", cbRCEMoved, sizeof(ULONG_PTR)*2, QWORD( cbRCEMoved ) );
		dprintf( "\n" );

		dprintf( "  Entries created by proxy: %d (0x%0*I64x)\n", cRCEProxy, sizeof(ULONG_PTR)*2, QWORD( cRCEProxy ) );
		dprintf( "  Total size of entries created by proxy (in bytes): %d (0x%0*I64x)\n", cbRCEProxy, sizeof(ULONG_PTR)*2, QWORD( cbRCEProxy ) );
		dprintf( "\n" );

		dprintf( "  Entries rolled-back: %d (0x%0*I64x)\n", cRCERolledBack, sizeof(ULONG_PTR)*2, QWORD( cRCERolledBack ) );
		dprintf( "  Total size of rolled-back entries (in bytes): %d (0x%0*I64x)\n", cbRCERolledBack, sizeof(ULONG_PTR)*2, QWORD( cbRCERolledBack ) );
		dprintf( "\n" );

		dprintf( "  Entries nullified: %d (0x%0*I64x)\n", cRCENullified, sizeof(ULONG_PTR)*2, QWORD( cRCENullified ) );
		dprintf( "  Total size of nullified entries (in bytes): %d (0x%0*I64x)\n", cbRCENullified, sizeof(ULONG_PTR)*2, QWORD( cbRCENullified ) );
		dprintf( "\n" );

		dprintf( "  Entries uncommitted: %d (0x%0*I64x)\n", cRCEUncommitted, sizeof(ULONG_PTR)*2, QWORD( cRCEUncommitted ) );
		dprintf( "  Total size of uncommitted entries (in bytes): %d (0x%0*I64x)\n", cbRCEUncommitted, sizeof(ULONG_PTR)*2, QWORD( cbRCEUncommitted ) );
		dprintf( "\n" );

		dprintf( "  Entries committed: %d (0x%0*I64x)\n", cRCECleanable+cRCEUncleanable, sizeof(ULONG_PTR)*2, QWORD( cRCECleanable+cRCEUncleanable ) );
		dprintf( "  Total size of committed entries (in bytes): %d (0x%0*I64x)\n", cbRCECleanable+cbRCEUncleanable, sizeof(ULONG_PTR)*2, QWORD( cbRCECleanable+cbRCEUncleanable ) );
		dprintf( "    Cleanable Entries: %d (0x%0*I64x)\n", cRCECleanable, sizeof(ULONG_PTR)*2, QWORD( cRCECleanable ) );
		dprintf( "    Total size of cleanable entries (in bytes): %d (0x%0*I64x)\n", cbRCECleanable, sizeof(ULONG_PTR)*2, QWORD( cbRCECleanable ) );
		dprintf( "      Cleanable 'FlagDelete' entries: %d (0x%0*I64x)\n", cRCECleanableFlagDelete, sizeof(ULONG_PTR)*2, QWORD( cRCECleanableFlagDelete ) );
		dprintf( "      Cleanable 'Delta' entries: %d (0x%0*I64x)\n", cRCECleanableDelta, sizeof(ULONG_PTR)*2, QWORD( cRCECleanableDelta ) );
		dprintf( "      Cleanable 'SLVSpace' entries:): %d (0x%0*I64x)\n", cRCECleanableSLVSpace, sizeof(ULONG_PTR)*2, QWORD( cRCECleanableSLVSpace ) );
		dprintf( "    Uncleanable Entries: %d (0x%0*I64x)\n", cRCEUncleanable, sizeof(ULONG_PTR)*2, QWORD( cRCEUncleanable ) );
		dprintf( "    Total size of Uncleanable entries (in bytes): %d (0x%0*I64x)\n", cbRCEUncleanable, sizeof(ULONG_PTR)*2, QWORD( cbRCEUncleanable ) );
		dprintf( "      Uncleanable 'FlagDelete' entries: %d (0x%0*I64x)\n", cRCEUncleanableFlagDelete, sizeof(ULONG_PTR)*2, QWORD( cRCEUncleanableFlagDelete ) );
		dprintf( "      Uncleanable 'Delta' entries: %d (0x%0*I64x)\n", cRCEUncleanableDelta, sizeof(ULONG_PTR)*2, QWORD( cRCEUncleanableDelta ) );
		dprintf( "      Uncleanable 'SLVSpace' entries:): %d (0x%0*I64x)\n", cRCEUncleanableSLVSpace, sizeof(ULONG_PTR)*2, QWORD( cRCEUncleanableSLVSpace ) );
		}
	else
		{
		dprintf( "Error: Could not read some version store variables.\n" );
		}

HandleError:
	dprintf( "\n--------------------\n\n" );

	Unfetch( pcresVER );
	Unfetch( pver );
	Unfetch( ppibTrxOldest );
	Unfetch( rgpls );
	Unfetch( pinst );
	}


//  ================================================================
DEBUG_EXT( EDBGMemory )
//  ================================================================
	{
	DWORD			cAllocHeapT;
	DWORD			cFreeHeapT;
	DWORD			cbAllocHeapT;
	DWORD_PTR		cbReservePageT;
	DWORD_PTR		cbCommitPageT;
	LONG			cbPageT;
	LONG_PTR		cbfCacheT;
	SIZE_T			cbHintCacheT;
	SIZE_T			maskHintCacheT;
	ULONG			ipinstMaxT;
	INST **			rgpinst				= NULL;
	CRES *			pcresVER			= NULL;

	dprintf( "\n" );

	if ( FReadGlobal( "cAllocHeap", &cAllocHeapT )
		&& FReadGlobal( "cFreeHeap", &cFreeHeapT )
		&& FReadGlobal( "cbAllocHeap", &cbAllocHeapT )
		&& FReadGlobal( "cbReservePage", &cbReservePageT )
		&& FReadGlobal( "cbCommitPage", &cbCommitPageT )
		&& FReadGlobal( "g_cbPage", &cbPageT )
		&& FReadGlobal( "cbfCache", &cbfCacheT )
		&& FReadGlobal( "CPAGE::cbHintCache", &cbHintCacheT )
		&& FReadGlobal( "CPAGE::maskHintCache", &maskHintCacheT )
		&& FReadGlobal( "ipinstMax", &ipinstMaxT )
		&& ipinstMaxT <= cMaxInstances
		&& FFetchGlobal( "g_rgpinst", &rgpinst, ipinstMaxT )
		&& FReadGlobalAndFetchVariable( "g_pcresVERPool", &pcresVER ) )
		{
		dprintf( "Heap Usage\n" );
		dprintf( "----------\n" );
		dprintf( "  Bytes currently allocated: %d (0x%0*I64x)\n", cbAllocHeapT, sizeof(DWORD_PTR)*2, QWORD( cbAllocHeapT ) );
		dprintf( "  Current allocations: %d (0x%0*I64x)\n", cAllocHeapT - cFreeHeapT, sizeof(DWORD_PTR)*2, QWORD( cAllocHeapT - cFreeHeapT ) );
		dprintf( "  Total allocations (life of instance): %d (0x%0*I64x)\n", cAllocHeapT, sizeof(DWORD_PTR)*2, QWORD( cAllocHeapT ) );
		dprintf( "  Total de-allocations (life of instance): %d (0x%0*I64x)\n", cFreeHeapT, sizeof(DWORD_PTR)*2, QWORD( cFreeHeapT ) );
		dprintf( "\n" );

		dprintf( "Virtual Address Space Usage\n" );
		dprintf( "---------------------------\n" );
		dprintf( "  Bytes reserved: %d (0x%0*I64x)\n", cbReservePageT, sizeof(DWORD_PTR)*2, QWORD( cbReservePageT ) );
		dprintf( "  Bytes committed: %d (0x%0*I64x)\n", cbCommitPageT, sizeof(DWORD_PTR)*2, QWORD( cbCommitPageT ) );
		dprintf( "\n" );


		//	break down virtual address space usage
		//	into its major consumers

		ULONG_PTR	cbucketDynamicAlloc		= 0;
		ULONG_PTR	cbTablesReserved		= 0;
		ULONG_PTR	cbTablesCommitted		= 0;
		ULONG_PTR	cbCursorsReserved		= 0;
		ULONG_PTR	cbCursorsCommitted		= 0;
		ULONG_PTR	cbSessionsReserved		= 0;
		ULONG_PTR	cbSessionsCommitted		= 0;
		ULONG_PTR	cbSortsReserved			= 0;
		ULONG_PTR	cbSortsCommitted		= 0;

		//	sum up individual instance totals to
		//	yield totals for the process

		for ( ULONG i = 0; i < ipinstMaxT; i++ )
			{
			if ( pinstNil == rgpinst[i] )
				continue;

			INST *	pinst		= NULL;
			VER *	pver		= NULL;
			CRES *	pcresFCB	= NULL;
			CRES *	pcresTDB	= NULL;
			CRES *	pcresIDB	= NULL;
			CRES *	pcresSCB	= NULL;
			CRES *	pcresFUCB	= NULL;
			CRES *	pcresPIB	= NULL;

			if ( FFetchVariable( rgpinst[i], &pinst )
				&& FFetchVariable( pinst->m_pver, &pver )
				&& FFetchVariable( pinst->m_pcresFCBPool, &pcresFCB )
				&& FFetchVariable( pinst->m_pcresTDBPool, &pcresTDB )
				&& FFetchVariable( pinst->m_pcresIDBPool, &pcresIDB )
				&& ( 0 == pinst->m_lTemporaryTablesMax || FFetchVariable( pinst->m_pcresSCBPool, &pcresSCB ) )
				&& FFetchVariable( pinst->m_pcresFUCBPool, &pcresFUCB )
				&& FFetchVariable( pinst->m_pcresPIBPool, &pcresPIB ) )
				{
				cbucketDynamicAlloc += pver->m_cbucketDynamicAlloc;

				cbTablesReserved += pcresFCB->CBlocksAllocated() * pcresFCB->CbBlock();
				cbTablesCommitted += pcresFCB->CBlockCommit() * pcresFCB->CbBlock();

				cbTablesReserved += pcresTDB->CBlocksAllocated() * pcresTDB->CbBlock();
				cbTablesCommitted += pcresTDB->CBlockCommit() * pcresTDB->CbBlock();

				cbTablesReserved += pcresIDB->CBlocksAllocated() * pcresIDB->CbBlock();
				cbTablesCommitted += pcresIDB->CBlockCommit() * pcresIDB->CbBlock();

				if ( pinst->m_lTemporaryTablesMax > 0 )
					{
					cbSortsReserved += pcresSCB->CBlocksAllocated() * pcresSCB->CbBlock();
					cbSortsCommitted += pcresSCB->CBlockCommit() * pcresSCB->CbBlock();
					}

				cbCursorsReserved += pcresFUCB->CBlocksAllocated() * pcresFUCB->CbBlock();
				cbCursorsCommitted += pcresFUCB->CBlockCommit() * pcresFUCB->CbBlock();

				cbSessionsReserved += pcresPIB->CBlocksAllocated() * pcresPIB->CbBlock();
				cbSessionsCommitted += pcresPIB->CBlockCommit() * pcresPIB->CbBlock();
				}
			else
				{
				dprintf( "Error: Could not read INST information. Skipped an instance.\n" );
				}

			Unfetch( pcresPIB );
			Unfetch( pcresFUCB );
			Unfetch( pcresSCB );
			Unfetch( pcresIDB );
			Unfetch( pcresTDB );
			Unfetch( pcresFCB );
			Unfetch( pver );
			Unfetch( pinst );
			}

		dprintf( "  Buffer Manager:\n" );
		dprintf( "    Current buffers: %d (0x%0*I64x)\n", cbfCacheT, sizeof(LONG_PTR)*2, QWORD( cbfCacheT ) );
		dprintf( "    Current cache size (in bytes): %d (0x%0*I64x)\n", cbfCacheT * cbPageT, sizeof(LONG_PTR)*2, QWORD( cbfCacheT * cbPageT ) );
		dprintf( "    Current cache overhead (in bytes): %d (0x%0*I64x)\n", cbfCacheT * sizeof(BF), sizeof(LONG_PTR)*2, QWORD( cbfCacheT * sizeof(BF) ) );
		dprintf( "    Committed hint cache size (in bytes): %d (0x%0*I64x)\n", cbHintCacheT, sizeof(SIZE_T)*2, QWORD( cbHintCacheT ) );
		dprintf( "    Hint cache size in use (in bytes): %d (0x%0*I64x)\n", ( maskHintCacheT + 1 ) * sizeof(DWORD_PTR), sizeof(SIZE_T)*2, QWORD( ( maskHintCacheT + 1 ) * sizeof(DWORD_PTR) ) );
		dprintf( "  Version Store:\n" );
		dprintf( "    Pre-reserved bytes: %d (0x%0*I64x)\n", pcresVER->CBlocksAllocated() * pcresVER->CbBlock(), sizeof(ULONG_PTR)*2, QWORD( pcresVER->CBlocksAllocated() * pcresVER->CbBlock() ) );
		dprintf( "    Pre-reserved bytes committed: %d (0x%0*I64x)\n", pcresVER->CBlockCommit() * pcresVER->CbBlock(), sizeof(ULONG_PTR)*2, QWORD( pcresVER->CBlockCommit() * pcresVER->CbBlock() ) );
		dprintf( "    Dynamic bytes committed: %d (0x%0*I64x)\n", cbucketDynamicAlloc * pcresVER->CbBlock(), sizeof(ULONG_PTR)*2, QWORD( cbucketDynamicAlloc * pcresVER->CbBlock() ) );
		dprintf( "  Tables and Indexes (FCB,TDB,IDB):\n" );
		dprintf( "    Bytes reserved: %d (0x%0*I64x)\n", cbTablesReserved, sizeof(ULONG_PTR)*2, QWORD( cbTablesReserved ) );
		dprintf( "    Bytes committed: %d (0x%0*I64x)\n", cbTablesCommitted, sizeof(ULONG_PTR)*2, QWORD( cbTablesCommitted ) );
		dprintf( "  Sorts and Temporary Tables (SCB):\n" );
		dprintf( "    Bytes reserved: %d (0x%0*I64x)\n", cbSortsReserved, sizeof(ULONG_PTR)*2, QWORD( cbSortsReserved ) );
		dprintf( "    Bytes committed: %d (0x%0*I64x)\n", cbSortsCommitted, sizeof(ULONG_PTR)*2, QWORD( cbSortsCommitted ) );
		dprintf( "  Cursors (FUCB):\n" );
		dprintf( "    Bytes reserved: %d (0x%0*I64x)\n", cbCursorsReserved, sizeof(ULONG_PTR)*2, QWORD( cbCursorsReserved ) );
		dprintf( "    Bytes committed: %d (0x%0*I64x)\n", cbCursorsCommitted, sizeof(ULONG_PTR)*2, QWORD( cbCursorsCommitted ) );
		dprintf( "  Sessions (PIB):\n" );
		dprintf( "    Bytes reserved: %d (0x%0*I64x)\n", cbSessionsReserved, sizeof(ULONG_PTR)*2, QWORD( cbSessionsReserved ) );
		dprintf( "    Bytes committed: %d (0x%0*I64x)\n", cbSessionsCommitted, sizeof(ULONG_PTR)*2, QWORD( cbSessionsCommitted ) );
		}
	else
		{
		dprintf( "Error: Could not read some/all memory-related variables.\n" );
		}

	dprintf( "\n--------------------\n\n" );

	Unfetch( pcresVER );
	Unfetch( rgpinst );
	}


//  ================================================================
DEBUG_EXT( EDBGHelpDump )
//  ================================================================
	{
	INT icdumpmap;
	for( icdumpmap = 0; icdumpmap < ccdumpmap; icdumpmap++ )
		{
		dprintf( "\t%s\n", rgcdumpmap[icdumpmap].szHelp );
		}
	dprintf( "\n--------------------\n\n" );
	}


//  ================================================================
DEBUG_EXT( EDBGDump )
//  ================================================================
	{
	if( argc < 2 )
		{
		EDBGHelpDump( hCurrentProcess, hCurrentThread, dwCurrentPc, lpExtensionApis, argc, argv );
		return;
		}

	INT icdumpmap;
	for( icdumpmap = 0; icdumpmap < ccdumpmap; ++icdumpmap )
		{
		if( FArgumentMatch( argv[0], rgcdumpmap[icdumpmap].szCommand ) )
			{
			(rgcdumpmap[icdumpmap].pcdump)->Dump(
				hCurrentProcess,
				hCurrentThread,
				dwCurrentPc,
				lpExtensionApis,
				argc - 1, argv + 1 );
			return;
			}
		}
	EDBGHelpDump( hCurrentProcess, hCurrentThread, dwCurrentPc, lpExtensionApis, argc, argv );
	}


//  ================================================================
template< class _STRUCT>
VOID CDUMPA<_STRUCT>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	_STRUCT* ptDebuggee;
	if (	argc != 1 ||
			!FAddressFromSz( argv[ 0 ], &ptDebuggee ) )
		{
		dprintf( "Usage: DUMP <class> <address>\n" );
		return;
		}

	_STRUCT* pt;
	if ( FFetchVariable( ptDebuggee, &pt ) )
		{
		dprintf(	"0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( _STRUCT ) ),
					ptDebuggee );
		pt->Dump( CPRINTFWDBG::PcprintfInstance(), (BYTE*)ptDebuggee - (BYTE*)pt );
		Unfetch( pt );
		}
	}


//  ================================================================
VOID CDUMPA<CPAGE>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	BOOL	fDumpAllocMap 	= fFalse;
	BOOL	fDumpBinary		= fFalse;
	BOOL	fDumpHeader 	= fFalse;
	BOOL	fDumpTags 		= fFalse;
	CHAR *	pchPageSize;

	switch ( argc )
		{
		case 1:
			fDumpHeader = fTrue;	//	no flags, so default is to dump header
			pchPageSize = NULL;
			break;

		case 2:
			fDumpAllocMap 	= ( strpbrk( argv[0], "aA*" ) != NULL );
			fDumpBinary 	= ( strpbrk( argv[0], "bB*" ) != NULL );
			fDumpHeader		= ( strpbrk( argv[0], "hH*" ) != NULL );
			fDumpTags		= ( strpbrk( argv[0], "tT*" ) != NULL );
			pchPageSize		= strpbrk( argv[0], "248" );
			break;

		default:
			dprintf( "Usage: DUMP PAGE [a|b|h|t|*|2|4|8] <address>\n" );
			return;
		}


	LONG	cbPage			= 0;
	BYTE *	rgbPageDebuggee	= NULL;
	BYTE *	rgbPage			= NULL;

	if ( NULL != pchPageSize )
		{
		Assert( '2' == *pchPageSize
			|| '4' == *pchPageSize
			|| '8' == *pchPageSize );
		cbPage = ( *pchPageSize - '0' ) * 1024;

		//	default to header dump if no other dumps specified
		if ( !fDumpAllocMap && !fDumpBinary && !fDumpTags )
			fDumpHeader = fTrue;
		}
	else if ( !FReadGlobal( "g_cbPage", &cbPage ) )
		{
		dprintf( "Error: Could not retrieve g_cbPage from the debuggee.\n" );
		return;
		}

	if ( !FAddressFromSz( argv[argc-1], &rgbPageDebuggee )
		|| !FFetchVariable( rgbPageDebuggee, &rgbPage, cbPage ) )
		{
		dprintf( "Error: Could not retrieve the required data from the debuggee.\n" );
		}
	else
		{
		g_cbPage = cbPage;
		SetCbPageRelated();

		CPAGE cpage;
		cpage.LoadPage( (void*)rgbPage );

		if ( fDumpHeader )
			{
			(VOID)cpage.DumpHeader( CPRINTFWDBG::PcprintfInstance(), rgbPageDebuggee - rgbPage );
			}
		if ( fDumpTags )
			{
			(VOID)cpage.DumpTags( CPRINTFWDBG::PcprintfInstance(), rgbPageDebuggee - rgbPage );
			}
		if ( fDumpAllocMap )
			{
			(VOID)cpage.DumpAllocMap( CPRINTFWDBG::PcprintfInstance() );
			}
		if ( fDumpBinary )
			{
			char* szBuf = (char*)LocalAlloc( 0, g_cbPageMax * 8 );
			if ( szBuf )
				{
				DBUTLSprintHex( (char* const)szBuf, rgbPage, g_cbPage, 16 );

				CHAR* szLine = strtok( (char* const)szBuf, "\n" );
				while ( szLine )
					{
					dprintf( "%s\n", szLine );
					szLine = strtok( NULL, "\n" );
					}

				LocalFree( (void*)szBuf );
				}
			}

		cpage.UnloadPage();
		}

	Unfetch( rgbPage );
	}


//  ================================================================
VOID CDUMPA<REC>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	BYTE*	rgbRecDebuggee;
	BYTE*	rgbRec;
	ULONG	cbRec;
	ULONG	cbPage				= 0;

	if (	argc != 2 ||
			!FAddressFromSz( argv[ 0 ], &rgbRecDebuggee ) ||
			!FUlFromSz( argv[ 1 ], &cbRec ) )
		{
		dprintf( "Usage: DUMP REC <address> <length>\n" );
		return;
		}

	else if ( !FFetchVariable( rgbRecDebuggee, &rgbRec, cbRec ) )
		{
		dprintf( "Error: Could not fetch record from debuggee.\n" );
		return;
		}

	else if ( FReadGlobal( "g_cbPage", &cbPage ) )
		{
		g_cbPage = cbPage;
		SetCbPageRelated();
		}

	else
		{
		dprintf( "Warning: Could not retrieve g_cbPage from the debuggee.\n" );
		dprintf( "         Record may not dump properly.\n" );
		}

	dprintf( "\n" );
	DBUTLDumpRec( rgbRec, cbRec, CPRINTFWDBG::PcprintfInstance(), 16 );

	Unfetch( rgbRec );
	}


//  ================================================================
VOID CDUMPA<MEMPOOL>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	MEMPOOL *	pmempool;
	MEMPOOL *	pmempoolDebuggee;
	BOOL		fDumpTags		= fFalse;
	BOOL		fDumpAll		= fFalse;
	ULONG		itagDump		= 0;

	dprintf( "\n" );

	if ( argc < 1
		|| argc > 2
		|| !FAddressFromSz( argv[0], &pmempoolDebuggee )
		|| ( ( fDumpTags = ( argc == 2 ) )
			&& !( fDumpAll = ( strpbrk( argv[1], "*" ) != NULL ) )
			&& !FUlFromSz( argv[1], &itagDump ) ) )
		{
		dprintf( "Usage: DUMP MEMPOOL <address> [<itag>|*]\n" );
		return;
		}

	if ( !FFetchVariable( pmempoolDebuggee, &pmempool ) )
		{
		dprintf( "Error: Could not fetch MEMPOOL.\n" );
		return;
		}

	pmempool->Dump(
		CPRINTFWDBG::PcprintfInstance(),
		fDumpTags,
		fDumpAll,
		(MEMPOOL::ITAG)itagDump,
		(BYTE *)pmempoolDebuggee - (BYTE *)pmempool );

	Unfetch( pmempool );
	}


//  ================================================================
VOID CSR::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_INT( CSR, this, m_dbtimeSeen, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSR, this, m_pgno, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSR, this, m_iline, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSR, this, m_latch, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSR, this, m_cpage, dwOffset ) );

	dprintf( "\n\t [CPAGE] 0x%0*I64X bytes @ 0x%p\n",  
			sizeof( size_t ) * 2,
			QWORD( sizeof( CPAGE ) ),
			(char *)this + dwOffset + OffsetOf( CSR, m_cpage )
			);

	(VOID)m_cpage.Dump( CPRINTFWDBG::PcprintfInstance(), dwOffset );
	}


//  ================================================================
VOID RCE::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_UINT( RCE, this, m_trxBegin0, dwOffset ) );

#ifdef NO_BEGIN0_COMMIT0_LOCK
	(*pcprintf)( FORMAT_UINT( RCE, this, m_trxCommittedInactive, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_ptrxCommitted, dwOffset ) );
#else
	(*pcprintf)( FORMAT_UINT( RCE, this, m_trxCommitted, dwOffset ) );
#endif

	(*pcprintf)( FORMAT_UINT( RCE, this, m_rceid, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_updateid, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_uiHash, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_ifmp, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_pgnoFDP, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_pgnoUndoInfo, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( RCE, this, m_pfcb, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_pfucb, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prceUndoInfoNext, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prceUndoInfoPrev, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prceNextOfSession, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prcePrevOfSession, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prceNextOfFCB, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prcePrevOfFCB, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prceNextOfNode, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prcePrevOfNode, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( RCE, this, m_prceHashOverflow, dwOffset ) );

	(*pcprintf)( FORMAT_INT( RCE, this, m_ulFlags, dwOffset ) );

	PRINT_METHOD_FLAG( pcprintf, FOperNull );
	PRINT_METHOD_FLAG( pcprintf, FOperDDL );
	PRINT_METHOD_FLAG( pcprintf, FUndoableLoggedOper );
	PRINT_METHOD_FLAG( pcprintf, FOperInHashTable );
	PRINT_METHOD_FLAG( pcprintf, FOperReplace );
	PRINT_METHOD_FLAG( pcprintf, FOperConcurrent );
	PRINT_METHOD_FLAG( pcprintf, FOperAffectsSecondaryIndex );
	PRINT_METHOD_FLAG( pcprintf, FMoved );
	PRINT_METHOD_FLAG( pcprintf, FProxy );

	(*pcprintf)( FORMAT_INT_BF( RCE, this, m_level, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( RCE, this, m_cbBookmarkKey, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_cbBookmarkData, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_cbData, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( RCE, this, m_oper, dwOffset ) );

	(*pcprintf)( FORMAT_0ARRAY( RCE, this, m_rgbData, dwOffset ) );
	}


//  ================================================================
VOID FCB::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_precdangling, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_ls, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( FCB, this, m_ptdb, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pfcbNextIndex, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pfcbLRU, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pfcbMRU, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pfcbNextList, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pfcbPrevList, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pfcbTable, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pidb, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_pfucb, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FCB, this, m_wRefCount, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FCB, this, m_objidFDP, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FCB, this, m_pgnoFDP, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FCB, this, m_pgnoOE, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FCB, this, m_pgnoAE, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FCB, this, m_ifmp, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FCB, this, m_ulFCBListFlags, dwOffset ) );
	PRINT_METHOD_FLAG( pcprintf, FInList );
	PRINT_METHOD_FLAG( pcprintf, FInLRU );

	(*pcprintf)( FORMAT_INT( FCB, this, m_ulFCBFlags, dwOffset ) );
	PRINT_METHOD_FLAG( pcprintf, FTypeDatabase );
	PRINT_METHOD_FLAG( pcprintf, FTypeTable );
	PRINT_METHOD_FLAG( pcprintf, FTypeSecondaryIndex );
	PRINT_METHOD_FLAG( pcprintf, FTypeTemporaryTable );
	PRINT_METHOD_FLAG( pcprintf, FTypeSort );
	PRINT_METHOD_FLAG( pcprintf, FTypeSentinel );
	PRINT_METHOD_FLAG( pcprintf, FTypeLV );
	PRINT_METHOD_FLAG( pcprintf, FTypeSLVAvail );
	PRINT_METHOD_FLAG( pcprintf, FTypeSLVOwnerMap );
	PRINT_METHOD_FLAG( pcprintf, FPrimaryIndex );
	PRINT_METHOD_FLAG( pcprintf, FSequentialIndex );
	PRINT_METHOD_FLAG( pcprintf, FFixedDDL );
	PRINT_METHOD_FLAG( pcprintf, FTemplateTable );
	PRINT_METHOD_FLAG( pcprintf, FDerivedTable );
	PRINT_METHOD_FLAG( pcprintf, FTemplateIndex );
	PRINT_METHOD_FLAG( pcprintf, FDerivedIndex );
	PRINT_METHOD_FLAG( pcprintf, FInitialized );
	PRINT_METHOD_FLAG( pcprintf, FAboveThreshold );
	PRINT_METHOD_FLAG( pcprintf, FDeletePending );
	PRINT_METHOD_FLAG( pcprintf, FDeleteCommitted );
	PRINT_METHOD_FLAG( pcprintf, FUnique );
	PRINT_METHOD_FLAG( pcprintf, FNonUnique );
	PRINT_METHOD_FLAG( pcprintf, FNoCache );
	PRINT_METHOD_FLAG( pcprintf, FPreread );

#ifdef TABLES_PERF
	(*pcprintf)( FORMAT_INT( FCB, this, m_tableclass, dwOffset ) );
#endif  //  TABLES_PERF

	(*pcprintf)( FORMAT_INT( FCB, this, m_cbDensityFree, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( FCB, this, m_prceNewest, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_prceOldest, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( FCB, this, m_ppibDomainDenyRead, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FCB, this, m_crefDomainDenyRead, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FCB, this, m_crefDomainDenyWrite, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FCB, this, m_pgnoNextAvailSE, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FCB, this, m_psplitbufdangling, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FCB, this, m_bflPgnoFDP, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( FCB, this, m_bflPgnoOE, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( FCB, this, m_bflPgnoAE, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FCB, this, m_ctasksActive, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FCB, this, m_errInit, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FCB, this, m_critRCEList, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FCB, this, m_sxwl, dwOffset ) );

	}


//  ================================================================
VOID IDB::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_INT( IDB, this, m_crefCurrentIndex, dwOffset ) );

	(*pcprintf)( FORMAT_INT( IDB, this, m_fidbPersisted, dwOffset ) );
	PRINT_METHOD_FLAG( pcprintf, FPrimary );
	PRINT_METHOD_FLAG( pcprintf, FUnique );
	PRINT_METHOD_FLAG( pcprintf, FAllowAllNulls );
	PRINT_METHOD_FLAG( pcprintf, FAllowFirstNull );
	PRINT_METHOD_FLAG( pcprintf, FAllowSomeNulls );
	PRINT_METHOD_FLAG( pcprintf, FNoNullSeg );
	PRINT_METHOD_FLAG( pcprintf, FSortNullsHigh );
	PRINT_METHOD_FLAG( pcprintf, FMultivalued );
	PRINT_METHOD_FLAG( pcprintf, FLocaleId );
	PRINT_METHOD_FLAG( pcprintf, FLocalizedText );
	PRINT_METHOD_FLAG( pcprintf, FTemplateIndex );
	PRINT_METHOD_FLAG( pcprintf, FDerivedIndex );

	(*pcprintf)( FORMAT_INT( IDB, this, m_fidbNonPersisted, dwOffset ) );
	PRINT_METHOD_FLAG( pcprintf, FVersioned );
	PRINT_METHOD_FLAG( pcprintf, FDeleted );
	PRINT_METHOD_FLAG( pcprintf, FVersionedCreate );
	PRINT_METHOD_FLAG( pcprintf, FHasPlaceholderColumn );
	PRINT_METHOD_FLAG( pcprintf, FSparseIndex );
	PRINT_METHOD_FLAG( pcprintf, FSparseConditionalIndex );
	PRINT_METHOD_FLAG( pcprintf, FTuples );

	(*pcprintf)( FORMAT_INT( IDB, this, m_idxunicode.lcid, dwOffset ) );
	(*pcprintf)( FORMAT_INT( IDB, this, m_idxunicode.dwMapFlags, dwOffset ) );
	(*pcprintf)( FORMAT_INT( IDB, this, m_crefVersionCheck, dwOffset ) );
	(*pcprintf)( FORMAT_INT( IDB, this, m_itagIndexName, dwOffset ) );
	(*pcprintf)( FORMAT_INT( IDB, this, m_cbVarSegMac, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( IDB, this, m_cidxseg, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( IDB, this, m_cidxsegConditional, dwOffset ) );
	if ( FIsRgidxsegInMempool() )
		{
		(*pcprintf)( "\tItagRgidxseg: %d\n", ItagRgidxseg() );
		}
	else
		{
		(*pcprintf)( FORMAT_VOID( IDB, this, rgidxseg, dwOffset ) );
		}
	if ( FIsRgidxsegConditionalInMempool() )
		{
		(*pcprintf)( "\tItagRgidxsegConditional: %d\n", ItagRgidxsegConditional() );
		}
	else
		{
		(*pcprintf)( FORMAT_VOID( IDB, this, rgidxsegConditional, dwOffset ) );
		}
	(*pcprintf)( FORMAT_VOID( IDB, this, m_rgbitIdx, dwOffset ) );
	}


//  ================================================================
VOID MEMPOOL::Dump(
	CPRINTF * 		pcprintf,
	const BOOL		fDumpTags,
	const BOOL		fDumpAll,
	const ITAG		itagDump,
	const DWORD_PTR	dwOffset )
//  ================================================================
	{
	(*pcprintf)( FORMAT_POINTER( MEMPOOL, this, m_pbuf, dwOffset ) );
	(*pcprintf)( FORMAT_INT( MEMPOOL, this, m_cbBufSize, dwOffset ) );
	(*pcprintf)( FORMAT_INT( MEMPOOL, this, m_ibBufFree, dwOffset ) );
	(*pcprintf)( FORMAT_INT( MEMPOOL, this, m_itagUnused, dwOffset ) );
	(*pcprintf)( FORMAT_INT( MEMPOOL, this, m_itagFreed, dwOffset ) );

	if ( fDumpTags )
		{
		BYTE *	rgbBufDebuggee	= Pbuf();
		BYTE *	rgbBuf;

		if ( FFetchVariable( rgbBufDebuggee, &rgbBuf, CbBufSize() ) )
			{
			SetPbuf( rgbBuf );

			dprintf( "\n" );
			if ( fDumpAll )
				{
				//	dump the entire mempool
				for ( ITAG itag = 0; itag < ItagUnused(); itag++ )
					{
					DumpTag( pcprintf, itag, rgbBufDebuggee - rgbBuf );
					}
				}
			else
				{
				//	dump just the specified tag
				DumpTag( pcprintf, itagDump, rgbBufDebuggee - rgbBuf );
				}
			dprintf( "\n--------------------\n\n" );
			Unfetch( rgbBuf );
			}
		else
			{
			dprintf( "Error: Could not fetch MEMPOOL buffer.\n" );
			}
		}
	}


//  ================================================================
VOID MEMPOOL::DumpTag( CPRINTF * pcprintf, const ITAG itag, const SIZE_T lOffset ) const
//  ================================================================
	{
	MEMPOOLTAG	* const rgbTags = (MEMPOOLTAG *)Pbuf();
	if( 0 != rgbTags[itag].cb )
		{
		//  this tag is used
		(*pcprintf)( "TAG %3d        address:0x%p    cb:0x%04x    ib:0x%04x\n",
				 	itag, (BYTE *)(&(rgbTags[itag])) + lOffset, rgbTags[itag].cb, rgbTags[itag].ib );
		(*pcprintf)( "\t%s", SzEDBGHexDump( Pbuf() + rgbTags[itag].ib, min( 32, rgbTags[itag].cb ) ) );
		}
	else
		{
		//  this is a free tag
		(*pcprintf)( "TAG %3d (FREE) address:0x%p    cb:0x%04x    ib:0x%04x\n",
				 	itag, (BYTE *)(&(rgbTags[itag])) + lOffset, rgbTags[itag].cb, rgbTags[itag].ib );
		}
	}


//  ================================================================
VOID PIB::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_UINT( PIB, this, trxBegin0, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( PIB, this, trxCommit0, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( PIB, this, m_pinst, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( PIB, this, dwTrxContext, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( PIB, this, m_fFlags, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fUserSession, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fAfterFirstBT, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fRecoveringEndAllSessions, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fOLD, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fSystemCallback, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fCIMCommitted, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fCIMDirty, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fSetAttachDB, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fUseSessionContextForTrxContext, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fBegin0Logged, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fLGWaiting, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fReadOnlyTrx, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fDistributedTrx, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( PIB, this, m_fPreparedToCommitTrx, dwOffset ) );

	(*pcprintf)( FORMAT_INT( PIB, this, m_cInJetAPI, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( PIB, this, prceNewest, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( PIB, this, pfucbOfSession, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( PIB, this, procid, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( PIB, this, rgcdbOpen, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( PIB, this, ptls, dwOffset ) );

	(*pcprintf)( FORMAT_INT( PIB, this, level, dwOffset ) );
	(*pcprintf)( FORMAT_INT( PIB, this, levelBegin, dwOffset ) );
	(*pcprintf)( FORMAT_INT( PIB, this, clevelsDeferBegin, dwOffset ) );
	(*pcprintf)( FORMAT_INT( PIB, this, levelRollback, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( PIB, this, ppibNext, dwOffset ) );

	(*pcprintf)( FORMAT_INT( PIB, this, updateid, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( PIB, this, critCursors, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( PIB, this, grbitsCommitDefault, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( PIB, this, m_ileTrxOldest, dwOffset ) );

	(*pcprintf)( FORMAT_LGPOS( PIB, this, lgposStart, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( PIB, this, lgposCommit0, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( PIB, this, asigWaitLogFlush, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( PIB, this, ppibNextWaitFlush, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( PIB, this, ppibPrevWaitFlush, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( PIB, this, critLogDeferBeginTrx, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( PIB, this, dwSessionContext, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( PIB, this, dwSessionContextThreadId, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( PIB, this, m_pplsTrxOldest, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( PIB, this, critTrx, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( PIB, this, m_pvRecordFormatConversionBuffer, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( PIB, this, m_pMacroNext, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( PIB, this, m_pfucbCache, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( PIB, this, m_cfucbCache, dwOffset ) );

	(*pcprintf)( FORMAT_INT( PIB, this, m_errRollbackFailure, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( PIB, this, m_simplelistRceidDeferred, dwOffset ) );

#ifdef INDEPENDENT_DB_FAILURE
	(*pcprintf)( FORMAT_INT( PIB, this, m_ifmpForceDetach, dwOffset ) );
#endif

#ifdef DTC
	(*pcprintf)( FORMAT_POINTER( PIB, this, m_pvDistributedTrxData, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( PIB, this, m_cbDistributedTrxData, dwOffset ) );
#endif
	}


//  ================================================================
VOID FUCB::Dump( CPRINTF * pcprintf, DWORD_PTR ulBase ) const
//  ================================================================
	{
	if( 0 == ulBase )
		{
		ulBase = reinterpret_cast<DWORD_PTR>( this );
		}

	(*pcprintf)( FORMAT_POINTER( FUCB, this, pvtfndef, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, ppib, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pfucbNextOfSession, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, u.pfcb, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, u.pscb, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, ifmp, ulBase ) );

	(*pcprintf)( FORMAT_UINT( FUCB, this, ulFlags, ulBase ) );

	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fIndex					, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fSecondary				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fCurrentSecondary		, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fLV					, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fSort					, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fSystemTable			, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fWrite					, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fDenyRead				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fDenyWrite				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fPermitDDL				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fDeferClose			, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fVersioned				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fLimstat				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fInclusive				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fUpper					, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fMayCacheLVCursor		, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fUpdateSeparateLV		, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fDeferredChecksum		, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fAvailExt				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fOwnExt				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fSequential			, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fPreread				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fPrereadForward		, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fPrereadBackward		, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fBookmarkPreviouslySaved, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fTouch					, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fRepair				, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fAlwaysRetrieveCopy	, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fNeverRetrieveCopy		, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fTagImplicitOp			, ulBase ) );
	(*pcprintf)( FORMAT_BOOL_BF( FUCB, this, fSLVOwnerMapNeedUpdate	, ulBase ) );

	(*pcprintf)( FORMAT_VOID( FUCB, this, csr, ulBase ) );

	(*pcprintf)( FORMAT_VOID( FUCB, this, kdfCurr, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, locLogical, ulBase ) );

	(*pcprintf)( FORMAT_INT( FUCB, this, cbBMBuffer, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pvBMBuffer, ulBase ) );
	(*pcprintf)( FORMAT_VOID( FUCB, this, bmCurr, ulBase ) );
	(*pcprintf)( FORMAT_VOID( FUCB, this, rgbBMCache, ulBase ) );

	(*pcprintf)( FORMAT_POINTER( FUCB, this, pfucbCurIndex, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pfucbLV, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pfucbNextOfFile, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pfucbPrevOfFile, ulBase ) );

	(*pcprintf)( FORMAT_INT( FUCB, this, ispairCurr, ulBase ) );

	(*pcprintf)( FORMAT_INT( FUCB, this, keystat, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, cColumnsInSearchKey, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, fUsingTableSearchKeyBuffer, ulBase ) );
	(*pcprintf)( FORMAT_VOID( FUCB, this, dataSearchKey, ulBase ) );

	(*pcprintf)( FORMAT_INT( FUCB, this, rceidBeginUpdate, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, updateid, ulBase ) );
	(*pcprintf)( FORMAT_UINT( FUCB, this, ulChecksum, ulBase ) );

	(*pcprintf)( FORMAT_INT( FUCB, this, levelOpen, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, levelNavigate, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, levelPrep, ulBase ) );

	(*pcprintf)( FORMAT_INT( FUCB, this, cbstat, ulBase ) );

	(*pcprintf)( FORMAT_VOID( FUCB, this, dataWorkBuf, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pvWorkBuf, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pvRCEBuffer, ulBase ) );

	(*pcprintf)( FORMAT_VOID( FUCB, this, rgbitSet, ulBase ) );

	(*pcprintf)( FORMAT_INT( FUCB, this, cpgPreread, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, cpgPrereadNotConsumed, ulBase ) );
	(*pcprintf)( FORMAT_INT( FUCB, this, cbSequentialDataRead, ulBase ) );

	(*pcprintf)( FORMAT_POINTER( FUCB, this, pfucbTable, ulBase ) );

	(*pcprintf)( FORMAT_POINTER( FUCB, this, pcsrRoot, ulBase ) );
	(*pcprintf)( FORMAT_POINTER( FUCB, this, pfucbCacheNext, ulBase ) );

	(*pcprintf)( FORMAT_VOID( FUCB, this, ls, ulBase ) );

	(*pcprintf)( FORMAT_UINT( FUCB, this, ulLTLast, ulBase ) );
	(*pcprintf)( FORMAT_UINT( FUCB, this, ulTotalLast, ulBase ) );
	(*pcprintf)( FORMAT_UINT( FUCB, this, ulLTCurr, ulBase ) );
	(*pcprintf)( FORMAT_UINT( FUCB, this, ulTotalCurr, ulBase ) );
	}


//  ================================================================
VOID TDB::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidTaggedFirst, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidTaggedLastInitial, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidFixedFirst, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidFixedLastInitial, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidVarFirst, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidVarLastInitial, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidTaggedLast, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidFixedLast, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidVarLast, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_itagTableName, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_ibEndFixedColumns, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( TDB, this, m_pfieldsInitial, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( TDB, this, m_pdataDefaultRecord, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( TDB, this, m_pfcbTemplateTable, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( TDB, this, m_pfcbLV, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_ulLongIdLast, dwOffset ) );

	(*pcprintf)( FORMAT_INT( TDB, this, m_fidTaggedLastOfESE97Template, dwOffset ) );

	(*pcprintf)( FORMAT_INT( TDB, this, m_usFlags, dwOffset ) );
	PRINT_METHOD_FLAG( pcprintf, FTemplateTable );
	PRINT_METHOD_FLAG( pcprintf, FESE97TemplateTable );
	PRINT_METHOD_FLAG( pcprintf, FDerivedTable );
	PRINT_METHOD_FLAG( pcprintf, FESE97DerivedTable );
	PRINT_METHOD_FLAG( pcprintf, FTableHasSLVColumn );
	PRINT_METHOD_FLAG( pcprintf, F8BytesAutoInc );
	PRINT_METHOD_FLAG( pcprintf, FTableHasDefault );
	PRINT_METHOD_FLAG( pcprintf, FTableHasNonEscrowDefault );
	PRINT_METHOD_FLAG( pcprintf, FTableHasUserDefinedDefault );
	PRINT_METHOD_FLAG( pcprintf, FInitialisingDefaultRecord );

	(*pcprintf)( FORMAT_INT( TDB, this, m_qwAutoincrement, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidVersion, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_fidAutoincrement, dwOffset ) );
	(*pcprintf)( FORMAT_INT( TDB, this, m_dbkMost, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( TDB, this, m_mempool, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( TDB, this, m_blIndexes, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( TDB, this, m_rgbitAllIndex, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( TDB, this, m_rwlDDL, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( TDB, this, m_pcbdesc, dwOffset ) );
	}


//	CRES dumping

CHAR mpresidsz[ residMax - residMin ][ 16 ] =
	{
	"residFCB",
	"residFUCB",
	"residTDB",
	"residIDB",
	"residPIB",
	"residSCB",
	"residVER",
	};

//  ================================================================
VOID CRES::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_INT( CRES, this, m_cbBlock, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CRES, this, m_cBlocksAllocated, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CRES, this, m_pbBlocksAllocated, dwOffset ) );

	(*pcprintf)( FORMAT_INT( CRES, this, m_cBlockAvail, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CRES, this, m_pbBlockAvail, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CRES, this, m_cBlockCommit, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( CRES, this, m_pinst, dwOffset ) );

	if ( residVER == m_resid )
		{
		(*pcprintf)( FORMAT_INT( CRES, this, m_cBlockCommitThreshold, dwOffset ) );
		(*pcprintf)( FORMAT_INT( CRES, this, m_iBlockToCommit, dwOffset ) );
		}
	else
		{
		(*pcprintf)( FORMAT_POINTER( CRES, this, m_pbPreferredThreshold, dwOffset ) );
		}

	(*pcprintf)( FORMAT_VOID( CRES, this, m_crit, dwOffset ) );
	(*pcprintf)( FORMAT_ENUM( CRES, this, m_resid, dwOffset, mpresidsz, residMin, residMax ) );
	}


//  ================================================================
VOID INST::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_POINTER( INST, this, m_rgEDBGGlobals, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_rgfmp, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_rgpinst, dwOffset ) );

	PRINT_SZ_ON_HEAP( pcprintf, INST, this, m_szInstanceName, dwOffset );
	PRINT_SZ_ON_HEAP( pcprintf, INST, this, m_szDisplayName, dwOffset );
	(*pcprintf)( FORMAT_INT( INST, this, m_iInstance, dwOffset ) );

	(*pcprintf)( FORMAT_INT( INST, this, m_cSessionInJetAPI, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fJetInitialized, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fTermInProgress, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fTermAbruptly, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fSTInit, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fBackupAllowed, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fStopJetService, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_errInstanceUnavailable, dwOffset ) );

	(*pcprintf)( FORMAT_INT( INST, this, m_lSessionsMax, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lVerPagesMax, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lVerPagesPreferredMax, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fPreferredSetByUser, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lOpenTablesMax, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lOpenTablesPreferredMax, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lTemporaryTablesMax, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lCursorsMax, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lLogBuffers, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lLogFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fSetLogFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lLogFileSizeDuringRecovery, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( INST, this, m_fUseRecoveryLogFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_cpgSESysMin, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lPageFragment, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_cpageTempDBMin, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_grbitsCommitDefault, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfnRuntimeCallback, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( INST, this, m_chIndexTuplesLengthMin, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( INST, this, m_chIndexTuplesLengthMax, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( INST, this, m_chIndexTuplesToIndexMax, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( INST, this, m_ulParams, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( INST, this, m_fTempTableVersioning, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( INST, this, m_fCreatePathIfNotExist, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( INST, this, m_fCleanupMismatchedLogFiles, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( INST, this, m_fNoInformationEvent, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( INST, this, m_fSLVProviderEnabled, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL_BF( INST, this, m_fEnableIndexChecking, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( INST, this, m_fEnableIndexCleanup, dwOffset ) );

	(*pcprintf)( FORMAT_INT( INST, this, m_fOLDLevel, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lEventLoggingLevel, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( INST, this, m_idxunicodeDefault, dwOffset ) );

	(*pcprintf)( FORMAT_SZ( INST, this, m_szSystemPath, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( INST, this, m_szTempDatabase, dwOffset ) );

	(*pcprintf)( FORMAT_SZ( INST, this, m_szEventSource, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( INST, this, m_szEventSourceKey, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( INST, this, m_critLV, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_ppibLV, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_plog, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pver, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( INST, this, m_mpdbidifmp, dwOffset ) );

	(*pcprintf)( FORMAT_INT( INST, this, m_updateid, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( INST, this, m_critPIB, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_pcresPIBPool, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_ppibGlobal, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_ppibGlobalMin, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_ppibGlobalMax, dwOffset ) );

	(*pcprintf)( FORMAT_INT( INST, this, m_trxNewest, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_pcresFCBPool, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pcresTDBPool, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pcresIDBPool, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_cFCBPreferredThreshold, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfcbhash, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( INST, this, m_critFCBList, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfcbList, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfcbAvailBelowMRU, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfcbAvailBelowLRU, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfcbAvailAboveMRU, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfcbAvailAboveLRU, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( INST, this, m_cFCBAvail, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( INST, this, m_cFCBAboveThresholdSinceLastPurge, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( INST, this, m_critFCBCreate, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_pcresFUCBPool, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_pcresSCBPool, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( INST, this, m_fFlushLogForCleanThread, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_pbAttach, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( INST, this, m_taskmgr, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_cOpenedSystemPibs, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_cUsedSystemPibs, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_rgoldstatDB, dwOffset ) );

	(*pcprintf)( FORMAT_INT( INST, this, m_lSLVDefragFreeThreshold, dwOffset ) );
	(*pcprintf)( FORMAT_INT( INST, this, m_lSLVDefragMoveThreshold, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_pfsapi, dwOffset ) );

	(*pcprintf)( FORMAT_INT( INST, this, m_cNonLoggedIndexCreators, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( INST, this, m_plnppibBegin, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_plnppibEnd, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( INST, this, m_critLNPPIB, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( INST, this, m_cpls, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( INST, this, m_rgpls, dwOffset ) );


	// m_mpdbidifmp

	FMP *		rgfmpDebuggee;
	IFMP		ifmpMaxDebuggee		= ifmpMax;
	const BOOL	fReadGlobals		= ( FReadGlobal( "rgfmp", &rgfmpDebuggee )
										&& FReadGlobal( "ifmpMax", &ifmpMaxDebuggee ));

	(*pcprintf)( "\t\tDatabases:\n" );

	for ( DBID dbid = 0; dbid < dbidMax; dbid++ )
		{
		//	UNDONE: what's the correct value to use
		//	if we can't read ifmpMax from the
		//	debuggee
		if ( m_mpdbidifmp[dbid] < ifmpMaxDebuggee )
			{
			(*pcprintf)( "\t\t\t [%d] = IFMP:0x%x (FMP:",
							dbid,
							m_mpdbidifmp[dbid] );
			if ( fReadGlobals )
				{
				(*pcprintf)( "0x%p)\n", rgfmpDebuggee + m_mpdbidifmp[dbid] );
				}
			else
				{
				(*pcprintf)( "\?\?\?)\n" );
				}
			}
		}

	dprintf( "\n--------------------\n\n" );
	}


//  ================================================================
VOID FMP::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	PRINT_SZ_ON_HEAP( pcprintf, FMP, this, m_szDatabaseName, dwOffset );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pinst, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_dbid, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( FMP, this, m_fFlags, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fCreatingDB, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fAttachingDB, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fDetachingDB, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fExclusiveOpen, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fReadOnlyAttach, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fLogOn, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fVersioningOff, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fSkippedAttach, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fAttached, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fDeferredAttach, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fRunningOLD, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fInBackupSession, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fAllowForceDetach, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fForceDetaching, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fRedoSLVProviderNotEnabled, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fDuringSnapshot, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fAllowHeaderUpdate, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fDefragSLVCopy, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( FMP, this, m_fCopiedPatchHeader, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FMP, this, m_dbtimeLast, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_dbtimeOldestGuaranteed, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_dbtimeOldestCandidate, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_dbtimeOldestTarget, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( FMP, this, m_trxOldestCandidate, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( FMP, this, m_trxOldestTarget, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FMP, this, m_objidLast, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FMP, this, m_ctasksActive, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pfapi, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pdbfilehdr, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_critLatch, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( FMP, this, m_gateWriteLatch, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( FMP, this, m_cbFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( FMP, this, m_semExtendingDB, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( FMP, this, m_semIOExtendDB, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FMP, this, m_cPin, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_crefWriteLatch, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_ppibWriteLatch, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_ppibExclusiveOpen, dwOffset ) );

	(*pcprintf)( FORMAT_LGPOS( FMP, this, m_lgposAttach, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( FMP, this, m_lgposDetach, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_rwlDetaching, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( FMP, this, m_trxNewestWhenDiscardsLastReported, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_cpgDatabaseSizeMax, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_pgnoMost, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_pgnoCopyMost, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_semRangeLock, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( FMP, this, m_msRangeLock, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_rgprangelock, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_rgdwBFContext, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_rwlBFContext, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_ileBFICleanList, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( FMP, this, m_fBFICleanFlags, dwOffset ) );
	PRINT_METHOD_FLAG( pcprintf, FBFICleanDb );
	PRINT_METHOD_FLAG( pcprintf, FBFICleanSLV );

	(*pcprintf)( FORMAT_INT( FMP, this, m_errPatch, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_cpagePatch, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_cPatchIO, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pfapiPatch, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( FMP, this, m_ppatchhdr, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FMP, this, m_dbtimeCurrentDuringRecovery, dwOffset ) );
	(*pcprintf)( FORMAT_INT( FMP, this, m_dbtimeUndoForceDetach, dwOffset ) );

	(*pcprintf)( FORMAT_INT( FMP, this, m_dbtimeLastScrub, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( FMP, this, m_logtimeScrub, dwOffset ) );

	PRINT_SZ_ON_HEAP( pcprintf, FMP, this, m_szPatchPath, dwOffset );

	(*pcprintf)( FORMAT_POINTER( FMP, this, m_patchchk, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_patchchkRestored, dwOffset ) );

#ifdef DISABLE_SLV
#else

 	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pslvspacenodecache, dwOffset ) );

	PRINT_SZ_ON_HEAP( pcprintf, FMP, this, m_szSLVName, dwOffset );
	PRINT_SZ_ON_HEAP( pcprintf, FMP, this, m_szSLVRoot, dwOffset );

	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pfapiSLV, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_slvrootSLV, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pfcbSLVAvail, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( FMP, this, m_pfcbSLVOwnerMap, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_cbSLVFileSize, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( FMP, this, m_rwlSLVSpace, dwOffset ) );

	(*pcprintf)(	"\t%*.*s <0x%0*I64x,%3i>:  \n",
					SYMBOL_LEN_MAX,
					SYMBOL_LEN_MAX,
					"m_rgcslvspaceoper",
					sizeof( char* ) * 2,
					QWORD( (char*)this + dwOffset + OffsetOf( FMP, m_rgcslvspaceoper ) ),
					sizeof( (this)->m_rgcslvspaceoper ) );
	(*pcprintf)(	"\t\t\t\tSLV Space Operation Summary:\n" );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperFreeToReserved",
					m_rgcslvspaceoper[ slvspaceoperFreeToReserved ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperReservedToCommitted",
					m_rgcslvspaceoper[ slvspaceoperReservedToCommitted ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperFreeToCommitted",
					m_rgcslvspaceoper[ slvspaceoperFreeToCommitted ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperCommittedToDeleted",
					m_rgcslvspaceoper[ slvspaceoperCommittedToDeleted ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperDeletedToFree",
					m_rgcslvspaceoper[ slvspaceoperDeletedToFree ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperFree",
					m_rgcslvspaceoper[ slvspaceoperFree ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperFreeReserved",
					m_rgcslvspaceoper[ slvspaceoperFreeReserved ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"slvspaceoperDeletedToCommitted",
					m_rgcslvspaceoper[ slvspaceoperDeletedToCommitted ] );
	(*pcprintf)(	"\t%*.*s <0x%0*I64x,%3i>:  \n",
					SYMBOL_LEN_MAX,
					SYMBOL_LEN_MAX,
					"m_rgcslvspace",
					sizeof( char* ) * 2,
					QWORD( (char*)this + dwOffset + OffsetOf( FMP, m_rgcslvspace ) ),
					sizeof( (this)->m_rgcslvspace ) );

	(*pcprintf)(	"\t\t\t\tSLV Space Summary:\n" );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"sFree",
					m_rgcslvspace[ SLVSPACENODE::sFree ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"sReserved",
					m_rgcslvspace[ SLVSPACENODE::sReserved ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"sDeleted",
					m_rgcslvspace[ SLVSPACENODE::sDeleted ] );
	(*pcprintf)(	"\t\t\t\t\t%-*s:  %I64d\n",
					32,
					"sCommitted",
					m_rgcslvspace[ SLVSPACENODE::sCommitted ] );
#endif	//	DISABLE_SLV
	}

//  ================================================================
VOID LOG::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pinst, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLogInitialized, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLogDisabled, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLogDisabledDueToRecoveryFailure, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fNewLogRecordAdded, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLGNoMoreLogWrite, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_errNoMoreLogWrite, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_ls, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLogSequenceEnd, dwOffset ) );

	(*pcprintf)( FORMAT_INT( LOG, this, m_errCheckpointUpdate, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposCheckpointUpdateError, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fRecovering, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_fRecoveringMode, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fHardRestore, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_fRestoreMode, dwOffset ) );

	(*pcprintf)( FORMAT_INT( LOG, this, m_fSnapshotMode, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposSnapshotStart, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLGCircularLogging, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLGFMPLoaded, dwOffset ) );

#ifdef UNLIMITED_DB
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbLGDbListBuffer, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_cbLGDbListBuffer, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_cbLGDbListInUse, dwOffset ) );
	(*pcprintf)( FORMAT_UINT_BF( LOG, this, m_cLGAttachments, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( LOG, this, m_fLGNeedToLogDbList, dwOffset ) );
#endif

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fAlternateDbDirDuringRecovery, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szAlternateDbDirDuringRecovery, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( LOG, this, m_signLog, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fSignLogSet, dwOffset ) );

	(*pcprintf)( FORMAT_SZ( LOG, this, m_szRecovery, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( LOG, this, m_csecLGFile, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_csecLGBuf, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLGIgnoreVersion, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pfapiLog, dwOffset ) );

	(*pcprintf)( FORMAT_SZ( LOG, this, m_szBaseName, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szJet, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szJetLog, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szJetLogNameTemplate, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szJetTmp, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szJetTmpLog, dwOffset ) );

	(*pcprintf)( FORMAT_SZ( LOG, this, m_szLogName, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szLogFilePath, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fCreateAsynchLogFile, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pfapiJetTmpLog, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fCreateAsynchResUsed, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_errCreateAsynch, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_asigCreateAsynchIOCompleted, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_ibJetTmpLog, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposCreateAsynchTrigger, dwOffset ) );

	PRINT_SZ_ON_HEAP( pcprintf, LOG, this, m_szLogCurrent, dwOffset );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_plgfilehdr, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_plgfilehdrT, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbLGBufMin, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbLGBufMax, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_osmmLGBuf, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( LOG, this, m_cbLGBuf, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbEntry, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbWrite, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_isecWrite, dwOffset ) );

	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposLogRec, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposToFlush, dwOffset ) );

	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposMaxFlushPoint, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbLGFileEnd, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_isecLGFileEnd, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbLastChecksum, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposLastChecksum, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fHaveShadow, dwOffset ) );

	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposStart, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposRecoveryUndo, dwOffset ) );

	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposFullBackup, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_logtimeFullBackup, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposIncBackup, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_logtimeIncBackup, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( LOG, this, m_cbSec, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_cbSecVolume, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_csecHeader, dwOffset ) );

	(*pcprintf)( FORMAT_INT( LOG, this, m_fLGFlushWait, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_msLGTaskExec, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_asigLogFlushDone, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fLGFailedToPostFlushTask, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( LOG, this, m_critLGResFiles, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_critLGFlush, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_critLGBuf, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_critLGTrace, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_critLGWaitQ, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_ppibLGFlushQHead, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_ppibLGFlushQTail, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( LOG, this, m_cLGWrapAround, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pcheckpoint, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_critCheckpoint, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDisableCheckpoint, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fReplayingReplicatedLogFiles, dwOffset ) );
#ifdef IGNORE_BAD_ATTACH
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fReplayingIgnoreMissingDB, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_rceidLast, dwOffset ) );
#endif

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbNext, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pbRead, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_isecRead, dwOffset ) );

	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposRedo, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposLastRec, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fAbruptEnd, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_plread, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( LOG, this, m_lrstatePreread, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposPbNextPreread, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposLastChecksumPreread, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_cPagesPrereadInitial, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_cPagesPrereadThreshold, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_cPagesPrereadAmount, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_cPageRefsConsumed, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fPreread, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_rgpgnoLast, dwOffset ) );

	(*pcprintf)( FORMAT_INT( LOG, this, m_errGlobalRedoError, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( LOG, this, m_fAfterEndAllSessions, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( LOG, this, m_fLastLRIsShutdown, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( LOG, this, m_fNeedInitialDbList, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposRedoShutDownMarkGlobal, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_rgcppib, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pcppibAvail, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_ccppib, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_ptablehfhash, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( LOG, this, m_critBackupInProgress, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fBackupInProgress, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fStopBackup, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposFullBackupMark, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( LOG, this, m_logtimeFullBackupMark, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_lgenCopyMic, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_lgenCopyMac, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_lgenDeleteMic, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_lgenDeleteMac, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_ppibBackup, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fBackupFull, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fBackupSnapshot, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fBackupBeginNewLogFile, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_rgppatchlst, dwOffset ) );

	(*pcprintf)( FORMAT_SZ( LOG, this, m_szRestorePath, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( LOG, this, m_szNewDestination, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_rgrstmap, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_irstmapMac, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_fBackupStatus, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fScrubDB, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pscrubdb, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( LOG, this, m_ulSecsStartScrub, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_dbtimeLastScrubNew, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fExternalRestore, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_lGenLowRestore, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_lGenHighRestore, dwOffset ) );

	(*pcprintf)( FORMAT_SZ( LOG, this, m_szTargetInstanceLogPath, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_lGenHighTargetInstance, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pcheckpointDeleted, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDumppingLogs, dwOffset ) );

	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pttFirst, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( LOG, this, m_pttLast, dwOffset ) );

#ifdef DEBUG
	(*pcprintf)( FORMAT_LGPOS( LOG, this, m_lgposLastLogRec, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_dwDBGLogThreadId, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDBGTraceLog, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDBGTraceLogWrite, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDBGFreezeCheckpoint, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDBGTraceRedo, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDBGTraceBR, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDBGNoLog, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_cbDBGCopied, dwOffset ) );
#endif

	(*pcprintf)( FORMAT_VOID( LOG, this, m_rgrhf, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_crhfMac, dwOffset ) );
	(*pcprintf)( FORMAT_INT( LOG, this, m_cNOP, dwOffset ) );

	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDeleteOldLogs, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( LOG, this, m_fDeleteOutOfRangeLogs, dwOffset ) );
	}


//  ================================================================
VOID VER::Dump( CPRINTF * pcprintf, DWORD_PTR dwOffset ) const
//  ================================================================
	{
	(*pcprintf)( FORMAT_POINTER( VER, this, m_pinst, dwOffset ) );

	(*pcprintf)( FORMAT_INT( VER, this, m_fVERCleanUpWait, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( VER, this, m_ulVERTasksPostMax, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( VER, this, m_tickLastRCEClean, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( VER, this, m_msigRCECleanPerformedRecently, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( VER, this, m_asigRCECleanDone, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( VER, this, m_critRCEClean, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( VER, this, m_critBucketGlobal, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( VER, this, m_pbucketGlobalHead, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( VER, this, m_pbucketGlobalTail, dwOffset ) );
	(*pcprintf)( FORMAT_INT( VER, this, m_cbucketGlobalAlloc, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( VER, this, m_cbucketDynamicAlloc, dwOffset ) );
	(*pcprintf)( FORMAT_INT( VER, this, m_cbucketGlobalAllocMost, dwOffset ) );
	(*pcprintf)( FORMAT_INT( VER, this, m_cbucketGlobalAllocPreferred, dwOffset ) );

#ifdef MOVEABLE_RCE
	(*pcprintf)( FORMAT_POINTER( VER, this, m_pbucketGlobalLastDelete, dwOffset ) );
	(*pcprintf)( FORMAT_INT( VER, this, m_cbucketGlobalAllocDelete, dwOffset ) );
#endif

	(*pcprintf)( FORMAT_POINTER( VER, this, m_ppibRCEClean, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( VER, this, m_ppibRCECleanCallback, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( VER, this, m_ppibTrxOldestLastLongRunningTransaction, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( VER, this, m_dwTrxContextLastLongRunningTransaction, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( VER, this, m_trxBegin0LastLongRunningTransaction, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( VER, this, m_fSyncronousTasks, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( VER, this, m_pcresVERPool, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( VER, this, m_rgrceheadHashTable, dwOffset ) );
	}


//  ================================================================
DEBUG_EXT( EDBGDumpAllFMPs )
//  ================================================================
	{
	FMP *	rgfmpDebuggee		= NULL;
	ULONG 	ifmpMaxDebuggee;
	BOOL	fDumpAll			= fFalse;
	BOOL	fValidUsage;

	switch ( argc )
		{
		case 0:
			//	use defaults
			fValidUsage = fTrue;
			break;
		case 1:
			//	'*' only
			fValidUsage = ( '*' == argv[0][0] );
			break;
		case 2:
			//	<rgfmp> and <ifmpMax>
			fValidUsage = ( FAddressFromSz( argv[0], &rgfmpDebuggee )
							&& FUlFromSz( argv[1], &ifmpMaxDebuggee ) );
			break;
		case 3:
			if ( '*' == argv[0][0] )
				{
				//	'*' followed by <rgfmp> and <ifmpMax>
				fDumpAll = fTrue;
				fValidUsage = ( FAddressFromSz( argv[1], &rgfmpDebuggee )
								&& FUlFromSz( argv[2], &ifmpMaxDebuggee ) );
				}
			else if ( '*' == argv[2][0] )
				{
				//	<rgfmp> and <ifmpMax> followed by '*'
				fDumpAll = fTrue;
				fValidUsage = ( FAddressFromSz( argv[0], &rgfmpDebuggee )
								&& FUlFromSz( argv[1], &ifmpMaxDebuggee ) );
				}
			else
				{
				//	neither first nor third argument is a '*', so must be an error
				fValidUsage = fFalse;
				}
			break;
		default:
			fValidUsage = fFalse;
			break;
		}

	if ( !fValidUsage )
		{
		dprintf( "Usage: DUMPFMPS [<rgfmp> <ifmpMax>] [*]\n" );
		return;
		}

	if ( NULL == rgfmpDebuggee )
		{
		if ( !FReadGlobal( "rgfmp", &rgfmpDebuggee )
			|| !FReadGlobal( "ifmpMax", &ifmpMaxDebuggee ) )
			{
			dprintf( "Error: Could not fetch FMP variables.\n" );
			return;
			}
		}

	dprintf( "\nScanning 0x%X FMPs starting at 0x%p...\n", ifmpMaxDebuggee, rgfmpDebuggee );

	for ( IFMP ifmp = 0; ifmp < ifmpMaxDebuggee; ifmp++ )
		{
		FMP *	pfmp				= NULL;
		CHAR *	szDatabaseName		= NULL;

		if ( !FFetchVariable( rgfmpDebuggee + ifmp, &pfmp ) ||
			( pfmp->FInUse() && !FFetchSz( pfmp->SzDatabaseName(), &szDatabaseName ) ) )
			{
			dprintf(	"\n rgfmp[0x%x]  Error: Could not fetch FMP at 0x%p. Aborting.\n",
						ifmp,
						rgfmpDebuggee + ifmp );

			//	force out of loop
			ifmp = ifmpMaxDebuggee;
			}
		else
			{
			if ( szDatabaseName || fDumpAll )
				{
				dprintf(	"\n rgfmp[0x%x]  (FMP 0x%p)\n",
							ifmp,
							rgfmpDebuggee + ifmp );
				dprintf(	"           \t %*.*s : %s\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_szDatabaseName",
							szDatabaseName );
				dprintf(	"           \t %*.*s : 0x%p\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_pinst",
							pfmp->Pinst() );
				}
			}

		Unfetch( pfmp );
		Unfetch( szDatabaseName );
		}

	dprintf( "\n--------------------\n\n" );
	}

//  ================================================================
DEBUG_EXT( EDBGDumpAllINSTs )
//  ================================================================
	{
	INST **		rgpinstDebuggee		= NULL;
	INST **		rgpinst				= NULL;

	if ( 0 != argc
		&& ( 1 != argc || !FAddressFromSz( argv[0], &rgpinstDebuggee ) ) )
		{
		dprintf( "Usage: DUMPINSTS [<g_rgpinst>]\n" );
		return;
		}

	if ( ( NULL == rgpinstDebuggee && !FAddressFromGlobal( "g_rgpinst", &rgpinstDebuggee ) )
		|| !FFetchVariable( rgpinstDebuggee, &rgpinst, cMaxInstances ) )
		{
		dprintf( "Error: Could not fetch instance table.\n" );
		return;
		}

	dprintf( "\nScanning 0x%X INST's starting at 0x%p...\n", cMaxInstances, rgpinstDebuggee );

	for ( SIZE_T ipinst = 0; ipinst < cMaxInstances; ipinst++ )
		{
		if ( rgpinst[ipinst] != pinstNil )
			{
			INST *	pinst			= NULL;
			CHAR *	szInstanceName	= NULL;

			if ( !FFetchVariable( rgpinst[ ipinst ], &pinst )
				|| ( pinst->m_szInstanceName && !FFetchSz( pinst->m_szInstanceName, &szInstanceName ) ) )
				{
				dprintf(	"\n g_rgpinst[0x%x]  Error: Could not fetch INST at 0x%p. Aborting.\n",
							ipinst,
							rgpinst[ipinst] );

				//	force out of loop
				ipinst = cMaxInstances;
				}
			else
				{
				dprintf(	"\n g_rgpinst[0x%x]  (INST 0x%p)\n",
							ipinst,
							rgpinst[ipinst] );
				dprintf(	"              \t %*.*s : %s\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_szInstanceName",
							szInstanceName );
				dprintf(	"              \t %*.*s : %s\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_szSystemPath",
							pinst->m_szSystemPath );
				dprintf(	"              \t %*.*s : %s\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_szTempDatabase",
							pinst->m_szTempDatabase );
				dprintf(	"              \t %*.*s : 0x%p\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_pver",
							pinst->m_pver );
				dprintf(	"              \t %*.*s : 0x%p\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_plog",
							pinst->m_plog );
				dprintf(	"              \t %*.*s : 0x%p\n",
							SYMBOL_LEN_MAX,
							SYMBOL_LEN_MAX,
							"m_pfsapi",
							pinst->m_pfsapi );
				}

			Unfetch( pinst );
			Unfetch( szInstanceName );
			}
		}

	dprintf( "\n--------------------\n\n" );

	Unfetch( rgpinst );
	}


// =========================================================================
VOID UtilUpdateSymPath(
	const HANDLE hCurrentProcess,
	const char * szSymPathIn
	)
// =========================================================================
	{
	char szNewSymPath[_MAX_PATH];

	strncpy( szNewSymPath, szSymPathIn, _MAX_PATH );
	szNewSymPath[_MAX_PATH-1] = 0;

	if ( !pfnSymSetSearchPath( hCurrentProcess, szNewSymPath ) )
		{
		dprintf( "Updating sympath failed\n" );
		}

	if ( SymLoadAllModules( hCurrentProcess, fTrue ) )
		{
		pfnSymGetSearchPath(hCurrentProcess, szNewSymPath, sizeof( szNewSymPath ));
		dprintf( "SYMPATH: %s\n", szNewSymPath );
		}
	else
		{
		dprintf("Reload Modules Failed \n");
		}
	}



// =========================================================================
DEBUG_EXT( EDBGSympath )
// =========================================================================
	{
	if ( 0 == argc )  // dump current sympath
		{
		char szOldSympath[_MAX_PATH];

		if ( pfnSymGetSearchPath(hCurrentProcess, szOldSympath, sizeof( szOldSympath ) ) )
			{
			dprintf( "SYMPATH: %s\n", szOldSympath );
			}
		else
			{
			dprintf("Unable to dump the current sympath\n");
			}
		}
	else if ( 1 == argc )
		{
		UtilUpdateSymPath( hCurrentProcess, argv[0] );
		}
	else 	// print usage error
		{
		dprintf( "Usage: SYMPATH [<pathname>]\n" );
		}
	}


//	SPLIT dumping

CHAR mpsplittypesz[ splittypeMax - splittypeMin ][ 32 ] =
	{
	"splittypeVertical",
	"splittypeRight",
	"splittypeAppend",
	};

CHAR mpsplitopersz[ splitoperMax - splitoperMin ][ 64 ] =
	{
	"splitoperNone",
	"splitoperInsert",
	"splitoperReplace",
	"splitoperFlagInsertAndReplaceData",
	};

//  ================================================================
VOID CDUMPA<SPLIT>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	SPLIT *		psplitDebuggee		= NULL;
	SPLIT *		psplit				= NULL;

	if ( argc != 1 || !FAddressFromSz( argv[0], &psplitDebuggee ) )
		{
		dprintf( "Usage: DUMP SPLIT <address>\n" );
		return;
		}

	if ( FFetchVariable( psplitDebuggee, &psplit ) )
		{
		const SIZE_T	dwOffset	= (BYTE *)psplitDebuggee - (BYTE *)psplit;

		dprintf(	"[SPLIT] 0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( SPLIT ) ),
					psplitDebuggee );

		dprintf( FORMAT_VOID( SPLIT, psplit, csrNew, dwOffset ) );
		dprintf( FORMAT_VOID( SPLIT, psplit, csrRight, dwOffset ) );
		dprintf( FORMAT_UINT( SPLIT, psplit, pgnoSplit, dwOffset ) );
		dprintf( FORMAT_UINT( SPLIT, psplit, pgnoNew, dwOffset ) );
		dprintf( FORMAT_INT( SPLIT, psplit, dbtimeRightBefore, dwOffset ) );
		dprintf( FORMAT_ENUM( SPLIT, psplit, splittype, dwOffset, mpsplittypesz, splittypeMin, splittypeMax ) );
		dprintf( FORMAT_POINTER( SPLIT, psplit, psplitPath, dwOffset ) );
		dprintf( FORMAT_ENUM( SPLIT, psplit, splitoper, dwOffset, mpsplitopersz, splitoperMin, splitoperMax ) );
		dprintf( FORMAT_INT( SPLIT, psplit, ilineOper, dwOffset ) );
		dprintf( FORMAT_INT( SPLIT, psplit, clines, dwOffset ) );
		dprintf( FORMAT_UINT( SPLIT, psplit, fNewPageFlags, dwOffset ) );
		dprintf( FORMAT_UINT( SPLIT, psplit, fSplitPageFlags, dwOffset ) );
		dprintf( FORMAT_INT( SPLIT, psplit, cbUncFreeDest, dwOffset ) );
		dprintf( FORMAT_INT( SPLIT, psplit, cbUncFreeSrc, dwOffset ) );
		dprintf( FORMAT_INT( SPLIT, psplit, ilineSplit, dwOffset ) );
		dprintf( FORMAT_VOID( SPLIT, psplit, prefixinfoSplit, dwOffset ) );
		dprintf( FORMAT_VOID( SPLIT, psplit, prefixSplitOld, dwOffset ) );
		dprintf( FORMAT_VOID( SPLIT, psplit, prefixSplitNew, dwOffset ) );
		dprintf( FORMAT_VOID( SPLIT, psplit, prefixinfoNew, dwOffset ) );
		dprintf( FORMAT_VOID( SPLIT, psplit, kdfParent, dwOffset ) );
		dprintf( FORMAT_BOOL_BF( SPLIT, psplit, fAllocParent, dwOffset ) );
		dprintf( FORMAT_BOOL_BF( SPLIT, psplit, fHotpoint, dwOffset ) );
		dprintf( FORMAT_POINTER( SPLIT, psplit, rglineinfo, dwOffset ) );

		Unfetch( psplit );
		}
	}

// SPLITPATH dumping
//  ================================================================
VOID CDUMPA<SPLITPATH>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	SPLITPATH *		psplitpathDebuggee	= NULL;
	SPLITPATH *		psplitpath			= NULL;

	if ( argc != 1 || !FAddressFromSz( argv[0], &psplitpathDebuggee ) )
		{
		dprintf( "Usage: DUMP SPLITPATH <address>\n" );
		return;
		}

	if ( FFetchVariable( psplitpathDebuggee, &psplitpath ) )
		{
		const SIZE_T	dwOffset		= (BYTE *)psplitpathDebuggee - (BYTE *)psplitpath;

		dprintf(	"[SPLITPATH] 0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( SPLITPATH ) ),
					psplitpathDebuggee );

		dprintf( FORMAT_VOID( SPLITPATH, psplitpath, csr, dwOffset ) );
		dprintf( FORMAT_INT( SPLITPATH, psplitpath, dbtimeBefore, dwOffset ) );
		dprintf( FORMAT_POINTER( SPLITPATH, psplitpath, psplitPathParent, dwOffset ) );
		dprintf( FORMAT_POINTER( SPLITPATH, psplitpath, psplitPathChild, dwOffset ) );
		dprintf( FORMAT_POINTER( SPLITPATH, psplitpath, psplit, dwOffset ) );

		Unfetch( psplitpath );
		}
	}


//	MERGE dumping

CHAR mpmergetypesz[ mergetypeMax - mergetypeMin ][ 32 ] =
	{
	"mergetypeNone",
	"mergetypeEmptyPage",
	"mergetypeFullRight",
	"mergetypePartialRight",
	"mergetypeEmptyTree",
	};

//  ================================================================
VOID CDUMPA<MERGE>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	MERGE *		pmergeDebuggee		= NULL;
	MERGE *		pmerge				= NULL;

	if ( argc != 1 || !FAddressFromSz( argv[0], &pmergeDebuggee ) )
		{
		dprintf( "Usage: DUMP MERGE <address>\n" );
		return;
		}

	if ( FFetchVariable( pmergeDebuggee, &pmerge ) )
		{
		const SIZE_T	dwOffset	= (BYTE *)pmergeDebuggee - (BYTE *)pmerge;

		dprintf(	"[MERGE] 0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( MERGE ) ),
					pmergeDebuggee );

		dprintf( FORMAT_VOID( MERGE, pmerge, csrLeft, dwOffset ) );
		dprintf( FORMAT_VOID( MERGE, pmerge, csrRight, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, dbtimeLeftBefore, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, dbtimeRightBefore, dwOffset ) );
		dprintf( FORMAT_ENUM( MERGE, pmerge, mergetype, dwOffset, mpmergetypesz, mergetypeMin, mergetypeMax ) );
		dprintf( FORMAT_POINTER( MERGE, pmerge, pmergePath, dwOffset ) );
		dprintf( FORMAT_VOID( MERGE, pmerge, kdfParentSep, dwOffset ) );
		dprintf( FORMAT_BOOL_BF( MERGE, pmerge, fAllocParentSep, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, ilineMerge, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, cbSavings, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, cbSizeTotal, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, cbSizeMaxTotal, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, cbUncFreeDest, dwOffset ) );
		dprintf( FORMAT_INT( MERGE, pmerge, clines, dwOffset ) );
		dprintf( FORMAT_POINTER( MERGE, pmerge, rglineinfo, dwOffset ) );

		Unfetch( pmerge );
		}
	}

// MERGEPATH dumping
//  ================================================================
VOID CDUMPA<MERGEPATH>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	MERGEPATH *		pmergepathDebuggee	= NULL;
	MERGEPATH *		pmergepath			= NULL;

	if ( argc != 1 || !FAddressFromSz( argv[0], &pmergepathDebuggee ) )
		{
		dprintf( "Usage: DUMP MERGEPATH <address>\n" );
		return;
		}

	if ( FFetchVariable( pmergepathDebuggee, &pmergepath ) )
		{
		const SIZE_T	dwOffset		= (BYTE *)pmergepathDebuggee - (BYTE *)pmergepath;

		dprintf(	"[MERGEPATH] 0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( MERGEPATH ) ),
					pmergepathDebuggee );

		dprintf( FORMAT_VOID( MERGEPATH, pmergepath, csr, dwOffset ) );
		dprintf( FORMAT_INT( MERGEPATH, pmergepath, dbtimeBefore, dwOffset ) );
		dprintf( FORMAT_POINTER( MERGEPATH, pmergepath, pmergePathParent, dwOffset ) );
		dprintf( FORMAT_POINTER( MERGEPATH, pmergepath, pmergePathChild, dwOffset ) );
		dprintf( FORMAT_POINTER( MERGEPATH, pmergepath, pmerge, dwOffset ) );
		dprintf( FORMAT_INT( MERGEPATH, pmergepath, iLine, dwOffset ) );
		dprintf( FORMAT_BOOL_BF( MERGEPATH, pmergepath, fKeyChange, dwOffset ) );
		dprintf( FORMAT_BOOL_BF( MERGEPATH, pmergepath, fDeleteNode, dwOffset ) );
		dprintf( FORMAT_BOOL_BF( MERGEPATH, pmergepath, fEmptyPage, dwOffset ) );

		Unfetch( pmergepath );
		}
	}


// DBFILEHDR dumping
//  ================================================================
VOID CDUMPA<DBFILEHDR>::Dump(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    INT argc, const CHAR * const argv[]
    )
//  ================================================================
	{
	DBFILEHDR *		pdbfilehdrDebuggee	= NULL;
	DBFILEHDR *		pdbfilehdr			= NULL;

	if ( argc != 1 || !FAddressFromSz( argv[0], &pdbfilehdrDebuggee ) )
		{
		dprintf( "Usage: DUMP DBFILEHDR <address>\n" );
		return;
		}

	if ( FFetchVariable( pdbfilehdrDebuggee, &pdbfilehdr ) )
		{
		const SIZE_T	dwOffset		= (BYTE *)pdbfilehdrDebuggee - (BYTE *)pdbfilehdr;

		dprintf(	"[DBFILEHDR] 0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( DBFILEHDR ) ),
					pdbfilehdrDebuggee );

		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_ulChecksum, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_ulMagic, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_ulVersion, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_attrib, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_dbtimeDirtied, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, signDb, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_dbstate, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, le_lgposConsistent, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, logtimeConsistent, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, logtimeAttach, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, le_lgposAttach, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, logtimeDetach, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, le_lgposDetach, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_dbid, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, signLog, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, bkinfoFullPrev, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, bkinfoIncPrev, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, bkinfoFullCur, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, m_ulDbFlags, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_objidLast, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_dwMajorVersion, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_dwMinorVersion, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_dwBuildNumber, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_lSPNumber, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_ulUpdate, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_cbPageSize, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_ulRepairCount, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, logtimeRepair, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, signSLV, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_dbtimeLastScrub, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, logtimeScrub, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_lGenMinRequired, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_lGenMaxRequired, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_cpgUpgrade55Format, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_cpgUpgradeFreePages, dwOffset ) );
		dprintf( FORMAT_INT( DBFILEHDR, pdbfilehdr, le_cpgUpgradeSpaceMapPages, dwOffset ) );
		dprintf( FORMAT_VOID( DBFILEHDR, pdbfilehdr, bkinfoSnapshotCur, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_ulCreateVersion, dwOffset ) );
		dprintf( FORMAT_UINT( DBFILEHDR, pdbfilehdr, le_ulCreateUpdate, dwOffset ) );

		Unfetch( pdbfilehdr );
		}
	}


// Dump members of CPAGE class
VOID CPAGE::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
    {
	(*pcprintf)( FORMAT_POINTER( CPAGE, this, m_ppib, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CPAGE, this, m_ifmp, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CPAGE, this, m_pgno, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CPAGE, this, m_bfl, dwOffset ) );
	}


//  BF Dumping

CHAR mpbfdfsz[ bfdfMax - bfdfMin ][ 16 ] =
	{
	"bfdfClean",
	"bfdfUntidy",
	"bfdfDirty",
	"bfdfFilthy",
	};

CHAR mpbflssz[ bflsMax - bflsMin ][ 16 ] =
	{
	"bflsNormal",
	"bflsNominee1",
	"bflsNominee2",
	"bflsNominee3",
	"bflsElect",
	"bflsHashed",
	};

CHAR mpbfrssz[ bfrsMax - bfrsMin ][ 32 ] =
	{
	"bfrsNotCommitted",
	"bfrsNewlyCommitted",
	"bfrsNotResident",
	"bfrsResident",
	};

void BF::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
#ifdef _WIN64

	(*pcprintf)( FORMAT_VOID( BF, this, ob0ic, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( BF, this, lgposOldestBegin0, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( BF, this, lgposModify, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( BF, this, sxwl, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, ifmp, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( BF, this, pgno, dwOffset ) );
	(*pcprintf)( FORMAT_INT( BF, this, err, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fNewlyEvicted, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fQuiesced, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fAvailable, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fMemory, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fWARLatch, dwOffset ) );
	(*pcprintf)( FORMAT_ENUM_BF( BF, this, bfdf, dwOffset, mpbfdfsz, bfdfMin, bfdfMax ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fInOB0OL, dwOffset ) );
	(*pcprintf)( FORMAT_UINT_BF( BF, this, irangelock, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fCurrentVersion, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fOlderVersion, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fFlushed, dwOffset ) );
	(*pcprintf)( FORMAT_ENUM_BF( BF, this, bfls, dwOffset, mpbflssz, bflsMin, bflsMax ) );
	if ( bfls == bflsHashed )
		{
		(*pcprintf)( FORMAT_UINT( BF, this, iHashedLatch, dwOffset ) );
		}
	else
		{
		(*pcprintf)( FORMAT_UINT( BF, this, tickEligibleForNomination, dwOffset ) );
		}
	(*pcprintf)( FORMAT_ENUM_BF( BF, this, bfrs, dwOffset, mpbfrssz, bfrsMin, bfrsMax ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pv, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( BF, this, lrukic, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfTimeDepChainPrev, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfTimeDepChainNext, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( BF, this, fDependentPurged, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, prceUndoInfoNext, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfDependent, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfDepChainHeadPrev, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfDepChainHeadNext, dwOffset ) );

#else  //  !_WIN64

	(*pcprintf)( FORMAT_VOID( BF, this, ob0ic, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( BF, this, lgposOldestBegin0, dwOffset ) );
	(*pcprintf)( FORMAT_LGPOS( BF, this, lgposModify, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfTimeDepChainPrev, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfTimeDepChainNext, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( BF, this, sxwl, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfDependent, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfDepChainHeadPrev, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pbfDepChainHeadNext, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, ifmp, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( BF, this, pgno, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, pv, dwOffset ) );
	(*pcprintf)( FORMAT_INT( BF, this, err, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fNewlyEvicted, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fQuiesced, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fAvailable, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fMemory, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fWARLatch, dwOffset ) );
	(*pcprintf)( FORMAT_ENUM_BF( BF, this, bfdf, dwOffset, mpbfdfsz, bfdfMin, bfdfMax ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fInOB0OL, dwOffset ) );
	(*pcprintf)( FORMAT_UINT_BF( BF, this, irangelock, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fCurrentVersion, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fOlderVersion, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL_BF( BF, this, fFlushed, dwOffset ) );
	(*pcprintf)( FORMAT_ENUM_BF( BF, this, bfls, dwOffset, mpbflssz, bflsMin, bflsMax ) );
	if ( bfls == bflsHashed )
		{
		(*pcprintf)( FORMAT_UINT( BF, this, iHashedLatch, dwOffset ) );
		}
	else
		{
		(*pcprintf)( FORMAT_UINT( BF, this, tickEligibleForNomination, dwOffset ) );
		}
	(*pcprintf)( FORMAT_ENUM_BF( BF, this, bfrs, dwOffset, mpbfrssz, bfrsMin, bfrsMax ) );
	(*pcprintf)( FORMAT_POINTER( BF, this, prceUndoInfoNext, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( BF, this, fDependentPurged, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( BF, this, lrukic, dwOffset ) );

#endif  //  _WIN64
	}


template< class CKey, class CEntry >
VOID CDynamicHashTable< CKey, CEntry >::Dump(
	CPRINTF *			pcprintf,
	const DWORD_PTR		dwOffset ) const
	{
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_cLoadFactor, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_centryBucket, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_cbBucket, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_rankDHTrwlBucket, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CDynamicHashTable, this, m_rghs, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_chs, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_cbucketMin, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( CDynamicHashTable, this, m_rgbRsvdNever, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( CDynamicHashTable, this, m_dirptrs, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CDynamicHashTable, this, m_rgrgBucket, dwOffset ) );

	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_cOpSensitivity, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_cBucketPreferred, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_stateCur, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( CDynamicHashTable, this, m_rgbRsvdOften, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( CDynamicHashTable, this, m_semPolicy, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CDynamicHashTable, this, m_cCompletions, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( CDynamicHashTable, this, m_rgbRsvdAlways, dwOffset ) );
	}

template< class CKey, class CEntry, PfnOffsetOf OffsetOfIC >
VOID CApproximateIndex< CKey, CEntry, OffsetOfIC >::Dump(
	CPRINTF *			pcprintf,
	const DWORD_PTR		dwOffset ) const
	{
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_shfKeyPrecision, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_shfKeyUncertainty, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_shfBucketHash, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_shfFillMSB, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_maskBucketKey, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_maskBucketPtr, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_maskBucketID, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_didRangeMost, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( CApproximateIndex, this, m_critUpdateIdRange, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_cidRange, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_idRangeFirst, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CApproximateIndex, this, m_idRangeLast, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CApproximateIndex, this, m_rgbReserved2, dwOffset ) );

	(*pcprintf)( FORMAT_VOID( CApproximateIndex, this, m_bt, dwOffset ) );
	}

VOID CDUMPA<CDynamicHashTableEDBG>::Dump(
	HANDLE					hCurrentProcess,
	HANDLE					hCurrentThread,
	DWORD					dwCurrentPc,
	PWINDBG_EXTENSION_APIS	lpExtensionApis,
	INT						argc,
	const CHAR * const		argv[] )
//  ================================================================
	{
	CDynamicHashTableEDBG *	pdhtDebuggee	= NULL;
	CDynamicHashTableEDBG *	pdht			= NULL;

	if ( argc != 1 || !FAddressFromSz( argv[0], &pdhtDebuggee ) )
		{
		dprintf( "Usage: DUMP CDynamicHashTable <address>\n" );
		return;
		}

	if ( FFetchVariable( pdhtDebuggee, &pdht ) )
		{
		const SIZE_T	dwOffset		= (BYTE *)pdhtDebuggee - (BYTE *)pdht;

		dprintf(	"[CDynamicHashTable] 0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( CDynamicHashTableEDBG ) ),
					pdhtDebuggee );

		pdht->Dump( CPRINTFWDBG::PcprintfInstance(), dwOffset );

		Unfetch( pdht );
		}
	}

VOID CDUMPA<CApproximateIndexEDBG>::Dump(
	HANDLE					hCurrentProcess,
	HANDLE					hCurrentThread,
	DWORD					dwCurrentPc,
	PWINDBG_EXTENSION_APIS	lpExtensionApis,
	INT						argc,
	const CHAR * const		argv[] )
//  ================================================================
	{
	CApproximateIndexEDBG *	paiDebuggee	= NULL;
	CApproximateIndexEDBG *	pai			= NULL;

	if ( argc != 1 || !FAddressFromSz( argv[0], &paiDebuggee ) )
		{
		dprintf( "Usage: DUMP CApproximateIndex <address>\n" );
		return;
		}

	if ( FFetchVariable( paiDebuggee, &pai ) )
		{
		const SIZE_T	dwOffset		= (BYTE *)paiDebuggee - (BYTE *)pai;

		dprintf(	"[CApproximateIndex] 0x%0*I64X bytes @ 0x%p\n",
					sizeof( size_t ) * 2,
					QWORD( sizeof( CApproximateIndexEDBG ) ),
					paiDebuggee );

		pai->Dump( CPRINTFWDBG::PcprintfInstance(), dwOffset );

		Unfetch( pai );
		}
	}


struct IFMPGEN
	{
	IFMP	ifmp;
	ULONG	lgen;
	};

DEBUG_EXT( EDBGBFOB0FindOldest )
	{
	IFMPGEN			ifmpgen				= { 0, 0 };
	FMP *			rgfmpDebuggee		= NULL;
	FMP *			pfmp				= NULL;
	ULONG			ifmpMaxDebuggee;
	BFFMPContext *	pbffmp;
	BOOL			fValidUsage;

	switch ( argc )
		{
		case 1:
			fValidUsage = ( FUlFromSz( argv[0], (ULONG *)&ifmpgen.ifmp ) );
			break;

		case 2:
			fValidUsage = ( FUlFromSz( argv[0], (ULONG *)&ifmpgen.ifmp )
							&& FUlFromSz( argv[1], &ifmpgen.lgen )
							&& ifmpgen.lgen > 0 );
			break;

		default:
			fValidUsage = fFalse;
			break;
		}

	dprintf( "\n" );

	if ( !fValidUsage )
		{
		dprintf( "Usage: BFOB0FINDOLDEST <ifmp> [<gen>]\n" );
		dprintf( "\n" );
		dprintf( "    <ifmp> - index to the FMP entry for the desired database file\n" );
		dprintf( "    <gen>  - find all BF's with lgposBegin0 less than or equal to\n" );
		dprintf( "             specified log generation\n" );
		return;
		}

	if ( !FReadGlobal( "rgfmp", &rgfmpDebuggee )
		|| !FReadGlobal( "ifmpMax", &ifmpMaxDebuggee ) )
		{
		dprintf( "Error: Could not fetch FMP variables.\n" );
		return;
		}

	if ( ifmpgen.ifmp >= ifmpMaxDebuggee )
		{
		dprintf( "Error: Illegal ifmp (ifmpMax==0x%x)\n", ifmpMaxDebuggee );
		return;
		}

	if ( !FFetchVariable( rgfmpDebuggee + ifmpgen.ifmp, &pfmp ) )
		{
		return;
		}

	if ( FFetchVariable( (BFFMPContext *)pfmp->DwBFContext( 0 ), &pbffmp ) )
		{
		dprintf( "BFOB0: 0x%p\n", pfmp->DwBFContext( 0 ) );
		pbffmp->bfob0.Scan( CPRINTFWDBG::PcprintfInstance(), (VOID *)&ifmpgen );

		Unfetch( pbffmp );
		}

	Unfetch( pfmp );
	}

VOID BFOB0::CBucketTable::Scan( CPRINTF * pcprintf, VOID * pv ) const
	{
	const IFMPGEN *	pifmpgen				= (IFMPGEN *)pv;
	const SIZE_T	crgBucket				= sizeof(m_rgrgBucket) / sizeof(m_rgrgBucket[0]);
	BYTE *			rgbBucket				= NULL;
	BYTE *			pbBucket				= NULL;
	PBF				pbf						= pbfNil;
	PBF				pbfOldestBegin0			= pbfNil;
	LGPOS			lgposOldestBegin0		= lgposMax;
	SIZE_T			ientryOldestBegin0		= 0;
	SIZE_T			iBucketOldestBegin0		= 0;
	SIZE_T			irgBucketOldestBegin0	= 0;
	BOOL			fFoundTargetGen			= fFalse;

	pbBucket = (BYTE *)LocalAlloc( 0, m_cbBucket );
	if ( NULL == pbBucket )
		{
		(*pcprintf)( "Error: Could not allocate BUCKET buffer.\n" );
		goto HandleError;
		}

	pbf = (PBF)LocalAlloc( 0, sizeof(BF) );
	if ( NULL == pbf )
		{
		(*pcprintf)( "Error: Could not allocate BF buffer.\n" );
		goto HandleError;
		}

	if ( pifmpgen->lgen > 0 )
		{
		(*pcprintf)(
			"Scanning BFOB0 [bucket array/bucket/entry] for ifmp 0x%x with lgposBegin0 at generation %d (0x%x) or older:\n",
			pifmpgen->ifmp,
			pifmpgen->lgen,
			pifmpgen->lgen );
		}
	else
		{
		(*pcprintf)( "Scanning all BFOB0 entries [bucket array/bucket/entry]...\n" );
		}

	//	scan each DHT directory entry
	//
    for ( SIZE_T irgBucket = 0; irgBucket < crgBucket && NULL != m_rgrgBucket[irgBucket]; irgBucket++ )
    	{
		//	number of buckets grows exponentially, with a minimum of 2
		//
		const PBUCKET	pbucket				= PBUCKET( pbBucket );
		const SIZE_T	cBuckets			= max( 2, 1 << irgBucket );
		BYTE * const	rgbBucketDebuggee	= m_rgrgBucket[irgBucket];

		if ( !FFetchVariable( rgbBucketDebuggee, &rgbBucket, m_cbBucket * cBuckets ) )
			{
			(*pcprintf)( "Error: Could not fetch bucket array at 0x%p.\n", rgbBucketDebuggee );
			goto HandleError;
			}

		//	scan all DHT buckets in this directory entry
		//
		for ( SIZE_T iBucket = 0; iBucket < cBuckets; iBucket++ )
			{
			//	scan this bucket and its overflow buckets
			//
			BOOL		fLastBucketInChain	= fFalse;
			BYTE *		pbBucketDebuggee	= rgbBucketDebuggee + ( iBucket * m_cbBucket );
			while ( !fLastBucketInChain )
				{
				const CKeyEntry *	pEntryMost;

				//	UNDONE: don't actually need to read the first bucket (it's
				//	already read in in the bucket array as the head of this
				//	chain), but it makes the loop easier to code if we do
				//
				if ( !FReadVariable( pbBucketDebuggee, pbBucket, m_cbBucket ) )
					{
					(*pcprintf)( "Error: Could not read bucket at 0x%p.\n", pbBucketDebuggee );
					goto HandleError;
					}

				if ( NULL == pbucket->m_pb )
					{
					//	bucket was empty, so end of chain was reached
					//
					fLastBucketInChain = fTrue;
					pEntryMost = pbucket->m_rgEntry;
					}
				else if ( pbucket->m_pb > pbBucketDebuggee
					&& pbucket->m_pb < pbBucketDebuggee + m_cbBucket )
					{
					//	m_pEntryLast is valid, so end of chain was reached
					//
					fLastBucketInChain = fTrue;
					pEntryMost = (CKeyEntry *)( pbBucket + ( pbucket->m_pb - pbBucketDebuggee ) );

					//	m_pEntryLast actually points to the last valid entry,
					//	so move one past
					//
					pEntryMost++;
					}
				else
					{
					pEntryMost = pbucket->m_rgEntry + m_centryBucket;
					}

				//	scan all entries in this bucket
				//

				for ( CKeyEntry * pEntryThis = pbucket->m_rgEntry;
					pEntryThis < pEntryMost;
					pEntryThis++ )
					{
					//	scan invasive list for this entry
					//
					BFOB0::CBucket		listHead;
					pEntryThis->GetEntry( &listHead );

					for ( PBF pbfDebuggee = listHead.m_il.PrevMost();
						pbfNil != pbfDebuggee;
						pbfDebuggee = listHead.m_il.Next( pbf ) )
						{
						if ( !FReadVariable( pbfDebuggee, pbf ) )
							{
							(*pcprintf)( "Error: Could not read BF at 0x%p.\n", pbfDebuggee );
							goto HandleError;
							}

						if ( pbf->ifmp == pifmpgen->ifmp
							&& ( pbf->fCurrentVersion || pbf->fOlderVersion ) )
							{
							if ( pifmpgen->lgen > 0 )
								{
								if ( pbf->lgposOldestBegin0.lGeneration <= pifmpgen->lgen )
									{
									(*pcprintf)(
										"    [%d/%d/%d] BF 0x%p: gen %d (0x%x)\n",
										irgBucket,
										iBucket,
										( pEntryThis - pbucket->m_rgEntry ),
										pbfDebuggee,
										pbf->lgposOldestBegin0.lGeneration,
										pbf->lgposOldestBegin0.lGeneration );
									fFoundTargetGen = fTrue;
									}
								}

							if ( CmpLgpos( &pbf->lgposOldestBegin0, &lgposOldestBegin0 ) < 0 )
								{
								lgposOldestBegin0 = pbf->lgposOldestBegin0;
								pbfOldestBegin0 = pbfDebuggee;
								irgBucketOldestBegin0 = irgBucket;
								iBucketOldestBegin0 = iBucket;
								ientryOldestBegin0 = ( pEntryThis - pbucket->m_rgEntry );
								}
							}
						}
					}

				pbBucketDebuggee = (BYTE *)pbucket->m_pBucketNext;
				}
			}

		Unfetch( rgbBucket );
		rgbBucket = NULL;
    	}

	if ( pifmpgen->lgen > 0 && !fFoundTargetGen )
		{
		//	didn't find any BF for this ifmp
		//
		(*pcprintf)( "    <none>\n" );
		}

	if ( pbfNil != pbfOldestBegin0 )
		{
		(*pcprintf)(
			"Oldest lgposBegin0 (generation 0x%x) for ifmp 0x%x is cached in BF 0x%p,",
			lgposOldestBegin0.lGeneration,
			pifmpgen->ifmp,
			pbfOldestBegin0 );
		(*pcprintf)(
			"which is in the BFOB0 at [%d/%d/%d].\n",
			irgBucketOldestBegin0,
			iBucketOldestBegin0,
			ientryOldestBegin0 );
		}
	else
		{
		(*pcprintf)( "No pages found for ifmp 0x%x.\n", pifmpgen->ifmp );
		}

HandleError:
	(*pcprintf)( "\n" );

	LocalFree( pbf );
	LocalFree( pbBucket );
	Unfetch( rgbBucket );
	}


#ifdef DISABLE_SLV
#else

void CSLVFileInfo::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_BOOL( CSLVFileInfo, this, m_fFreeCache, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVFileInfo, this, m_fUpdateChecksum, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVFileInfo, this, m_fUpdateSlist, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVFileInfo, this, m_fCloseBuffer, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVFileInfo, this, m_fCloseCursor, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVFileInfo, this, m_rgbLocalCache, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_rgbCache, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSLVFileInfo, this, m_cbCache, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_pffeainf, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSLVFileInfo, this, m_cbffeainf, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_wszFileName, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVFileInfo, this, m_cwchFileName, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_pstatusCommit, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_pdwInstanceID, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_pdwChecksum, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_ptickOpenDeadline, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_pslist, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSLVFileInfo, this, m_cbslist, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSLVFileInfo, this, m_irun, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileInfo, this, m_psle, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVFileInfo, this, m_buffer, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVFileInfo, this, m_cursor, dwOffset ) );
	}

void CSLVFileTable::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_BOOL( CSLVFileTable, this, m_fInit, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVFileTable, this, m_pslvroot, dwOffset ) );
	(*pcprintf)( FORMAT_INT( CSLVFileTable, this, m_cDeferredCleanup, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVFileTable, this, m_semCleanup, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVFileTable, this, m_fileidNextCleanup, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVFileTable, this, m_cbReserved, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVFileTable, this, m_cbDeleted, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVFileTable, this, m_centryInsert, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVFileTable, this, m_centryDelete, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVFileTable, this, m_centryClean, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVFileTable, this, m_et, dwOffset ) );
	}

void CSLVInfo::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_POINTER( CSLVInfo, this, m_pfucb, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_columnid, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_itagSequence, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVInfo, this, m_fCopyBuffer, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_ibOffsetChunkMic, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_ibOffsetChunkMac, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_ibOffsetRunMic, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_ibOffsetRunMac, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_ibOffsetRun, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVInfo, this, m_fCacheDirty, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( CSLVInfo, this, m_pvCache, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_cbCache, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVInfo, this, m_rgbSmallCache, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVInfo, this, m_fHeaderDirty, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVInfo, this, m_header, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( CSLVInfo, this, m_fFileNameVolatile, dwOffset ) );
	(*pcprintf)( FORMAT_WSZ( CSLVInfo, this, m_wszFileName, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( CSLVInfo, this, m_fileid, dwOffset ) );
	}

void _SLVROOT::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_UINT( _SLVROOT, this, hFileRoot, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( _SLVROOT, this, dwInstanceID, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( _SLVROOT, this, pfapiBackingFile, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( _SLVROOT, this, pslvft, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( _SLVROOT, this, pfnSpaceReq, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( _SLVROOT, this, dwSpaceReqKey, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( _SLVROOT, this, semSpaceReq, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( _SLVROOT, this, semSpaceReqComp, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( _SLVROOT, this, msigTerm, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( _SLVROOT, this, msigTermAck, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( _SLVROOT, this, cbGrant, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( _SLVROOT, this, cbCommit, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( _SLVROOT, this, pfnSpaceFree, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( _SLVROOT, this, dwSpaceFreeKey, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( _SLVROOT, this, cbfgeainf, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( _SLVROOT, this, rgbEA, dwOffset ) );
	(*pcprintf)( FORMAT_WSZ( _SLVROOT, this, wszRootName, dwOffset ) );
	}

void CSLVBackingFile::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	COSFile::Dump( pcprintf, dwOffset );
	(*pcprintf)( FORMAT_POINTER( CSLVBackingFile, this, m_slvroot, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( CSLVBackingFile, this, m_szAbsPathSLV, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( CSLVBackingFile, this, m_semSetSize, dwOffset ) );
	}

#endif	//	DISABLE_SLV


void COSFileSystem::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_BOOL( COSFileSystem, this, m_fWin9x, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileSystem, this, m_hmodKernel32, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( COSFileSystem, this, m_pfnGetVolumePathName, dwOffset ) );
	}

void COSFileFind::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_POINTER( COSFileFind, this, m_posfs, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( COSFileFind, this, m_szFindPath, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( COSFileFind, this, m_szAbsFindPath, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( COSFileFind, this, m_fBeforeFirst, dwOffset ) );
	(*pcprintf)( FORMAT_INT( COSFileFind, this, m_errFirst, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileFind, this, m_hFileFind, dwOffset ) );
	(*pcprintf)( FORMAT_INT( COSFileFind, this, m_errCurrent, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( COSFileFind, this, m_fFolder, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( COSFileFind, this, m_szAbsFoundPath, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileFind, this, m_cbSize, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( COSFileFind, this, m_fReadOnly, dwOffset ) );
	}

void COSFile::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_SZ( COSFile, this, m_szAbsPath, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFile, this, m_hFile, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFile, this, m_cbFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( COSFile, this, m_fReadOnly, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFile, this, m_cbIOSize, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFile, this, m_cbMMSize, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( COSFile, this, m_pfnEndUpdate, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFile, this, m_keyEndUpdate, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( COSFile, this, m_pfnBeginUpdate, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFile, this, m_keyBeginUpdate, dwOffset ) );
	(*pcprintf)( FORMAT_POINTER( COSFile, this, m_p_osf, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( COSFile, this, m_msFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( COSFile, this, m_rgcbFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( COSFile, this, m_semChangeFileSize, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( COSFile, this, m_critDefer, dwOffset ) );
	(*pcprintf)( FORMAT_VOID( COSFile, this, m_ilDefer, dwOffset ) );
	}

void COSFileLayout::Dump( CPRINTF* pcprintf, DWORD_PTR dwOffset ) const
	{
	(*pcprintf)( FORMAT_POINTER( COSFileLayout, this, m_posf, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileLayout, this, m_ibVirtualFind, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileLayout, this, m_cbSizeFind, dwOffset ) );
	(*pcprintf)( FORMAT_BOOL( COSFileLayout, this, m_fBeforeFirst, dwOffset ) );
	(*pcprintf)( FORMAT_INT( COSFileLayout, this, m_errFirst, dwOffset ) );
	(*pcprintf)( FORMAT_INT( COSFileLayout, this, m_errCurrent, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( COSFileLayout, this, m_szAbsVirtualPath, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileLayout, this, m_ibVirtual, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileLayout, this, m_cbSize, dwOffset ) );
	(*pcprintf)( FORMAT_SZ( COSFileLayout, this, m_szAbsLogicalPath, dwOffset ) );
	(*pcprintf)( FORMAT_UINT( COSFileLayout, this, m_ibLogical, dwOffset ) );
	}

#endif	//	DEBUGGER_EXTENSION


extern "C" {

//	================================================================
VOID __stdcall ese(
	HANDLE hCurrentProcess,
	HANDLE hCurrentThread,
	DWORD dwCurrentPc,
	PWINDBG_EXTENSION_APIS lpExtensionApis,
	LPSTR lpArgumentString )
//	================================================================
	{
#ifdef DEBUGGER_EXTENSION

	TRY
		{
#ifdef DEBUG
		//  we don't want assertions appearing on the screen during debugging
		if( !fDebugMode )
			{
			g_wAssertAction = 3/* no action */;
			}
		else
			{
			g_wAssertAction = JET_AssertMsgBox;
			}
#endif

		if ( sizeof( WINDBG_EXTENSION_APIS ) != lpExtensionApis->nSize )
			{
			(lpExtensionApis->lpOutputRoutine)(
					"WARNING: EXTENSION_APIS has unexpected size: 0x%x bytes (expected 0x%x bytes)\n",
					lpExtensionApis->nSize,
					sizeof( WINDBG_EXTENSION_APIS ) );
			}

	    ExtensionApis 	= *lpExtensionApis;

		if ( !fInit && NULL != hCurrentProcess && cSymInitFail < cSymInitAttemptsMax )
			{
			//	UNDONE: Is the symbols sub-system even necessary anymore?
			//	A much cleaner approach is just to use the GLOBALS extension
			//	to load the global variables needed by all the other
			//	extensions
			//
			fInit = FSymInit( hCurrentProcess );
			if ( fInit )
				{
				dprintf( "Symbols sub-system successfully initialised.\n" );
				}
			else
				{
				dprintf( "WARNING: Failed initialisation of symbols sub-system.\n" );
				if ( ++cSymInitFail >= cSymInitAttemptsMax )
					dprintf( "WARNING: No further attempts will be made to initialise the symbols sub-system.\n" );
				}
			}

		VOID * pv = LocalAlloc( LMEM_ZEROINIT, strlen( lpArgumentString ) + 1 );
		if ( NULL == pv )
			{
			dprintf( "ERROR: Could not parse argument string (OutOfMemory).\n" );
			goto Return;
			}

		memcpy( pv, lpArgumentString, strlen( lpArgumentString ) );
		CHAR * argv[256];
		INT argc = SzToRgsz( argv, reinterpret_cast<CHAR *>( pv ) );

		if( argc < 1 )
			{
			EDBGHelp( hCurrentProcess, hCurrentThread, dwCurrentPc, lpExtensionApis, argc, (const CHAR **)argv );
			goto Return;
			}

		INT ifuncmap;
		for( ifuncmap = 0; ifuncmap < cfuncmap; ifuncmap++ )
			{
			if( FArgumentMatch( argv[0], rgfuncmap[ifuncmap].szCommand ) )
				{
				(rgfuncmap[ifuncmap].function)(
					hCurrentProcess,
					hCurrentThread,
					dwCurrentPc,
					lpExtensionApis,
					argc - 1, (const CHAR **)( argv + 1 ) );
				goto Return;
				}
			}

		EDBGHelp( hCurrentProcess, hCurrentThread, dwCurrentPc, lpExtensionApis, argc, (const CHAR **)argv );
		goto Return;

	Return:
		if ( NULL != pv )
			{
			LocalFree( pv );
			pv = NULL;
			}
		}
	EXCEPT( fDebugMode ? ExceptionFail( _T( "ESE Debugger Extension" ) ) : efaContinueSearch )
		{
		}
	ENDEXCEPT

#endif	//	DEBUGGER_EXTENSION
    }

//	================================================================
VOID __stdcall esent(
	DWORD dwCurrentPc,
	PWINDBG_EXTENSION_APIS lpExtensionApis,
	LPSTR lpArgumentString )
//	================================================================
	{
	//	UNDONE: this entry point is a hack to allow debugger
	//	extensions to work with KD - the true fix is to export
	//	the entry points necessary to comply with current
	//	WINDBG protocol (since we don't, the WINDBG engine
	//	invokes us with obsolete APIs)
	ese( NULL, NULL, dwCurrentPc, lpExtensionApis, lpArgumentString );
	}


#ifdef DEBUG

//	sample callback function
//
//  ================================================================
JET_ERR JET_API YouHaveBadSymbols(
	JET_SESID 		sesid,
	JET_DBID 		ifmp,
	JET_TABLEID 	tableid,
	JET_CBTYP 		cbtyp,
	void *			pvArg1,
	void *			pvArg2,
	void *			pvContext,
	ULONG_PTR		ulUnused )
//  ================================================================
	{
	//  this line should only compile if the signatures match
	JET_CALLBACK	callback = YouHaveBadSymbols;

	static BOOL fMessageBox = fTrue;

	const char * szCbtyp = "UNKNOWN";
	switch( cbtyp )
		{
		case JET_cbtypNull:
			szCbtyp = "NULL";
			break;
		case JET_cbtypBeforeInsert:
			szCbtyp = "BeforeInsert";
			break;
		case JET_cbtypAfterInsert:
			szCbtyp = "AfterInsert";
			break;
		case JET_cbtypBeforeReplace:
			szCbtyp = "BeforeReplace";
			break;
		case JET_cbtypAfterReplace:
			szCbtyp = "AfterReplace";
			break;
		case JET_cbtypBeforeDelete:
			szCbtyp = "BeforeDelete";
			break;
		case JET_cbtypAfterDelete:
			szCbtyp = "AfterDelete";
			break;
		}

	CHAR szMessage[384];
	sprintf( szMessage,
			"YouHaveBadSymbols:\n"
			"\tsesid   		= 0x%x\n"
			"\tifmp    		= 0x%x\n"
			"\ttableid 		= 0x%x\n"
			"\tcbtyp   		= 0x%x (%s)\n"
			"\tpvArg1  		= 0x%0*I64X\n"
			"\tpvArg2 		= 0x%0*I64X\n"
			"\tpvContext  	= 0x%0*I64X\n"
			"\tulUnused  	= 0x%x\n"
			"\n",
			sesid, ifmp, tableid, cbtyp, szCbtyp,
				sizeof( LONG_PTR )*2, pvArg1,
				sizeof( LONG_PTR )*2, pvArg2,
				sizeof( LONG_PTR )*2, pvContext,
				ulUnused );
	printf( "%s", szMessage );

	JET_ERR err;
	char szName[JET_cbNameMost+1];

	err = JetGetTableInfo( sesid, tableid, szName, sizeof( szName ), JET_TblInfoName );
	if( JET_errSuccess != err )
		{
		printf( "JetGetTableInfo returns %d\n", err );
		}
	else
		{
		char szBuf[JET_cbColumnMost + 16];
		sprintf( szBuf, "Table \"%s\"\n", szName );
		strcat( szMessage, szBuf );
		printf( "%s", szBuf );
		}

	err = JetGetCurrentIndex( sesid, tableid, szName, sizeof( szName ) );
	if( JET_errSuccess != err )
		{
		printf( "JetGetCurrentIndex returns %d\n", err );
		}
	else
		{
		char szBuf[JET_cbColumnMost + 16];
		sprintf( szBuf, "Index \"%s\"\n", szName );
		strcat( szMessage, szBuf );
		printf( "%s", szBuf );
		}

	err = JetMove( sesid, tableid, JET_MoveFirst, NO_GRBIT );
	Assert( JET_errSuccess != err );
	err = JetSetCurrentIndex( sesid, tableid, NULL );
	Assert( JET_errSuccess != err );
	JET_TABLEID tableid2;
	err = JetDupCursor( sesid, tableid, &tableid2, NO_GRBIT );
	Assert( JET_errSuccess == err );
	err = JetCloseTable( sesid, tableid );
	Assert( JET_errSuccess != err );
	err = JetCloseTable( sesid, tableid2 );
	Assert( JET_errSuccess == err );

	err = JET_errSuccess;

	switch( cbtyp )
		{
		case JET_cbtypBeforeInsert:
		case JET_cbtypBeforeReplace:
		case JET_cbtypBeforeDelete:
			if( fMessageBox )
				{
				strcat( szMessage,
						"\n"
						"Allow the callback to succeed?" );
				const int id = UtilMessageBox(
					szMessage,
					"YouHaveBadSymbols",
					MB_SERVICE_NOTIFICATION | MB_SYSTEMMODAL | MB_ICONQUESTION | MB_YESNOCANCEL );
				if( IDNO == id )
					{
					err = JET_errCallbackFailed;
					}
				else if( IDCANCEL == id )
					{
					fMessageBox = fFalse;
					}
				}
			break;
		}

	printf( "YouHaveBadSymbols returns %d.\n\n", err );
	return err;
	}

#endif	//	DEBUG

}	//	OSSYM


//  post-terminate edbg subsystem

void OSEdbgPostterm()
	{
	//  nop
	}

//  pre-init edbg subsystem

BOOL FOSEdbgPreinit()
	{
	//  nop

	return fTrue;
	}


//  terminate edbg subsystem

void OSEdbgTerm()
	{
	//  term OSSYM
#ifdef DEBUGGER_EXTENSION
	SymTerm();
#endif
	}

//  init edbg subsystem

ERR ErrOSEdbgInit()
	{
	//  nop

	return JET_errSuccess;
	}

