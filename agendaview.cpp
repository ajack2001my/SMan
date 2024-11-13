#include "sman.h"
#include "agendaview.h"
#include <SMan.rsg>
#include <eikhlbv.h>
#include <gulicon.h>
#include <eikon.mbg>
#include <agenda.mbg>

// For a list of agenda model panics, look at TAgmPanic in agmpanic.h

/*************************************************************
*
* View
*
**************************************************************/

CSMan2AgendaView::CSMan2AgendaView(CConfig *cData) : CViewBase(cData)
{
	reader = new (ELeave) CAgendaFileReader(configData);
}

TBool CSMan2AgendaView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2AgendaView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2AgendaView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
		reader->CloseAgendaFile();
	}
}

void CSMan2AgendaView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
}

TVwsViewId CSMan2AgendaView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidAgendaView);
}

CSMan2AgendaView::~CSMan2AgendaView()
{
	// Don't need to delete these as the hierarchical list box owns them
	//delete hierModel;
	//delete hierDrawer;
	// DEBUG
	releaseFont();
	delete reader;
	delete internalHierModel;
}

void CSMan2AgendaView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	// Initialize model
	hierModel = new (ELeave) CHierModel();
	hierModel->ConstructL();
	
	internalHierModel = new (ELeave) CHierModel();
	internalHierModel->ConstructL();
	
	hierModel->fullHierModel = internalHierModel;

	// Initialize item drawer
	CArrayPtrFlat<CGulIcon>* bitmaps = new (ELeave) CArrayPtrFlat<CGulIcon>(2);
	CleanupStack::PushL(bitmaps);
	// Level 0 and 1 icons
	bitmaps->AppendL(CEikonEnv::Static()->CreateIconL(_L("*"), EMbmMyheaderTodayviewclosed, EMbmMyheaderTodayviewclosedmask));
	bitmaps->AppendL(CEikonEnv::Static()->CreateIconL(_L("*"), EMbmMyheaderTodayviewopen, EMbmMyheaderTodayviewopenmask));
	//_LIT(KMBMPath, "Z:\\SYSTEM\\APPS\\AGENDA\\AGENDA.MBM");

	// Level 2 icon for outstanding items
	bitmaps->AppendL(CEikonEnv::Static()->CreateIconL(_L("*"), EMbmMyHeaderAgendanotdone, EMbmMyHeaderAgendanotdonemask));

	// Level 2 icon for completed items
	bitmaps->AppendL(CEikonEnv::Static()->CreateIconL(_L("*"), EMbmMyHeaderAgendadone, EMbmMyHeaderAgendadonemask));
	
	CleanupStack::Pop();
	hierDrawer = new (ELeave) CHierItemDrawer(hierModel, CEikonEnv::Static()->NormalFont(), bitmaps);
	hierDrawer->fullHierModel = internalHierModel;

	cHierListBox = new (ELeave) CEikHierarchicalListBox();
	cHierListBox->ConstructL(hierModel, hierDrawer, this, 0);
	cHierListBox->CreateScrollBarFrameL();
	cHierListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	cHierListBox->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, aRect.Size().iHeight - EQikToolbarHeight - 4));
	cHierListBox->SetFocus(ETrue, EDrawNow);
	controlsArray->AppendL(cHierListBox);

	viewId = CSMan2AppUi::EViewAgenda;
	iBJackIconPosition = TPoint(2, cHierListBox->Position().iY + cHierListBox->Size().iHeight);
	InitBJackIcon();
	
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
		SetZoomLevel(configData->zoomAgenda);
	
	MakeVisible(EFalse);
	ActivateL();
}

TBool CSMan2AgendaView::SetMaxTodoPriority()
{
	TUint iTempPriority;
	
	CAgendaMaxTodoPriority *iDialog = new (ELeave) CAgendaMaxTodoPriority();
	iTempPriority = configData->iTodoMaxPriority;
	iDialog->iMaxPriority = &iTempPriority;
	if (iDialog->ExecuteLD(R_DIALOG_AGENDA_MAXTODOPRIORITY) == EEikBidYes)
	{
		configData->iTodoMaxPriority = iTempPriority;
		return ETrue;
	}
	else
		return EFalse;
}

/*
void CSMan2AgendaView::DoAutoRefresh()
{
	TTime iCurrentTime;
	
	iCurrentTime.HomeTime();
	if (!IsSameDate(&iCurrentTime, &(reader->iDay)))
		ReadAgenda(EFalse);
}
*/

void CSMan2AgendaView::FixSynchTimeZone()
{
	EikFileUtils::DeleteFile(_L("C:\\SYSTEM\\DATA\\GMTOFFSET.INI"));
	EikFileUtils::DeleteFile(_L("C:\\SYSTEM\\DATA\\DAYLIGHT.INI"));
	EikFileUtils::DeleteFile(_L("C:\\SYSTEM\\APPS\\AGENDA\\AGENDA.INI"));
	CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
}

void CSMan2AgendaView::ReadAgenda(TBool dateIsSet)
{	
	if (reader->activeSchedulerStarted)
		return;

	TBuf<50> rootEntry;
	
	CEikonEnv::Static()->BusyMsgL(R_TBUF_AGENDA_BUSY, 0);	
	
	cHierListBox->SetCurrentItemIndex(0);
	internalHierModel->RemoveAllItems();

	TTime tTime;		
	if (!dateIsSet)
	{
		tTime.HomeTime();
		reader->iDay = TDateTime(tTime.DateTime().Year(), tTime.DateTime().Month(), tTime.DateTime().Day(), 0, 0, 0, 0);
	}
	else
		tTime = reader->iDay;
	
	tTime.FormatL(rootEntry, _L("%/0%1%/1%2%/2%3%/3"));
	
	tTime.HomeTime();
	if (IsSameDate(&tTime, &(reader->iDay)))
		rootEntry.Insert(0, _L("Today "));
	else
		rootEntry.Insert(0, _L("On "));

	//CHierListItem *listItem = new (ELeave) CHierListItem(0);
	CHListItem *listItem = new (ELeave) CHListItem(0);
	listItem->SetTextL(rootEntry);
	internalHierModel->AddItemL(listItem, -1, -1);

	listItem = new (ELeave) CHListItem(0);
	listItem->SetTextL(_L("Calendar"));
	internalHierModel->AddItemL(listItem, 0, KEikHierListInsertAsFirstSibling);
	
	listItem = new (ELeave) CHListItem(0);
	listItem->SetTextL(_L("Todo"));
	internalHierModel->AddItemL(listItem, 0, 1);

	/*
	Read calendar items. This class destroys itself when it's done.
	My approach to handling ExpandItemL is this:
	- create another model (data model) that will hold the COMPLETE expanded list of agenda entries 
	- based on the previous model, populate the display model as appropriate
	I also subclass CHierListItem by adding in 3 additional bits of information as explained below.
	
	TInt iFullHierListModelIndex 
	Is used in the display list to cross reference the item back to the complete list of agenda entries. By
	doing so, i can handle ExpandItemL properly since i can then obtain a list of child items for the correct
	node being expanded.
	
	TBool iComplete
	Indicates if the item was completed or not. This is done by checking against the current home time
	and also whether the item is crossed out
	
	TInt iDayListIndex
	This is cross referenced to iDayList. iDayList maintains an array of TAgnInstanceId. This is used when
	tapping on an individual item and we want to query the agenda to obtain the detailed information about
	that particular instance
	*/
	reader->ExecuteL(internalHierModel);
	
	// Populate the display model with the first item
	hierModel->RemoveAllItems();
	listItem = new (ELeave) CHListItem(0);
	listItem->SetTextL(internalHierModel->ItemText(0));
	listItem->iFullHierListModelIndex = 0;
	cHierListBox->HlModel()->AddItemL(listItem, -1, -1);

	CEikonEnv::Static()->BusyMsgCancel();

	cHierListBox->HlModel()->ExpandItemL(0);
	// If you think about it, u'll realize these need to be expanded in reverse order else i can't hard-code
	// the itemindex numbers :)
	if (configData->agendaAutoExpandTodo)
		cHierListBox->HlModel()->ExpandItemL(2);
	if (configData->agendaAutoExpandCalendar)
		cHierListBox->HlModel()->ExpandItemL(1);
	
	cHierListBox->HandleItemAdditionL();
}

TBool CSMan2AgendaView::IsSameDate(TTime *aDate1, TTime *aDate2)
{
	TInt retVal = EFalse;
	if ((aDate1->DateTime().Day() == aDate2->DateTime().Day()) &&
		(aDate1->DateTime().Month() == aDate2->DateTime().Month()) &&
		(aDate1->DateTime().Year() == aDate2->DateTime().Year()))
			retVal = ETrue;
	return retVal;
}

void CSMan2AgendaView::FetchEntry()
{
	CAgnEntry *theEntry = reader->GetOneEntry(cHierListBox->CurrentItemIndex());
	if (theEntry != NULL)
	{
		HBufC *noneText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_AGENDA_NONE);
		HBufC *notImplementedText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_AGENDA_NOTIMPLEMENTED);
		if (theEntry->Type() != CAgnEntry::ETodo)
		{
			CAgnEntry *calEntry = theEntry;
			CAgendaDetail *detailDialog = new (ELeave) CAgendaDetail;

			TBuf<40> startDTText;
			TTime startDT = calEntry->InstanceStartDate();
			startDT.FormatL(startDTText, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%+B  "));
			detailDialog->startDateTime = &startDTText;

			TBuf<40> endDTText;
			TTime endDT = calEntry->InstanceEndDate();
			if (!IsSameDate(&startDT, &endDT))
				endDT.FormatL(endDTText, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%+B  "));
			else
				endDT.FormatL(endDTText, _L("%J%:1%T%+B  "));
			detailDialog->endDateTime = &endDTText;
			
			TBuf<40> alarmDTText;
			alarmDTText.Copy(*noneText);
			if (calEntry->HasAlarm())
			{
				TTime alarmDT = calEntry->AlarmInstanceDateTime();
				if (!IsSameDate(&startDT, &alarmDT))
					alarmDT.FormatL(alarmDTText, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%+B  "));
				else
					alarmDT.FormatL(alarmDTText, _L("%J%:1%T%+B  "));
			}
			detailDialog->alarmDateTime = &alarmDTText;
		
			HBufC *locationText = HBufC::NewL(calEntry->Location().Length() + noneText->Length());
			if (calEntry->Location().Length() == 0)
				locationText->Des().Copy(*noneText);
			else
				locationText->Des().Copy(calEntry->Location());
			detailDialog->location = locationText;
			
			calEntry->LoadAllComponentsL(CEikonEnv::Static()->PictureFactory());
			calEntry->NotesTextL();
			detailDialog->notes.Set(calEntry->NotesTextL());

			detailDialog->ExecuteLD(R_DIALOG_AGENDA_CALENDAR_DETAILED);
			delete locationText;
		}
		else //if (theEntry->Type() == CAgnEntry::ETodo)
		{
			CAgnTodo* todoEntry = theEntry->CastToTodo();
			CTodoDetail *detailDialog = new (ELeave) CTodoDetail;
			TTime iCrossedOutDate = todoEntry->CrossedOutDate();
			TTime iOldCrossedOutDate = iCrossedOutDate;

			detailDialog->iCrossedOutDate = &iCrossedOutDate;
			
			TBuf<40> dueDTText;
			TTime dueDT = TTime(0);
			if (todoEntry->IsDated())
			{
				dueDT = todoEntry->DueDate();
				dueDT.FormatL(dueDTText, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%+B  "));
			}
			else
				dueDTText.Copy(*noneText);
			detailDialog->dueDateTime = &dueDTText;

			TBuf<40> alarmDTText;
			alarmDTText.Copy(*noneText);
			if (todoEntry->HasAlarm())
			{
				TTime alarmDT = todoEntry->AlarmInstanceDateTime();
				if (!IsSameDate(&dueDT, &alarmDT))
					alarmDT.FormatL(alarmDTText, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%+B  "));
				else
					alarmDT.FormatL(alarmDTText, _L("%J%:1%T%+B  "));

				//TTime alarmDT = todoEntry->AlarmInstanceDateTime();
				//alarmDT.FormatL(alarmDTText, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%+B  "));
			}
			detailDialog->alarmDateTime = &alarmDTText;
		
			TBuf<6> priorityText;
			if (todoEntry->Priority() != 0)
				priorityText.Format(_L("%d"), todoEntry->Priority());
			else
				priorityText.Copy(*noneText);
			detailDialog->priority = &priorityText;
			
			todoEntry->LoadAllComponentsL(CEikonEnv::Static()->PictureFactory());
			todoEntry->NotesTextL();
			detailDialog->notes.Set(todoEntry->NotesTextL());

			detailDialog->ExecuteLD(R_DIALOG_AGENDA_TODO_DETAILED);
			if (iCrossedOutDate != Time::NullTTime())
				todoEntry->CrossOut(iCrossedOutDate);
			else
				todoEntry->UnCrossOut();
				
			if (iCrossedOutDate != iOldCrossedOutDate)
			{
				reader->UpdateInstance(todoEntry);
				ReadAgenda(ETrue);
			}
		}
		delete noneText;
		delete notImplementedText;
		delete theEntry;
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_AGENDA_NOTFOUND_OR_ERROR);
}

void CSMan2AgendaView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	TInt theItem;
	TBool pointValid = cHierListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, theItem);
	
	if (!(reader->activeSchedulerStarted))
	{
		cHierListBox->HandlePointerEventL(aPointerEvent);
		if (aPointerEvent.iType == TPointerEvent::EButton1Up)
		{
			if (pointValid)
			{
				CHierListItem *itemToBeExpanded = cHierListBox->HlModel()->Item(cHierListBox->CurrentItemIndex());
				if (itemToBeExpanded->Level() == 2)
					FetchEntry();
			}
		}
	}
}

TKeyResponse CSMan2AgendaView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType == EEventKey)
	{
		TBool consumed = EFalse;
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			cHierListBox->View()->MoveCursorL(CListBoxView::ECursorNextItem, CListBoxView::ENoSelection);
			consumed = ETrue;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			cHierListBox->View()->MoveCursorL(CListBoxView::ECursorPreviousItem, CListBoxView::ENoSelection);
			consumed = ETrue;
		}
		else	if ((aKeyEvent.iCode == EQuartzKeyConfirm) && (!(reader->activeSchedulerStarted)))
		{
			CHierListItem *itemToBeExpanded = cHierListBox->HlModel()->Item(cHierListBox->CurrentItemIndex());
			if (itemToBeExpanded->Level() < 2)
			{
				if (!itemToBeExpanded->IsExpanded())
					cHierListBox->HlModel()->ExpandItemL(cHierListBox->CurrentItemIndex());
				else
					cHierListBox->HlModel()->CollapseItem(cHierListBox->CurrentItemIndex());
				// For some god forsaken reason, HandleItemRemovalL() doesn't work when I
				// collapse the node. Sheesh... :(
				cHierListBox->HandleItemAdditionL();
			}
			else
				FetchEntry();
			consumed = ETrue;
		}
		if (consumed)
		{
			cHierListBox->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return cHierListBox->OfferKeyEventL(aKeyEvent, aType);
}

void CSMan2AgendaView::SetDateFromCalendarAndDrawNow(const TTime& aDate)
{
	reader->iDay = TDateTime(aDate.DateTime().Year(), aDate.DateTime().Month(), aDate.DateTime().Day(), 0, 0, 0, 0);
}

void CSMan2AgendaView::GetMinimumAndMaximumAndInitialDatesForCalendarL(TTime& aMinimumDate, TTime& aMaximumDate, TTime& aInitialDate) const
{
	aMinimumDate = TTime(_L("19000101:000000.000000"));
	aMaximumDate = TTime(_L("20990101:000000.000000"));
	aInitialDate = reader->iDay;
}

void CSMan2AgendaView::ViewOtherDate()
{
	TTime now;
	now.HomeTime();

	CEikCalendar *calendar = new (ELeave) CEikCalendar;
	calendar->SetCalendarObserver(this);
	
	// There must be an easier way than making 3 function calls. :( Anyway, this is to ensure the hotkey
	// does not activate while the calendar is visible
	Window().SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->DrawableWindow()->SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
	CEikonEnv::Static()->AppUiFactory()->ToolBar()->DrawableWindow()->SetFaded(ETrue, RWindowTreeNode::EFadeIncludeChildren);
	
	calendar->ExecuteLD();
	Window().SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
	CEikonEnv::Static()->AppUiFactory()->MenuBar()->DrawableWindow()->SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
	CEikonEnv::Static()->AppUiFactory()->ToolBar()->DrawableWindow()->SetFaded(EFalse, RWindowTreeNode::EFadeIncludeChildren);
	ReadAgenda(ETrue);
}

void CSMan2AgendaView::SetZoomLevel(TInt zoomLevel)
{
	// DEBUG
	releaseFont();
	generateZoomFont(zoomLevel);
	((CTextListItemDrawer*)cHierListBox->View()->ItemDrawer())->SetFont(font);
	cHierListBox->SetItemHeightL(font->HeightInPixels() + cHierListBox->VerticalInterItemGap());
	configData->zoomAgenda = zoomLevel;
}

/*************************************************************
*
* Item drawer
*
**************************************************************/

CHierItemDrawer::CHierItemDrawer(CHierModel* aModel, const CFont* aFont, CArrayPtrFlat<CGulIcon>* aBitmaps) 
: CHierarchicalListItemDrawer(aModel, aFont, aBitmaps)
{
}

CGulIcon* CHierItemDrawer::ItemIcon(TInt aItemIndex) const
{
	if ((!iIconArray) || (aItemIndex <= -1) || (aItemIndex >= iModel->NumberOfItems()))
		return NULL;

	TInt bitmapIndex;

	if (iModel->Item(aItemIndex)->Level() == 2)
	{
		if (aItemIndex >= fullHierModel->NumberOfItems())
			bitmapIndex = 2;
		else
		{
			if (((CHListItem*)(fullHierModel->Item(aItemIndex)))->iComplete)
				bitmapIndex = 3;
			else
				bitmapIndex = 2;
		}
	}
	else
	{
		if (iModel->Item(aItemIndex)->IsExpanded())
			bitmapIndex = 1;
		else
			bitmapIndex = 0;
	}
	return (*iIconArray)[bitmapIndex];
}

/*************************************************************
*
* Listbox model
*
**************************************************************/

void CHierModel::ExpandItemL(TInt aItemIndex)
{
	CHListItem* itemToBeExpanded = (CHListItem*)Item(aItemIndex);
	if (itemToBeExpanded->Level() < 2)
	{
		if (itemToBeExpanded->IsExpanded())
			return;
		else
		{
			itemToBeExpanded->SetExpanded();

			// Build all child items for this item
			CArrayFix<TInt>* childItems;
			childItems = new (ELeave) CArrayFixSeg<TInt>(2);
			fullHierModel->GetChildrenIndexesL(itemToBeExpanded->iFullHierListModelIndex, childItems);
			CHListItem *listItem;
			for (int i = childItems->Count() - 1; i >= 0; i--)
			{
				listItem = new (ELeave) CHListItem(0);
				listItem->SetTextL(fullHierModel->ItemText(childItems->At(i)));
				listItem->iFullHierListModelIndex = childItems->At(i);
				AddItemL(listItem, aItemIndex, KEikHierListInsertAsFirstSibling);
			}
			delete childItems;
		}
	}
}

/*************************************************************
*
* Agenda file reader
*
**************************************************************/

CAgendaFileReader::CAgendaFileReader(CConfig* cData)
{
 	configData = cData;
	iAgnServ = RAgendaServ::NewL();
	User::LeaveIfError(iAgnServ->Connect());
	agnModel = CAgnModel::NewL(this);
	agnModel->SetServer(iAgnServ);
	iDay.HomeTime();
	iDayList = CAgnDayList<TAgnInstanceId>::NewL(iDay.DateTime());
	activeSchedulerStarted = EFalse;	
}

CAgendaFileReader::~CAgendaFileReader()
{
	CloseAgendaFile();
	// You must delete the model before closing the server.
	delete agnModel;
	iAgnServ->Close();
	delete iAgnServ;
	delete iDayList;
}

CAgnEntry* CAgendaFileReader::GetOneEntry(TInt aIndex)
{
	TBool fileOk = ETrue;
	
	if (!iAgnServ->FileLoaded())
	{
		if (!activeSchedulerStarted)
		{
			fileOk = OpenAgendaFile();
			if (fileOk)
			{
				activeSchedulerStarted = ETrue;
				CActiveScheduler::Start();
			}
		}
		else
			fileOk = EFalse;
	}
		
	if (fileOk)
	{
		CHListItem *listItem = (CHListItem *)hierListModel->Item(aIndex);
		CAgnEntry *retVal = agnModel->FetchInstanceL((*iDayList)[listItem->iDayListIndex]);
		return retVal;
	}
	else
		return NULL;
}

TBool CAgendaFileReader::OpenAgendaFile()
{
	CEikonEnv::Static()->BusyMsgL(R_TBUF_AGENDA_BUSY, 0);	

	TRAPD(error, agnModel->OpenL(_L("C:\\DOCUMENTS\\AGENDA\\AGENDA"), TTimeIntervalMinutes(0), 
		TTimeIntervalMinutes(0), TTimeIntervalMinutes(0), this, EFalse));
	
	if (error == KErrNone)
	{
		iAgnServ->WaitUntilLoaded();
		CEikonEnv::Static()->BusyMsgCancel();
		return ETrue;
	}
	else
	{
		CEikonEnv::Static()->BusyMsgCancel();
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_AGENDA_OPENERROR);
		CEikonEnv::Static()->InfoMsg(*dataBuffer);
		delete dataBuffer;
		CloseAgendaFile();
		return EFalse;
	}
}

void CAgendaFileReader::CloseAgendaFile()
{
	if (iAgnServ->FileLoaded())
		iAgnServ->CloseAgenda();
}

void CAgendaFileReader::UpdateInstance(CAgnEntry *aAgnEntry)
{
	TRAPD(iError, agnModel->UpdateInstanceL(aAgnEntry, ECurrentInstance));
	if (iError != KErrNone)
		CEikonEnv::Static()->InfoMsg(R_TBUF_AGENDA_ERRUPDATETODO);
}

void CAgendaFileReader::ReadDayListL()
{
	// fill the day list
	// filter allows particular types of instance to be excluded/included
	TAgnFilter filter;
	/*
	filter.SetIncludeEvents(ETrue);
	filter.SetIncludeCrossedOutOnly(EFalse);
	filter.SetIncludeAlarmedOnly(EFalse);
	filter.SetIncludeAnnivs(ETrue);
	filter.SetIncludeRptsNextInstanceOnly(EFalse);
	*/
	iDayList->Reset();
	iDayList->SetDay(iDay);
	agnModel->PopulateDayInstanceListL(iDayList, filter, iDay);
	// Count() returns the number of instances in the list
	iCount = iDayList->Count();
	if (iCount > 0)
	   	ReadEntriesL();
}

void CAgendaFileReader::ReadEntriesL()
{
	_LIT(KDfagndayFormat,"%-B%:0%J%:1%T%+B");

	TBuf<256> nodeText;

	todoRootOffset = 2;
	TTime currentTime;
	currentTime.HomeTime();
	/*
	TTime iTodayDate;
	iTodayDate = TDateTime(currentTime.DateTime().Year(), currentTime.DateTime().Month(),
		currentTime.DateTime().Day(), 0, 0, 0, 0);
	*/
	for (TInt ii = iCount; ii > 0; --ii)
	{
		// Get each instance from the list, one at a time in reverse order
		CAgnEntry* entry = agnModel->FetchInstanceL((*iDayList)[ii-1]);
		entry->RichTextL()->Extract(iText, 0, 50);
		//if (entry->Type() == CAgnEntry::EAppt)
		if (entry->Type() != CAgnEntry::ETodo)
		{
			//CAgnAppt* appt = entry->CastToAppt();

			CAgnEntry *appt = entry;
			TTime startTime = appt->InstanceStartDate();
			if (!(configData->agendaShowCompleteCalendar))
			{
				// Note: EEvent type entries will never be selected on a different date
				if (entry->Type() != CAgnEntry::EEvent)
				{
					if (appt->InstanceEndDate() < currentTime)
						continue;
				}
			}
			TBuf<30> timeString;
			startTime.FormatL(timeString, KDfagndayFormat);
			
			CHListItem *listItem;
			listItem = new (ELeave) CHListItem(0);
			if (entry->Type() != CAgnEntry::EEvent)
			{
				// Note: EEvent type entries will never be selected on a different date
				if (appt->InstanceEndDate() < currentTime)
					listItem->iComplete = ETrue;
				else
					listItem->iComplete = EFalse;
			}
			nodeText.Copy(timeString);
			nodeText.Append(_L(" "));
			nodeText.Append(iText);
			
			listItem->SetTextL(nodeText);
			listItem->iDayListIndex = ii - 1;
			hierListModel->AddItemL(listItem, 1, KEikHierListInsertAsFirstSibling);
			todoRootOffset++;
		}
		else if (entry->Type() == CAgnEntry::ETodo)
		{
			CAgnTodo* todo = entry->CastToTodo();

			if (!configData->agendaShowCompleteTodo)
			{
				if (todo->CrossedOutDate() != Time::NullTTime())
					continue;
			}
			
			if (todo->Priority() <= configData->iTodoMaxPriority)
			{
				CHListItem *listItem;
				listItem = new (ELeave) CHListItem(0);				
				if (todo->CrossedOutDate() != Time::NullTTime())
					listItem->iComplete = ETrue;
				else
					listItem->iComplete = EFalse;
				
				nodeText.Copy(_L("["));
				nodeText.AppendNum(todo->Priority(), 10);
				nodeText.Append(_L("] "));
				nodeText.Append(iText);

				listItem->SetTextL(nodeText);
				listItem->iDayListIndex = ii - 1;
				hierListModel->AddItemL(listItem, todoRootOffset, KEikHierListInsertAsFirstSibling);
			}
		}
		delete entry;					
	}
}

void CAgendaFileReader::ExecuteL(CHierarchicalListBoxModel *hlModel)
{
	hierListModel = hlModel;

	if (!activeSchedulerStarted)
	{
		if (iAgnServ->FileLoaded())
		{
			ReadDayListL();
		}
		else
		{
			if (OpenAgendaFile())
			{
				activeSchedulerStarted = ETrue;
				CActiveScheduler::Start();
				ReadDayListL();
			}
		}
	}
}

// Rightfully, these should be owned by a view.
void CAgendaFileReader::Completed(TInt aError) 
{ 
	if (aError != KErrNone)
	{
		TBuf<10> errNum;
		errNum.Format(_L("%d"), aError);
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_AGENDA_OPERR);
		dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + 11);
		dataBuffer->Des().Append(errNum);
		CEikonEnv::Static()->InfoMsg(*dataBuffer);
	}
}; 

void CAgendaFileReader::StateCallBack(CAgnEntryModel::TState aState) 
{	
	switch (aState)
	{
		case CAgnModel::ENoFile: 
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_AGENDA_NOFILE);
			if (activeSchedulerStarted)
			{
				CActiveScheduler::Stop();
				activeSchedulerStarted = EFalse;
			}
			break;
		}
		case CAgnModel::EBlocked: 
		{
			break;
		}
		case CAgnModel::EOk: 
		{
			if (activeSchedulerStarted)
			{
				CActiveScheduler::Stop();
				activeSchedulerStarted = EFalse;
			}
			break;
		}
		default: break;
	}
}

void CAgendaFileReader::Progress(TInt /*aPercentageCompleted*/)
{
}

CHListItem::CHListItem(TInt aFlags) : CHierListItem(aFlags)
{
}
