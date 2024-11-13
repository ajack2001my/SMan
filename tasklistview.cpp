#include "tasklistview.h"
#include "sman.h"
#include <gulicon.h>
#include <SMan.rsg>

/*************************************************************
*
* Task view object
*
**************************************************************/

CSMan2TaskListView::CSMan2TaskListView(CConfig *cData) : CViewBase(cData)
{
	iWindowUidList = new (ELeave) CArrayFixFlat<TUid>(1);
	iSnapshotTaskCaption = new (ELeave) CArrayFixSeg<TApaAppCaption>(1);
	iSnapshotTaskUid = new (ELeave) CArrayFixFlat<TUid>(1);
	iCapturedHotkeyShortHandle = 0;
	iCapturedHotkeyLongHandle = 0;
}

TBool CSMan2TaskListView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2TaskListView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2TaskListView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
	}
}

void CSMan2TaskListView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
	refreshTaskList(TODO_REFRESH);
}

TVwsViewId CSMan2TaskListView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidTaskView);
}

CSMan2TaskListView::~CSMan2TaskListView()
{
	releaseFont();
	if (taskRefresher->IsActive())
		taskRefresher->Cancel();
	delete taskRefresher;
	delete iSnapshotTaskCaption;
	delete iSnapshotTaskUid;
	delete iWindowUidList;
}

void CSMan2TaskListView::ToggleHotKey(void)
{
	if (!static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetHotkey))
		return;
	
	// The capturelongkey routine cannot be used in isolation. Otherwise, you will only see
	// long key presses. You must couple it with the capturekey routine. The way the 
	// capturelongkey works is as follows:
	//
	// aInputKeyCode - this is the key you want the window server to watch for
	// aOutputKeyCode - this is the key the window server will report to your app as being pressed
	// when the long key press happens
	//
	// The aOutputKeyCode is necessary because if you couple capturelongkey with capturekey,
	// you have no way of differentiating which event was fired. You must set aOutputKeyCode to
	// a different value from aInputKeyCode. Then, in your capturekey call, you specify 
	// the value of aInputKeyCode as well. This way, capturekey will be reported to your app
	// with aInputKeyCode as being pressed and capturelongkey will be reported to your app
	// with aOutputKeyCode as being pressed. Only then can you differentiate the two.
	//
	// The SDK doesn't document this clearly :(
	if (iCapturedHotkeyShortHandle != 0)
	{
		CEikonEnv::Static()->RootWin().CancelCaptureKey(iCapturedHotkeyShortHandle);
		CEikonEnv::Static()->RootWin().CancelCaptureLongKey(iCapturedHotkeyLongHandle);
	}
	iCapturedHotkeyShortHandle = 0;
	iCapturedHotkeyLongHandle = 0;	
	if (configData->hotkeyEnabled)
	{
		if ((configData->iHotkeyDialogResult != 0) && (configData->iHotkeyDialogResult != 3)) // 3 = flip open
		{
			iCapturedHotkeyShortHandle = CEikonEnv::Static()->RootWin().CaptureKey(sCaptureKeyCodes[configData->iHotkeyDialogResult], 0, 0, 2);
			iCapturedHotkeyLongHandle = CEikonEnv::Static()->RootWin().CaptureLongKey(sCaptureKeyCodes[configData->iHotkeyDialogResult], 
				CAPTURE_LONG_HOTKEY_REPLACEMENT, 0, 0, 2, ELongCaptureWaitShort);
			/*
			CapturedHotkeyHandle = CEikonEnv::Static()->RootWin().CaptureLongKey(sCaptureKeyCodes[configData->iHotkeyDialogResult], 
				sCaptureKeyCodes[configData->iHotkeyDialogResult], 0, 0, 0, ELongCaptureWaitShort);
			iCapturedHotkeyHandle = CEikonEnv::Static()->RootWin().CaptureKey(sCaptureKeyCodes[configData->iHotkeyDialogResult],0,0,2);
			*/
		}
	}
}

void CSMan2TaskListView::SetIgnoreHarakiriState()
{
	CEikonEnv::Static()->SetSystem(configData->persist);
/*
	TInt iWindowId = CEikonEnv::Static()->RootWin().Identifier();
	TApaTask iTApaTask(CEikonEnv::Static()->WsSession());
	iTApaTask.SetWgId(iWindowId);
	RThread iThread;
	if (iThread.Open(iTApaTask.ThreadId()) == KErrNone)
		iThread.SetProtected(configData->persist);
*/
}

int CSMan2TaskListView::loadSnapShot(void)
{
	RFile inStream;
	int retVal;
	
	retVal = 1;
	iSnapshotTaskCaption->Reset();
	iSnapshotTaskUid->Reset();
	if (inStream.Open(CEikonEnv::Static()->Static()->FsSession(), snapshotFileName, EFileRead) == KErrNone)
	{
		TInt fileSize;
		
		inStream.Size(fileSize);
		if (fileSize > 1)
		{
			TFileText snapshotFile;
			TBuf<128> iCaptionBuffer;
			TPckgBuf<TUid> iUidBuffer;
			TInt iError = KErrNone;
			
			snapshotFile.Set(inStream);
			retVal = 0;
			while ((iError == KErrNone) && (retVal == 0))
			{
				iError = snapshotFile.Read(iCaptionBuffer);
				if (iError == KErrNone)
				{
					iSnapshotTaskCaption->AppendL(iCaptionBuffer);
					TBuf<30> iDesBuffer;
					//iDesBuffer.Copy(iUidBuffer);
					iError = snapshotFile.Read(iDesBuffer);
					if (iError == KErrNone)
					{
						iUidBuffer.Copy(iDesBuffer);
						iSnapshotTaskUid->AppendL(iUidBuffer());
					}
					else
						retVal = 1;
				}
				else
					retVal = 1;
			}
			if ((retVal == 1) && (iError != KErrEof))
			{
				iSnapshotTaskCaption->Reset();
				iSnapshotTaskUid->Reset();
			}
			else
				retVal = 0;
		}
		inStream.Close();
	}
	return retVal;
}

int CSMan2TaskListView::saveSnapShot(void)
{
	RFile outStream;
	TInt iRetVal = KErrGeneral;
	
	if (outStream.Replace(CEikonEnv::Static()->Static()->FsSession(), snapshotFileName, EFileRead) == KErrNone)
	{
		TFileText snapshotFile;
		TPckgBuf<TUid> iUidBuffer;
		TBuf<30> iUidDes;
		
		iRetVal = KErrNone;
		snapshotFile.Set(outStream);
		for (int i = 0; (i < iSnapshotTaskCaption->Count()) && (iRetVal == KErrNone); i++)
		{
			iRetVal = snapshotFile.Write(iSnapshotTaskCaption->At(i));
			if (iRetVal == KErrNone)
			{
				iUidBuffer = iSnapshotTaskUid->At(i);
				iUidDes.Copy(iUidBuffer);
				iRetVal = snapshotFile.Write(iUidDes);
			}
		}
		iRetVal = outStream.Flush();
		outStream.Close();
	}
	return iRetVal;
}

TKeyResponse CSMan2TaskListView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyConfirm)
		{
			refreshTaskList(TODO_FOCUS);
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			cTaskListBox->View()->MoveCursorL(CListBoxView::ECursorNextItem, CListBoxView::ENoSelection);
			cTaskListBox->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			cTaskListBox->View()->MoveCursorL(CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection);
			cTaskListBox->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return cTaskListBox->OfferKeyEventL(aKeyEvent, aType);
}

void CSMan2TaskListView::InitToolBarButton(CEikBitmapButton **aButton, TPoint aPosition, TFileName *bitmapFile, TInt aBitmapId, TInt aBitmapMaskId)
{
	*aButton = new (ELeave) CEikBitmapButton();
	(*aButton)->SetPictureFromFileL(*bitmapFile, aBitmapId, aBitmapMaskId, CEikCommandButtonBase::EFirst);
	(*aButton)->SetContainerWindowL(*this);
	(*aButton)->SetObserver(this);
	(*aButton)->SetExtent(aPosition, TSize(42, 26));
	controlsArray->AppendL(*aButton);
}

void CSMan2TaskListView::ConstructL(const TRect& aRect)
{	
	oldItem = -1;
	
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());
	
	snapshotFileName.Copy(appPathNoExt);
	snapshotFileName.Append(_L("snp"));

	if (loadSnapShot() == 1)
		CEikonEnv::Static()->InfoMsgWithAlignment(TGulAlignment(EHRightVCenter), R_TBUF_NOPREVSNAPSHOT);
	else 
		CEikonEnv::Static()->InfoMsgWithAlignment(TGulAlignment(EHRightVCenter), R_TBUF_SNAPSHOTLOADED);

	// Initialize listbox
	cTaskListBox = new(ELeave) CEikColumnListBox;
	cTaskListBox->ConstructL(this, CEikColumnListBox::EMultipleSelection);
	CColumnListBoxData* listBoxColumns = ((CColumnListBoxItemDrawer*)cTaskListBox->ItemDrawer())->ColumnData();
	listBoxColumns->SetGraphicsColumnL(0, ETrue);
	listBoxColumns->SetColumnWidthPixelL(0, int(Size().iWidth * 0.10));
	listBoxColumns->SetColumnWidthPixelL(1, int(Size().iWidth * 0.45));
	listBoxColumns->SetColumnWidthPixelL(2, int(Size().iWidth * 0.32));
	listBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ECenter);
	listBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ELeft);
	listBoxColumns->SetColumnAlignmentL(2, CGraphicsContext::ERight);
	
	// This is destroyed on first call to refreshtasklist
	CArrayPtr<CGulIcon>* iconArray = new CArrayPtrFlat<CGulIcon>(1);
	listBoxColumns->SetIconArray(iconArray);
	
	cTaskListBox->CreateScrollBarFrameL();
	cTaskListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	cTaskListBox->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, int(aRect.Size().iHeight * 0.84)));
	controlsArray->AppendL(cTaskListBox);

	TFileName bitmapFile;
	TInt horizOffset, vertOffset;

	horizOffset = 0;
	vertOffset = 0;
	bitmapFile.Copy(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName());
	
	// Flush button
	InitToolBarButton(&flushButton, TPoint(0 + horizOffset, cTaskListBox->Position().iY + 
		cTaskListBox->Size().iHeight + vertOffset), &bitmapFile, EMbmMyheaderFlushout, EMbmMyheaderFlushoutmask);

	// End task button
	InitToolBarButton(&endTaskButton, TPoint(flushButton->Size().iWidth + horizOffset, cTaskListBox->Position().iY + 
		cTaskListBox->Size().iHeight + vertOffset), &bitmapFile, EMbmMyheaderEndtask, EMbmMyheaderEndtaskmask);

	// Refresh button
	InitToolBarButton(&refreshButton, TPoint(flushButton->Size().iWidth * 2 + horizOffset, cTaskListBox->Position().iY + 
		cTaskListBox->Size().iHeight + vertOffset), &bitmapFile, EMbmMyheaderRefresh, EMbmMyheaderRefreshmask);

	// Snapshot button
	InitToolBarButton(&snapshotButton, TPoint(flushButton->Size().iWidth * 3 + horizOffset, cTaskListBox->Position().iY + 
		cTaskListBox->Size().iHeight + vertOffset), &bitmapFile, EMbmMyheaderSnapshot, EMbmMyheaderSnapshotmask);

	// Compress heap button
	InitToolBarButton(&compHeapButton, TPoint(flushButton->Size().iWidth * 4 + horizOffset, cTaskListBox->Position().iY + 
		cTaskListBox->Size().iHeight + vertOffset), &bitmapFile, EMbmMyheaderCompress, EMbmMyheaderCompressmask);
	
	compHeapButton->SetSize(TSize(41, 26));
	
	// Initialize memory bar
	CEikProgressInfo::SInfo iProgressAttr;
	iProgressAttr.iTextType = EEikProgressTextPercentage;
	iProgressAttr.iSplitsInBlock = 0;
	iProgressAttr.iFinalValue = 100;
	CEikProgressInfo::SLayout iProgressLayout;
	iProgressLayout.iEmptyColor = KRgbBlue;
	iProgressLayout.iEmptyTextColor = KRgbCyan;
	iProgressLayout.iFillColor = KRgbCyan;
	iProgressLayout.iFillTextColor = KRgbBlue;
	iProgressLayout.iGapBetweenBlocks = 1;
	iProgressLayout.iFont = CEikonEnv::Static()->NormalFont();
	iMemoryBar = new (ELeave) CEikProgressInfo(iProgressAttr);
	iMemoryBar->SetLayout(iProgressLayout);	
	iMemoryBar->SetContainerWindowL(*this);
	iMemoryBar->SetBorder(TGulBorder::ENone);
	iMemoryBar->SetExtent(TPoint(aRect.Size().iWidth - (aRect.Size().iWidth / 2), compHeapButton->Position().iY + compHeapButton->Size().iHeight), 
		TSize(aRect.Size().iWidth / 2, 14));
	controlsArray->AppendL(iMemoryBar);

	// Bluejack icon.	
	viewId = CSMan2AppUi::EViewTask;
	iBJackIconPosition = TPoint(2, compHeapButton->Position().iY + compHeapButton->Size().iHeight + 6);
	InitBJackIcon();

	ToggleHotKey();
	taskRefresher = new (ELeave) CRefreshTaskList;

	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
		SetZoomLevel(configData->zoomMain);
	this->EnableDragEvents();
	
	MakeVisible(EFalse);
	ActivateL();
}

void CSMan2TaskListView::SetZoomLevel(TInt zoomLevel)
{
	releaseFont();
	generateZoomFont(zoomLevel);
	CTextListItemDrawer* itemDrawer = ((CTextListItemDrawer*)cTaskListBox->View()->ItemDrawer());
	itemDrawer->SetFont(font);
	cTaskListBox->SetItemHeightL(font->HeightInPixels() + cTaskListBox->VerticalInterItemGap());
	configData->zoomMain = zoomLevel;
}

void CSMan2TaskListView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	TBool consumed = EFalse;
	TInt theItem;
	TBool pointValid = cTaskListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, theItem);
	
	if (aPointerEvent.iType == TPointerEvent::EDrag)
	{
		if ((pointValid) && (oldItem != theItem))
		{
			consumed = ETrue;
			cTaskListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
			cTaskListBox->UpdateScrollBarsL();
			oldItem = theItem;
		}
	}
	else if (aPointerEvent.iType == TPointerEvent::EButton1Down)
	{
		if (pointValid)
		{
			oldItem = theItem;
			consumed = ETrue;
			if (aPointerEvent.iPosition.iX > cTaskListBox->View()->ItemDrawer()->MarkColumn())
			{
				cTaskListBox->ClearSelection();
				cTaskListBox->SetCurrentItemIndex(theItem);
				refreshTaskList(TODO_FOCUS);
			}
			else
			{
				cTaskListBox->View()->VerticalMoveToItemL(theItem, CListBoxView::EDisjointSelection);
				cTaskListBox->UpdateScrollBarsL();
			}
		}
	}
	else if (aPointerEvent.iType == TPointerEvent::EButton1Up)
	{
		if ((aPointerEvent.iPosition.iX >= iMemoryBar->Position().iX) && 
			(aPointerEvent.iPosition.iX <= (iMemoryBar->Position().iX + iMemoryBar->Size().iWidth)) &&
			(aPointerEvent.iPosition.iY >= iMemoryBar->Position().iY) && 
			(aPointerEvent.iPosition.iY <= (iMemoryBar->Position().iY + iMemoryBar->Size().iHeight)))
			{
				TInt iFreeRAM, iTotalRAM;
				HAL::Get(HALData::EMemoryRAMFree, iFreeRAM);
				HAL::Get(HALData::EMemoryRAM, iTotalRAM);
				TBuf<25> iMessage;
				iMessage.Zero();
				iMessage.AppendNum(iFreeRAM / 1024);
				iMessage.Append(_L("KB / "));
				iMessage.AppendNum(iTotalRAM / 1024);
				iMessage.Append(_L("KB"));
				CEikonEnv::Static()->InfoMsg(iMessage);
				consumed = ETrue;
			}
	}
	if (!consumed)
		CCoeControl::HandlePointerEventL(aPointerEvent);
}

void CSMan2TaskListView::refreshTaskList(TInt intTodo)
{
	RApaLsSession oApaLsSession;
	CApaWindowGroupName* oWindowGroupName = NULL;
	CArrayFixFlat<TInt>* iWindowGroupList = NULL;
	TInt iCurrentItemIndex = NULL;
	CDesCArray *iListBoxArray;
	CArrayFixSeg<TApaAppCaption>* iMarkedArrayText = NULL;
	CArrayFixFlat<TUid> *iMarkedWindowUidList = NULL;
	TPtrC iCurrentTaskCaption, iTempPtr;
	TApaAppCaption iHighlightedTask;
	TUid iHighlightedTaskUid;
	RThread iCurrentTaskThread;
	TBool iTaskRemoved;
	TThreadId iThreadId;
	TFullName iTempCaption;
	TUid iCurrentTaskUid;
   	TKeyArrayFix iKeyArray = TKeyArrayFix(0, ECmpNormal);
   	TKeyArrayFix iKeyArrayNumber = TKeyArrayFix(0, ECmpTInt32);
   	TInt iFoundIndex;
	
	iCurrentItemIndex = cTaskListBox->CurrentItemIndex();
	iListBoxArray = ((CDesCArray *) cTaskListBox->Model()->ItemTextArray());
	
	CArrayPtr<CGulIcon>* iconArray = ((CColumnListBoxItemDrawer*)cTaskListBox->ItemDrawer())->ColumnData()->IconArray();
	iconArray->ResetAndDestroy();
	delete iconArray;
	iconArray = new CArrayPtrFlat<CGulIcon>(3);
	((CColumnListBoxItemDrawer*)cTaskListBox->ItemDrawer())->ColumnData()->SetIconArray(iconArray);
	
	// Build the marked array. This is used for TODO_TERMINATE and TODO_FLUSH only
	if ((intTodo == TODO_TERMINATE) || (intTodo == TODO_FLUSH))
	{	
		iMarkedArrayText = new CArrayFixSeg<TApaAppCaption>(1);
		iMarkedWindowUidList = new CArrayFixFlat<TUid>(1);
		for (int i = 0; i < cTaskListBox->SelectionIndexes()->Count(); i++)
		{
			iTempPtr.Set(cTaskListBox->Model()->ItemText(cTaskListBox->SelectionIndexes()->At(i)));
			TextUtils::ColumnText(iCurrentTaskCaption, 1, &iTempPtr);
			iMarkedArrayText->AppendL(iCurrentTaskCaption);
			iMarkedWindowUidList->AppendL(iWindowUidList->At(cTaskListBox->SelectionIndexes()->At(i)));
		}
	}
	
	// Get the currently highlighted item. Used for focusing to a task	
	if (intTodo == TODO_FOCUS)
	{
		iTempPtr.Set(cTaskListBox->Model()->ItemText(iCurrentItemIndex));
		TextUtils::ColumnText(iCurrentTaskCaption, 1, &iTempPtr);
		iHighlightedTask.Copy(iCurrentTaskCaption);
		iHighlightedTaskUid = iWindowUidList->At(iCurrentItemIndex);
	}
	
	// Cycle through the list of tasks and rebuild the elements in the listbox	
	iWindowUidList->Reset();
	iListBoxArray->Reset();
	iListBoxArray->Compress();
	cTaskListBox->HandleItemRemovalL();
	iWindowGroupList = new CArrayFixFlat<TInt>(5);
	CEikonEnv::Static()->WsSession().WindowGroupList(iWindowGroupList);
	oWindowGroupName = CApaWindowGroupName::NewL(CEikonEnv::Static()->WsSession());
	oApaLsSession.Connect();	
	for (int posIndex = 0; posIndex < iWindowGroupList->Count(); posIndex++)
	{
		// Check whether this is a task we should display
		oWindowGroupName->ConstructFromWgIdL(iWindowGroupList->At(posIndex));
		// If we are doing a snapshot the behaviour is:
		// - if hidden tasks is turned off, assume all hidden tasks are checked by default
		// - if hidden tasks is turned on, explicitly follow what the user checked
		// else just follow the hidden task flag as normal
		iTempCaption.Copy(oWindowGroupName->Caption());
		iCurrentTaskUid = oWindowGroupName->AppUid();
		
		if ((iTempCaption.Length() == 0) &&  (!oWindowGroupName->IsSystem()))
		{
			TThreadId iThread;
			TFullName aResult;
			RThread theThread;
			
			if (CEikonEnv::Static()->WsSession().GetWindowGroupClientThreadId(iWindowGroupList->At(posIndex), iThread) == KErrNone)
			{
				TFindThread findThread;

				while (findThread.Next(aResult) == KErrNone)
				{
					theThread.Open(findThread);
					if (iThread == theThread.Id())
					{
						RProcess ownerProcess;
						theThread.Process(ownerProcess);
						if (ownerProcess.LoadedFromRam())
						{
							// Keep copying until you find a '[' or you reach the end of the string
							TInt x = aResult.Find(_L("["));
							if (x != KErrNotFound)
								iTempCaption.Copy(aResult.Left(x));
							else
								iTempCaption.Copy(aResult);
						}
						ownerProcess.Close();
						theThread.Close();
						break;
					}
					theThread.Close();
				}
			}	
		}		

		if ((!configData->showHiddenTasks) && (intTodo != TODO_SNAPSHOT))
		{
			if (((iTempCaption.Length() == 0) || (oWindowGroupName->IsSystem()) || (oWindowGroupName->Hidden())) && (oWindowGroupName->AppUid() != KUidSMan2App))
				continue;
		}
		else
		{
			//if (oWindowGroupName->Caption().Length() == 0)
			if (iTempCaption.Length() == 0)
				continue;
		}

		if (CEikonEnv::Static()->WsSession().GetWindowGroupClientThreadId(iWindowGroupList->At(posIndex), iThreadId) != KErrNone)
			continue;
			
    	iCurrentTaskThread.Open(iThreadId);
    	if (iCurrentTaskThread.Protected()) // && (oWindowGroupName->AppUid() != KUidSMan2App))
    	{
    		iCurrentTaskThread.Close();
    		continue;
    	}
    	iCurrentTaskThread.Close();

    	// Now we can do what we were called for
    	iTaskRemoved = EFalse;
		iCurrentTaskCaption.Set(iTempCaption);

		CApaMaskedBitmap *tempBitmap = CApaMaskedBitmap::NewLC();
		if (oApaLsSession.GetAppIcon(oWindowGroupName->AppUid(), TSize(20, 16), *tempBitmap) == KErrNone)
		{
			CApaMaskedBitmap *appBitmap = CApaMaskedBitmap::NewL(tempBitmap);
			iconArray->AppendL(CGulIcon::NewL(appBitmap, NULL));
		}
		else
		{
			CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
			User::LeaveIfError(bitmap->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderBlankappicon));
			iconArray->AppendL(CGulIcon::NewL(bitmap, NULL));  // empty bitmap
		}
		CleanupStack::PopAndDestroy(); // tempBitmap
    	
    	if (intTodo == TODO_FOCUS)
    	{
    		TBool iDoFocus = EFalse;
    		
    		if (iCurrentTaskUid != KNullUid)
    		{
    			if (iCurrentTaskUid == iHighlightedTaskUid)
	    			iDoFocus = ETrue;
	    	}
    		else if (iCurrentTaskCaption.Compare(iHighlightedTask) == 0)
    			iDoFocus = ETrue;
    			
    		if (iDoFocus)
    		{
	    		User::InfoPrint(iCurrentTaskCaption);
			//static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->SetAppPosition(EFalse);
			// Due to some strange java problem, this line of code creates an issue with SMan (and i think any
			// other app). When i set the ordinal position of the java window to 0, SMan still thinks it has focus.
			// Normally this isn't a problem but because of the way SMan reacts to the hotkey, it is. Sending SMan
			// to the background doesn't always work since the java window may already be in the foreground.
			// The solution lies in maintaining a TBool flag (appInForeground) that is modified in the HandleForegroundEventL() 
			// member. This flag is checked in the hotkey handler and the appropriate action is then taken.
			// ----------------------------------------------
			// This problem was first discovered with Malcolm Bryant's jCompile utility.
			CEikonEnv::Static()->WsSession().SetWindowGroupOrdinalPosition(iWindowGroupList->At(posIndex), 0);
			intTodo = TODO_NOTHING;
    		}
    	}
    	else if ((intTodo == TODO_TERMINATE) && (oWindowGroupName->AppUid() != KUidSMan2App) && (iCurrentTaskCaption.Compare(_L("EiksrvBackdrop")) != 0))
    	{
    		TBool iDoTerminate = EFalse;
    		
    		if (iCurrentTaskUid != KNullUid)
    		{
    			if (iMarkedWindowUidList->Find(iCurrentTaskUid, iKeyArrayNumber, iFoundIndex) == 0)
    				iDoTerminate = ETrue;
    		}
    		else if (iMarkedArrayText->Find(iCurrentTaskCaption, iKeyArray, iFoundIndex) == 0)
    			iDoTerminate = ETrue;
    			
    		if (iDoTerminate)
    		{
				if (!configData->forceEndTask)
					GentleEndTask(iCurrentTaskCaption, iCurrentTaskUid);
				else
				{
					ForceEndTask(iCurrentTaskCaption, iCurrentTaskUid);
					iTaskRemoved = ETrue;
					/*
					iCurrentTaskThread.Open(iThreadId);
					RProcess iOwnerProcess;
					iCurrentTaskThread.Process(iOwnerProcess);
					iCurrentTaskThread.Close();
					iOwnerProcess.Kill(0);
					iOwnerProcess.Close();
					iTaskRemoved = ETrue;
					iMarkedArrayText->Delete(iFoundIndex);
					*/
				}
    		}
    	}
    	else if (intTodo == TODO_SNAPSHOT)
    	{
			iSnapshotTaskCaption->AppendL(iCurrentTaskCaption);
			iSnapshotTaskUid->AppendL(iCurrentTaskUid);
			if (!configData->showHiddenTasks)
			{
				if ((oWindowGroupName->IsSystem() || oWindowGroupName->Hidden()) && (oWindowGroupName->AppUid() != KUidSMan2App))
					iTaskRemoved = ETrue;
			}
		}
		else if ((intTodo == TODO_FLUSH) && (oWindowGroupName->AppUid() != KUidSMan2App))
		{
			// Search for the task in the snapshot. If the task wasn't found, kill it
			TBool iDoFlush = EFalse;
			
			if (iCurrentTaskUid != KNullUid)
			{
				if (iSnapshotTaskUid->Find(iCurrentTaskUid, iKeyArrayNumber, iFoundIndex) != 0)
					iDoFlush = ETrue;
			}
			else if (iSnapshotTaskCaption->Find(iCurrentTaskCaption, iKeyArray, iFoundIndex) != 0)
				iDoFlush = ETrue;
				
			if (iDoFlush)
			{
				if (configData->iFlushExclude)
				{
					// Since flush exclude is switched on, look for the task in the markedarray
					if (iCurrentTaskUid != KNullUid)
					{
						if (iMarkedWindowUidList->Find(iCurrentTaskUid, iKeyArrayNumber, iFoundIndex) == 0)
							iDoFlush = EFalse;
					}
					else if (iMarkedArrayText->Find(iCurrentTaskCaption, iKeyArray, iFoundIndex) == 0)
		    			iDoFlush = EFalse;
				}
				
				if (iDoFlush)
				{
					if (!configData->forceEndTask)
						GentleEndTask(iCurrentTaskCaption, iCurrentTaskUid);
					else
					{
						ForceEndTask(iCurrentTaskCaption, iCurrentTaskUid);
						iTaskRemoved = ETrue;
					}
					/*
					{
						iCurrentTaskThread.Open(iThreadId);
						RProcess ownerProcess;
						iCurrentTaskThread.Process(ownerProcess);
						iCurrentTaskThread.Close();
						ownerProcess.Kill(0);
						ownerProcess.Close();
						iTaskRemoved = ETrue;
					}
					*/
				}
			}			
		}
			
		// Done! Now, if the task was still alive we calculate the amount of RAM it
		// is consuming.

		if (!iTaskRemoved)
		{
			// Weird problem here. I couldn't call GetMemoryInfo for an RProcess. Kept
			// getting KErrNotSupported errors. Calling the deprecated GetRamSizes gives
			// me a panic. Only GetRamSizes for an RThread works. So I have to walk 
			// through all the threads and see which process id it belongs to and add
			// up the RAM utilization based on the pid that I want.

			RProcess targetProcess, currentProcess;
			TFullName procName;
			RThread currentThread;
			TInt iRamSize = 0;
			TBuf<10> procSizeInText;
			TBuf<10> iconIndex;
			TApaAppCaption tempText;
			TInt a, b;
			TFindThread findThread;

			iCurrentTaskThread.Open(iThreadId);
			iCurrentTaskThread.Process(targetProcess);
			iRamSize = 0;
			while(findThread.Next(procName) == KErrNone)
			{
				currentThread.Open(findThread);
				currentThread.Process(currentProcess);
				if (currentProcess.Id() == targetProcess.Id())
				{
					currentThread.GetRamSizes(a, b);
					iRamSize = iRamSize + a + b;
				}
				currentProcess.Close();
				currentThread.Close();
			}
			targetProcess.Close();
			iCurrentTaskThread.Close();

			iRamSize = iRamSize / 1024;
			iconIndex.Num(iconArray->Count() - 1);
			tempText.Copy(iconIndex);
			tempText.Append(KColumnListSeparator);
			tempText.Append(iCurrentTaskCaption);
			tempText.Append(KColumnListSeparator);
			procSizeInText.Num(iRamSize);
			tempText.Append(procSizeInText);
			tempText.Append(_L("k"));
			iListBoxArray->AppendL(tempText);
			iWindowUidList->AppendL(iCurrentTaskUid);
		}
	}
	oApaLsSession.Close();
	delete iWindowGroupList;
	delete oWindowGroupName;
	delete iMarkedArrayText;
	delete iMarkedWindowUidList;
	
	// Update amount of free memory in system
	TInt iFreeRAM, iTotalRAM;
	HAL::Get(HALData::EMemoryRAMFree, iFreeRAM);
	HAL::Get(HALData::EMemoryRAM, iTotalRAM);
	iFreeRAM = TInt(TReal(iFreeRAM) * TReal(100) / TReal(iTotalRAM));
	iMemoryBar->SetAndDraw(iFreeRAM);

	// Do some sorting...
	iListBoxArray = ((CDesCArray *) cTaskListBox->Model()->ItemTextArray());
	// Move the highlight back to the item that it was originally at but only if this is
	// a sane value
	// Grab this pointer again because I'm not sure if HandleItemAdditionL() may have
	// moved the address of the array
	// Updated, 25th Feb 2004 @ 5:45AM. Learned from the file manager that I must use
	// >= operator instead of just > operator sman sometimes crashes with a listbox panic!
	if (iCurrentItemIndex >= (iListBoxArray->MdcaCount() - 1))
		iCurrentItemIndex = iListBoxArray->MdcaCount() - 1;
	// This may be -1 if there were no items in the list box (i.e. we just started up this
	// program)
	if (iCurrentItemIndex < 0)
		iCurrentItemIndex = 0;
	cTaskListBox->SetCurrentItemIndex(iCurrentItemIndex);
	cTaskListBox->HandleItemAdditionL();
	cTaskListBox->UpdateScrollBarsL();
	cTaskListBox->SetFocus(ETrue, ENoDrawNow);
	cTaskListBox->DrawDeferred();

	if (configData->forceEndTask == EFalse)
	{
		if ((intTodo == TODO_TERMINATE) || (intTodo == TODO_FLUSH))
		{
			if (taskRefresher->IsActive())
				taskRefresher->Cancel();
			taskRefresher->startRefresh(REFRESH_DELAY_IF_NOT_FORCE_ENDTASK);
		}
		else if (intTodo == TODO_REFRESH)
		{
			if (taskRefresher->IsActive())
				taskRefresher->Cancel();		
		}
	}
}

void CSMan2TaskListView::GentleEndTask(TPtrC iTaskCaption, TUid iTaskUid)
{
	TApaTaskList iTaskList(CEikonEnv::Static()->WsSession());

	if (iTaskUid != KNullUid)
	{
		TApaTask iOneTask = iTaskList.FindApp(iTaskUid);
		if (iOneTask.Exists())
			iOneTask.EndTask();
	}
	else
	{
		TApaTask iOneTask = iTaskList.FindApp(iTaskCaption);
		if (iOneTask.Exists())
			iOneTask.EndTask();
	}
}

void CSMan2TaskListView::ForceEndTask(TPtrC iTaskCaption, TUid iTaskUid)
{
	TApaTaskList iTaskList(CEikonEnv::Static()->WsSession());

	if (iTaskUid != KNullUid)
	{
		TApaTask iOneTask = iTaskList.FindApp(iTaskUid);
		if (iOneTask.Exists())
			iOneTask.KillTask();
	}
	else
	{
		TApaTask iOneTask = iTaskList.FindApp(iTaskCaption);
		if (iOneTask.Exists())
			iOneTask.KillTask();
	}
}

void CSMan2TaskListView::HandleControlEventL(CCoeControl* aControl, TCoeEvent /*aEventType*/)
{
	TInt iControl = Index(aControl);
	
	switch (iControl)
	{
		case 1 : CEikonEnv::Static()->EikAppUi()->HandleCommandL(cmdFlushOut); break;
		case 2 : CEikonEnv::Static()->EikAppUi()->HandleCommandL(cmdEndTask); break;
		case 3 : CEikonEnv::Static()->EikAppUi()->HandleCommandL(cmdRefresh); break;
		case 4 : CEikonEnv::Static()->EikAppUi()->HandleCommandL(cmdSnapshot); break;
		case 5 : CEikonEnv::Static()->EikAppUi()->HandleCommandL(cmdCompressHeap); break;
	}
}

/*
void CSMan2TaskListView::Draw(const TRect& aRect) const
{
	TFileName mbmName;
	
	mbmName.Copy(appPathNoExt);
	mbmName.Append(_L("MBM"));
	
	CWindowGc& gc = SystemGc();
	CFbsBitmap* iAppBitmap = new (ELeave) CFbsBitmap();
	//User::LeaveIfError(iAppBitmap->Load(mbmName, EMbmMyheaderFlushout));
	TInt err = iAppBitmap->Load(mbmName, EMbmMyheaderFlushout);
	gc.UseBrushPattern(iAppBitmap);
	gc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
	gc.DrawRect(aRect);
	delete  iAppBitmap;
}
*/