/*++

   Copyright    (c)    1997-2002    Microsoft Corporation

   Module  Name :
       irtldbg.h

   Abstract:
       Some simple debugging macros that look and behave a lot like their
       namesakes in MFC.  These macros should work in both C and C++ and do
       something useful with almost any Win32 compiler.

   Author:
       George V. Reilly      (GeorgeRe)     06-Jan-1998

   Environment:
       Win32 - User Mode

   Project:
       LKRhash

   Revision History:

--*/

#ifndef __IRTLDBG_H__
#define __IRTLDBG_H__

#ifndef __IRTLMISC_H__
# include <irtlmisc.h>
#endif

/* Ensure that MessageBoxes can popup */
# define IRTLDBG_RUNNING_AS_SERVICE 1

#include <tchar.h>


// Compile-time (not run-time) assertion. Code will not compile if
// expr is false. Note: there is no non-debug version of this; we
// want this for all builds. The compiler optimizes the code away.
template <bool> struct static_checker;
template <> struct static_checker<true> {};  // specialize only for `true'
#define STATIC_ASSERT(expr) static_checker< (expr) >()


# ifndef _AFX
  /* Assure compatiblity with MFC */

# if defined(NDEBUG)
#  undef IRTLDEBUG
# else
#  if DBG || defined(_DEBUG)
#   define IRTLDEBUG
#  endif
# endif

# ifdef IRTLDEBUG

#  if defined(IRTLDBG_KERNEL_MODE)

#   define IRTLASSERT(f) ASSERT(f)

#  elif !defined(USE_DEBUG_CRTS)

    /* IIS (and NT) do not ship msvcrtD.dll, per the VC license,
     * so we can't use the assertion code from <crtdbg.h>.  Use similar
     * macros from <pudebug.h> instead. */
#   include <pudebug.h>

    /* workaround for /W4 warnings about 'constant expressions' */
#   define IRTLASSERT(f)                                    \
    ((void) ((f) || (PuDbgAssertFailed(DBG_CONTEXT, #f), 0) ))

#  elif defined(_DEBUG)  &&  defined(_MSC_VER)  &&  (_MSC_VER >= 1000)

    /* Use the new debugging tools in Visual C++ 4.x */
#   include <crtdbg.h>

    /* _ASSERTE will give a more meaningful message, but the string takes
     * space.  Use _ASSERT if this is an issue. */
#   define IRTLASSERT(f) _ASSERTE(f)

#  else

#   include <assert.h>
#   define IRTLASSERT(f) assert(f)

#  endif /* different definitions of IRTLASSERT */

#  define IRTLVERIFY(f)               IRTLASSERT(f)
#  ifndef DEBUG_ONLY
#   define DEBUG_ONLY(f)              (f)
#  endif
#  define IRTLTRACE                   IrtlTrace
#  define IRTLTRACE0(psz)             IrtlTrace(_T("%s"), _T(psz))
#  define IRTLTRACE1(psz, p1)         IrtlTrace(_T(psz), p1)
#  define IRTLTRACE2(psz, p1, p2)     IrtlTrace(_T(psz), p1, p2)
#  define IRTLTRACE3(psz, p1, p2, p3) IrtlTrace(_T(psz), p1, p2, p3)
#  define IRTLTRACE4(psz, p1, p2, p3, p4) \
                                      IrtlTrace(_T(psz), p1, p2, p3, p4)
#  define IRTLTRACE5(psz, p1, p2, p3, p4, p5) \
                                      IrtlTrace(_T(psz), p1, p2, p3, p4, p5)
#  define ASSERT_VALID(pObj)  \
     do {IRTLASSERT((pObj) != NULL); (pObj)->AssertValid();} while (0)
#  define DUMP(pObj)  \
     do {IRTLASSERT((pObj) != NULL); (pObj)->Dump();} while (0)

# else /* !IRTLDEBUG */

  /* These macros should all compile away to nothing */
#  define IRTLASSERT(f)           ((void)0)
#  define IRTLVERIFY(f)           ((void)(f))
#  ifndef DEBUG_ONLY
#   define DEBUG_ONLY(f)          ((void)0)
#  endif
#  define IRTLTRACE               1 ? (void)0 : IrtlTrace
#  define IRTLTRACE0(psz)         1 ? (void)0 : IrtlTrace
#  define IRTLTRACE1(psz, p1)     1 ? (void)0 : IrtlTrace
#  define IRTLTRACE2(psz, p1, p2) 1 ? (void)0 : IrtlTrace
#  define IRTLTRACE3(psz, p1, p2, p3)         1 ? (void)0 : IrtlTrace
#  define IRTLTRACE4(psz, p1, p2, p3, p4)     1 ? (void)0 : IrtlTrace
#  define IRTLTRACE5(psz, p1, p2, p3, p4, p5) 1 ? (void)0 : IrtlTrace
#  define ASSERT_VALID(pObj)      ((void)0)
#  define DUMP(pObj)              ((void)0)

# endif /* !IRTLDEBUG */


# define ASSERT_POINTER(p, type) \
    IRTLASSERT(((p) != NULL)  &&  IsValidAddress((p), sizeof(type), FALSE))

# define ASSERT_NULL_OR_POINTER(p, type) \
    IRTLASSERT(((p) == NULL)  ||  IsValidAddress((p), sizeof(type), FALSE))

#define ASSERT_STRING(s) \
    IRTLASSERT(((s) != NULL)  &&  IsValidString((s), -1))

#define ASSERT_NULL_OR_STRING(s) \
    IRTLASSERT(((s) == NULL)  ||  IsValidString((s), -1))


/* Declarations for non-Windows apps */

# ifndef _WINDEF_
typedef void*           LPVOID;
typedef const void*     LPCVOID;
typedef unsigned int    UINT;
typedef int             BOOL;
typedef const char*     LPCTSTR;
# endif /* _WINDEF_ */

# ifndef TRUE
#  define FALSE  0
#  define TRUE   1
# endif


# ifdef __cplusplus
extern "C" {

/* Low-level sanity checks for memory blocks */
IRTL_DLLEXP BOOL IsValidAddress(LPCVOID pv, UINT nBytes, BOOL fReadWrite = TRUE);
IRTL_DLLEXP BOOL IsValidString(LPCTSTR ptsz, int nLength = -1);

}

# else /* !__cplusplus */

/* Low-level sanity checks for memory blocks */
IRTL_DLLEXP BOOL IsValidAddress(LPCVOID pv, UINT nBytes, BOOL fReadWrite);
IRTL_DLLEXP BOOL IsValidString(LPCTSTR ptsz, int nLength);

# endif /* !__cplusplus */

#else
# define IRTLASSERT(f) _ASSERTE(f)

#endif /* !_AFX */


/* Writes trace messages to debug stream */
#ifdef __cplusplus
extern "C" {
#endif /* !__cplusplus */

IRTL_DLLEXP
void __cdecl
IrtlTrace(
    LPCTSTR ptszFormat,
    ...);

IRTL_DLLEXP
DWORD
IrtlSetDebugOutput(
    DWORD dwFlags);

/* should be called from main(), WinMain(), or DllMain() */
IRTL_DLLEXP void
IrtlDebugInit();

IRTL_DLLEXP void
IrtlDebugTerm();

#ifdef __cplusplus
}; // extern "C"
#endif /* __cplusplus */


#ifdef IRTLDEBUG
# define IRTL_DEBUG_INIT()            IrtlDebugInit()
# define IRTL_DEBUG_TERM()            IrtlDebugTerm()
#else /* !IRTLDEBUG */
# define IRTL_DEBUG_INIT()            ((void)0)
# define IRTL_DEBUG_TERM()            ((void)0)
#endif /* !IRTLDEBUG */


#endif /* __IRTLDBG_H__ */
