
/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    sessionMgr.cpp

Abstract:

    Implementation of Network session mennager class.

Author:

    Uri Habusha (urih)
--*/


#include "stdh.h"
#include "uniansi.h"
#include <malloc.h>
#include "qmp.h"
#include "sessmgr.h"
#include "cqmgr.h"
#include "qmthrd.h"
#include "CDeferredExecutionList.h"
#include "cgroup.h"
#include "admin.h"
#include "qmutil.h"
#include "proxy.h"
#include "ping.h"
#include <Fn.h>
#include "qmta.h"
#include "autohandle.h"
#include <mqstl.h>
#include <No.h>

#include "sessmgr.tmh"

extern CQueueMgr QueueMgr;
extern CQGroup* g_pgroupNonactive;
extern CQGroup* g_pgroupWaiting;

extern CDeferredExecutionList<CQmPacket> g_RequeuePacketList;

extern UINT  g_dwIPPort ;
extern DWORD g_dwThreadsNo ;

static WCHAR *s_FN=L"sessmgr";

//
// Extern variables
//
extern CSessionMgr SessionMgr;
extern CAdmin      Admin;


DWORD CSessionMgr::m_dwSessionCleanTimeout  = MSMQ_DEFAULT_CLIENT_CLEANUP;
DWORD CSessionMgr::m_dwQoSSessionCleanTimeoutMultiplier  = MSMQ_DEFAULT_QOS_CLEANUP_MULTIPLIER;
DWORD CSessionMgr::m_dwSessionAckTimeout = INFINITE;
DWORD CSessionMgr::m_dwSessionStoreAckTimeout = INFINITE;
DWORD CSessionMgr::m_dwIdleAckDelay = MSMQ_DEFAULT_IDLE_ACK_DELAY;
BOOL  CSessionMgr::m_fUsePing = TRUE;
HANDLE CSessionMgr::m_hAcceptAllowed = NULL;
bool  CSessionMgr::m_fUseQoS = false;
AP<char> CSessionMgr::m_pszMsmqAppName;       // = 0 - initialized by AP<> constructor
AP<char> CSessionMgr::m_pszMsmqPolicyLocator; //                    "
bool  CSessionMgr::m_fAllocateMore = false;
DWORD CSessionMgr::m_DeliveryRetryTimeOutScale = DEFAULT_MSMQ_DELIVERY_RETRY_TIMEOUT_SCALE;



BOOL g_fTcpNoDelay = FALSE;

#define MAX_ADDRESS_SIZE 16


struct CWaitingQueue
{
    CWaitingQueue(CQueue* pQueue, CTimer::CALLBACK_ROUTINE pfnCallback);

    CTimer m_Timer;
    CQueue* m_pQueue;
};

inline
CWaitingQueue::CWaitingQueue(
    CQueue* pQueue,
    CTimer::CALLBACK_ROUTINE pfnCallback
    ) :
    m_Timer(pfnCallback),
    m_pQueue(pQueue)
{
}
    
/*====================================================

CompareElements  of WAIT_INFO

Arguments:

Return Value:


=====================================================*/

template<>
BOOL AFXAPI  CompareElements(IN WAIT_INFO* const * pElem1,
                             IN WAIT_INFO* const * pElem2)
{
    const WAIT_INFO* pInfo1 = *pElem1;
    const WAIT_INFO* pInfo2 = *pElem2;

    const TA_ADDRESS* pAddr1 = pInfo1->pAddr;
    const TA_ADDRESS* pAddr2 = pInfo2->pAddr;

    if(pAddr1->AddressType != pAddr2->AddressType)
        return FALSE;

    if(memcmp(pAddr1->Address, pAddr2->Address, pAddr1->AddressLength) != 0)
        return FALSE;

    if (pInfo1->fQoS != pInfo2->fQoS)
        return FALSE;

    if(pInfo1->guidQMId == GUID_NULL)
        return TRUE;

    if(pInfo2->guidQMId == GUID_NULL)
        return TRUE;

    if(pInfo1->guidQMId == pInfo2->guidQMId)
        return TRUE;

    return FALSE;
}

/*====================================================

DestructElements of WAIT_INFO

Arguments:

Return Value:


=====================================================*/

template<>
void AFXAPI DestructElements(IN WAIT_INFO** ppNextHop, int n)
{
    for (int i=0; i<n ; i++)
    {
        delete (*ppNextHop)->pAddr;
        delete *ppNextHop;
        ppNextHop++;
    }
}

/*====================================================

HashKey For WAIT_INFO

Arguments:

Return Value:


=====================================================*/

template<>
UINT AFXAPI HashKey(IN WAIT_INFO* key)
{
    TA_ADDRESS* pAddr = key->pAddr;
    UINT nHash = 0;
    PUCHAR  p = pAddr->Address;

    for (int i = 0; i < pAddr->AddressLength; i++)
        nHash = (nHash<<5) + *p++;

    return nHash;
}

/********************************************************************************/
/*           I P     H E L P E R     R O U T I N E S                            */
/********************************************************************************/
SOCKET g_sockListen;

static DWORD WINAPI AcceptIPThread(PVOID /*pParam*/)
{

    SOCKADDR_IN acc_sin;
    int acc_sin_len = sizeof(acc_sin);

    char buff[TA_ADDRESS_SIZE + IP_ADDRESS_LEN];
    TA_ADDRESS* pa = (TA_ADDRESS*)&buff[0];

    for(;;)
    {
        try
        {
            CSocketHandle RcvSock = WSAAccept(
										g_sockListen,
										(struct sockaddr FAR *)&acc_sin,
										(int FAR *) &acc_sin_len,
										0,
										0
										);

            ASSERT(RcvSock != NULL);
            if (RcvSock == INVALID_SOCKET)
            {
                DWORD rc = WSAGetLastError();
                TrERROR(NETWORKING, "IP accept failed, error = %d", rc);
                DBG_USED(rc);
                continue;
            }

			TrTRACE(NETWORKING, "Socket accept() successfully and Client IP address is [%hs]\n",
                       inet_ntoa(*((in_addr *)&acc_sin.sin_addr.S_un.S_addr)));


            if (CSessionMgr::m_fUseQoS)
            {
                QOS  Qos;
                memset ( &Qos, QOS_NOT_SPECIFIED, sizeof(QOS) );

                //
                // ps buf is not required
                //
                Qos.ProviderSpecific.len = 0;
                Qos.ProviderSpecific.buf = NULL;

                //
                // sending flowspec
                //
                Qos.SendingFlowspec.ServiceType = SERVICETYPE_QUALITATIVE;

                //
                // receiving flowspec
                //
                Qos.ReceivingFlowspec.ServiceType = SERVICETYPE_QUALITATIVE;

                DWORD  dwBytesRet ;

                int rc = WSAIoctl( RcvSock,
                                   SIO_SET_QOS,
                                  &Qos,
                                   sizeof(QOS),
                                   NULL,
                                   0,
                                  &dwBytesRet,
                                   NULL,
                                   NULL ) ;
                if (rc != 0)
                {
                    DWORD dwErrorCode = WSAGetLastError();
                    TrWARNING(NETWORKING, "AcceptIPThread - WSAIoctl() failed, Error %d", dwErrorCode);
                    LogNTStatus(dwErrorCode,  s_FN, 110);

                    //
                    // Continue anyway...
                    //
                }
            }

            //
            // If the machine is in disconnected state, don't accept the incoming
            // connection.
            //
			ASSERT(("m_hAcceptAllowed should initialize before use", CSessionMgr::m_hAcceptAllowed != NULL));
            DWORD dwResult = WaitForSingleObject(CSessionMgr::m_hAcceptAllowed, INFINITE);
            if (dwResult != WAIT_OBJECT_0)
            {
                LogNTStatus(GetLastError(), s_FN, 201);
            }

            //
            // Build a TA format address
            //
            pa->AddressLength = IP_ADDRESS_LEN;
            pa->AddressType =  IP_ADDRESS_TYPE;
            * ((DWORD *)&(pa->Address)) = acc_sin.sin_addr.S_un.S_addr;

            //
            // Tell the session manager to create an Sock sesion object
            //
            SessionMgr.AcceptSockSession(pa, RcvSock);
        }
        catch(const exception&)
        {
            //
            //  No resources; accept next
            //
            LogIllegalPoint(s_FN, 76);
        }
    }
}

/*====================================================

CSessionMgr::CSessionMgr  - Constructor

Arguments:

Return Value:

=====================================================*/
CSessionMgr::CSessionMgr() :
    m_fCleanupTimerScheduled(FALSE),
    m_CleanupTimer(TimeToSessionCleanup),
    m_fUpdateWinSizeTimerScheduled(FALSE),
    m_wCurrentWinSize(MSMQ_DEFAULT_WINDOW_SIZE_PACKET),
    m_wMaxWinSize(MSMQ_DEFAULT_WINDOW_SIZE_PACKET),
    m_UpdateWinSizeTimer(TimeToUpdateWindowSize),
    m_fTryConnectTimerScheduled(FALSE),
    m_TryConnectTimer(TimeToTryConnect)
{
}


/*====================================================

CSessionMgr::Init

Arguments:

Return Value:

Thread Context: Main

=====================================================*/

HRESULT CSessionMgr::Init()
{
    m_hAcceptAllowed = CreateEvent(NULL, TRUE, TRUE, NULL);
	if (m_hAcceptAllowed == NULL)
	{
		TrERROR(NETWORKING, "Failed to create event for Session Manager uses. %!winerr!", GetLastError());
		throw bad_alloc();
	}

    DWORD dwSize = sizeof(DWORD);
    DWORD dwType;
    DWORD dwDefaultVal;
    HRESULT rc;
    //
    // Set Session clean-up timeout
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    if (!IsRoutingServer())   //[adsrv] CQueueMgr::GetMQS() == SERVICE_NONE)
    {
        //
        // In Client the default Release session timeout is 5 minitues
        //
        dwDefaultVal = MSMQ_DEFAULT_CLIENT_CLEANUP;
    }
    else
    {
        //
        // In FRS the default Release session timeout is 2 minitues
        //
        dwDefaultVal = MSMQ_DEFAULT_SERVER_CLEANUP;
    }

    rc = GetFalconKeyValue(
            MSMQ_CLEANUP_INTERVAL_REGNAME,
            &dwType,
            &m_dwSessionCleanTimeout,
            &dwSize,
            (LPCTSTR)&dwDefaultVal
            );

    if (rc != ERROR_SUCCESS)
    {
        m_dwSessionCleanTimeout = dwDefaultVal;
    }

    //
    // Get Cleanup interval multiplier for QoS sessions
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;

    rc = GetFalconKeyValue(MSMQ_QOS_CLEANUP_INTERVAL_MULTIPLIER_REGNAME,
                           &dwType,
                           &m_dwQoSSessionCleanTimeoutMultiplier,
                           &dwSize
                          );

    if (rc != ERROR_SUCCESS)
    {
        m_dwQoSSessionCleanTimeoutMultiplier = MSMQ_DEFAULT_QOS_CLEANUP_MULTIPLIER;
    }

    //
    // Get Max Unacked packet number
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    DWORD dwWindowSize;

    rc = GetFalconKeyValue(MSMQ_MAX_WINDOW_SIZE_REGNAME,
                           &dwType,
                           &dwWindowSize,
                           &dwSize
                          );

    if (rc != ERROR_SUCCESS)
    {
        m_wMaxWinSize = MSMQ_DEFAULT_WINDOW_SIZE_PACKET;
    }
    else
    {
        m_wMaxWinSize = (WORD) dwWindowSize;
    }

    m_wCurrentWinSize = m_wMaxWinSize;

    //
    // Get session Storage ack timeout
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    rc = GetFalconKeyValue(MSMQ_ACKTIMEOUT_REGNAME,
                           &dwType,
                           &m_dwSessionAckTimeout,
                           &dwSize
                          );
    if  (rc != ERROR_SUCCESS)
    {
        m_dwSessionAckTimeout = INFINITE;
    }


    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    rc = GetFalconKeyValue(MSMQ_STORE_ACKTIMEOUT_REGNAME,
                           &dwType,
                           &m_dwSessionStoreAckTimeout,
                           &dwSize
                          );
    if  (rc != ERROR_SUCCESS)
    {
        m_dwSessionStoreAckTimeout = INFINITE;
    }


    //
    // Get session Maximum acknowledge delay
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    rc = GetFalconKeyValue(MSMQ_IDLE_ACK_DELAY_REGNAME,
                           &dwType,
                           &m_dwIdleAckDelay,
                           &dwSize
                          );

    //
    // Get Max wait time
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;

    rc = GetFalconKeyValue(FALCON_WAIT_TIMEOUT_REGNAME,
                           &dwType,
                           &m_dwMaxWaitTime,
                           &dwSize
                           );
    if  (rc != ERROR_SUCCESS)
    {
        m_dwMaxWaitTime = 0;
    }

    //
    // use ping mechanism
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    DWORD dwUsePing;

    rc = GetFalconKeyValue(FALCON_USING_PING_REGNAME,
                           &dwType,
                           &dwUsePing,
                           &dwSize
                          );
    if  ((rc != ERROR_SUCCESS) || (dwUsePing != 0))
    {
        m_fUsePing = TRUE;
    }
    else
    {
        m_fUsePing = FALSE;
    }

    //
    // Use TCP_NODELAY socket option flag
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    DWORD dwNoDelay = 0;

    rc = GetFalconKeyValue(
            MSMQ_TCP_NODELAY_REGNAME,
            &dwType,
            &dwNoDelay,
            &dwSize
            );

    if((rc == ERROR_SUCCESS) && (dwNoDelay != 0))
    {
        g_fTcpNoDelay = TRUE;
    }
    else
    {
        g_fTcpNoDelay = FALSE;
    }

    //
    // use Quality Of Service (QoS)
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    DWORD dwUseQoS;

    rc = GetFalconKeyValue(MSMQ_USING_QOS_REGNAME,
                           &dwType,
                           &dwUseQoS,
                           &dwSize
                          );
    if  ((rc != ERROR_SUCCESS) || (dwUseQoS == 0))
    {
        m_fUseQoS = false;
    }
    else
    {
        m_fUseQoS = true;
    }

    if (m_fUseQoS)
    {
        //
        // Application name and policy locator name - used for the header of a QoS session
        //
        GetAndAllocateCharKeyValue(
            MSMQ_QOS_SESSIONAPP_REGNAME,
            &m_pszMsmqAppName,
            DEFAULT_MSMQ_QOS_SESSION_APP_NAME
            );

        GetAndAllocateCharKeyValue(
            MSMQ_QOS_POLICYLOCATOR_REGNAME,
            &m_pszMsmqPolicyLocator,
            DEFAULT_MSMQ_QOS_POLICY_LOCATOR
            );
    }

    //
    // Allocate More (for connector)
    //
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
    DWORD dwAllocateMore;

    rc = GetFalconKeyValue(MSMQ_ALLOCATE_MORE_REGNAME,
                           &dwType,
                           &dwAllocateMore,
                           &dwSize
                          );
    if  ((rc != ERROR_SUCCESS) || (dwAllocateMore == 0))
    {
        m_fAllocateMore = false;
    }
    else
    {
        m_fAllocateMore = true;
    }

	//
	// Read from registry flag that scales the retry timeout  on delivery error
	// by a factor
	//
    dwSize = sizeof(DWORD);
    dwType = REG_DWORD;
	rc = GetFalconKeyValue(
					MSMQ_DELIVERY_RETRY_TIMEOUT_SCALE_REGNAME,
					&dwType,
                    &m_DeliveryRetryTimeOutScale,
                    &dwSize
					);

	if(rc != ERROR_SUCCESS)
	{
		m_DeliveryRetryTimeOutScale = DEFAULT_MSMQ_DELIVERY_RETRY_TIMEOUT_SCALE;		
	}
	m_DeliveryRetryTimeOutScale = min(m_DeliveryRetryTimeOutScale, 10);
	

    return(MQ_OK);
}

/*====================================================

CSessionMgr::GetAndAllocateCharKeyValue
Utility private function (used by Init()) to get a char
value out of the MSMQ registry
Two versions: With default and without default.
=====================================================*/
void  CSessionMgr::GetAndAllocateCharKeyValue(
        LPCTSTR pszValueName,
        char  **ppchResult,
        const char *pchDefault
        )
{
    if (GetAndAllocateCharKeyValue(pszValueName, ppchResult))
    {
        return;
    }

    //
    // Either there is no reg. key, or some error happend during read.
    // Get the default.
    //
    *ppchResult = new char[strlen(pchDefault) + 1];
    strcpy(*ppchResult, pchDefault);
}

//
// GetAndAllocateCharKeyValue without default (used by the version with default)
// Returns true if the key was found, false otherwise.
//
bool CSessionMgr::GetAndAllocateCharKeyValue(
        LPCTSTR pszValueName,
        char  **ppchResult
        )
{
    DWORD dwSize = 0;
    DWORD dwType = REG_SZ;

    //
    // First call - get the size
    //
    HRESULT rc =
        GetFalconKeyValue(
            pszValueName,
            &dwType,
            0,
            &dwSize
            );

    if (rc != ERROR_SUCCESS)
    {
        //
        // Probably there is no key
        //
        return false;
    }

    //
    // There is a reg key - we did not supply buffer, but we have the
    // right size (note - size is in bytes)
    //

    AP<WCHAR> pwstrBuf = new WCHAR[dwSize / sizeof(WCHAR)];
    rc = GetFalconKeyValue(
            pszValueName,
            &dwType,
            pwstrBuf,
            &dwSize
            );

    if (rc != ERROR_SUCCESS)
    {
        //
        // We got the size allright. We should not fail here
        //
        ASSERT(0);
        LogHR(MQ_ERROR, s_FN, 150);
        return false;
    }

    //
    // We got the UNICODE value OK. Convert it to ANSI
    //
    DWORD dwMultibyteBufferSize = dwSize;
    *ppchResult = new char[dwMultibyteBufferSize];
    DWORD dwBytesCopied =
        ConvertToMultiByteString(pwstrBuf, *ppchResult, dwMultibyteBufferSize);

    if (dwBytesCopied == 0)
    {
        ASSERT(0); // Should not fail....
        delete [] *ppchResult;
        LogHR(MQ_ERROR, s_FN, 155);

        return false;
    }

    //
    // Success - got the value from the registry
    //
    return true;
}






/*====================================================

CSessionMgr::BeginAccept

Arguments:

Return Value:

Thread Context: Main

=====================================================*/
void CSessionMgr::BeginAccept()
{
	try
	{
		StartPingServer();
		StartPingClient();
	}
	catch(const bad_win32_error&)
	{
		//
		// Ignore failures in ping port initialization
		//
	}

	
    //
    // In WIN95/SP4 RAS, it is possible that the list is empty
    // if we are currently offline, and the IP RAS addresses are released.
    // However later we may dial.
    // We want to have an accept thread on IP even if the list is empty.
    //
    IPInit();
}


bool
CSessionMgr::IsReusedSession(
    const CTransportBase* pSession,
    DWORD noOfAddress,
    const CAddress* pAddress,
    const GUID** pGuid,
    bool         fQoS
    )
{
    //
    // If the connection is closed
    //
    if((pSession->GetSessionStatus() == ssNotConnect) || pSession->IsDisconnected())
        return false;

    const TA_ADDRESS* pa = pSession->GetSessionAddress();

    for(DWORD i = 0; i < noOfAddress; ++i)
    {
        if (memcmp(&pAddress[i], pa, (TA_ADDRESS_SIZE + pa->AddressLength)) != 0)
            continue;

        //
        // Check that destination QM guid is identical
        //

        //
        // If we do not use QoS, We would like to use the session if either:
        // 1. We opened a session to a private or public queue, and the other QM's
        //    GUID match the GUID of the QM, on which the queue resides (This check
        //    will catch multiple QM and change of address).
        // 2. We opened a session for a direct queue (we don't know the QMID
        //    anyway, so address match is good enough for us).
        //
        // However, if we use QoS, we want to use one session for public or
        // private queues (no QoS), and one session for direct queues (with QoS).
        //

        if (m_fUseQoS)
        {
            if (!fQoS && !pSession->IsQoS())
            {
                ASSERT(*pGuid[i] != GUID_NULL);

                if (*pSession->GetQMId() == *pGuid[i])
                    return true;

                continue;
            }

            if (fQoS && pSession->IsQoS())
            {
                ASSERT(*pGuid[i] == GUID_NULL);

                return true;
            }

            continue;
        }

        //
        // !m_fUseQoS
        //
        ASSERT(!fQoS);
        if (*pSession->GetQMId() == *pGuid[i])
            return true;

        if(*pGuid[i] == GUID_NULL)
            return true;
    }

    return false;
}


/*====================================================

CSessionMgr::GetSessionForDirectQueue

Arguments:

Return Value:

Thread Context:

=====================================================*/
HRESULT
CSessionMgr::GetSessionForDirectQueue(IN  CQueue*     pQueue,
                                      OUT CTransportBase**  ppSession)
{
    HRESULT rc;

    DirectQueueType dqt;
    AP<WCHAR> MachineName;

    try
    {
        LPCWSTR lpwsDirectQueuePath = FnParseDirectQueueType(pQueue->GetQueueName(), &dqt);
        FnExtractMachineNameFromPathName(
            lpwsDirectQueuePath,
            MachineName
            );
    }
    catch(const exception&)
    {
        return LogHR(MQ_ERROR_ILLEGAL_FORMATNAME, s_FN, 10);
    }

    DWORD dwMachineNameLen = wcslen(MachineName.get()) + 1;

    SP<CAddress> apTaAddr;
    DWORD dwAddressNo = 0;

    TrTRACE(NETWORKING, "Try to create direct connection with %ls", MachineName.get());

    switch (dqt)
    {
        case dtTCP:
        {
            SP<char> cTCPAddress;
            StackAllocSP(cTCPAddress, sizeof(char) * dwMachineNameLen);

            wcstombs(cTCPAddress.get(), MachineName.get(), dwMachineNameLen);
            cTCPAddress[dwMachineNameLen-1] = '\0';

            //
            // Check if TCP/IP is installed and enabled
            //
            ULONG Address = inet_addr(cTCPAddress.get());
            if (Address == INADDR_NONE)
            {
                return LogHR(MQ_ERROR, s_FN, 30);
            }


            StackAllocSP(apTaAddr, sizeof(CAddress));
            apTaAddr[0].AddressType = IP_ADDRESS_TYPE;
            apTaAddr[0].AddressLength = IP_ADDRESS_LEN;
            *reinterpret_cast<ULONG*>(&(apTaAddr[0].Address)) = Address;

            dwAddressNo++;

            break;
        }

        case dtOS:
        {
            bool fUseCache = (pQueue->GetRoutingRetry() % 16) != 15;
            std::vector<SOCKADDR_IN> Addresses;
			bool fSucc = NoGetHostByName(MachineName, &Addresses, fUseCache);
			if(!fSucc)
			{
				TrERROR(NETWORKING, "Failed to get host by name for %ls.", MachineName);
				return MQ_ERROR;
			}
			size_t nAddresses = Addresses.size();

            if(nAddresses == 0)
                return LogHR(MQ_ERROR, s_FN, 40);

            StackAllocSP(apTaAddr, sizeof(CAddress) * nAddresses);

			for (DWORD i = 0; i < nAddresses; ++i)
			{
				apTaAddr[i].AddressType = IP_ADDRESS_TYPE;
				apTaAddr[i].AddressLength = IP_ADDRESS_LEN;
				*(reinterpret_cast<ULONG*>(&(apTaAddr[i].Address))) = Addresses[i].sin_addr.s_addr;
				dwAddressNo++;
			}

            break;
        }

        default:
            return LogHR(MQ_ERROR_ILLEGAL_FORMATNAME, s_FN, 50);
    }

    SP<const GUID*> paQmId;
    StackAllocSP(paQmId, sizeof(GUID*) * dwAddressNo);

    for (DWORD i=0; i < dwAddressNo; i++)
    {
        paQmId[i] = &GUID_NULL;
    }

    //
    // Get session. If we use QoS, we ask for a session with QoS
    // flag set for direct queue
    //
    rc = GetSession(SESSION_RETRY,
                    pQueue,
                    dwAddressNo,
                    apTaAddr.get(),
                    paQmId.get(),
                    m_fUseQoS,
                    ppSession
                    );

    return LogHR(rc, s_FN, 70);
}


/*====================================================

CSessionMgr::GetSession

Arguments:

Return Value:

Called by the routing component to get a session
pointer for  a specific GUID.

=====================================================*/
HRESULT  CSessionMgr::GetSession(
                IN DWORD            dwFlag,
                IN const CQueue*    pDstQ,
                IN DWORD            dwNo,
                IN const CAddress*  apTaAddr,
                IN const GUID*      apQMId[],
                IN bool             fQoS,
                OUT CTransportBase** ppSession)
{
    DWORD            i;
    POSITION         pos;
    HRESULT          rc;

    *ppSession = NULL;

    {
        //
        //Scan the list to see if there is already a opened session
        //Dont have to make it under critical section, because no other
        //thread is supposed to change that list
        //Scan the map of queue handles
        // We use all addresses, it's a fast operation, not blocking, can save sessions
        //
        CTransportBase* pSess;

        CS lock(m_csListSess);

        pos = m_listSess.GetHeadPosition();
        while(pos != NULL)
        {
            pSess = m_listSess.GetNext(pos);
            if(IsReusedSession(pSess, dwNo, apTaAddr, apQMId, fQoS))
            {
                *ppSession = pSess;
                return(MQ_OK);
            }
        }
    }

    if(dwFlag == SESSION_CHECK_EXIST)
    {
        //
        // We just wanted to see
        // if we had such a session, and we
        // did not find such.
        //
        return LogHR(MQ_ERROR, s_FN, 80);
    }

    //
    //  No Open sessions, so try to open one. First check if this machine
    // can create a new session according to its license
    //

    //
    // This is a blocking path. Do not try on too many addresses if all fail
    // This is important for example if trying to connect to a site with many
    // FRSs when line is down
    // In case that we fail, we will try later with all the list
    //
    #define MAX_ADDRESSES_SINGLE_TRY    5
    DWORD dwLimit = (dwNo > MAX_ADDRESSES_SINGLE_TRY) ? MAX_ADDRESSES_SINGLE_TRY : dwNo;
    for(i = 0; i < dwLimit; i++)
    {

        P<CTransportBase> pSess;

        switch (apTaAddr[i].AddressType)
        {
            case IP_ADDRESS_TYPE:
                pSess = (CTransportBase *)new CSockTransport();
                break;

            case FOREIGN_ADDRESS_TYPE:
                pSess = (CTransportBase *)new CProxyTransport();
                break;

            default:
                ASSERT(0);
                continue;
        }

        //
        // Notify of a newly created session
        //
        rc = pSess->CreateConnection(reinterpret_cast<const TA_ADDRESS*>(&apTaAddr[i]), apQMId[i]);
        if(SUCCEEDED(rc))
        {
            NewSession(pSess);
            *ppSession = pSess.detach();
            return(MQ_OK);
        }

    }

    //
    //What to do next? We cant find a session, we cant create
    //a new one. The flag will tell us if to retry or to give up
    //
    if(dwFlag == SESSION_ONE_TRY)
    {
        return LogHR(MQ_ERROR, s_FN, 90);
    }

    ASSERT(dwFlag == SESSION_RETRY);

    //
    // use all addresses, it is not blocking
    //
    AddWaitingSessions(dwNo, apTaAddr, apQMId, fQoS, const_cast<CQueue*>(pDstQ));

    return(MQ_OK);
}

//+---------------------------------------------------------------------
//
// CSessionMgr::NotifyWaitingQueue()
//
//  This function is called from the session object, after session is
//  successfully established with remote side.
//
//+----------------------------------------------------------------------

void
CSessionMgr::NotifyWaitingQueue(
    IN const TA_ADDRESS* pa,
    IN CTransportBase * pSess
    )
    throw()
{
	try
	{
		//
		// In case we have deferred requeues, we do not want to move a queue
		// from the waiting group directly to the session. we want it to move
		// to the non active group first so that to make sure that all the
		// requeue operations are done prior to the queue moving to the active
		// session.
		//
		if (!g_RequeuePacketList.IsExecutionDone())
			return;
		
	    //
	    // Connect succeeded
	    //
	    CS lock(m_csMapWaiting);

	    CList <const CQueue *, const CQueue *&>* plist;
	    WAIT_INFO WaitInfo(const_cast<TA_ADDRESS*>(pa), *pSess->GetQMId(), pSess->IsQoS());

	    //
	    // Connect succeeded, check that no one remove the entry from the map
	    //
	    if (m_mapWaiting.Lookup(&WaitInfo, plist))
	    {
	        //
	        // A queue might be waiting for multiple
	        // sessions. So delete the queue from all other
	        // sessions that the queue was waiting for.
	        //
	        POSITION pos = plist->GetHeadPosition();
	        while (pos != NULL)
	        {
	        	CQueue* pQueue = const_cast<CQueue*>(plist->GetNext(pos));
	        	
	            if (pQueue->IsOnHold())
			    {
			    	CQGroup::MoveQueueToGroup(pQueue, g_pgroupNonactive);
			    	plist->GetNext(pos);
			    	continue;
			    }

	            pQueue->Connect(pSess);
	
	            //
	            // Scan the map of all the waiting sessions
	            //
	            RemoveWaitingQueue(pQueue);
	        }
	    }
	}
	catch(const exception&)
	{
		//
		// The QM failed to connect the queue to the session. However the code handle
		// it later. When the timer is expired, the queue will be moved to non-active
		// group for creating a connection
		//
	}
}


DWORD CSessionMgr::GetWaitingTimeForQueue(const CQueue* pQueue)
{
	static DWORD RequeueWaitTimeOut[] = {
         4 * 1000,
         8 * 1000,
         12 * 1000,
         16 * 1000,
         24  * 1000,
		 32 * 1000,
		 42 * 1000,
		 54 * 1000,
		 64 * 1000
    };
	const int  MaxTimeOutIndex = TABLE_SIZE(RequeueWaitTimeOut);

    ASSERT(pQueue->GetRoutingRetry() > 0);

    if (m_dwMaxWaitTime == 0)
    {
        if (pQueue->GetRoutingRetry() > MaxTimeOutIndex)
        {
            return RequeueWaitTimeOut[MaxTimeOutIndex -1];
        }

        return RequeueWaitTimeOut[pQueue->GetRoutingRetry() -1];
    }

    //
    // take the waiting time from the registery
    //
    return m_dwMaxWaitTime;
}


/*======================================================

Function:      CSessionMgr::AddWaitingQueue

Description:   add queue to waiting queue list

========================================================*/
void
CSessionMgr::AddWaitingQueue(CQueue* pQueue)
{
    CS lock(m_csMapWaiting);

    //
    // Increment the refernce count. We do it to promise that the queue object doen't remove
    // during the cleaning-up, while we continue to wait for conection. (can happen when the
    // message is expired and the application close the handle to it
    //
    R<CQueue> pRefQueue = SafeAddRef(pQueue);

	P<CWaitingQueue> p = new CWaitingQueue(pQueue, TimeToRemoveFromWaitingGroup);

	m_listWaitToConnect.AddTail(pQueue);

	try
	{
		CQGroup::MoveQueueToGroup(pQueue, g_pgroupWaiting);
	}
	catch(const exception&)
	{
		TrERROR(GENERAL, "Failed to move queue: %ls to waiting group", pQueue->GetQueueName());
		m_listWaitToConnect.RemoveTail();
		throw;
	}
	
	//
	// Schedule a retry sometimes in the near future
	//
	DWORD dwTime = GetWaitingTimeForQueue(pQueue);
	ExSetTimer(&p->m_Timer, CTimeDuration::FromMilliSeconds(dwTime * m_DeliveryRetryTimeOutScale));

	pRefQueue.detach();
	p.detach();

	#ifdef _DEBUG
    	AP<WCHAR> lpcsQueueName;

    	pQueue->GetQueue(&lpcsQueueName);
    	TrTRACE(NETWORKING, "Add queue: %ls to m_listWaitToConnect (AddWaitingQueue)", lpcsQueueName);
	#endif
}

/*======================================================

Function:      CQueueMgr::MoveQueueFromWaitingToNonActiveGroup

Description:   Move queue from waiting to Nonactive Group

Arguments:

Return Value:  None

Thread Context:

History Change:

========================================================*/
void
CSessionMgr::MoveQueueFromWaitingToNonActiveGroup(
    CQueue* pQueue
    )
{
    POSITION poslist;

    CS lock(m_csMapWaiting);

    if (m_listWaitToConnect.IsEmpty() ||
        ((poslist = m_listWaitToConnect.Find(pQueue, NULL)) == NULL))
    {
        return;
    }
    //
    // Move the queue to NON-active group
    //
    CQGroup::MoveQueueToGroup(pQueue, g_pgroupNonactive);
   	RemoveWaitingQueue(pQueue);
    //
    // The queue is removed from the list in SessionMgr.RemoveWaitingQueue
    //
}


void CSessionMgr::MoveAllQueuesFromWaitingToNonActiveGroup(void)
{
    CS lock(m_csMapWaiting);

    POSITION pos = m_listWaitToConnect.GetHeadPosition();
    while (pos)
    {
        CQueue* pQueue = const_cast<CQueue*>(m_listWaitToConnect.GetNext(pos));

		//
		// Move the queue to NON-active group
		//
		CQGroup::MoveQueueToGroup(pQueue, g_pgroupNonactive);
		RemoveWaitingQueue(pQueue);
		//
		// The queue is removed from the list in SessionMgr.RemoveWaitingQueue
		//
    }
}


void
WINAPI
CSessionMgr::TimeToRemoveFromWaitingGroup(
    CTimer* pTimer
    )
{
       P<CWaitingQueue> p = CONTAINING_RECORD(pTimer, CWaitingQueue, m_Timer);

	bool fRemoved = p->m_pQueue->TimeToRemoveFromWaitingGroup(pTimer, m_DeliveryRetryTimeOutScale);
	if(!fRemoved)
	{
		p.detach();
	}
}


static BOOL IsDuplicateAddress(const CAddress* apTaAddr, DWORD i)
{
    for (DWORD j = 0; j < i; j++)
    {
        if ((apTaAddr[i].AddressType == apTaAddr[j].AddressType) &&
            !(memcmp(apTaAddr[i].Address, apTaAddr[j].Address, apTaAddr[i].AddressLength)))
        {
            //
            // Identical address skip it.
            //
            return TRUE;
        }
    }

    return FALSE;
}

/*====================================================

CSessionMgr::AddWaitingSessions

Arguments:

Return Value:

=====================================================*/
void CSessionMgr::AddWaitingSessions(IN DWORD dwNo,
                                     IN const CAddress* apTaAddr,
                                     IN const GUID* aQMId[],
                                     IN bool        fQoS,
                                     IN CQueue *pDstQ)
{
    CS lock(m_csMapWaiting);

    for(DWORD i = 0; i < dwNo; i++)
    {
        if(IsDuplicateAddress(apTaAddr, i))
            continue;


        CList<const CQueue*, const CQueue*&>* plist;
        TA_ADDRESS *pa = reinterpret_cast<TA_ADDRESS*>(const_cast<CAddress*>(&apTaAddr[i]));
        P<WAIT_INFO> pWaitRouteInfo = new WAIT_INFO(pa, *aQMId[i], fQoS);
        if(m_mapWaiting.Lookup(pWaitRouteInfo, plist) == FALSE)
        {
            //
            // copy the TA address, only if the entry was not found (performance)
            //
            pWaitRouteInfo->pAddr = (TA_ADDRESS*) new UCHAR[TA_ADDRESS_SIZE + pa->AddressLength];
            memcpy(pWaitRouteInfo->pAddr, pa, TA_ADDRESS_SIZE+pa->AddressLength);

            plist = new CList<const CQueue*, const CQueue*&>;
            m_mapWaiting[pWaitRouteInfo] = plist;

            pWaitRouteInfo.detach();
        }

        TCHAR szAddr[30];
        TA2StringAddr(pa, szAddr, 30);
        TrTRACE(NETWORKING, "Add queue %ls to Waiting map for address %ls", pDstQ->GetQueueName(), szAddr);

        plist->AddTail(pDstQ);
    }

    SessionMgr.AddWaitingQueue(pDstQ);

    if (! m_fTryConnectTimerScheduled)
    {
        //
        // The try connect scheduler wasn't set yet. Set it now
        //
        ExSetTimer(&m_TryConnectTimer, CTimeDuration::FromMilliSeconds(5000));
        m_fTryConnectTimerScheduled = TRUE;
    }
}

/*====================================================

CSessionMgr::ReleaseSession

Arguments:

Return Value:

Some queue wants to release the use of a session.

Thread Context:

=====================================================*/
inline void CSessionMgr::ReleaseSession(void)
{
    POSITION pos,  prevpos;
    CTransportBase* pSession;

    CS lock(m_csListSess);

    static DWORD dwReleaseSessionCounter = 0;
    dwReleaseSessionCounter++;

    ASSERT(m_fCleanupTimerScheduled);

    pos = m_listSess.GetHeadPosition();
    while(pos != NULL)
    {
        prevpos = pos;
        pSession = m_listSess.GetNext(pos);

        //
        // For QoS (direct) sessions, execute cleanup only once each
        // m_dwQoSSessionCleanTimeoutMultiplier times
        //

        if (m_fUseQoS && pSession->IsQoS())
        {
            if ((dwReleaseSessionCounter % m_dwQoSSessionCleanTimeoutMultiplier) != 0)
            {
                continue;
            }
        }

        if(! pSession->IsUsedSession())
        {
            //
            // If no one is waiting on the session or it is
            // not used in last period, remove it from
            // the session manager and delete it
            //
            Close_ConnectionNoError(pSession, L"Release Unused session");
            if (pSession->GetRef() == 0)
            {
                m_listSess.RemoveAt(prevpos);
                delete pSession;
            }
        }
        else
        {
            pSession->SetUsedFlag(FALSE);
        }

    }

    if (m_listSess.IsEmpty())
    {
        m_fCleanupTimerScheduled = FALSE;
        return;
    }

    //
    // Still active session. Set a new timer for Session cleaning
    //
    ExSetTimer(&m_CleanupTimer, CTimeDuration::FromMilliSeconds(m_dwSessionCleanTimeout));
}


void
WINAPI
CSessionMgr::TimeToSessionCleanup(
    CTimer* pTimer
    )
/*++
Routine Description:
    The function is called from scheduler when fast session cleanup
    timeout is expired. The routine retrive the session manager
    object and calls the ReleaseSession function member.

Arguments:
    pTimer - Pointer to Timer structure. pTimer is part of the CSessionMgr
             object and it use to retrive the transport object.

Return Value:
    None

--*/
{
    CSessionMgr* pSessMgr = CONTAINING_RECORD(pTimer, CSessionMgr, m_CleanupTimer);

    TrTRACE(NETWORKING, "Call Session Cleanup");
    pSessMgr->ReleaseSession();
}


/*====================================================

CSessionMgr::RemoveWaitingQueue

Arguments:

Return Value:

  remove queue from waiting list. It should be done in following cases:
     - when a connection is established
     - when a queue is closed/deleted
     - when queue is moved from waiting group to nonactive group.

=====================================================*/
void
CSessionMgr::RemoveWaitingQueue(CQueue* pQueue)
{
    CS lock(m_csMapWaiting);

    //
    // Scan the map of all the waiting sessions
    //
    POSITION posmap;
    POSITION poslist;
    posmap = m_mapWaiting.GetStartPosition();
    while(posmap != NULL)
    {
        WAIT_INFO* pWaitRouteInfo;
        CList <const CQueue *, const CQueue *&>* plist;

        m_mapWaiting.GetNextAssoc(posmap, pWaitRouteInfo, plist);

        //
        // Get the list of queue waiting for
        // a specific session
        //
        poslist = plist->Find(pQueue, NULL);
        if (poslist != NULL)
        {
            plist->RemoveAt(poslist);
        }

        //
        // Even if the list of queues  waiting for session is empty,
        // do not delete it, and leave the entry in the map. This is
        // in order to fix Windows bug 612988.
        // Before fix, we had the following code:
        //
        // if (plist->IsEmpty())
        // {
        //     m_mapWaiting.RemoveKey(pWaitRouteInfo);
        //     delete plist;
        // }
        //
        // Code moved to ::TryConnect() below.
    }

    //
    // Remove the queue from waiting queue list.
    //
    poslist = m_listWaitToConnect.Find(pQueue, NULL);
    if (poslist != NULL)
    {
 #ifdef _DEBUG
        AP<WCHAR> lpcsQueueName;

        pQueue->GetQueue(&lpcsQueueName);
        TrTRACE(NETWORKING, "Remove queue: %ls from m_listWaitToConnect (RemoveWaitingQueue)", lpcsQueueName.get());
#endif
        m_listWaitToConnect.RemoveAt(poslist);
        pQueue->Release();
    }
}


/*====================================================

CSessionMgr::ListPossibleNextHops
    The routine is used for administration purpose. The routine returnes a
    list of addresses that can be the next hop for a waiting queue

Arguments:
    pQueue - pointer to the queue object
    pNextHopAddress - pointer to array of strings in which the routine
                      returnes the next hops.
    pNoOfNextHops - pointer to DWORD in which the routine returnes the
                    number of next hops

Return Value:
    HRESULT: MQ_ERROR is returned when the queue isn't in waiting state; MQ_OK otherwise.

=====================================================*/
HRESULT
CSessionMgr::ListPossibleNextHops(
    const CQueue* pQueue,
    LPWSTR** pNextHopAddress,
    DWORD* pNoOfNextHops
    )
{
    *pNoOfNextHops = 0;
    *pNextHopAddress = NULL;

    CS lock(m_csMapWaiting);

    if (m_listWaitToConnect.IsEmpty() ||
         ((m_listWaitToConnect.Find(pQueue, NULL)) == NULL))
    {
        return LogHR(MQ_ERROR, s_FN, 100);
    }


    CList <const TA_ADDRESS*, const TA_ADDRESS*> NextHopAddressList;

    //
    // Scan the map of all the waiting sessions
    //
    POSITION pos = m_mapWaiting.GetStartPosition();
    while(pos != NULL)
    {
        WAIT_INFO* pWaitRouteInfo;
        CList <const CQueue *, const CQueue *&>* plist;
        m_mapWaiting.GetNextAssoc(pos, pWaitRouteInfo, plist);

        //
        // Get the list of queue waiting for
        // a specific session
        //
        POSITION poslist = plist->Find(pQueue, NULL);
        if (poslist != NULL)
        {
            NextHopAddressList.AddTail(pWaitRouteInfo->pAddr);
        }
    }

    //
    // In some cases the queue can be in waiting state and don't have
    // next waiting hop. It can happened when the queue is a direct queue
    // and getHostByName failed
    //
    if (NextHopAddressList.IsEmpty())
    {
        return MQ_OK;
    }

    int Index = 0;
    AP<LPWSTR> pNext = new LPWSTR[NextHopAddressList.GetCount()];

    try
    {
        pos = NextHopAddressList.GetHeadPosition();
        while(pos != NULL)
        {
            const TA_ADDRESS* pAddr = NextHopAddressList.GetNext(pos);

            pNext[Index] = GetReadableNextHop(pAddr);
            ++Index;
        }
    }
    catch(const bad_alloc&)
    {
        while(Index)
        {
            delete [] pNext[--Index];
        }

        LogIllegalPoint(s_FN, 79);
        throw;
    }

    ASSERT(Index == NextHopAddressList.GetCount());

    *pNoOfNextHops = NextHopAddressList.GetCount();
    *pNextHopAddress = pNext.detach();

    return MQ_OK;
}

//+-----------------------------------------------------------------------
//
//  CSessionMgr::MarkAddressAsNotConnecting()
//
//  Windows bug 61298.
//  this function reset the "fInConnectionProcess" flag to FALSE.
//
//+-----------------------------------------------------------------------

void  CSessionMgr::MarkAddressAsNotConnecting(const TA_ADDRESS *pAddr,
                                              const GUID&       guidQMId,
                                              BOOL              fQoS )
{
    CS lock(m_csMapWaiting);

    CList <const CQueue *, const CQueue *&>* plist;
    WAIT_INFO WaitInfo(const_cast<TA_ADDRESS*> (pAddr), guidQMId, fQoS) ;

    if (m_mapWaiting.Lookup(&WaitInfo, plist))
    {
        POSITION pos = m_mapWaiting.GetStartPosition();

        while(pos != NULL)
        {
            WAIT_INFO *pWaitInfo ;
            CList<const CQueue*, const CQueue*&>* plistTmp ;

            m_mapWaiting.GetNextAssoc(pos, pWaitInfo, plistTmp);

            if (plistTmp == plist)
            {
                //
                // entry found. reset the flag.
                // Can it already be false ? yes:
                //
                // 1. You send a message. It reach target computer.
                // 2. network fail, so session ack does not arrive.
                // 3. CSockTransport::CheckForAck() wake up, and close the
                //    connection.
                // 4. CSockTransport::CloseConnection() move queue to the
                //    nonactive group. The reference count on the Transport
                //    object is 3 or greater. So it's still alive and used.
                // 5. This queue is now rescheduled for  a new session.
                // 6. CSessionMgr::AddWaitingSessions() is called, and create
                //    a WAIT_INFO structure.
                // 7. CSockTransport::CloseConnection() is called again (on
                //    old connection) from failing socket operations.
                // 8. so we reach here again from 7, and flag can be TRUE or
                //    FALSE, sporadically. It depend on the status of the new
                //    connection that is now established.
                //
                // Setting it to FALSE doesn't do any harm, it just make the
                // fix to Windows bug 612988 less than optimal.
                //
                pWaitInfo->fInConnectionProcess = FALSE ;
                return ;
            }
        }
    }
}

//+-----------------------------------------------------------------------
//
//  BOOL  CSessionMgr::GetAddressToTryConnect()
//
// Get an address of remote computer so we can try to connect to.
// Do not return addresses that are now used by other threads.
// Fix for Windows bug 612988, where multiple worker threads tried to
// connect to same address.
//
//+-----------------------------------------------------------------------

BOOL  CSessionMgr::GetAddressToTryConnect( OUT WAIT_INFO **ppWaitConnectInfo )
{
    static int s_iteration = 0;

    CS lock(m_csMapWaiting);

    //
    // Check which session to try
    //
    int iMaxIteration = m_mapWaiting.GetCount() ;

    s_iteration++;
    if (s_iteration > iMaxIteration)
    {
        s_iteration = 1;
    }

    //
    // And get to it
    //
    int i = 0 ;
    CList<const CQueue*, const CQueue*&>* plist;
    POSITION pos = m_mapWaiting.GetStartPosition();

    for ( ; i < s_iteration; i++)
    {
        m_mapWaiting.GetNextAssoc(pos, *ppWaitConnectInfo, plist);
    }

    //
    // Check if no other thread is trying to connect to this address.
    //
    if (!((*ppWaitConnectInfo)->fInConnectionProcess))
    {
        return TRUE ;
    }

    //
    // Try other addresses, from present position to end of map.
    //
    for ( ; i < iMaxIteration ; i++)
    {
        m_mapWaiting.GetNextAssoc(pos, *ppWaitConnectInfo, plist);

        if (!((*ppWaitConnectInfo)->fInConnectionProcess))
        {
            return TRUE ;
        }
    }

    //
    // Address not found. Try to find one in the first entries of the map.
    //
    pos = m_mapWaiting.GetStartPosition();
    for ( i = 0 ; i < (s_iteration-1) ; i++)
    {
        m_mapWaiting.GetNextAssoc(pos, *ppWaitConnectInfo, plist);

        if (!((*ppWaitConnectInfo)->fInConnectionProcess))
        {
            return TRUE ;
        }
    }

    //
    // didn't find a suitable address.
    //
    return FALSE ;
}

/*====================================================

CSessionMgr::TryConnect

Arguments:

Return Value:

Check if there are some waiting sessions, and try to connect
to them

Thread Context: Scheduler

=====================================================*/

inline void CSessionMgr::TryConnect()
{
    static DWORD s_dwConnectingThreads = 0 ;
    //
    // This count the number of threads that call CreateConnection().
    // We want that at any givem time, at least one worker thread will
    // be availalbe for other operations and won't be blocked on
    // CreateConnection(). Fix for bug 6375.
    //

    GUID gQMId;
    BOOL fQoS;
    P<TA_ADDRESS> pa;

    {
        CS lock(m_csMapWaiting);

        ASSERT(m_fTryConnectTimerScheduled);

        //
        // First, cleanup entries that need to be removed.
        //
        POSITION posmap;
        posmap = m_mapWaiting.GetStartPosition();
        while(posmap != NULL)
        {
            WAIT_INFO* pWaitRouteInfo;
            CList <const CQueue *, const CQueue *&>* plist;

            m_mapWaiting.GetNextAssoc(posmap, pWaitRouteInfo, plist);

            //
            // If the list of queues waiting for session is empty,
            // delete it, and delete the entry from the map.
            // If we're trying now to connect to this address, then
            // leave entry in map. It will be removed later.
            //
            if ( plist->IsEmpty()                        &&
                !(pWaitRouteInfo->fInConnectionProcess) )
            {
                m_mapWaiting.RemoveKey(pWaitRouteInfo);
                delete plist;
            }
        }

        //
        // If no waiting sessions, return
        //
        if(m_mapWaiting.IsEmpty())
        {
            m_fTryConnectTimerScheduled = FALSE;
            return;
        }

        //
        // Reschedule the timer. The address is removed from the map only when the
        // connection is completed successfully, therfore we need to reschedule the time
        // even if it is the last address in the map. However if the connection was successed,
        // next time the scheduler is called, the map will be empty and the scheduler doesn't
        // set anymore
        //
        ExSetTimer(&m_TryConnectTimer, CTimeDuration::FromMilliSeconds(5000));

        if (s_dwConnectingThreads >= (g_dwThreadsNo - 1))
        {
            //
            // enough threads are trying to connect. Leave this one
            // free for other operations.
            //
            return ;
        }

        WAIT_INFO* pWaitRouteInfo;
        BOOL f = GetAddressToTryConnect( &pWaitRouteInfo) ;
        if (!f)
        {
            //
            // Didn't find any address.
            //
            return ;
        }
        pWaitRouteInfo->fInConnectionProcess = TRUE ;

        //
        // since the key can be destruct during the Network connect, we copy the address
        // and the QM guid. We can use a CriticalSection to avoid this situation, but the
        // the NetworkConnect function can take lot of time (access to DNS) and try to avoid
        // the case that all the threads are waiting for this critical section.
        //
        pa = (TA_ADDRESS*) new UCHAR[sizeof(TA_ADDRESS)+ (pWaitRouteInfo->pAddr)->AddressLength];
        memcpy(pa,pWaitRouteInfo->pAddr, TA_ADDRESS_SIZE+(pWaitRouteInfo->pAddr)->AddressLength);
        gQMId = pWaitRouteInfo->guidQMId;
        fQoS = pWaitRouteInfo->fQoS ;

        s_dwConnectingThreads++ ;
    }

    //
    // And try to open a session with it.
    //
    P<CTransportBase> pSess = new CSockTransport();
    HRESULT rcCreate = pSess->CreateConnection(pa, &gQMId, FALSE);
    if(SUCCEEDED(rcCreate))
    {
        //
        // Notify the Session Manager of a newly created session
        //
        NewSession(pSess);
        pSess.detach();
    }

    {
        CS lock(m_csMapWaiting);

        if (FAILED(rcCreate))
        {
            MarkAddressAsNotConnecting( pa, gQMId, fQoS ) ;
        }

        s_dwConnectingThreads-- ;
        ASSERT(((LONG)s_dwConnectingThreads) >= 0) ;
    }
}


void
WINAPI
CSessionMgr::TimeToTryConnect(
    CTimer* pTimer
    )
/*++
Routine Description:
    The function is called from scheduler to try connecton to next address,
    when timeout is expired. The routine retrive the session manager
    object and calls the TryConnect memeber function

Arguments:
    pTimer - Pointer to Timer structure. pTimer is part of the SessionMgr
             object and it use to retrive it.

Return Value:
    None

--*/
{
    CSessionMgr* pSessMgr = CONTAINING_RECORD(pTimer, CSessionMgr, m_TryConnectTimer);

    TrTRACE(NETWORKING, "Call Try Connect");
    pSessMgr->TryConnect();
}


/*====================================================

CSessionMgr::AcceptSockSession

Arguments:

Return Value:

Called when a Sock connection was accepted.

=====================================================*/
void CSessionMgr::AcceptSockSession(IN TA_ADDRESS *pa,
                                    IN CSocketHandle& pSocketHandle)
{
    ASSERT(pa != NULL);

    //
    // Create a new session
    //
    P<CSockTransport> pSess = new CSockTransport;

    //
    // And pass to the session object
    //
    pSess->Connect(pa, pSocketHandle);

    //
    // Notify of a newly created session
    //
    NewSession(pSess);
	pSess.detach();
}


void
CSessionMgr::IPInit(void)
{
    g_sockListen = QmpCreateSocket(m_fUseQoS);

    ASSERT(g_dwIPPort);

    SOCKADDR_IN local_sin;  /* Local socket - internet style */
    local_sin.sin_family = AF_INET;
    local_sin.sin_port = htons(DWORD_TO_WORD(g_dwIPPort));        /* Convert to network ordering */
    local_sin.sin_addr.s_addr = GetBindingIPAddress();

    // to make sure that we open the port exclusivly
    BOOL exclusive = TRUE;
    int rc = setsockopt( g_sockListen, SOL_SOCKET, SO_EXCLUSIVEADDRUSE , (char *)&exclusive, sizeof(exclusive));
    if (rc != 0)
    {
        DWORD gle = WSAGetLastError();
        TrERROR(NETWORKING, "failed to set SO_EXCLUSIVEADDRUSE option to listening socket, %!winerr!, QM Terminates", gle);
        throw bad_win32_error(gle);
    }

    rc = bind( g_sockListen, (struct sockaddr FAR *) &local_sin, sizeof(local_sin));
    if (rc != 0)
    {
        DWORD gle = WSAGetLastError();
        TrERROR(NETWORKING, "bind to port %d failed. %!winerr!", g_dwIPPort, gle);

		if((gle == WSAEADDRINUSE) && IsLocalSystemCluster())
		{
			//
			// This is the case when msmq service was started on the physical node
			// before the machine was clusterized.
			// As a result msmq on the physical node is not cluster aware and listen on all ip addresses
			// including cluster ip.
			// This prevents msmq resource to listen on cluster IP, fails with WSAEADDRINUSE.
			// The user need to restart msmq service on the physical node
			// so it will be cluster aware.
			//
			EvReportWithError(EVENT_ERROR_QM_NOT_CLUSTER_AWARE, gle);
		}

		EvReportWithError(EVENT_ERROR_IP_BIND_FAILED, gle);
        throw bad_win32_error(gle);
    }

    rc = listen( g_sockListen, 5 ); // 5 is the maximum allowed length the queue of pending connections may grow
    if (rc != 0)
    {
        DWORD gle = WSAGetLastError();
        TrERROR(NETWORKING, "Listen failed, %!winerr!", gle);
        throw bad_win32_error(gle);
    }

	DWORD dwThreadId;
    HANDLE hThread = CreateThread(
							NULL,
							0,
							AcceptIPThread,
							NULL,
							0,
							&dwThreadId
							);

    if(hThread == NULL)
    {
        DWORD gle = GetLastError();
        TrERROR(NETWORKING, "Creation of listening thread failed , %!winerr!", gle);
        LogHR(gle, s_FN, 160);
        throw bad_alloc();
    }
    CloseHandle(hThread);
}





/*====================================================

CSessionMgr::SetWindowSize

Arguments: Window size

Return Value:  None

Called when the write to socket failed .

=====================================================*/
void CSessionMgr::SetWindowSize(WORD wWinSize)
{
    CS lock(m_csWinSize);

    m_wCurrentWinSize = wWinSize;

    //
    // The routine can calls multiple times. If the timer already set, try to cancel
    // it. If the cancel failed, don't care, it means there is another timer that already
    // expired but doesn't execute yet. This timer will update the window size and reschedule
    // the timer
    //
    if (!m_fUpdateWinSizeTimerScheduled || ExCancelTimer(&m_UpdateWinSizeTimer))
    {
        ExSetTimer(&m_UpdateWinSizeTimer, CTimeDuration::FromMilliSeconds(90*1000));
        m_fUpdateWinSizeTimerScheduled = TRUE;
    }

    TrTRACE(NETWORKING, "QM window size set to: %d", m_wCurrentWinSize);
}

/*====================================================

CSessionMgr::UpdateWindowSize

Arguments: None

Return Value:  None

Called from the scheduler to update the window size.

=====================================================*/
inline void CSessionMgr::UpdateWindowSize()
{
    CS lock(m_csWinSize);

    //
    // Update the window size until it reach the max size
    //
    m_wCurrentWinSize = (WORD)min((m_wCurrentWinSize * 2), m_wMaxWinSize);

    TrTRACE(NETWORKING, "QM window size set to: %d", m_wCurrentWinSize);

    if (m_wCurrentWinSize != m_wMaxWinSize)
    {
        //
        // Doesn't reach the maximum, reschedule for update
        //
        ExSetTimer(&m_UpdateWinSizeTimer, CTimeDuration::FromMilliSeconds(30*1000));
        return;
    }

    m_fUpdateWinSizeTimerScheduled = FALSE;
}


void
WINAPI
CSessionMgr::TimeToUpdateWindowSize(
    CTimer* pTimer
    )
/*++
Routine Description:
    The function is called from scheduler to update the machine window size,
    when timeout is expired. The routine retrive the session manager
    object and calls the UpdateWindowSize function member.

Arguments:
    pTimer - Pointer to Timer structure. pTimer is part of the SessionMgr
             object and it use to retrive the transport object.

Return Value:
    None

--*/
{
    CSessionMgr* pSessMgr = CONTAINING_RECORD(pTimer, CSessionMgr, m_UpdateWinSizeTimer);

    TrTRACE(NETWORKING, "Call window update size");
    pSessMgr->UpdateWindowSize();
}


void
CSessionMgr::ConnectNetwork(
    void
    )
//
// Routine Description:
//      The routine move the network from disconnected state to connected. The
// routine, resume the accept threads to allow acception of new session.
//
// Arguments:
//      None.
//
// Returned Value:
//		None.
//
{
    //
    // Allow accept of incoming connection
    //                                    '
	ASSERT(("m_hAcceptAllowed should initialize before use", m_hAcceptAllowed != NULL));
    SetEvent(m_hAcceptAllowed);
}


void
CSessionMgr::DisconnectNetwork(
    void
    )
//
// Routine Description:
//      The routine move the network from connected state to disconnected. The
// routine, suspend the accept threads to disallow acception of new session,
// and inform all the active session about the new state
//
// Arguments:
//      None.
//
// Returned Value:
//		None.
//
// Note:
//     The routine can throw an exception
//
{
   //
    // Don't allow accept of incoming connection
    //
	ASSERT(("m_hAcceptAllowed should initialize before use", m_hAcceptAllowed != NULL));
    ResetEvent(m_hAcceptAllowed);

	try
	{
	    //
	    // move all the waiting queues to nonactive group for re-routing
	    //
		MoveAllQueuesFromWaitingToNonActiveGroup();

	    //
	    // scan the open session and return them to connected state
	    //
        CS lock(m_csListSess);

	    POSITION pos = m_listSess.GetHeadPosition();
	    while(pos != NULL)
	    {
	        CTransportBase* pSess = m_listSess.GetNext(pos);
	        pSess->Disconnect();
	    }
	}
	catch(const exception&)
	{
	    SetEvent(m_hAcceptAllowed);
	    throw;
	}
		
}


void
CSessionMgr::NewSession(
    CTransportBase *pSession
    )
{
    CS lock(m_csListSess);

    //
    // Add the session to the list of sessions
    //
    m_listSess.AddTail(pSession);

    //
    // Set the cleanup timer, if it was not set yet
    //
    if (!m_fCleanupTimerScheduled)
    {
        ExSetTimer(&m_CleanupTimer, CTimeDuration::FromMilliSeconds(m_dwSessionCleanTimeout));
        m_fCleanupTimerScheduled = TRUE;
    }
}
