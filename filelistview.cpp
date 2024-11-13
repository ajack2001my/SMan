#include "filelistview.h"
#include "sman.h"
#include <SMan.rsg>

/*************************************************************
*
* File browser view.
*
**************************************************************/

CSMan2FileListView::CSMan2FileListView(CConfig *cData) : CViewBase(cData)
{
	for (int i = 0; i < KMaxDrives; i++)
	{
		driveOldPath[i] = NULL;
		iDirHighlightIndex[i] = NULL;
	}
	
	crcCalculator = new (ELeave) CCRCCalculator;

	blueBeam = new (ELeave) CBlueBeam(configData);
	blueBeam->logBuffer = &beamLog;
	fileList = NULL;

	clipBoard = new (ELeave) CArrayFixSeg<TFileName>(1);	
	currentPathScroller = new (ELeave) CMarquee;
	
	iOldPath.Zero();
	currentPath.Zero();
	
	iFileManager = new (ELeave) CSManFileMan(configData);
}

TBool CSMan2FileListView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2FileListView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2FileListView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		// Stop the marquee to conserve battery power
		if (currentPathScroller->IsActive())
			currentPathScroller->Cancel();
			
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
	}
}

void CSMan2FileListView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
	if (!currentPathScroller->IsActive())
		currentPathScroller->startMarquee(labelCurrentFolder, &currentPath);
}

TVwsViewId CSMan2FileListView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidFileView);
}

CSMan2FileListView::~CSMan2FileListView()
{
	// DEBUG
	releaseFont();
	for (int i = 0; i < KMaxDrives; i++)
	{
		if (driveOldPath[i] != NULL)
			delete driveOldPath[i];
		if (iDirHighlightIndex[i] != NULL)
			delete iDirHighlightIndex[i];
	}
	delete crcCalculator;
	delete blueBeam;
	delete clipBoard;
	if (fileList)
		delete fileList;
	if (currentPathScroller->IsActive())
		currentPathScroller->Cancel();
	delete currentPathScroller;
	currentPathScroller = NULL;
	
	delete iFileManager;
}

TKeyResponse CSMan2FileListView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyConfirm)
		{
			TPointerEvent fakeEvent;

			// This modifier is almost impossible to get in real life (and definitely impossible on the P800 due to lack of keyboard).
			// We use this modifier to tell the pointer handler that this is a fake event.
			fakeEvent.iModifiers = EAllModifiers;
			fakeEvent.iPosition = cFileListBox->View()->ItemPos(cFileListBox->CurrentItemIndex());
			fakeEvent.iPosition.iX = cFileListBox->View()->ItemDrawer()->MarkColumn() + 1;	// Force x coordinate to the right.
			fakeEvent.iType = TPointerEvent::EButton1Down;
			// Note, this event must never be passed to the default pointer event handler!!
			// In this case, we're sure it will never happen because our overridden handler
			// will consume it.
			HandlePointerEventL(fakeEvent);
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			cFileListBox->View()->MoveCursorL(CListBoxView::ECursorNextItem, CListBoxView::ENoSelection);
			cFileListBox->UpdateScrollBarsL();
			updateFileDateTime();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			cFileListBox->View()->MoveCursorL(CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection);
			cFileListBox->UpdateScrollBarsL();
			updateFileDateTime();
			return EKeyWasConsumed;
		}
	}
	return cFileListBox->OfferKeyEventL(aKeyEvent, aType);
}

void CSMan2FileListView::getFileAttrib(TDes *attribText, TInt index)
{
	if ((*fileList)[index].IsReadOnly())
		attribText->Append(_L("R"));
	else
		attribText->Append(_L("-"));
		
	if ((*fileList)[index].IsSystem())
		attribText->Append(_L("S"));
	else
		attribText->Append(_L("-"));

	if ((*fileList)[index].IsHidden())
		attribText->Append(_L("H"));
	else
		attribText->Append(_L("-"));
		
	if ((*fileList)[index].IsArchive())
		attribText->Append(_L("A"));
	else
		attribText->Append(_L("-"));
}

void CSMan2FileListView::updateFileDateTime(void)
{
	TTime fileDateTime;
	TBuf<7> attrib;
	
	attrib.Copy(_L(""));
	textFileDateTime = _L("");
	if (fileList)
	{
		if (fileList->Count() > 0)
		{
			// Compensate for ".." entry. fileList does not have this entry while cFileListBox
			// does!
			if (currentPath.Length() > 3)
			{
				if ((cFileListBox->CurrentItemIndex() > 0) && (cFileListBox->CurrentItemIndex() <= fileList->Count()))
				{
					fileDateTime = (*fileList)[cFileListBox->CurrentItemIndex() - 1].iModified;
					fileDateTime.FormatL(textFileDateTime, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B"));
					getFileAttrib(&attrib, cFileListBox->CurrentItemIndex() - 1);
				}
			}
			else
			{
				if (cFileListBox->CurrentItemIndex() <= fileList->Count() - 1)
				{
					fileDateTime = (*fileList)[cFileListBox->CurrentItemIndex()].iModified;
					fileDateTime.FormatL(textFileDateTime, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B"));
					getFileAttrib(&attrib, cFileListBox->CurrentItemIndex());
				}
			}
		}
	}
	attrib.Append(_L("  ")); 
	textFileDateTime.Insert(0, attrib);
	labelFileDateTime->SetTextL(textFileDateTime);
	labelFileDateTime->DrawNow();
}

void CSMan2FileListView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	TBool consumed = EFalse;
	TInt theItem;
	TBool pointValid = cFileListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, theItem);
	TPtrC tempText, selectedType, selectedName;

	if ((cFileListBox->Model()->ItemTextArray()->MdcaCount() > 0) && (pointValid) && (!cFileListBox->DrawableWindow()->IsFaded()))
	{
		tempText.Set(cFileListBox->Model()->ItemTextArray()->MdcaPoint(theItem));
		TextUtils::ColumnText(selectedType, 1, &tempText);
		TextUtils::ColumnText(selectedName, 0, &tempText);

		if (aPointerEvent.iType == TPointerEvent::EDrag)
		{
			if ((pointValid) && (oldItem != theItem) && (aPointerEvent.iPosition.iX <= cFileListBox->View()->ItemDrawer()->MarkColumn()))
			{
				consumed = ETrue;
				if ((selectedType.Compare(_L("<drv>")) != 0) && (selectedName.Compare(_L("..")) != 0))
				{
					cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
					cFileListBox->UpdateScrollBarsL();
				}
				oldItem = theItem;
			}
		}
		else if (aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
			consumed = ETrue;
			oldItem = theItem;
			if (aPointerEvent.iPosition.iX > cFileListBox->View()->ItemDrawer()->MarkColumn())
			{
				// Change folder
				tempText.Set(cFileListBox->Model()->ItemTextArray()->MdcaPoint(theItem));
				
				// Check if user selected directory
				if ((selectedType.Compare(_L("<dir>")) == 0) || (selectedType.Compare(_L("<drv>")) == 0))
				{
					// Did user click on the filename?
					if (aPointerEvent.iPosition.iX < (((CColumnListBoxItemDrawer*)cFileListBox->ItemDrawer())->ColumnData()->ColumnWidthPixel(0) + cFileListBox->View()->ItemDrawer()->MarkColumn()))
					{
						cFileListBox->ClearSelection();
						// Highlight the item very briefly
						cFileListBox->SetCurrentItemIndexAndDraw(theItem);
						cFileListBox->SetFocus(ETrue, EDrawNow);
						User::After(5000);
						
						//cFileListBox->Reset();
						//((CDesCArray *) cFileListBox->Model()->ItemTextArray())->Compress();
						
						if (selectedName.Compare(_L("..")) == 0)
						{
							// One level up
							currentPath.SetLength(currentPath.Length() - 1); // remove slash
							currentPath.SetLength(currentPath.LocateReverse(TChar('\\')));
						}
						else
						{
							if (selectedName.Right(1) != _L(":"))
								currentPath.Append(selectedName);
							else
							{
								TInt driveIndex = (selectedName.Left(1))[0] - 65;
								if (driveOldPath[driveIndex] != NULL)
									currentPath.Copy(*driveOldPath[driveIndex]);
								else
									currentPath.Copy(selectedName);
								if (currentPath.Right(1).Compare(_L("\\")) == 0)
									currentPath.SetLength(currentPath.Length() - 1);
							}
						}
						currentPath.Append(_L("\\"));
						fillListBox(currentPath);
					}
					else
					// No, user clicked on file size column... then highlight only
					{
						cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::ENoSelection);
						cFileListBox->UpdateScrollBarsL();
						updateFileDateTime();
					}
				}
				else
				{
					// Did user click on the filename?
					if (aPointerEvent.iPosition.iX < (((CColumnListBoxItemDrawer*)cFileListBox->ItemDrawer())->ColumnData()->ColumnWidthPixel(0) + cFileListBox->View()->ItemDrawer()->MarkColumn()))
					{
						// Is this a fake pointer event generated by our key handler? If not, process it
						// like an open command else do nothing
						if (aPointerEvent.iModifiers == EAllModifiers)
							cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
						else
							cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::ENoSelection);
						cFileListBox->UpdateScrollBarsL();
						updateFileDateTime();
						
						if ((aPointerEvent.iModifiers != EAllModifiers) && ((configData->autoOpenFile) || (configData->autoRunApp)))
							static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->doProcessCommand(cmdFileManOpen);

/*					
						// Modified 24th Apr. Pressing jogdial down now opens the file by default. If
						// the autoopen file / autorunapp is not configured, it reverts to old behaviour
						
						// Is this a fake pointer event generated by our key handler? If not, process it
						// like an open command else do nothing
						if (aPointerEvent.iModifiers == EAllModifiers)
							cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
						else
							cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::ENoSelection);
						cFileListBox->UpdateScrollBarsL();
						updateFileDateTime();
						
						if ((configData->autoOpenFile) || (configData->autoRunApp))
							static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->doProcessCommand(cmdFileManOpen);
						else if (aPointerEvent.iModifiers == EAllModifiers)
							cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
*/
					}
					else
					// No, user clicked on file size column... then highlight only
					{
						cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::ENoSelection);
						cFileListBox->UpdateScrollBarsL();
						updateFileDateTime();
					}
				}					
			}
			else
			{
				tempText.Set(cFileListBox->Model()->ItemTextArray()->MdcaPoint(theItem));
				
				// Only allow the user to select if it is not a drive or ".."
				if ((selectedType.Compare(_L("<drv>")) != 0) && (selectedName.Compare(_L("..")) != 0))
				{
					cFileListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
					cFileListBox->UpdateScrollBarsL();
					updateFileDateTime();
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_WRONGSELECT);
			}
		}
	}
	// Prevent user from clicking on the label buttons we use as display for
	// folder path and file date time
	if (aPointerEvent.iPosition.iY < cFileListBox->Position().iY)
		consumed = ETrue;

	if (!consumed)
		CCoeControl::HandlePointerEventL(aPointerEvent);
}

TInt CSMan2FileListView::GetDirHighlightIndex(TPath *aPath)
{
	TInt iDirArrayIndex = -1;
	TInt iDriveIndex = aPath->Left(1)[0] - 65;
	
	if (!iDirHighlightIndex[iDriveIndex])
		iDirHighlightIndex[iDriveIndex] = new (ELeave) CArrayFixFlat<TInt>(1);
	for (int i = 0; i < aPath->Length(); i++)
	{
		if (aPath->Mid(i, 1).Compare(_L("\\")) == 0)
			iDirArrayIndex++;
			
		if (iDirArrayIndex >= iDirHighlightIndex[iDriveIndex]->Count())
			iDirHighlightIndex[iDriveIndex]->AppendL(0);
	}
	return iDirHighlightIndex[iDriveIndex]->At(iDirArrayIndex);
}

void CSMan2FileListView::SetDirHighlightIndex(TPath *aPath, TInt aIndex)
{
	TInt iDirArrayIndex = -1;
	TInt iDriveIndex = aPath->Left(1)[0] - 65;

	if (!iDirHighlightIndex[iDriveIndex])
		iDirHighlightIndex[iDriveIndex] = new (ELeave) CArrayFixFlat<TInt>(1);
	for (int i = 0; i < aPath->Length(); i++)
	{
		if (aPath->Mid(i, 1).Compare(_L("\\")) == 0)
			iDirArrayIndex++;
		if (iDirArrayIndex >= iDirHighlightIndex[iDriveIndex]->Count())
			iDirHighlightIndex[iDriveIndex]->AppendL(0);			
	}
	iDirHighlightIndex[iDriveIndex]->Delete(iDirArrayIndex);
	iDirHighlightIndex[iDriveIndex]->InsertL(iDirArrayIndex, aIndex);
	while ((iDirHighlightIndex[iDriveIndex]->Count() - 1) > iDirArrayIndex)
		iDirHighlightIndex[iDriveIndex]->Delete(iDirHighlightIndex[iDriveIndex]->Count() - 1);
}

// Returns TRUE if there were no errors in reading the folder and FALSE otherwise
TBool CSMan2FileListView::fillListBox(TPath thePath)
{
	RFs fileServer;
	TPath temp;
	TBuf<20> fileSize;
	CDesCArray *iArray;
	TInt i, iCurrentHighlightPos;
	TBool iErrorReadingFolder = EFalse;
	
	iCurrentHighlightPos = cFileListBox->CurrentItemIndex();
	if (iCurrentHighlightPos < 0)
		iCurrentHighlightPos = 0;

	temp.Copy(thePath);
	temp.Append(_L("*"));
	fileServer.Connect();
	cFileListBox->ClearSelection();
	iArray = ((CDesCArray *) cFileListBox->Model()->ItemTextArray());
	iArray->Reset();
	iArray->Compress();
	cFileListBox->HandleItemRemovalL();
	
	if (fileList)
		delete fileList;
		
	TInt attribs = KEntryAttMaskSupported;
	if (!configData->showHiddenFiles)
		attribs ^= KEntryAttHidden;
		
	TInt dirDirection = EDirsLast;
	if (configData->filemanSortOrder == EAscending)
		dirDirection = EDirsFirst;

	TFileName tempText;
	if (fileServer.GetDir(temp, attribs, configData->filemanSortBy | configData->filemanSortOrder | dirDirection, fileList) == KErrNone)
	{
		if (iOldPath.Length() > 0)
			SetDirHighlightIndex(&iOldPath, iCurrentHighlightPos);

		currentPath.SetLength(0);
		
		// Navigation upwards. Only allow if thePath is not a root folder
		if (thePath.Length() > 3)
		{
			tempText.Copy(_L(".."));
			tempText.Append(KColumnListSeparator);
			tempText.Append(_L("<dir>"));
			iArray->AppendL(tempText);
		}
		currentPath.Append(thePath);
		for (i = 0; i < fileList->Count(); i++)
		{
			tempText = (*fileList)[i].iName;
			tempText.Append(KColumnListSeparator);
			
			// Dir check
			if ((*fileList)[i].IsDir())
				tempText.Append(_L("<dir>"));
			else
			{
				if (((*fileList)[i].iSize) < 1024)
				{
					fileSize.Num((*fileList)[i].iSize);
					fileSize.Append(_L("b"));
				}
				else
				{
					fileSize.Num((*fileList)[i].iSize / 1024);
					fileSize.Append(_L("k"));
				}
				tempText.Append(fileSize);
			}
			iArray->AppendL(tempText);
		}
	}
	else
	{
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRREADFOLDER);
		labelFileDateTime->SetTextL(_L(""));
		labelFileDateTime->DrawNow();
		iErrorReadingFolder = ETrue;
	}
	// Add drive letters
	TVolumeInfo volInfo;	
	for (i = 0; i < KMaxDrives; i++)
	{
		if (CEikonEnv::Static()->FsSession().Volume(volInfo, EDriveA + i) == KErrNone)
		{
			tempText.Format(_L("%c:"), 0x41 + i);
			tempText.Append(KColumnListSeparator);
			tempText.Append(_L("<drv>"));
			iArray->AppendL(tempText);	
		}
		else
		{
			if (driveOldPath[i])
			{
				delete driveOldPath[i];
				driveOldPath[i] = NULL;
			}
		}
	}
	if ((currentPath.Length() > 0) && (iErrorReadingFolder == EFalse))
	{
		TInt iDriveIndex = currentPath.Left(1)[0] - 65;	// 65 = 'A'
		if (!driveOldPath[iDriveIndex])
			driveOldPath[iDriveIndex] = HBufC::NewL(KMaxPath);
		driveOldPath[iDriveIndex]->Des().Copy(currentPath);
	}
	// Finish up
	cFileListBox->HandleItemRemovalL();
	cFileListBox->SetFocus(ETrue);	
	iArray = ((CDesCArray *) cFileListBox->Model()->ItemTextArray());
	if (iArray->MdcaCount() > 0)
	{
		iCurrentHighlightPos = GetDirHighlightIndex(&currentPath);
		if (iCurrentHighlightPos >= iArray->MdcaCount())
			iCurrentHighlightPos = iArray->MdcaCount() - 1;
		cFileListBox->SetCurrentItemIndex(iCurrentHighlightPos);
		cFileListBox->ScrollToMakeItemVisible(iCurrentHighlightPos);
		updateFileDateTime();
	}
	cFileListBox->DrawNow();
	
	// DEBUG cFileListBox->HandleItemAdditionL();
	fileServer.Close();
	labelCurrentFolder->SetTextL(currentPath);
	labelCurrentFolder->DrawNow();
	iOldPath.Copy(currentPath);
	
	return (!iErrorReadingFolder);
}

void CSMan2FileListView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	// Initialize current folder label
	labelCurrentFolder = new (ELeave) CEikTextButton;
	labelCurrentFolder->SetTextL(currentPath);
	labelCurrentFolder->SetContainerWindowL(*this);
	labelCurrentFolder->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, 22));
	labelCurrentFolder->SetBorder(TGulBorder::EDeepSunken);
	//labelCurrentFolder->Label()->SetEmphasis(CEikLabel::EFullEmphasis);
	controlsArray->AppendL(labelCurrentFolder);
		
	// Initialize file date time
	labelFileDateTime = new (ELeave) CEikTextButton;
	textFileDateTime.Copy(_L(""));
	labelFileDateTime->SetTextL(textFileDateTime);
	labelFileDateTime->SetContainerWindowL(*this);
	labelFileDateTime->SetExtent(TPoint(0, labelCurrentFolder->Size().iHeight), TSize(aRect.Size().iWidth, labelCurrentFolder->Size().iHeight));
	labelFileDateTime->SetBorder(TGulBorder::ESingleDotted);
	labelCurrentFolder->Label()->SetEmphasis(CEikLabel::ENoEmphasis);
	controlsArray->AppendL(labelFileDateTime);

	// Initialize listbox
	cFileListBox = new(ELeave) CEikColumnListBox;
	cFileListBox->ConstructL(this, CEikColumnListBox::EMultipleSelection);
	CColumnListBoxData* listBoxColumns = ((CColumnListBoxItemDrawer*)cFileListBox->ItemDrawer())->ColumnData();
	listBoxColumns->SetColumnWidthPixelL(0, int(Size().iWidth * 0.58));
	listBoxColumns->SetColumnWidthPixelL(1, int(Size().iWidth * 0.30));
	listBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	listBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ERight);
	cFileListBox->CreateScrollBarFrameL();
	cFileListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	cFileListBox->SetExtent(TPoint(0, labelCurrentFolder->Size().iHeight + labelFileDateTime->Size().iHeight), TSize(aRect.Size().iWidth, 
		Size().iHeight - labelCurrentFolder->Size().iHeight - labelFileDateTime->Size().iHeight - EQikToolbarHeight + 4 - 10));
	cFileListBox->View()->ItemDrawer()->SetMarkGutter(0);
	cFileListBox->View()->ItemDrawer()->SetMarkColumnWidth(10);		
	controlsArray->AppendL(cFileListBox);

	if (configData->beamProtocol == CBlueBeam::protocolIrDA)
		SetHighlightColor(KRgbRed, KRgbWhite);
	else
		SetHighlightColor(KRgbBlue, KRgbWhite);

	viewId = CSMan2AppUi::EViewFile;
	iBJackIconPosition = TPoint(2, cFileListBox->Position().iY + cFileListBox->Size().iHeight);
	InitBJackIcon();

	EnableDragEvents();

	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
		SetZoomLevel(configData->zoomFile);	
		
	MakeVisible(EFalse);
	ActivateL();
}

void CSMan2FileListView::SetZoomLevel(TInt zoomLevel)
{
	// DEBUG
	releaseFont();
	generateZoomFont(zoomLevel);
	CTextListItemDrawer* itemDrawer = ((CTextListItemDrawer*)cFileListBox->View()->ItemDrawer());
	itemDrawer->SetFont(font);
	cFileListBox->SetItemHeightL(font->HeightInPixels() + cFileListBox->VerticalInterItemGap());
	configData->zoomFile = zoomLevel;
	//DEBUG releaseFont();
}

TInt CSMan2FileListView::populateClipboard(TInt op)
{
	TPtrC tempText, entryName;
	TFileName currentFileName;
	TInt i;
	
	clipBoard->Reset();
	for (i = 0; i < cFileListBox->SelectionIndexes()->Count(); i++)
	{
		tempText.Set(cFileListBox->Model()->ItemTextArray()->MdcaPoint((cFileListBox->SelectionIndexes()->At(i))));
		TextUtils::ColumnText(entryName, 0, &tempText);
		currentFileName.Copy(currentPath);
		currentFileName.Append(entryName);
		clipBoard->AppendL(currentFileName);
	}
	clipBoardOperation = op;
	return i;
}

void CSMan2FileListView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}

void CSMan2FileListView::GetFile()
{
	TPtrC iTempText;
	
	iTempText.Set(cFileListBox->Model()->ItemTextArray()->MdcaPoint(cFileListBox->View()->CurrentItemIndex()));
	TextUtils::ColumnText(iEntryName, 0, &iTempText);
	TextUtils::ColumnText(iEntryType, 1, &iTempText);
}

void CSMan2FileListView::SetHighlightColor(TRgb aRgbHiLiteColor, TRgb aRgbTextColor)
{
	cFileListBox->ItemDrawer()->SetHighlightedBackColor(aRgbHiLiteColor);
	cFileListBox->ItemDrawer()->SetHighlightedTextColor(aRgbTextColor);
	cFileListBox->DrawDeferred();
}

void CSMan2FileListView::DoOpenFile(TInt aCommand)
{
	GetFile();
	
	iFileName.Copy(currentPath);
	iFileName.Append(iEntryName);
	iFileName.UpperCase();

	TParsePtr iTempName(iFileName);
	if (iTempName.Ext() == _L(".APP"))
	{
		if ((configData->autoRunApp) || (aCommand == cmdFileManOpenOverride))
			static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->FindAndLaunchAppNoUid(&(iTempName.FullName()));
	}
	else if (iTempName.Ext() == _L(".EXE"))
	{
		if ((configData->autoRunApp) || (aCommand == cmdFileManOpenOverride))
			EikDll::StartExeL(iTempName.FullName());
	}
	else if ((configData->autoOpenFile) || (aCommand == cmdFileManOpenOverride))
	{
		QikFileUtils::StartAppL(iFileName);
	}
}

void CSMan2FileListView::CalculateCRC()
{
	GetFile();
	iFileName.Copy(currentPath);
	iFileName.Append(iEntryName);
	crcCalculator->doCRC(iFileName);
}

void CSMan2FileListView::DoShowProperties()
{
	GetFile();
	CFileManAttrDialog *iAttribDialog = new (ELeave) CFileManAttrDialog();
	iAttribDialog->theFile.Copy(currentPath);
	iAttribDialog->theFile.Append(iEntryName);
	TInt iRetVal = iAttribDialog->ExecuteLD(R_DIALOG_FILEMANATT);
	if (iRetVal == EEikBidOk)
		fillListBox(currentPath);
}

void CSMan2FileListView::SetAllSelectionStatus(TBool aSelectionState)
{
	if (aSelectionState)
	{
		TPtrC iTempText;
		for (int i = 0; i < cFileListBox->Model()->ItemTextArray()->MdcaCount(); i++)
		{
			iTempText.Set(cFileListBox->Model()->ItemTextArray()->MdcaPoint(i));
			TextUtils::ColumnText(iEntryType, 1, &iTempText);
			TextUtils::ColumnText(iEntryName, 0, &iTempText);
			if ((iEntryType.Compare(_L("<drv>")) != 0) && (iEntryName.Compare(_L("..")) != 0))
				cFileListBox->View()->SelectItemL(i);
		}
	}
	else
		cFileListBox->ClearSelection();
}

void CSMan2FileListView::DoBeamFile()
{
	blueBeam->logHelper->logDialog = NULL;
	blueBeam->logHelper->logTarget = CLogger::logToDialog;
	blueBeam->beamerProtocol = configData->beamProtocol;
	blueBeam->startBeam();
}

void CSMan2FileListView::ReadPath()
{
	fillListBox(currentPath);
}

TBool CSMan2FileListView::ReadAlternatePath(TPath *aPath)
{
	return fillListBox(*aPath);
}

void CSMan2FileListView::DoCopyAs()
{
	GetFile();
	
	// There is an implicit assumption here that a fully qualified path (including filename)
	// will not be longer than KMaxFileName (which is 256 characters)

	TUint iFileCounter = 0;
	TBuf<KMaxFileName> iDuplicateName, iNoPathDuplicateName;
	TBuf<KMaxFileName> iSourceName;
	TBool iIsFolder, iDoDupe;
	TInt iRecursiveFlag = 0;
	
	iSourceName.Copy(currentPath);
	iSourceName.Append(iEntryName);
	
  	// Iterate through the list of files and search for the next available numerical slot
  	// The naming convention of COPYAS is:
  	// x.<original filename> where x = running number (without zero padding)
  	// The value of x can be as large as required provided you don't run out of numbers in
  	// TUint or the filenam length is not more than KMaxFileName
  	
  	if (iEntryType.Compare(_L("<dir>")) == 0)
  		iIsFolder = ETrue;
  	else
  		iIsFolder = EFalse;
  	
  	CEikonEnv::Static()->BusyMsgL(R_TBUF_FILEMAN_FINDINGSLOT, 0);
	while (iFileCounter <= 0xFFFFFFFF)
	{
		iDuplicateName.Format(_L("%u."), iFileCounter);
		iNoPathDuplicateName.Copy(iDuplicateName);
		iNoPathDuplicateName.Append(iEntryName);
		iDuplicateName.Insert(0, currentPath);
		if (iDuplicateName.Length() + iEntryName.Length() <= KMaxFileName)
		{
			iDoDupe = EFalse;
			iDuplicateName.Append(iEntryName);
			if (iIsFolder)
			{
				if (!EikFileUtils::FolderExists(iDuplicateName))
					iDoDupe = ETrue;
				iRecursiveFlag = CFileMan::ERecurse;
			}
			else
			{
				if (!ConeUtils::FileExists(iDuplicateName))
					iDoDupe = ETrue;
			}
			
			if (iDoDupe)
			{
				CEikonEnv::Static()->BusyMsgL(R_TBUF_FILEMAN_DUPLICATING);
				// Be very careful here. The call to CopyFile will succeed even with a 
				// locked file as the source name IF you specify to use the recursive flag..
				TInt iError = EikFileUtils::CopyFile(iSourceName, iDuplicateName, iRecursiveFlag);
				if (iError != KErrNone)
				{
					CEikonEnv::Static()->BusyMsgCancel();
					CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRORDUPE);
				}
				else
				{
					CEikonEnv::Static()->BusyMsgCancel();

					TTime tTime;
					tTime.HomeTime();
					CEikonEnv::Static()->FsSession().SetModified(iDuplicateName, tTime);
					HBufC *iMessage = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_DUPEDONE);
					iMessage = iMessage->ReAllocL(iMessage->Length() + iNoPathDuplicateName.Length());
					iMessage->Des().Append(iNoPathDuplicateName);
					CEikonEnv::Static()->InfoMsg(*iMessage);
					delete iMessage;
					ReadPath();
				}
				break;
			}
		}
		else
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_TOOMANYDUPES);
			break;
		}
		iFileCounter++;
	}
	CEikonEnv::Static()->BusyMsgCancel();
}

void CSMan2FileListView::DoCopyOrCut(TInt aCommand)
{
	TInt iOperation = (aCommand == cmdFileManCopy) ? CSManFileMan::operationCopy : CSManFileMan::operationCut;
	TInt iNumFiles;

	iNumFiles = populateClipboard(iOperation);
	if (iOperation == CSManFileMan::operationCut)
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_CUT, iNumFiles);
	else if (iOperation == CSManFileMan::operationCopy)
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_COPY, iNumFiles);
}

void CSMan2FileListView::DoFileManagerOperation(TInt aOperation)
{
	iFileManager->fileList = clipBoard;
	iFileManager->fileManOperation = clipBoardOperation;
	iFileManager->targetPath.Copy(currentPath);
	iFileManager->currentOperation = aOperation;
	iFileManager->doWork();
}

void CSMan2FileListView::ClearClipBoard()
{
	populateClipboard(-1);
}

void CSMan2FileListView::DoCreateNewFolder()
{
	CFileManNewDirDialog *iNewFolderDialog = new (ELeave) CFileManNewDirDialog;
	iNewFolderDialog->currentPath.Copy(currentPath);
	iNewFolderDialog->ExecuteLD(R_DIALOG_FILEMANNEWFOLDER);
	ReadPath();
}

void CSMan2FileListView::DoPrepareHexEditor(CSMan2HexEditorView *aHexEditView)
{
	GetFile();
	aHexEditView->iFileName.Copy(currentPath);
	aHexEditView->iFileName.Append(iEntryName);	
}

void CSMan2FileListView::DoPrepareFindFileCriteria(TFileName *aCriteria)
{
	aCriteria->Copy(currentPath);
	GetFile();
	if ((iEntryType.Compare(_L("<dir>")) == 0) && (iEntryName.Compare(_L("..")) != 0))
	{
		aCriteria->Append(iEntryName);
		aCriteria->Append(_L("\\"));
	}
	else
		aCriteria->Copy(_L(""));
}

void CSMan2FileListView::DoSwitchDrive(TChar aDriveLetter)
{
	TBuf<3> iDrive;
	
	iDrive.Format(_L("%c:\\"), aDriveLetter);
	fillListBox(iDrive);
}

void CSMan2FileListView::StartScroller()
{
	if (!(currentPathScroller->IsActive()))
		currentPathScroller->startMarquee(labelCurrentFolder, &(currentPath));
}

TInt CSMan2FileListView::NumItemsInClipboard()
{
	return clipBoard->Count();
}

/*************************************************************
*
* Provides asynchronous CRC calculation
*
**************************************************************/

CCRCCalculator::CCRCCalculator() : CActive(EPriorityNormal)
{
	crcDialogActive = EFalse;
//	inputBuffer = NULL;
	CActiveScheduler::Add(this);
	
	// Ansi C++ doesn't allow initialization of arrays. :(
	// No, I didn't hand-edit this. A simple VBS script formatted it for me. :)
	crcTable[0] = 0x0000;
	crcTable[1] = 0x77073096;
	crcTable[2] = 0xee0e612c;
	crcTable[3] = 0x990951ba;
	crcTable[4] = 0x076dc419;
	crcTable[5] = 0x706af48f;
	crcTable[6] = 0xe963a535;
	crcTable[7] = 0x9e6495a3;
	crcTable[8] = 0x0edb8832;
	crcTable[9] = 0x79dcb8a4;
	crcTable[10] = 0xe0d5e91e;
	crcTable[11] = 0x97d2d988;
	crcTable[12] = 0x09b64c2b;
	crcTable[13] = 0x7eb17cbd;
	crcTable[14] = 0xe7b82d07;
	crcTable[15] = 0x90bf1d91;
	crcTable[16] = 0x1db71064;
	crcTable[17] = 0x6ab020f2;
	crcTable[18] = 0xf3b97148;
	crcTable[19] = 0x84be41de;
	crcTable[20] = 0x1adad47d;
	crcTable[21] = 0x6ddde4eb;
	crcTable[22] = 0xf4d4b551;
	crcTable[23] = 0x83d385c7;
	crcTable[24] = 0x136c9856;
	crcTable[25] = 0x646ba8c0;
	crcTable[26] = 0xfd62f97a;
	crcTable[27] = 0x8a65c9ec;
	crcTable[28] = 0x14015c4f;
	crcTable[29] = 0x63066cd9;
	crcTable[30] = 0xfa0f3d63;
	crcTable[31] = 0x8d080df5;
	crcTable[32] = 0x3b6e20c8;
	crcTable[33] = 0x4c69105e;
	crcTable[34] = 0xd56041e4;
	crcTable[35] = 0xa2677172;
	crcTable[36] = 0x3c03e4d1;
	crcTable[37] = 0x4b04d447;
	crcTable[38] = 0xd20d85fd;
	crcTable[39] = 0xa50ab56b;
	crcTable[40] = 0x35b5a8fa;
	crcTable[41] = 0x42b2986c;
	crcTable[42] = 0xdbbbc9d6;
	crcTable[43] = 0xacbcf940;
	crcTable[44] = 0x32d86ce3;
	crcTable[45] = 0x45df5c75;
	crcTable[46] = 0xdcd60dcf;
	crcTable[47] = 0xabd13d59;
	crcTable[48] = 0x26d930ac;
	crcTable[49] = 0x51de003a;
	crcTable[50] = 0xc8d75180;
	crcTable[51] = 0xbfd06116;
	crcTable[52] = 0x21b4f4b5;
	crcTable[53] = 0x56b3c423;
	crcTable[54] = 0xcfba9599;
	crcTable[55] = 0xb8bda50f;
	crcTable[56] = 0x2802b89e;
	crcTable[57] = 0x5f058808;
	crcTable[58] = 0xc60cd9b2;
	crcTable[59] = 0xb10be924;
	crcTable[60] = 0x2f6f7c87;
	crcTable[61] = 0x58684c11;
	crcTable[62] = 0xc1611dab;
	crcTable[63] = 0xb6662d3d;
	crcTable[64] = 0x76dc4190;
	crcTable[65] = 0x01db7106;
	crcTable[66] = 0x98d220bc;
	crcTable[67] = 0xefd5102a;
	crcTable[68] = 0x71b18589;
	crcTable[69] = 0x06b6b51f;
	crcTable[70] = 0x9fbfe4a5;
	crcTable[71] = 0xe8b8d433;
	crcTable[72] = 0x7807c9a2;
	crcTable[73] = 0x0f00f934;
	crcTable[74] = 0x9609a88e;
	crcTable[75] = 0xe10e9818;
	crcTable[76] = 0x7f6a0dbb;
	crcTable[77] = 0x086d3d2d;
	crcTable[78] = 0x91646c97;
	crcTable[79] = 0xe6635c01;
	crcTable[80] = 0x6b6b51f4;
	crcTable[81] = 0x1c6c6162;
	crcTable[82] = 0x856530d8;
	crcTable[83] = 0xf262004e;
	crcTable[84] = 0x6c0695ed;
	crcTable[85] = 0x1b01a57b;
	crcTable[86] = 0x8208f4c1;
	crcTable[87] = 0xf50fc457;
	crcTable[88] = 0x65b0d9c6;
	crcTable[89] = 0x12b7e950;
	crcTable[90] = 0x8bbeb8ea;
	crcTable[91] = 0xfcb9887c;
	crcTable[92] = 0x62dd1ddf;
	crcTable[93] = 0x15da2d49;
	crcTable[94] = 0x8cd37cf3;
	crcTable[95] = 0xfbd44c65;
	crcTable[96] = 0x4db26158;
	crcTable[97] = 0x3ab551ce;
	crcTable[98] = 0xa3bc0074;
	crcTable[99] = 0xd4bb30e2;
	crcTable[100] = 0x4adfa541;
	crcTable[101] = 0x3dd895d7;
	crcTable[102] = 0xa4d1c46d;
	crcTable[103] = 0xd3d6f4fb;
	crcTable[104] = 0x4369e96a;
	crcTable[105] = 0x346ed9fc;
	crcTable[106] = 0xad678846;
	crcTable[107] = 0xda60b8d0;
	crcTable[108] = 0x44042d73;
	crcTable[109] = 0x33031de5;
	crcTable[110] = 0xaa0a4c5f;
	crcTable[111] = 0xdd0d7cc9;
	crcTable[112] = 0x5005713c;
	crcTable[113] = 0x270241aa;
	crcTable[114] = 0xbe0b1010;
	crcTable[115] = 0xc90c2086;
	crcTable[116] = 0x5768b525;
	crcTable[117] = 0x206f85b3;
	crcTable[118] = 0xb966d409;
	crcTable[119] = 0xce61e49f;
	crcTable[120] = 0x5edef90e;
	crcTable[121] = 0x29d9c998;
	crcTable[122] = 0xb0d09822;
	crcTable[123] = 0xc7d7a8b4;
	crcTable[124] = 0x59b33d17;
	crcTable[125] = 0x2eb40d81;
	crcTable[126] = 0xb7bd5c3b;
	crcTable[127] = 0xc0ba6cad;
	crcTable[128] = 0xedb88320;
	crcTable[129] = 0x9abfb3b6;
	crcTable[130] = 0x03b6e20c;
	crcTable[131] = 0x74b1d29a;
	crcTable[132] = 0xead54739;
	crcTable[133] = 0x9dd277af;
	crcTable[134] = 0x04db2615;
	crcTable[135] = 0x73dc1683;
	crcTable[136] = 0xe3630b12;
	crcTable[137] = 0x94643b84;
	crcTable[138] = 0x0d6d6a3e;
	crcTable[139] = 0x7a6a5aa8;
	crcTable[140] = 0xe40ecf0b;
	crcTable[141] = 0x9309ff9d;
	crcTable[142] = 0x0a00ae27;
	crcTable[143] = 0x7d079eb1;
	crcTable[144] = 0xf00f9344;
	crcTable[145] = 0x8708a3d2;
	crcTable[146] = 0x1e01f268;
	crcTable[147] = 0x6906c2fe;
	crcTable[148] = 0xf762575d;
	crcTable[149] = 0x806567cb;
	crcTable[150] = 0x196c3671;
	crcTable[151] = 0x6e6b06e7;
	crcTable[152] = 0xfed41b76;
	crcTable[153] = 0x89d32be0;
	crcTable[154] = 0x10da7a5a;
	crcTable[155] = 0x67dd4acc;
	crcTable[156] = 0xf9b9df6f;
	crcTable[157] = 0x8ebeeff9;
	crcTable[158] = 0x17b7be43;
	crcTable[159] = 0x60b08ed5;
	crcTable[160] = 0xd6d6a3e8;
	crcTable[161] = 0xa1d1937e;
	crcTable[162] = 0x38d8c2c4;
	crcTable[163] = 0x4fdff252;
	crcTable[164] = 0xd1bb67f1;
	crcTable[165] = 0xa6bc5767;
	crcTable[166] = 0x3fb506dd;
	crcTable[167] = 0x48b2364b;
	crcTable[168] = 0xd80d2bda;
	crcTable[169] = 0xaf0a1b4c;
	crcTable[170] = 0x36034af6;
	crcTable[171] = 0x41047a60;
	crcTable[172] = 0xdf60efc3;
	crcTable[173] = 0xa867df55;
	crcTable[174] = 0x316e8eef;
	crcTable[175] = 0x4669be79;
	crcTable[176] = 0xcb61b38c;
	crcTable[177] = 0xbc66831a;
	crcTable[178] = 0x256fd2a0;
	crcTable[179] = 0x5268e236;
	crcTable[180] = 0xcc0c7795;
	crcTable[181] = 0xbb0b4703;
	crcTable[182] = 0x220216b9;
	crcTable[183] = 0x5505262f;
	crcTable[184] = 0xc5ba3bbe;
	crcTable[185] = 0xb2bd0b28;
	crcTable[186] = 0x2bb45a92;
	crcTable[187] = 0x5cb36a04;
	crcTable[188] = 0xc2d7ffa7;
	crcTable[189] = 0xb5d0cf31;
	crcTable[190] = 0x2cd99e8b;
	crcTable[191] = 0x5bdeae1d;
	crcTable[192] = 0x9b64c2b0;
	crcTable[193] = 0xec63f226;
	crcTable[194] = 0x756aa39c;
	crcTable[195] = 0x026d930a;
	crcTable[196] = 0x9c0906a9;
	crcTable[197] = 0xeb0e363f;
	crcTable[198] = 0x72076785;
	crcTable[199] = 0x05005713;
	crcTable[200] = 0x95bf4a82;
	crcTable[201] = 0xe2b87a14;
	crcTable[202] = 0x7bb12bae;
	crcTable[203] = 0x0cb61b38;
	crcTable[204] = 0x92d28e9b;
	crcTable[205] = 0xe5d5be0d;
	crcTable[206] = 0x7cdcefb7;
	crcTable[207] = 0x0bdbdf21;
	crcTable[208] = 0x86d3d2d4;
	crcTable[209] = 0xf1d4e242;
	crcTable[210] = 0x68ddb3f8;
	crcTable[211] = 0x1fda836e;
	crcTable[212] = 0x81be16cd;
	crcTable[213] = 0xf6b9265b;
	crcTable[214] = 0x6fb077e1;
	crcTable[215] = 0x18b74777;
	crcTable[216] = 0x88085ae6;
	crcTable[217] = 0xff0f6a70;
	crcTable[218] = 0x66063bca;
	crcTable[219] = 0x11010b5c;
	crcTable[220] = 0x8f659eff;
	crcTable[221] = 0xf862ae69;
	crcTable[222] = 0x616bffd3;
	crcTable[223] = 0x166ccf45;
	crcTable[224] = 0xa00ae278;
	crcTable[225] = 0xd70dd2ee;
	crcTable[226] = 0x4e048354;
	crcTable[227] = 0x3903b3c2;
	crcTable[228] = 0xa7672661;
	crcTable[229] = 0xd06016f7;
	crcTable[230] = 0x4969474d;
	crcTable[231] = 0x3e6e77db;
	crcTable[232] = 0xaed16a4a;
	crcTable[233] = 0xd9d65adc;
	crcTable[234] = 0x40df0b66;
	crcTable[235] = 0x37d83bf0;
	crcTable[236] = 0xa9bcae53;
	crcTable[237] = 0xdebb9ec5;
	crcTable[238] = 0x47b2cf7f;
	crcTable[239] = 0x30b5ffe9;
	crcTable[240] = 0xbdbdf21c;
	crcTable[241] = 0xcabac28a;
	crcTable[242] = 0x53b39330;
	crcTable[243] = 0x24b4a3a6;
	crcTable[244] = 0xbad03605;
	crcTable[245] = 0xcdd70693;
	crcTable[246] = 0x54de5729;
	crcTable[247] = 0x23d967bf;
	crcTable[248] = 0xb3667a2e;
	crcTable[249] = 0xc4614ab8;
	crcTable[250] = 0x5d681b02;
	crcTable[251] = 0x2a6f2b94;
	crcTable[252] = 0xb40bbe37;
	crcTable[253] = 0xc30c8ea1;
	crcTable[254] = 0x5a05df1b;
	crcTable[255] = 0x2d02ef8d;
}

CCRCCalculator::~CCRCCalculator()
{
}

void CCRCCalculator::stopCRC(TInt aReason)
{
	if (crcDialogActive)
		delete crcDialog;
	crcDialogActive = EFalse;
/*
	if (inputBuffer)
	{
		delete inputBuffer;
		inputBuffer = NULL;
	}
*/
	inputStream.Close();
	if (aReason == CRCCancelled)
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_CRCCANCELLED);
	else if (aReason == CRCError)
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRREADFILE);
}

void CCRCCalculator::DoCancel()
{
	stopCRC(CRCCancelled);
}

void CCRCCalculator::finishCRC()
{
	if (crcDialogActive)
	{
		TBuf<12> crcText;
		
		crcText.Format(_L("0x%x"), crcValue);
		crcDialog->updateProgress(100);
		crcDialog->ButtonGroupContainer().DimCommand(EEikBidOk, EFalse);
		crcDialog->ButtonGroupContainer().DimCommand(EEikBidCancel, ETrue);
		static_cast<CEikLabel*>(crcDialog->Control(cCRCValue))->SetTextL(crcText);
		static_cast<CEikLabel*>(crcDialog->Control(cCRCValue))->DrawNow();
	}
	//delete inputBuffer;
	//inputBuffer = NULL;
	inputStream.Close();
}

void CCRCCalculator::RunL()
{
	if (!crcDialogActive)
		stopCRC(CRCCancelled);
	else
	{
		if (iStatus == KErrNone)
		{
			// File read finished?
			if (inputBuffer.Length() == 0)
			{
				finishCRC();
			}
			else
			{
				// This if check isn't entirely redundant even though it was performed at the
				// beginning of this member function. A user may task away...and I'm not sure if
				// this action runs as a parallel thread (not part of an active object on the 
				// server)...........???
				if (crcDialogActive)
				{
					numBytesRead += inputBuffer.Length();
					crcDialog->updateProgress(TInt((TReal(numBytesRead) / TReal(fileSize)) * 100));
				}
				
				for (int i = 0; i < inputBuffer.Length(); i++)
					//crcValue = (crcValue >> 8) ^ crcTable[(crcValue & 0xff) ^ *(inputBuffer->Ptr() + i)];
					crcValue = (crcValue >> 8) ^ crcTable[(crcValue & 0xff) ^ (inputBuffer[i])];
					
				if (numBytesRead != fileSize)
				{
					//TPtr8 memBuffer(inputBuffer->Des());
					iStatus = KRequestPending;
					//inputStream.Read(memBuffer, CRC_BUFFER_SIZE / 2, iStatus);
					inputStream.Read(inputBuffer, iStatus);					
					SetActive();
				}
				else
					finishCRC();
			}
		}
		else
		{
			stopCRC(CRCError);
		}
	}
}

void CCRCCalculator::doCRC(TPtrC fileName)
{	
	/*
	if (inputBuffer)
		delete inputBuffer;
	inputBuffer = HBufC8::NewL(CRC_BUFFER_SIZE);
	*/
	if (inputStream.Open(CEikonEnv::Static()->FsSession(), fileName, EFileRead) == KErrNone)
	{
		crcValue = 0;
		crcDialog = new (ELeave) CCRCDialog(fileName);
		crcDialog->ExecuteLD(R_DIALOG_FILEMAN_CRC);
		numBytesRead = 0;
		inputStream.Size(fileSize);

		//TPtr8 memBuffer(inputBuffer->Des());
		iStatus = KRequestPending;
		//inputStream.Read(memBuffer, CRC_BUFFER_SIZE / 2, iStatus);
		inputStream.Read(inputBuffer, iStatus);
		SetActive();
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRREADFILE);
}