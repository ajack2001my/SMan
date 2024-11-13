#include "dbhelper.h"
#include <sman.rsg>

CDBHelper::CDBHelper()
{
	iDBIsOpened = EFalse;
	iSQLStatement.Copy(_L(""));
	iDBWindow = NULL;
	iFileStore = NULL;	
}

CDBHelper::~CDBHelper()
{
	DeInitDB();
}

TBool CDBHelper::DeleteDB()
{
	TBool iRetVal = EFalse;
	
	if (iDBIsOpened)
		DeInitDB();
		
	if (EikFileUtils::DeleteFile(iDBFileName) == KErrNone)
		iRetVal = ETrue;

	return iRetVal;
}

TBool CDBHelper::CreateDB()
{
	if (!iDBIsOpened)
	{
		if (!ConeUtils::FileExists(iDBFileName))
		{
			TRAPD(iErr, iFileStore = CPermanentFileStore::CreateL(CEikonEnv::Static()->FsSession(), iDBFileName, EFileRead | EFileWrite));
			if (!iErr)
			{
				TStreamId iStreamId;

				iFileStore->SetTypeL(iFileStore->Layout());
				iStreamId = iDBStore.CreateL(iFileStore);
				iFileStore->SetRootL(iStreamId);
				iFileStore->CommitL();
				iDBIsOpened = ETrue;
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_CREATE);
		}
	}
	return iDBIsOpened;
}

TBool CDBHelper::InitDB()
{
	if (!iDBIsOpened)
	{
		if (iFileStore)
		{
			delete iFileStore;
			iFileStore = NULL;
		}
		if (ConeUtils::FileExists(iDBFileName))
		{
			TRAPD(iErr, iFileStore = CFileStore::OpenL(CEikonEnv::Static()->FsSession(), iDBFileName, EFileRead | EFileWrite));
			if (iErr == KErrNone)
			{
				TRAPD(iErr2, iDBStore.OpenL(iFileStore, iFileStore->Root()));
				if (iErr2 == KErrNone)
					iDBIsOpened = ETrue;
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);			
		}
	}
	return iDBIsOpened;
}

void CDBHelper::DeInitDB()
{
	if (iDBIsOpened)
	{
		iDBReadStream.Close();
		iDBWriteStream.Close();
		iDBView.Close();
		iDBStore.Close();
		iDBIsOpened = EFalse;
	}
	if (iDBWindow)
	{
		delete iDBWindow;
		iDBWindow = NULL;
	}
	if (iFileStore)
	{
		delete iFileStore;
		iFileStore = NULL;
	}
}

// This converts all occurrences of a particular character in a text string to some other character.
// The caller is responsible for ensuring there is sufficient space in the 
// passed in TDes to hold the new string. 
void CDBHelper::ConvertTextStatement(TDes *aTheString, TPtrC aStrToReplace, TPtrC aReplacementStr)
{
	HBufC *iTempBuffer = HBufC::NewL(aTheString->Length() * 2);
	iTempBuffer->Des().Zero();
	for (int i = 0; i < aTheString->Length(); i++)
	{
		if ((i + aStrToReplace.Length() - 1) < aTheString->Length())
		{
			if (aTheString->Mid(i, aStrToReplace.Length()).Compare(aStrToReplace) == 0)
			{
				iTempBuffer->Des().Append(aReplacementStr);
				i += aStrToReplace.Length() - 1;
			}
			else
				iTempBuffer->Des().Append(aTheString->Mid(i, 1));
		}
		else
			iTempBuffer->Des().Append(aTheString->Mid(i, 1));
	}
	aTheString->Copy(*iTempBuffer);
	delete iTempBuffer;
	/*
	if (aTheString->Length() > 0)
	{
		TInt iStartLocation = aTheString->Find(aStrToReplace);
		while (iStartLocation > 0)
		{
			aTheString->Replace(iStartLocation, aStrToReplace.Length(), aReplacementStr);
			iStartLocation = aTheString->Find(aStrToReplace);
		}
	}
	*/
}

// This functions assumes ReadDB() was called so that iDBView is ready.
// Also, the caller must call iDBView.GetL() to populate the view before
// calling this function
TBool CDBHelper::PrepareReadDBLongField(TInt aColumnNumber)
{
	TBool iRetVal = EFalse;
	iDBReadStream.Close();
	TRAPD(iError, iDBReadStream.OpenL(iDBView, aColumnNumber));
	if (iError == KErrNone)
		iRetVal = ETrue;
	return iRetVal;
}

// This functions assumes ReadDB() was called so that iDBView is ready.
TBool CDBHelper::PrepareWriteDBLongField(TInt aColumnNumber)
{
	TBool iRetVal = EFalse;
	iDBWriteStream.Close();
	TRAPD(iError, iDBWriteStream.OpenL(iDBView, aColumnNumber));
	if (iError == KErrNone)
		iRetVal = ETrue;
	return iRetVal;
}

TBool CDBHelper::ReadDB()
{
	TBool iRetVal = EFalse;
	
	if (!iDBIsOpened)
		InitDB();
	
	CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY, 3);
	if (iDBIsOpened)
	{
		iDBView.Close();
		if (iDBWindow)
		{
			delete iDBWindow;
			iDBWindow = NULL;
		}
		iDBWindow = new (ELeave) TDbWindow(TDbWindow::EUnlimited);

		// By default, this constructor gives EUpdatable access.
		if (iDBView.Prepare(iDBStore, TDbQuery(iSQLStatement.Left(iSQLStatement.Length()), EDbCompareNormal), *iDBWindow) == KErrNone)
		{
			if (iDBView.EvaluateAll() == KErrNone)
				iRetVal = ETrue;
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);
		
	CEikonEnv::Static()->BusyMsgCancel();
	return iRetVal;
}

TInt CDBHelper::ExecuteSQL()
{
	TInt iRetVal = KErrGeneral;
	if (iDBIsOpened)
		iRetVal = iDBStore.Execute(iSQLStatement);
	return iRetVal;
}

TBool CDBHelper::RecoverDB()
{
	TBool iRetVal = EFalse;
	
	if (!iDBIsOpened)
		InitDB();
		
	if (iDBIsOpened)
	{
		if (iDBStore.IsDamaged())
		{
			TInt iStepNumber = 0;
			TInt iResult;

			CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_DBRECOVERY);
			iResult = iIncrementalDBOp.Recover(iDBStore, iStepNumber);
			if (iResult == KErrNone)
				iRetVal = DoLongDBOp(iStepNumber);
			CEikonEnv::Static()->BusyMsgCancel();
		}
		else
			iRetVal = ETrue;
	}
	return iRetVal;
}

TBool CDBHelper::UpdateDBStats()
{
	TBool iRetVal = EFalse;
	
	if (!iDBIsOpened)
		InitDB();
	
	if (iDBIsOpened)
	{
		TInt iStepNumber = 0;
		TInt iResult;
		
		CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_DBUPDATESTATS);
		iResult = iIncrementalDBOp.UpdateStats(iDBStore, iStepNumber);
		if (iResult == KErrNone)
			iRetVal = DoLongDBOp(iStepNumber);
		CEikonEnv::Static()->BusyMsgCancel();
	}
	return iRetVal;
}
	
TBool CDBHelper::DoLongDBOp(TInt iStepNumber)
{
	TInt iBaseStepNumber;
	TInt iResult = KErrNone;
	TBool iRetVal = EFalse;

	iBaseStepNumber = iStepNumber;
	CDBProgressDialog* iProgressDialog = new (ELeave) CDBProgressDialog;
	iProgressDialog->ExecuteLD(R_DIALOG_DB_COMPRESS);
	iProgressDialog->setProgressMaxValue(iBaseStepNumber);
	iProgressDialog->DrawNow();
	while ((iStepNumber > 0) && (iResult == KErrNone))
	{
		iResult = iIncrementalDBOp.Next(iStepNumber);
		iProgressDialog->updateProgress(iBaseStepNumber - iStepNumber);
	}
	// Irregardless of whether it was successful or not, close this
	iIncrementalDBOp.Close();

	delete iProgressDialog;
	if (iResult == KErrNone)
		iRetVal = ETrue;
	return iRetVal;
}

TBool CDBHelper::CompactDB()
{
	TInt iRetVal = EFalse;
	
	if (!iDBIsOpened)
		InitDB();
	if (iDBIsOpened)
	{
		TInt iStepNumber = 0;
		TInt iResult = iIncrementalDBOp.Compact(iDBStore, iStepNumber);
		
		if (iResult == KErrNone)
			iRetVal = DoLongDBOp(iStepNumber);
	}
	return iRetVal;
}

TInt CDBHelper::BeginTransaction()
{
	if (iDBIsOpened)
		return iDBStore.Begin();
	else
		return KErrNotReady;
}

void CDBHelper::RollbackTransaction()
{
	if ((iDBIsOpened) && (iDBStore.InTransaction()))
		return iDBStore.Rollback();
}

TInt CDBHelper::CommitTransaction()
{
	if ((iDBIsOpened) && (iDBStore.InTransaction()))
		return iDBStore.Commit();
	else
		return KErrNotReady;
}
