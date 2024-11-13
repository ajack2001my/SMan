#include "viewbase.h"
#include "sman.h"
#include <sman.rsg>

/*************************************************************
*
* HexEditor view.
*
* The hexedit's main view is implemented using two CEikEdwin
* One editor contains the byte display and the other contains 
* the ASCII display. This gives us the benefit of cut, paste, 
* select, delete etc.
*
* Due to size limitations on the device screen, we depart from
* the traditional hexeditor view of left side for bytes, right
* side for ASCII to top for bytes, bottom for ASCII. This means
* there is no direct coordinate correspondance of byte to
* ASCII character positions between the top and bottom edwins
*
* This class implements its own custom text wrapper. The text 
* wrapper disables wrapping but not like the edwin's no-wrapping 
* algorithm (which basically creates a horizontal scrollbar). 
* This custom text wrapper wraps the text at the horizontal 
* boundary on a character basis (any character)
*
* The ASCII control only allows pointer taps and drag events. You
* CANNOT edit its contents. Contents can only be edited in the hex
* control. The hex control also supports pointer taps and drag 
* events.
*
* When inserting bytes into the hex control, you can insert up to
* a maximum of twice the size of the read buffer. When deleting bytes,
* you can delete the entire page away!
*
* I made a mistake in the class design but am too lazy to rewrite
* it...the hexeditor window should be integrated with the ascii
* window into one single CCoeControl-subclassed class. sheesh...
* WHAT WAS I THINKING?!?!?
*
**************************************************************/

CSMan2HexEditorView::CSMan2HexEditorView(CConfig *cData) : CViewBase(cData)
{
	iFileSize = -1;
	iCurrentFilePageOffset = 0;
	iFileName.Copy(_L(""));
	iFileIsOpen = EFalse;
	iDataBuffer = NULL;
	iBackupBuffer = NULL;
	iIsPageModified = EFalse;
}

TBool CSMan2HexEditorView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2HexEditorView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2HexEditorView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
		ReleaseFileAndBuffer();
	}
}

void CSMan2HexEditorView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
	
	// Offset text color
	/*
	TRgb iColor = CEikonEnv::Static()->Color(EColorMenuPaneText);
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->OverrideColorL(EColorMenuPaneDimmedText, iColor);
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetTitleDimmed(1, ETrue);
	UpdateOffsetAndFileSizeText();
	*/
}

TVwsViewId CSMan2HexEditorView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidHexEditorView);
}

CSMan2HexEditorView::~CSMan2HexEditorView()
{
	ReleaseFileAndBuffer();
	delete iLineCursorBitmap;
}

void CSMan2HexEditorView::UpdateOffsetAndFileSizeText()
{
	TUint iCurrentOffset;
	TBuf<3> iMediaIndicator;
	
	if (FileIsInROM())
		iMediaIndicator.Copy(_L("ROM"));
	else
		iMediaIndicator.Copy(_L("RAM"));
	
	iCurrentOffset = iCurrentFilePageOffset + (iHexEditControl->CursorPos() / 3);
	if (iFileSize > 0)
		iOffsetAndFileSizeText.Format(_L("$%08x:$%08x, "), iCurrentOffset, iFileSize - 1);
	else
		iOffsetAndFileSizeText.Format(_L("$%08x:$%08x, "), iCurrentOffset, 0);
	iOffsetAndFileSizeText.Append(iMediaIndicator);

	iOffsetAndFileSize->SetTextL(iOffsetAndFileSizeText);
	iOffsetAndFileSize->DrawDeferred();

/*
	TBuf<9> iTextOffset;
	TUint iCurrentOffset;
	
	iCurrentOffset = iCurrentFilePageOffset + (iHexEditControl->CursorPos() / 3);
	iTextOffset.Format(_L("$%07x"), iCurrentOffset);

	CEikMenuBar::CTitleArray *iTitleArray = CEikonEnv::Static()->AppUiFactory()->MenuBar()->MenuTitleArray();
	CEikMenuBarTitle *iMenuItemTitle = iTitleArray->At(1);
	iMenuItemTitle->iData.iText.Copy(iTextOffset);
	
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->DrawItem(1);
*/	
}

TKeyResponse CSMan2HexEditorView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	TKeyResponse iRet = iHexEditControl->OfferKeyEventL(aKeyEvent, aType);		
	if (iRet == EKeyWasConsumed)
	{
		UpdateOffsetAndFileSizeText();
		iIsPageModified = ETrue;
	}
	return iRet;
}

void CSMan2HexEditorView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	CCoeControl::HandlePointerEventL(aPointerEvent);
	UpdateOffsetAndFileSizeText();
}

void CSMan2HexEditorView::SetHexControlExtent()
{
	iHexEditControl->SetExtent(TPoint(0, 0), TSize(iClientRect.Size().iWidth, ((iClientRect.Size().iHeight - EQikToolbarHeight - 10) * configData->iHexWindowSizeProportion) / 10));
}

void CSMan2HexEditorView::SetASCIIControlExtent()
{
	iASCIIControl->SetExtent(TPoint(0, iHexEditControl->Position().iY + iHexEditControl->Size().iHeight), TSize(iClientRect.Size().iWidth, 
		 iOffsetAndFileSize->Position().iY - iHexEditControl->Position().iY - iHexEditControl->Size().iHeight));
}

TBool CSMan2HexEditorView::SetHexAndASCIIControlsExtent()
{
	TBool iRetVal = EFalse;
	
	CHexEditResize *iResizeDialog = new (ELeave) CHexEditResize(&(configData->iHexWindowSizeProportion));
	if (iResizeDialog->ExecuteLD(R_DIALOG_HEXEDIT_RESIZE) == EEikBidYes)
	{
		SetHexControlExtent();
		SetASCIIControlExtent();
		iHexEditControl->DrawDeferred();
		iASCIIControl->DrawDeferred();
		iRetVal = ETrue;
	}
	return iRetVal;
}

void CSMan2HexEditorView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());
	
	iClientRect = aRect;
	
	iLineCursorBitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iLineCursorBitmap->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderLinecursor));	

	iASCIIControl = new (ELeave) CASCIIDisplayControl;
	iASCIIControl->ConstructL(this, 20, PAGE_SIZE, CEikEdwin::EAlwaysShowSelection | CEikEdwin::EReadOnly | CEikEdwin::ENoAutoSelection | CEikEdwin::EOnlyASCIIChars, EGulFontControlAll, EGulAllFonts);

	iHexEditControl = new (ELeave) CHexEditorControl;
	iHexEditControl->ConstructL(this, 20, PAGE_SIZE * 3, CEikEdwin::ELineCursor | CEikEdwin::ENoAutoSelection | CEikEdwin::EOnlyASCIIChars, EGulFontControlAll, EGulAllFonts);
	SetHexControlExtent();
	iHexEditControl->CreateScrollBarFrameL();
	iHexEditControl->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	controlsArray->AppendL(iHexEditControl);
	iHexEditControl->iASCIIWindow = iASCIIControl;

	iOffsetAndFileSize = new (ELeave) CEikLabel;
	iOffsetAndFileSizeText.Copy(_L("$00000000:$00000000, ROM"));
	iOffsetAndFileSize->SetTextL(iOffsetAndFileSizeText);
	iOffsetAndFileSize->SetContainerWindowL(*this);
	iOffsetAndFileSize->SetExtent(TPoint(0, aRect.Size().iHeight - 10 - EQikToolbarHeight - 14), TSize(aRect.Size().iWidth, 14));
	//iOffsetAndFileSize->SetEmphasis(CEikLabel::EFullEmphasis);
	iOffsetAndFileSize->SetAlignment(EHCenterVCenter);
	controlsArray->AppendL(iOffsetAndFileSize);

	SetASCIIControlExtent();
	iASCIIControl->SetFocus(ETrue, ENoDrawNow);
	iASCIIControl->CreateScrollBarFrameL();
	iASCIIControl->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	controlsArray->AppendL(iASCIIControl);
	iASCIIControl->iHexWindow = iHexEditControl;
	iASCIIControl->InitState();

	//iHexEditControl->CopyByteDataToDisplay();
	iHexEditControl->InitState(iLineCursorBitmap);
	iHexEditControl->SetAllowInsertDelete(EFalse, EFalse);
	iHexEditControl->SetFocus(ETrue, ENoDrawNow);
	
	// Finish up view initialisation
	viewId = CSMan2AppUi::EViewHexEditor;
	iBJackIconPosition = TPoint(2, iOffsetAndFileSize->Position().iY + iOffsetAndFileSize->Size().iHeight + 2);
	InitBJackIcon();
	this->EnableDragEvents();
		
	MakeVisible(EFalse);
	ActivateL();
}

void CSMan2HexEditorView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}

void CSMan2HexEditorView::ModifyHexEditorSelection()
{
	iHexEditControl->ModifySelection();
}

TBool CSMan2HexEditorView::FileHasReadOnlyAttribute()
{
	TEntry iFileAttrib;
	TBool iReturnValue = ETrue;
	
	if (CEikonEnv::Static()->FsSession().Entry(iFileName, iFileAttrib) == KErrNone)
	{
		if (!iFileAttrib.IsReadOnly())
			iReturnValue = EFalse;
	}
	return iReturnValue;
}

// Returns
// KErrNotReady = couldn't get volume info (volume un-mounted?)
// 0 = File is not in ROM
// 1 = File is in ROM
TInt CSMan2HexEditorView::FileIsInROM()
{
	TInt iReturnValue = KErrNotReady;
	
	TBuf<1> iFileDriveLetter;
	iFileDriveLetter.Copy(iFileName.Left(1));
	
	TInt iDriveIndex = iFileDriveLetter[0] - 65;	// 65 = 'A'
	TVolumeInfo iVolumeInfo;
	if (CEikonEnv::Static()->FsSession().Volume(iVolumeInfo, EDriveA + iDriveIndex) == KErrNone)
	{
		if (iVolumeInfo.iDrive.iType != EMediaRom)
			iReturnValue = 0;
		else
			iReturnValue = 1;
	}
	
	return iReturnValue;
}

// Returns ETrue if the config was changed and must be saved
TBool CSMan2HexEditorView::ToggleReadOnly()
{
	TBool iReturnValue = EFalse;
	
	TInt iResult = FileIsInROM(); 
	if (iResult == KErrNotReady)
		CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_NOTREADY);
	else if (iResult == 1)
		CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_FILEINROM);
	else if (FileHasReadOnlyAttribute())
		CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_FILEISREADONLY);
	else // iResult == 0
	{
		configData->iHexEditIsReadOnly = !configData->iHexEditIsReadOnly;
		SetHexControlIsReadOnly(configData->iHexEditIsReadOnly);
		iReturnValue = ETrue;
	}
	return iReturnValue;
}

void CSMan2HexEditorView::SetHexControlIsReadOnly(TBool aIsReadOnly)
{
	iHexEditControl->SetAllowEdit(!aIsReadOnly);
}

TBool CSMan2HexEditorView::OpenFile()
{
	TBool retVal = EFalse;
	
	if (!iFileIsOpen)
	{
		TInt iFileOpenMode = EFileShareExclusive | EFileWrite;
		
		// If the file is in ROM, the setting in configData is ignored and the hexeditor
		// will always be in readonly mode. However, the setting in configData is untouched.
		// Its original setting will return when editing a file in RAM
		// The same if the file has its READONLY attribute set (irregardless of whether it's
		// in RAM or ROM)
		if ((FileIsInROM()) || (FileHasReadOnlyAttribute()))
		{
			iFileOpenMode = EFileShareReadersOnly | EFileRead;
			SetHexControlIsReadOnly(ETrue);
			//configData->iHexEditIsReadOnly = ETrue;
		}
		else
			SetHexControlIsReadOnly(configData->iHexEditIsReadOnly);
		
		if (iFile.Open(CEikonEnv::Static()->FsSession(), iFileName, iFileOpenMode) == KErrNone)
		{
			iFileSize = -1;
			if (iFile.Size(iFileSize) == KErrNone)
			{
				//if (iFileSize < 1)
				//	CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_EMPTYFILE);
				//else
				{
					iFileIsOpen = ETrue;
					retVal = ETrue;
				}
			}
			else
				ReleaseFileAndBuffer();
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_OPENERR);
	}
	else
		retVal = ETrue;
		
	return retVal;
}

void CSMan2HexEditorView::ReleaseFileAndBuffer()
{
	if (iFileIsOpen)
	{
		iFile.Close();
		iFileIsOpen = EFalse;
	}
		
	if (iBackupBuffer)
	{
		delete iBackupBuffer;
		iBackupBuffer = NULL;
	}

	if (iDataBuffer)
	{
		delete iDataBuffer;
		iDataBuffer = NULL;
	}
}

TBool CSMan2HexEditorView::HasSelection()
{
	return (iHexEditControl->SelectionLength() > 0);
}

void CSMan2HexEditorView::DeleteBytes()
{
	iIsPageModified = ETrue;
	iHexEditControl->DoDelete();
	UpdateOffsetAndFileSizeText();
}

TBool CSMan2HexEditorView::IsAtLastPage() 
{
	return ((iCurrentFilePageOffset + READ_BUFFER_SIZE) >= iFileSize);
}

TInt CSMan2HexEditorView::WritePage()
{
	// Here are the use cases:
	//
	// No changes to commit = return -1
	// User committed changes = return 0
	// User chose not to commit changes = return 1
	// User committed changes but write operation failed = return 2
	// User chose to cancel commit request = return 3
	//
	// The TInt returned is used by the page readers to decide whether to move to the next page
	// or not

	TInt iRetVal = 3;
	TTime iFileModifiedDateTime;
	TBool iDateTimeRetrieveSuccessful = EFalse;
	
	if (iFileIsOpen)
	{
		// Prompt user if changes were made
		if (iIsPageModified)
		{
			CEikDialog *iConfirmChangesDialog = new (ELeave) CEikDialog;
			TInt iUserValue = iConfirmChangesDialog->ExecuteLD(R_DIALOG_HEXEDIT_CONFIRMCHANGES);
			
			// User wants to commit changes
			if (iUserValue == EEikBidYes)
			{
				iHexEditControl->CopyDisplayToByteData(iDataBuffer);
				iRetVal = 2;
				
				if (configData->iHexEditPreserveDate)
				{
					if (iFile.Modified(iFileModifiedDateTime) == KErrNone)
						iDateTimeRetrieveSuccessful = ETrue;
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_GETDATEERROR);
				}
				
				if (iFile.Seek(ESeekStart, iCurrentFilePageOffset) == KErrNone)
				{
					if (iFile.Write(iDataBuffer->Des()) == KErrNone)
					{
						TInt iTruncateFileResult = KErrNone;
						
						if (IsAtLastPage())
							iTruncateFileResult = iFile.SetSize(iCurrentFilePageOffset + iDataBuffer->Length());
						
						if (iTruncateFileResult == KErrNone)
						{
							if (iFile.Flush() == KErrNone)
							{
								if (iFile.Size(iFileSize) == KErrNone)
								{
									if (configData->iHexEditPreserveDate)
									{
										if (iDateTimeRetrieveSuccessful)
										{
											if (iFile.SetModified(iFileModifiedDateTime) != KErrNone)
												CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRSETMODIFIED);
										}
									}
									iRetVal = 0;
									iIsPageModified = EFalse;
								}
								else
									CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_SIZEERROR);
							}
							else
								CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_FLUSHERROR);
						}
						else
							CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_TRUNCERROR);
					}
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_WRITEERROR);
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_SEEKERROR);
			}
			// User discarded changes
			else if (iUserValue == EEikBidOk)
				iRetVal = 1;
			// User doesn't want to navigate away from this page
			else if (iUserValue == EEikBidSpace)
				iRetVal = 3;
		}
		else
			iRetVal = -1;
	}
	else
		iRetVal = 2;
	
	return iRetVal;
}

TInt CSMan2HexEditorView::ReadPage(TInt aPageOffsetToRead)
{
	// 0 = read failed
	// 1 = already at end of page
	// 2 = read successful
	TInt retVal = 0;
	
	if (iFileIsOpen)
	{
		TBool iWritePassed = ETrue;
		
		if (iIsPageModified)
			iWritePassed = WritePage();
			
		if ((iWritePassed == 0) || (iWritePassed == 1))
		{
		 	if (!iDataBuffer)
		 		iDataBuffer = HBufC8::NewL(PAGE_SIZE);
		 	if (!iBackupBuffer)
			 	iBackupBuffer = HBufC8::NewL(PAGE_SIZE);
		 	iDataBuffer->Des().SetLength(0);
		 	TPtr8 iDataDes(iDataBuffer->Des());
		 	if (iFile.Read(aPageOffsetToRead, iDataDes, READ_BUFFER_SIZE) == KErrNone)
		 	{
		 		TInt iNumBytesRead = iDataBuffer->Length();
		 		// End of file?
		 		if (iDataBuffer->Length() == 0)
		 		{
		 			CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_ENDOFFILE);
		 			if (iWritePassed == 1)
			 			// Undo changes to current page
			 			iDataBuffer->Des().Copy(*iBackupBuffer);
		 			else // if (iWritePassed == 0)
		 				// Else update the backup buffer to reflect changes
		 				iHexEditControl->CopyDisplayToByteData(iBackupBuffer);
		 			// In either case, the page buffer should be synchronized to the file's
		 			// physical contents
		 			iIsPageModified = EFalse;
		 			retVal = 1;
		 		}
		 		if ((iNumBytesRead > 0) || (iFileSize == 0))
		 		{
		 			iBackupBuffer->Des().Copy(*iDataBuffer);
			 		retVal = 2;
			 		iIsPageModified = EFalse;
			 		
		 			if ((aPageOffsetToRead + READ_BUFFER_SIZE) >= iFileSize)
			 			// User can insert/delete if at final page
			 			iHexEditControl->SetAllowInsertDelete(ETrue, ETrue);
					else
				 		// User cannot do this in the middle of the file
			 			iHexEditControl->SetAllowInsertDelete(EFalse, EFalse);
			 	}
		 	}
		 	else
			 	CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRREADFILE);
		}
	}
	return retVal;
}

TBool CSMan2HexEditorView::DoEditFile()
{
	TBool retVal = EFalse;
	TBuf<1> iBlank;
	
	iBlank.Copy(_L(""));
	iHexEditControl->SetTextL(&iBlank);
	iASCIIControl->SetTextL(&iBlank);
	iIsPageModified = EFalse;
	if (OpenFile())
	{
		iCurrentFilePageOffset = 0;
		if ((ReadPage(0) != 0) || (iFileSize == 0))
		{
			retVal = ETrue;
			PopulateHexControl();
		}
	}
	return retVal;
}

void CSMan2HexEditorView::PopulateHexControl()
{
	if (iDataBuffer->Length() > 0)
	{
		iHexEditControl->iByteData = iDataBuffer;
		iHexEditControl->CopyByteDataToDisplay();
		if (iHexEditControl->SelectionLength() == 0)
		{
			TInt iByteChunk = iHexEditControl->CursorPos() / 3;
			iASCIIControl->SetSelectionL(iByteChunk, iByteChunk + 1);
		}
	}
	iASCIIControl->DrawDeferred();
	iHexEditControl->DrawDeferred();
	UpdateOffsetAndFileSizeText();
}

void CSMan2HexEditorView::DoFileSave()
{
	TInt iWritePassed = WritePage();
	switch (iWritePassed)
	{
		case 0 : 
			{
				CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_WRITEOK);
				break;
			}
		case 1 :
			{
				iDataBuffer->Des().Copy(*iBackupBuffer);
				PopulateHexControl();
				iIsPageModified = EFalse;
				break;
			}
		case -1:
			{
				CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_NOCHANGES);
				break;
			}
	}
}

void CSMan2HexEditorView::ReadBeginPage()
{
	if (iFileSize > 0)
	{
		if (ReadPage(0) != 0)
		{
			iCurrentFilePageOffset = 0;
			PopulateHexControl();
		}
	}
}

void CSMan2HexEditorView::DoReload()
{
	// 0 = read failed
	// 1 = already at end of page
	// 2 = read successful

	TInt iReadResult = ReadPage(iCurrentFilePageOffset);
	if (iReadResult != 0)
	{
		PopulateHexControl();
	}
}

void CSMan2HexEditorView::ReadEndPage()
{
	if (iFileSize > 0)
	{
		if (iFileSize <= READ_BUFFER_SIZE)
			ReadBeginPage();
		else
		{
			TInt iPageOffsetToRead = iFileSize - (iFileSize % READ_BUFFER_SIZE);
			if (iPageOffsetToRead == iFileSize)
				iPageOffsetToRead -= READ_BUFFER_SIZE;
				
			TInt iReadResult = ReadPage(iPageOffsetToRead);		
			if (iReadResult != 0)
			{
				if (iReadResult == 2)
					iCurrentFilePageOffset = iPageOffsetToRead;
				PopulateHexControl();
			}
		}
	}
}

void CSMan2HexEditorView::ReadNextPage()
{
	TUint iPageOffsetToRead = iCurrentFilePageOffset + (iHexEditControl->TextLength() + 1) / 3;
	TInt iReadResult = ReadPage(iPageOffsetToRead);
	if (iReadResult != 0)
	{
		if (iReadResult == 2)
			iCurrentFilePageOffset = iPageOffsetToRead;
		PopulateHexControl();
	}
}

void CSMan2HexEditorView::ReadPrevPage()
{
	if (iCurrentFilePageOffset == 0)
		CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_BEGINNINGOFFILE);
	else
	{
		TUint iPageOffsetToRead = iCurrentFilePageOffset;
		if (iPageOffsetToRead >= READ_BUFFER_SIZE)
			iPageOffsetToRead -= READ_BUFFER_SIZE;
		else
			iPageOffsetToRead = 0;

		TInt iReadResult = ReadPage(iPageOffsetToRead);
		if (iReadResult != 0)
		{
			if (iReadResult == 2)
				iCurrentFilePageOffset = iPageOffsetToRead;
			PopulateHexControl();
		}
	}
}

void CSMan2HexEditorView::GotoOffset()
{
	TBool iWritePassed = ETrue;
	TBool iOldIsPageModified = iIsPageModified;
	
	if (iIsPageModified)
		iWritePassed = WritePage();
		
	if ((iWritePassed == 0) || (iWritePassed == 1))
	{
		TInt iNewOffset = 0;
		TInt iCurrentOffset = iCurrentFilePageOffset + (iHexEditControl->CursorPos() / 3);
		TInt iMaxOffset = iFileSize - 1;
		
		if (iFileSize == 0)
			iMaxOffset = 0;

		CHexEditGotoOffset *iHexEditOffsetDialog = new (ELeave) CHexEditGotoOffset(iMaxOffset, &iNewOffset, iCurrentOffset);

		if (iHexEditOffsetDialog->ExecuteLD(R_DIALOG_HEXEDIT_GOTOOFFSET) == EEikBidYes)
		{
			// Only read if the new offset is on a different page or the page was previously modified
			if ((iNewOffset < iCurrentFilePageOffset) || (iNewOffset >= (iCurrentFilePageOffset + ((iHexEditControl->TextLength() + 1) / 3))) || (iOldIsPageModified))
			{
				// Round it to the nearest READ_BUFFER_SIZE multiple
				TInt iPageOffsetToRead = iNewOffset - (iNewOffset % READ_BUFFER_SIZE);
				if (ReadPage(iPageOffsetToRead) != 0)
				{
					iCurrentFilePageOffset = iPageOffsetToRead;
					PopulateHexControl();
					
					// Position cursor at the proper location
					iNewOffset = iNewOffset % READ_BUFFER_SIZE;
					iHexEditControl->MoveCursorToRelativeOffset(iNewOffset);
					UpdateOffsetAndFileSizeText();
				}
			}
			else
			{
				iNewOffset = iNewOffset % READ_BUFFER_SIZE;
				iHexEditControl->MoveCursorToRelativeOffset(iNewOffset);
				UpdateOffsetAndFileSizeText();
			}
		}
	}
}

/*************************************************************
*
* HexEditor control
*
**************************************************************/

CHexEditorControl::CHexEditorControl() : CEikGlobalTextEditor()
{
	iByteData = NULL;
	iAllowInsert = EFalse;
	iAllowDelete = EFalse;
	iAllowEdit = EFalse;
	iASCIIWindow = NULL;
	
	// Note, fullstops are used to replace non printable chars.
	// If you adjust the number of chars in this array, make sure
	// iAllowableChars in the header file is large enough
	char iTempCharArray[] = 
	{
	'A','B','C','D','E','F','G','H','I','J','K',
	'L','M','N','O','P','Q','R','S','T','U','V',
	'W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k',
	'l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9',
	'/','!','@','#','$','%','^','&','(',')','-',
	'_','=','+',']','[','}','{',':',';','?','<',
	'>','|','\\',',',0x22,	// 0x22 = double quotes
	'\'',' ','*','~','`','.'
	};
	
	TInt iArraySize = sizeof(iTempCharArray) / sizeof(char);
	iNumAllowableChars = iArraySize;
	iArraySize--;
	do
	{
		iAllowableChars[iArraySize] = iTempCharArray[iArraySize];
		iArraySize--;
	} while (iArraySize >= 0);
}

void CHexEditorControl::SetAllowInsertDelete(TBool aAllowInsert, TBool aAllowDelete)
{
	iAllowInsert = aAllowInsert;
	iAllowDelete = aAllowDelete;
}

void CHexEditorControl::SetAllowEdit(TBool aAllowEdit)
{
	iAllowEdit = aAllowEdit;
}

CHexEditorControl::~CHexEditorControl()
{
}

// This is called after the edwin is initialized, but before it is ready for display
void CHexEditorControl::InitState(CFbsBitmap *aLineCursorBitmap)
{
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	charFormat.iFontSpec.iTypeface.SetIsProportional(EFalse);
	charFormatMask.SetAttrib(EAttFontTypeface);
	ApplyCharFormatL(charFormat, charFormatMask);
	if (iASCIIWindow)
	{
		if (iASCIIWindow->TextLength() > 0)
			iASCIIWindow->SetSelectionL(0, 1);
	}	
	// Even though I set the bitmap, it doesn't get used. ?!?!?!?. If i don't set the bitmap,
	// calling SetFocus on the control crashes it. :( DOH?
	TextView()->SetLineCursorBitmap(aLineCursorBitmap);
	TextView()->SetMarginWidths(0, 12);
	TextView()->SetCursorVisibilityL(TCursor::EFCursorFlashing, TCursor::EFCursorFlashing);
}

TBool CHexEditorControl::IsDisplayableChar(char iChar)
{
	TBool iFoundAllowable = EFalse;

	for (int j = 0; j < iNumAllowableChars; j++)
	{
		if (iChar == iAllowableChars[j])
		{
			iFoundAllowable = ETrue;
			break;
		}
	}
	return iFoundAllowable;
}

void CHexEditorControl::DoDelete()
{
	TInt iCursorPos = CursorPos();

	InsertDeleteCharsL(0, _L(""), Selection());
	SetSelectionL(0, 0);
	if (iCursorPos > TextLength())
		iCursorPos = TextLength();
	SetCursorPosL(iCursorPos, EFalse);
	if (iASCIIWindow)
		iASCIIWindow->DoDelete();
}

void CHexEditorControl::CopyDisplayToByteData(HBufC8 *aBuffer)
{
	TLex iConverter;
	TUint iByteNumber;
	iConverter.Assign(iText->Read(0, TextLength()));
	
	aBuffer->Des().SetLength(0);
	while (!iConverter.Eos())
	{
		iConverter.Val(iByteNumber, EHex);
		aBuffer->Des().Append(TChar(iByteNumber));
		iConverter.SkipSpace();
	}
}

void CHexEditorControl::CopyByteDataToDisplay()
{
	if (iByteData->Length() > 0)
	{
		TInt i;	
		TBuf<4> iHexToStringBuffer;
		HBufC *iHexString = HBufC::NewL(iByteData->Length() * 3);
		for (i = 0; i < iByteData->Length(); i++)
		{
			iHexToStringBuffer.Format(_L("%02x "), iByteData->Des()[i]);
			iHexString->Des().Append(iHexToStringBuffer);
		}
		// Remove the last space character
		iHexString->Des().Delete(iHexString->Des().Length() - 1, 1);
		
		SetTextL(&(*iHexString));

		for (i = 0; i < iByteData->Length(); i++)
		{
			if (!IsDisplayableChar(iByteData->Des()[i]))
				iByteData->Des()[i] = TChar('.');
		}

		// This does the conversion from 8bit to 16bit
		iHexString->Des().Copy(iByteData->Des());
		iASCIIWindow->SetTextL(&(*iHexString));
		iASCIIWindow->UpdateScrollBarsL();
		delete iHexString;
	}
}

void CHexEditorControl::ModifySelection()
{
	// This should only happen once because the anchor position is fixed. Only the cursor
	// position changes during a drag
	if ((Selection().iAnchorPos %3) != 0)
	{
		if (Selection().iAnchorPos < Selection().iCursorPos)
			SetSelectionL(Selection().iCursorPos, Selection().iAnchorPos - (Selection().iAnchorPos % 3));
		else if (Selection().iAnchorPos > Selection().iCursorPos)
		{
			TInt iNewAnchorPos = Selection().iAnchorPos + 3 - (Selection().iAnchorPos % 3);
			if (iNewAnchorPos > (TextLength() - 1))
				iNewAnchorPos = TextLength();
			SetSelectionL(Selection().iCursorPos, iNewAnchorPos);
		}
	}
	
	if ((Selection().iCursorPos % 3) != 0)
	{
		if (Selection().iCursorPos > Selection().iAnchorPos)
		{
			TInt iNewCursorPos = Selection().iCursorPos + 3 - (Selection().iCursorPos % 3);
			if (iNewCursorPos > (TextLength() - 1))
				iNewCursorPos = TextLength();
			SetSelectionL(iNewCursorPos, Selection().iAnchorPos);
		}
		else if (Selection().iCursorPos < Selection().iAnchorPos)
			SetSelectionL(Selection().iCursorPos - (Selection().iCursorPos % 3), Selection().iAnchorPos);
	}
	
	// Adjust cursor position
	TPtrC iChar = iText->Read(CursorPos(), 1);
	if (iChar.Compare(_L(" ")) == 0)
		SetCursorPosL(CursorPos() + 1, EFalse);
		
	SynchronizeWithASCIIWindow(EFalse, EFalse);
}

void CHexEditorControl::SynchronizeWithASCIIWindow(TBool aIsDelete, TBool aIsChunkUpdate)
{
	if (iASCIIWindow)
	{
		if (!aIsChunkUpdate)
		{
			if (SelectionLength() > 0)
			{
				// Selection synchronization
				TCursorSelection iHexCursor = Selection();
				if (iHexCursor.iAnchorPos == TextLength())
					iHexCursor.iAnchorPos++;
				if (iHexCursor.iCursorPos == TextLength())
					iHexCursor.iCursorPos++; 
				iASCIIWindow->SetSelectionL(TInt(iHexCursor.iCursorPos / 3), TInt(iHexCursor.iAnchorPos / 3));
			}

			TInt iByteChunk = (CursorPos() - 1) / 3;
			if (!aIsDelete)
			{
				TInt iByteStartPos = iByteChunk * 3;
				TInt iByteEndPos = iByteStartPos + 2;
				
				// CursorPos() should always be > 0
				if (iByteEndPos > TextLength())
					iByteEndPos = TextLength();
					
				TBuf<2> iNewByte;
				iText->Extract(iNewByte, iByteStartPos, iByteEndPos - iByteStartPos);
				
				TLex iConverter;
				TUint iByteNumber;
				iConverter.Assign(iNewByte);
				iConverter.Val(iByteNumber, EHex);
				
				TBuf<1> iASCIIChar;
				iASCIIChar.Format(_L("%c"), iByteNumber);
				if (!IsDisplayableChar(iASCIIChar[0]))
					iASCIIChar.Copy(_L("."));

				iByteEndPos = iByteChunk + 1;
				if (iByteEndPos > iASCIIWindow->TextLength())
					iByteEndPos = iASCIIWindow->TextLength();
				iASCIIWindow->InsertDeleteCharsL(iByteChunk, iASCIIChar, TCursorSelection(iByteChunk, iByteEndPos));
			}
			else
			{
				TInt iByteEndPos = iByteChunk + 1;
				if (iByteEndPos > iASCIIWindow->TextLength())
					iByteEndPos = iASCIIWindow->TextLength();
				iASCIIWindow->InsertDeleteCharsL(iByteChunk, _L(""), TCursorSelection(iByteChunk, iByteEndPos));
			}
			
			if (SelectionLength() == 0)
			{
				iByteChunk = CursorPos() / 3;
				iASCIIWindow->SetSelectionL(iByteChunk, iByteChunk + 1);
			}
		}
		iASCIIWindow->UpdateScrollBarsL();
	}
}

void CHexEditorControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	if (TextLength() > 0)
	{
		// This function needs to handle a user tapping on the empty spaces between byte
		// displays. It moves the cursor to the previous character
		if (aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
			TPoint iPointerPosition = ((TPointerEvent)(aPointerEvent)).iPosition;
			TInt iTextCharPosition = TextView()->XyPosToDocPosL(iPointerPosition);
			TBool iInvalidChar = EFalse;
			if (iText->DocumentLength() > 0)
			{
				TPtrC iChar = iText->Read(iTextCharPosition, 1);
				if (iChar.Compare(_L(" ")) == 0)
				{
					iInvalidChar = ETrue;
	/*
					// For some reason, this code doesn't work anymore. :(
					
					if (iTextCharPosition > 0)
						iTextCharPosition--;

					TPoint iNewPosition;
					TextView()->DocPosToXyPosL(iTextCharPosition, iNewPosition);
					((TPointerEvent)(aPointerEvent)).iPosition.iX = iNewPosition.iX;
					((TPointerEvent)(aPointerEvent)).iPosition.iY = iNewPosition.iY;	
	*/
				}
			}
			
			if (!iInvalidChar)
				CEikEdwin::HandlePointerEventL(aPointerEvent);
			SynchronizeWithASCIIWindow(EFalse, EFalse);
		}
		else if (aPointerEvent.iType == TPointerEvent::EDrag)
		{
			TPoint iPointerPosition = ((TPointerEvent)(aPointerEvent)).iPosition;
			TInt iTextCharPosition = TextView()->XyPosToDocPosL(iPointerPosition);
			if (iText->DocumentLength() > 0)
			{
				TPtrC iChar = iText->Read(iTextCharPosition, 1);
				if (iChar.Compare(_L(" ")) != 0)
					CEikEdwin::HandlePointerEventL(aPointerEvent);
				ModifySelection();
			}		
		}
	}
}

void CHexEditorControl::UpdateASCIIWindowScrollPosition()
{
	// Update the ascii window position
	if (iASCIIWindow)
	{
		// We make use of a little trick. In an edwin, the scrollbar thumb position and
		// scrollspan correspond to the current y-pixel position and maximum y-pixel
		// position of the entire document, respectively.
		
		// Get the hex window's current position
		CEikScrollBar *iHexWindowScrollBar = ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical);
		CEikScrollBar *iASCIIWindowScrollBar = iASCIIWindow->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical);		
		if ((iASCIIWindowScrollBar) && (iHexWindowScrollBar))
		{
			TInt iCurrentDocPos  = iHexWindowScrollBar->ThumbPosition();
			// Figure out the ascii window's current position
			TInt iCurrentASCIIPos = iASCIIWindowScrollBar->ThumbPosition();
			// Figure out the ascii window's FINAL RELATIVE position
			TInt iFinalRelativeASCIIPos = (iCurrentDocPos * iASCIIWindowScrollBar->Model()->MaxThumbPos()) / 
				ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical)->Model()->MaxThumbPos();
			// Figure out the difference between ascii window's current position and final relative position
			TInt iDeltaASCIIPos = iCurrentASCIIPos - iFinalRelativeASCIIPos;
			// Now scroll the ascii window by that amount of pixels
			iASCIIWindow->ScrollDisplayByPixels(iDeltaASCIIPos);
		}
	}
}

TKeyResponse CHexEditorControl::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	TKeyResponse iRetVal = EKeyWasNotConsumed;
	
	if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
	{
		MoveDisplayL(TCursorPosition::EFLineDown);
		UpdateASCIIWindowScrollPosition();
	}
	else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
	{
		MoveDisplayL(TCursorPosition::EFLineUp);
		UpdateASCIIWindowScrollPosition();
	}
	// Only accept 0-9, a-f, A-F, backspace
	// 0-9 = 48-57, a-f = 97-102, A-Z = 65-70
	else if ((((aKeyEvent.iCode >= 48) && (aKeyEvent.iCode <= 57)) || 
		((aKeyEvent.iCode >= 97) && (aKeyEvent.iCode <= 102)) ||
		((aKeyEvent.iCode >= 65) && (aKeyEvent.iCode <= 70))))
	{
		if (iAllowEdit)
		{
			if ((CursorPos() == TextLength()) && (iAllowInsert))
			{
				if ((TextLength() + 1) >= PAGE_SIZE * 3)
				{
					CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_BUFFULL);
					return EKeyWasConsumed;
				}
				else
				{
					TBuf<3> iEmptyByte;
					if (TextLength() != 0)
						iEmptyByte.Copy(_L(" 00"));
					else
						iEmptyByte.Copy(_L("00"));
					InsertDeleteCharsL(TextLength(), iEmptyByte, TCursorSelection(0, 0));
					SetSelectionL(TextLength() - 1, TextLength() - 2);
					CEikEdwin::OfferKeyEventL(aKeyEvent, aType);
					SynchronizeWithASCIIWindow(EFalse, EFalse);
					iRetVal = EKeyWasConsumed;		
				}
			}
			else
			{
				ClearSelectionL();
				if (CursorPos() < TextLength())
				{
					TPtrC iCurrentChar = iText->Read(CursorPos(), 1);
					if (iCurrentChar.Compare(_L(" ")) != 0)
					{
						SetSelectionL(CursorPos() + 1, CursorPos());
						TKeyResponse iResponse = CEikEdwin::OfferKeyEventL(aKeyEvent, aType);
						SynchronizeWithASCIIWindow(EFalse, EFalse);
						if (iResponse == EKeyWasConsumed)
						{
							TPtrC iChar = iText->Read(CursorPos(), 1);
							if (iChar.Compare(_L(" ")) == 0)
								SetCursorPosL(CursorPos() + 1, EFalse);
							iRetVal = EKeyWasConsumed;
						}
					}
				}
			}
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_READONLY);
	}
	else if ((aKeyEvent.iCode == 8) && (iAllowDelete) && (TextLength() > 0))
	{
		if (iAllowEdit)
		{
			// Position 0 doesn't belong to anyone
			if (CursorPos() > 0)
			{
				TInt iByteChunk = (CursorPos() - 1) / 3;
				TInt iByteStartPos = iByteChunk * 3;
				TInt iByteEndPos = iByteStartPos + 3;
				if (iByteEndPos > TextLength())
				{
					iByteStartPos--;
					if (iByteStartPos < 0)
						iByteStartPos = 0;
					iByteEndPos = TextLength();
				}
				SynchronizeWithASCIIWindow(ETrue, EFalse);
				InsertDeleteCharsL(iByteStartPos, _L(""), TCursorSelection(iByteStartPos, iByteEndPos));
				SetCursorPosL(iByteStartPos, EFalse);
				iRetVal = EKeyWasConsumed;
			}
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_READONLY);
	}
	
	// Adjust cursor position
	TPtrC iChar = iText->Read(CursorPos(), 1);
	if (iChar.Compare(_L(" ")) == 0)
		SetCursorPosL(CursorPos() + 1, EFalse);

	return iRetVal;
}

void CHexEditorControl::MoveCursorToRelativeOffset(TInt aOffset)
{
	if ((aOffset * 3) > (TextLength() - 1))
		aOffset = 0;
	SetCursorPosL(aOffset * 3, EFalse);
	SynchronizeWithASCIIWindow(EFalse, EFalse);
}

void CHexEditorControl::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
	CEikEdwin::HandleScrollEventL(aScrollBar, aEventType);
	UpdateASCIIWindowScrollPosition();
}

void CHexEditorControl::ScrollDisplayByPixels(TInt aDeltaY)
{
	TextView()->ScrollDisplayPixelsL(aDeltaY);
	ForceScrollBarUpdateL();
}

/*************************************************************
*
* ASCII control
*
**************************************************************/
CASCIIDisplayControl::CASCIIDisplayControl()
{
	iHexWindow = NULL;
}

TKeyResponse CASCIIDisplayControl::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
{
	return EKeyWasNotConsumed;
}

void CASCIIDisplayControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	if (TextLength() > 0)
	{
		CEikEdwin::HandlePointerEventL(aPointerEvent);

		if (aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
			ClearSelectionL();
		}
		else if ((aPointerEvent.iType == TPointerEvent::EDrag) || (aPointerEvent.iType == TPointerEvent::EButton1Up))
		{
			if (iHexWindow)
			{
				TCursorSelection iSelection = Selection();
				TInt iNewAnchor = iSelection.iAnchorPos * 3;
				TInt iNewCursor = iSelection.iCursorPos * 3;
				if (iNewAnchor > iHexWindow->TextLength())
					iNewAnchor = iHexWindow->TextLength();
				if (iNewCursor > iHexWindow->TextLength())
					iNewCursor = iHexWindow->TextLength();
				iHexWindow->SetSelectionL(iNewCursor, iNewAnchor);
				if (iHexWindow->SelectionLength() == 0)
				{
					TInt iTextCharPosition = CursorPos();
					if (iTextCharPosition > (TextLength() - 1))
						iTextCharPosition = TextLength() - 1;
					SetSelectionL(iTextCharPosition, iTextCharPosition + 1);
					iHexWindow->SetCursorPosL(iTextCharPosition * 3, EFalse);
				}
			}
		}
		else if (SelectionLength() == 0)
		{
			if (iHexWindow)
			{
				TInt iTextCharPosition = CursorPos();
				if (iTextCharPosition > (TextLength() - 1))
					iTextCharPosition = TextLength() - 1;
				SetSelectionL(iTextCharPosition, iTextCharPosition + 1);
				iHexWindow->SetCursorPosL(iTextCharPosition * 3, EFalse);
			}
		}
	}
}

void CASCIIDisplayControl::InitState()
{
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	charFormat.iFontSpec.iTypeface.SetIsProportional(EFalse);
	charFormatMask.SetAttrib(EAttFontTypeface);
	ApplyCharFormatL(charFormat, charFormatMask);
	ApplyCharFormatL(charFormat, charFormatMask);
	iLayout->SetCustomWrap(this);
}

TBool CASCIIDisplayControl::LineBreakPossible(TUint /*aPrevClass*/, TUint /*aNextClass*/, TBool /*aHaveSpaces*/) const
{
	return EFalse;
}

void CASCIIDisplayControl::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
	CEikEdwin::HandleScrollEventL(aScrollBar, aEventType);
	
	// Update the hex window position
	if (iHexWindow)
	{
		// We make use of a little trick. In an edwin, the scrollbar thumb position and
		// scrollspan correspond to the current y-pixel position and maximum y-pixel
		// position of the entire document, respectively.
		
		CEikScrollBar *iHexWindowScrollBar = iHexWindow->ScrollBarFrame()->GetScrollBarHandle(CEikScrollBar::EVertical);

		if (iHexWindowScrollBar)
		{
			// Get the ascii window's current position
			TInt iCurrentDocPos  = aScrollBar->ThumbPosition();
			// Figure out the hex window's current position
			TInt iCurrentHexPos = iHexWindowScrollBar->ThumbPosition();
			// Figure out the hex window's FINAL RELATIVE position
			TInt iFinalRelativeHexPos = (iCurrentDocPos * iHexWindowScrollBar->Model()->MaxThumbPos()) / aScrollBar->Model()->MaxThumbPos();
			// Figure out the difference between hex window's current position and final relative position
			TInt iDeltaHexPos = iCurrentHexPos - iFinalRelativeHexPos;
			// Now scroll the hex window by that amount of pixels
			iHexWindow->ScrollDisplayByPixels(iDeltaHexPos);
		}
	}
}

void CASCIIDisplayControl::ScrollDisplayByPixels(TInt aDeltaY)
{
	TextView()->ScrollDisplayPixelsL(aDeltaY);
	ForceScrollBarUpdateL();
}

void CASCIIDisplayControl::DoDelete()
{
	TInt iCursorPos = CursorPos();

	InsertDeleteCharsL(0, _L(""), Selection());
	SetSelectionL(0, 0);
	if (iCursorPos > TextLength())
		iCursorPos = TextLength();
	SetCursorPosL(iCursorPos, EFalse);
}
