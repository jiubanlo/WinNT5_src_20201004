#include "priv.h"
#include "schedule.h"

// debug stuff for tracking critical section owners.....
#ifdef DEBUG
#define DECLARE_CRITICAL_SECTION(x)         CRITICAL_SECTION x;                              \
                                            DWORD dwThread##x;

#define STATIC_DECLARE_CRITICAL_SECTION(x)  static CRITICAL_SECTION x;                      \
                                            static DWORD dwThread##x;

#define STATIC_INIT_CRITICAL_SECTION(c,x)   CRITICAL_SECTION c::x = {0};             \
                                            DWORD c::dwThread##x;

#define ASSERT_CRITICAL_SECTION(x)          ASSERT( dwThread##x == GetCurrentThreadId() );

#define OBJECT_ASSERT_CRITICAL_SECTION(o,x) ASSERT( o->dwThread##x == GetCurrentThreadId() );

#define ENTER_CRITICAL_SECTION(x)           EnterCriticalSection(&x);                        \
                                            dwThread##x = GetCurrentThreadId();

#define OBJECT_ENTER_CRITICAL_SECTION(o,x)  EnterCriticalSection(&o->x);                        \
                                            o->dwThread##x = GetCurrentThreadId();

#define LEAVE_CRITICAL_SECTION(x)           ASSERT_CRITICAL_SECTION(x);    \
                                            LeaveCriticalSection(&x);

#define OBJECT_LEAVE_CRITICAL_SECTION(o,x)  OBJECT_ASSERT_CRITICAL_SECTION(o,x);    \
                                            LeaveCriticalSection(&o->x);
#else
#define DECLARE_CRITICAL_SECTION(x)         CRITICAL_SECTION x;

#define STATIC_DECLARE_CRITICAL_SECTION(x)  static CRITICAL_SECTION x;

#define STATIC_INIT_CRITICAL_SECTION(c,x)   CRITICAL_SECTION c::x = {0};

#define ASSERT_CRITICAL_SECTION(x)

#define OBJECT_ASSERT_CRITICAL_SECTION(o,x)

#define ENTER_CRITICAL_SECTION(x)           EnterCriticalSection(&x);

#define OBJECT_ENTER_CRITICAL_SECTION(o,x)  EnterCriticalSection(&o->x);

#define LEAVE_CRITICAL_SECTION(x)           LeaveCriticalSection(&x);

#define OBJECT_LEAVE_CRITICAL_SECTION(o,x)  LeaveCriticalSection(&o->x);
#endif

#define TF_SCHEDULER     0x20

// struct to hold the details for each task that is to be executed....
struct TaskNode
{
    LPRUNNABLETASK pTask;
    TASKOWNERID toid;
    DWORD dwPriority;
    DWORD_PTR dwLParam;
    BOOL fSuspended;
};


class CShellTaskScheduler : public IShellTaskScheduler2
{
    public:
        CShellTaskScheduler( HRESULT * pHr );
        ~CShellTaskScheduler();

        STDMETHOD (QueryInterface) (REFIID riid, LPVOID * ppvObj );
        STDMETHOD_(ULONG, AddRef)( void );
        STDMETHOD_(ULONG,Release)( void );

        STDMETHOD (AddTask)(IRunnableTask * pTask,
                   REFTASKOWNERID rtoid,
                   DWORD_PTR lParam,
                   DWORD dwPriority );
        STDMETHOD (RemoveTasks)( REFTASKOWNERID rtoid,
                   DWORD_PTR dwLParam,
                   BOOL fWaitIfRunning );
        STDMETHOD (Status)( DWORD dwStatus, DWORD dwThreadTimeout );
        STDMETHOD_(UINT, CountTasks)(REFTASKOWNERID rtoid);

        STDMETHOD (AddTask2)(IRunnableTask * pTask,
                   REFTASKOWNERID rtoid,
                   DWORD_PTR lParam,
                   DWORD dwPriority,
                   DWORD grfFlags);
        STDMETHOD (MoveTask)(REFTASKOWNERID rtoid,
                   DWORD_PTR dwLParam,
                   DWORD dwPriority,
                   DWORD grfFlags );

    protected:

        // data held by a task scheduler to refer to the current worker that it has....
        struct WorkerData
        {
            BOOL Init(CShellTaskScheduler *pts);

            // this (pThis) is used to pass the controlling
            // object back and forth to the thread, so that threads can be moved
            // back and forth from objects as they need them.
            CShellTaskScheduler *   pThis;

#ifdef DEBUG
            DWORD                   dwThreadID;
#endif
        };

        friend UINT CShellTaskScheduler_ThreadProc( LPVOID pParam );
        friend int CALLBACK ListDestroyCallback( LPVOID p, LPVOID pData );

        VOID _KillScheduler( BOOL bKillCurTask );
        BOOL _WakeScheduler( void );

        BOOL _RemoveTasksFromList( REFTASKOWNERID rtoid, DWORD_PTR dwLParam );


        // create a worker thread data block that can be associated with a task scheduler....
        WorkerData * FetchWorker( void );

        // from a worker thread, let go of the scheduler it is associated...
        static BOOL ReleaseWorker( WorkerData * pThread );


        /***********PERINSTANCE DATA ************/
        DECLARE_CRITICAL_SECTION( m_csListLock )
        BOOL m_bListLockInited;
        HDPA m_hTaskList;

        WorkerData * m_pWorkerThread;

        // the currently running task...
        TaskNode * m_pRunning;

        // a semaphore that counts, so that all waiters canbe released...
        HANDLE m_hCurTaskEnded;

        DWORD m_dwStatus;

        int m_iSignalCurTask;                // - tell the thread to signal when the
                                             //   current task is finished if non-zero
                                             //   the other thread will signal the 
                                             //   handle as many times as this variable
                                             //   holds.
        BOOL m_fEmptyQueueAndSleep;          // - tell the thread to empty itself and
                                             //   go to sleep (usually it is dying....

        int m_iGoToSleep;                    // - tell the tread to go to sleep without emptying the queue

        long m_cRef;

#ifdef DEBUG
        void AssertForNoOneWaiting( void )
        {
            // no one should be queued for waiting
            ASSERT( m_iSignalCurTask == 0 );

            // release the semaphore by zero to get the current count....
            LONG lPrevCount = 0;
            ReleaseSemaphore( m_hCurTaskEnded, 0, &lPrevCount );
            ASSERT( lPrevCount == 0 );
        };
#endif
        
        void IWantToKnowWhenCurTaskDone( void )
        {
            m_iSignalCurTask ++;
        };
};

// private messages sent to the scheduler thread...
#define WM_SCH_WAKEUP       WM_USER + 0x600
#define WM_SCH_TERMINATE    WM_USER + 0x601

STDAPI CShellTaskScheduler_CreateInstance(IUnknown* pUnkOuter, IUnknown** ppunk, LPCOBJECTINFO poi)
{
    if ( pUnkOuter )
    {
        return CLASS_E_NOAGGREGATION;
    }

    HRESULT hr = NOERROR;
    CShellTaskScheduler * pScheduler = new CShellTaskScheduler( & hr );
    if ( !pScheduler )
    {
        return E_OUTOFMEMORY;
    }
    if ( FAILED( hr ))
    {
        delete pScheduler;
        return hr;
    }

    *ppunk = SAFECAST(pScheduler, IShellTaskScheduler *);
    return NOERROR;
}

// Global ExplorerTaskScheduler object that is used by multiple components.
IShellTaskScheduler * g_pTaskScheduler = NULL;


// This is the class factory routine for creating the one and only ExplorerTaskScheduler object.
// We have a static object (g_pTaskScheduler) that everyone who wants to use it shares.
STDAPI CSharedTaskScheduler_CreateInstance(IUnknown* pUnkOuter, IUnknown** ppunk, LPCOBJECTINFO poi)
{
    HRESULT hr = NOERROR;

    if (pUnkOuter)
        return CLASS_E_NOAGGREGATION;

    ENTERCRITICAL;
    if (g_pTaskScheduler)
    {
        g_pTaskScheduler->AddRef();
    }
    else
    {
        hr = CShellTaskScheduler_CreateInstance(NULL, (LPUNKNOWN*)&g_pTaskScheduler, NULL);
        if (SUCCEEDED(hr))
        {
            // set timeout to be 1 minute.....
            g_pTaskScheduler->Status( ITSSFLAG_KILL_ON_DESTROY, 1 * 60 * 1000 );

            // keep an additional ref for us..
            g_pTaskScheduler->AddRef();
        }
    }

    *ppunk = SAFECAST(g_pTaskScheduler, IShellTaskScheduler*);
    LEAVECRITICAL;

    return hr;
}

STDAPI SHIsThereASystemScheduler( void )
{
    return ( g_pTaskScheduler ? S_OK : S_FALSE );
}

// use CoCreateInstance - thread pool removes need for global scheduler
STDAPI SHGetSystemScheduler( LPSHELLTASKSCHEDULER * ppScheduler )
{
    if ( !ppScheduler )
    {
        return E_INVALIDARG;
    }

    return CSharedTaskScheduler_CreateInstance(NULL, (IUnknown **)ppScheduler, NULL );
}

// use CoCreateInstance - thread pool removes need for global scheduler
STDAPI SHFreeSystemScheduler( void )
{
    TraceMsg(TF_SCHEDULER, "SHfss: g_pTaskSched=%x", g_pTaskScheduler);

    IShellTaskScheduler * pSched;

    ENTERCRITICAL;
    pSched = g_pTaskScheduler;
    g_pTaskScheduler = NULL;
    LEAVECRITICAL;
    if ( pSched )
    {
        // assume the scheduler is empty....
        pSched->RemoveTasks( TOID_NULL, ITSAT_DEFAULT_LPARAM, FALSE );

        pSched->Release();
    }
    return NOERROR;
}

#ifdef DEBUG
STDAPI_(void) SHValidateEmptySystemScheduler()
{
    if ( g_pTaskScheduler )
    {
        ASSERT( g_pTaskScheduler->CountTasks( TOID_NULL ) == 0 );
    }
}
#endif

int InsertInPriorityOrder( HDPA hTaskList, TaskNode * pNewNode, BOOL fBefore );
int CALLBACK ListDestroyCallback( LPVOID p, LPVOID pData )
{
    ASSERT( p != NULL );
    if ( ! p )
    {
        TraceMsg( TF_ERROR, "ListDestroyCallback() - p is NULL!" );
        return TRUE;
    }

    CShellTaskScheduler * pThis = (CShellTaskScheduler *) pData;
    ASSERT( pThis );
    if ( ! pThis )
    {
        TraceMsg( TF_ERROR, "ListDestroyCallback() - pThis is NULL!" );
        return TRUE;
    }

    TaskNode * pNode = (TaskNode *) p;
    ASSERT( pNode != NULL );
    ASSERT( pNode->pTask != NULL );

#ifdef DEBUG
    if ( pThis->m_pWorkerThread )
    {
        // notify the thread that we are emptying the list from here, so remove these
        // items from its mem track list
    }
#endif

    // if it is suspended, kill it. If it is not suspended, then it has
    // probably never been started..
    if ( pNode->fSuspended )
    {
        pNode->pTask->Kill( pThis->m_dwStatus == ITSSFLAG_COMPLETE_ON_DESTROY );
    }
    pNode->pTask->Release();
    delete pNode;

    return TRUE;
}

STDMETHODIMP CShellTaskScheduler::QueryInterface( REFIID riid, LPVOID * ppvObj )
{
    static const QITAB qit[] = {
        QITABENT(CShellTaskScheduler, IShellTaskScheduler),
        QITABENT(CShellTaskScheduler, IShellTaskScheduler2),
        { 0 },
    };

    return QISearch(this, qit, riid, ppvObj);
}

STDMETHODIMP_ (ULONG) CShellTaskScheduler::AddRef()
{
    return InterlockedIncrement( &m_cRef );
}
STDMETHODIMP_ (ULONG) CShellTaskScheduler::Release()
{
    ASSERT( 0 != m_cRef );
    ULONG cRef = InterlockedDecrement( &m_cRef );
    if ( 0 == cRef)
    {
        delete this;
    }
    return cRef;
}

CShellTaskScheduler::CShellTaskScheduler( HRESULT * pHr) : m_cRef(1)
{
    *pHr = S_OK;
    
    ASSERT(!m_bListLockInited);
    __try
    {
        InitializeCriticalSection(&m_csListLock);
        m_bListLockInited = TRUE;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        *pHr = E_OUTOFMEMORY;
    }

    ASSERT(m_pWorkerThread == NULL);
    ASSERT(m_pRunning == NULL);

    m_dwStatus = ITSSFLAG_COMPLETE_ON_DESTROY;

    // grow queue by five each time...
    m_hTaskList = DPA_Create( 5 );
    if ( !m_hTaskList )
    {
        *pHr = E_OUTOFMEMORY;
    }

    m_hCurTaskEnded = CreateSemaphoreWrap( NULL, 0, 0xffff, NULL );
    if ( !m_hCurTaskEnded )
    {
        *pHr = E_FAIL;
    }

    DllAddRef();

}

CShellTaskScheduler::~CShellTaskScheduler()
{
    // if we don't have a tasklist and semaphore (constructor failure), we can't have a workerthread
    ASSERT((m_hTaskList && m_hCurTaskEnded) || !m_pWorkerThread);

    // but if we have a task list...
    if ( m_hTaskList )
    {
        EnterCriticalSection( &m_csListLock );

        // if we have a background worker thread, then it MUST be doing something as we
        // are now in the crit section so it can't go away
        if ( m_pWorkerThread )
        {
            // we tell the object we need to know when it has done with its stuff....
            // we reuse the event we already have...
            m_fEmptyQueueAndSleep = TRUE;

#ifdef DEBUG
            AssertForNoOneWaiting();
#endif

            IWantToKnowWhenCurTaskDone();
            
            // tell the cur task to go away.....
            TraceMsg(TF_SCHEDULER, "(%x)csts.dtor: call _KillScheduler", GetCurrentThreadId());
            _KillScheduler( m_dwStatus == ITSSFLAG_KILL_ON_DESTROY );

            // free the thread. At this point there is always
            LeaveCriticalSection( &m_csListLock );

            TraceMsg(TF_SCHEDULER, "csts.dtor: call u.WFSMT(m_hCurTaskEnded=%x)", m_hCurTaskEnded);

            DWORD dwRes = SHWaitForSendMessageThread(m_hCurTaskEnded, INFINITE);
            ASSERT(dwRes == WAIT_OBJECT_0);
            TraceMsg(TF_SCHEDULER, "csts.dtor: u.WFSMT() done");

            ASSERT( !m_pWorkerThread );
        }
        else
        {
            LeaveCriticalSection( &m_csListLock );
        }

        // empty the list incase it is not empty (it should be)
        DPA_EnumCallback( m_hTaskList, ListDestroyCallback, this );
        DPA_DeleteAllPtrs( m_hTaskList );

        DPA_Destroy( m_hTaskList );
        m_hTaskList = NULL;
    }

    if ( m_hCurTaskEnded )
        CloseHandle( m_hCurTaskEnded );

    if (m_bListLockInited)
        DeleteCriticalSection( &m_csListLock );

    DllRelease();

}

STDMETHODIMP CShellTaskScheduler::AddTask( IRunnableTask * pTask,
                                      REFTASKOWNERID rtoid,
                                      DWORD_PTR dwLParam,
                                      DWORD dwPriority )
{
    return AddTask2(pTask, rtoid, dwLParam, dwPriority, ITSSFLAG_TASK_PLACEINBACK);
}

STDMETHODIMP CShellTaskScheduler::AddTask2( IRunnableTask * pTask,
                                      REFTASKOWNERID rtoid,
                                      DWORD_PTR dwLParam,
                                      DWORD dwPriority,
                                      DWORD grfFlags )
{                                      
    if ( !pTask )
        return E_INVALIDARG;

    HRESULT hr = E_OUTOFMEMORY;   // assume failure

    TaskNode * pNewNode = new TaskNode;
    if ( pNewNode )
    {
        pNewNode->pTask = pTask;
        pTask->AddRef();
        pNewNode->toid = rtoid;
        pNewNode->dwPriority = dwPriority;
        pNewNode->dwLParam = dwLParam;
        pNewNode->fSuspended = FALSE;

        EnterCriticalSection( &m_csListLock );

        int iPos = -1;

        if (grfFlags & ITSSFLAG_TASK_PLACEINFRONT)
        {
            iPos = InsertInPriorityOrder( m_hTaskList, pNewNode, TRUE );
        }
        else if (grfFlags & ITSSFLAG_TASK_PLACEINBACK)
        {
            iPos = InsertInPriorityOrder( m_hTaskList, pNewNode, FALSE );
        }

        if ( iPos != -1 && m_pRunning )
        {
            if ( m_pRunning->dwPriority < dwPriority )
            {
                // try to suspend the current task. If this works, the task will
                // return to the scheduler with E_PENDING. It will then be added
                // suspended in the queue to be Resumed later....
                m_pRunning->pTask->Suspend();
            }
        }

        BOOL bRes = FALSE;

        if ( iPos != -1 )
        {
            // get a worker thread and awaken it...
            // we do this in the crit section because we need to test m_pWorkerThread and
            // to save us from releasing and grabbing it again...
            bRes = _WakeScheduler();

#ifdef DEBUG
            if ( bRes && m_pWorkerThread )
            {
                //
                // We are putting this memory block in a linked list and it will most likely be freed
                // from the background thread. Remove it from the per-thread memory list to avoid
                // detecting it as a memory leak.
                //
                // WARNING - WARNING - WARNING:
                // We cannot...
                // assume that when pTask is Released it will be deleted, so move it
                // to the other thread's memory list.
                // 
                // This will be incorrect some of the time and we don't want to investigate
                // fake leaks. -BryanSt
                //transfer_to_thread_memlist( m_pWorkerThread->dwThreadID, pNewNode->pTask );
            }
#endif
        }
        LeaveCriticalSection( &m_csListLock );

        // we failed to add it to the list
        if ( iPos == -1 )
        {
            // we failed to add it to the list, must have been a memory failure...
            pTask->Release();       // for the AddRef above
            delete pNewNode;
            goto Leave;
        }

        hr = bRes ? NOERROR : E_FAIL;
    }
Leave:
    return hr;
}

STDMETHODIMP CShellTaskScheduler::RemoveTasks( REFTASKOWNERID rtoid,
                                               DWORD_PTR dwLParam,
                                               BOOL fWaitIfRunning )
{
    BOOL fRemoveAll = IsEqualGUID( TOID_NULL, rtoid );
    BOOL fAllItems = (dwLParam == ITSAT_DEFAULT_LPARAM );
    BOOL fWaitOnHandle = FALSE;

    // note, this ignores the current
    EnterCriticalSection( &m_csListLock );

    _RemoveTasksFromList( rtoid, dwLParam );

    if ( m_pRunning && ( fWaitIfRunning || m_dwStatus == ITSSFLAG_KILL_ON_DESTROY ))
    {
        // kill the current task ...
        if (( fRemoveAll || IsEqualGUID( rtoid, m_pRunning->toid )) &&
            ( fAllItems || dwLParam == m_pRunning->dwLParam ))
        {
            ASSERT( m_pRunning->pTask );
            if ( m_dwStatus == ITSSFLAG_KILL_ON_DESTROY )
            {
                m_pRunning->pTask->Kill( fWaitIfRunning );
            }

            // definitive support for waiting until they are done...
            // (note, only do it is there is a task running, otherwise we'll sit
            // on a handle that will never fire)
            if ( fWaitIfRunning )
            {
                IWantToKnowWhenCurTaskDone();

                // don't use this directly outside of the cirtical section because it can change...
                ASSERT ( m_iSignalCurTask );

                fWaitOnHandle = TRUE;
                m_iGoToSleep++;
            }
        }
    }

    LeaveCriticalSection( &m_csListLock );

    // now wait if we need to......
    if ( fWaitOnHandle )
    {
        DWORD dwRes = SHWaitForSendMessageThread(m_hCurTaskEnded, INFINITE);
        ASSERT(dwRes == WAIT_OBJECT_0);

        EnterCriticalSection( &m_csListLock );

        // Remove tasks that might have been added while the last task was finishing
        _RemoveTasksFromList( rtoid, dwLParam );

        m_iGoToSleep--;
        // See if we need to wake the thread now.
        if ( m_iGoToSleep == 0 && DPA_GetPtrCount( m_hTaskList ) > 0 )
            _WakeScheduler();

        LeaveCriticalSection( &m_csListLock );
    }

    return NOERROR;
}

BOOL CShellTaskScheduler::_RemoveTasksFromList( REFTASKOWNERID rtoid, DWORD_PTR dwLParam )
{
    // assumes that we are already holding the critical section
    
    BOOL fRemoveAll = IsEqualGUID( TOID_NULL, rtoid );
    BOOL fAllItems = (dwLParam == ITSAT_DEFAULT_LPARAM );
    int iIndex = 0;

    do
    {
        TaskNode * pNode = (TaskNode *) DPA_GetPtr( m_hTaskList, iIndex );
        if ( !pNode )
        {
            break;
        }

        ASSERT( pNode );
        ASSERT( pNode->pTask );

        if (( fRemoveAll || IsEqualGUID( pNode->toid, rtoid )) && ( fAllItems || dwLParam == pNode->dwLParam ))
        {
            // remove it
            DPA_DeletePtr( m_hTaskList, iIndex );

            if ( pNode->fSuspended )
            {
                // kill it just incase....
                pNode->pTask->Kill( FALSE );
            }
            pNode->pTask->Release();
            delete pNode;
        }
        else
        {
            iIndex ++;
        }
    }
    while ( TRUE );
    return TRUE;
}

//
// CShellTaskScheduler::MoveTask
//
STDMETHODIMP CShellTaskScheduler::MoveTask( REFTASKOWNERID rtoid,
                                            DWORD_PTR dwLParam,
                                            DWORD dwPriority,
                                            DWORD grfFlags )
{
    int  iInsert;
    int  iIndex;
    BOOL fMoveAll  = IsEqualGUID( TOID_NULL, rtoid );
    BOOL fAllItems = (dwLParam == ITSAT_DEFAULT_LPARAM );
    BOOL bMatch    = FALSE ;
    int  iIndexStart;
    int  iIndexInc;

    EnterCriticalSection( &m_csListLock );

    // Init direction of search
    if (grfFlags & ITSSFLAG_TASK_PLACEINFRONT)
    {
        iIndexStart = 0;
        iInsert = DPA_GetPtrCount( m_hTaskList );
        iIndexInc = 1;
    }
    else if (grfFlags & ITSSFLAG_TASK_PLACEINBACK)
    {
        iIndexStart = iInsert = DPA_GetPtrCount( m_hTaskList );
        iIndexInc = -1;
    }

    // Find insert point (based on priority)
    iIndex = 0;
    do
    {
        TaskNode * pNode = (TaskNode *) DPA_GetPtr( m_hTaskList, iIndex );
        if ( !pNode )
        {
            break;
        }

        if (grfFlags & ITSSFLAG_TASK_PLACEINFRONT)
        {
            if (pNode->dwPriority <= dwPriority)
            {
                iInsert = iIndex;
                break;
            }
        }
        else if (grfFlags & ITSSFLAG_TASK_PLACEINBACK)
        {
            if (pNode->dwPriority > dwPriority)
            {
                iInsert = iIndex;
            }
            else
            {
                break;
            }
        }

        iIndex++;
    }
    while (TRUE);

    // Now try and locate any items.
    iIndex = iIndexStart;
    do
    {
        TaskNode * pNode = (TaskNode *) DPA_GetPtr( m_hTaskList, iIndex );
        if ( !pNode )
        {
            break;
        }

        if (( fMoveAll || IsEqualGUID( pNode->toid, rtoid )) && 
            ( fAllItems || dwLParam == pNode->dwLParam ))
        {
            bMatch = TRUE;

            // Can we move this node?
            if ( iIndex != iInsert )
            {
                int iPos = DPA_InsertPtr( m_hTaskList, iInsert, pNode );
                if (iPos != -1)
                {
                    if ( iIndex > iInsert )
                    {
                        DPA_DeletePtr( m_hTaskList, iIndex + 1);  // Will have shifted one
                    }
                    else
                    {
                        DPA_DeletePtr( m_hTaskList, iIndex);
                    }
                }
            }
        }
        iIndex += iIndexInc;
    }
    while ( !bMatch );
    
    LeaveCriticalSection( &m_csListLock );

    return (bMatch ? S_OK : S_FALSE);
}

BOOL CShellTaskScheduler::_WakeScheduler( )
{
    // assume we are in the object's critsection.....

    if ( NULL == m_pWorkerThread )
    {
        // we need a worker quick ....
        m_pWorkerThread = FetchWorker();
    }

    return ( NULL != m_pWorkerThread );
}

VOID CShellTaskScheduler::_KillScheduler( BOOL bKillCurTask )
{
    // assumes that we are already holding the critical section
    if ( m_pRunning != NULL && bKillCurTask )
    {
        ASSERT( m_pRunning->pTask );

        // tell the currently running task that it should die
        // quickly, because we are a separate thread than the
        // one that is running the task, it can be notified
        m_pRunning->pTask->Kill( FALSE );
    }
}

UINT CShellTaskScheduler_ThreadProc( LPVOID pParam )
{
    // make sure we have a message QUEUE // BOGUS - why do we need this?
    MSG msg;
    PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );

    ASSERT( pParam );

    HRESULT hrInit = SHCoInitialize();

    CShellTaskScheduler::WorkerData * pWorker = (CShellTaskScheduler::WorkerData *) pParam;
    DWORD dwRes = 0;

    TraceMsg(TF_SCHEDULER, "(?%x)ShellTaskScheduler::Thread Started", GetCurrentThreadId());

#ifdef DEBUG
    pWorker->dwThreadID = GetCurrentThreadId();
#endif

    // figure out who we are attatched to (where the queue is we get tasks from)
    CShellTaskScheduler * pThis = pWorker->pThis;

    // we must always have a valid parent object at this point....
    ASSERT( pThis && IS_VALID_WRITE_PTR( pThis, CShellTaskScheduler ));

    do
    {
        MSG msg;
        HRESULT hr = NOERROR;
        TaskNode * pTask = NULL;

        OBJECT_ENTER_CRITICAL_SECTION( pThis, m_csListLock );

        // this means we are being told to quit...
        if ( pThis->m_fEmptyQueueAndSleep )
        {
            // we are being told to empty the queue .....
            DPA_EnumCallback( pThis->m_hTaskList, ListDestroyCallback, pThis );
            DPA_DeleteAllPtrs( pThis->m_hTaskList );
        }
        else if ( !pThis->m_iGoToSleep )
        {
            // get the first item...
            pTask = (TaskNode *) DPA_GetPtr( pThis->m_hTaskList, 0 );
        }

        if ( pTask )
        {
            // remove from the list...
            DPA_DeletePtr( pThis->m_hTaskList, 0 );
        }
        pThis->m_pRunning = pTask;

        OBJECT_LEAVE_CRITICAL_SECTION( pThis, m_csListLock );

        if ( pTask == NULL )
        {
            // cache the scheduler pointer, as we need it to leave the crit section
            CShellTaskScheduler * pScheduler = pThis;

            // queue is empty, go back on the thread pool.....
            // we are about to enter a deep deep sleep/coma, so remove us from the object....
            OBJECT_ENTER_CRITICAL_SECTION( pScheduler, m_csListLock );

            HANDLE hSleep = pThis->m_fEmptyQueueAndSleep ? pThis->m_hCurTaskEnded : NULL;
            BOOL fEmptyAndLeave = pThis->m_fEmptyQueueAndSleep;

            // make sure they didn't just add something to the queue, or have we been asked to go to sleep
            if ( pThis->m_iGoToSleep || DPA_GetPtrCount( pThis->m_hTaskList ) == 0)
            {
                if ( CShellTaskScheduler::ReleaseWorker( pWorker ))
                {
                    pThis = NULL;
                }
            }
            OBJECT_LEAVE_CRITICAL_SECTION( pScheduler, m_csListLock );

            if ( pThis && !fEmptyAndLeave )
            {
                // they must have added something at the last moment...
                continue;
            }

            // we are being emptied, tell them we are no longer attatched....
            if ( hSleep )
            {
                ReleaseSemaphore( hSleep, 1, NULL);
            }

            break;
        }
        else
        {
#ifndef DEBUG
            //__try
            {
#endif
                if ( pTask->fSuspended )
                {
                    pTask->fSuspended = FALSE;
                    hr = pTask->pTask->Resume();
                }
                else
                {
                    // run the task...
                    hr = pTask->pTask->Run( );
                }
#ifndef DEBUG
            }
            //__except( EXCEPTION_EXECUTE_HANDLER )
           // {
                // ignore it.... and pray we are fine...
            //}
           // __endexcept
#endif

            BOOL fEmptyQueue;
            OBJECT_ENTER_CRITICAL_SECTION( pThis, m_csListLock );
            {
                pThis->m_pRunning = NULL;

                // check to see if we have been asked to notify them
                // on completion....
                // NOTE: the NOT clause is needed so that we release ourselves
                // NOTE: and signal them at the right point, if we do it here,
                // NOTE: they leave us stranded, delete the crit section and
                // NOTE: we fault.
                if ( pThis->m_iSignalCurTask && !pThis->m_fEmptyQueueAndSleep )
                {
                    LONG lPrevCount = 0;

                    // release all those that are waiting. (we are using a semaphore
                    // because we are a free threaded object and God knows how many
                    // threads are waiting, and he passed on the information in the
                    // iSignalCurTask variable
                    ReleaseSemaphore( pThis->m_hCurTaskEnded, pThis->m_iSignalCurTask, &lPrevCount );

                    // reset the count.
                    pThis->m_iSignalCurTask = 0;
                }
                fEmptyQueue = pThis->m_fEmptyQueueAndSleep;
            }
            OBJECT_LEAVE_CRITICAL_SECTION( pThis, m_csListLock );

            if ( hr != E_PENDING || fEmptyQueue )
            {
                ULONG cRef = pTask->pTask->Release();
                delete pTask;
                pTask = NULL;
            }

            // empty the message queue...
            while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
            {
                {
#ifdef DEBUG
                    if (msg.message == WM_ENDSESSION)
                        TraceMsg(TF_SCHEDULER, "(?%x)csts.tp: peek #2 got WM_ENDESSION", GetCurrentThreadId());
#endif
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
            }

            ASSERT( pThis && IS_VALID_WRITE_PTR( pThis, CShellTaskScheduler ));

            // the task must have been suspended because a higher priority
            // task has been added to the queue..... (this only works if the
            // task supports the Suspend() method).
            if ( hr == E_PENDING && pTask && !fEmptyQueue )
            {
                // put the task on the Suspended Queue ....
                pTask->fSuspended = TRUE;
                OBJECT_ENTER_CRITICAL_SECTION( pThis, m_csListLock );
                int iIndex = InsertInPriorityOrder( pThis->m_hTaskList, pTask, TRUE );
                OBJECT_LEAVE_CRITICAL_SECTION( pThis, m_csListLock );

                if ( iIndex == -1 )
                {
                    // we are so low on memory, kill it...
                    pTask->pTask->Kill( FALSE );
                    pTask->pTask->Release();
                    delete pTask;
                }
                pTask = NULL;
           }
       }
    }
    while ( TRUE );

    TraceMsg(TF_SCHEDULER, "(?%x)ShellTaskScheduler::Thread Ended", GetCurrentThreadId());
    SHCoUninitialize(hrInit);

    return 0;
}


STDMETHODIMP CShellTaskScheduler::Status( DWORD dwStatus, DWORD dwThreadTimeout )
{
    m_dwStatus = dwStatus & ITSSFLAG_FLAGS_MASK;
    if ( dwThreadTimeout != ITSS_THREAD_TIMEOUT_NO_CHANGE )
    {
/*
 * We don't support thread termination or pool timeout any more

        if ( dwStatus & ITSSFLAG_THREAD_TERMINATE_TIMEOUT )
        {
            m_dwThreadRlsKillTimeout = dwThreadTimeout;
        }
        else if ( dwStatus & ITSSFLAG_THREAD_POOL_TIMEOUT )
        {
            CShellTaskScheduler::s_dwComaTimeout = dwThreadTimeout;
        }
*/
    }
    return NOERROR;
}

STDMETHODIMP_(UINT) CShellTaskScheduler::CountTasks(REFTASKOWNERID rtoid)
{
    UINT iMatch = 0;
    BOOL fMatchAll = IsEqualGUID( TOID_NULL, rtoid );

    ENTER_CRITICAL_SECTION( m_csListLock );
    if ( fMatchAll )
    {
        iMatch = DPA_GetPtrCount( m_hTaskList );
    }
    else
    {
        int iIndex = 0;
        do
        {
            TaskNode * pNode = (TaskNode * )DPA_GetPtr( m_hTaskList, iIndex ++ );
            if ( !pNode )
            {
                break;
            }

            if ( IsEqualGUID( pNode->toid, rtoid ))
            {
                iMatch ++;
            }
        }
        while ( TRUE );
    }

    if ( m_pRunning )
    {
        if ( fMatchAll || IsEqualGUID( rtoid, m_pRunning->toid ))
        {
            iMatch ++;
        }
    }

    LEAVE_CRITICAL_SECTION( m_csListLock );

    return iMatch;

}


////////////////////////////////////////////////////////////////////////////////////
int InsertInPriorityOrder( HDPA hTaskList, TaskNode * pNewNode, BOOL fStart )
{
    // routine assumes that we are thread safe, therfore grab the crit-section
    // prior to calling this function

    int iPos = -1;
    int iIndex = 0;
    do
    {
        TaskNode * pNode = (TaskNode *) DPA_GetPtr( hTaskList, iIndex );
        if ( !pNode )
        {
            break;
        }

        // the fStart allows us to either add it before other tasks of the same
        // priority or after.
        if ((( pNode->dwPriority < pNewNode->dwPriority ) && !fStart ) || (( pNode->dwPriority <= pNewNode->dwPriority ) && fStart ))
        {
            iPos = DPA_InsertPtr( hTaskList, iIndex, pNewNode );
            break;
        }
        iIndex ++;
    }
    while ( TRUE );

    if ( iPos == -1 )
    {
        // add item to end of list...
        iPos = DPA_AppendPtr( hTaskList, pNewNode );
    }

    return iPos;
}


CShellTaskScheduler::WorkerData * CShellTaskScheduler::FetchWorker()
{
    WorkerData * pWorker = new WorkerData;

    if ( pWorker )
    {
        // call to Shlwapi thread pool
        if ( pWorker->Init(this) && SHQueueUserWorkItem( (LPTHREAD_START_ROUTINE)CShellTaskScheduler_ThreadProc,
                                                     pWorker,
                                                     0,
                                                     (DWORD_PTR)NULL,
                                                     (DWORD_PTR *)NULL,
                                                     "browseui.dll",
                                                     TPS_LONGEXECTIME | TPS_DEMANDTHREAD
                                                     ) )
        {
            return pWorker;
        }
        else
            delete pWorker;
    }

    return NULL;
}


// used by main thread proc to release its link the the task scheduler because it
// has run out of things to do....
BOOL CShellTaskScheduler::ReleaseWorker( WorkerData * pWorker )
{
    ASSERT( pWorker && IS_VALID_WRITE_PTR( pWorker, WorkerData ));

    CShellTaskScheduler * pThis = pWorker->pThis;

    OBJECT_ASSERT_CRITICAL_SECTION( pThis, m_csListLock );

    ASSERT( pWorker && IS_VALID_WRITE_PTR( pWorker, CShellTaskScheduler::WorkerData ));

    if ( DPA_GetPtrCount( pThis->m_hTaskList ) > 0 )
    {
        // something was added to the queue at the last minute....
        return FALSE;
    }

    // we assume we have entered the critsection of pThis
    pThis->m_pWorkerThread = NULL;
    pWorker->pThis = NULL;

    delete pWorker;

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CShellTaskScheduler::WorkerData::Init(CShellTaskScheduler *pts)
{
    pThis = pts;

    return TRUE;
}
