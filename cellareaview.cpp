#include "cellareaview.h"
#include "sman.h"
#include <SMan.rsg>

/*************************************************************
*
* Cell area view.
*
* CellId information, signal strength information etc is only queried
* ONCE during view activation. Subsequently, a change notification
* request is sent to the telephony server to give efficient interrupt
* driven monitoring. As it turns out, the telephony server can
* accept more than 1 change notification registration
*
* The only reason the schema is so denormalized is because Symbian
* doesn't support table joins :( :( :( :(
*
**************************************************************/

/* Schema of DB storage

Table: Cells_List
Cell_Id	Country_Code	Network_Id	Description	Group_Id	Group_Priority	Group_State
Tuint	String(4)		String(8)	String(50)	Tuint		TUint			Tinyint

Table: Groups_List		
Group_Id	Group_Name	State	Priority
Counter		String(50)	Tinyint	Counter

Table: Events_List	
Event_Id	Event_Name	Event_Type	Event_Data	Group_Id	State	Priority	Type
Counter		String(50)	Tuint		String(255)	TUint		Tinyint	Counter		TUint

Cells is linked to Groups via Group_Id
Events is linked to Groups via Group_Id
Priority = 0, disabled; >0 the order in which events are fired off for a group
State in Groups/Events = 1, object is enabled; 0 = disabled
Group_Priority in Cells_List = Priority in Groups_List
Type in Events_List = 0, enter group event, 1 = leave group event, 2 = enter+leave group event

A disabled group is not searched when trying to locate the group to which a cell belongs

*/

CSMan2CellAreaView::CSMan2CellAreaView(CConfig *cData) : CViewBase(cData)
{
	iOldGroupIdNotInitialized = ETrue;
	iRClassesInitialized = EFalse;
	iNetworkInfoRefresher = new (ELeave) CSMan2NetworkInfoRefresh(&iMobilePhone, this);
	iSignalStrengthRefresher = new (ELeave) CSMan2SignalStrengthRefresh(&iMobilePhone, this);
	iDBHelper = NULL;
	iIsRecording = EFalse;
	iCurrentAutoRecordingGroupId = 0;
	iCurrentAutoRecordingGroupPriority = 0;
	iCurrentCountryCode.SetLength(0);
	iCurrentNetworkId.SetLength(0);
	iProcessEvents = ETrue;
	iEventFireTimeHistoryCurrentIndex = 0;

	iShortNameText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_SHORTNAME);
	iLongNameText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_LONGNAME);
	iCountryCodeText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_COUNTRYCODE);
	iNetworkIdText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_NETWORKID);
	iCellText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_CELL);
	iCellIdText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_CELLID);
	iGroupText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_GROUP);
	iMultiGroupText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_MULTIGROUP);
	iSignalStrengthText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_SIGNALSTRENGTH);
	
	iEventDataBuffer = HBufC::NewL(256);
	iBuffer = HBufC::NewL(256);
	iBuffer2 = HBufC::NewL(256);
	iTempTime = HBufC::NewL(128);
	iTSYName = HBufC::NewL(KMaxFileName);
	
	iAudioPlayer = NULL;
	// Interface to our ECOM plugin for doing vibration
	TRAPD(iError, iSManECom = CSManEComInterfaceDefinition::NewL(_L8("DoVibrate")));
	if (iError)
		iSManECom = NULL;

	iEventFireTimeHistory = new (ELeave) CArrayFixFlat<SEventFireTimeHistory>(NUM_EVENT_FIRE_TIME_HISTORY);
	
	// Read iEventsTriggerDesc from the resource file
	iEventTriggerIds = new (ELeave) CArrayFixFlat<TUint>(3);	
	CDesCArrayFlat *iTempEventArray = CEikonEnv::Static()->ReadDesCArrayResourceL(R_EVENT_TYPE_TRIGGER);
	TPtrC iNumberColumn;
	TBuf<10> iNumberBuffer;
	TBuf<50> iTextBuffer;
	TLex iLexConverter;
	TInt iTempEventTypeNumber;
	for (int i = 0; i < iTempEventArray->MdcaCount(); i++)
	{
		iTextBuffer.Copy(iTempEventArray->MdcaPoint(i));
		TextUtils::ColumnText(iNumberColumn, 1, &iTextBuffer, TChar(';'));
		
		iNumberBuffer.Copy(iNumberColumn);
		iLexConverter.Assign(iNumberBuffer);
		iLexConverter.Val(iTempEventTypeNumber);
		
		// At this point, iTempEventTypeNumber is ready
		iEventTriggerIds->AppendL(iTempEventTypeNumber);
	}
	delete iTempEventArray;
}

CSMan2CellAreaView::~CSMan2CellAreaView()
{
	StopAsynchRequests();
	DeInitRClasses();
	
	delete iNetworkInfoRefresher;
	delete iSignalStrengthRefresher;
	
	delete iShortNameText;
	delete iLongNameText;
	delete iCountryCodeText;
	delete iNetworkIdText;
	delete iCellText;
	delete iCellIdText;
	delete iGroupText;
	delete iMultiGroupText;
	delete iSignalStrengthText;
	delete iEventDataBuffer;
	delete iBuffer;
	delete iBuffer2;
	delete iTempTime;
	delete iEventTriggerIds;
	delete iTSYName;
	delete iEventFireTimeHistory;
	
	if (iAudioPlayer)
		delete iAudioPlayer;
	if (iSManECom)
	{
		iSManECom->DoVibrateOff();
		delete iSManECom;
	}
}

TBool CSMan2CellAreaView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2CellAreaView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2CellAreaView::DeInitDBHelper()
{
	if (iDBHelper)
	{
		iDBHelper->DeInitDB();
		delete iDBHelper;
		iDBHelper = NULL;
	}
}

TBool CSMan2CellAreaView::IsRecording()
{
	return iIsRecording;
}

void CSMan2CellAreaView::ToggleRecording()
{
	iIsRecording = !iIsRecording;
	if (iIsRecording)
	{
		// This lines will do nothing if the dbhelper instance is already created
		InitDBHelper();
		
		if (!iDBHelper->InitDB())
		{
			// Failed to initialize. Issue error to user and switch off auto recording
			iIsRecording = EFalse;
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRINITSTORE);
		}
		else
		{
			// create a new group using this name ddmonyyyy_hhmmss
			// extract the group id
			TBool iAutoGroupInitOk = EFalse;
			TBuf<50> iGroupName;
			TTime iTime;
			typedef TBuf<10> TMonthType;
			TMonthType iMonth[12] = {	_L("Jan"), _L("Feb"), _L("Mar"), _L("Apr"), _L("May"), _L("Jun"), 
										_L("Jul"), _L("Aug"), _L("Sep"), _L("Oct"), _L("Nov"), _L("Dec") };							
			iTime.HomeTime();
			iGroupName.Format(_L("%02u%04u_%02u%02u%02u"), iTime.DateTime().Day() + 1,
							iTime.DateTime().Year(), iTime.DateTime().Hour(),
							iTime.DateTime().Minute(), iTime.DateTime().Second());
			iGroupName.Insert(2, iMonth[iTime.DateTime().Month()]);
			iDBHelper->iSQLStatement.Copy(_L("INSERT INTO Groups_List (Group_Name, State) VALUES ('"));
			iDBHelper->iSQLStatement.Append(iGroupName);
			// Set state to disabled by default
			iDBHelper->iSQLStatement.Append(_L("', 1)"));
			if (iDBHelper->ExecuteSQL() >= 0)
			{
				iDBHelper->iSQLStatement.Copy(_L("SELECT Group_Id, Priority FROM Groups_List ORDER BY Priority DESC"));
				if (iDBHelper->ReadDB())
				{
					iDBHelper->iDBView.FirstL();
					if (iDBHelper->iDBView.AtRow())
					{
						iDBHelper->iDBView.GetL();
						iCurrentAutoRecordingGroupId = iDBHelper->iDBView.ColUint32(1);
						iCurrentAutoRecordingGroupPriority = iDBHelper->iDBView.ColUint32(2);
						iAutoGroupInitOk = ETrue;
						CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_AUTORECORDOK);
					}
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRREADGROUPS);
					iDBHelper->iDBView.Close();
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRREADGROUPS);
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRCREATEGROUP);
				
			if (!iAutoGroupInitOk)
			{
				iCurrentAutoRecordingGroupId = 0;
				iCurrentAutoRecordingGroupPriority = 0;
				iIsRecording = EFalse;	
			}
		}
	}
	else
	{
		iIsRecording = EFalse;
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_AUTORECORDOFF);
	}
	
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->UpdateSystemIcons(CSMan2AppUi::EIconNetworkInfoRecording, iIsRecording);
}

TBool CSMan2CellAreaView::IsStorageOK()
{
	return iDBHelper->iDBIsOpened;
}

TBool CSMan2CellAreaView::ReInitStorage()
{
	TBool iResult = EFalse;
	TInt iDBResult = KErrGeneral;

	InitDBHelper();
	iDBHelper->DeleteDB();
	if (iDBHelper->CreateDB())
	{
		iDBHelper->iSQLStatement.Copy(_L("CREATE TABLE Cells_List (Cell_Id UNSIGNED INTEGER, Country_Code VARCHAR(4), Network_Id VARCHAR(8), Description VARCHAR(50), Group_Id UNSIGNED INTEGER, Group_Priority UNSIGNED INTEGER, Group_State TINYINT)"));
		if (iDBHelper->ExecuteSQL() >= 0)
		{
			iDBHelper->iSQLStatement.Copy(_L("CREATE UNIQUE INDEX Index_Cell_Id_Group_Id ON Cells_List (Cell_Id, Group_Id)"));
			if (iDBHelper->ExecuteSQL() >= 0)
			{
				iDBHelper->iSQLStatement.Copy(_L("CREATE INDEX Index_Group_Id_Group_State ON Cells_List (Group_Id, Group_State)"));
				if (iDBHelper->ExecuteSQL() >= 0)
				{
					iDBHelper->iSQLStatement.Copy(_L("CREATE TABLE Groups_List (Group_Id COUNTER, Group_Name VARCHAR(50), State TINYINT, Priority COUNTER)"));
					if (iDBHelper->ExecuteSQL() >= 0)
					{
						iDBHelper->iSQLStatement.Copy(_L("CREATE UNIQUE INDEX Index_Group_Id ON Groups_List (Group_Id)"));
						if (iDBHelper->ExecuteSQL() >= 0)
						{
							iDBHelper->iSQLStatement.Copy(_L("CREATE INDEX Index_Group_Id_State ON Groups_List (Group_Id, State)"));
							if (iDBHelper->ExecuteSQL() >= 0)
							{
								iDBHelper->iSQLStatement.Copy(_L("CREATE TABLE Events_List (Event_Id COUNTER, Event_Name VARCHAR(50), Event_Type UNSIGNED INTEGER, Event_Data VARCHAR(255), Group_Id UNSIGNED INTEGER, Priority COUNTER, State TINYINT, Type UNSIGNED INTEGER)"));
								if (iDBHelper->ExecuteSQL() >= 0)
								{
									iDBHelper->iSQLStatement.Copy(_L("CREATE UNIQUE INDEX Index_Event_Id ON Events_List (Event_Id)"));
									if (iDBHelper->ExecuteSQL() >= 0)
									{
										iDBHelper->iSQLStatement.Copy(_L("CREATE INDEX Index_Group_Id_State_Type ON Events_List (Group_Id, State, Type)"));
										iDBResult = iDBHelper->ExecuteSQL();
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (iDBResult < 0)
	{
		iDBHelper->DeInitDB();
		iDBHelper->DeleteDB();
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRINITSTORE);
	}
	else
	{
		iResult = ETrue;
		CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
	}
	return iResult;
}

void CSMan2CellAreaView::DoCompress()
{
	InitDBHelper();
	if (!iDBHelper->CompactDB())
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRDBCOMPRESS);
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
}

void CSMan2CellAreaView::DoUpdateStats()
{
	InitDBHelper();
	if (!iDBHelper->UpdateDBStats())
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRDBUPDATESTATS);
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
}

void CSMan2CellAreaView::InitDBHelper()
{
	if (!iDBHelper)
	{
		iDBHelper = new (ELeave) CDBHelper();
#ifdef __WINS__
		iDBHelper->iDBFileName.Copy(_L("D:\\SMAN.NDB"));
#else	
		iDBHelper->iDBFileName.Copy(appPathNoExt);
		iDBHelper->iDBFileName.Append(_L("ndb"));
#endif
		if (!iDBHelper->RecoverDB())
			CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERROPENDB);
	}
}

void CSMan2CellAreaView::DoEdit()
{
	InitDBHelper();
	CEditCellDBDialog *iEditDialog = new (ELeave) CEditCellDBDialog(iDBHelper);
	iEditDialog->iDefaultNetworkIdForNewCells = &iCurrentNetworkId;
	iEditDialog->iDefaultCountryCodeForNewCells = &iCurrentCountryCode;
	iEditDialog->ExecuteLD(R_EDIT_CELLS_DIALOG);
}

void CSMan2CellAreaView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		if (!(configData->iCellAreaAlwaysRun))
			StopAsynchRequests();
		if (!IsRecording())
			DeInitDBHelper();
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
	}
}

void CSMan2CellAreaView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
	StartAsynchRequests();
	
	InitDBHelper();
	iDBHelper->InitDB();
	
	iNetworkInfoLabel->MakeVisible(EFalse);
	iNetworkInfoLabel->MakeVisible(ETrue);
	iSignalStrengthLabel->MakeVisible(EFalse);
	iSignalStrengthLabel->MakeVisible(ETrue);
}

TVwsViewId CSMan2CellAreaView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidCellAreaView);
}

TKeyResponse CSMan2CellAreaView::OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
{
	return EKeyWasNotConsumed;
}

void CSMan2CellAreaView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	CCoeControl::HandlePointerEventL(aPointerEvent);
}

void CSMan2CellAreaView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	// Initialize cell info labels
	
	TBuf<1> temp;
	temp.Copy(_L(""));
	iNetworkInfoLabel = new (ELeave) CEikLabel;
	iNetworkInfoLabel->SetTextL(temp);
	iNetworkInfoLabel->SetContainerWindowL(*this);
	iNetworkInfoLabel->SetExtent(TPoint(0, 50), TSize(aRect.Size().iWidth, aRect.Size().iHeight - 50));
	iNetworkInfoLabel->SetAlignment(EHCenterVCenter);
	controlsArray->AppendL(iNetworkInfoLabel);
	
	iSignalStrengthLabel = new (ELeave) CEikLabel;
	iSignalStrengthLabel->SetTextL(temp);
	iSignalStrengthLabel->SetContainerWindowL(*this);
	iSignalStrengthLabel->SetExtent(TPoint(0, 25), TSize(aRect.Size().iWidth, 25));
	iSignalStrengthLabel->SetAlignment(EHCenterVCenter);
	controlsArray->AppendL(iSignalStrengthLabel);

	viewId = CSMan2AppUi::EViewCellArea;
	iBJackIconPosition = TPoint(2, Size().iHeight - 10);
	InitBJackIcon();
	
	MakeVisible(EFalse);
	ActivateL();
	
	AlwaysRunFlagChanged(EFalse);
}

void CSMan2CellAreaView::AlwaysRunFlagChanged(TBool aViewIsInFocus)
{
	if (configData->iCellAreaAlwaysRun)
		StartAsynchRequests();
	else
	{
		// If this view is currently in focus, you should not stop the request!!
		// The request will eventually be stopped in ViewDeactivated()
		if (!aViewIsInFocus)
			StopAsynchRequests();
	}
}

void CSMan2CellAreaView::InitRClasses()
{
	if (!iRClassesInitialized)
	{
		CCommsDatabase* const iCommsDB = CCommsDatabase::NewL(EDatabaseTypeUnspecified);
		CleanupStack::PushL(iCommsDB); // PUSH
		TUint32 iModemId = 0;
		iCommsDB->GetGlobalSettingL(TPtrC(MODEM_PHONE_SERVICES_SMS), iModemId);

		CCommsDbTableView* const iCommsDBView = iCommsDB->OpenViewMatchingUintLC(TPtrC(MODEM), TPtrC(COMMDB_ID), iModemId); // PUSH
		TInt iErr = iCommsDBView->GotoFirstRecord();
		User::LeaveIfError(iErr);
		TPtr iPtr = iTSYName->Des();
		TDes *iDes = &iPtr;
		iCommsDBView->ReadTextL(TPtrC(MODEM_TSY_NAME), *iDes);
		CleanupStack::PopAndDestroy(2, iCommsDB); // view & db 	
	
		User::LeaveIfError(iTelServer.Connect());
		if (iTelServer.GetTsyName(0, *iDes) == KErrNone)
		{
			User::LeaveIfError(iTelServer.LoadPhoneModule(*iDes));
			User::LeaveIfError(iTelServer.GetPhoneInfo(0, iPhoneInfo));
			User::LeaveIfError(iMobilePhone.Open(iTelServer, iPhoneInfo.iName));
			iRClassesInitialized = ETrue;
		}
	}
}

void CSMan2CellAreaView::DeInitRClasses()
{
	if (iRClassesInitialized)
	{
		iRClassesInitialized = EFalse;		
		iMobilePhone.Close();
		TPtr iPtr = iTSYName->Des();
		TDes *iDes = &iPtr;
		iTelServer.UnloadPhoneModule(*iDes);
		iTelServer.Close();
	}
}

void CSMan2CellAreaView::StopAsynchRequests()
{
	iNetworkInfoRefresher->Cancel();
	iSignalStrengthRefresher->Cancel();
	//DeInitRClasses();
}

void CSMan2CellAreaView::StartAsynchRequests()
{
	InitRClasses();
	if (iRClassesInitialized)
	{
		iNetworkInfoRefresher->ActivateNotification();
		iSignalStrengthRefresher->ActivateNotification();
	}
}

void CSMan2CellAreaView::DoEventPlayAudio(HBufC *iEventData)
{
	if (iAudioPlayer)
	{
		delete iAudioPlayer;
		iAudioPlayer = NULL;
	}
	TPtrC iPtr = TPtrC(iEventData->Des());
	TRAPD(iErr, iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(iPtr, *this));
	if (iErr)
		MapcPlayComplete(iErr);
}

void CSMan2CellAreaView::DoEventVibrate()
{
	if (iSManECom)
	{
		iSManECom->DoVibrateOff();
		iSManECom->DoVibrateOn(1, 10, 4);
	}
}

void CSMan2CellAreaView::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
	if (aError)
		MapcPlayComplete(aError);
	else
		iAudioPlayer->Play();
}

void CSMan2CellAreaView::MapcPlayComplete(TInt /*aError*/)
{
	if (iAudioPlayer)
	{
		delete iAudioPlayer;
		iAudioPlayer = NULL;
	}
}

void CSMan2CellAreaView::DoEvents(TUint aGroupId, TUint aEventFiringType)
{
	// Process all events in iGroupId
	iDBHelper->iSQLStatement.Format(_L("SELECT Event_Type, Event_Data, Event_Id FROM Events_List WHERE Group_Id = %u AND State = 1 AND (Type = %u OR Type = 2) ORDER BY Priority"), aGroupId, aEventFiringType);
	if (iDBHelper->ReadDB())
	{
		TUint iEventType = 0;
		TUint32 iEventId = 0;
		TTime iTargetTime;
		TTime iCurrentTime;
		TBool iEventFound;
		TBool iEventShouldFire;
		
		iCurrentTime.HomeTime();
		iTargetTime = iCurrentTime;
		iTargetTime -= TTimeIntervalMinutes(configData->iCellEventFiringDelayInMinutes);
		
		while (iDBHelper->iDBView.NextL())
		{
			iDBHelper->iDBView.GetL();
			
			// Check if this event should fire, but only if the user didn't switch off this
			// feature i.e. configData->iCellEventFiringDelayInMinutes > 0
			iEventShouldFire = ETrue;
			if (configData->iCellEventFiringDelayInMinutes > 0)
			{
				iEventId = iDBHelper->iDBView.ColUint32(3);
				iEventFound = EFalse;
				for (int i = 0; i < iEventFireTimeHistory->Count(); i++)
				{
					if (iEventFireTimeHistory->At(i).iEventId == iEventId)
					{
						iEventFound = ETrue;
						if (iEventFireTimeHistory->At(i).iFireTime >= iTargetTime)
						{
							iEventShouldFire = EFalse;
							iEventFireTimeHistory->At(i).iFireTime = iCurrentTime;
						}
						break;
					}
				}
				if (!iEventFound)
				{
					if (iEventFireTimeHistory->Count() < NUM_EVENT_FIRE_TIME_HISTORY)
					{
						SEventFireTimeHistory iEventHistory;
						
						iEventHistory.iEventId = iEventId;
						iEventHistory.iFireTime = iCurrentTime;
						iEventFireTimeHistory->AppendL(iEventHistory);
					}
					else
					{
						iEventFireTimeHistory->At(iEventFireTimeHistoryCurrentIndex).iEventId = iEventId;
						iEventFireTimeHistory->At(iEventFireTimeHistoryCurrentIndex).iFireTime = iCurrentTime;
						iEventFireTimeHistoryCurrentIndex++;
						if (iEventFireTimeHistoryCurrentIndex >= NUM_EVENT_FIRE_TIME_HISTORY)
							iEventFireTimeHistoryCurrentIndex = 0;
					}
				}
			}
			
			if (iEventShouldFire)
			{
				iEventType = iDBHelper->iDBView.ColUint(1);
				iEventDataBuffer->Des().Copy(iDBHelper->iDBView.ColDes(2));
				switch (iEventType)
				{
					// Play audio
					case 0 :
						{
							DoEventPlayAudio(iEventDataBuffer);
							break;
						};
					// Vibrate
					case 1 :
						{
							DoEventVibrate();
							break;
						};
				};
			}
		}
		iDBHelper->iDBView.Close();
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRREADDB);
}

void CSMan2CellAreaView::UpdateNetworkInfo(CSMan2NetworkInfoRefresh::sCellInfo* aCellId, TDesC *aShortName, TDesC *aLongName)
{
	iCurrentCountryCode.Copy(aCellId[2].iCountryCode);
	iCurrentNetworkId.Copy(aCellId[2].iNetworkId);

	if (!iDBHelper)
	{
		InitDBHelper();
		iDBHelper->InitDB();
	}

	// Autorecording is on?
	if (iIsRecording)
	{
		if (iDBHelper)
		{
			if (aCellId[2].iCellId != 0)
			{
				static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->UpdateSystemIcons(CSMan2AppUi::EIconNetworkInfoRecording, ETrue);
				
				TBuf<8> iCountryCode;
				TBuf<16> iNetworkId;
				
				iDBHelper->iSQLStatement.Copy(_L("INSERT INTO Cells_List VALUES ("));
				iDBHelper->iSQLStatement.AppendNum(aCellId[2].iCellId);
				iDBHelper->iSQLStatement.Append(_L(",'"));
				iCountryCode.Copy(aCellId[2].iCountryCode);
				iDBHelper->ConvertTextStatement(&iCountryCode, _L("'"), _L("''"));
				iDBHelper->iSQLStatement.Append(iCountryCode);
				iDBHelper->iSQLStatement.Append(_L("','"));
				iNetworkId.Copy(aCellId[2].iNetworkId);
				iDBHelper->ConvertTextStatement(&iNetworkId, _L("'"), _L("''"));
				iDBHelper->iSQLStatement.Append(iNetworkId);
				iDBHelper->iSQLStatement.Append(_L("','"));
				
				// By default, description is date+time stamp. We recycle the iEventDataBuffer
				// variable so as not to waste space
				TTime iTime;
				iTime.HomeTime();
				TPtr iPtr = TPtr(iEventDataBuffer->Des());
				TDes *iTempDes = &iPtr;
				iTime.FormatL(*iTempDes, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B"));
				iDBHelper->iSQLStatement.Append(*iEventDataBuffer);
				iDBHelper->iSQLStatement.Append(_L("',"));
				
				iDBHelper->iSQLStatement.AppendNum(iCurrentAutoRecordingGroupId);
				iDBHelper->iSQLStatement.Append(_L(","));
				iDBHelper->iSQLStatement.AppendNum(iCurrentAutoRecordingGroupPriority);
				iDBHelper->iSQLStatement.Append(_L(","));
				iDBHelper->iSQLStatement.AppendNum(1);
				iDBHelper->iSQLStatement.Append(_L(")"));
				
				// We terminate if there was any error other than a index key violation
				// Key violation is ok because we may end up being at the same cell within a group
				// We just ignore this cell but we shouldn't terminate auto-recording
				TInt iDBResult = iDBHelper->ExecuteSQL();
				if ((iDBResult < 0) && (iDBResult != KErrAlreadyExists))
				{
					iCurrentAutoRecordingGroupId = 0;
					iIsRecording = 0;
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRAUTOSAVE);
				}
			}
		}
	}
	
	// Find the cell's group name. Since a cell can belong to more than one group, only the
	// first group (ordered by priority then by group_name) is displayed
	TBool iGroupNameFound = EFalse;
	TBool iCellExistsInMultipleGroups = EFalse;
	TUint iGroupId = 0;
	if (aCellId[2].iCellId != 0)
	{
		TBuf<16> iTempBuffer;
		iDBHelper->iSQLStatement.Format(_L("SELECT Group_Id, Description FROM Cells_List WHERE Group_State = 1 AND Cell_Id=%u AND "), aCellId[2].iCellId);
		iDBHelper->iSQLStatement.Append(_L("Country_Code='"));
		iTempBuffer.Copy(aCellId[2].iCountryCode);
		iDBHelper->ConvertTextStatement(&iTempBuffer, _L("'"), _L("''"));
		iDBHelper->iSQLStatement.Append(iTempBuffer);
		iDBHelper->iSQLStatement.Append(_L("' AND Network_Id='"));
		iTempBuffer.Copy(aCellId[2].iNetworkId);
		iDBHelper->ConvertTextStatement(&iTempBuffer, _L("'"), _L("''"));
		iDBHelper->iSQLStatement.Append(iTempBuffer);
		iDBHelper->iSQLStatement.Append(_L("' ORDER BY Group_Priority"));
		if (iDBHelper->ReadDB())
		{
			if (iDBHelper->iDBView.CountL() > 0)
			{
				if (iDBHelper->iDBView.CountL() > 1)
				{
					iCellExistsInMultipleGroups = ETrue;
				}
				iDBHelper->iDBView.FirstL();
				iDBHelper->iDBView.GetL();
				iGroupId = iDBHelper->iDBView.ColUint32(1);
				aCellId[2].iCellDesc.Copy(iDBHelper->iDBView.ColDes(2));
				iDBHelper->iDBView.Close();
				
				// Lookup group name. Only search in active groups
				iDBHelper->iSQLStatement.Format(_L("SELECT Group_Name FROM Groups_List WHERE Group_Id = %u AND State = 1"), iGroupId);
				if (iDBHelper->ReadDB())
				{
					if (iDBHelper->iDBView.CountL() > 0)
					{
						iDBHelper->iDBView.FirstL();
						iDBHelper->iDBView.GetL();
						aCellId[2].iGroupName.Copy(iDBHelper->iDBView.ColDes(1));
						iGroupNameFound = ETrue;
					}
					iDBHelper->iDBView.Close();
				}
			}
		}
	}
	// A group can change under the following conditions
	// iGroupNameFound = ETrue
	//		iOldGroupIdNotInitialized = EFalse
	//			iOldGroupId != iGroupId
	//		iOldGroupIdNotInitialized = ETrue
	// iGroupNameFound = EFalse
	//		iOldGroupIdNotInitialized = EFalse	
	TBool iGroupChanged = EFalse;
	if (iGroupNameFound)
	{
		if (!iOldGroupIdNotInitialized)
		{
			if (iOldGroupId != iGroupId)
				iGroupChanged = ETrue;
		}
		else
			iGroupChanged = ETrue;
	}
	else
	{
		if (iOldGroupIdNotInitialized == EFalse)
			iGroupChanged = ETrue;
	}

	if (!iGroupNameFound)
	{
		aCellId[2].iGroupName.Copy(_L("?????"));
		aCellId[2].iCellDesc.Copy(_L("?????"));
	}
		
	if ((iProcessEvents) && (iGroupChanged))
	{
		for (int i = 0; i < iEventTriggerIds->Count(); i++)
		{
			switch (iEventTriggerIds->At(i))
			{
				// Enter event
				case 0 : 
					{
						if (iGroupNameFound)
							DoEvents(iGroupId, 0);
						break;
					};
				// Leave event
				case 1 : 
					{
						if (!iOldGroupIdNotInitialized)
							DoEvents(iOldGroupId, 1); 
						break;
					};
				// Both
				case 2 :
					{
						// Do nothing. This is hard coded in DoEvents(...)
						break;
					};
			};
			
		}
	}		
	if (iGroupNameFound)
	{
		iOldGroupId = iGroupId;
		iOldGroupIdNotInitialized = EFalse;
	}
	else
	{
		iOldGroupId = 0;
		iOldGroupIdNotInitialized = ETrue;
	}

	iBuffer2->Des().Copy(*iCellIdText);
	iBuffer2->Des().Append(CEditableText::ELineBreak);
	
	TPtr iPtr = TPtr(iTempTime->Des());
	TDes *iTemp = &iPtr;
	iBuffer2->Des().Append(_L("[%05d - "));
	aCellId[2].iLogonTime.FormatL(*iTemp, _L("%-B%:0%J%:1%T%:2%S%:3%+B"));
	iBuffer2->Des().Append(*iTemp);
	iBuffer2->Des().Append(_L("]"));
	iBuffer2->Des().Append(CEditableText::ELineBreak);
	
	iBuffer2->Des().Append(_L("%05d - "));
	aCellId[1].iLogonTime.FormatL(*iTemp, _L("%-B%:0%J%:1%T%:2%S%:3%+B"));
	iBuffer2->Des().Append(*iTemp);
	iBuffer2->Des().Append(CEditableText::ELineBreak);
	
	iBuffer2->Des().Append(_L("%05d - "));
	aCellId[0].iLogonTime.FormatL(*iTemp, _L("%-B%:0%J%:1%T%:2%S%:3%+B"));
	iBuffer2->Des().Append(*iTemp);

	iBuffer2->Des().Append(CEditableText::ELineBreak);
	if (!iCellExistsInMultipleGroups)
		iBuffer2->Des().Append(*iGroupText);
	else
		iBuffer2->Des().Append(*iMultiGroupText);
	iBuffer2->Des().Append(aCellId[2].iGroupName);

	iBuffer2->Des().Append(CEditableText::ELineBreak);
	iBuffer2->Des().Append(*iCellText);
	iBuffer2->Des().Append(aCellId[2].iCellDesc);

	iBuffer->Des().Format(*iBuffer2, aCellId[2].iCellId, aCellId[1].iCellId, aCellId[0].iCellId);

	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iFCCellAreaView->iNetworkInfoLabel->MakeVisible(EFalse);
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iFCCellAreaView->iNetworkInfoLabel->SetTextL(*iBuffer);
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iFCCellAreaView->iNetworkInfoLabel->MakeVisible(ETrue);

	iBuffer->Des().Append(CEditableText::ELineBreak);
	iBuffer->Des().Append(*iShortNameText);
	iBuffer->Des().Append(*aShortName);
	
	iBuffer->Des().Append(CEditableText::ELineBreak);
	iBuffer->Des().Append(*iLongNameText);
	iBuffer->Des().Append(*aLongName);

	iBuffer->Des().Append(CEditableText::ELineBreak);
	iBuffer->Des().Append(*iCountryCodeText);
	iBuffer->Des().Append(aCellId[2].iCountryCode);

	iBuffer->Des().Append(CEditableText::ELineBreak);
	iBuffer->Des().Append(*iNetworkIdText);
	iBuffer->Des().Append(aCellId[2].iNetworkId);

	iNetworkInfoLabel->MakeVisible(EFalse);
	iNetworkInfoLabel->SetTextL(*iBuffer);
	iNetworkInfoLabel->MakeVisible(ETrue);
}

void CSMan2CellAreaView::UpdateSignalStrength(TInt32 aSignalStrength)
{
	TBuf<30> iBuffer;
	iBuffer.Copy(*iSignalStrengthText);
	if (aSignalStrength > 62)
		iBuffer.Append(_L("> -47"));
	else
		// Casting this should be fine. Very unlikely that aSignalStrength will reach the limits
		// of the data type.
		iBuffer.AppendNum(TInt(aSignalStrength) - TInt(110));
	iBuffer.Append(_L("dBm"));
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iFCCellAreaView->iSignalStrengthLabel->MakeVisible(EFalse);
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iFCCellAreaView->iSignalStrengthLabel->SetTextL(iBuffer);
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iFCCellAreaView->iSignalStrengthLabel->MakeVisible(ETrue);

	iSignalStrengthLabel->MakeVisible(EFalse);
	iSignalStrengthLabel->SetTextL(iBuffer);	
	iSignalStrengthLabel->MakeVisible(ETrue);
}

void CSMan2CellAreaView::HandleSwitchToFlightMode()
{
	iSignalStrengthRefresher->iSignalStrengthdB = 0;
	UpdateSignalStrength(0);
}

void CSMan2CellAreaView::ForceCellInfoUpdate()
{
	iNetworkInfoRefresher->InfoChanged();
}

TBool CSMan2CellAreaView::DoChangeEventDelay()
{
	CCellEventDelay *iEventDelayDialog = new (ELeave) CCellEventDelay();
	iEventDelayDialog->iDelay = &(configData->iCellEventFiringDelayInMinutes);
	if (iEventDelayDialog->ExecuteLD(R_DIALOG_CELLAREA_EVENT_FIRING_DELAY) == EEikBidYes)
	{
		if (configData->iCellEventFiringDelayInMinutes == 0)
		{
			iEventFireTimeHistory->Reset();
			iEventFireTimeHistory->Compress();
		}
		return ETrue;
	}
	else
		return EFalse;
}

/*************************************************************
*
* Network info notification handler
*
* Caller is responsible for making sure no outstanding
* asynch request is present
*
**************************************************************/

CSMan2NetworkInfoRefresh::CSMan2NetworkInfoRefresh(RMobilePhone *aPhoneServer, CSMan2CellAreaView *aView) : CActive(EPriorityNormal)
{
	CActiveScheduler::Add(this);
	iNetworkInfoPackage = new (ELeave) RMobilePhone::TMobilePhoneNetworkInfoV1Pckg(iNetworkInfo);
	iOldCurrentCellId = 0;
	iPastCellId[0].iCellId = 0;
	iPastCellId[0].iGroupName.SetLength(0);
	iPastCellId[0].iCellDesc.SetLength(0);
	iPastCellId[0].iLogonTime = TTime(0);
	iPastCellId[1].iCellId = 0;
	iPastCellId[1].iGroupName.SetLength(0);
	iPastCellId[1].iCellDesc.SetLength(0);
	iPastCellId[1].iLogonTime = TTime(0);
	iPastCellId[2].iCellId = 0;
	iPastCellId[2].iGroupName.SetLength(0);
	iPastCellId[2].iCellDesc.SetLength(0);
	iPastCellId[2].iLogonTime = TTime(0);
	iCurrentOperation = EGettingFirstReading;
	iLocalPhoneServer = aPhoneServer;
	iLocalView = aView;
}

CSMan2NetworkInfoRefresh::~CSMan2NetworkInfoRefresh()
{
	delete iNetworkInfoPackage;
}

void CSMan2NetworkInfoRefresh::ActivateNotification()
{
	if (!IsActive())
	{
		iCurrentOperation = EGettingFirstReading;
		iStatus = KRequestPending;
		iLocalPhoneServer->GetCurrentNetwork(iStatus, *iNetworkInfoPackage, iCellInfo);
		SetActive();
	}
}

void CSMan2NetworkInfoRefresh::DeactivateNotification()
{
	if (IsActive())
	{
		switch (iCurrentOperation)
		{
			case EGettingFirstReading : iLocalPhoneServer->CancelAsyncRequest(EMobilePhoneGetCurrentNetwork); break;
			case EGettingSubsequentReading : iLocalPhoneServer->CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
		}
	}
}

void CSMan2NetworkInfoRefresh::DoCancel()
{
	DeactivateNotification();
}

void CSMan2NetworkInfoRefresh::InfoChanged()
{
	TUint iTempCellId;
	
	if ((iCellInfo.iCellId == 0) && (iNetworkInfo.iShortName.Length() != 0))
		iTempCellId = iOldCurrentCellId;
	else
		iTempCellId = iCellInfo.iCellId;

	if (iPastCellId[2].iCellId != iTempCellId)
	{
		iPastCellId[0] = iPastCellId[1];
		iPastCellId[1] = iPastCellId[2];
		iPastCellId[2].iCellId = iTempCellId;
		iPastCellId[2].iLogonTime.HomeTime();
		iPastCellId[2].iCountryCode.Copy(iNetworkInfo.iCountryCode);
		iPastCellId[2].iNetworkId.Copy(iNetworkInfo.iNetworkId);
	}

	if (iCellInfo.iCellId != 0)
		iOldCurrentCellId = iCellInfo.iCellId;
	iLocalView->UpdateNetworkInfo(&iPastCellId[0], &(iNetworkInfo.iShortName), &(iNetworkInfo.iLongName));
	
	// Did the phone just enter flight mode?
	if (iNetworkInfo.iShortName.Length() == 0)
		iLocalView->HandleSwitchToFlightMode();
}

void CSMan2NetworkInfoRefresh::RunL()
{
	InfoChanged();
	// Register for subsequent notifications
	iCurrentOperation = EGettingSubsequentReading;
	iStatus = KRequestPending;
	iLocalPhoneServer->NotifyCurrentNetworkChange(iStatus, *iNetworkInfoPackage, iCellInfo);
	SetActive();
}

/*************************************************************
*
* Network info notification handler
*
* Caller is responsible for making sure no outstanding
* asynch request is present
*
**************************************************************/

CSMan2SignalStrengthRefresh::CSMan2SignalStrengthRefresh(RMobilePhone *aPhoneServer, CSMan2CellAreaView *aView) : CActive(EPriorityNormal)
{
	CActiveScheduler::Add(this);
	iSignalStrengthdB = 0;
	iSignalStrengthBar = 0;
	iCurrentOperation = EGettingFirstReading;
	iLocalPhoneServer = aPhoneServer;
	iLocalView = aView;
}

CSMan2SignalStrengthRefresh::~CSMan2SignalStrengthRefresh()
{
}

void CSMan2SignalStrengthRefresh::ActivateNotification()
{
	if (!IsActive())
	{
		iCurrentOperation = EGettingFirstReading;
		iStatus = KRequestPending;
		iLocalPhoneServer->GetSignalStrength(iStatus, iSignalStrengthdB, iSignalStrengthBar);
		SetActive();
	}
}

void CSMan2SignalStrengthRefresh::DeactivateNotification()
{
	if (IsActive())
	{
		switch (iCurrentOperation)
		{
			case EGettingFirstReading : iLocalPhoneServer->CancelAsyncRequest(EMobilePhoneGetSignalStrength); break;
			case EGettingSubsequentReading : iLocalPhoneServer->CancelAsyncRequest(EMobilePhoneNotifySignalStrengthChange);
		}
	}
}

void CSMan2SignalStrengthRefresh::DoCancel()
{
	DeactivateNotification();
}

void CSMan2SignalStrengthRefresh::RunL()
{
	iLocalView->UpdateSignalStrength(iSignalStrengthdB);
	
	// Register for subsequent notifications
	iCurrentOperation = EGettingSubsequentReading;
	iStatus = KRequestPending;
	iLocalPhoneServer->NotifySignalStrengthChange(iStatus, iSignalStrengthdB, iSignalStrengthBar);
	SetActive();
}
