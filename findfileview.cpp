#include "findfileview.h"
#include "sman.h"
#include <SMan.rsg>

/*************************************************************
*
* File scanner
*
* Mar 10, 2004
* ------------
* You can see from the design of this class that it was
* retrofitted :( only because I *JUST* found out Symbian doesn't
* support multiple inheritence. Sigh....but, it's a good thing, eh?
*
* See:
* http://www.symbian.com/developer/techlib/v70docs/SDL_v7.0/
* doc_source/DevGuides/EssentialIdioms/MultipleInheritance.guide.html
*
**************************************************************/

CSMan2FindFileScanner::CSMan2FindFileScanner(CSMan2FindFileView *aFindFileView) : CActive(EPriorityNormal)
{
	CActiveScheduler::Add(this);
	iLocalFindFileView = aFindFileView;	
	iCurrentOperation = ScanningFolders;
}

void CSMan2FindFileScanner::ScanDir(TInt aNextOperation)
{
	TFileName iTempSearchWildCard;
	TUint iAttMask = 0;

	if (aNextOperation)
	{
		iTempSearchWildCard.Copy(iLocalFindFileView->iFoldersToScan->MdcaPoint(iLocalFindFileView->iIndexToFoldersToScan));
		if (iCurrentOperation == ScanningFolders)
		{
			iSearchSpecNoExt = EFalse;
			iAttMask = KEntryAttNormal | KEntryAttSystem | KEntryAttHidden;
			iCurrentOperation = ScanningFiles;
			
			/*************************************************************

			The RDir class is very badly documented. Inconsistencies in
			Read() documentation, behaviour, and in some cases two separate
			sections of the SDK describe the Read() function differently :(

			We make use of an "undocumented" behaviour in RDir that allows
			you to use wildcards to list folders. The only exception is
			when you specify a blank extension e.g. *. When you specify this, 
			nothing is returned!

			So we handle this case specially. If both the search specification
			and file found do not have an extension	then this is a match.

			**************************************************************/
			
			iTempSearchWildCard.Append(iLocalFindFileView->iSearchWildCard);
			if (iTempSearchWildCard[iTempSearchWildCard.Length() - 1] == 0x2e)	// 0x2e = full stop
			{
				iSearchSpecNoExt = ETrue;
				iTempSearchWildCard.Append(_L("*"));
			}
		}
		else if (iCurrentOperation == ScanningFiles)
		{

			/* Here's what the SDK says about the exclusive attribute

			Bit mask flag provided for exclusive matching. When OR'ed with one or more file 
			attribute constant, eg KEntryAttNormal, ensures that only the files with those 
			attributes are matched. When OR’ed with KEntryAttDir, directories only (not hidden 
			or system) are matched.
			*/

			iAttMask = KEntryAttDir | KEntryAttMatchExclusive;
			iCurrentOperation = ScanningFolders;
		}

		if (iLocalFindFileView->iDirScanner->Open(CEikonEnv::Static()->FsSession(), iTempSearchWildCard, iAttMask) == KErrNone)
		{
			iStatus = KRequestPending;
			iLocalFindFileView->iDirScanner->Read(iEntriesFiltered, iStatus);
			SetActive();
		}
		else
		{
			iLocalFindFileView->iErrorInScan = ETrue;
			
			// Terminate the scan the moment we find an error
			iLocalFindFileView->StopFileFind();
		}
	}
	else
	{
		iStatus = KRequestPending;
		iLocalFindFileView->iDirScanner->Read(iEntriesFiltered, iStatus);
		SetActive();
	}
}

void CSMan2FindFileScanner::InitDateCriteria()
{
	iDateCriteriaType = 0;
	if (iLocalFindFileView->iModifiedDateOption == 1)
	{
		iDateCriteriaType = 1;
		iTargetDate.HomeTime();
	}
	else if (iLocalFindFileView->iModifiedDateOption == 2)
	{
		iDateCriteriaType = 1;
		iTargetDate.HomeTime();
		iTargetDate -= TTimeIntervalDays(1);
	}
	else if (iLocalFindFileView->iModifiedDateOption == 3)
	{
		iDateCriteriaType = 2;
	}
}

void CSMan2FindFileScanner::RunL()
{
	if (iLocalFindFileView->iDirScanner)
	{
		// We could close this later on but i'd rather do it here to free up resources
		// quickly
		if (iStatus == KErrEof)
			iLocalFindFileView->iDirScanner->Close();
			
		if ((iStatus == KErrEof) || (iStatus == KErrNone))
		{		
			// Fish out all matching entries
			TFileName iToAppend;
			TInt iOne;
			
			if (iLocalFindFileView->iIndexToFoldersToScan > 0)
				iOne = 1;
			else
				iOne = 0;

			// I want this code to be fast and tight
			TInt i, x;
			if (iCurrentOperation == ScanningFolders)
			{
				TBool iIsDuplicate;
				TInt iFoldersToScanCount = iLocalFindFileView->iFoldersToScan->MdcaCount();
				for (i = 0; i < iEntriesFiltered.Count(); i++)
				{
					iToAppend.Copy(iLocalFindFileView->iFoldersToScan->MdcaPoint(iLocalFindFileView->iIndexToFoldersToScan));
					iToAppend.Append(iEntriesFiltered[i].iName);
					iToAppend.Append(_L("\\"));

					// Do not insert duplicates
					iIsDuplicate = EFalse;
					for (x = iLocalFindFileView->iIndexToFoldersToScan - iOne; x < iFoldersToScanCount; x++)
					{
						if (iToAppend.Compare(iLocalFindFileView->iFoldersToScan->MdcaPoint(x)) == 0)
						{
							iIsDuplicate = ETrue;
							break;
						}
					}
					if (!iIsDuplicate)
						iLocalFindFileView->iFoldersToScan->AppendL(iToAppend);
				}
			}
			else if (iCurrentOperation == ScanningFiles)
			{
				TBool iIsSpecialCaseMatch;
				TParse iFileFound;
				TInt iEntriesFilteredCount = iEntriesFiltered.Count();

				for (i = 0; i < iEntriesFilteredCount; i++)
				{
					iToAppend.Copy(iLocalFindFileView->iFoldersToScan->MdcaPoint(iLocalFindFileView->iIndexToFoldersToScan));
					iToAppend.Append(iEntriesFiltered[i].iName);
				
					iIsSpecialCaseMatch = EFalse;					
					if (iSearchSpecNoExt)
					{
						iFileFound.Set(iToAppend, NULL, NULL);
						if (!iFileFound.ExtPresent())
							iIsSpecialCaseMatch = ETrue;
					}
					
					if (iDateCriteriaType == 1)
					{
						if ((iEntriesFiltered[i].iModified.DayNoInYear() == iTargetDate.DayNoInYear())
							&& (iEntriesFiltered[i].iModified.DateTime().Year() == iTargetDate.DateTime().Year()))
						{
						 	if ((iIsSpecialCaseMatch) || (!iSearchSpecNoExt))
								((CDesCArray *)iLocalFindFileView->iResultList->Model()->ItemTextArray())->AppendL(iToAppend);
						}
					}
					else if (iDateCriteriaType == 2)
					{
						iEntriesFiltered[i].iModified.DateTime().SetHour(0);
						iEntriesFiltered[i].iModified.DateTime().SetMinute(0);
						iEntriesFiltered[i].iModified.DateTime().SetSecond(0);
						iEntriesFiltered[i].iModified.DateTime().SetMicroSecond(0);
						
						if ((iEntriesFiltered[i].iModified >= iLocalFindFileView->iFromTime)
							&& ((iEntriesFiltered[i].iModified <= iLocalFindFileView->iToTime)))
						{
							if ((iIsSpecialCaseMatch) || (!iSearchSpecNoExt))
								((CDesCArray *)iLocalFindFileView->iResultList->Model()->ItemTextArray())->AppendL(iToAppend);
						}
					}
					else if ((iIsSpecialCaseMatch) || (!iSearchSpecNoExt))
						((CDesCArray *)iLocalFindFileView->iResultList->Model()->ItemTextArray())->AppendL(iToAppend);
				}
			}
		}
		else
			iLocalFindFileView->iErrorInScan = ETrue;

		TBool iNextOperation = EFalse;

		if (iStatus == KErrEof)
		{
			iNextOperation = ETrue;
			if (iCurrentOperation == ScanningFiles)
				iLocalFindFileView->iIndexToFoldersToScan++;
		}
		
		if (iLocalFindFileView->iIndexToFoldersToScan < iLocalFindFileView->iFoldersToScan->MdcaCount())
			ScanDir(iNextOperation);
		else
			iLocalFindFileView->ScanningEnded();
	}
}

void CSMan2FindFileScanner::DoCancel()
{
	iLocalFindFileView->StopFileFind();
}

/*************************************************************
*
* Find file view.
*
**************************************************************/

CSMan2FindFileView::CSMan2FindFileView(CConfig *cData) : CViewBase(cData)
{	
	iDirScanner = NULL;
	iModifiedDateOption = 0;
	iFromTime.HomeTime();
	iToTime.HomeTime();
	iSearchWildCard.Copy(_L("*.*"));
	
	iFoldersToScan = new (ELeave) CDesCArraySeg(3);
	iAnchorFoldersToScan = 0;
	
	iFindFileScanner = new (ELeave) CSMan2FindFileScanner(this);
}

TBool CSMan2FindFileView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2FindFileView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2FindFileView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
		if (iDirScanner)
		{
			delete iDirScanner;
			iDirScanner = NULL;
		}
		((CDesCArray *)iResultList->Model()->ItemTextArray())->Compress();
		User::Heap().Compress();
	}
}

void CSMan2FindFileView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
	SetToolbarButtonState(EFalse);
}

TVwsViewId CSMan2FindFileView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidFindFileView);
}

CSMan2FindFileView::~CSMan2FindFileView()
{
	// DEBUG
	releaseFont();
	if (iDirScanner)
	{
		delete iDirScanner;
		iDirScanner = NULL;
	}
	delete iFoldersToScan;
	delete iFindFileScanner;
}

void CSMan2FindFileView::GotoFile()
{
	if (iResultList->Model()->NumberOfItems() > 0)
	{
		TFileName iHighlightedFileName, iTempUCaseFileName;
		TParse iParsedHighlightedItem;
		CSMan2FileListView *iFileListView = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView;
		TPtrC iTempText, iTempName, iTempType;
		
		iParsedHighlightedItem.Set(iResultList->Model()->ItemTextArray()->MdcaPoint(iResultList->View()->CurrentItemIndex()), NULL, NULL);
		TPath iTempPath;
		iTempPath.Copy(iParsedHighlightedItem.DriveAndPath());
		if (iFileListView->ReadAlternatePath(&iTempPath))
		{
			iHighlightedFileName.Copy(iParsedHighlightedItem.NameAndExt());
			iHighlightedFileName.UpperCase();
			CDesCArray *iFileListViewListBoxArray = (CDesCArray *)iFileListView->cFileListBox->Model()->ItemTextArray();
			for (int i = 0; i < iFileListViewListBoxArray->Count(); i++)
			{
				iTempText.Set(iFileListViewListBoxArray->MdcaPoint(i));

				TextUtils::ColumnText(iTempName, 0, &iTempText);
				TextUtils::ColumnText(iTempType, 1, &iTempText);
				
				if ((iTempType.Compare(_L("<dir>")) != 0) && (iTempType.Compare(_L("<drv>")) != 0)
					&& (iTempName.Compare(_L("..")) != 0))
				{
					iTempUCaseFileName.Copy(iTempName);
					iTempUCaseFileName.UpperCase();
					if (iHighlightedFileName.Compare(iTempUCaseFileName) == 0)
					{
						iFileListView->cFileListBox->View()->VerticalMoveToItemL(i, CListBoxView::ENoSelection);
						iFileListView->cFileListBox->UpdateScrollBarsL();
						iFileListView->updateFileDateTime();
						break;
					}
				}
			}
		}
		else
			iFileListView->ReadPath();
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->ActivateViewL(TVwsViewId(KUidSMan2App, KUidFileView));
	}
}

TKeyResponse CSMan2FindFileView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if ((aType == EEventKey) && (aKeyEvent.iCode == EQuartzKeyConfirm))
	{
		GotoFile();
		return EKeyWasConsumed;
	}
	return iResultList->OfferKeyEventL(aKeyEvent, aType);
}

// Could have used MEikListBoxObserver but nevermind.....
void CSMan2FindFileView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	TBool iPointerConsumed = EFalse;
	TInt iClickedItem;
	TBool iPointIsValid = iResultList->View()->XYPosToItemIndex(aPointerEvent.iPosition, iClickedItem);

	if (aPointerEvent.iType == TPointerEvent::EButton1Down)
	{
		if (iPointIsValid)
		{
			iPointerConsumed = ETrue;
			iResultList->View()->VerticalMoveToItemL(iClickedItem, CListBoxView::EDisjointSelection);
			iResultList->UpdateScrollBarsL();
			GotoFile();
		}
	}
	if (!iPointerConsumed)
		return iResultList->HandlePointerEventL(aPointerEvent);
}

void CSMan2FindFileView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	iResultList = new (ELeave) CEikTextListBox();
	iResultList->ConstructL(this, 0); //CEikListBox::EMultipleSelection | CEikListBox::ENoExtendedSelection);
	iResultList->CreateScrollBarFrameL();
	iResultList->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	iResultList->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, aRect.Size().iHeight /* - EQikToolbarHeight*/ - 10));
	controlsArray->AppendL(iResultList);
	iResultList->SetFocus(ETrue);
	
	viewId = CSMan2AppUi::EViewFindFile;
	iBJackIconPosition = TPoint(2, iResultList->Position().iY + iResultList->Size().iHeight);
	InitBJackIcon();
	
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
		SetZoomLevel(configData->zoomFindFile);
	
	EnableDragEvents();
	MakeVisible(EFalse);
	ActivateL();
}

void CSMan2FindFileView::SetZoomLevel(TInt zoomLevel)
{
	// DEBUG
	releaseFont();
	generateZoomFont(zoomLevel);
	CTextListItemDrawer* itemDrawer = ((CTextListItemDrawer*)iResultList->View()->ItemDrawer());
	itemDrawer->SetFont(font);
	iResultList->SetItemHeightL(font->HeightInPixels() + iResultList->VerticalInterItemGap());
	configData->zoomFindFile = zoomLevel;
	//DEBUG releaseFont();
}

void CSMan2FindFileView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}

void CSMan2FindFileView::SetToolbarButtonState(TBool aFindFileIsRunning)
{
	CEikButtonGroupContainer *iToolBar = CEikonEnv::Static()->AppUiFactory()->ToolBar();
	if (iToolBar)
	{
		if ((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileStart)))
			((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileStart)))->SetDimmed(aFindFileIsRunning);
		if ((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileStop)))
			((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileStop)))->SetDimmed(!aFindFileIsRunning);
		if ((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileBack)))
			((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileBack)))->SetDimmed(aFindFileIsRunning);
		if ((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileCriteria)))
			((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileCriteria)))->SetDimmed(aFindFileIsRunning);
			
		// Cannot start scanning until user has specified a search location
		if (iFoldersToScan->MdcaCount() == 0)
			if ((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileStart)))
				((CEikCommandButton*)(iToolBar->ControlOrNull(cmdFindFileStart)))->SetDimmed(ETrue);

		iToolBar->DrawNow();
	}
}

void CSMan2FindFileView::StartFileFind()
{
	CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY, 0);
	
	((CDesCArray *)iResultList->Model()->ItemTextArray())->Reset();
	iResultList->HandleItemRemovalL();
	((CDesCArray *)iResultList->Model()->ItemTextArray())->Compress();
	iResultList->DrawDeferred();

	if (!iDirScanner)
		iDirScanner = new (ELeave) RDir();

	iErrorInScan = EFalse;
	iIndexToFoldersToScan = 0;
	iAnchorFoldersToScan = iFoldersToScan->MdcaCount();
	
	// Set the flag to scanning files to initialize scanning. ScanDir() will immediately see
	// the flag as scanning files and will toggle it to scanning folders. Thus scanning folders
	// begins!
	iFindFileScanner->InitDateCriteria();
	iFindFileScanner->iCurrentOperation = CSMan2FindFileScanner::ScanningFiles;
	iFindFileScanner->ScanDir(ETrue);

	if (!iErrorInScan)
		SetToolbarButtonState(ETrue);
}

void CSMan2FindFileView::StopFileFind()
{
	if (iDirScanner)
	{
		delete iDirScanner;
		iDirScanner = NULL;
	}
	ScanningEnded();
}

void CSMan2FindFileView::ScanningEnded()
{	
	// No more entries to scan. Remove all the added folders
	if (iAnchorFoldersToScan > iFoldersToScan->MdcaCount())
		iFoldersToScan->Delete(iAnchorFoldersToScan, iFoldersToScan->MdcaCount() - iAnchorFoldersToScan);
	iFoldersToScan->Compress();
	iResultList->HandleItemAdditionL();
	
	CEikonEnv::Static()->BusyMsgCancel();
	CEikonEnv::Static()->BusyMsgL(R_TBUF_FINDFILE_SORTING, 0);
	((CDesCArray *)iResultList->Model()->ItemTextArray())->Sort();
	CEikonEnv::Static()->BusyMsgCancel();
	iResultList->DrawDeferred();
	
	if (iErrorInScan)
		CEikonEnv::Static()->InfoMsg(R_TBUF_FINDFILE_ERRORSINSEARCH);
	else
		CEikonEnv::Static()->InfoMsgWithAlignment(TGulAlignment(EHRightVCenter), R_TBUF_FINDFILE_DONE, iResultList->Model()->NumberOfItems());

	SetToolbarButtonState(EFalse);
}

void CSMan2FindFileView::SetCriteria(TFileName *aHighlightedFolder)
{
	CFindFileCriteria *iCriteriaDialog = new (ELeave) CFindFileCriteria(iFoldersToScan, &iModifiedDateOption, &iFromTime, &iToTime, &iSearchWildCard, aHighlightedFolder);
	iCriteriaDialog->ExecuteLD(R_DIALOG_FINDFILE_CRITERIA);
	SetToolbarButtonState(EFalse);
}