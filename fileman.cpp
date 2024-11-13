#include "fileman.h"
#include "sman.h"
#include <SMan.rsg>

/*************************************************************
*
* File manager class and observer
*
**************************************************************/

CSManFileMan::~CSManFileMan()
{
	delete fileMan;
	delete logObj;
	fileServer.Close();
}

CSManFileMan::CSManFileMan(CConfig *cData)
{
	User::LeaveIfError(fileServer.Connect());
	fileMan = CFileMan::NewL(fileServer);
	fileMan->SetObserver(this);
	logObj = new (ELeave) CLogger(cData);
}

void CSManFileMan::setupDialog()
{
	progressDialog = new (ELeave) CFileManLogDialog();
	progressDialog->progressValue = -1;
	progressDialog->ExecuteLD(R_DIALOG_FILEMANPROGRESS_NOWAIT);
	progressText = STATIC_CAST(CEikGlobalTextEditor*, progressDialog->Control(cFileManProgressText));
	progressBar = STATIC_CAST(CEikProgressInfo*, progressDialog->Control(cFileManProgressBar));
	progressBar->SetFinalValue(100);
	progressDialog->ButtonGroupContainer().DimCommand(EEikBidOverwrite, ETrue);
	progressDialog->ButtonGroupContainer().DimCommand(EEikBidOverwriteAll, ETrue);
	progressDialog->ButtonGroupContainer().DimCommand(EEikBidSkip, ETrue);
	progressDialog->ButtonGroupContainer().DimCommand(EEikBidSkipAll, ETrue);
	progressDialog->ButtonGroupContainer().DimCommand(EEikBidAbort, ETrue);
	progressDialog->DrawNow();
}

void CSManFileMan::doWork()
{
	TInt res;
	TPath originalPath;
		
	originalPath.Copy(targetPath);
	userRet = 0;
	prevOverwriteStatus = KErrNone;
	progressDialog = NULL;
	
	if (currentOperation == isRename)
	{
		CFileManRenDialog *renDlg;
		for (currentFile = 0; currentFile < fileList->Count(); currentFile++)
		{
			TParsePtr pName(fileList->At(currentFile));
			renDlg = new (ELeave) CFileManRenDialog();
			renDlg->fileName.Copy(pName.NameAndExt());
			renDlg->filePath.Copy(pName.DriveAndPath());
			if (renDlg->ExecuteLD(R_DIALOG_FILEMANRENAME) == EEikBidAbort)
				 break;
		}
	}
	else if (currentOperation == isDelete)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_DELETE_TITLE);
		HBufC* dataBuffer2 = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_DELETE);
		if (CEikonEnv::Static()->QueryWinL(*dataBuffer, *dataBuffer2))
		{
			// Again, I'm just paranoid
			delete dataBuffer;
			delete dataBuffer2;
			
			setupDialog();
			for (currentFile = 0; currentFile < fileList->Count(); currentFile++)
			{
				if (EikFileUtils::FolderExists(fileList->At(currentFile)))
				{
					TFileName pop;
					pop.Copy(fileList->At(currentFile));
					pop.Append(_L("\\"));
					res = fileMan->RmDir(pop);
				}
				else
					res = fileMan->Delete(fileList->At(currentFile), 0);
				if (userRet == EEikBidAbort)
					break;
			}
		}
		else
		{
			delete dataBuffer;
			delete dataBuffer2;
		}
	}
	else if (currentOperation == isPaste)
	{
		if (fileList->Count() > 0)
		{
			TParsePtr temp(fileList->At(0));

			if (temp.DriveAndPath() == targetPath)
				CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_SAMETARGETSOURCE);
			else
			{
				setupDialog();
				TFileName sourceFile, targetFile;
				TBool moveFolderToDiffDrive, sourceIsFolder;
				for (currentFile = 0; currentFile < fileList->Count(); currentFile++)
				{
					moveFolderToDiffDrive = EFalse;
					sourceIsFolder = EFalse;
				  	sourceFile.Copy(fileList->At(currentFile));
				  	targetFile.Copy(targetPath);
				  	opSwitch = 0;

					if (EikFileUtils::FolderExists(sourceFile))
					{
						opSwitch = CFileMan::ERecurse;
						sourceIsFolder = ETrue;
						sourceFile.Append(_L("\\"));
						targetFile.Append(EikFileUtils::FolderNameFromFullName(sourceFile));
						targetFile.Append(_L("\\"));
						
						/*
						the == operator isn't so safe to use with objects but is a nice shortcut. :)
						
						Here, checking if we are moving a folder to a different drive.
						There is a bug in CFileMan::Move() that can cause you to lose files.
						Here's my post in the symbian newsgroups:
						
						i've tested this using:
						1) my own implementation (on emulator and real device)
						2) qfileman on real device (on P800)
						3) qfileman (on emulator)
						4) tracker v2.0 (on P800)
						
						all exhibit the same behaviour.
						
						this is how you reproduce the problem:
						1) go to c:\ and create a folder called c:\temp
						2) under that, create another folder called c:\temp\ppp
						3) under that, create another folder called c:\temp\ppp\kkk
						4) under c:\temp\ppp\kkk, create 2 files
						5) go to d:\ and create a folder called d:\aaa
						6) prior to calling CFileMan::Move(), your code should check if the source
						is a folder and if so, it should create the target folder i.e. D:\AAA\PPP\.
						i found that CFileMan::Move() needs to have the target existing first, which
						is fine.
						7) at this point, D:\AAA should have a subfolder in it called D:\AAA\PPP
						8) call CFileMan::Move() with the following parameters
						    - source = C:\TEMP\PPP\
						    - target = D:\AAA\PPP\
						    - flag = CFileMan::ERecurse
						9) the source is empty and the target only has D:\AAA\PPP, which is also
						empty. all the files/folders are gone!
						
						some testing seems to reveal that
						1) this only happens when moving across drives
						2) it looks as though CFileMan::Move() is ignoring the recursive flag
						somehow when dealing with the target but not the source. when i dump a file
						in C:\TEMP\PPP, that file (AND ONLY THAT FILE) gets MOVEd across
						successfully. everything else under C:\TEMP\PPP\KKK doesn't get operated on.
						
						is this a bug?? can someone from Symbian confirm this??
						
						i have a workaround but error handling isn't exactly elegant all the time.
						still, it gets the job done.
						*/

						if (fileManOperation == CSManFileMan::operationCut)
						{
							if (!EikFileUtils::FolderExists(targetFile))
								fileServer.MkDir(targetFile);

							if (sourceFile.Left(1).FindC(targetFile.Left(1)) != 0)
							{
								// The workaround to the move problem is to do a COPY to target then 
								// DEL the source
								moveFolderToDiffDrive = ETrue;
							}
						}
					}
					
					// Check if destination is subfolder of source
					if (targetFile.Left(sourceFile.Length()) != sourceFile)
					{
						// Nope. Operation is valid.
						if (fileManOperation == CSManFileMan::operationCopy)
							res = fileMan->Copy(sourceFile, targetFile, opSwitch);
						else if (fileManOperation == CSManFileMan::operationCut)
						{
							if (!moveFolderToDiffDrive)
							{
								res = fileMan->Move(sourceFile, targetFile, opSwitch);
								if ((sourceIsFolder) && (res == KErrNone))
								{
									res = fileMan->RmDir(sourceFile);
								}
							}
							else
							{
								res = fileMan->Copy(sourceFile, targetFile, opSwitch);
								res = fileMan->RmDir(sourceFile);
							}
						}
					}
					else
					{
						HBufC* dataBuffer = NULL;
						
						// Notify the user
						if (fileManOperation == CSManFileMan::operationCopy)
							dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_COPYING);
						else if (fileManOperation == CSManFileMan::operationCut)
							dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_MOVING);
						progressText->SetTextL(dataBuffer);
						progressText->InsertDeleteCharsL(progressText->TextLength(), fileList->At(currentFile), TCursorSelection(0, 0));
						progressText->InsertDeleteCharsL(progressText->TextLength(), _L(" -> "), TCursorSelection(0, 0));
						progressText->InsertDeleteCharsL(progressText->TextLength(), targetPath, TCursorSelection(0, 0));
						dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + 5);
						delete dataBuffer;
						TBuf<5> temp;
						temp.Copy(_L(""));
						temp.Append(CEditableText::ELineBreak);
						temp.Append(CEditableText::ELineBreak);
						progressText->InsertDeleteCharsL(progressText->TextLength(), temp, TCursorSelection(0, 0));
						dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_SAMELOCATION);
						progressText->InsertDeleteCharsL(progressText->TextLength(), *dataBuffer, TCursorSelection(0, 0));
						delete dataBuffer;
						displayPromptDialog(EFalse);
					}
					if (userRet == EEikBidAbort)
						break;
				}
			}
		}
	}
	if (!((currentOperation == isPaste) && (fileManOperation == CSManFileMan::operationCopy)))
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->ClearClipBoard();
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->ReadAlternatePath(&originalPath);
	if (progressDialog)
		delete progressDialog; // close the dialog
	User::Heap().Compress();
}

void CSManFileMan::displayPromptDialog(TBool showOverwrite)
{
	updateErrorMsg(progressText);
	TPtrC text = progressText->Text()->Read(0, progressText->TextLength());
	CFileManLogDialog *askUser = new (ELeave) CFileManLogDialog();
	askUser->progressValue = progressBar->CurrentValue();
	askUser->text.Set(text);
	askUser->showOverwrite = showOverwrite;
	// Fading flag set and unset to reduce screen flicker
	progressDialog->DrawableWindow()->SetNonFading(ETrue);
	userRet = askUser->ExecuteLD(R_DIALOG_FILEMANPROGRESS_WAIT);
	progressDialog->DrawableWindow()->SetNonFading(EFalse);
	progressDialog->DrawNow();
}

void CSManFileMan::updateText(CEikGlobalTextEditor *textEdit)
{
	HBufC *opDesc;
	TFileName srcFile, trgFile;
	
	switch (fileMan->CurrentAction())
	{
		case CFileMan::ECopy: 
			opDesc = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_COPYING);
			break;
		case CFileMan::EDelete:
			opDesc = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_DELETING);
			break;
		case CFileMan::ERmDir:
			opDesc = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_DELETING);
			break;
		case CFileMan::EMove: 
			opDesc = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_MOVING);
			break;
		case CFileMan::ERename: 
			if (currentOperation != isPaste)
				opDesc = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_RENAMING);
			else
				opDesc = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_DELETING);
			break;
		default: 
			opDesc = HBufC::NewL(2);
			opDesc->Des().Copy(_L(""));
			break;
	}
	textEdit->SetTextL(opDesc);
	delete opDesc;
	fileMan->GetCurrentSource(srcFile);
	
	textEdit->InsertDeleteCharsL(textEdit->TextLength(), srcFile, TCursorSelection(0, 0));
	if ((fileMan->CurrentAction() == CFileMan::ECopy) || (fileMan->CurrentAction() == CFileMan::EMove) || (fileMan->CurrentAction() == CFileMan::ERename))
	{
		fileMan->GetCurrentTarget(trgFile);
		textEdit->InsertDeleteCharsL(textEdit->TextLength(), _L(" -> "), TCursorSelection(0, 0));
		textEdit->InsertDeleteCharsL(textEdit->TextLength(), trgFile, TCursorSelection(0, 0));
	}
	textEdit->SetCursorPosL(textEdit->TextLength(), 0);
	textEdit->DrawNow();
}

void CSManFileMan::updateErrorMsg(CEikGlobalTextEditor *textEdit)
{
	TInt err = fileMan->GetLastError();
	if (err == KErrNone)
		return;
		
	TFileManError error = fileMan->GetMoreInfoAboutError();
	
	HBufC *errorMsg = NULL;
	if (error == EInitializationFailed)
		errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_INITFAILURE);
	else if (error == EScanNextDirectoryFailed)
		errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRSCANSUBFOLDER);
	else if (error == ESrcOpenFailed)
		errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERROPENSOURCE);
	else if ((error == ETrgOpenFailed) || (err == KErrAlreadyExists))
		errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_CANTWRITETARGET);
	else if (error == ENoExtraInformation)
	{
		// Not sure if ALL of these will ever appear. I've included them only because
		// the constant name seems reasonable....
		if (err == KErrPathNotFound)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRPATHNOTFOUND);
		else if (err == KErrWrite)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRWRITE);
		else if (err == KErrDiskFull)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRDISKFULL);
		else if (err == KErrAccessDenied)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRACCESSDENIED);
		else if (err == KErrNotReady)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRDEVNOTREADY);
		else if (err == KErrLocked)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRFILELOCK);
		else if (err == KErrDisMounted)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRDEVDISMOUNT);
		else if (err == KErrDirFull)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRDIRFULL);
		else if (err == KErrInUse)
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRSOURCELOCK);
		else
		{
			TBuf<10> genErr;
			genErr.Format(_L("%d"), err);
			errorMsg = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRGENERAL);
			errorMsg->Des().Append(genErr);
		}
	}
	TBuf<5> newLine;
	newLine.Append(CEditableText::ELineBreak);
	newLine.Append(CEditableText::ELineBreak);
	textEdit->InsertDeleteCharsL(textEdit->TextLength(), newLine, TCursorSelection(0, 0));
	textEdit->InsertDeleteCharsL(textEdit->TextLength(), *errorMsg, TCursorSelection(0, 0));
	textEdit->DrawNow();
	delete errorMsg;
}

MFileManObserver::TControl CSManFileMan::NotifyFileManStarted()
{
	progressBar->SetAndDraw(0);
	updateText(progressText);
	progressText->DrawNow();
	currentBytes = 0;
	totalBytes = fileMan->CurrentEntry().iSize;
	return MFileManObserver::EContinue;
}

MFileManObserver::TControl CSManFileMan::NotifyFileManOperation()
{
	currentBytes += fileMan->BytesTransferredByCopyStep();
	if (totalBytes > 0)
		progressBar->SetAndDraw(TInt((TReal(currentBytes) / TReal(totalBytes)) * 100));
	return MFileManObserver::EContinue;
}

MFileManObserver::TControl CSManFileMan::NotifyFileManEnded()
{
	MFileManObserver::TControl retVal;
	
	retVal = MFileManObserver::EContinue;
	TInt err = fileMan->GetLastError();
	if (err != KErrNone)
	{
		if (userRet == EEikBidSkipAll)
			return MFileManObserver::ECancel;

		//TFileManError error = fileMan->GetMoreInfoAboutError();
		//updateErrorMsg(progressText, err, error);
		
		// Must be careful with this dialog. We can get stuck in an endless loop:
		// 1. error occurs. user choses to overwrite all
		// 2. the overwrite operation fails
		// 3. since overwrite all is set, it will try to overwrite again
		// 4. but overwrite fails. loop back to #3.
		// So we have to check the flags carefully. If overwrite fails while overwrite
		// all is set, we prompt the user again
//		if ((userRet != EEikBidOverwriteAll) && (fileMan->GetLastError() == KErrAlreadyExists))
		if ((userRet != EEikBidOverwriteAll) || ((userRet == EEikBidOverwriteAll) && (prevOverwriteStatus != KErrNone)))
		{
			if (currentOperation == isPaste)
				displayPromptDialog(ETrue);
			else
				displayPromptDialog(EFalse);
		}
		
		if ((userRet == EEikBidOverwrite) || (userRet == EEikBidOverwriteAll))
		{
			// delete the target
			TFileName trgFile;
			fileMan->GetCurrentTarget(trgFile);
			if (fileMan->CurrentEntry().IsDir())
			{
				trgFile.Append(_L("\\"));
				CFileMan *tempFM;
				tempFM = CFileMan::NewL(fileServer);
				prevOverwriteStatus = tempFM->RmDir(trgFile);
				delete tempFM;
			}
			else
				prevOverwriteStatus = EikFileUtils::DeleteFile(trgFile);
			retVal = MFileManObserver::ERetry;
		}
		else if (userRet == EEikBidSkip)
			retVal = MFileManObserver::ECancel;
		else if (userRet == EEikBidAbort)
			retVal = MFileManObserver::EAbort;
	}
	progressBar->SetAndDraw(100);
	return retVal;
}