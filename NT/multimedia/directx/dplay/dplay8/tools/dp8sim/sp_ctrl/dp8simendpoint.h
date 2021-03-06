/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dp8simendpoint.h
 *
 *  Content:	Header for endpoint object class.
 *
 *  History:
 *   Date      By        Reason
 *  ========  ========  =========
 *  05/08/01  VanceO    Created.
 *
 ***************************************************************************/



//=============================================================================
// Object flags
//=============================================================================
#define DP8SIMENDPOINTOBJ_DISCONNECTING		0x01	// the endpoint is disconnecting





//=============================================================================
// Endpoint object class
//=============================================================================
class CDP8SimEndpoint
{
	public:

		inline BOOL IsValidObject(void)
		{
			if ((this == NULL) || (IsBadWritePtr(this, sizeof(CDP8SimEndpoint))))
			{
				return FALSE;
			}

			if (*((DWORD*) (&this->m_Sig)) != 0x454d4953)	// 0x45 0x4d 0x49 0x53 = 'EMIS' = 'SIME' in Intel order
			{
				return FALSE;
			}

			return TRUE;
		};




		static BOOL FPMAlloc(void* pvItem, void* pvContext)
		{
			CDP8SimEndpoint *	pDP8SimEndpoint = (CDP8SimEndpoint*) pvItem;


			pDP8SimEndpoint->m_Sig[0] = 'S';
			pDP8SimEndpoint->m_Sig[1] = 'I';
			pDP8SimEndpoint->m_Sig[2] = 'M';
			pDP8SimEndpoint->m_Sig[3] = 'e';	// start with lower case so we can tell when it's in the pool or not

			pDP8SimEndpoint->m_lRefCount			= 0;

			if (! DNInitializeCriticalSection(&pDP8SimEndpoint->m_csLock))
			{
				return FALSE;
			}

			//
			// Don't allow critical section re-entry.
			//
			DebugSetCriticalSectionRecursionCount(&pDP8SimEndpoint->m_csLock, 0);

			pDP8SimEndpoint->m_dwFlags				= 0;
			pDP8SimEndpoint->m_hRealSPEndpoint		= NULL;
			pDP8SimEndpoint->m_pvUserContext		= NULL;

			return TRUE;
		}


#undef DPF_MODNAME
#define DPF_MODNAME "CDP8SimEndpoint::FPMInitialize"
		static void FPMInitialize(void* pvItem, void* pvContext)
		{
			CDP8SimEndpoint *	pDP8SimEndpoint = (CDP8SimEndpoint*) pvItem;


			pDP8SimEndpoint->m_lRefCount++;	// somebody is getting a pointer to this object
			DNASSERT(pDP8SimEndpoint->m_lRefCount == 1);


			//
			// Reset the flags.
			//
			pDP8SimEndpoint->m_dwFlags = 0;


			//
			// Save the real SP's endpoint handle.
			//
			pDP8SimEndpoint->m_hRealSPEndpoint = (HANDLE) pvContext;


			//
			// Change the signature before handing it out.
			//
			pDP8SimEndpoint->m_Sig[3]	= 'E';
		}


#undef DPF_MODNAME
#define DPF_MODNAME "CDP8SimEndpoint::FPMRelease"
		static void FPMRelease(void* pvItem)
		{
			CDP8SimEndpoint *	pDP8SimEndpoint = (CDP8SimEndpoint*) pvItem;


			DNASSERT(pDP8SimEndpoint->m_lRefCount == 0);


			//
			// Change the signature before putting the object back in the pool.
			//
			pDP8SimEndpoint->m_Sig[3]	= 'e';
		}


#undef DPF_MODNAME
#define DPF_MODNAME "CDP8SimEndpoint::FPMDealloc"
		static void FPMDealloc(void* pvItem)
		{
			CDP8SimEndpoint *	pDP8SimEndpoint = (CDP8SimEndpoint*) pvItem;


			DNASSERT(pDP8SimEndpoint->m_lRefCount == 0);

			DNDeleteCriticalSection(&pDP8SimEndpoint->m_csLock);
		}




#undef DPF_MODNAME
#define DPF_MODNAME "CDP8SimEndpoint::AddRef"
		inline void AddRef(void)
		{
			LONG	lResult;


			lResult = InterlockedIncrement(&this->m_lRefCount);
			DNASSERT(lResult > 0);
			DPFX(DPFPREP, 9, "Endpoint 0x%p refcount = %u.", this, lResult);
		};

#undef DPF_MODNAME
#define DPF_MODNAME "CDP8SimEndpoint::Release"
		inline void Release(void)
		{
			LONG	lResult;


			lResult = InterlockedDecrement(&this->m_lRefCount);
			DNASSERT(lResult >= 0);
			if (lResult == 0)
			{
				DPFX(DPFPREP, 9, "Endpoint 0x%p refcount = 0, returning to pool.", this);

				//
				// Time to return this object to the pool.
				//
				g_FPOOLEndpoint.Release(this);
			}
			else
			{
				DPFX(DPFPREP, 9, "Endpoint 0x%p refcount = %u.", this, lResult);
			}
		};


		inline void Lock(void)		{ DNEnterCriticalSection(&this->m_csLock); };
		inline void Unlock(void)	{ DNLeaveCriticalSection(&this->m_csLock); };


#undef DPF_MODNAME
#define DPF_MODNAME "CDP8SimEndpoint::IsDisconnecting"
		inline BOOL IsDisconnecting(void)
		{
			AssertCriticalSectionIsTakenByThisThread(&this->m_csLock, TRUE);
			return ((this->m_dwFlags & DP8SIMENDPOINTOBJ_DISCONNECTING) ? TRUE: FALSE);
		};


#undef DPF_MODNAME
#define DPF_MODNAME "CDP8SimEndpoint::NoteDisconnecting"
		inline void NoteDisconnecting(void)
		{
			AssertCriticalSectionIsTakenByThisThread(&this->m_csLock, TRUE);
#pragma TODO(vanceo, "Have separate upper/lower layer disconnect flags")
			//DNASSERT(! (this->m_dwFlags & DP8SIMENDPOINTOBJ_DISCONNECTING));
			this->m_dwFlags |= DP8SIMENDPOINTOBJ_DISCONNECTING;
		};


		inline HANDLE GetRealSPEndpoint(void)				{ return this->m_hRealSPEndpoint; };
		inline PVOID GetUserContext(void)					{ return this->m_pvUserContext; };

		inline void SetUserContext(PVOID pvUserContext)		{ this->m_pvUserContext = pvUserContext; };

	
	private:
		BYTE				m_Sig[4];				// debugging signature ('SIME')
		LONG				m_lRefCount;			// number of references for this object
		DNCRITICAL_SECTION	m_csLock;				// lock protecting the endpoint data
		DWORD				m_dwFlags;				// flags for this endpoint
		HANDLE				m_hRealSPEndpoint;		// real service provider's endpoint handle
		PVOID				m_pvUserContext;		// upper layer user context for endpoint
};

