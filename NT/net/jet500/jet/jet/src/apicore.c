/***********************************************************************
* Microsoft Jet
*
* Microsoft Confidential.  Copyright 1991-1992 Microsoft Corporation.
*
* Component:
*
* File: apicore.c
*
* File Comments:
*
* Revision History:
*
*    [0]  09-Sep-91  richards	Split from isamapi.c
*
***********************************************************************/

#include "std.h"

#include "jetord.h"
#include "_jetstr.h"		       /* global strings */

#include "vdbmgr.h"		       /* database id manager prototypes */
#include "vtmgr.h"		       /* table id manager prototypes */
#include "_vtmgr.h"		       /* table id manager prototypes */

#include "isammgr.h"		       /* installable ISAMs */

#include <stdlib.h>
#include <string.h>

ERR VTAPI ErrIsamSetColumn( JET_VSESID ppib, JET_VTID pfucb, unsigned long ulFieldId,
	char *pbData, unsigned long cbData, JET_GRBIT grbit, JET_SETINFO *psetinfo );

ERR VTAPI ErrIsamRetrieveColumn( JET_VSESID ppib, JET_VTID pfucb, unsigned long ulFieldId,
	char *pbData, unsigned long cbDataMax, unsigned long *pcbDataActual,
	JET_GRBIT grbit, JET_RETINFO *pretinfo );

ERR VTAPI ErrIsamSetColumns(
	JET_VSESID		ppib,
	JET_VTID			pfucb,
	JET_SETCOLUMN	*psetcols,
	unsigned long	csetcols );

ERR VTAPI ErrIsamRetrieveColumns(
	JET_VSESID					ppib,
	JET_VTID						pfucb,
	JET_RETRIEVECOLUMN 	*pretcols,
	unsigned long				cretcols );

DeclAssertFile;


/* C6BUG: Remove this when the compiler can handle C functions in plmf */

STATIC void NEAR StrNCpy(char __far *szDest, char __far *szSrc,size_t cb)
	{
	(void) strncpy(szDest, szSrc, cb);
	}


/***********************************************************************/
/***********************  JET API FUNCTIONS  ***************************/
/***********************************************************************/


	/* The following pragma affects the code generated by the C */
	/* compiler for all FAR functions.  Do NOT place any non-API */
	/* functions beyond this point in this file. */

/* NOTE: This will be the shipping version of JetIdle */
/*=================================================================
JetIdle

Description:
  Performs idle time processing.

Parameters:
  sesid			uniquely identifies session
  grbit			processing options

Return Value:
  Error code

Errors/Warnings:
  JET_errSuccess		some idle processing occurred
  JET_wrnNoIdleActivity no idle processing occurred
=================================================================*/

JET_ERR JET_API JetIdle(JET_SESID sesid, JET_GRBIT grbit)
	{
	ERR	err;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	/* Let the built-in ISAM do some idle processing */
	err = ErrIsamIdle(sesid, grbit);

	APIReturn(err);

	/* CONSIDER: This API cannot fail.  Any errors generated should be */
	/* CONSIDER: suppressed and reported by a later API call. */
	}


/*=================================================================
JetGetLastErrorInfo

Description:
  Returns extended error info to the user.

Parameters:
  sesid			uniquely identifies session
  pexterr		pointer to JET_EXTERR structure (NULL if not desired)
  cbexterrMax	size of buffer pointed to by pexterr
  sz1			pointer to buffer for error string #1 (NULL if not desired)
  cch1Max		size of buffer pointed to by sz1
  sz2			pointer to buffer for error string #2 (NULL if not desired)
  cch2Max		size of buffer pointed to by sz2

Return Value:
  JET error code

Errors/Warnings:
  JET_errSuccess		if error info was retrieved.
  JET_wrnNoErrorInfo	if there was no error info to retrieve.  In this case,
					    none of the output parameters are filled in.
=================================================================*/
JET_ERR JET_API JetGetLastErrorInfo(JET_SESID sesid,
	JET_EXTERR __far *pexterr, unsigned long cbexterrMax,
	char __far *sz1, unsigned long cch1Max,
	char __far *sz2, unsigned long cch2Max,
	char __far *sz3, unsigned long cch3Max,
	unsigned long __far *pcch3Actual)
	{
	int isib;
	SIB __near *psib;

	APIEnter();

	FillClientBuffer(pexterr, cbexterrMax);
	FillClientBuffer(sz1, cch1Max);
	FillClientBuffer(sz2, cch2Max);
	FillClientBuffer(sz3, cch3Max);

	/*** Get SIB for this session ***/
	if ((isib = UtilGetIsibOfSesid(sesid)) == -1)
		APIReturn(JET_errInvalidSesid);

	psib = rgsib + isib;

	/*** No error info?  Return ***/
	if (psib->exterr.err == JET_wrnNoErrorInfo)
		{
		pexterr->cbStruct = 0;
		APIReturn(JET_wrnNoErrorInfo);
		}

	/*** Fill in return values ***/
	if (pexterr != NULL && cbexterrMax >= sizeof(JET_EXTERR))
		{
		pexterr->cbStruct = sizeof(JET_EXTERR);
		pexterr->err = psib->exterr.err;
		pexterr->ul1 = psib->exterr.ul1;
		pexterr->ul2 = psib->exterr.ul2;
		pexterr->ul3 = psib->exterr.ul3;
		}

	if (sz1 != NULL && cch1Max > 0)
		{
		if (psib->sz1 == NULL)
			*sz1 = '\0';
		else
			{
			StrNCpy(sz1, psib->sz1, (size_t)cch1Max);
			sz1[cch1Max-1] = '\0';
			}
		}

	if (sz2 != NULL && cch2Max > 0)
		{
		if (psib->sz2 == NULL)
			*sz2 = '\0';
		else
			{
			StrNCpy(sz2, psib->sz2, (size_t)cch2Max);
			sz2[cch2Max-1] = '\0';
			}
		}

	if (sz3 != NULL && cch3Max > 0)
		{
		if (psib->sz3 == NULL)
			*sz3 = '\0';
		else
			{
			StrNCpy(sz3, psib->sz3, (size_t)cch3Max);
			sz3[cch3Max-1] = '\0';
			if (pcch3Actual != NULL && (unsigned long)strlen(psib->sz3) > cch3Max-1)
				*pcch3Actual = strlen(psib->sz3);
			}
		}

	APIReturn(JET_errSuccess);
	}


JET_ERR JET_API JetGetTableIndexInfo(JET_SESID sesid, JET_TABLEID tableid,
	const char __far *szIndexName, void __far *pvResult,
	unsigned long cbResult, unsigned long InfoLevel)
	{
	ERR err;

	APIEnter();

	FillClientBuffer(pvResult, cbResult);

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	switch (InfoLevel)
		{
	case JET_IdxInfo :
	case JET_IdxInfoList :
		if (cbResult < sizeof(JET_INDEXLIST))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_IdxInfoSysTabCursor :
		if (cbResult < sizeof(JET_TABLEID))
			APIReturn(JET_errBufferTooSmall);
		break;
		}

	err = ErrDispGetTableIndexInfo(sesid, tableid, szIndexName, pvResult, cbResult, InfoLevel);
#ifdef DEBUG
	switch (InfoLevel)
		{
	case JET_IdxInfo :
	case JET_IdxInfoList :
		MarkTableidExported(err, ((JET_INDEXLIST*)pvResult)->tableid);
		break;
	case JET_IdxInfoSysTabCursor :
		MarkTableidExported(err, *(JET_TABLEID*)pvResult);
		break;
		}
#endif
	APIReturn(err);
	}


JET_ERR JET_API JetGetIndexInfo(JET_SESID sesid, JET_DBID dbid,
	const char __far *szTableName, const char __far *szIndexName,
	void __far *pvResult, unsigned long cbResult,
	unsigned long InfoLevel)
	{
	ERR	err;
	OLD_OUTDATA	outdata;       /* CONSIDER: OUTDATA */

	APIEnter();

	FillClientBuffer(pvResult, cbResult);

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	switch (InfoLevel)
		{
	case JET_IdxInfo :
	case JET_IdxInfoList :
		if (cbResult < sizeof(JET_INDEXLIST))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_IdxInfoSysTabCursor :
		if (cbResult < sizeof(JET_TABLEID))
			APIReturn(JET_errBufferTooSmall);
		break;
		}

	outdata.cbMax = cbResult;      /* CONSIDER: OUTDATA */
	outdata.pb    = pvResult;      /* CONSIDER: OUTDATA */

	err = ErrDispGetIndexInfo(sesid, dbid, szTableName, szIndexName,
		&outdata, InfoLevel);

#ifdef DEBUG
	switch (InfoLevel)
		{
	case JET_IdxInfo :
	case JET_IdxInfoList :
		MarkTableidExported(err, ((JET_INDEXLIST*)pvResult)->tableid);
		break;
	case JET_IdxInfoSysTabCursor :
		MarkTableidExported(err, *(JET_TABLEID*)pvResult);
		break;
		}
#endif
	APIReturn(err);
	}


JET_ERR JET_API JetGetObjectInfo(JET_SESID sesid, JET_DBID dbid,
	JET_OBJTYP objtyp, const char __far *szContainerName,
	const char __far *szObjectName, void __far *pvResult,
	unsigned long cbMax, unsigned long InfoLevel)
	{
	OLD_OUTDATA	outdata;       /* CONSIDER: OUTDATA */
	JET_ERR err;

	APIEnter();

	FillClientBuffer(pvResult, cbMax);

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	switch (InfoLevel)
		{
	case JET_ObjInfo :
	case JET_ObjInfoNoStats :
		if (cbMax < sizeof(JET_OBJECTINFO))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_ObjInfoListNoStats :
	case JET_ObjInfoList :
		if (cbMax < sizeof(JET_OBJECTLIST))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_ObjInfoSysTabCursor :
	case JET_ObjInfoSysTabReadOnly:
		if (cbMax < sizeof(JET_TABLEID))
			APIReturn(JET_errBufferTooSmall);
		break;
	default:
		APIReturn(JET_errInvalidParameter);
		}

	outdata.cbMax = cbMax;	       /* CONSIDER: OUTDATA */
	outdata.pb    = pvResult;      /* CONSIDER: OUTDATA */

	err = ErrDispGetObjectInfo(sesid, dbid, objtyp, szContainerName,
		szObjectName, &outdata, InfoLevel);
#ifdef DEBUG
	switch (InfoLevel)
		{
	case JET_ObjInfoListNoStats :
	case JET_ObjInfoList :
		MarkTableidExported(err, ((JET_OBJECTLIST*)pvResult)->tableid);
		break;
	case JET_ObjInfoSysTabCursor :
	case JET_ObjInfoSysTabReadOnly:
		MarkTableidExported(err, *(JET_TABLEID*)pvResult);
		break;
		}
#endif
	APIReturn(err);
	}


JET_ERR JET_API JetGetTableInfo(JET_SESID sesid, JET_TABLEID tableid,
	void __far *pvResult, unsigned long cbMax, unsigned long InfoLevel)
	{
	APIEnter();

	FillClientBuffer(pvResult, cbMax);

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	if (cbMax < sizeof(JET_OBJECTINFO))
		APIReturn(JET_errBufferTooSmall);

	CheckTableidExported(tableid);

	APIReturn(ErrDispGetTableInfo(sesid, tableid, pvResult, cbMax, InfoLevel));
	}


JET_ERR JET_API JetCreateObject(JET_SESID sesid, JET_DBID dbid,
	const char __far *szContainerName, const char __far *szObjectName,
	JET_OBJTYP objtyp)
	{
	OBJID objidParentId;
	OBJID objidNewObject;
	BOOL fContainerObj;
	ERR err;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

#ifdef	LATER
	if (szObjectName == NULL)
		{
		APIReturn(JET_errInvalidParameter);
		}
#endif	/* LATER */

	/*	validate szContainerName & szObjectName...
	*/
	fContainerObj = (szContainerName == NULL) || (szContainerName[0] == '\0');

	/*	deal with the container issue...
	*/
	if (fContainerObj)
		{
		/*	Container objects must have OBJTYP of JET_objtypContainer...
		*/
		if (objtyp != JET_objtypContainer)
			{
			APIReturn(JET_errInvalidParameter);
			}

		objidParentId = objidRoot;
		}
	else	/* !fContainerObj */
		{
		/*	Non-container objects cannot have OBJTYP < JET_objtypClientMin...
		*/
		if ((objtyp < JET_objtypClientMin) || (objtyp > 0xffff))
			{
			APIReturn(JET_errInvalidParameter);
			}

		/*	get the objid of the Container object (objidParentId)...
		*/
		if ((err = ErrDispGetObjidFromName(sesid, dbid, NULL,
			szContainerName, &objidParentId)) < 0)
			APIReturn(err);
		}

	/*	Create the object record in MSysObjects...
	*/
	if ((err = ErrDispCreateObject(sesid, dbid, objidParentId, szObjectName,
		objtyp)) < 0)
		APIReturn(err);

	/*	set object owner and propagate ACEs...
	*/
	for(;;) /* Dummy loop - break out on error */
		{
		if ((err = ErrDispGetObjidFromName(sesid, dbid,
	 	       szContainerName, szObjectName, &objidNewObject)) < 0)
		break;

		APIReturn(err);
		}

/*  Error Recovery Delete the newly created object */
	AssertGe(ErrDispDeleteObject(sesid, dbid, objidNewObject), 0);
	APIReturn(err);
	}


JET_ERR JET_API JetDeleteObject(JET_SESID sesid, JET_DBID dbid,
	const char __far *szContainerName, const char __far *szObjectName)
	{
	OBJID objidObject;
	ERR err;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	if ((err = ErrDispGetObjidFromName(sesid, dbid,
		szContainerName, szObjectName, &objidObject)) < 0)
		APIReturn(err);

	APIReturn(ErrDispDeleteObject(sesid, dbid, objidObject));
	}


JET_ERR JET_API JetRenameObject(JET_SESID sesid, JET_DBID dbid,
	const char __far *szContainerName, const char __far *szObjectName,
	const char __far *szObjectNew)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	APIReturn(ErrDispRenameObject(sesid, dbid, szContainerName, szObjectName, szObjectNew));
	}


#ifdef WIN32
JET_ERR JET_API JetStringCompare(char __far *pb1, unsigned long cb1,
	char __far *pb2, unsigned long cb2, unsigned long sort,
	long __far *plResult)
{
return -1;
}
#endif


JET_ERR JET_API JetBeginTransaction(JET_SESID sesid)
	{
	ERR	err = 0;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	err = ErrIsamBeginTransaction(sesid);

	APIReturn(err);
	}


JET_ERR JET_API JetCommitTransaction(JET_SESID sesid, JET_GRBIT grbit)
	{
	ERR	err = 0;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	if (grbit & ~JET_bitCommitFlush)
		APIReturn(JET_errInvalidParameter);

	err = ErrIsamCommitTransaction(sesid, grbit);

	APIReturn(err);
	}


JET_ERR JET_API JetRollback(JET_SESID sesid, JET_GRBIT grbit)
	{
	ERR	err = 0;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	if (grbit & ~JET_bitRollbackAll)
		APIReturn(JET_errInvalidParameter);

	err = ErrIsamRollback(sesid, grbit);

	APIReturn(err);
	}


JET_ERR JET_API JetOpenTable(JET_SESID sesid, JET_DBID dbid,
	const char __far *szTableName, const void __far *pvParameters,
	unsigned long cbParameters, JET_GRBIT grbit,
	JET_TABLEID __far *ptableid)
	{
	ERR err;

	APIEnter();

	if ( !FValidSesid(sesid) )
		APIReturn(JET_errInvalidSesid);

	err = ErrDispOpenTable(sesid, dbid, ptableid, szTableName, grbit);

// AllDone:
#ifdef DEBUG
	if (!(grbit & JET_bitTableBulk))
		MarkTableidExported(err, *ptableid);
#endif
	APIReturn(err);
	}


JET_ERR JET_API JetDupCursor(JET_SESID sesid, JET_TABLEID tableid,
	JET_TABLEID *ptableid, JET_GRBIT grbit)
	{
	ERR	err;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	err = ErrDispDupCursor(sesid, tableid, ptableid, grbit);

	MarkTableidExported(err, *ptableid);
	APIReturn(err);
	}


JET_ERR JET_API JetCloseTable(JET_SESID sesid, JET_TABLEID tableid)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispCloseTable(sesid, tableid));
	}


JET_ERR JET_API JetGetTableColumnInfo(JET_SESID sesid, JET_TABLEID tableid,
	const char __far *szColumnName, void __far *pvResult,
	unsigned long cbMax, unsigned long InfoLevel)
	{
	ERR err;

	APIEnter();

	FillClientBuffer(pvResult, cbMax);

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	switch (InfoLevel)
		{
	case JET_ColInfo :
		if (cbMax < sizeof(JET_COLUMNDEF))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_ColInfoList :
	case 2 :
		if (cbMax < sizeof(JET_COLUMNLIST))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_ColInfoSysTabCursor :
		if (cbMax < sizeof(JET_TABLEID))
			APIReturn(JET_errBufferTooSmall);
		break;

	case JET_ColInfoBase :
		if (cbMax < sizeof(JET_COLUMNBASE))
			APIReturn(JET_errBufferTooSmall);
		break;
		}

	err = ErrDispGetTableColumnInfo(sesid, tableid, szColumnName,
		pvResult, cbMax, InfoLevel);
#ifdef DEBUG
	switch (InfoLevel)
		{
	case JET_ColInfoList :
	case 2 :
		MarkTableidExported(err, ((JET_COLUMNLIST*)pvResult)->tableid);
		break;
	case JET_ColInfoSysTabCursor :
		MarkTableidExported(err, *(JET_TABLEID*)pvResult);
		break;
		}
#endif
	APIReturn(err);
	}


JET_ERR JET_API JetGetColumnInfo(JET_SESID sesid, JET_DBID dbid,
	const char __far *szTableName, const char __far *szColumnName,
	void __far *pvResult, unsigned long cbMax, unsigned long InfoLevel)
	{
	ERR	err;
	OLD_OUTDATA	outdata;       /* CONSIDER: OUTDATA */

	APIEnter();

	FillClientBuffer(pvResult, cbMax);

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	switch (InfoLevel)
		{
	case JET_ColInfo :
		if (cbMax < sizeof(JET_COLUMNDEF))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_ColInfoList :
	case 2 :
		if (cbMax < sizeof(JET_COLUMNLIST))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_ColInfoSysTabCursor :
		if (cbMax < sizeof(JET_TABLEID))
			APIReturn(JET_errBufferTooSmall);
		break;
	case JET_ColInfoBase :
		if (cbMax < sizeof(JET_COLUMNBASE))
			APIReturn(JET_errBufferTooSmall);
		break;
		}

	outdata.cbMax = cbMax;	       /* CONSIDER: OUTDATA */
	outdata.pb    = pvResult;      /* CONSIDER: OUTDATA */

	err = ErrDispGetColumnInfo(sesid, dbid, szTableName, szColumnName,
		&outdata, InfoLevel);

	Assert(err != JET_errSQLLinkNotSupported);

#ifdef DEBUG
	switch (InfoLevel)
		{
	case JET_ColInfoList :
	case 2 :
		MarkTableidExported(err, ((JET_COLUMNLIST*)pvResult)->tableid);
		break;
	case JET_ColInfoSysTabCursor :
		MarkTableidExported(err, *(JET_TABLEID*)pvResult);
		break;
		}
#endif
	APIReturn(err);
	}


JET_ERR JET_API JetRetrieveColumn(JET_SESID sesid, JET_TABLEID tableid,
	JET_COLUMNID columnid, void __far *pvData, unsigned long cbData,
	unsigned long __far *pcbActual, JET_GRBIT grbit,
	JET_RETINFO __far *pretinfo)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		{
		FillClientBuffer(pvData, cbData);

		APIReturn(JET_errInvalidSesid);
		}

	CheckTableidExported(tableid);

	APIReturn(ErrDispRetrieveColumn(sesid, tableid, columnid, pvData,
		cbData, pcbActual, grbit, pretinfo));
	}


JET_ERR JET_API JetRetrieveColumns( JET_SESID sesid, JET_TABLEID tableid,
	JET_RETRIEVECOLUMN *pretrievecolumn, unsigned long cretrievecolumn )
	{
	JET_ERR					err = JET_errSuccess;
	JET_RETRIEVECOLUMN	*pretrievecolumnMax = pretrievecolumn + cretrievecolumn;
	JET_VSESID			vsesid;
	JET_VTID				vtid;

	APIEnter();

	if (!FValidSesid(sesid))
		{
#ifdef DEBUG
		unsigned long iretrievecolumn = 0;
		for ( ; iretrievecolumn < cretrievecolumn; iretrievecolumn++ )
			{
			FillClientBuffer( pretrievecolumn[iretrievecolumn].pvData,
				pretrievecolumn[iretrievecolumn].cbData );
			}
#endif

		APIReturn(JET_errInvalidSesid);
		}

	CheckTableidExported(tableid);

	vsesid = rgvtdef[tableid].vsesid;
  if (vsesid == (JET_VSESID) 0xFFFFFFFF)
     vsesid = (JET_VSESID) sesid;

	vtid = rgvtdef[tableid].vtid;

	err = ErrIsamRetrieveColumns( vsesid, vtid, pretrievecolumn, cretrievecolumn );

/*	for ( ; pretrievecolumn < pretrievecolumnMax; pretrievecolumn++ )
		{
		err = ErrIsamRetrieveColumn( vsesid, vtid, pretrievecolumn->columnid,
				pretrievecolumn->pvData, pretrievecolumn->cbData,
				&pretrievecolumn->cbActual, pretrievecolumn->grbit,
				pretrievecolumn->pretinfo );
		if ( err < 0 )
			break;
		} */

	APIReturn( err );
	}


JET_ERR JET_API JetSetColumn(JET_SESID sesid, JET_TABLEID tableid,
	JET_COLUMNID columnid, const void __far *pvData, unsigned long cbData,
	JET_GRBIT grbit, JET_SETINFO __far *psetinfo)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispSetColumn(sesid, tableid, columnid, pvData, cbData,
		grbit, psetinfo));
	}


JET_ERR JET_API JetSetColumns(JET_SESID sesid, JET_TABLEID tableid,
	JET_SETCOLUMN *psetcolumn, unsigned long csetcolumn )
	{
	JET_ERR					err = JET_errSuccess;
	JET_SETCOLUMN		*psetcolumnMax = psetcolumn + csetcolumn;
	JET_VSESID			vsesid;
	JET_VTID				vtid;

	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	vsesid = rgvtdef[tableid].vsesid;
  if (vsesid == (JET_VSESID) 0xFFFFFFFF)
     vsesid = (JET_VSESID) sesid;

	vtid = rgvtdef[tableid].vtid;

	err = ErrIsamSetColumns( vsesid, vtid, psetcolumn, csetcolumn );

/*	for ( ; psetcolumn < psetcolumnMax; psetcolumn++ )
		{
		err = ErrIsamSetColumn( vsesid, vtid, psetcolumn->columnid,
			psetcolumn->pvData, psetcolumn->cbData, psetcolumn->grbit,
			psetcolumn->psetinfo );
		if ( err < 0 )
			break;
		} */

	APIReturn( err );
	}


JET_ERR JET_API JetPrepareUpdate(JET_SESID sesid, JET_TABLEID tableid,
	unsigned long prep)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	switch (prep)
		{
	case JET_prepCancel:
		APIReturn(ErrDispPrepareUpdate(sesid, tableid, JET_prepCancel));

	case JET_prepInsert:
	case JET_prepInsertCopy:
	case JET_prepInsertBeforeCurrent:
		APIReturn(ErrDispPrepareUpdate(sesid, tableid, prep));

	case JET_prepReplace:
	case JET_prepReplaceNoLock:
		APIReturn(ErrDispPrepareUpdate(sesid, tableid, prep));

	default:
		APIReturn(JET_errInvalidParameter);
		}
	}


JET_ERR JET_API JetUpdate(JET_SESID sesid, JET_TABLEID tableid,
	void __far *pvBookmark, unsigned long cbMax,
	unsigned long __far *pcbActual)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		{
		FillClientBuffer(pvBookmark, cbMax);

		APIReturn(JET_errInvalidSesid);
		}

	if (cbMax == 0) 	       /* Protect against benign reference */
		pvBookmark = NULL;

	CheckTableidExported(tableid);

	APIReturn(ErrDispUpdate(sesid, tableid, pvBookmark, cbMax, pcbActual));
	}


JET_ERR JET_API JetDelete(JET_SESID sesid, JET_TABLEID tableid)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispDelete(sesid, tableid));
	}


JET_ERR JET_API JetGetCursorInfo(JET_SESID sesid, JET_TABLEID tableid,
	void __far *pvResult, unsigned long cbMax, unsigned long InfoLevel)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		{
		FillClientBuffer(pvResult, cbMax);

		APIReturn(JET_errInvalidSesid);
		}

	CheckTableidExported(tableid);

	APIReturn(ErrDispGetCursorInfo(sesid, tableid, pvResult, cbMax, InfoLevel));
	}


JET_ERR JET_API JetGetCurrentIndex(JET_SESID sesid, JET_TABLEID tableid,
	char __far *szIndexName, unsigned long cchIndexName)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		{
		FillClientBuffer(szIndexName, cchIndexName);

		APIReturn(JET_errInvalidSesid);
		}

	CheckTableidExported(tableid);

	APIReturn(ErrDispGetCurrentIndex(sesid, tableid, szIndexName, cchIndexName));
	}


JET_ERR JET_API JetSetCurrentIndex(JET_SESID sesid, JET_TABLEID tableid,
	const char __far *szIndexName)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispSetCurrentIndex(sesid, tableid, szIndexName));
	}


JET_ERR JET_API JetMove(JET_SESID sesid, JET_TABLEID tableid,
	signed long cRow, JET_GRBIT grbit)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispMove(sesid, tableid, cRow, grbit));
	}


JET_ERR JET_API JetMakeKey(JET_SESID sesid, JET_TABLEID tableid,
	const void __far *pvData, unsigned long cbData, JET_GRBIT grbit)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispMakeKey(sesid, tableid, pvData, cbData, grbit));
	}


JET_ERR JET_API JetSeek(JET_SESID sesid, JET_TABLEID tableid, JET_GRBIT grbit)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispSeek(sesid, tableid, grbit));
	}


JET_ERR JET_API JetGetBookmark(JET_SESID sesid, JET_TABLEID tableid,
	void __far *pvBookmark, unsigned long cbMax,
	unsigned long __far *pcbActual)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		{
		FillClientBuffer(pvBookmark, cbMax);

		APIReturn(JET_errInvalidSesid);
		}

	if (cbMax == 0) 	       /* Protect against benign reference */
		pvBookmark = NULL;

	CheckTableidExported(tableid);

	APIReturn(ErrDispGetBookmark(sesid, tableid, pvBookmark, cbMax, pcbActual));
	}


JET_ERR JET_API JetGotoBookmark(JET_SESID sesid, JET_TABLEID tableid,
	void __far *pvBookmark, unsigned long cbBookmark)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispGotoBookmark(sesid, tableid, pvBookmark, cbBookmark));
	}


JET_ERR JET_API JetGetRecordPosition(JET_SESID sesid, JET_TABLEID tableid,
	JET_RECPOS __far *precpos, unsigned long cbKeypos)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		{
		FillClientBuffer(precpos, cbKeypos);

		APIReturn(JET_errInvalidSesid);
		}

	CheckTableidExported(tableid);

	APIReturn(ErrDispGetRecordPosition(sesid, tableid, precpos, cbKeypos));
	}


JET_ERR JET_API JetGotoPosition(JET_SESID sesid, JET_TABLEID tableid,
	JET_RECPOS *precpos)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		APIReturn(JET_errInvalidSesid);

	CheckTableidExported(tableid);

	APIReturn(ErrDispGotoPosition(sesid, tableid, precpos));
	}


JET_ERR JET_API JetRetrieveKey(JET_SESID sesid, JET_TABLEID tableid,
	void __far *pvKey, unsigned long cbMax,
	unsigned long __far *pcbActual, JET_GRBIT grbit)
	{
	APIEnter();

	if (!FValidSesid(sesid))
		{
		FillClientBuffer(pvKey, cbMax);

		APIReturn(JET_errInvalidSesid);
		}

	CheckTableidExported(tableid);

	APIReturn(ErrDispRetrieveKey(sesid, tableid, pvKey, cbMax, pcbActual, grbit));
	}
