#include "dbview.h"
#include "sman.h"
#include "textutils.h"
#include <SMan.rsg>

/***********************************************

Here are the various DBMS panics from Symbian's knowledge
base.
---------------------------------------------------------

ud_std.h (DBMS) 
EDbUnimplemented, 0
EDbInvalidColumn, 1
EDbUninitialised, 2
EDbRowLengthOverflow, 3
EDbTooManyKeys, 4
EDbInvalidViewWindowParameters, 5
EDbWrongType, 6
EDbInvalidIncrementalStep, 7
EDbNoColumnsInSeekKey 8 

sd_std.h (DBMS-Server) 
EDbsUnimplemented, 0
EDbsInvalidColumn, 1
EDbsWrongType, 2
EDbsNoRowData, 3
EDbsNotInUpdate, 4
EDbsBadDescriptor, 5
EDbsBadHandle, 6
EDbsStreamMarkInvalid, 7
EDbsStreamLocationInvalid, 8
EDbsObserverRequestPending 9

us_std.h (DBMS-Store) 
EDbUnimplemented, 0
EDbNoStore, 1
EDbCannotSeek, 2
EDbNotFixedFieldType, 3
EDbWrongType 4

ut_std.h (DBMS-Table) 
EDbInvalidColumn, 0
EDbWrongType, 1 
EDbInUpdate, 2
EDbNotInUpdate, 3
EDbInvalidRow, 4
EDbRowNotRead, 5
EDbReadOnly, 6
EDbTableOpen, 7
EDbNotEvaluated, 8
EDbStreamOpen, 9
EDbRowSetConstraintMismatch, 10
EDbBeginNestedTransaction, 11
EDbUpdatesPendingOnCommit, 12
EDbUpdatesPendingOnRollback, 13
EDbNoCurrentTransaction, 14
EDbStreamsPendingOnCommit, 15
EDbStreamsPendingOnRollback, 16
EDbInvalidBookmark 17 
***********************************************/

/*************************************************************
*
* Database view
*
* The database code is based on the sample DBMS application that comes with 
* the SDK (with HEAVY modifications :))
*
**************************************************************/

CSMan2DBView::CSMan2DBView(CConfig *cData) : CViewBase(cData)
{
}

TBool CSMan2DBView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2DBView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2DBView::ViewDeactivated()
{
	// Note: ViewActivated of the new view is called BEFORE ViewDeactivated of the old view. 
	// Therefore, we mustn't make any changes to the toolbar in our call to doCloseDB else it will
	// overwrite any toolbar that has been set by the new view. Hence we call doCloseDB with
	// EFalse
	doCloseDB(EFalse);
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
	}
}

void CSMan2DBView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();		
}

TVwsViewId CSMan2DBView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidDBView);
}

CSMan2DBView::~CSMan2DBView()
{
	// DEBUG
	releaseFont();
	DeInitDB();
	delete fieldLabelsArray;
}

void CSMan2DBView::doChangePassword()
{
	// Can't use the standard set password dialog because it doesn't expose the new password
	// after it's done
	
	if (!dbIsOpened)
		InitDB();
		
	if (dbIsOpened)
	{
		TPassword promptedPassword;
		CSetPasswordDialog *passwordDialog = new (ELeave) CSetPasswordDialog(&promptedPassword);
		if (passwordDialog->ExecuteLD(R_DIALOG_DB_CHANGEPASSWORD) == EEikBidOk)
		{
			// Need to check if db was closed. If we task away while the change password dialog
			// box is opened, the OkToExitL function seems to receive an aButtonId of EEikBidOk!?!?
			// which isn't really true. By the time control is returned to this function, the view
			// was already deactivated and the database was closed. Therefore, we must do another
			// check to see if it's opened else the code in the 2nd IF block will run and crash
			if (dbIsOpened)
			{
				// Only re-encrypt if passwords are different
				if (promptedPassword.Compare(dbPassword) != 0)
				{
					// Old stuff for accessing the DB
					CSecurityBase *oldSecurityBase = Security::NewL();
					oldSecurityBase->SetL(TPtrC(), dbPassword);
					TBuf8<KMaxPassword> password;
					password.Copy(dbPassword);
					
					// New stuff for re-encrypting the DB
					CSecurityBase *newSecurityBase = Security::NewL();
					newSecurityBase->SetL(TPtrC(), promptedPassword);
					TBuf8<KMaxPassword> newPassword;
					newPassword.Copy(promptedPassword);
					
					CEikonEnv::Static()->BusyMsgL(R_TBUF_DB_MSG_ENCRYPTING);
					if (dbStore.ChangeSecurity(oldSecurityBase->NewDecryptL(password), newSecurityBase->NewEncryptL(newPassword)) != KErrNone)
						CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_CHANGEPASSWORD);
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
					CEikonEnv::Static()->BusyMsgCancel();
					
					delete oldSecurityBase;
					delete newSecurityBase;
				}
			}
		}
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_CHANGEPASSWORD);
}

// WARNING: This function deletes the entire database and reinitializes it!! For those times when
// you forgot your password. :(
//
// Update: 19th May 2004
// Changed this so that you are now required to key in your password prior to reinitializing the
// db. If you do forget your password, use the file manager to delete the DB file.
void CSMan2DBView::doReInitDB()
{
	TBool iOldOpenState = dbIsOpened;
	
	if (!dbIsOpened)
		InitDB();

	if (dbIsOpened)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_DELETE_TITLE);
		HBufC* dataBuffer2 = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_DELETE);

		if (CEikonEnv::Static()->QueryWinL(*dataBuffer, *dataBuffer2))
		{
			// Not really necessary but i'm just paranoid. :)
			delete dataBuffer;
			delete dataBuffer2;
			
			if (dbIsOpened)
				DeInitDB();

			// Faster than the Destroy() member call
			TInt retVal = EikFileUtils::DeleteFile(dbFileName);
			if (retVal == KErrNone)
			{
				InitDB();
				if (dbIsOpened)
				{
					DeInitDB();
					CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
				}
				HideAllControls();
			}
			else if (retVal != KErrNotFound)
				CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_REINIT);
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_NODB);
		}
		else
		{
			if (!iOldOpenState)
				DeInitDB();
				
			delete dataBuffer;
			delete dataBuffer2;
		}
	}
}

TBool CSMan2DBView::doLongDBOp(TInt stepNumber)
{
	TInt baseStepNumber;
	TInt res = KErrNone;
	TBool retVal = EFalse;

	baseStepNumber = stepNumber;
	CDBProgressDialog* progressDialog = new (ELeave) CDBProgressDialog;
	progressDialog->ExecuteLD(R_DIALOG_DB_COMPRESS);
	progressDialog->setProgressMaxValue(baseStepNumber);
	progressDialog->DrawNow();

	while ((stepNumber > 0) && (res == KErrNone))
	{
		res = incDB.Next(stepNumber);
		progressDialog->updateProgress(baseStepNumber - stepNumber);
	}
	// Irregardless of whether it was successful or not, close this
	incDB.Close();

	delete progressDialog;
	if (res == KErrNone)
		retVal = ETrue;
	return retVal;
}

TBool CSMan2DBView::doCompact()
{
	TInt retVal = EFalse;
	
	if (!dbIsOpened)
		InitDB();
		
	if (dbIsOpened)
	{
		TInt stepNumber;
		TInt res = incDB.Compact(dbStore, stepNumber);
		
		if (res == KErrNone)
			retVal = doLongDBOp(stepNumber);
	}
	return retVal;
}

TBool CSMan2DBView::doUpdateStats()
{
	TInt retVal = EFalse;
	
	if (!dbIsOpened)
		InitDB();
		
	if (dbIsOpened)
	{
		TInt stepNumber = 0;
		TInt res;
		
		if (dbStore.IsDamaged())
		{
			CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_DBRECOVERY);
			res = incDB.Recover(dbStore, stepNumber);
			if (res == KErrNone)
				retVal = doLongDBOp(stepNumber);
			CEikonEnv::Static()->BusyMsgCancel();
		}
		
		CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_DBUPDATESTATS);
		res = incDB.UpdateStats(dbStore, stepNumber);
		if (res == KErrNone)
			retVal = doLongDBOp(stepNumber);
		else
			retVal = EFalse;
		CEikonEnv::Static()->BusyMsgCancel();
	}
	return retVal;
}

TBool CSMan2DBView::doDelete()
{
	TPtrC tempText, recordIndex;
	TBool retVal = ETrue;
	TLex converter;
	TUint32 value = 0;
	
	if (cIndexListBox->SelectionIndexes()->Count() < 1)
		CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_NORECORDS);
	else
	{
		CEikonEnv::Static()->BusyMsgL(R_TBUF_DB_MSG_DELETING);
		for (int i = 0; i < cIndexListBox->SelectionIndexes()->Count(); i++)
		{
			tempText.Set(cIndexListBox->Model()->ItemText(cIndexListBox->SelectionIndexes()->At(i)));
			TextUtils::ColumnText(recordIndex, 1, &tempText);
			converter.Assign(recordIndex);
			converter.Val(value, EDecimal);
			if (!DeleteRecord(value))
				retVal = EFalse;
		}
		CEikonEnv::Static()->BusyMsgCancel();
	}
	return retVal;
}

void CSMan2DBView::doDisplayOneRecord(TBool aIsReadOnly)
{
	if (cIndexListBox->Model()->ItemTextArray()->MdcaCount() > 0)
	{
		// Extract the index number of the currently highlighted record
		TPtrC entryIndex, entryRow;
		TUint32 recordIndex = 0;
		TLex converter;
		
		entryRow.Set(cIndexListBox->Model()->ItemTextArray()->MdcaPoint(cIndexListBox->View()->CurrentItemIndex()));
		TextUtils::ColumnText(entryIndex, 1, &entryRow);
		converter.Assign(entryIndex);
		converter.Val(recordIndex, EDecimal);
		currentRecordIndexIsValid = ETrue;
		
		if (!aIsReadOnly)
			ShowRecordsSingle(recordIndex, EFalse);
		else
			ShowRecordsSingleReadOnly(recordIndex);
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_NORECORDS);
}

void CSMan2DBView::MoveEditCursors(TCursorPosition::TMovementType aCursorDirection)
{
	if (fieldDisplayName->IsFocused())
		fieldDisplayName->MoveCursorL(aCursorDirection, EFalse);
	else
	{
		for (int j = 0; j < NUM_DATA_FIELDS; j++)
		{
			if (fieldLabels[j]->IsFocused())
				fieldLabels[j]->Edwin()->MoveCursorL(aCursorDirection, EFalse);
			else if (fieldValues[j]->IsFocused())
				fieldValues[j]->MoveCursorL(aCursorDirection, EFalse);
		}
	}
}

TKeyResponse CSMan2DBView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (currentDBView == DBView_Single_Editable)
	{
		if (aType == EEventKey)
		{
			if (aKeyEvent.iCode == EQuartzKeyConfirm)
			{
				if (SaveRecord(currentRecordIndex))
					ShowRecordsList(ETrue, ETrue);
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_SAVE);
				return EKeyWasConsumed;
			}
			else if (aKeyEvent.iCode == EQuartzKeyTwoWayDown) 
				MoveEditCursors(TCursorPosition::EFRight);
			else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
				MoveEditCursors(TCursorPosition::EFLeft);
		}
		return scrollableContainer->OfferKeyEventL(aKeyEvent, aType);
	}
	else if (currentDBView == DBView_Single_ReadOnly)
	{
		if (aType == EEventKey)
		{
			if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
			{
				readOnlyDisplay->MoveDisplayL(TCursorPosition::EFLineDown);
				readOnlyDisplay->UpdateScrollBarsL();
				return EKeyWasConsumed;
			}
			else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
			{
				readOnlyDisplay->MoveDisplayL(TCursorPosition::EFLineUp);
				readOnlyDisplay->UpdateScrollBarsL();
				return EKeyWasConsumed;
			}
			else if (aKeyEvent.iCode == EQuartzKeyConfirm)
			{
				ShowRecordsList(EFalse, ETrue);
				return EKeyWasConsumed;
			}
		}
		return readOnlyDisplay->OfferKeyEventL(aKeyEvent, aType);		
	}
	else // should be DBView_List
	{
		if (cIndexListBox->Model()->NumberOfItems() > 0)
		{
			if (aType == EEventKey)
			{
				if (aKeyEvent.iCode == EQuartzKeyConfirm)
				{
					doDisplayOneRecord(ETrue);
					return EKeyWasConsumed;
				}
				else if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
				{
					cIndexListBox->View()->MoveCursorL(CListBoxView::ECursorNextItem, CListBoxView::ENoSelection);
					cIndexListBox->UpdateScrollBarsL();
					return EKeyWasConsumed;
				}
				else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
				{
					cIndexListBox->View()->MoveCursorL(CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection);
					cIndexListBox->UpdateScrollBarsL();
					return EKeyWasConsumed;
				}
			}
			return cIndexListBox->OfferKeyEventL(aKeyEvent, aType);
		}
	}
	return EKeyWasNotConsumed;
}

void CSMan2DBView::ExportDB()
{
	TBool iOldDBState = dbIsOpened;
	
	if (!dbIsOpened)
		InitDB();
		
	if (dbIsOpened)
	{
		TFileName iExportFileName;
		iExportFileName.SetLength(0);
		CFolderBrowser *iFolderBrowser = new (ELeave) CFolderBrowser();
		iFolderBrowser->iPath = &iExportFileName;
		iFolderBrowser->iFileSpec.Copy(_L("*.CSV"));
		iFolderBrowser->iAllowSelection = EFalse;
		if (iFolderBrowser->ExecuteLD(R_DIALOG_FOLDER_FILE_BROWSER) == EEikBidYes)
		{
			TBuf<23> iSQLStatement;
			HBufC *iOutputBuffer = HBufC::NewL(1024);
			TBool iErrorsEncountered = EFalse;

			TBool iProgressDialogActive = ETrue;
			// Borrow this dialog for doing progress
			CProgressDialog *iProgressDialog = new (ELeave) CProgressDialog();
			iProgressDialog->iDialogStillActive = &iProgressDialogActive;
			iProgressDialog->ExecuteLD(R_DIALOG_CELLS_IMPORTEXPORT_PROGRESS);
			iProgressDialog->SetButtonDim(ETrue);
			iProgressDialog->DrawNow();								
			// Give dialog time to draw itself
			User::After(300000);
			
			iExportFileName.Append(_L("SMANDB.CSV"));
			RFile iBaseOutputFile;
			CTextFileReader *iTextFile = new (ELeave) CTextFileReader();
			CCSVLineParser *iCSVParser = new (ELeave) CCSVLineParser();
			if (iBaseOutputFile.Replace(CEikonEnv::Static()->FsSession(), iExportFileName, EFileWrite | EFileStreamText) == KErrNone)
			{
				TInt iNumRecords = 0, iCurrentRecord = 0;
				TDes *iTempDes;
				
				iTextFile->Set(&iBaseOutputFile);
				iSQLStatement.Copy(_L("SELECT * FROM Databank"));
				
				if (iProgressDialogActive)
				{
					HBufC *iTempBuf = HBufC::NewL(KMaxFileName);
					iTempBuf->Des().Copy(iExportFileName);
					iProgressDialog->AppendText(R_TBUF_DB_EXPORT, iTempBuf);
					delete iTempBuf;
				}
				
				if (ReadDB(iSQLStatement))
				{
					HBufC *iTempBuffer = HBufC::NewL(100);

					iNumRecords = dbView.CountL();
					while (dbView.NextL())
					{
						dbView.GetL();
						iOutputBuffer->Des().Zero();
						for (int i = 2; i < 13; i++)
						{
							iTempBuffer->Des().Copy(dbView.ColDes(i));
							TPtr iPtr = TPtr(iTempBuffer->Des());
							iTempDes = &iPtr;
							iCSVParser->ReplaceAll(iTempDes, _L("\""), _L("\"\""));
							iOutputBuffer->Des().Append(_L("\""));
							iOutputBuffer->Des().Append(*iTempBuffer);
							iOutputBuffer->Des().Append(_L("\","));
						}
						iOutputBuffer->Des().SetLength(iOutputBuffer->Des().Length() - 1);
						TPtr iPtr = TPtr(iOutputBuffer->Des());
						iTempDes = &iPtr;
						if (iTextFile->WriteLine(iTempDes) != KErrNone)
						{
							iErrorsEncountered = ETrue;
							break;
						}
						iCurrentRecord++;
						if (iProgressDialogActive)
							iProgressDialog->SetProgressValue(TInt(TReal(iCurrentRecord) / TReal(iNumRecords) * TReal(100)));
					}
					dbView.Close();
				}
				else
				{
					iErrorsEncountered = ETrue;
					if (iProgressDialogActive)
						iProgressDialog->AppendText(R_TBUF_DB_ERR_READ, (HBufC *)(NULL));
				}
				iBaseOutputFile.Close();
			}
			else
			{
				iErrorsEncountered = ETrue;
				if (iProgressDialogActive)
					iProgressDialog->AppendText(R_TBUF_DB_EXPORT_ERROUTPUT, (HBufC *)(NULL));
			}
			delete iCSVParser;
			delete iTextFile;
			delete iOutputBuffer;

			if (iProgressDialogActive)
			{
				if (!iErrorsEncountered)
					iProgressDialog->AppendText(R_TBUF_MISC_DONE, (HBufC *)(NULL));
				else
					iProgressDialog->AppendText(R_TBUF_MISC_ERRORS, (HBufC *)(NULL));
				iProgressDialog->SetButtonDim(EFalse);
			}
		}
	}
	
	if (!iOldDBState)
		DeInitDB();
}

void CSMan2DBView::ImportDB()
{
	TBool iOldDBState = dbIsOpened;
	
	if (!dbIsOpened)
		InitDB();
		
	if (dbIsOpened)
	{
		TFileName iImportPath;
		iImportPath.SetLength(0);
		CFolderBrowser *iFolderBrowser = new (ELeave) CFolderBrowser();
		iFolderBrowser->iPath = &iImportPath;
		iFolderBrowser->iAllowSelection = ETrue;
		iFolderBrowser->iFileSpec.Copy(_L("*.CSV"));
		CDesCArray *iFilesArray = new (ELeave) CDesCArrayFlat(2);
		iFolderBrowser->iFilesSelected = iFilesArray;
		if (iFolderBrowser->ExecuteLD(R_DIALOG_FOLDER_FILE_BROWSER) == EEikBidYes)
		{
			if (iFilesArray->Count() > 0)
			{
				HBufC *iReadBuffer = HBufC::NewL(1024);
				HBufC *iSQLStatement = HBufC::NewL(1024 + 500);
				TBool iProgressDialogActive = ETrue;
				RFile iCSVFile;
				CTextFileReader *iTextFileReader = new (ELeave) CTextFileReader();
				CCSVLineParser *iCSVParser = new (ELeave) CCSVLineParser();
				TBool iErrorEncountered = EFalse;
				TDes *iTempDes;
				HBufC *iFileMask = HBufC::NewL(KMaxFileName);
				
				CProgressDialog *iProgressDialog = new (ELeave) CProgressDialog();
				iProgressDialog->iDialogStillActive = &iProgressDialogActive;
				iProgressDialog->ExecuteLD(R_DIALOG_CELLS_IMPORTEXPORT_PROGRESS);
				iProgressDialog->SetButtonDim(ETrue);
				iProgressDialog->DrawNow();
				
				// Give dialog time to draw itself
				User::After(300000);
				
				for (int i = 0; i < iFilesArray->Count(); i++)
				{
					iFileMask->Des().Copy(iImportPath);
					iFileMask->Des().Append(iFilesArray->MdcaPoint(i));
					
					if (iProgressDialogActive)
						iProgressDialog->AppendText(-1, iFileMask);
						
					if (iCSVFile.Open(CEikonEnv::Static()->FsSession(), iFileMask->Des(), EFileRead) == KErrNone)
					{
						iTextFileReader->Set(&iCSVFile);
						TInt iReadResult = KErrGeneral;
						TInt iCurrentCSVLine = -1;
						TBool iFieldNameWasBlank = EFalse;
						
						do
						{
							iCurrentCSVLine++;
							TPtr iPtr = TPtr(iReadBuffer->Des());
							iTempDes = &iPtr;
							iReadResult = iTextFileReader->ReadLine(iTempDes);

							if (iProgressDialogActive)
								iProgressDialog->SetProgressValue(iTextFileReader->iPercentRead);
								
							if (iReadResult == KErrNone)
							{	
								// Read display name
								TPtr iPtr = TPtr(iReadBuffer->Des());
								iTempDes = &iPtr;
								TBuf<100> iColumnExtracted;
								if (iCSVParser->GetColumn(iTempDes, &iColumnExtracted, 0) == KErrNone)
								{
									iCSVParser->ReplaceAll(&iColumnExtracted, _L("'"), _L("''"));
									iSQLStatement->Des().Copy(_L("'"));
									iSQLStatement->Des().Append(iColumnExtracted);
									iSQLStatement->Des().Append(_L("',"));
									
									// Read the rest of the columns
									for (int i = 1; i < 11; i++)
									{
										if (iCSVParser->GetColumn(iTempDes, &iColumnExtracted, i) == KErrNone)
										{
											// Is this a field name? If it is, check if it's blank
											if ((i % 2) == 1)
											{
												// We don't trim field data because the user may
												// purposely want spaces in the data. However, for field
												// names, it is trimmed
												if (iColumnExtracted.Length() == 0)
													iFieldNameWasBlank = ETrue;
											}
											else
											{
												// Since this is the even-numbered field, it must be the
												// field data. We check if the field name was blank previously.
												// If so, throw away this field value. This is the same input
												// validation applied when using InfoStore with the GUI
												if (iFieldNameWasBlank)
												{
													iColumnExtracted.Zero();
													iFieldNameWasBlank = EFalse;
												}
											}
											iCSVParser->ReplaceAll(&iColumnExtracted, _L("'"), _L("''"));								
											iSQLStatement->Des().Append(_L("'"));
											iSQLStatement->Des().Append(iColumnExtracted);
											iSQLStatement->Des().Append(_L("',"));
										}
										else
										{
											if (iProgressDialogActive)
											{
												HBufC *iErrorText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_IMPEXP_GENERALERROR);
												iErrorText = iErrorText->ReAllocL(iErrorText->Length() + 11);
												iErrorText->Des().AppendNum(iCurrentCSVLine);
												iProgressDialog->AppendText(R_TBUF_CELLID_ERRPARSINGIMPORTFILE, iErrorText);
												delete iErrorText;
											}
											iErrorEncountered = ETrue;
											break;
										}
									}
									if (!iErrorEncountered)
									{
										iSQLStatement->Des().SetLength(iSQLStatement->Des().Length() - 1);
										iSQLStatement->Des().Insert(0, _L("INSERT INTO DataBank (DisplayName,NameField1,ContentField1,NameField2,ContentField2,NameField3,ContentField3,NameField4,ContentField4,NameField5,ContentField5) VALUES ("));
										iSQLStatement->Des().Append(_L(")"));
										if (dbStore.Execute(*iSQLStatement) < 1)
										{
											if (iProgressDialogActive)
												iProgressDialog->AppendText(R_TBUF_DB_CREATERECORD, (HBufC *)(NULL));
											iErrorEncountered = ETrue;
											break;
										}
									}
								}
								else
								{
									if (iProgressDialogActive)
									{
										HBufC *iErrorText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_IMPEXP_GENERALERROR);
										iErrorText = iErrorText->ReAllocL(iErrorText->Length() + 11);
										iErrorText->Des().AppendNum(iCurrentCSVLine);
										iProgressDialog->AppendText(R_TBUF_CELLID_ERRPARSINGIMPORTFILE, iErrorText);
										delete iErrorText;													
									}
									iErrorEncountered = ETrue;
								}
							}
						} while ((iReadResult == KErrNone) && (!iErrorEncountered)); // end-do
					}
					iCSVFile.Close();
				}
				delete iCSVParser;
				delete iTextFileReader;
				delete iReadBuffer;
				delete iSQLStatement;
				delete iFileMask;
				
				if (iProgressDialogActive)
				{
					if (iErrorEncountered)
						iProgressDialog->AppendText(R_TBUF_MISC_ERRORS, (HBufC *)(NULL));
					else
						iProgressDialog->AppendText(R_TBUF_MISC_DONE, (HBufC *)(NULL));
					iProgressDialog->SetButtonDim(EFalse);
				}
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOIMPORTFILES);
		}
		delete iFilesArray;
	}
	
	if (!iOldDBState)
		DeInitDB();
	else
		doReadDB();
}

// The next two routines are only called by InitDB once the DB file is initialied. Therefore, we can safely 
// assume the database is opened and healthy
void CSMan2DBView::doCreateTable()
{
	// Create a table
	_LIT(KSQLCreateData, "CREATE TABLE Databank (Index COUNTER, DisplayName VARCHAR(30), NameField1 VARCHAR(30), ContentField1 VARCHAR(50), NameField2 VARCHAR(30), ContentField2 VARCHAR(50), NameField3 VARCHAR(30), ContentField3 VARCHAR(50), NameField4 VARCHAR(30), ContentField4 VARCHAR(50), NameField5 VARCHAR(30), ContentField5 VARCHAR(50))");
	User::LeaveIfError(dbStore.Execute(KSQLCreateData));
}

void CSMan2DBView::doCreateIndex()
{
	_LIT(KSQLCreate1, "CREATE UNIQUE INDEX Index_Index ON Databank (Index)");
	User::LeaveIfError(dbStore.Execute(KSQLCreate1));
	_LIT(KSQLCreate2, "CREATE INDEX Index_DisplayName ON Databank (DisplayName)");
	User::LeaveIfError(dbStore.Execute(KSQLCreate2));
}

TBool CSMan2DBView::DeleteRecord(TUint32 aIndex)
{
	TBool retVal = EFalse;
	TBuf<100> sqlString;
	
	if (!dbIsOpened)
		InitDB();
	
	if (dbIsOpened)
	{
		sqlString.Format(_L("DELETE FROM Databank WHERE Index = %u"), aIndex);
		if (dbStore.Execute(sqlString) < 0)
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_DELETE);
		else
			retVal = ETrue;
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_DELETE);
	return retVal;
}

void CSMan2DBView::DeInitDB()
{
	if (dbIsOpened)
	{
		dbView.Close();
		dbStore.Close();
		dbIsOpened = EFalse;
	}
	if (dbWindow)
	{
		delete dbWindow;
		dbWindow = NULL;
	}
	if (fileStore)
	{
		delete fileStore;
		fileStore = NULL;
	}
	if (securityBase)
	{
		delete securityBase;
		securityBase = NULL;
	}
	currentRecordIndex = 0;
	currentRecordIndexIsValid = EFalse;	
	dbPassword.Copy(DEFAULT_PASSWORD);
}

// Returns 0 if init was successful
// Returns 1 if init was unsuccessful
// Returns 2 if user cancelled the request
TInt CSMan2DBView::InitDB()
{
	TInt iInitResult = 1;
	
	// No point showing InfoMsgs here because the call to ShowRecordList() will take care of that

	// Note: the call to RDbStoreDatabase::OpenL and CreateL destroys "decryptor" / "encryptor"

	// Security is handled slightly different here. The encryption / decryption key is set by
	// the security data of the security classes i.e. CSecurityBase::SecurityData() - NOT THE PASSWORD
	// Since we do not know the password beforehand, the routines here set the security data to be
	// the password itself. The APIs for CreateL and OpenL will fail accordingly if your encryption / decryption
	// key is wrong. Everything works nicely except for the standard password dialogs. We cannot use that
	// because these dialogs assume you know the password beforehand. It's a simple matter of creating
	// our own dialog.
	
	if (securityBase)
		delete securityBase;
	securityBase = Security::NewL();
	securityBase->SetL(TPtrC(), dbPassword);

	if (dbIsOpened)
		DeInitDB();
	
	if (!dbIsOpened)
	{
		TBuf8<KMaxPassword> tempPassword;

		if (fileStore)
		{
			delete fileStore;
			fileStore = NULL;
		}
		if (!ConeUtils::FileExists(dbFileName))
		{
			TRAPD(err, fileStore = CPermanentFileStore::CreateL(CEikonEnv::Static()->FsSession(), dbFileName, EFileRead | EFileWrite));
			if (!err)
			{
				TStreamId streamId;

				tempPassword.Copy(DEFAULT_PASSWORD);
				fileStore->SetTypeL(fileStore->Layout());
				encryptor = securityBase->NewEncryptL(tempPassword);
				streamId = dbStore.CreateL(fileStore, encryptor);
				fileStore->SetRootL(streamId);
				fileStore->CommitL();
				doCreateTable();
				// The choice of index is, unfortunately, a little arbitrary at this point.
				doCreateIndex();
				dbIsOpened = ETrue;
				iInitResult = 0;
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_CREATE);
		}
		else
		{
			TRAPD(err, fileStore = CFileStore::OpenL(CEikonEnv::Static()->FsSession(), dbFileName, EFileRead | EFileWrite));
			if (!err)
			{
				TBool retVal = EFalse;
		
				tempPassword.Copy(dbPassword);
				decryptor = securityBase->NewDecryptL(tempPassword);
				TRAPD(err2, dbStore.OpenL(fileStore, fileStore->Root(), decryptor));
				if (err2)
				{
					if (err2 == KErrGeneral)
					{
						TPassword thePassword;
						CGetPasswordDialog *passwordDialog;
						
						passwordDialog = new (ELeave) CGetPasswordDialog(&thePassword);
						if (passwordDialog->ExecuteLD(R_DIALOG_DB_GETPASSWORD) == EEikBidOk)
						{
							securityBase->SetL(dbPassword, thePassword);
							tempPassword.Copy(thePassword);
							decryptor = securityBase->NewDecryptL(tempPassword);
							TRAP(err2, dbStore.OpenL(fileStore, fileStore->Root(), decryptor));
							if (!err2)
							{
								dbPassword.Copy(thePassword);
								dbIsOpened = ETrue;
								retVal = ETrue;
								iInitResult = 0;
							}
						}
						else
						{
							retVal = ETrue;
							iInitResult = 3;
						}
					}
					if (!retVal)
						CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);
				}
				else
					dbIsOpened = ETrue;
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);
		}
	}
	
	if (!dbIsOpened)
	{
		if (fileStore)
		{
			delete fileStore;
			fileStore = NULL;
		}
		doCloseDB(ETrue);
	}	
	delete securityBase;
	securityBase = NULL;
	
	return iInitResult;
}

void CSMan2DBView::doReadDB()
{
	//if (!dbIsOpened)
	//	DeInitDB();
	ShowRecordsList(ETrue, ETrue);
}

void CSMan2DBView::doCloseDB(TBool aDrawToolbar)
{
	if (dbIsOpened)
		DeInitDB();
	HideAllControls();
	currentDBView = DBView_List;
	currentRecordIndexIsValid = EFalse;
	if (aDrawToolbar)
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->SetToolbarL(R_TOOLBAR_DB_LIST);
	((CDesCArray*)cIndexListBox->Model()->ItemTextArray())->Reset();
	((CDesCArray*)cIndexListBox->Model()->ItemTextArray())->Compress();
	cIndexListBox->HandleItemRemovalL();
}

void CSMan2DBView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	if (currentDBView == DBView_List)
	{
		TBool consumed = EFalse;
		TInt theItem;
		TBool pointValid = cIndexListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, theItem);
		
		if (aPointerEvent.iType == TPointerEvent::EDrag)
		{
			if (aPointerEvent.iPosition.iX < (cIndexListBox->View()->ItemDrawer()->MarkColumn()))
			{
				if ((pointValid) && (oldItem != theItem))
				{
					consumed = ETrue;
					cIndexListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
					cIndexListBox->UpdateScrollBarsL();
					oldItem = theItem;
				}
			}
		}
		else if (aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
			if (pointValid)
			{
				oldItem = theItem;
				consumed = ETrue;
				if (aPointerEvent.iPosition.iX > cIndexListBox->View()->ItemDrawer()->MarkColumn())
				{
					cIndexListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::ENoSelection);
					doDisplayOneRecord(ETrue);
				}
				else
					cIndexListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
				cIndexListBox->UpdateScrollBarsL();
			}
		}
		if (!consumed)
			CCoeControl::HandlePointerEventL(aPointerEvent);
	}
	else
		CViewBase::HandlePointerEventL(aPointerEvent);
}

TBool CSMan2DBView::ReadDB(TPtrC sqlStatement)
{
	TInt retVal = EFalse;
	
	if (!dbIsOpened)
		InitDB();
	
	if (dbIsOpened)
	{
		// Symbian has a very VERY useful feature in its DBMS engine called pre-evaluation. :) :) :) :) 
		// Solves the classical problem of memory vs performance issue when navigating a db
		// This technology is similar to cursors in other databases. In our case, we don't need to use it
		// but i've included this functionality for future versions of SMan. I have a strong feeling, v1.3++ will
		// be using this.
		dbView.Close();
		if (dbWindow)
		{
			delete dbWindow;
			dbWindow = NULL;
		}
		dbWindow = new (ELeave) TDbWindow(TDbWindow::EUnlimited);

		// By default, this constructor gives EUpdatable access.
		if (dbView.Prepare(dbStore, TDbQuery(sqlStatement.Left(sqlStatement.Length()), EDbCompareNormal), *dbWindow) == KErrNone)
		{
			if (dbView.EvaluateAll() == KErrNone)
				retVal = ETrue;
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);	
	return retVal;
}

void CSMan2DBView::HideRecordsList()
{
	cIndexListBox->MakeVisible(EFalse);
	cIndexListBox->SetFocus(EFalse);
}

void CSMan2DBView::HideRecordsSingleReadOnly()
{
	readOnlyDisplay->MakeVisible(EFalse);
	// If i don't set size to shrink it, making the control invisible still exhibits the following problems:
	// 1) The scrollbar is still visible!!! Very persistent bugger
	// 2) When setting zoom (applying paragraph and character format), the control momentarily does a
	//     redraw and is visible
	// So i cheat by making the control really really small.
	readOnlyDisplay->SetSize(TSize(1, 1));
	readOnlyDisplay->SetFocus(EFalse);
}

void CSMan2DBView::HideRecordsSingle()
{
	for (int i = 0; i < NUM_DATA_FIELDS; i++)
	{
		fieldLabels[i]->SetFocus(EFalse);
		fieldValues[i]->SetFocus(EFalse);
	}
	
	// Note: I found that if i don't do the following block of code, the FEP will go crazy when I switch
	// the view back to single records i.e. make the scrollable container visible. When it switches back,
	// you won't see any flashing cursor ANYWHERE but the FEP's black triangle is visible indicating some
	// control is awaiting FEP input. The moment you scribble anywhere, an ETEXT 12 panic occurs. Weird.
	TBuf<1> empty;
	empty.Copy(_L(""));
	fieldDisplayName->SetTextL(&empty);
	
	fieldDisplayName->SetFocus(EFalse);
	scrollableContainer->MakeVisible(EFalse);
	scrollableContainer->SetFocus(EFalse);
}

TBool CSMan2DBView::ReadRecordsList()
{
	TBool retVal = EFalse;
	TInt iOpenResult = 1;
	
	CDesCArray *iArray = ((CDesCArray *) cIndexListBox->Model()->ItemTextArray());
	iArray->Reset();
	iArray->Compress();
	cIndexListBox->HandleItemRemovalL();
	
	if (!dbIsOpened)
		iOpenResult = InitDB();
	
	if (dbIsOpened)
	{
		TBuf<70> sqlString;
		sqlString.Copy(_L("SELECT Index, DisplayName FROM Databank ORDER BY DisplayName"));
		if (!configData->dbSortAscending)
			sqlString.Append(_L(" DESC"));
			
		if (ReadDB(sqlString))
		{
			TBuf<100> rowBuffer;
			TBuf<50> displayNameBuffer;
			TInt indexBuffer;
			
			//for (dbView.FirstL(); dbView.AtRow(); dbView.NextL())
			while (dbView.NextL())
			{
				dbView.GetL();
				displayNameBuffer.Copy(dbView.ColDes(2));
				indexBuffer = dbView.ColUint32(1);
				rowBuffer.Copy(displayNameBuffer);
				rowBuffer.Append(KColumnListSeparator);
				displayNameBuffer.Format(_L("%u"), indexBuffer);
				rowBuffer.Append(displayNameBuffer);
				iArray->AppendL(rowBuffer);
			}
			retVal = ETrue;
			cIndexListBox->HandleItemAdditionL();
		}
		// Free up resources as the listbox now owns the data
		dbView.Close();
		iOpenResult = 0;
	}
	else
	{
		if (iOpenResult == 1)
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_READ);
	}
	return retVal;
}

void CSMan2DBView::ShowRecordsList(TBool aRefresh, TBool showToolbar)
{
	TBool readOk = EFalse;
	
	currentRecordIndex = 0;
	currentRecordIndexIsValid = EFalse;
	HideRecordsSingle();
	HideRecordsSingleReadOnly();
	if (aRefresh)
		readOk = ReadRecordsList();
	else
		readOk = ETrue;
		
	if (showToolbar)
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->SetToolbarL(R_TOOLBAR_DB_LIST);
		
	if (readOk)
	{
		cIndexListBox->MakeVisible(ETrue);
		cIndexListBox->SetFocus(ETrue, ENoDrawNow);
		cIndexListBox->DrawDeferred();
	}
	currentRecordIndexIsValid = EFalse;
	currentDBOp = DBOp_Browsing;
	currentDBView = DBView_List;
}

void CSMan2DBView::ShowRecordsSingleReadOnly(TUint32 aIndex)
{
	HideRecordsSingle();
	HideRecordsList();
		
	if (!dbIsOpened)
		InitDB();

	if (dbIsOpened)
	{
		TBuf<255> sqlString;
		TBool readOK = EFalse;
		
		sqlString.Format(_L("SELECT DisplayName, NameField1, ContentField1, NameField2, ContentField2, NameField3, ContentField3, NameField4, ContentField4, NameField5, ContentField5 FROM Databank WHERE Index = %u"), aIndex);
		if (ReadDB(sqlString))
		{
			if (dbView.CountL() == 1)
			{
				dbView.FirstL();
				if (dbView.AtRow())
				{
					TBuf<1> lineBreak;
					lineBreak.Copy(_L(""));
					readOnlyDisplay->SetTextL(&lineBreak);
					lineBreak.Append(CEditableText::ELineBreak);

					dbView.GetL();
					readOnlyDisplay->InsertDeleteCharsL(0, dbView.ColDes(1), TCursorSelection(0, 0));
					readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), lineBreak, TCursorSelection(0, 0));
					readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), lineBreak, TCursorSelection(0, 0));
					for (int i = 0; i < NUM_DATA_FIELDS; i++)
					{
						// Because doWriteDB removes all the empty fields and shift the data upwards, 
						// we will never end up with a gap in the middle. Therefore, the moment
						// we see a gap we can assume we have reached the end of the record
						if (dbView.ColDes(2 + i * 2).Length() == 0)
							break;

						readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), dbView.ColDes(2 + i * 2), TCursorSelection(0, 0));
						sqlString.Copy(_L(":"));
						readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), sqlString, TCursorSelection(0, 0));						
						readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), lineBreak, TCursorSelection(0, 0));
						readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), dbView.ColDes(3 + i * 2), TCursorSelection(0, 0));
						readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), lineBreak, TCursorSelection(0, 0));
						readOnlyDisplay->InsertDeleteCharsL(readOnlyDisplay->TextLength(), lineBreak, TCursorSelection(0, 0));
					}
					readOnlyDisplay->SetSize(TSize(Size().iWidth, Size().iHeight - 10));
					readOnlyDisplay->MakeVisible(ETrue);
					readOnlyDisplay->SetFocus(ETrue);
					readOnlyDisplay->SetCursorPosL(0, EFalse);
					readOnlyDisplay->UpdateScrollBarsL();
					readOnlyDisplay->DrawNow();
					currentRecordIndex = aIndex;
					currentRecordIndexIsValid = ETrue;
					currentDBOp = DBOp_Viewing;
					readOK = ETrue;

					static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->SetToolbarL(R_TOOLBAR_DB_SINGLE);
					currentDBView = DBView_Single_ReadOnly;
				}
			}
		}
		if (!readOK)
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_RECORDMISSING);
			ShowRecordsList(ETrue, ETrue);
			return;
		}
	}
}

void CSMan2DBView::ShowRecordsSingle(TUint32 aIndex, TBool aIsNewRecord)
{
	TInt i;
	
	HideRecordsList();
	HideRecordsSingleReadOnly();
	currentRecordIndexIsValid = EFalse;
	
	if (!dbIsOpened)
		InitDB();

	TBuf<1> empty;
	empty.Copy(_L(""));
	for (i = 0; i < NUM_DATA_FIELDS; i++)
	{
		fieldLabels[i]->SetTextL(&empty);
		fieldLabels[i]->SetFocus(EFalse);
		fieldValues[i]->SetTextL(&empty);
		fieldValues[i]->SetFocus(EFalse);
	}
	fieldDisplayName->SetTextL(&empty);
		
	if (aIsNewRecord)
	{
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->SetToolbarL(R_TOOLBAR_DB_SINGLE);
		currentDBOp = DBOp_Adding;
		currentDBView = DBView_Single_Editable;
	}
	
	if ((!aIsNewRecord) && (dbIsOpened))
	{
		TBuf<255> sqlString;
		TBool readOK = EFalse;
		
		sqlString.Format(_L("SELECT DisplayName, NameField1, ContentField1, NameField2, ContentField2, NameField3, ContentField3, NameField4, ContentField4, NameField5, ContentField5 FROM Databank WHERE Index = %u"), aIndex);
		if (ReadDB(sqlString))
		{
			if (dbView.CountL() == 1)
			{
				dbView.FirstL();
				if (dbView.AtRow())
				{
					dbView.GetL();
					for (i = 0; i < NUM_DATA_FIELDS; i++)
					{
						sqlString.Copy(dbView.ColDes(2 + i * 2));
						fieldLabels[i]->SetTextL(&sqlString);
						sqlString.Copy(dbView.ColDes(3 + i * 2));
						fieldValues[i]->SetTextL(&sqlString);
					}
					sqlString.Copy(dbView.ColDes(1));
					fieldDisplayName->SetTextL(&sqlString);
					currentDBOp = DBOp_Editing;
					readOK = ETrue;
					currentRecordIndex = aIndex;
					currentRecordIndexIsValid = ETrue;
					
					static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->SetToolbarL(R_TOOLBAR_DB_SINGLE);
					currentDBView = DBView_Single_Editable;
				}
			}
		}
		
		if (!readOK)
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_RECORDMISSING);
			ShowRecordsList(ETrue, ETrue);
			return;
		}
	}
	dbView.Close();	
	scrollableContainer->MakeVisible(ETrue);
	scrollableContainer->SetFocus(ETrue, EDrawNow);
	// We've turned off the horizontal scrollbar and with the way we've positioned the controls, the 
	// vertical scrollbar will ALWAYS appear. However,  to be on the safe side we still check if the
	// vertical scrollbar is there
	TBool vertScroller, horizScroller;
	scrollableContainer->ScrollBarsNeeded(vertScroller, horizScroller);
	if (vertScroller)
	{
		scrollableContainer->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical)->OverrideColorL(EColorScrollBarShaft, KRgbCyan);
		scrollableContainer->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical)->OverrideColorL(EColorScrollThumbEdge, KRgbCyan);
		scrollableContainer->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical)->OverrideColorL(EColorScrollButtonThumbBackground, KRgbCyan);
		scrollableContainer->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical)->OverrideColorL(EColorScrollButtonThumbBackgroundPressed, KRgbCyan);
	}
	fieldDisplayName->SetFocus(ETrue);
}

TBool CSMan2DBView::doWriteDB()
{
	TBuf<50> buffer;
	TInt i;
	TBool retVal = EFalse;
	
	if (dbIsOpened)
	{
		fieldDisplayName->GetText(buffer);
		if (buffer.Length() > 0)
		{
			dbView.SetColL(2, buffer);
			TInt j = 0;
			for (i = 0; i < NUM_DATA_FIELDS; i++)
			{
				fieldLabels[i]->GetText(buffer);
				if (buffer.Length() > 0)
				{
					dbView.SetColL(3 + j * 2, buffer);
					fieldValues[i]->GetText(buffer);
					dbView.SetColL(4 + j * 2, buffer);
					j++;
				}
				else
				{
					dbView.SetColL(3 + j * 2, _L(""));
					dbView.SetColL(4 + j * 2, _L(""));
				}
			}
			TRAPD(err, dbView.PutL());
			if (err)
			{
				dbView.Cancel();
				dbView.Reset();
				dbView.Close();
				CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_SAVE);
			}
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_NO_DISPLAYNAME);
		dbView.Close();
		retVal = ETrue;
	}
	return retVal;
}

TBool CSMan2DBView::SaveRecord(TUint32 aIndex)
{
	TBuf<50> sqlString;
	TBool res = EFalse;

	if (!dbIsOpened)
		InitDB();
		
	if (dbIsOpened)
	{
		if (currentRecordIndexIsValid)
		{
			if (fieldDisplayName->TextLength() == 0)
			{
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_DELETE_TITLE);
				HBufC* dataBuffer2 = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_DELETE);
				if (CEikonEnv::Static()->QueryWinL(*dataBuffer, *dataBuffer2))
					res = DeleteRecord(aIndex);
				delete dataBuffer;
				delete dataBuffer2;
			}
			else
			{
				// Update the record
				sqlString.Format(_L("SELECT * FROM Databank WHERE Index = %u"), aIndex);
				if (ReadDB(sqlString))
				{
					dbView.FirstL();
					if (dbView.AtRow())
					{
						dbView.UpdateL();
						res = doWriteDB();
					}
				}
				dbView.Close();
			}
		}
		else
		{
			// Insert new record
			sqlString.Copy(_L("SELECT * FROM Databank WHERE Index = 0"));
			if (ReadDB(sqlString))
			{
				dbView.InsertL();
				res = doWriteDB();
			}
			dbView.Close();
		}
	}
	return res;
}

void CSMan2DBView::ConstructL(const TRect& aRect)
{
	oldItem = -1;
	currentRecordIndex = 0;
	currentRecordIndexIsValid = EFalse;
	dbWindow = NULL;
	fileStore = NULL;
	
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	securityBase = NULL;
	decryptor = NULL;
	encryptor = NULL;
	dbPassword.Copy(DEFAULT_PASSWORD);

	//TFileName rscFile;
	//RResourceFile resourceFile;

#ifdef __WINS__
	dbFileName.Copy(_L("D:\\SMAN.DB"));
#else
	dbFileName.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName());
	dbFileName.SetLength(dbFileName.Length() - 3);
	dbFileName.Append(_L("DB"));
#endif
	
	TResourceReader resourceReader;
	HBufC8* dataBuffer = CEikonEnv::Static()->AllocReadResourceAsDes8L(R_ARRAY_DB_FIELDNAME);
	resourceReader.SetBuffer(dataBuffer);
	fieldLabelsArray = resourceReader.ReadDesCArrayL();
	delete dataBuffer;
	
	// Initialize the multi record view
	cIndexListBox = new(ELeave) CEikColumnListBox;
	cIndexListBox->ConstructL(this, CEikColumnListBox::EMultipleSelection);
	CColumnListBoxData* listBoxColumns = ((CColumnListBoxItemDrawer*)cIndexListBox->ItemDrawer())->ColumnData();
	listBoxColumns->SetColumnWidthPixelL(0, int(Size().iWidth * 0.9));
	listBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	// Hidden column. Stores the index values of each row.
	listBoxColumns->SetColumnWidthPixelL(1, 0);
	listBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ELeft);
	cIndexListBox->CreateScrollBarFrameL();
	cIndexListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	cIndexListBox->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, aRect.Size().iHeight - EQikToolbarHeight - 4));
	controlsArray->AppendL(cIndexListBox);

	// Initialize the single record view - readonly
	readOnlyDisplay = new (ELeave) CGlobalTextEditor;
	readOnlyDisplay->ConstructL(this, 9999, 0, CEikEdwin::EReadOnly | CEikEdwin::EDisplayOnly | CEikEdwin::EInclusiveSizeFixed, EGulFontControlAll, EGulAllFonts);
	readOnlyDisplay->SetExtent(TPoint(0, 0), TSize(1, 1));
	//Size().iWidth, Size().iHeight - EQikToolbarHeight - 10));
	readOnlyDisplay->CreateScrollBarFrameL();
	readOnlyDisplay->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	controlsArray->AppendL(readOnlyDisplay);

	// Initialize the single record view - non readonly 
	scrollableContainer = new (ELeave) CQikScrollableContainer;
	scrollableContainer->SetContainerWindowL(*this);
	scrollableContainer->ConstructL(ETrue);
	scrollableContainer->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	scrollableContainer->OverrideColorL(EColorWindowBackground, KRgbCyan);
	scrollableContainer->SetBorder(TGulBorder::ESingleGray);
	controlsArray->AppendL(scrollableContainer);

	fieldDisplayName = new (ELeave) CGlobalTextEditor;
	fieldDisplayName->ConstructL(scrollableContainer, 1, 30, CEikEdwin::ENoAutoSelection, EGulFontControlAll, EGulAllFonts);
	scrollableContainer->AddControlL(fieldDisplayName);
	fieldDisplayName->SetExtent(TPoint(0, 0), TSize(1, 1));
	//fieldDisplayName->SetExtent(TPoint(2, 3), TSize(Size().iWidth - scrollableContainer->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EHorizontal) - 2 - 5, 25));
	
	TBuf<2> numberLabel;
	//TInt previousPos = fieldDisplayName->Position().iY + fieldDisplayName->Size().iHeight + 3;
	for (int i = 0; i < NUM_DATA_FIELDS; i++)
	{
		fieldLabelsNumbering[i] = new (ELeave) CEikLabel;
		numberLabel.Format(_L("%d"), i + 1);
		fieldLabelsNumbering[i]->SetTextL(numberLabel);
		fieldLabelsNumbering[i]->SetContainerWindowL(*scrollableContainer);
		scrollableContainer->AddControlL(fieldLabelsNumbering[i]);

		fieldLabelsNumbering[i]->SetExtent(TPoint(0, 0), TSize(1, 1));

		fieldLabels[i] = new (ELeave) CComboBox;
		fieldLabels[i]->ConstructL(*scrollableContainer, 10, 30, 6);
		fieldLabels[i]->SetArray(fieldLabelsArray, CEikComboBox::EArrayExternalOwner);
		// you MUST add the control PRIOR to setting its extent! otherwise, the scrollable container will
		// do crazy things with the control. to be more exact, it's not the extent but the position of
		// the control
		scrollableContainer->AddControlL(fieldLabels[i]);
		fieldLabels[i]->SetExtent(TPoint(0, 0), TSize(1, 1));
		//fieldLabels[i]->SetExtent(TPoint(fieldLabelsNumbering[i]->Position().iX + fieldLabelsNumbering[i]->Size().iWidth + 2, previousPos), 
		//	TSize(Size().iWidth - scrollableContainer->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EHorizontal) - 5 - 2 - fieldLabelsNumbering[i]->Position().iX - fieldLabelsNumbering[i]->Size().iWidth, 25));

		fieldValues[i] = new (ELeave) CGlobalTextEditor;
		fieldValues[i]->ConstructL(scrollableContainer, 20, 50, CEikEdwin::ENoAutoSelection, EGulFontControlAll, EGulAllFonts);
		fieldValues[i]->CreateScrollBarFrameL();
		fieldValues[i]->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
		scrollableContainer->AddControlL(fieldValues[i]);
		fieldValues[i]->SetExtent(TPoint(0, 0), TSize(1, 1));
		//fieldValues[i]->SetExtent(TPoint(fieldLabels[i]->Position().iX, fieldLabels[i]->Position().iY + fieldLabels[i]->Size().iHeight + 2), 
		//	TSize(Size().iWidth - scrollableContainer->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EHorizontal) - 2 - 5, 60));
		
		//previousPos = fieldValues[i]->Position().iY + fieldValues[i]->Size().iHeight + 10;
	}
	fieldDisplayName->SetFocus(ETrue);
	//TSize minSize = scrollableContainer->MinimumPageSize();
	//scrollableContainer->SetPageSize(minSize);
	scrollableContainer->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, aRect.Size().iHeight - EQikToolbarHeight - 4));
	//scrollableContainer->UpdateScrollBarsL();
	//scrollableContainer->SetComponentsToInheritVisibility(ETrue);

	viewId = CSMan2AppUi::EViewDB;
	iBJackIconPosition = TPoint(2, scrollableContainer->Position().iY + scrollableContainer->Size().iHeight);
	InitBJackIcon();

	//ShowRecordsList(ETrue, EFalse);
	currentDBView = DBView_List;
	HideAllControls();
	//ShowRecordsList(EFalse, EFalse);
	
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
		SetZoomLevel(configData->zoomDB);
	else
	{
		font = (CFont *)(CEikonEnv::Static()->NormalFont());
		PositionRecordsSingleControls();
		font = NULL;
	}
		
	MakeVisible(EFalse);
	ActivateL();
	DeInitDB();
}

void CSMan2DBView::HideAllControls()
{
	HideRecordsList();
	HideRecordsSingle();
	HideRecordsSingleReadOnly();
}

void CSMan2DBView::PositionRecordsSingleControls()
{
	fieldDisplayName->SetExtent(TPoint(2, 3), TSize(Size().iWidth - scrollableContainer->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EHorizontal) - 2 - 5, font->HeightInPixels() * 2 + 2));
	TInt previousLabelPos = fieldDisplayName->Position().iY + fieldDisplayName->Size().iHeight + 3;
	for (int i = 0; i < NUM_DATA_FIELDS; i++)
	{
		fieldLabelsNumbering[i]->SetExtent(TPoint(2, previousLabelPos), TSize(font->CharWidthInPixels(TChar(38)) + 1, font->HeightInPixels() * 2 + 2));
		fieldLabels[i]->SetExtent(TPoint(fieldLabelsNumbering[i]->Position().iX + fieldLabelsNumbering[i]->Size().iWidth, previousLabelPos), 
			TSize(Size().iWidth - scrollableContainer->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EHorizontal) - 2 - 5 - fieldLabelsNumbering[i]->Position().iX - fieldLabelsNumbering[i]->Size().iWidth, font->HeightInPixels() * 2 + 2));
		fieldValues[i]->SetExtent(TPoint(fieldLabelsNumbering[i]->Position().iX, fieldLabels[i]->Position().iY + fieldLabels[i]->Size().iHeight + 2), 
			TSize(Size().iWidth - scrollableContainer->ScrollBarFrame()->ScrollBarBreadth(CEikScrollBar::EHorizontal) - 2 - 5, font->HeightInPixels() * 5));
		previousLabelPos = fieldValues[i]->Position().iY + fieldValues[i]->Size().iHeight + 10;
	}
	scrollableContainer->SetPageSize(scrollableContainer->MinimumPageSize());
	scrollableContainer->UpdateScrollBarsL();
}

void CSMan2DBView::SetZoomLevel(TInt zoomLevel)
{
	// DEBUG
	releaseFont();
	generateZoomFont(zoomLevel);
	
	// Set list view zoom
	CTextListItemDrawer* itemDrawer = ((CTextListItemDrawer*)cIndexListBox->View()->ItemDrawer());
	itemDrawer->SetFont(font);
	cIndexListBox->SetItemHeightL(font->HeightInPixels() + cIndexListBox->VerticalInterItemGap());

	// Set single view zoom
	TInt lineSpaceTwips[3] = {150, 200, 300};
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	
	// Formatting for display name and field value
	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;
	
	charFormat.iFontSpec.iHeight = font->FontSpecInTwips().iHeight;
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
	charFormatMask.SetAttrib(EAttFontHeight);
	charFormatMask.SetAttrib(EAttFontStrokeWeight);
	paraFormat.iLineSpacingInTwips = lineSpaceTwips[(zoomLevel / ZOOM_INCREMENT) - 1];
	paraFormatMask.SetAttrib(EAttLineSpacing);

	fieldDisplayName->ApplyCharFormatL(charFormat, charFormatMask);
	fieldDisplayName->ApplyParaFormatL(&paraFormat, paraFormatMask);
	
	// Set single readonly view zoom
	readOnlyDisplay->ApplyCharFormatL(charFormat, charFormatMask);
	readOnlyDisplay->ApplyParaFormatL(&paraFormat, paraFormatMask);

	// Formatting for field labels
	TCharFormat labelCharFormat;
	TCharFormatMask labelCharFormatMask;
	CParaFormat labelParaFormat;
	TParaFormatMask labelParaFormatMask;
	TInt labelLineSpaceTwips[3] = {150, 200, 350};

	labelCharFormat.iFontSpec.iHeight = font->FontSpecInTwips().iHeight;
	labelCharFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
	labelCharFormatMask.SetAttrib(EAttFontHeight);
	labelCharFormatMask.SetAttrib(EAttFontStrokeWeight);
 	labelParaFormat.iLineSpacingInTwips = labelLineSpaceTwips[(zoomLevel / ZOOM_INCREMENT) - 1];;
	labelParaFormatMask.SetAttrib(EAttLineSpacing);

	CCharFormatLayer *charFormatLayer = CCharFormatLayer::NewL(((const TCharFormat)labelCharFormat), (const TCharFormatMask)labelCharFormatMask);
	CParaFormatLayer *paraFormatLayer = CParaFormatLayer::NewL(&labelParaFormat, (const TParaFormatMask)labelParaFormatMask);
		
	TBuf<50> buffer;
	for (int i = 0; i < NUM_DATA_FIELDS; i++)
	{
		fieldLabelsNumbering[i]->SetFont(font);
		
		fieldLabels[i]->GetText(buffer);
		CGlobalText *globalText = CGlobalText::NewL(paraFormatLayer, charFormatLayer);
		globalText->InsertL(0, buffer);
		
		// This is what the SDK says
		//-------------------------------
		//Sets the Edwin’s editable content. Before calling this function you must get the iText 
		//pointer as iText is replaced by aText. You must also free the memory by deleting the 
		//previous iText contents. 
		//-------------------------------
		// Somehow, when i tried what the SDK suggested, it doesn't work. The delete operation
		// succeeds but the call to SetDocumentContentL crashes
		//delete fieldLabels[i]->Edwin()->Text();
		fieldLabels[i]->Edwin()->SetDocumentContentL(*globalText);
		delete globalText;
		fieldLabels[i]->theFont = font;
		
		fieldValues[i]->ApplyCharFormatL(charFormat, charFormatMask);
		fieldValues[i]->ApplyParaFormatL(&paraFormat, paraFormatMask);	
	}
	
	// Reposition all the controls
	PositionRecordsSingleControls();
	
	delete charFormatLayer;
	delete paraFormatLayer;
	
	//DEBUG releaseFont();
	configData->zoomDB = zoomLevel;
}

void CSMan2DBView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}

void CSMan2DBView::ClipboardAction(CEikEdwin *aEdwin, TInt aCommand)
{
	CClipboard *oClipboard;

	if ((aCommand == cmdEditCopy) || (aCommand == cmdEditCut))
	{
		if (aEdwin->SelectionLength() > 0)
		{
			oClipboard = CClipboard::NewForWritingLC(CEikonEnv::Static()->FsSession());
			aEdwin->CopyToStoreL(oClipboard->Store(), oClipboard->StreamDictionary());
			oClipboard->CommitL();			
			if (aCommand == cmdEditCut)
			{
				aEdwin->InsertDeleteCharsL(0, _L(""), aEdwin->Selection());
				aEdwin->ClearSelectionL();
			}
			CleanupStack::PopAndDestroy(); // oClipboard
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_EDIT_NOTEXTSELECTED);
	}
	else if (aCommand == cmdEditPaste)
	{
		oClipboard = CClipboard::NewForReadingLC(CEikonEnv::Static()->FsSession());
		aEdwin->PasteFromStoreL(oClipboard->Store(), oClipboard->StreamDictionary());
		CleanupStack::PopAndDestroy(); // oClipboard	
	}
}

void CSMan2DBView::DoEditCommands(TInt aCommand)
{
	TBool focusFound = EFalse, focusFoundInLabel = EFalse;
	TInt i;
	
	for (i = 0; i < NUM_DATA_FIELDS; i++)
	{
		if (fieldValues[i]->IsFocused())
		{
			focusFound = ETrue;
			break;
		}
		else if (fieldLabels[i]->IsFocused())
		{
			focusFoundInLabel = ETrue;
			break;
		}
	}
	
	if (focusFound)
		ClipboardAction(fieldValues[i], aCommand);
	else if (focusFoundInLabel)
		ClipboardAction(fieldLabels[i]->Edwin(), aCommand);
	else if (fieldDisplayName->IsFocused())
		ClipboardAction(fieldDisplayName, aCommand);
}

void CSMan2DBView::swapFocus()
{
	TInt j;
	TBool focusFound = EFalse, focusFoundInLabel = EFalse;
	
	for (j = 0; j < NUM_DATA_FIELDS; j++)
	{
		if (fieldLabels[j]->IsFocused())
		{
			fieldLabels[j]->SetFocus(EFalse);
			fieldLabels[j]->Edwin()->SetCursorPosL(0, EFalse);
			focusFound = ETrue;
			focusFoundInLabel = ETrue;
			break;
		}
		else if (fieldValues[j]->IsFocused())
		{
			fieldValues[j]->SetFocus(EFalse);
			fieldValues[j]->SetCursorPosL(0, EFalse);
			focusFound = ETrue;
			break;
		}
	}
	if (!focusFound)
	{
		if (fieldDisplayName->IsFocused())
		{
			fieldDisplayName->SetFocus(EFalse);
			fieldDisplayName->SetCursorPosL(0, EFalse);
		}
	}
	
	// Update the history list of the combo box
	if (focusFoundInLabel)
	{
		TBuf<50> currentEntry;
		
		fieldLabels[j]->GetText(currentEntry);
		if (currentEntry.Length() != 0)
		{
			TBool found = EFalse;
			TInt i;
			
			for (i = 0; i < fieldLabelsArray->MdcaCount(); i++)
			{
				if (fieldLabelsArray->MdcaPoint(i).Compare(currentEntry) == 0)
				{
					found = ETrue;
					break;
				}
			}
			if (!found)
			{
				fieldLabelsArray->InsertL(0, currentEntry);
				fieldLabelsArray->Delete(fieldLabelsArray->MdcaCount() - 1);
			}
		}
	}
}

/*************************************************************
*
* Behaves like normal except 
* 1) when focus is lost it will bring the cursor of the edwin to the front
* 2) it will call the dbview to unfocus the previous control
* 3) supports zooming of the list box
*
**************************************************************/

CComboBox::~CComboBox()
{
}

CComboBox::CComboBox() : CEikComboBox()
{
	popoutNew = EFalse;
}

void CComboBox::SetTextListBoxZoom()
{
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
	{
		CTextListItemDrawer* itemDrawer = ((CTextListItemDrawer*)iPopout->View()->ItemDrawer());
		itemDrawer->SetFont(theFont);
		iPopout->SetItemHeightL(theFont->HeightInPixels() + iPopout->VerticalInterItemGap());
		while (iPopout->View()->NumberOfItemsThatFitInRect(iPopout->Size()) > iPopout->Model()->ItemTextArray()->MdcaCount())
			iPopout->SetSize(TSize(iPopout->Size().iWidth, iPopout->Size().iHeight - iPopout->ItemHeight()));
		iPopout->DrawNow();
	}
}

void CComboBox::HandleControlEventL(CCoeControl *aControl, TCoeEvent aEvent)
{
	CEikComboBox::HandleControlEventL(aControl, aEvent);
	if (iPopout)
	{
		if (!popoutNew)
		{
			popoutNew = ETrue;
			SetTextListBoxZoom();
		}
	}
	else
		popoutNew = EFalse;
}

void CComboBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	if ((aPointerEvent.iType == TPointerEvent::EButton1Down) && (!IsFocused()))
	{
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iDBView->swapFocus();
		SetFocus(ETrue);
	}
	CEikComboBox::HandlePointerEventL(aPointerEvent);
}

/*************************************************************
*
* As per the combo box class but without zooming support since this is already
* inherent in the class
*
**************************************************************/

CGlobalTextEditor::CGlobalTextEditor() : CEikGlobalTextEditor()
{
}

void CGlobalTextEditor::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	if ((aPointerEvent.iType == TPointerEvent::EButton1Down) && (!IsFocused()))
	{
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iDBView->swapFocus();
		SetFocus(ETrue);
	}
	CEikGlobalTextEditor::HandlePointerEventL(aPointerEvent);
}
