/*==========================================================================
 *
 *  Copyright (C) 1999-2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       OSInd.h
 *  Content:	OS indirection functions to abstract OS specific items.
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	07/12/1999	jtk		Created
 *	10/16/2001	vanceo		Added AssertNoCriticalSectionsTakenByThisThread capability
 *
 ***************************************************************************/

#ifndef	__OSIND_H__
#define	__OSIND_H__

#include "CallStack.h"
#include "ClassBilink.h"
#include "HandleTracking.h"
#include "CritsecTracking.h"
#include "MemoryTracking.h"


//**********************************************************************
// Constant definitions
//**********************************************************************

#define GUID_STRING_LEN 39

//**********************************************************************
// Macro definitions
//**********************************************************************

#ifndef	OFFSETOF
#define OFFSETOF(s,m)				( ( INT_PTR ) ( ( PVOID ) &( ( (s*) 0 )->m ) ) )
#endif // OFFSETOF

//returns a pointer to a container structure given an internal member
//con_type is the container type, mem_name is the member name and mem_ptr is the
//pointer to the member

#ifndef CONTAINEROF
#define CONTAINEROF(con_type,mem_name,mem_ptr)	((con_type * ) (((char * ) mem_ptr)-\
						( ( int ) ( ( void * ) &( ( (con_type*) 0 )->mem_name ) ) )));
#endif // CONTAINEROF

#ifndef	LENGTHOF
#define	LENGTHOF( arg )				( sizeof( arg ) / sizeof( arg[ 0 ] ) )
#endif // OFFSETOF

#ifndef _MIN
#define _MIN(a, b) ((a) < (b) ? (a) : (b))
#endif // _MIN

#ifndef _MAX
#define _MAX(a, b) ((a) > (b) ? (a) : (b))
#endif // _MAX

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//
// initialization functions
//
BOOL	DNOSIndirectionInit( DWORD_PTR dwpMaxMemUsage );
void	DNOSIndirectionDeinit( void );

#ifndef DPNBUILD_NOPARAMVAL

extern BOOL IsValidStringA( const CHAR * const swzString );
#define DNVALID_STRING_A(a)		IsValidStringA(a)

extern BOOL IsValidStringW( const WCHAR * const szString );
#define DNVALID_STRING_W(a)		IsValidStringW(a)

#define DNVALID_WRITEPTR(a,b)	(!IsBadWritePtr(a,b))
#define DNVALID_READPTR(a,b)	(!IsBadReadPtr(a,b))

#endif // ! DPNBUILD_NOPARAMVAL

//
// Function to get OS version.  Supported returns:
//	VER_PLATFORM_WIN32_WINDOWS - Win9x
//	VER_PLATFORM_WIN32_NT - WinNT
//	VER_PLATFORM_WIN32s - Win32s on Win3.1
//	VER_PLATFORM_WIN32_CE - WinCE
//	
#if ((! defined(WINCE)) && (! defined(_XBOX)))
UINT_PTR	DNGetOSType( void );
#endif // ! WINCE and ! _XBOX

struct in_addr;
typedef struct in_addr IN_ADDR;
void DNinet_ntow( IN_ADDR sin, WCHAR* pwsz );

#ifdef WINNT
BOOL		DNOSIsXPOrGreater( void );
#endif // WINNT

#ifndef DPNBUILD_NOSERIALSP
// Used only by serial provider
HINSTANCE	DNGetApplicationInstance( void );
#endif // ! DPNBUILD_NOSERIALSP

#ifdef WINNT
PSECURITY_ATTRIBUTES DNGetNullDacl();
#else
#define DNGetNullDacl() 0
#endif // WINNT

#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE           3
#endif // VER_PLATFORM_WIN32_CE

#if ((defined(WINCE)) || (defined(_XBOX)))
#define	IsUnicodePlatform TRUE
#else // ! WINCE and ! _XBOX
#define	IsUnicodePlatform (DNGetOSType() == VER_PLATFORM_WIN32_NT || DNGetOSType() == VER_PLATFORM_WIN32_CE)
#endif // ! WINCE and ! _XBOX


#ifdef WINCE
#define GETTIMESTAMP() GetTickCount()
#else
#define GETTIMESTAMP() timeGetTime()
#endif // WINCE

	//return a quick to generate but not particularly random number
inline DWORD DNGetFastRandomNumber()
{
	return (rand() | (rand() << 16));
}

#ifndef _XBOX

extern HCRYPTPROV  g_hCryptProv;

	//fill out an arbitary length buffer with good quality random data
inline void DNGetGoodRandomData(void * pvData, DWORD dwNumBytes)
{
	CryptGenRandom(g_hCryptProv, dwNumBytes, (BYTE * ) pvData);
}

#else

	//fill out an arbitary length buffer with good quality random data
inline void DNGetGoodRandomData(void * pvData, DWORD dwNumBytes)
{
	XNetRandom((BYTE * ) pvData, dwNumBytes);
}

#endif //!_XBOX

	//return a highly random number (suitable for crypto and key protocols)
inline DWORD DNGetGoodRandomNumber()
{
	DWORD dwRetVal;
	DNGetGoodRandomData(&dwRetVal, sizeof(dwRetVal));
	return dwRetVal;
}		


//
// Interlocked functions (not actually interlocked when DPNBUILD_ONLYONETHREAD)
//
#ifdef DPNBUILD_ONLYONETHREAD
inline LONG DNInterlockedIncrement( IN OUT LONG volatile *Addend )
{
	return ++(*Addend);
}
inline LONG DNInterlockedDecrement( IN OUT LONG volatile *Addend )
{
	return --(*Addend);
}
inline LONG DNInterlockedExchange( IN OUT LONG volatile *Target, IN LONG Value )
{
	LONG	Previous;


	Previous = *Target;
	*Target = Value;
	return Previous;
}
inline LONG DNInterlockedExchangeAdd( IN OUT LONG volatile *Addend, IN LONG Value )
{
	LONG	Previous;


	Previous = *Addend;
	*Addend = Previous + Value;
	return Previous;
}
inline LONG DNInterlockedCompareExchange( IN OUT LONG volatile *Destination, IN LONG Exchange, IN LONG Comperand )
{
	LONG	Previous;


	Previous = *Destination;
	if (Previous == Comperand)
	{
		*Destination = Exchange;
	}
	return Previous;
}
inline PVOID DNInterlockedCompareExchangePointer( IN OUT PVOID volatile *Destination, IN PVOID Exchange, IN PVOID Comperand )
{
	PVOID	Previous;


	Previous = *Destination;
	if (Previous == Comperand)
	{
		*Destination = Exchange;
	}
	return Previous;
}
inline PVOID DNInterlockedExchangePointer( IN OUT PVOID volatile *Target, IN PVOID Value )
{
	PVOID	Previous;


	Previous = *Target;
	*Target = Value;
	return Previous;
}
#else // ! DPNBUILD_ONLYONETHREAD
/*
#ifdef WINCE
#if defined(_ARM_)
#define InterlockedExchangeAdd \
        ((long (*)(long *target, long increment))(PUserKData+0x3C0))
#elif defined(_X86_)
LONG WINAPI InterlockedExchangeAdd( LPLONG Addend, LONG Increment );
#else
#error("Unknown platform")
#endif // Platform
#endif // WINCE
*/
#define DNInterlockedIncrement( Addend )											InterlockedIncrement( Addend )
#define DNInterlockedDecrement( Addend )											InterlockedDecrement( Addend )
#define DNInterlockedExchange( Target, Value )										InterlockedExchange( Target, Value )
#define DNInterlockedExchangeAdd( Target, Value )									InterlockedExchangeAdd( Target, Value )
#ifdef WINCE
// NOTE: InterlockedTestExchange params 2 and 3 reversed intentionally, CE is that way
#define DNInterlockedCompareExchange( Destination, Exchange, Comperand )			InterlockedTestExchange( Destination, Comperand, Exchange )
#define DNInterlockedCompareExchangePointer( Destination, Exchange, Comperand )		(PVOID) (DNInterlockedCompareExchange( (LPLONG) Destination, (LONG) Exchange, (LONG) Comperand ))
#define DNInterlockedExchangePointer( Target, Value )								(PVOID) (DNInterlockedExchange( (LPLONG) (Target), (LPLONG) (Value) ))
#else // ! WINCE
#define DNInterlockedCompareExchange( Destination, Exchange, Comperand )			InterlockedCompareExchange( Destination, Exchange, Comperand )
#define DNInterlockedCompareExchangePointer( Destination, Exchange, Comperand )		InterlockedCompareExchangePointer( Destination, Exchange, Comperand )
#define DNInterlockedExchangePointer( Target, Value )								InterlockedExchangePointer( Target, Value )
#endif // WINCE
#endif // ! DPNBUILD_ONLYONETHREAD

// Special initialize to set spin count, avoid out-of-memory exceptions at Enter/Leave
BOOL DNOSInitializeCriticalSection( CRITICAL_SECTION* pCriticalSection );

#ifdef WINNT
#define GLOBALIZE_STR _T("Global\\")
#else
#define GLOBALIZE_STR _T("")
#endif // WINNT

#if defined(WINCE) && !defined(WINCE_ON_DESKTOP)
#define _TWINCE(x) __T(x)
#else
#define _TWINCE(x) x
#endif // WINCE

//
// Memory functions
//

#ifdef DPNBUILD_LIBINTERFACE

#define new		__wont_compile_dont_use_new_operator__
#define delete	__wont_compile_dont_use_delete_operator__

#else // ! DPNBUILD_LIBINTERFACE

//**********************************************************************
// ------------------------------
// operator new - allocate memory for a C++ class
//
// Entry:		Size of memory to allocate
//
// Exit:		Pointer to memory
//				NULL = no memory available
//
// Notes:	This function is for classes only and will ASSERT on zero sized
//			allocations!  This function also doesn't do the whole proper class
//			thing of checking for replacement 'new handlers' and will not throw
//			an exception if allocation fails.
// ------------------------------
inline	void*	__cdecl operator new( size_t size )
{
	return DNMalloc( size );
}
//**********************************************************************


//**********************************************************************
// ------------------------------
// operator delete - deallocate memory for a C++ class
//
// Entry:		Pointer to memory
//
// Exit:		Nothing
//
// Notes:	This function is for classes only and will ASSERT on NULL frees!
// ------------------------------
inline	void	__cdecl operator delete( void *pData )
{
	//
	// Voice and lobby currently try allocating 0 byte buffers, can't disable this check yet.
	//
	if( pData == NULL )
		return;
	
	DNFree( pData );
}
//**********************************************************************

#endif // ! DPNBUILD_LIBINTERFACE


#ifdef WINCE
#ifdef DBG
UINT DNGetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
#endif // DBG

#ifndef WINCE_ON_DESKTOP
HANDLE WINAPI OpenEvent(IN DWORD dwDesiredAccess, IN BOOL bInheritHandle, IN LPCWSTR lpName);
HANDLE WINAPI OpenFileMapping(IN DWORD dwDesiredAccess, IN BOOL bInheritHandle, IN LPCWSTR lpName);
HANDLE WINAPI OpenMutex(IN DWORD dwDesiredAccess, IN BOOL bInheritHandle, IN LPCWSTR lpName);
#endif // !WINCE_ON_DESKTOP

#define WaitForSingleObjectEx(handle, time, fAlertable) WaitForSingleObject(handle, time)
#define WaitForMultipleObjectsEx(count, handles, waitall, time, fAlertable) WaitForMultipleObjects(count, handles, waitall, time)
#ifndef WINCE_ON_DESKTOP
#define GetWindowLongPtr(a, b) GetWindowLong(a, b)
#define GWLP_USERDATA GWL_USERDATA
#define SetWindowLongPtr(a, b, c) SetWindowLong(a, b, c)
#endif // WINCE_ON_DESKTOP
#define SleepEx(a, b) Sleep(a)

#ifndef MUTEX_ALL_ACCESS
#define MUTEX_ALL_ACCESS 0
#endif // MUTEX_ALL_ACCESS
#ifndef NORMAL_PRIORITY_CLASS
#define NORMAL_PRIORITY_CLASS 0
#endif // NORMAL_PRIORITY_CLASS

#else // ! WINCE
#ifdef DBG
#if ((defined(_XBOX)) && (! defined(XBOX_ON_DESKTOP)))
UINT DNGetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault);
#else // ! _XBOX or XBOX_ON_DESKTOP
#define DNGetProfileInt(lpszSection, lpszEntry, nDefault)	GetProfileInt(lpszSection, lpszEntry, nDefault)
#endif// ! _XBOX or XBOX_ON_DESKTOP
#endif // DBG
#endif // ! WINCE

#if ((defined(WINCE)) || (defined(DPNBUILD_LIBINTERFACE)))
HRESULT DNCoCreateGuid(GUID* pguid);
#else // ! WINCE and ! DPNBUILD_LIBINTERFACE
#define DNCoCreateGuid CoCreateGuid
#endif // ! WINCE and ! DPNBUILD_LIBINTERFACE


#ifdef _XBOX

#define swprintf	wsprintfW

#else // ! _XBOX

#ifdef WINCE
static inline FARPROC DNGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
	{	return GetProcAddressA(hModule, lpProcName);	};
#else
static inline FARPROC DNGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
	{	return GetProcAddress(hModule, lpProcName);	};
#endif

#endif // _XBOX

#endif	// __OSIND_H__
