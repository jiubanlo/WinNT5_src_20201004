/*++

   Copyright    (c) 1997-2002    Microsoft Corporation

   Module  Name :
       LKR-old-iter.cpp

   Abstract:
       Implements the old, deprecated iterators for LKRhash.
       Use the STL-style iterators instead. These will go away soon.

   Author:
       George V. Reilly      (GeorgeRe)     1998

   Project:
       LKRhash

--*/

#include "precomp.hxx"


#ifndef LIB_IMPLEMENTATION
# define DLL_IMPLEMENTATION
# define IMPLEMENTATION_EXPORT
#endif // !LIB_IMPLEMENTATION

#include <lkrhash.h>

#include "i-lkrhash.h"


#ifndef __LKRHASH_NO_NAMESPACE__
namespace LKRhash {
#endif // !__LKRHASH_NO_NAMESPACE__

#ifdef LKR_DEPRECATED_ITERATORS

//------------------------------------------------------------------------
// Function: CLKRLinearHashTable::_InitializeIterator
// Synopsis: Make the iterator point to the first record in the hash subtable.
//------------------------------------------------------------------------

LK_RETCODE
CLKRLinearHashTable::_InitializeIterator(
    CIterator* piter)
{
    if (!IsUsable())
        return LK_UNUSABLE;

    IRTLASSERT(piter != NULL);
    IRTLASSERT(piter->m_lkl == LKL_WRITELOCK
               ?  this->IsWriteLocked()
               :  this->IsReadLocked());
    if (piter == NULL  ||  piter->m_plht != NULL)
        return LK_BAD_ITERATOR;

    piter->m_plht = this;
    piter->m_dwBucketAddr = 0;

    PBucket pbkt = _BucketFromAddress(piter->m_dwBucketAddr);
    IRTLASSERT(pbkt != NULL);

    if (_UseBucketLocking())
    {
        if (piter->m_lkl == LKL_WRITELOCK)
            pbkt->WriteLock();
        else
            pbkt->ReadLock();
    }

    piter->m_pnc = &pbkt->m_ncFirst;
    piter->m_iNode = _NodeBegin() - _NodeStep();

    // Let IncrementIterator do the hard work of finding the first
    // slot in use.
    return IncrementIterator(piter);
} // CLKRLinearHashTable::_InitializeIterator



//------------------------------------------------------------------------
// Function: CLKRHashTable::InitializeIterator
// Synopsis: make the iterator point to the first record in the hash table
//------------------------------------------------------------------------

LK_RETCODE
CLKRHashTable::InitializeIterator(
    CIterator* piter)
{
    if (!IsUsable())
        return LK_UNUSABLE;

    IRTLASSERT(piter != NULL  &&  piter->m_pht == NULL);
    if (piter == NULL  ||  piter->m_pht != NULL)
        return LK_BAD_ITERATOR;

    // First, lock all the subtables
    if (piter->m_lkl == LKL_WRITELOCK)
        this->WriteLock();
    else
        this->ReadLock();

    // Must call IsValid inside a lock to ensure that none of the state
    // variables change while it's being evaluated
    IRTLASSERT(IsValid());
    if (!IsValid())
        return LK_UNUSABLE;

    piter->m_pht  = this;
    piter->m_ist  = -1;
    piter->m_plht = NULL;

    // Let IncrementIterator do the hard work of finding the first
    // valid node in the subtables.
    return IncrementIterator(piter);
} // CLKRHashTable::InitializeIterator



//------------------------------------------------------------------------
// Function: CLKRLinearHashTable::IncrementIterator
// Synopsis: move the iterator on to the next record in the hash subtable
//------------------------------------------------------------------------

LK_RETCODE
CLKRLinearHashTable::IncrementIterator(
    CIterator* piter)
{
    if (!IsUsable())
        return LK_UNUSABLE;

    IRTLASSERT(piter != NULL);
    IRTLASSERT(piter->m_plht == this);
    IRTLASSERT(piter->m_lkl == LKL_WRITELOCK
               ?  this->IsWriteLocked()
               :  this->IsReadLocked());
    IRTLASSERT(piter->m_dwBucketAddr < m_cActiveBuckets);
    IRTLASSERT(piter->m_pnc != NULL);
    IRTLASSERT((0 <= piter->m_iNode  &&  piter->m_iNode < _NodesPerClump())
               || (_NodeBegin() - _NodeStep() == piter->m_iNode));

    if (piter == NULL  ||  piter->m_plht != this)
        return LK_BAD_ITERATOR;

    const void* pvRecord = NULL;

    if (piter->m_iNode != _NodeBegin() - _NodeStep())
    {
        // Release the reference acquired in the previous call to
        // IncrementIterator
        pvRecord = piter->m_pnc->m_pvNode[piter->m_iNode];
        _AddRefRecord(pvRecord, LKAR_ITER_RELEASE);
    }

    do
    {
        do
        {
            // find the next slot in the nodeclump that's in use
            while ((piter->m_iNode += _NodeStep())  !=  _NodeEnd())
            {
                pvRecord = piter->m_pnc->m_pvNode[piter->m_iNode];

                if (piter->m_pnc->m_dwKeySigs[piter->m_iNode]
                        != HASH_INVALID_SIGNATURE)
                {
                    // Add a new reference
                    _AddRefRecord(pvRecord, LKAR_ITER_ACQUIRE);
                    return LK_SUCCESS;
                }
                else
                {
                    // Check that all the remaining nodes are empty
                    IRTLASSERT(piter->m_pnc->NoMoreValidSlots(piter->m_iNode));
                    break; // rest of nodeclump is empty
                }
            }

            // try the next nodeclump in the bucket chain
            piter->m_iNode = _NodeBegin() - _NodeStep();
            piter->m_pnc = piter->m_pnc->m_pncNext;
        } while (piter->m_pnc != NULL);

        // Exhausted this bucket chain.  Unlock it.
        PBucket pbkt = _BucketFromAddress(piter->m_dwBucketAddr);
        IRTLASSERT(pbkt != NULL);

        if (_UseBucketLocking())
        {
            IRTLASSERT(piter->m_lkl == LKL_WRITELOCK
                       ?  pbkt->IsWriteLocked()
                       :  pbkt->IsReadLocked());
        
            if (piter->m_lkl == LKL_WRITELOCK)
                pbkt->WriteUnlock();
            else
                pbkt->ReadUnlock();
        }

        // Try the next bucket, if there is one
        if (++piter->m_dwBucketAddr < m_cActiveBuckets)
        {
            pbkt = _BucketFromAddress(piter->m_dwBucketAddr);
            IRTLASSERT(pbkt != NULL);

            if (_UseBucketLocking())
            {
                if (piter->m_lkl == LKL_WRITELOCK)
                    pbkt->WriteLock();
                else
                    pbkt->ReadLock();
            }
            
            piter->m_pnc = &pbkt->m_ncFirst;
        }
    } while (piter->m_dwBucketAddr < m_cActiveBuckets);

    // We have fallen off the end of the hashtable
    piter->m_iNode = _NodeBegin() - _NodeStep();
    piter->m_pnc = NULL;

    return LK_NO_MORE_ELEMENTS;
} // CLKRLinearHashTable::IncrementIterator



//------------------------------------------------------------------------
// Function: CLKRHashTable::IncrementIterator
// Synopsis: move the iterator on to the next record in the hash table
//------------------------------------------------------------------------

LK_RETCODE
CLKRHashTable::IncrementIterator(
    CIterator* piter)
{
    if (!IsUsable())
        return LK_UNUSABLE;

    IRTLASSERT(piter != NULL);
    IRTLASSERT(piter->m_pht == this);
    IRTLASSERT(-1 <= piter->m_ist
               &&  piter->m_ist < static_cast<int>(m_cSubTables));

    if (piter == NULL  ||  piter->m_pht != this)
        return LK_BAD_ITERATOR;

    // Table is already locked
    if (!IsValid())
        return LK_UNUSABLE;

    LK_RETCODE lkrc;
    CLHTIterator* pBaseIter = static_cast<CLHTIterator*>(piter);

    for (;;)
    {
        // Do we have a valid iterator into a subtable?  If not, get one.
        while (piter->m_plht == NULL)
        {
            while (++piter->m_ist < static_cast<int>(m_cSubTables))
            {
                lkrc = m_palhtDir[piter->m_ist]->_InitializeIterator(piter);
                if (lkrc == LK_SUCCESS)
                {
                    IRTLASSERT(m_palhtDir[piter->m_ist] == piter->m_plht);
                    return lkrc;
                }
                else if (lkrc == LK_NO_MORE_ELEMENTS)
                    lkrc = piter->m_plht->_CloseIterator(pBaseIter);

                if (lkrc != LK_SUCCESS)
                    return lkrc;
            }

            // There are no more subtables left.
            return LK_NO_MORE_ELEMENTS;
        }

        // We already have a valid iterator into a subtable.  Increment it.
        lkrc = piter->m_plht->IncrementIterator(pBaseIter);
        if (lkrc == LK_SUCCESS)
            return lkrc;

        // We've exhausted that subtable.  Move on.
        if (lkrc == LK_NO_MORE_ELEMENTS)
            lkrc = piter->m_plht->_CloseIterator(pBaseIter);

        if (lkrc != LK_SUCCESS)
            return lkrc;
    }
} // CLKRHashTable::IncrementIterator



//------------------------------------------------------------------------
// Function: CLKRLinearHashTable::_CloseIterator
// Synopsis: release the resources held by the iterator
//------------------------------------------------------------------------

LK_RETCODE
CLKRLinearHashTable::_CloseIterator(
    CIterator* piter)
{
    if (!IsUsable())
        return LK_UNUSABLE;

    IRTLASSERT(piter != NULL);
    IRTLASSERT(piter->m_plht == this);
    IRTLASSERT(piter->m_lkl == LKL_WRITELOCK
               ?  this->IsWriteLocked()
               :  this->IsReadLocked());
    IRTLASSERT(piter->m_dwBucketAddr <= m_cActiveBuckets);
    IRTLASSERT((0 <= piter->m_iNode  &&  piter->m_iNode < _NodesPerClump())
               || (_NodeBegin() - _NodeStep() == piter->m_iNode));

    if (piter == NULL  ||  piter->m_plht != this)
        return LK_BAD_ITERATOR;

    // Are we abandoning the iterator before the end of the subtable?
    // If so, need to unlock the bucket.
    if (piter->m_dwBucketAddr < m_cActiveBuckets)
    {
        PBucket pbkt = _BucketFromAddress(piter->m_dwBucketAddr);
        IRTLASSERT(pbkt != NULL);

        if (_UseBucketLocking())
        {
            IRTLASSERT(piter->m_lkl == LKL_WRITELOCK
                       ?  pbkt->IsWriteLocked()
                       :  pbkt->IsReadLocked());
        }
        
        if (0 <= piter->m_iNode  &&  piter->m_iNode < _NodesPerClump())
        {
            IRTLASSERT(piter->m_pnc != NULL);
            const void* pvRecord = piter->m_pnc->m_pvNode[piter->m_iNode];
            _AddRefRecord(pvRecord, LKAR_ITER_CLOSE);
        }

        if (_UseBucketLocking())
        {
            if (piter->m_lkl == LKL_WRITELOCK)
                pbkt->WriteUnlock();
            else
                pbkt->ReadUnlock();
        }
    }

    piter->m_plht = NULL;
    piter->m_pnc  = NULL;

    return LK_SUCCESS;
} // CLKRLinearHashTable::_CloseIterator



//------------------------------------------------------------------------
// Function: CLKRHashTable::CloseIterator
// Synopsis: release the resources held by the iterator
//------------------------------------------------------------------------

LK_RETCODE
CLKRHashTable::CloseIterator(
    CIterator* piter)
{
    if (!IsUsable())
        return LK_UNUSABLE;

    IRTLASSERT(piter != NULL);
    IRTLASSERT(piter->m_pht == this);
    IRTLASSERT(-1 <= piter->m_ist
               &&  piter->m_ist <= static_cast<int>(m_cSubTables));

    if (piter == NULL  ||  piter->m_pht != this)
        return LK_BAD_ITERATOR;

    LK_RETCODE lkrc = LK_SUCCESS;

    if (!IsValid())
        lkrc = LK_UNUSABLE;
    else
    {
        // Are we abandoning the iterator before we've reached the end?
        // If so, close the subtable iterator.
        if (piter->m_plht != NULL)
        {
            IRTLASSERT(piter->m_ist < static_cast<int>(m_cSubTables));
            CLHTIterator* pBaseIter = static_cast<CLHTIterator*>(piter);
            piter->m_plht->_CloseIterator(pBaseIter);
        }
    }

    // Unlock all the subtables
    if (piter->m_lkl == LKL_WRITELOCK)
        this->WriteUnlock();
    else
        this->ReadUnlock();

    piter->m_plht = NULL;
    piter->m_pht  = NULL;
    piter->m_ist  = -1;

    return lkrc;
} // CLKRHashTable::CloseIterator

#endif // LKR_DEPRECATED_ITERATORS


#ifndef __LKRHASH_NO_NAMESPACE__
};
#endif // !__LKRHASH_NO_NAMESPACE__
