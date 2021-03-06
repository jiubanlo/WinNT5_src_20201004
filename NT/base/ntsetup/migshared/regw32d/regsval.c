//
//  REGSVAL.C
//
//  Copyright (C) Microsoft Corporation, 1995
//
//  Implementation of RegSetValue, RegSetValueEx and supporting functions.
//

#include "pch.h"

//
//  RgReAllocKeyRecord
//

int
INTERNAL
RgReAllocKeyRecord(
    HKEY hKey,
    DWORD Length,
    LPKEY_RECORD FAR* lplpKeyRecord
    )
{

    int ErrorCode;
    LPKEY_RECORD lpOldKeyRecord;
    UINT BlockIndex;
    UINT KeyRecordIndex;
    LPDATABLOCK_INFO lpOldDatablockInfo;
    LPKEYNODE lpKeynode;

    if (Length > MAXIMUM_KEY_RECORD_SIZE) {
        return ERROR_BIGKEY_NEEDED;         // A big key is required
    }

    lpOldKeyRecord = *lplpKeyRecord;

    BlockIndex = HIWORD(lpOldKeyRecord-> DatablockAddress);
    KeyRecordIndex = LOWORD(lpOldKeyRecord-> DatablockAddress);

    //
    //  Check if we can simply extend this key record by taking space from an
    //  adjacent free record.
    //

    if (RgExtendKeyRecord(hKey-> lpFileInfo, BlockIndex, (UINT) Length,
        lpOldKeyRecord) == ERROR_SUCCESS)
        return ERROR_SUCCESS;

    //
    //  Check if there's enough space in the datablock lpCurrKeyRecord is in to
    //  contain a key record of the specified size.  If so, then we don't have
    //  to dirty the keynode.
    //

    if (RgAllocKeyRecordFromDatablock(hKey-> lpFileInfo, BlockIndex,
        (UINT) Length, lplpKeyRecord) == ERROR_SUCCESS) {

        //  After an alloc, we must refetch these pointers because they may be
        //  invalid.
        lpOldDatablockInfo = RgIndexDatablockInfoPtr(hKey-> lpFileInfo,
            BlockIndex);
        lpOldKeyRecord = RgIndexKeyRecordPtr(lpOldDatablockInfo,
            KeyRecordIndex);

        //  Transfer all the data to the new record, except for the allocated
        //  size which is already correctly set.
        MoveMemory(&(*lplpKeyRecord)-> DatablockAddress, &lpOldKeyRecord->
            DatablockAddress, SmallDword(lpOldKeyRecord-> RecordSize) -
            sizeof(DWORD));

        RgFreeKeyRecord(lpOldDatablockInfo, lpOldKeyRecord);

        //  Update the key record table to point to the new key record.
        lpOldDatablockInfo-> lpKeyRecordTable[KeyRecordIndex] =
            (KEY_RECORD_TABLE_ENTRY) ((LPBYTE) (*lplpKeyRecord) -
            (LPBYTE) lpOldDatablockInfo-> lpDatablockHeader);

        return ERROR_SUCCESS;

    }

    //
    //  Check if we can allocate a key record from another datablock.  If so,
    //  then copy the key to the other datablock and update the keynode.
    //

    if (RgLockInUseKeynode(hKey-> lpFileInfo, hKey-> KeynodeIndex,
        &lpKeynode) == ERROR_SUCCESS) {

        if ((ErrorCode = RgAllocKeyRecord(hKey-> lpFileInfo, (UINT) Length,
            lplpKeyRecord)) == ERROR_SUCCESS) {

            //  After an alloc, we must refetch these pointers because they may
            //  be invalid.
            lpOldDatablockInfo = RgIndexDatablockInfoPtr(hKey-> lpFileInfo,
                BlockIndex);
            lpOldKeyRecord = RgIndexKeyRecordPtr(lpOldDatablockInfo,
                KeyRecordIndex);

            //  Transfer all the data to the new record, except for the
            //  allocated size which is already correctly set.
            MoveMemory(&(*lplpKeyRecord)-> RecordSize, &lpOldKeyRecord->
                RecordSize, SmallDword(lpOldKeyRecord-> RecordSize) -
                (sizeof(DWORD) * 2));

            RgFreeKeyRecord(lpOldDatablockInfo, lpOldKeyRecord);
            RgFreeKeyRecordIndex(lpOldDatablockInfo, KeyRecordIndex);

            //  Unlock the old datablock.
            RgUnlockDatablock(hKey-> lpFileInfo, BlockIndex, TRUE);

            //  Update the open key and keynode to point to the key record in
            //  the new datablock.
            hKey-> BlockIndex = (*lplpKeyRecord)-> BlockIndex;
            hKey-> BigKeyLockedBlockIndex = hKey-> BlockIndex;
            lpKeynode-> BlockIndex = hKey-> BlockIndex;
            hKey-> KeyRecordIndex = (BYTE) (*lplpKeyRecord)-> KeyRecordIndex;
            lpKeynode-> KeyRecordIndex = hKey-> KeyRecordIndex;

        }

        RgUnlockKeynode(hKey-> lpFileInfo, hKey-> KeynodeIndex, TRUE);

        return ErrorCode;

    }

    return ERROR_OUTOFMEMORY;

}


//
//  RgSetValue
//  (BIGKEY aware)
//

int
INTERNAL
RgSetValue(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Type,
    LPBYTE lpData,
    UINT cbData
    )
{
    int ErrorCode;
    HKEY hKeyExtent;
    UINT Index;
    LPSTR ExtentKeyName;
    DWORD cbExtentKeyName;
    WORD NameID = 1;
    WORD MaxNameID = 0;
    LPKEY_RECORD lpKeyRecord;
    LPVALUE_RECORD lpValueRecord;
    LPKEYNODE lpKeynode;
    BOOL fTryRoot = FALSE;


    ErrorCode = RgSetValueStd(hKey, lpValueName, Type, lpData, cbData, FALSE);

    if (ErrorCode == ERROR_BIGKEY_NEEDED)
    {
        //
        // Couldn't fit the value in the key, make it a big key
        // (if it isn't one already)
        //


        // First delete its old value if it exists
        ErrorCode = RgLookupValueByName(hKey, lpValueName, &lpKeyRecord,
            &lpValueRecord);

        if (ErrorCode == ERROR_SUCCESS)
        {
            // If a value record already existed, and it was not in the root of the big key
            // then we should try inserting the new value record into the root, after deleting
            // it from its old location.
            if (hKey-> BigKeyLockedBlockIndex != hKey-> BlockIndex)
                fTryRoot = TRUE;

            RgDeleteValueRecord(lpKeyRecord, lpValueRecord);
            RgUnlockDatablock(hKey-> lpFileInfo, hKey-> BigKeyLockedBlockIndex, TRUE);
        }
        else if (ErrorCode != ERROR_CANTREAD16_FILENOTFOUND32) {
            return ERROR_OUTOFMEMORY;
        }

        if (IsNullPtr(ExtentKeyName = RgSmAllocMemory(MAXIMUM_SUB_KEY_LENGTH)))
            return ERROR_OUTOFMEMORY;

        // Second, search for room in each of the big key's extents
        // (we should never mark the root with an LK_BIGKEYEXT, otherwise it won't be found
        // by RgLookupKey and RgLookupKeyByIndex)
        if ((hKey-> Flags & KEYF_BIGKEYROOT)) {
            if (fTryRoot) {
                // This happens if the value record previously existed in a big key extension,
                // but the new value record doesn't fit in the same extension, so we want to try
                // the root of the big key.
                if ((ErrorCode = RgSetValueStd(hKey, lpValueName, Type, lpData, cbData, TRUE)) ==
                    ERROR_SUCCESS) {
                    goto lFreeKeyName;
                }
            }

            Index = 0;
        
            do {
                cbExtentKeyName = MAXIMUM_SUB_KEY_LENGTH;
                if (RgLookupKeyByIndex(hKey, Index++, ExtentKeyName, &cbExtentKeyName, LK_BIGKEYEXT) !=
                    ERROR_SUCCESS) {
                    goto lGrowKey;
                }

                NameID = RgAtoW(ExtentKeyName);

                if (NameID > MaxNameID)
                    MaxNameID = NameID;

                if (RgLookupKey(hKey, ExtentKeyName, &hKeyExtent, LK_OPEN | LK_BIGKEYEXT) != ERROR_SUCCESS) {
                    goto lGrowKey;
                }

                ErrorCode = RgSetValueStd(hKeyExtent, lpValueName, Type, lpData, cbData, TRUE);

                RgDestroyKeyHandle(hKeyExtent);

            } while (ErrorCode == ERROR_BIGKEY_NEEDED);

            goto lFreeKeyName;
        }

        // Third, make it a big key, or if it is a big key, then grow it
lGrowKey:
        // Create a unique name for the big key extent
        if (MaxNameID)
            NameID = MaxNameID + 1;

        RgWtoA(NameID, ExtentKeyName);

        if ((ErrorCode = RgLookupKey(hKey, ExtentKeyName, &hKeyExtent, LK_CREATE)) ==
            ERROR_SUCCESS) {

            // Mark the parent as the big key root, if it isn't already
            if (!(hKey-> Flags & KEYF_BIGKEYROOT))
            {
                if ((ErrorCode = RgLockInUseKeynode(hKey-> lpFileInfo, hKey-> KeynodeIndex,
                    &lpKeynode)) != ERROR_SUCCESS)
                    goto lFreeKeyName;

                lpKeynode-> Flags |= KNF_BIGKEYROOT;
                hKey-> Flags |= KEYF_BIGKEYROOT;

                RgUnlockKeynode(hKey-> lpFileInfo, hKey-> KeynodeIndex, TRUE);
            }

            // Mark the new key as a big key extent
            if ((ErrorCode = RgLockInUseKeynode(hKeyExtent-> lpFileInfo, hKeyExtent-> KeynodeIndex,
                &lpKeynode)) != ERROR_SUCCESS)
                goto lFreeKeyName;

            lpKeynode-> Flags |= KNF_BIGKEYEXT;
            
            RgUnlockKeynode(hKeyExtent-> lpFileInfo, hKeyExtent-> KeynodeIndex, TRUE);

            // Now add the value record to the new key
            ErrorCode = RgSetValueStd(hKeyExtent, lpValueName, Type, lpData, cbData, TRUE);

            ASSERT(ErrorCode != ERROR_BIGKEY_NEEDED);
            RgDestroyKeyHandle(hKeyExtent);
        }

lFreeKeyName:
        RgSmFreeMemory(ExtentKeyName);

        if (ErrorCode == ERROR_BIGKEY_NEEDED)
            ErrorCode = ERROR_OUTOFMEMORY;
    }

    return ErrorCode;
}


//
//  RgSetValueStd
//

int
INTERNAL
RgSetValueStd(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Type,
    LPBYTE lpData,
    UINT cbData,
    BOOL fBigKeyExtent
    )
{

    int ErrorCode;
    UINT ValueNameLength;
    UINT NewValueRecordLength;
    LPKEY_RECORD lpKeyRecord;
    LPVALUE_RECORD lpValueRecord;
    UINT CurrentValueRecordLength;
    LPBYTE lpDestination;
    UINT BytesToExtend;
    UINT TempCount;
    LPKEYNODE lpKeynode;

    ValueNameLength = (IsNullPtr(lpValueName) ? 0 : (UINT) StrLen(lpValueName));

    if (ValueNameLength > MAXIMUM_VALUE_NAME_LENGTH - 1)
        return ERROR_INVALID_PARAMETER;

    NewValueRecordLength = sizeof(VALUE_RECORD) + ValueNameLength + cbData - 1;

    if (!fBigKeyExtent) {
        ErrorCode = RgLookupValueByName(hKey, lpValueName, &lpKeyRecord,
            &lpValueRecord);
    }
    else {
        // If we didn't find it searching from the root of a bigkey, then we won't
        // find it beginning from an extent.
        ErrorCode = ERROR_CANTREAD16_FILENOTFOUND32;
    }

    //
    //  A value with this name already exists, so update the existing
    //  VALUE_RECORD with the new information.
    //

    if (ErrorCode == ERROR_SUCCESS) {

        CurrentValueRecordLength = sizeof(VALUE_RECORD) + lpValueRecord->
            NameLength + lpValueRecord-> DataLength - 1;

        // Is the value record staying the same?
        if (NewValueRecordLength == CurrentValueRecordLength) {
            if (lpValueRecord-> DataLength == cbData && lpValueRecord->
                DataType == Type && CompareMemory((LPBYTE) lpValueRecord->
                Name + ValueNameLength, lpData, cbData) == 0) {
                RgUnlockDatablock(hKey-> lpFileInfo, hKey-> BigKeyLockedBlockIndex, FALSE);
                return ERROR_SUCCESS;
            }
        }

        // Is the value record shrinking?
        if (NewValueRecordLength < CurrentValueRecordLength) {
            lpKeyRecord-> RecordSize -= (CurrentValueRecordLength -
                NewValueRecordLength);
        }

        // Is the value record growing?
        else if (NewValueRecordLength > CurrentValueRecordLength) {

            BytesToExtend = NewValueRecordLength - CurrentValueRecordLength;

            // Does the value record fit in the allocated key size?
            if (BytesToExtend > SmallDword(lpKeyRecord-> AllocatedSize) -
                SmallDword(lpKeyRecord-> RecordSize)) {

                TempCount = (LPBYTE) lpValueRecord - (LPBYTE) lpKeyRecord;

                // Grow the key record
                if ((ErrorCode = RgReAllocKeyRecord(hKey, lpKeyRecord->
                    RecordSize + BytesToExtend, &lpKeyRecord)) !=
                    ERROR_SUCCESS) {
                    RgUnlockDatablock(hKey-> lpFileInfo, hKey-> BigKeyLockedBlockIndex,
                        FALSE);
                    return ErrorCode;
                }

                lpValueRecord = (LPVALUE_RECORD) ((LPBYTE) lpKeyRecord +
                    TempCount);

            }

            lpKeyRecord-> RecordSize += BytesToExtend;

        }

        lpDestination = (LPBYTE) lpValueRecord + NewValueRecordLength;
        TempCount = (UINT) ((LPBYTE) lpKeyRecord + SmallDword(lpKeyRecord->
            RecordSize) - lpDestination);

        if (TempCount > 0) {
            MoveMemory(lpDestination, (LPBYTE) lpValueRecord +
                CurrentValueRecordLength, TempCount);
        }

    }

    //
    //  No value exists with this name.  Place a new VALUE_RECORD at the end of
    //  the KEY_RECORD.
    //

    else if (ErrorCode == ERROR_CANTREAD16_FILENOTFOUND32) {

        //  Handle Win95 registries that don't have a key record for the root
        //  key.  We don't check if this is really the root key, but it doesn't
        //  matter much.
        if (IsNullBlockIndex(hKey-> BlockIndex)) {

            if (RgLockInUseKeynode(hKey-> lpFileInfo, hKey-> KeynodeIndex,
                &lpKeynode) != ERROR_SUCCESS)
                goto LockKeynodeFailed;

            if (RgAllocKeyRecord(hKey-> lpFileInfo, sizeof(KEY_RECORD) +
                NewValueRecordLength, &lpKeyRecord) != ERROR_SUCCESS) {
                RgUnlockKeynode(hKey-> lpFileInfo, hKey-> KeynodeIndex, FALSE);
LockKeynodeFailed:
                TRAP();
                return ERROR_CANTOPEN;          //  Win95 compatibility
            }

            lpKeyRecord-> RecordSize = sizeof(KEY_RECORD);
            lpKeyRecord-> NameLength = 1;       //  Win95 compatibility
            lpKeyRecord-> Name[0] = '\0';       //  Win95 compatibility
            lpKeyRecord-> ValueCount = 0;
            lpKeyRecord-> ClassLength = 0;
            lpKeyRecord-> Reserved = 0;

            lpKeynode-> BlockIndex = lpKeyRecord-> BlockIndex;
            lpKeynode-> KeyRecordIndex = lpKeyRecord-> KeyRecordIndex;

            hKey-> BlockIndex = (WORD) lpKeynode-> BlockIndex;
            hKey-> KeyRecordIndex = (BYTE) lpKeynode-> KeyRecordIndex;

            RgUnlockKeynode(hKey-> lpFileInfo, hKey-> KeynodeIndex, TRUE);

            ErrorCode = ERROR_SUCCESS;
            goto AddValueRecord;

        }

        if ((ErrorCode = RgLockKeyRecord(hKey-> lpFileInfo, hKey-> BlockIndex,
            hKey-> KeyRecordIndex, &lpKeyRecord)) == ERROR_SUCCESS) {

            if (NewValueRecordLength > SmallDword(lpKeyRecord-> AllocatedSize) -
                SmallDword(lpKeyRecord-> RecordSize)) {

                if ((ErrorCode = RgReAllocKeyRecord(hKey, lpKeyRecord->
                    RecordSize + NewValueRecordLength, &lpKeyRecord)) !=
                    ERROR_SUCCESS) {
                    RgUnlockDatablock(hKey-> lpFileInfo, hKey-> BlockIndex,
                        FALSE);
                    return ErrorCode;
                }

            }

AddValueRecord:
            hKey-> BigKeyLockedBlockIndex = hKey-> BlockIndex;
            lpValueRecord = (LPVALUE_RECORD) ((LPBYTE) lpKeyRecord +
                SmallDword(lpKeyRecord-> RecordSize));
            lpKeyRecord-> RecordSize += NewValueRecordLength;
            lpKeyRecord-> ValueCount++;

        }

    }

    //
    //  If we're successful at this point, then lpValueRecord is valid and we
    //  should copy the data into this record.
    //

    if (ErrorCode == ERROR_SUCCESS) {

        lpValueRecord-> DataType = Type;

        lpValueRecord-> NameLength = (WORD) ValueNameLength;
        MoveMemory(lpValueRecord-> Name, lpValueName, ValueNameLength);

        lpValueRecord-> DataLength = (WORD) cbData;
        MoveMemory((LPBYTE) lpValueRecord-> Name + ValueNameLength, lpData,
            cbData);

        RgUnlockDatablock(hKey-> lpFileInfo, hKey-> BigKeyLockedBlockIndex, TRUE);

    }

    return ErrorCode;

}

//
//  VMMRegSetValueEx
//
//  See Win32 documentation of RegSetValueEx.
//

LONG
REGAPI
VMMRegSetValueEx(
    HKEY hKey,
    LPCSTR lpValueName,
    DWORD Reserved,
    DWORD Type,
    LPBYTE lpData,
    DWORD cbData
    )
{

    int ErrorCode;

    if (IsBadOptionalStringPtr(lpValueName, (UINT) -1))
        return ERROR_INVALID_PARAMETER;

    //
    //  bad Windows 95 compatibility problem.  If the type is REG_SZ,
    //  then override cbData with the length of the string pointed to by lpData.
    //  This should have only been done in RegSetValue, but we're stuck with it
    //  now...
    //

    if (Type == REG_SZ) {
        if (IsBadStringPtr(lpData, (UINT) -1))
            return ERROR_INVALID_PARAMETER;
        cbData = StrLen(lpData);

        // Must leave room for the null terminator
        if (cbData >= MAXIMUM_DATA_LENGTH)
                return ERROR_INVALID_PARAMETER;
    }
    else {
        if (cbData > 0 && IsBadHugeReadPtr(lpData, cbData))
            return ERROR_INVALID_PARAMETER;
    }

    if (cbData > MAXIMUM_DATA_LENGTH)
        return ERROR_INVALID_PARAMETER;

    if (!RgLockRegistry())
        return ERROR_LOCK_FAILED;

    if ((ErrorCode = RgValidateAndConvertKeyHandle(&hKey)) == ERROR_SUCCESS) {
        if (IsDynDataKey(hKey) || (hKey-> lpFileInfo-> Flags & FI_READONLY))
            ErrorCode = ERROR_ACCESS_DENIED;
        else {
            if ((ErrorCode = RgSetValue(hKey, lpValueName, Type, lpData,
                (UINT) cbData)) == ERROR_SUCCESS) {
                RgSignalWaitingNotifies(hKey-> lpFileInfo, hKey-> KeynodeIndex,
                    REG_NOTIFY_CHANGE_LAST_SET);
            }
        }
    }

    RgUnlockRegistry();

    return ErrorCode;

    UNREFERENCED_PARAMETER(Reserved);

}

//
//  VMMRegSetValue
//
//  See Win32 documentation of RegSetValue.
//

LONG
REGAPI
VMMRegSetValue(
    HKEY hKey,
    LPCSTR lpSubKey,
    DWORD Type,
    LPBYTE lpData,
    DWORD cbData
    )
{

    LONG ErrorCode;
    HKEY hSubKey;

    if ((ErrorCode = RgCreateOrOpenKey(hKey, lpSubKey, &hSubKey, LK_CREATE)) ==
        ERROR_SUCCESS) {
        ErrorCode = VMMRegSetValueEx(hSubKey, NULL, 0, REG_SZ, lpData, 0);
        VMMRegCloseKey(hSubKey);
    }

    return ErrorCode;

    UNREFERENCED_PARAMETER(Type);
    UNREFERENCED_PARAMETER(cbData);

}
