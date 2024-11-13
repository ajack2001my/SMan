#include "dialogs.h"
#include "sman.h"
#include "textutils.h"
#include <eikrted.h>
#include <QikDateEditor.h>
#include <QikTimeAndDateEditor.h>
#include <gulftflg.hrh>
#include <gulicon.h>
#include <sman.rsg>

/*************************************************************
*
* Log level dialog
*
**************************************************************/

CLogLevelDialog::~CLogLevelDialog()
{
}

CLogLevelDialog::CLogLevelDialog(TUint *logFlags)
{
	localLogFlags = logFlags;
}

TBool CLogLevelDialog::OkToExitL(TInt aButtonId)
{
	CEikCheckBox *controlLogBluejack = STATIC_CAST(CEikCheckBox*, Control(cLogBluejack));
	CEikCheckBox *controlLogOBEX = STATIC_CAST(CEikCheckBox*, Control(cLogOBEX));
	CEikCheckBox *controlLogScanning = STATIC_CAST(CEikCheckBox*, Control(cLogScanning));
	CEikCheckBox *controlLogDecays = STATIC_CAST(CEikCheckBox*, Control(cLogDecays));
	if (aButtonId == EEikBidOk)
	{
		TUint tempFlags;
		
		tempFlags = 0;
		if (controlLogBluejack->State() == CEikButtonBase::ESet)
			tempFlags = (tempFlags | CLogger::logBluejack);
		if (controlLogOBEX->State() == CEikButtonBase::ESet)
			tempFlags = (tempFlags | CLogger::logOBEX);
		if (controlLogScanning->State() == CEikButtonBase::ESet)
			tempFlags = (tempFlags | CLogger::logScanning);
		if (controlLogDecays->State() == CEikButtonBase::ESet)
			tempFlags = (tempFlags | CLogger::logDecays);
		*localLogFlags = tempFlags;
		return ETrue;
	}
	return ETrue;
} 

void CLogLevelDialog::PreLayoutDynInitL()
{
	CEikCheckBox *controlLogBluejack = STATIC_CAST(CEikCheckBox*, Control(cLogBluejack));
	CEikCheckBox *controlLogOBEX = STATIC_CAST(CEikCheckBox*, Control(cLogOBEX));
	CEikCheckBox *controlLogScanning = STATIC_CAST(CEikCheckBox*, Control(cLogScanning));
	CEikCheckBox *controlLogDecays = STATIC_CAST(CEikCheckBox*, Control(cLogDecays));
	
	if (((*localLogFlags) & CLogger::logBluejack) == CLogger::logBluejack)
		controlLogBluejack->SetState(CEikButtonBase::ESet);
	if (((*localLogFlags) & CLogger::logOBEX) == CLogger::logOBEX)
		controlLogOBEX->SetState(CEikButtonBase::ESet);
	if (((*localLogFlags) & CLogger::logScanning) == CLogger::logScanning)
		controlLogScanning->SetState(CEikButtonBase::ESet);
	if (((*localLogFlags) & CLogger::logDecays) == CLogger::logDecays)
		controlLogDecays->SetState(CEikButtonBase::ESet);
}

/*************************************************************
*
* Bluejack timeout dialogs
*
**************************************************************/

CBluejackTimerDialog::~CBluejackTimerDialog()
{
}

CBluejackTimerDialog::CBluejackTimerDialog(TInt *noDevicePause, TInt *noDeviceAttempts, TInt *decayTime, TInt *obexTimeout)
{
	localNoDevicePause = noDevicePause;
	localNoDeviceAttempts = noDeviceAttempts;
	localDecayTime = decayTime;
	localObexTimeout = obexTimeout;
}

TBool CBluejackTimerDialog::OkToExitL(TInt aButtonId)
{
	CQikNumberEditor *noDevicePause = STATIC_CAST(CQikNumberEditor*, Control(cNoDevicePause));
	CQikNumberEditor *noDeviceAttempts = STATIC_CAST(CQikNumberEditor*, Control(cNoDeviceAttempts));
	CQikNumberEditor *decayTime = STATIC_CAST(CQikNumberEditor*, Control(cDecayTime));
	CQikNumberEditor *obexTimeout = STATIC_CAST(CQikNumberEditor*, Control(cOBEXTimeout));
	if (aButtonId == EEikBidYes)
	{
		noDevicePause->SetValueL(CONFIG_NO_DEVICE_PAUSE);
		noDeviceAttempts->SetValueL(CONFIG_NO_DEVICE_ATTEMPTS);
		decayTime->SetValueL(CONFIG_DECAY_TIME);
		obexTimeout->SetValueL(CONFIG_OBEX_TIMEOUT);
		return EFalse;
	}
	else if (aButtonId == EEikBidOk)
	{
		*localNoDevicePause = noDevicePause->Value();
		*localNoDeviceAttempts = noDeviceAttempts->Value();
		*localDecayTime = decayTime->Value();
		*localObexTimeout = obexTimeout->Value();
		return ETrue;
	}
	return ETrue;
} 

void CBluejackTimerDialog::PreLayoutDynInitL()
{
	CQikNumberEditor *noDevicePause = STATIC_CAST(CQikNumberEditor*, Control(cNoDevicePause));
	noDevicePause->SetValueL(*localNoDevicePause);
	CQikNumberEditor *noDeviceAttempts = STATIC_CAST(CQikNumberEditor*, Control(cNoDeviceAttempts));
	noDeviceAttempts->SetValueL(*localNoDeviceAttempts);
	CQikNumberEditor *decayTime = STATIC_CAST(CQikNumberEditor*, Control(cDecayTime));
	decayTime->SetValueL(*localDecayTime);
	CQikNumberEditor *obexTimeout = STATIC_CAST(CQikNumberEditor*, Control(cOBEXTimeout));
	obexTimeout->SetValueL(*localObexTimeout);
	
	CEikCaptionedControl *decayTimeControl = Line(cDecayTime);
	decayTimeControl->SetDividerAfter(ETrue);
}

/*************************************************************
*
* Hotkey dialog
*
**************************************************************/

CHotkeyDialog::CHotkeyDialog()
{
	iShortcutNumbers_FO = new (ELeave) CArrayFixFlat<TInt>(3);
	iShortcutNumbers_FC = new (ELeave) CArrayFixFlat<TInt>(3);
}

CHotkeyDialog::~CHotkeyDialog()
{
	delete iShortcutNumbers_FO;
	delete iShortcutNumbers_FC;
	delete iFOShortcutArray;
	delete iFCShortcutArray;
	delete iHotkeyArray;
}

TBool CHotkeyDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidOk)
	{
		*iHotkey = static_cast<CEikChoiceList*>(Control(cHotkeyControl))->CurrentItem();
	
		// Retrieve FO values
		*iInFocusShort_FO = iShortcutNumbers_FO->At(static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFO))->CurrentItem());
		*iInFocusLong_FO = iShortcutNumbers_FO->At(static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFO))->CurrentItem());
		*iExFocusShort_FO = iShortcutNumbers_FO->At(static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFO))->CurrentItem());
		*iExFocusLong_FO = iShortcutNumbers_FO->At(static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFO))->CurrentItem());

		// Retrieve FC values
		*iInFocusShort_FC = iShortcutNumbers_FC->At(static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFC))->CurrentItem());
		*iInFocusLong_FC = iShortcutNumbers_FC->At(static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFC))->CurrentItem());
		*iExFocusShort_FC = iShortcutNumbers_FC->At(static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFC))->CurrentItem());
		*iExFocusLong_FC = iShortcutNumbers_FC->At(static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFC))->CurrentItem());		
	}
	return ETrue;
} 

void CHotkeyDialog::HandleControlStateChangeL(TInt aControlId)
{
	if (aControlId == cHotkeyControl)
	{
		InitFOPage_Controls();
		if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasFlip))
			InitFCPage_Controls();
	}
}

void CHotkeyDialog::InitFOPage_Controls()
{
	TBool iDimState = EFalse;
	
	if ((static_cast<CEikChoiceList*>(Control(cHotkeyControl))->CurrentItem() != 1) &&
		(static_cast<CEikChoiceList*>(Control(cHotkeyControl))->CurrentItem() != 2))
		iDimState = ETrue;
		
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFO))->SetDimmed(iDimState);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFO))->SetDimmed(iDimState);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFO))->SetDimmed(iDimState);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFO))->SetDimmed(iDimState);
}

void CHotkeyDialog::InitFCPage_Controls()
{
	TBool iDimState = EFalse;
	
	if ((static_cast<CEikChoiceList*>(Control(cHotkeyControl))->CurrentItem() != 1) &&
		(static_cast<CEikChoiceList*>(Control(cHotkeyControl))->CurrentItem() != 2))
		iDimState = ETrue;
		
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFC))->SetDimmed(iDimState);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFC))->SetDimmed(iDimState);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFC))->SetDimmed(iDimState);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFC))->SetDimmed(iDimState);
}

void CHotkeyDialog::PreLayoutDynInitL()
{
	iFOShortcutArray = CEikonEnv::Static()->ReadDesCArrayResourceL(R_HOTKEY_CHOICES_FO);
	iFCShortcutArray = CEikonEnv::Static()->ReadDesCArrayResourceL(R_HOTKEY_CHOICES_FC);
	
	TUint iRunningPhoneModel = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iRunningPhoneModel;
	if ((iRunningPhoneModel == CSMan2AppUi::EPhoneModel_A920) || 
		(iRunningPhoneModel == CSMan2AppUi::EPhoneModel_A925) ||
		(iRunningPhoneModel == CSMan2AppUi::EPhoneModel_A925_C) ||
		(iRunningPhoneModel == CSMan2AppUi::EPhoneModel_A920_C))
		iHotkeyArray = CEikonEnv::Static()->ReadDesCArrayResourceL(R_HOTKEY_ARRAY_A920_A925);
	else
		iHotkeyArray = CEikonEnv::Static()->ReadDesCArrayResourceL(R_HOTKEY_ARRAY_P800_P900);

	static_cast<CEikChoiceList*>(Control(cHotkeyControl))->SetArrayL(iHotkeyArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyControl))->SetArrayExternalOwnership(ETrue);
	static_cast<CEikChoiceList*>(Control(cHotkeyControl))->SetCurrentItem(*iHotkey);

	
	iFOShortcutArray->Sort();
	iFCShortcutArray->Sort();
	
	TPtrC iTextColumn, iNumberColumn;
	TBuf<50> iTextBuffer;
	TLex iLexConverter;
	TInt iTempNumberExtracted;
	
	// Extract numbers for FO array
	for (int i = 0; i < iFOShortcutArray->MdcaCount(); i++)
	{
		iTextBuffer.Copy(iFOShortcutArray->MdcaPoint(i));
		TextUtils::ColumnText(iTextColumn, 0, &iTextBuffer, TChar(';'));
		TextUtils::ColumnText(iNumberColumn, 1, &iTextBuffer, TChar(';'));
		
		iLexConverter.Assign(iNumberColumn);
		iLexConverter.Val(iTempNumberExtracted);
		iShortcutNumbers_FO->AppendL(iTempNumberExtracted);
		
		iTextBuffer.Copy(iTextColumn);		
		iFOShortcutArray->Delete(i);
		iFOShortcutArray->InsertL(i, iTextBuffer);
	}

	// Extract numbers for FC array
	for (int i = 0; i < iFCShortcutArray->MdcaCount(); i++)
	{
		iTextBuffer.Copy(iFCShortcutArray->MdcaPoint(i));
		TextUtils::ColumnText(iTextColumn, 0, &iTextBuffer, TChar(';'));
		TextUtils::ColumnText(iNumberColumn, 1, &iTextBuffer, TChar(';'));
		
		iLexConverter.Assign(iNumberColumn);
		iLexConverter.Val(iTempNumberExtracted);
		iShortcutNumbers_FC->AppendL(iTempNumberExtracted);
		
		iTextBuffer.Copy(iTextColumn);		
		iFCShortcutArray->Delete(i);
		iFCShortcutArray->InsertL(i, iTextBuffer);
	}
	
	// Set array for FO controls
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFO))->SetArrayL(iFOShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFO))->SetArrayExternalOwnership(ETrue);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFO))->SetArrayL(iFOShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFO))->SetArrayExternalOwnership(ETrue);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFO))->SetArrayL(iFOShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFO))->SetArrayExternalOwnership(ETrue);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFO))->SetArrayL(iFOShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFO))->SetArrayExternalOwnership(ETrue);

	// Set array for FC controls
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFC))->SetArrayL(iFCShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFC))->SetArrayExternalOwnership(ETrue);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFC))->SetArrayL(iFCShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFC))->SetArrayExternalOwnership(ETrue);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFC))->SetArrayL(iFCShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFC))->SetArrayExternalOwnership(ETrue);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFC))->SetArrayL(iFCShortcutArray);
	static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFC))->SetArrayExternalOwnership(ETrue);
	
	// Disable controls if not the correct hardware
	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasFlip)))
	{
		static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFC))->SetDimmed(ETrue);
		static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFC))->SetDimmed(ETrue);
		static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFC))->SetDimmed(ETrue);
		static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFC))->SetDimmed(ETrue);

		// Assume all other phones don't have a FC mode
		RemoveFCAction(2);
		RemoveFCAction(3);
		RemoveFCAction(8);
		RemoveFCAction(9);
		RemoveFCAction(10);
		RemoveFCAction(11);
		RemoveFCAction(14);
		RemoveFCAction(18);
	}
	
	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanAutoStart)))
	{
		RemoveFOAction(1);
		RemoveFOAction(2);
		RemoveFOAction(3);
		
		RemoveFCAction(1);
		RemoveFCAction(2);
		RemoveFCAction(3);
	}
		
	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasIRDACPL)))
		RemoveFOAction(5);

	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasPhoneCPL)))
		RemoveFOAction(7); 

	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasAppListBug)))
		RemoveFOAction(11);
		
	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasTimeSynchZoneBug)))	
		RemoveFOAction(12);

	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanChangeFont)))	
		RemoveFOAction(13);

	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasNetworkInfoTSY)))
		RemoveFOAction(18);

	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasBluetooth)))
		RemoveFOAction(14);
		
	if (!(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasBluetoothCPL)))
		RemoveFOAction(4);

	// Initialize current items for FO
	TInt iArrayIndex = 0;
	TKeyArrayFix iKeyArray = TKeyArrayFix(0, ECmpTInt);
	if (iShortcutNumbers_FO->Find(*iInFocusShort_FO, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFO))->SetCurrentItem(iArrayIndex);	
	iArrayIndex = 0;
	if (iShortcutNumbers_FO->Find(*iInFocusLong_FO, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFO))->SetCurrentItem(iArrayIndex);	
	iArrayIndex = 0;
	if (iShortcutNumbers_FO->Find(*iExFocusShort_FO, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFO))->SetCurrentItem(iArrayIndex);	
	iArrayIndex = 0;
	if (iShortcutNumbers_FO->Find(*iExFocusLong_FO, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFO))->SetCurrentItem(iArrayIndex);	

	// Initialize current items for FC
	iArrayIndex = 0;
	if (iShortcutNumbers_FC->Find(*iInFocusShort_FC, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyInFocusShortFC))->SetCurrentItem(iArrayIndex);
	iArrayIndex = 0;
	if (iShortcutNumbers_FC->Find(*iInFocusLong_FC, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyInFocusLongFC))->SetCurrentItem(iArrayIndex);	
	iArrayIndex = 0;
	if (iShortcutNumbers_FC->Find(*iExFocusShort_FC, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyExFocusShortFC))->SetCurrentItem(iArrayIndex);	
	iArrayIndex = 0;
	if (iShortcutNumbers_FC->Find(*iExFocusLong_FC, iKeyArray, iArrayIndex) == 0)
		static_cast<CEikChoiceList*>(Control(cHotkeyExFocusLongFC))->SetCurrentItem(iArrayIndex);		
		
	InitFOPage_Controls();
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasFlip))
		InitFCPage_Controls();
}

void CHotkeyDialog::RemoveFOAction(TInt aActionNumber_FO)
{
	if (aActionNumber_FO > -1)
	{
		TInt iArrayIndex = aActionNumber_FO;
		TKeyArrayFix iKeyArray = TKeyArrayFix(0, ECmpTInt);
		if (iShortcutNumbers_FO->Find(aActionNumber_FO, iKeyArray, iArrayIndex) == 0)
		{
			iShortcutNumbers_FO->Delete(iArrayIndex);
			iFOShortcutArray->Delete(iArrayIndex);
		}
	}
}
	
void CHotkeyDialog::RemoveFCAction(TInt aActionNumber_FC)
{
	if (aActionNumber_FC > -1)
	{
		TInt iArrayIndex = aActionNumber_FC;
		TKeyArrayFix iKeyArray = TKeyArrayFix(0, ECmpTInt);
		if (iShortcutNumbers_FC->Find(aActionNumber_FC, iKeyArray, iArrayIndex) == 0)
		{
			iShortcutNumbers_FC->Delete(iArrayIndex);
			iFCShortcutArray->Delete(iArrayIndex);
		}
	}
}

/*************************************************************
*
* Shortcut dialog
*
**************************************************************/

CShortcutDialog::CShortcutDialog(CArrayFixFlat<TUid> *shortCut)
{
	tempUid = shortCut;
	// For each app we get from the app server, store its caption and uid
	appListArray = new (ELeave) CDesCArraySeg(3);
	appListUid = new (ELeave) CArrayFixFlat<TUid>(3);
}

CShortcutDialog::~CShortcutDialog()
{
	delete appListArray;
	delete appListUid;
}

TBool CShortcutDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidOk)
	{
		if (tempUid != NULL)
		{
			for (int i = 0; i < tempUid->Count(); i++)
			{
				CEikChoiceList *myShortCut = STATIC_CAST(CEikChoiceList*, Control(cShortcut1 + i));
				if (tempUid != NULL)
					tempUid->At(i) = appListUid->At(myShortCut->CurrentItem());
			}
		}
		return ETrue;
	}
	return EFalse;
} 

void CShortcutDialog::PreLayoutDynInitL()
{
	int dump;
	TKeyArrayFix keyArray = TKeyArrayFix(0, ECmpTUint);
	
	populateAppArray();
	if (tempUid != NULL)
	{
		for (int i = 0; i < tempUid->Count(); i++)
		{
			CEikChoiceList *myShortCut = STATIC_CAST(CEikChoiceList*, Control(cShortcut1 + i));
			myShortCut->SetArrayL(appListArray);
			myShortCut->SetArrayExternalOwnership(ETrue);
			if (appListUid->Find(tempUid->At(i), keyArray, dump) == 0)
			{
				myShortCut->SetCurrentItem(dump);
			}
			else
			{
				myShortCut->SetCurrentItem(0);
				tempUid->At(i) = KNullUid;
			}
		}
	}
}

void CShortcutDialog::populateAppArray(void)
{
	// Scan the list of applications registered with the app server
	// For each shortcut uid, set focus to that item

	RApaLsSession mySession;
	TApaAppInfo theAppInfo;
	TApaAppCapabilityBuf aCapabilityBuf;
	TBuf<1> bSeparator;
	TBuf<10> appUid;
	HBufC* aCaption;
	TUid aUid;
	TPtrC currentCaption, temp, temp1;
	TLex lxUid;
	
	mySession.Connect();
	mySession.GetAllApps();
	bSeparator.Append(255);

	// Need to add the length of appUid and bSeparator into this because in the next for-loop, we are
	// appending these to aCaption. By right, we only need 10 + 1 but I'm being paranoid. Besides, what 
	// is an additional 19 bytes anyway? :)
	aCaption = HBufC::NewL(KApaMaxAppCaption + 30);

	// Query the app server
	while (mySession.GetNextApp(theAppInfo) == KErrNone)
	{
		if (mySession.GetAppCapability(aCapabilityBuf, theAppInfo.iUid) == KErrNone)
		{
			// Only scan applications that are not hidden
			if (aCapabilityBuf().iAppIsHidden == EFalse)
			{
				// For each app we get from the app server, store its caption and uid
				aCaption->Des().Copy(theAppInfo.iCaption);
				aCaption->Des().Append(bSeparator);
				appUid.Format(_L("%u"), theAppInfo.iUid);
				aCaption->Des().Append(appUid);
				appListArray->AppendL(*aCaption);
			}
		}
	}
	mySession.Close();
	appListArray->Sort();

	// The first item is for unassigning a shortcut
	if (tempUid != NULL)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UNASSIGNED);
		HBufC* tempCaption = HBufC::NewL(dataBuffer->Length() + 5);
		tempCaption->Des().Format(*dataBuffer, 255, KNullUid);
		delete dataBuffer;
		appListArray->InsertL(0, *tempCaption);
		delete tempCaption;
	}

	// After sorting, separate the UIDs out. I'm doing this because I couldn't find a
	// nicer way of linking the UID to the Caption while sorting.
	// The assumption is there is no app with a 0xff char in its caption...pretty safe i
	// think!!
	for (int i = 0; i < appListArray->Count(); i++)
	{		
		// Get app caption & uid
		currentCaption.Set(appListArray->MdcaPoint(i));
		TextUtils::ColumnText(temp, 0, &currentCaption, TChar(255));
		TextUtils::ColumnText(temp1, 1, &currentCaption, TChar(255));
		aCaption->Des().Copy(temp);
		lxUid.Assign(temp1);
		lxUid.Val(aUid.iUid);
		appListUid->AppendL(aUid);
		appListArray->Delete(i);
		appListArray->InsertL(i, *aCaption);
	}
	
	appListArray->Compress();
	delete aCaption;
}

/*************************************************************
*
* Autostart dialog. This dialog is self-contained. It has code 
* to read and write SMan.boot.
*
**************************************************************/

CAutostartDialog::CAutostartDialog()
{
	appListCaption = new (ELeave) CDesCArrayFlat(3);
	appListPath = new (ELeave) CArrayFixSeg<TFileName>(3);
	appListUid = new (ELeave) CArrayFixFlat<TUid>(3);
	for (int i = 0; i < MAX_AUTOSTART; i++)
	{
		autoStartConfig.autoStartApp[i].Copy(_L(""));
		autoStartConfig.autoStartUid[i] = KNullUid;
		autoStartConfig.autoStartDefaultDoc[i].Copy(_L(""));
	}
}

CAutostartDialog::~CAutostartDialog()
{
	delete appListCaption;
	delete appListPath;
	delete appListUid;
}

void CAutostartDialog::populateAppArray(void)
{
	// Scan the list of applications registered with the app server
	// Store the app's caption and path

	RApaLsSession mySession;
	TApaAppInfo theAppInfo;
	TApaAppCapabilityBuf aCapabilityBuf;
	TBuf<1> bSeparator;
	HBufC* aCaption;
	TPtrC currentCaption, temp, temp1, temp2;
	TBuf<10> appUid;
	TUid aUid;
	TLex lxUid;
	
	mySession.Connect();
	mySession.GetAllApps();
	bSeparator.Append(255);

	aCaption = HBufC::NewL(KApaMaxAppCaption + 30 + KMaxFileName);

	// Query the app server
	while (mySession.GetNextApp(theAppInfo) == KErrNone)
	{
		if (mySession.GetAppCapability(aCapabilityBuf, theAppInfo.iUid) == KErrNone)
		{
			// Only scan applications that are not hidden
			if (aCapabilityBuf().iAppIsHidden == EFalse)
			{
				// For each app we get from the app server, store its caption and path
				aCaption->Des().Copy(theAppInfo.iCaption);
				aCaption->Des().Append(bSeparator);
				aCaption->Des().Append(theAppInfo.iFullName);
				aCaption->Des().Append(bSeparator);
				appUid.Format(_L("%u"), theAppInfo.iUid);
				aCaption->Des().Append(appUid);
				appListCaption->AppendL(*aCaption);
			}
		}
	}
	mySession.Close();
	appListCaption->Sort();

	// The first item is for unassigning an autostart
	HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UNASSIGNED2);
	HBufC* tempCaption = HBufC::NewL(dataBuffer->Length() + 10);
	tempCaption->Des().Format(*dataBuffer, 255, 0, 255, KNullUid);
	delete dataBuffer;
	appListCaption->InsertL(0, *tempCaption);
	delete tempCaption;

/*
	aCaption = aCaption->ReAllocL(KApaMaxAppCaption + dataBuffer->Length() + 5);
	aCaption->Des().Format(*dataBuffer, 255, 0, 255, KNullUid);
	delete dataBuffer;
	//aCaption.Format(_L("<unassigned>%c%c%c%c"), 255, 0, 255, KNullUid);
	appListCaption->InsertL(0, *aCaption);
*/

	for (int i = 0; i < appListCaption->Count(); i++)
	{		
		// Get app caption, path and uid
		currentCaption.Set(appListCaption->MdcaPoint(i));
		TextUtils::ColumnText(temp, 0, &currentCaption, TChar(255));
		TextUtils::ColumnText(temp1, 1, &currentCaption, TChar(255));
		TextUtils::ColumnText(temp2, 2, &currentCaption, TChar(255));
		appListPath->AppendL(temp1);
		aCaption->Des().Copy(temp);
		lxUid.Assign(temp2);
		lxUid.Val(aUid.iUid);
		appListUid->AppendL(aUid);		
		appListCaption->Delete(i);
		appListCaption->InsertL(i, *aCaption);
	}
	delete aCaption;
	appListCaption->Compress();
}

void CAutostartDialog::PreLayoutDynInitL()
{
	loadBootFile();
	populateAppArray();

	int dump;
	TKeyArrayFix keyArray = TKeyArrayFix(0, ECmpNormal);
	
	for (int i = 0; i < MAX_AUTOSTART - 1; i++)
	{
		CEikChoiceList *myChoice1 = STATIC_CAST(CEikChoiceList*, Control(cAutoStart1 + i));
		myChoice1->SetArrayL(appListCaption);
		myChoice1->SetArrayExternalOwnership(ETrue);
		if (appListPath->Find(autoStartConfig.autoStartApp[i], keyArray, dump) == 0)
		{
			myChoice1->SetCurrentItem(dump);
		}
		else
		{
			myChoice1->SetCurrentItem(0);
		}
	}
}

TBool CAutostartDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidOk)
	{
		for (int i = 0; i < MAX_AUTOSTART - 1; i++)
		{
			CEikChoiceList *myChoice1 = STATIC_CAST(CEikChoiceList*, Control(cAutoStart1 + i));
			if (myChoice1->CurrentItem() > 0)
			{
				autoStartConfig.autoStartApp[i].Copy(appListPath->At(myChoice1->CurrentItem()));
				autoStartConfig.autoStartUid[i] = appListUid->At(myChoice1->CurrentItem());
				TRAPD(err, QikFileUtils::MakeAppDocumentFileNameL(autoStartConfig.autoStartDefaultDoc[i], autoStartConfig.autoStartUid[i]));
				if (err != KErrNone)
					autoStartConfig.autoStartDefaultDoc[i].Copy(_L(""));
			}
			else
			{
				autoStartConfig.autoStartApp[i].Copy(_L(""));
				autoStartConfig.autoStartUid[i] = KNullUid;
				autoStartConfig.autoStartDefaultDoc[i].Copy(_L(""));
			}
		}
		if (saveBootFile() != 0)
			CEikonEnv::Static()->InfoMsgWithAlignment(TGulAlignment(EHRightVCenter), R_TBUF_ERRSAVECONFIG);
	}
	return ETrue;
}

TUint CAutostartDialog::loadBootFile(void)
{
	RFile inStream;
	TInt retVal;
	
	retVal = 1;
	if (inStream.Open(CEikonEnv::Static()->Static()->FsSession(), _L("C:\\SMan.boot"), EFileRead) == KErrNone)
	{
		TPckg<CAutoStartConfig> pkgConfigData(autoStartConfig);
		if (inStream.Read(pkgConfigData) == KErrNone)
			retVal = 0;
		inStream.Close();
	}
	return retVal;
}

TUint CAutostartDialog::saveBootFile(void)
{
	RFile outStream;
	TInt retVal;
	
	retVal = 1;
	if (outStream.Replace(CEikonEnv::Static()->Static()->FsSession(), _L("C:\\SMan.boot"), EFileRead) == KErrNone)
	{
		TPckg<CAutoStartConfig> pkgConfigData(autoStartConfig);
		if (outStream.Write(pkgConfigData) == KErrNone)
			retVal = 0;
		outStream.Close();
	}
	return retVal;
}

/*************************************************************
*
* Bluebeam log dialog.
*
**************************************************************/

CBluebeamLogDialog::CBluebeamLogDialog(CLogger *logObj)
{
	localLogObj = logObj;
}

void CBluebeamLogDialog::PostLayoutDynInitL()
{
	localLogObj->logDialog = this;
}

CBluebeamLogDialog::~CBluebeamLogDialog()
{
	localLogObj->logDialog = NULL;
}

TKeyResponse CBluebeamLogDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cBluebeamLog))->MoveDisplayL(TCursorPosition::EFLineDown);
			static_cast<CEikGlobalTextEditor*>(Control(cBluebeamLog))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cBluebeamLog))->MoveDisplayL(TCursorPosition::EFLineUp);
			static_cast<CEikGlobalTextEditor*>(Control(cBluebeamLog))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
}

void CBluebeamLogDialog::PreLayoutDynInitL()
{
	CEikGlobalTextEditor* textEditor = STATIC_CAST(CEikGlobalTextEditor*,Control(cBluebeamLog));
	
	textEditor->CreateScrollBarFrameL();
	textEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
    
	ButtonGroupContainer().DimCommand(EEikBidYes, ETrue);
}

TBool CBluebeamLogDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId != EEikBidYes)
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->blueBeam->Cancel();
	return ETrue;
}

/*************************************************************
*
* Bluejack notifications dialog.
*
**************************************************************/
CBluejackNotifications::CBluejackNotifications(CConfig *cData)
{
	configData = cData;
}

CBluejackNotifications::~CBluejackNotifications()
{
}

void CBluejackNotifications::PreLayoutDynInitL()
{
	CEikCheckBox *controlVibraNotify = STATIC_CAST(CEikCheckBox*, Control(cVibraNotify));
	CEikCheckBox *controlSystemMsgNotify = STATIC_CAST(CEikCheckBox*, Control(cSystemMsgNotify));

	// myVibra is set to NULL if the vibrationi API wasn't found. By default, the P900 has it
	// factory installed.
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iBJackView->oScanner->iSManECom == NULL)
		static_cast<CEikCheckBox*>(Control(cVibraNotify))->SetDimmed(ETrue);
	else
	{
		if (configData->vibraNotify == true)
			controlVibraNotify->SetState(CEikButtonBase::ESet);
	}

	if (configData->showBluejackSysMsg == true)
		controlSystemMsgNotify->SetState(CEikButtonBase::ESet);
	CEikChoiceList *audioClip = STATIC_CAST(CEikChoiceList*, Control(cAudioFileName));
	audioClip->SetArrayL(R_ARRAY_AUDIONOTIFY);
	if (configData->audioFileName.Length() == 0)
		audioClip->SetCurrentItem(0);
	else
	{
		selectedAudioFileName.Copy(configData->audioFileName);
		TParsePtr parsedAudioFileName(configData->audioFileName);
		audioClip->DesCArray()->AppendL(parsedAudioFileName.NameAndExt());
		audioClip->SetCurrentItem(2);
	}
}

TBool CBluejackNotifications::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidOk)
	{
		CEikCheckBox *controlVibraNotify = STATIC_CAST(CEikCheckBox*, Control(cVibraNotify));
		CEikCheckBox *controlSystemMsgNotify = STATIC_CAST(CEikCheckBox*, Control(cSystemMsgNotify));		
		if (controlVibraNotify->State() == CEikButtonBase::ESet)
			configData->vibraNotify = ETrue;
		else
			configData->vibraNotify = EFalse;
		if (controlSystemMsgNotify->State() == CEikButtonBase::ESet)
			configData->showBluejackSysMsg = ETrue;
		else
			configData->showBluejackSysMsg = EFalse;
		CEikChoiceList *audioClip = STATIC_CAST(CEikChoiceList*, Control(cAudioFileName));
		if (audioClip->CurrentItem() == 0)
			configData->audioFileName.Copy(_L(""));
		else
			configData->audioFileName.Copy(selectedAudioFileName);
	}
	return ETrue;
}

void CBluejackNotifications::HandleControlStateChangeL(TInt aControlId)
{
	if (aControlId == cAudioFileName)
	{
		CEikChoiceList *audioClip = STATIC_CAST(CEikChoiceList*, Control(cAudioFileName));
		if (audioClip->CurrentItem() == 1)
		{
			pickAudioFile();
		}
	}
}

void CBluejackNotifications::pickAudioFile()
{
	CDesCArray* audioMimeArray = new(ELeave) CDesCArrayFlat(2);
	CEikChoiceList *audioClip = STATIC_CAST(CEikChoiceList*, Control(cAudioFileName));
	audioMimeArray->AppendL(_L("audio/x-wav"));		// WAV
	audioMimeArray->AppendL(_L("audio/x-midi"));		// MID
	audioMimeArray->AppendL(_L("audio/x-rmf"));		// RMF
	if (CQikSelectMediaFileDlg::RunDlgLD(*audioMimeArray, selectedAudioFileName))
	{
		if (audioClip->DesCArray()->MdcaCount() > 2)
			audioClip->DesCArray()->Delete(2);
		TParsePtr parsedAudioFileName(selectedAudioFileName);
		configData->audioFileName.Copy(selectedAudioFileName);
		audioClip->DesCArray()->AppendL(parsedAudioFileName.NameAndExt());
		audioClip->SetCurrentItem(2);
	}
	else
	{
		audioClip->SetCurrentItem(0);
	}
	audioClip->DesCArray()->Compress();
	delete audioMimeArray;
}

/*************************************************************
*
* Edit the bluejack vcard
*
**************************************************************/
CBluejackMessageDialog::CBluejackMessageDialog(CScanner *scannerObj)
{
	localScannerObj = scannerObj;
}

CBluejackMessageDialog::~CBluejackMessageDialog()
{
}

void CBluejackMessageDialog::makeVCard(void)
{
	RFile outStream;
	TBuf8<MAX_VCARD_SIZE> vcardBuffer8bit;
	
	vcardBuffer8bit.Copy(currBJackMsg);
	vcardBuffer8bit.Insert(0, _L8("BEGIN:VCARD\r\nVERSION:2.1\r\nN:"));
	vcardBuffer8bit.Append(_L8("\r\nEND:VCARD"));
	if (outStream.Replace(CEikonEnv::Static()->Static()->FsSession(), localScannerObj->vCardNames[0], EFileRead) == KErrNone)
	{
		outStream.Write(vcardBuffer8bit);
		outStream.Close();
	}
}

TBool CBluejackMessageDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidOk)
	{
		CEikGlobalTextEditor* textEditor = STATIC_CAST(CEikGlobalTextEditor*,Control(cBluejackMessage));
		textEditor->GetText(currBJackMsg);
		makeVCard();
	}
	return ETrue;
}

void CBluejackMessageDialog::PreLayoutDynInitL()
{
	// Read the existing vcard
	
	RFile inStream;
	TBuf8<MAX_VCARD_SIZE> buffer8bit;

	buffer8bit.Copy(_L(""));
	if (inStream.Open(CEikonEnv::Static()->Static()->FsSession(), localScannerObj->vCardNames[0], EFileRead) == KErrNone)
	{
		TInt fileSize;
		
		inStream.Size(fileSize);
		if ((fileSize > 1) && (fileSize < MAX_VCARD_SIZE))
			inStream.Read(buffer8bit);
		inStream.Close();
	}
	currBJackMsg.Copy(buffer8bit);
	TInt nameField = currBJackMsg.Find(_L("\r\nN:"));
	if (nameField != KErrNotFound)
	{
		currBJackMsg = currBJackMsg.Mid(nameField + 4, currBJackMsg.Length() - nameField - 4);
		nameField = currBJackMsg.Find(_L("\r\n"));
		if (nameField != KErrNotFound)
			currBJackMsg.SetLength(nameField);
	}
	if (currBJackMsg.Length() > MAX_VCARD_FULLNAME)
		currBJackMsg.SetLength(MAX_VCARD_FULLNAME);
	CEikGlobalTextEditor* textEditor = STATIC_CAST(CEikGlobalTextEditor*,Control(cBluejackMessage));
	textEditor->SetTextL(&currBJackMsg);
}

/*************************************************************
*
* Fileman log dialog - with wait
*
**************************************************************/
void CFileManLogDialog::PostLayoutDynInitL()
{
	CEikGlobalTextEditor* textEditor = STATIC_CAST(CEikGlobalTextEditor*,Control(cFileManProgressText));
	
	textEditor->CreateScrollBarFrameL();
	textEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	textEditor->SetTextL(&text);
	if (!showOverwrite)
	{
		ButtonGroupContainer().DimCommand(EEikBidOverwrite, ETrue);
		ButtonGroupContainer().DimCommand(EEikBidOverwriteAll, ETrue);
	}
}

/*************************************************************
*
* Fileman rename dialog
*
**************************************************************/
void CFileManRenDialog::PreLayoutDynInitL()
{
	static_cast<CEikGlobalTextEditor*>(Control(cFileManNewName))->SetTextL(&fileName);
	fileName.Insert(0, filePath);
}

TBool CFileManRenDialog::OkToExitL(TInt aButtonId)
{
	TBool retVal;
		
	retVal = EFalse;
	if ((aButtonId == EEikBidAbort) || (aButtonId == EEikBidSkip))
		retVal = ETrue;
	else if (aButtonId == EEikBidOk)
	{
		static_cast<CEikGlobalTextEditor*>(Control(cFileManNewName))->GetText(newName);

		TParsePtr sanityCheck(newName);
		if (!sanityCheck.PathPresent())
		{
			newName.Insert(0, filePath);
			if (EikFileUtils::RenameFile(fileName, newName, 0) == KErrNone)
				retVal = ETrue;
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRRENAME);
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_INVALIDFILENAME);
	}
	return retVal;
}

/*************************************************************
*
* Fileman new folder dialog
*
**************************************************************/
TBool CFileManNewDirDialog::OkToExitL(TInt aButtonId)
{
	TBool retVal;
	retVal = EFalse;

	if (aButtonId == EEikBidOk)
	{
		TFileName newName;
		RFs fileServer;
		
		static_cast<CEikGlobalTextEditor*>(Control(cFileManNewName))->GetText(newName);
		User::LeaveIfError(fileServer.Connect());
		newName.Insert(0, currentPath);
		newName.Append(_L("\\"));
		if (fileServer.MkDirAll(newName) == KErrNone)
			retVal = ETrue;
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRMKDIR);
		fileServer.Close();
	}
	return retVal;
}

/*************************************************************
*
* Fileman attributes dialog
*
**************************************************************/
TKeyResponse CFileManAttrDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cFileAttName))->MoveDisplayL(TCursorPosition::EFPageDown);
			static_cast<CEikGlobalTextEditor*>(Control(cFileAttName))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cFileAttName))->MoveDisplayL(TCursorPosition::EFPageUp);
			static_cast<CEikGlobalTextEditor*>(Control(cFileAttName))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
}

void CFileManAttrDialog::PostLayoutDynInitL()
{
	TEntry fileAtt;
	CEikGlobalTextEditor *textEditor = STATIC_CAST(CEikGlobalTextEditor*, Control(cFileAttName));
	textEditor->CreateScrollBarFrameL();
	textEditor->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);

	if (CEikonEnv::Static()->FsSession().Entry(theFile, fileAtt) == KErrNone)
	{
		TParsePtr parsedFile(theFile);
		textEditor->SetTextL(&(parsedFile.FullName()));
		
		HBufC* dataBufferSize;
		TBuf<2> lineBreak;
		lineBreak.Append(CEditableText::ELineBreak);
		lineBreak.Append(CEditableText::ELineBreak);
		if (!fileAtt.IsDir())
		{
			dataBufferSize = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_PROP_SIZE);
			
			TInt comma = 0;
			TBuf<20> textNumeric;
			textNumeric.Format(_L("%d"), fileAtt.iSize);
			if (fileAtt.iSize > 999)
			{
				textNumeric.Insert(textNumeric.Length() - 3, _L(","));
				comma++;
			}
			if (fileAtt.iSize > 999999)
				textNumeric.Insert(textNumeric.Length() - 3 - comma - 3, _L(","));
			
			dataBufferSize = dataBufferSize->ReAllocL(dataBufferSize->Length() + textNumeric.Length());
			dataBufferSize->Des().Append(textNumeric);
	
			HBufC* dataBufferByte = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_PROP_BYTE);
			dataBufferSize = dataBufferSize->ReAllocL(dataBufferSize->Length() + dataBufferByte->Length());
			dataBufferSize->Des().Append(*dataBufferByte);
			delete dataBufferByte;
		}
		else
			dataBufferSize = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_PROP_ISFOLDER);
					
		TBuf<50> tempTime;
		fileAtt.iModified.FormatL(tempTime, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B  "));
		HBufC* dataBufferLastMod = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_PROP_LASTMOD);
		dataBufferSize = dataBufferSize->ReAllocL(dataBufferSize->Length() + tempTime.Length() + lineBreak.Length() + dataBufferLastMod->Length());
		dataBufferSize->Des().Append(lineBreak);
		dataBufferSize->Des().Append(*dataBufferLastMod);
		delete dataBufferLastMod;
		dataBufferSize->Des().Append(tempTime);
		
		textEditor->InsertDeleteCharsL(textEditor->TextLength(), lineBreak, TCursorSelection(0, 0));
		textEditor->InsertDeleteCharsL(textEditor->TextLength(), *dataBufferSize, TCursorSelection(0, 0));
		textEditor->SetCursorPosL(0, EFalse);
		
		delete dataBufferSize;
		
		if (fileAtt.IsReadOnly())
			static_cast<CEikCheckBox*>(Control(cFileAttReadOnly))->SetState(CEikButtonBase::ESet);
		if (fileAtt.IsArchive())
			static_cast<CEikCheckBox*>(Control(cFileAttArchive))->SetState(CEikButtonBase::ESet);
		if (fileAtt.IsHidden())
			static_cast<CEikCheckBox*>(Control(cFileAttHidden))->SetState(CEikButtonBase::ESet);
		if (fileAtt.IsSystem())
			static_cast<CEikCheckBox*>(Control(cFileAttSystem))->SetState(CEikButtonBase::ESet);
		
		static_cast<CQikTimeAndDateEditor*>(Control(cFileModifiedDateTime))->SetTimeL(fileAtt.iModified);
	}
	else
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FILEMAN_ERRREADATTR);
		textEditor->SetTextL(dataBuffer);
		delete dataBuffer;
	}
}

TBool CFileManAttrDialog::OkToExitL(TInt aButtonId)
{
	TInt attSetFlags = 0;
	TInt attClearFlags = 0;
	
	if (static_cast<CEikCheckBox*>(Control(cFileAttReadOnly))->State() == CEikButtonBase::ESet)
		attSetFlags = attSetFlags | KEntryAttReadOnly;
	else
		attClearFlags = attClearFlags | KEntryAttReadOnly;

	if (static_cast<CEikCheckBox*>(Control(cFileAttHidden))->State() == CEikButtonBase::ESet)
		attSetFlags = attSetFlags | KEntryAttHidden;
	else
		attClearFlags = attClearFlags | KEntryAttHidden;

	if (static_cast<CEikCheckBox*>(Control(cFileAttSystem))->State() == CEikButtonBase::ESet)
		attSetFlags = attSetFlags | KEntryAttSystem;
	else
		attClearFlags = attClearFlags | KEntryAttSystem;

	if (static_cast<CEikCheckBox*>(Control(cFileAttArchive))->State() == CEikButtonBase::ESet)
		attSetFlags = attSetFlags | KEntryAttArchive;
	else
		attClearFlags = attClearFlags | KEntryAttArchive;

	TTime iNewModifiedTime;
	iNewModifiedTime = static_cast<CQikTimeAndDateEditor*>(Control(cFileModifiedDateTime))->Time();
	
	if (aButtonId == EEikBidOk)
	{
		if (CEikonEnv::Static()->FsSession().SetAtt(theFile, attSetFlags, attClearFlags) != KErrNone)
			CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRSETATTRIBS);
		if (CEikonEnv::Static()->FsSession().SetModified(theFile, iNewModifiedTime) != KErrNone)
			CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRSETMODIFIED);
	}
	return ETrue;
}

/*************************************************************
*
* Fn Menu toggling
*
**************************************************************/

CFnMenuToggle::CFnMenuToggle(TUint *bits, TInt *defaultView)
{
	toggleBits = bits;
	theDefaultView = defaultView;
	iDefaultViewIndex = new (ELeave) CArrayFixFlat<TInt>(6);
	iViewIndexes = new (ELeave) CArrayFixFlat<TInt>(6);
}

CFnMenuToggle::~CFnMenuToggle()
{
	delete iDefaultViewIndex;
	delete iViewIndexes;
}

TBool CFnMenuToggle::OkToExitL(TInt aButtonId)
{
	TUint tempBits = 0;
	
	if (aButtonId == EEikBidOk)
	{
		CEikTextListBox *iModuleListBox = (CEikTextListBox*)(Control(cFnModuleList));
		TInt iPowerOfTwo;
		for (int i = 0; i < iModuleListBox->SelectionIndexes()->Count(); i++)
		{
			iPowerOfTwo = 1 << iViewIndexes->At(iModuleListBox->SelectionIndexes()->At(i));
			//iDefaultViewIndex->At(iDefaultViewIndex->Count() - iModuleListBox->SelectionIndexes()->At(i) - 1);
			tempBits = tempBits | iPowerOfTwo;
		}
		if (tempBits > 0)
		{
			*toggleBits = tempBits;
			TInt iIndex = iDefaultViewIndex->Count() - static_cast<CEikChoiceList*>(Control(cFnStartupView))->CurrentItem() - 1;
			if (iIndex > -1)
				*theDefaultView = iDefaultViewIndex->At(iDefaultViewIndex->Count() - static_cast<CEikChoiceList*>(Control(cFnStartupView))->CurrentItem() - 1);
		}
		else
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_NAVIGATION_NOSELECTION);
			return EFalse;
		}
	}
	return ETrue;
}

void CFnMenuToggle::PostLayoutDynInitL()
{
	SynchronizeControls();
}

void CFnMenuToggle::PreLayoutDynInitL()
{
	CEikTextListBox *iModuleListBox = (CEikTextListBox*)(Control(cFnModuleList));
	CSMan2AppUi *iSManApp = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi());
	TInt iPowerOfTwo = 1, i;

	iModuleListBox->CreateScrollBarFrameL();
	iModuleListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	
	// You must delete in reverse order from largest to smallest or the indexing will screw up
	for (i = 0; i < iModuleListBox->Model()->ItemTextArray()->MdcaCount(); i++)
		iViewIndexes->AppendL(i);
	if (!iSManApp->PhoneIsCapable(CSMan2AppUi::EPhone_HasBluetooth))
	{
		((CDesCArray*)iModuleListBox->Model()->ItemTextArray())->Delete(7);
		iViewIndexes->Delete(7);
	}
	if (!iSManApp->PhoneIsCapable(CSMan2AppUi::EPhone_HasNetworkInfoTSY))
	{
		((CDesCArray*)iModuleListBox->Model()->ItemTextArray())->Delete(2);
		iViewIndexes->Delete(2);
	}
	iModuleListBox->HandleItemRemovalL();

	for (i = 0; i < iModuleListBox->Model()->NumberOfItems(); i++)
	{
		iPowerOfTwo = 1 << iViewIndexes->At(i);
		if ((*toggleBits & iPowerOfTwo) > 0)
			iModuleListBox->View()->SelectItemL(i);
	}
	CEikChoiceList *defaultViewChoice = STATIC_CAST(CEikChoiceList*, Control(cFnStartupView));
	defaultViewChoice->SetCurrentItem(*theDefaultView);
}

void CFnMenuToggle::SynchronizeControls()
{
	HBufC *iCurrentSelection = NULL;
	TInt i, iFoundIndexLocation;
	TKeyArrayFix iKeyArrayInt(0, ECmpTInt);
	CEikTextListBox *iModuleListBox = (CEikTextListBox*)(Control(cFnModuleList));
	CEikChoiceList *iStartupList = (CEikChoiceList*)(Control(cFnStartupView));
	
	if (iStartupList->DesCArray()->MdcaCount() > 0)
	{
		iCurrentSelection = HBufC::NewL(iStartupList->DesCArray()->MdcaPoint(iStartupList->CurrentItem()).Length());			
		iCurrentSelection->Des().Copy(iStartupList->DesCArray()->MdcaPoint(iStartupList->CurrentItem()));
	}
	
	CArrayFixFlat<TInt> *iFinalIndexes;
	iFinalIndexes = new (ELeave) CArrayFixFlat<TInt>(6);
	for (i = 0; i < iModuleListBox->SelectionIndexes()->Count(); i++)
		iFinalIndexes->AppendL(iViewIndexes->At(iModuleListBox->SelectionIndexes()->At(i)));
			
	iStartupList->SetArrayL(R_FN_ARRAY);
	for (i = iStartupList->DesCArray()->MdcaCount() - 1; i > -1; i--)
	{
		if (iFinalIndexes->Find(i, iKeyArrayInt, iFoundIndexLocation) != 0)
			iStartupList->DesCArray()->Delete(i);
		else
			iDefaultViewIndex->AppendL(i);
	}
	delete iFinalIndexes;
	
	if (iStartupList->DesCArray()->MdcaCount() == 0)
		iStartupList->SetDimmed(ETrue);
	else
		iStartupList->SetDimmed(EFalse);

	if (iCurrentSelection)
	{
		if (iStartupList->DesCArray()->Find(iCurrentSelection->Des(), iFoundIndexLocation) == 0)
			iStartupList->SetCurrentItem(iFoundIndexLocation);
		delete iCurrentSelection;
	}
	iStartupList->DrawDeferred();
}

void CFnMenuToggle::HandleControlStateChangeL(TInt aControlId)
{
	if (aControlId == cFnModuleList)
		SynchronizeControls();
}

/*************************************************************
*
* CRC progress dialog
*
**************************************************************/

CCRCDialog::CCRCDialog(TPtrC fileName)
{
	theFileName.Set(fileName);
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->crcCalculator->crcDialogActive = ETrue;
}

void CCRCDialog::stopCRC()
{
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->crcCalculator->crcDialogActive = EFalse;
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->crcCalculator->Cancel();
}

CCRCDialog::~CCRCDialog()
{
	stopCRC();
}

void CCRCDialog::PreLayoutDynInitL()
{
	static_cast<CEikProgressInfo*>(Control(cCRCProgress))->SetFinalValue(100);
	static_cast<CEikProgressInfo*>(Control(cCRCProgress))->SetAndDraw(0);
	static_cast<CEikLabel*>(Control(cCRCValue))->SetTextL(_L("Calculating....."));
	TFileName tempCopy;
	tempCopy.Copy(theFileName);
	TParsePtr theParsedFile(tempCopy);
	static_cast<CEikLabel*>(Control(cCRCFileName))->SetTextL(theParsedFile.NameAndExt());
	ButtonGroupContainer().DimCommand(EEikBidOk, ETrue);
}

void CCRCDialog::updateProgress(TInt aProgress)
{
	static_cast<CEikProgressInfo*>(Control(cCRCProgress))->SetAndDraw(aProgress);
}

/*************************************************************
*
* DB progress dialog
*
**************************************************************/

CDBProgressDialog::CDBProgressDialog()
{
}

CDBProgressDialog::~CDBProgressDialog()
{
}

void CDBProgressDialog::PostLayoutDynInitL()
{
	static_cast<CEikProgressInfo*>(Control(cDBCompactProgress))->SetFinalValue(0);
	static_cast<CEikProgressInfo*>(Control(cDBCompactProgress))->SetAndDraw(0);
}

void CDBProgressDialog::updateProgress(TInt aProgress)
{
	static_cast<CEikProgressInfo*>(Control(cDBCompactProgress))->SetAndDraw(aProgress);
}

void CDBProgressDialog::setProgressMaxValue(TInt aMaxValue)
{
	static_cast<CEikProgressInfo*>(Control(cDBCompactProgress))->SetFinalValue(aMaxValue);
}

/*************************************************************
*
* Generic dialog to get password
*
**************************************************************/

CGetPasswordDialog::CGetPasswordDialog(TPassword *thePassword)
{
	localPassword = thePassword;
}

CGetPasswordDialog::~CGetPasswordDialog()
{
}

TBool CGetPasswordDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidOk)
		static_cast<CEikSecretEditor*>(Control(cDBPassword))->GetText(*localPassword);
	return ETrue;
}

/*************************************************************
*
* Generic dialog to change password
*
**************************************************************/

CSetPasswordDialog::CSetPasswordDialog(TPassword *thePassword)
{
	localPassword = thePassword;
}

CSetPasswordDialog::~CSetPasswordDialog()
{
}

TBool CSetPasswordDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidOk)
	{
		TPassword password1, password2;
		static_cast<CEikSecretEditor*>(Control(cDBPassword))->GetText(password1);
		static_cast<CEikSecretEditor*>(Control(cDBPasswordReEnter))->GetText(password2);
		
		if (password1.Compare(password2) == 0)
			static_cast<CEikSecretEditor*>(Control(cDBPassword))->GetText(*localPassword);
		else
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_PASSWORDNOMATCH);
			return EFalse;
		}
	}
	return ETrue;
}

/*************************************************************
*
* Detailed calendar dialog
*
**************************************************************/
TKeyResponse CAgendaDetail::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (ActivePageId() == idAgendaPage_Notes)
	{
		if (aType == EEventKey)
		{
			if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
			{
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->MoveDisplayL(TCursorPosition::EFPageDown);
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->UpdateScrollBarsL();
				return EKeyWasConsumed;
			}
			else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
			{
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->MoveDisplayL(TCursorPosition::EFPageUp);
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->UpdateScrollBarsL();
				return EKeyWasConsumed;
			}
		}
	}
	return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
}

void CAgendaDetail::PostLayoutDynInitL()
{
	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->CreateScrollBarFrameL();
	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->SetCursorPosL(0, EFalse);	
}

void CAgendaDetail::PreLayoutDynInitL()
{
	static_cast<CEikLabel*>(Control(cAgendaDetailStartDateTime))->SetTextL(*startDateTime);
	static_cast<CEikLabel*>(Control(cAgendaDetailEndDateTime))->SetTextL(*endDateTime);
	static_cast<CEikLabel*>(Control(cAgendaDetailAlarmDateTime))->SetTextL(*alarmDateTime);
	static_cast<CEikLabel*>(Control(cAgendaDetailLocation))->SetTextL(*location);

	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->SetTextL(&notes);
}

/*************************************************************
*
* Detailed todo dialog
*
**************************************************************/
TKeyResponse CTodoDetail::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (ActivePageId() == idAgendaPage_Notes)
	{
		if (aType == EEventKey)
		{
			if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
			{
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->MoveDisplayL(TCursorPosition::EFPageDown);
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->UpdateScrollBarsL();
				return EKeyWasConsumed;
			}
			else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
			{
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->MoveDisplayL(TCursorPosition::EFPageUp);
				static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->UpdateScrollBarsL();
				return EKeyWasConsumed;
			}
		}
	}
	return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
}

TBool CTodoDetail::OkToExitL(TInt /*aButtonId*/)
{
	if (static_cast<CEikCheckBox*>(Control(cAgendaDetailTodoDone))->State() == CEikButtonBase::ESet)
		iCrossedOutDate->HomeTime();
	else
		*iCrossedOutDate = Time::NullTTime();
	return ETrue;
}

void CTodoDetail::PostLayoutDynInitL()
{
	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->CreateScrollBarFrameL();
	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->SetCursorPosL(0, EFalse);
}

void CTodoDetail::PreLayoutDynInitL()
{
	if (*iCrossedOutDate != Time::NullTTime())
		static_cast<CEikCheckBox*>(Control(cAgendaDetailTodoDone))->SetState(CEikButtonBase::ESet);
	
	static_cast<CEikLabel*>(Control(cAgendaDetailEndDateTime))->SetTextL(*dueDateTime);
	static_cast<CEikLabel*>(Control(cAgendaDetailAlarmDateTime))->SetTextL(*alarmDateTime);
	static_cast<CEikLabel*>(Control(cAgendaDetailPriority))->SetTextL(*priority);

	static_cast<CEikGlobalTextEditor*>(Control(cAgendaDetailNotes))->SetTextL(&notes);
}

/*************************************************************
*
* Hexeditor GOTO OFFSET dialog
*
**************************************************************/

CHexEditGotoOffset::CHexEditGotoOffset(TInt aMaxOffset, TInt *aSelectedOffset, TInt aCurrentOffset)
{
	iMaxOffset = aMaxOffset;
	iSelectedOffset = aSelectedOffset;
	iCurrentOffset = aCurrentOffset;	
}

void CHexEditGotoOffset::SetHexControlValue(TInt aValue)
{
	TBuf<10> iHexString;
	iHexString.Format(_L("%x"), aValue);
	static_cast<CEikEdwin*>(Control(cHexEditHexOffset))->SetTextL(&iHexString);
	static_cast<CEikEdwin*>(Control(cHexEditHexOffset))->DrawDeferred();
}

void CHexEditGotoOffset::SetDecControlValue(TInt aValue)
{
	static_cast<CQikNumberEditor*>(Control(cHexEditDecOffset))->SetValueL(aValue);
	static_cast<CQikNumberEditor*>(Control(cHexEditDecOffset))->DrawDeferred();
}

void CHexEditGotoOffset::SetSliderControlValue(TInt aValue)
{
	static_cast<CQikSlider*>(Control(cHexEditSliderOffset))->SetValue(aValue);
	static_cast<CQikSlider*>(Control(cHexEditSliderOffset))->DrawDeferred();
}

void CHexEditGotoOffset::HandleControlStateChangeL(TInt aControlId)
{
	if (aControlId == cHexEditSliderOffset)
	{
		TInt iSliderValue = static_cast<CQikSlider*>(Control(cHexEditSliderOffset))->CurrentValue();
		SetDecControlValue(iSliderValue);
		SetHexControlValue(iSliderValue);
	}
}

void CHexEditGotoOffset::PrepareForFocusTransitionL()
{
	if (IdOfFocusControl() == cHexEditHexOffset)
	{
		// This can happen during a backspace
		if (static_cast<CEikEdwin*>(Control(cHexEditHexOffset))->TextLength() == 0)
		{
			TBuf<1> iZero;
			iZero.Copy(_L("0"));
			static_cast<CEikEdwin*>(Control(cHexEditHexOffset))->SetTextL(&iZero);
		}
	}

}

TKeyResponse CHexEditGotoOffset::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse iRetVal = EKeyWasNotConsumed;
	
	if (static_cast<CEikEdwin*>(Control(cHexEditHexOffset))->IsFocused())
	{
		if (((aKeyEvent.iCode >= 48) && (aKeyEvent.iCode <= 57)) || 
		((aKeyEvent.iCode >= 97) && (aKeyEvent.iCode <= 102)) ||
		((aKeyEvent.iCode >= 65) && (aKeyEvent.iCode <= 70)) ||
		(aKeyEvent.iCode == 8))
		{
			static_cast<CEikEdwin*>(Control(cHexEditHexOffset))->OfferKeyEventL(aKeyEvent, aType);
			
			// Convert the hex value and populate the decimal editor control
			TBuf<8> iHexBuffer;
			static_cast<CEikEdwin*>(Control(cHexEditHexOffset))->GetText(iHexBuffer);

			TLex iConverter;
			TInt iOffsetNumber = 0;
			TUint iDummyVar = 0;
			iConverter.Assign(iHexBuffer);
			iConverter.Val(iDummyVar, EHex);
			iOffsetNumber = iDummyVar;
			if (iOffsetNumber > iMaxOffset)
			{
				// The number has overflown. We should reset it back in the edwin
				iOffsetNumber = iMaxOffset;
				SetHexControlValue(iOffsetNumber);
			}
			SetDecControlValue(iOffsetNumber);
			SetSliderControlValue(iOffsetNumber);
			iRetVal = EKeyWasConsumed;
		}
	}
	else if (static_cast<CQikNumberEditor*>(Control(cHexEditDecOffset))->IsFocused())
	{
		iRetVal = static_cast<CQikNumberEditor*>(Control(cHexEditDecOffset))->OfferKeyEventL(aKeyEvent, aType);
		if (iRetVal == EKeyWasConsumed)
		{
			// Convert the dec value and populate the hex editor control. Note, the number editor
			// only supports TInt (no TUint!)
			TInt iOffsetNumber = static_cast<CQikNumberEditor*>(Control(cHexEditDecOffset))->Value();
			
			if (iOffsetNumber > iMaxOffset)
			{
				// The number has overflown. We should reset it back in the number editor
				iOffsetNumber = iMaxOffset;
				SetDecControlValue(iOffsetNumber);
			}
			SetHexControlValue(iOffsetNumber);
			SetSliderControlValue(iOffsetNumber);
		}
	}
	return iRetVal;
}

void CHexEditGotoOffset::PreLayoutDynInitL()
{
	static_cast<CQikSlider*>(Control(cHexEditSliderOffset))->SetMaxValueL(iMaxOffset);
	SetHexControlValue(iCurrentOffset);
	SetDecControlValue(iCurrentOffset);
	static_cast<CQikNumberEditor*>(Control(cHexEditDecOffset))->SetMinimumAndMaximum(0, iMaxOffset);
	SetSliderControlValue(iCurrentOffset);	
}


void CHexEditGotoOffset::PostLayoutDynInitL()
{
	// This is how you remove the Cut/Copy/Paste info button that appears on a dialog's title bar
	// when there is an edwin in the dialog. Luckily, no illegal hacks. Phew :)
	Title().SetExpectNoPointerEvents(ETrue);
	
	if (iMaxOffset == 0)
		static_cast<CQikSlider*>(Control(cHexEditSliderOffset))->SetDimmed(ETrue);
}

TBool CHexEditGotoOffset::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
		*iSelectedOffset = static_cast<CQikNumberEditor*>(Control(cHexEditDecOffset))->Value();
	return ETrue;
}

/*************************************************************
*
* Hexeditor window resize dialog
*
**************************************************************/

CHexEditResize::CHexEditResize(TInt *aCurrentSize)
{
	iCurrentSize = aCurrentSize;
}

void CHexEditResize::PreLayoutDynInitL()
{
	static_cast<CQikSlider*>(Control(cHexEditSliderResize))->SetValue(*iCurrentSize);
}

TBool CHexEditResize::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
		*iCurrentSize = static_cast<CQikSlider*>(Control(cHexEditSliderResize))->CurrentValue();
	return ETrue;
}

/*************************************************************
*
* FindFile criteria dialog
*
**************************************************************/

CFindFileCriteria::CFindFileCriteria(CDesCArray *aFoldersToScan, TInt *aModifiedDateOption, TTime *aFromTime, TTime *aToTime, TFileName *aSearchWildCard, TFileName *aHighlightedFolder)
{
	iLocalFoldersToScan = aFoldersToScan;
	iLocalModifiedDateOption = aModifiedDateOption;
	iLocalFromTime = aFromTime;
	iLocalToTime = aToTime;
	iLocalSearchWildCard = aSearchWildCard;
	iLocalHighlightedFolder = aHighlightedFolder;
}

void CFindFileCriteria::PreLayoutDynInitL()
{
	static_cast<CEikTextListBox*>(Control(cFindFileLocation))->CreateScrollBarFrameL();
	static_cast<CEikTextListBox*>(Control(cFindFileLocation))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	
	//DEBUG static_cast<CEikGlobalTextEditor*>(Control(cFindFileSearchName))->SetTextL(iLocalSearchWildCard);
	static_cast<CEikComboBox*>(Control(cFindFileSearchName))->SetTextL(iLocalSearchWildCard);
	static_cast<CEikComboBox*>(Control(cFindFileSearchName))->SetArrayL(R_FINDFILE_CRITERIA_ARRAY);
	
	if (*iLocalModifiedDateOption == 3)
	{
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->SetTimeL(*iLocalFromTime);
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->SetTimeL(*iLocalToTime);
	}
	else
	{
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->SetDimmed(ETrue);
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->SetDimmed(ETrue);
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->SetTimeL(*iLocalFromTime);
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->SetTimeL(*iLocalToTime);
	}
	static_cast<CEikChoiceList*>(Control(cFindFileModifiedOption))->SetCurrentItem(*iLocalModifiedDateOption);

	CEikTextListBox *iLocationListBox = static_cast<CEikTextListBox*>(Control(cFindFileLocation));
	((CDesCArray *)iLocationListBox->Model()->ItemTextArray())->Reset();
	if (iLocalHighlightedFolder->Length() > 0)
		((CDesCArray *)static_cast<CEikTextListBox*>(Control(cFindFileLocation))->Model()->ItemTextArray())->AppendL(*iLocalHighlightedFolder);
	
	// Populate drive letters
	TVolumeInfo iVolumeInfo;
	TBuf<3> iDriveText;
	TInt i;
	
	for (i = 0; i < KMaxDrives; i++)
	{
		if (CEikonEnv::Static()->FsSession().Volume(iVolumeInfo, EDriveA + i) == KErrNone)
		{
			iDriveText.Format(_L("%c:\\"), 0x41 + i);
			((CDesCArray *)iLocationListBox->Model()->ItemTextArray())->AppendL(iDriveText);
		}
	}
	iLocationListBox->HandleItemAdditionL();
	
	// Check the relevant items.....
	iLocationListBox->ClearSelection();
	for (i = 0; i < iLocalFoldersToScan->Count(); i++)
	{
		for (TInt j = 0; j < iLocationListBox->Model()->ItemTextArray()->MdcaCount(); j++)
		{
			if (iLocalFoldersToScan->MdcaPoint(i).Compare(iLocationListBox->Model()->ItemTextArray()->MdcaPoint(j)) == 0)
				iLocationListBox->View()->SelectItemL(j);
		}
	}
	
	// Uncheck the highlighted folder
	if (iLocalHighlightedFolder->Length() > 0)
		iLocationListBox->View()->DeselectItem(0);
}

TBool CFindFileCriteria::OkToExitL(TInt aButtonId)
{
	TBool iRetVal = ETrue;
	
	if (aButtonId == EEikBidYes)
	{
		TTime iTime = static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->Time();
		TDateTime iOnlyDate;
		iOnlyDate = iTime.DateTime();
		iOnlyDate.SetHour(0);
		iOnlyDate.SetMinute(0);
		iOnlyDate.SetSecond(0);
		iOnlyDate.SetMicroSecond(0);
		iTime = TTime(iOnlyDate);
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->SetTimeL(iTime);

		iTime = static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->Time();
		iOnlyDate = iTime.DateTime();
		iOnlyDate.SetHour(23);
		iOnlyDate.SetMinute(59);
		iOnlyDate.SetSecond(59);
		iOnlyDate.SetMicroSecond(999999);
		iTime = TTime(iOnlyDate);
		static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->SetTimeL(iTime);
	
		if ((static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->Time() > static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->Time()) &&
			(static_cast<CEikChoiceList*>(Control(cFindFileModifiedOption))->CurrentItem() == 3))
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_FINDFILE_TOPRECEEDSFROM);
			iRetVal = EFalse;
		}
		else if (static_cast<CEikTextListBox*>(Control(cFindFileLocation))->SelectionIndexes()->Count() == 0)
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_FINDFILE_NOLOCATION);
			iRetVal = EFalse;
		}
		else
		{
			TFileName iTempSearchWildCards;
			TParse iParsedSearchWildCards;

			//DEBUG static_cast<CEikGlobalTextEditor*>(Control(cFindFileSearchName))->GetText(iTempSearchWildCards);
			static_cast<CEikComboBox*>(Control(cFindFileSearchName))->GetText(iTempSearchWildCards);
			if (iParsedSearchWildCards.Set(iTempSearchWildCards, NULL, NULL) == KErrNone)
			{
				// > " / | are all illegal chars in filenames
				if ((iParsedSearchWildCards.DrivePresent()) ||
					(iParsedSearchWildCards.PathPresent()) ||
					(!iParsedSearchWildCards.NamePresent()) ||
					(iTempSearchWildCards.Find(_L(">")) > 0) ||
					(iTempSearchWildCards.Find(_L("\"")) > 0) ||
					(iTempSearchWildCards.Find(_L("\\")) > 0) ||
					(iTempSearchWildCards.Find(_L("|")) > 0))
				{
					CEikonEnv::Static()->InfoMsg(R_TBUF_FINDFILE_INVALIDSEARCHNAME);
					iRetVal = EFalse;
				}
			}
		}
		
		if (iRetVal)
		{
			*iLocalModifiedDateOption = static_cast<CEikChoiceList*>(Control(cFindFileModifiedOption))->CurrentItem();
			switch (*iLocalModifiedDateOption)
			{
				case 1 : 
						{
							TTime iTime;
							iTime.HomeTime();
							*iLocalToTime = iTime;
							iTime -= TTimeIntervalDays(1);
							*iLocalFromTime = iTime;
							break;
						}
				case 2 :
						{
							TTime iTime;
							iTime.HomeTime();
							*iLocalToTime = iTime;
							iTime -= TTimeIntervalDays(2);
							*iLocalFromTime = iTime;
							break;
						}
				case 3 :
						{
							*iLocalToTime = (static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->Time());
							*iLocalFromTime =(static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->Time());

							iOnlyDate = iLocalFromTime->DateTime();
							iOnlyDate.SetHour(0);
							iOnlyDate.SetMinute(0);
							iOnlyDate.SetSecond(0);
							iOnlyDate.SetMicroSecond(0);
							*iLocalFromTime = TTime(iOnlyDate);

							iOnlyDate = iLocalToTime->DateTime();
							iOnlyDate.SetHour(23);
							iOnlyDate.SetMinute(59);
							iOnlyDate.SetSecond(59);
							iOnlyDate.SetMicroSecond(999999);
							*iLocalToTime = TTime(iOnlyDate);
							break;
						}
			}
			//DEBUG static_cast<CEikGlobalTextEditor*>(Control(cFindFileSearchName))->GetText(*iLocalSearchWildCard);
			static_cast<CEikComboBox*>(Control(cFindFileSearchName))->GetText(*iLocalSearchWildCard);
			iLocalFoldersToScan->Reset();
			CEikTextListBox *iLocationListBox = static_cast<CEikTextListBox*>(Control(cFindFileLocation));
			for (int i = 0; i < iLocationListBox->SelectionIndexes()->Count(); i++)
				iLocalFoldersToScan->AppendL(iLocationListBox->Model()->ItemText(iLocationListBox->SelectionIndexes()->At(i)));
		}
	}		
	return iRetVal;
}

void CFindFileCriteria::AutoCompleteOtherLocation()
{
}

void CFindFileCriteria::HandleControlStateChangeL(TInt aControlId)
{
	if (aControlId == cFindFileModifiedOption)
	{
		if (static_cast<CEikChoiceList*>(Control(cFindFileModifiedOption))->CurrentItem() != 3)
		{
			static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->SetDimmed(ETrue);
			static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->SetDimmed(ETrue);
		}
		else
		{
			static_cast<CQikDateEditor*>(Control(cFindFileModifiedFromDate))->SetDimmed(EFalse);
			static_cast<CQikDateEditor*>(Control(cFindFileModifiedToDate))->SetDimmed(EFalse);
		}
	}
}

/*************************************************************
*
* Font selector dialog
*
**************************************************************/

CFontSelector::CFontSelector()
{
	_LIT(KFontPreviewText, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	iRichText = CRichText::NewL(CParaFormatLayer::NewL(), CCharFormatLayer::NewL());
	iRichText->InsertL(0, KFontPreviewText);

	// Bold font
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	charFormatMask.ClearAll();
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
	charFormatMask.SetAttrib(EAttFontStrokeWeight);
	iRichText->ApplyCharFormatL(charFormat, charFormatMask, 0, 13);
	iRichText->ApplyCharFormatL(charFormat, charFormatMask, 27, 13);
	iRichText->ApplyCharFormatL(charFormat, charFormatMask, 52, 5);
}

CFontSelector::~CFontSelector()
{
	delete iRichText;
}

/*** Following section of code taken from my-symbian.com *****/
/* Some minor modifications made to it */

CDictionaryStore* CFontSelector::OpenFontIniFileLC(TBool aResetFile)
{ 
	RFs& fs = CEikonEnv::Static()->FsSession();
	
	if (aResetFile)
		fs.Delete(SYSTEMFONT_INIFILE);
		
	// open the ini file if it exists, otherwise create a new one
	CDictionaryStore* iniFile = NULL;

	// if first attempt to open ini fails because of corrupt file, delete it and try again.
	TRAPD(err, iniFile = CDictionaryFileStore::OpenL(fs, SYSTEMFONT_INIFILE, KSystemFontStoreUid));
	if (err == KErrNone)
		CleanupStack::PushL(iniFile);
	else if ((err == KErrEof) || (err == KErrCorrupt))
	{ 
		User::LeaveIfError(fs.Delete(SYSTEMFONT_INIFILE));
		iniFile = CDictionaryFileStore::OpenLC(fs, SYSTEMFONT_INIFILE, KSystemFontStoreUid); 
		err = KErrNone; 
	}
	User::LeaveIfError(err); 
	return iniFile;
} 

void CFontSelector::WriteAntiAliasingStateToIniFileL(TBool aUseAntiAliasing)
{ 
	// Open the Ini-file 
	CDictionaryStore* iniFile = OpenFontIniFileLC(EFalse); 
	User::LeaveIfNull(iniFile); 

	// The stream containing the preference will be associated 
	// with a Uid. AssignL() creates the stream if it does not exist 
	RDictionaryWriteStream writeStream; 
	writeStream.AssignLC(*iniFile,  KSystemFontAntiAliasingStreamUid); 

	// Externalize data 
	writeStream.WriteInt32L(aUseAntiAliasing); 
	writeStream.CommitL(); 

	CleanupStack::PopAndDestroy();   // writeStream 

	// Commit changes to the store if no errors, otherwise revert the ini-file 
	if (iniFile->Commit() != KErrNone) 
		iniFile->RevertL(); 

	// Can now delete the "ini" file object (i.e. close the "ini" file) 
	CleanupStack::PopAndDestroy();   // iniFile 
} 

void CFontSelector::WriteSystemFontNameAndSizeToIniFileL(const TDesC& aFontName, TInt aSizeInPixel)
{ 
	// Open the Ini-file 
	CDictionaryStore* iniFile = OpenFontIniFileLC(ETrue); 
	User::LeaveIfNull(iniFile); 

	// The stream containing the preference will be associated 
	// with a Uid. AssignL() creates the stream if it does not exist 
	RDictionaryWriteStream writeStream; 
	writeStream.AssignLC(*iniFile, KSystemFontNameAndSizeStreamUid); 

	// Externalize data 
	TBuf<KMaxTypefaceNameLength> fontName(aFontName); 
	writeStream.WriteInt32L(fontName.Length()); 
	writeStream.WriteL(fontName);
	writeStream.WriteInt32L(aSizeInPixel); 
	writeStream.CommitL(); 

	CleanupStack::PopAndDestroy();   // writeStream 

	// Commit changes to the store if no errors, otherwise revert the ini-file 
	if (iniFile->Commit() != KErrNone) 
		iniFile->RevertL(); 

	// Can now delete the "ini" file object (i.e. close the "ini" file) 
	CleanupStack::PopAndDestroy();   // iniFile 
}

/*** end of code snippet *******************************/

TBool CFontSelector::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		TBuf<KMaxTypefaceNameLength> iFontName;
		CEikChoiceList *iFontChoiceList = static_cast<CEikChoiceList*>(Control(cFontSelectorFontList));
		iFontName.Copy(iFontChoiceList->DesCArray()->MdcaPoint(iFontChoiceList->CurrentItem()));
		WriteSystemFontNameAndSizeToIniFileL(iFontName, static_cast<CQikNumberEditor*>(Control(cFontSelectorFontSize))->Value());
		//CEikonEnv::Static()->NormalFont()->HeightInPixels());
		if (static_cast<CEikCheckBox*>(Control(cFontSelectorAntiAlias))->State() == CEikButtonBase::ESet)
			WriteAntiAliasingStateToIniFileL(ETrue);
		else
			WriteAntiAliasingStateToIniFileL(EFalse);
		CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_MISC_REBOOT);
		return ETrue;
	}
	else if (aButtonId == EEikBidOk)
	{
		EikFileUtils::DeleteFile(SYSTEMFONT_INIFILE);
		CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_MISC_REBOOT);
		return ETrue;
	}
	else
		return EFalse;
}

TKeyResponse CFontSelector::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview))->MoveDisplayL(TCursorPosition::EFLineDown);
			static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview))->MoveDisplayL(TCursorPosition::EFLineUp);
			static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
}

void CFontSelector::PostLayoutDynInitL()
{
	SetPreviewWindowFont();
}

void CFontSelector::PreLayoutDynInitL()
{	
	CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
	
	static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview))->CreateScrollBarFrameL();
	static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

	CCoeEnv *iEnv = CCoeEnv::Static();
	CDesCArray *iFontList = new (ELeave) CDesCArrayFlat(4);
	FontUtils::GetAvailableFontsL(*iEnv->SystemGc().Device(), *iFontList, EGulNoSymbolFonts);
	iFontList->Sort();

	CDesCArray *iFontChoiceListArray = (CDesCArray *)static_cast<CEikChoiceList*>(Control(cFontSelectorFontList))->DesCArray();
	iFontChoiceListArray->Reset();
	for (i = 0; i < iFontList->Count(); i++)
		iFontChoiceListArray->AppendL(iFontList->MdcaPoint(i));
	delete iFontList;

	//_LIT(KFontPreviewText, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	//static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview))->SetTextL(&KFontPreviewText);

	TBool iIsAntiAlias = EFalse;
	TInt iFontSize = CEikonEnv::Static()->NormalFont()->HeightInPixels();

	// Read the existing font name and set it, provided SysFnt.ini exists...
	if (ConeUtils::FileExists(SYSTEMFONT_INIFILE))
	{
		CDictionaryStore *iniFile = NULL;
		iniFile = OpenFontIniFileLC(EFalse);
		if (iniFile)
		{			
			RDictionaryReadStream readStream;
			readStream.OpenLC(*iniFile, KSystemFontNameAndSizeStreamUid);

			// Internalize data for font name
			TInt iNameLength;
			iNameLength = readStream.ReadInt32L();
			iFontName.SetLength(iNameLength);
			readStream.ReadL(iFontName, iNameLength);
			iFontSize = readStream.ReadInt32L();
			readStream.Close();
			CleanupStack::PopAndDestroy();   // readStream
			
			readStream.OpenLC(*iniFile, KSystemFontAntiAliasingStreamUid);
			// Internalize data for anti aliasing
			TRAPD(err, iIsAntiAlias = readStream.ReadInt32L());
			readStream.Close();
			CleanupStack::PopAndDestroy();   // readStream
			CleanupStack::PopAndDestroy();   // iniFile 
		}
	}
	else
	{
		// Else, read from the current system font
		TFontSpec iCurrentFontSpec = CEikonEnv::Static()->NormalFont()->FontSpecInTwips();
		iFontName.Copy(iCurrentFontSpec.iTypeface.iName);
	}
	
	// Now, highlight the appropriate entry
	for (i = 0; i < iFontChoiceListArray->MdcaCount(); i++)
	{
		if (iFontChoiceListArray->MdcaPoint(i).Compare(iFontName) == 0)
		{
			static_cast<CEikChoiceList*>(Control(cFontSelectorFontList))->SetCurrentItem(i);
			break;
		}
	}
	// Set anti-alias checkbox
	if (iIsAntiAlias)
		static_cast<CEikCheckBox*>(Control(cFontSelectorAntiAlias))->SetState(CEikButtonBase::ESet);
	else
		static_cast<CEikCheckBox*>(Control(cFontSelectorAntiAlias))->SetState(CEikButtonBase::EClear);
		
	// Set font size
	static_cast<CQikNumberEditor*>(Control(cFontSelectorFontSize))->SetValueL(iFontSize);
	
	CEikonEnv::Static()->BusyMsgCancel();	
}	

void CFontSelector::SetPreviewWindowFont()
{
	CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
	
	// Save the current font name so we don't end up releasing it!
	TFontSpec iCurrentFontSpec = CEikonEnv::Static()->NormalFont()->FontSpecInTwips();
	TBuf<KMaxTypefaceNameLength> iCurrentFontName;
	iCurrentFontName.Copy(iCurrentFontSpec.iTypeface.iName);

	// Get typeface name
	CEikChoiceList *iFontChoiceList = static_cast<CEikChoiceList*>(Control(cFontSelectorFontList));
	iFontName.Copy(iFontChoiceList->DesCArray()->MdcaPoint(iFontChoiceList->CurrentItem()));
	CGraphicsDevice* iScreenDevice = CCoeEnv::Static()->ScreenDevice();
	TInt iPreviewFontHeightInTwips = iScreenDevice->PixelsToTwips(TPoint(0, static_cast<CQikNumberEditor*>(Control(cFontSelectorFontSize))->Value())).iY;
	CFont* iFont;
	TFontSpec iFontSpec(iFontName, iPreviewFontHeightInTwips);
	iScreenDevice->GetNearestFontInTwips(iFont, iFontSpec);

	//_LIT(KFontPreviewText, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	//CRichText *iRichText = CRichText::NewL(CParaFormatLayer::NewL(), CCharFormatLayer::NewL());
	//iRichText->InsertL(0, KFontPreviewText);

	CEikRichTextEditor *iRichTextEditor = static_cast<CEikRichTextEditor*>(Control(cFontSelectorPreview));	
	
	// Set the preview window's font
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	TTypeface iFontTypeFace;
	iFontTypeFace.iName = iFontName;
	charFormat.iFontSpec.iTypeface = iFontTypeFace;
	charFormat.iFontSpec.iHeight = iPreviewFontHeightInTwips;
	charFormatMask.ClearAll();
	charFormatMask.SetAttrib(EAttFontTypeface);
	charFormatMask.SetAttrib(EAttFontHeight);
	iRichText->ApplyCharFormatL(charFormat, charFormatMask, 0, iRichText->DocumentLength());

	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;
	paraFormat.iLineSpacingInTwips = iFont->FontSpecInTwips().iHeight;
	paraFormat.iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInTwips;
	paraFormatMask.ClearAll();
	paraFormatMask.SetAttrib(EAttLineSpacing);
	paraFormatMask.SetAttrib(EAttLineSpacingControl);
	iRichText->ApplyParaFormatL(&paraFormat, paraFormatMask, 0, iRichText->DocumentLength());
	
	iRichTextEditor->SetDocumentContentL((CGlobalText&)*iRichText);
	
	// Some simple cleanup to get visually correct results
	iRichTextEditor->SetCursorPosL(0, EFalse);
	iRichTextEditor->DrawDeferred();
	
	// Release the font if it isn't the current system font or we'll end up with
	// unpredictable views!
	if (iCurrentFontName.Compare(iFontName) != 0)
		iScreenDevice->ReleaseFont(iFont);
		
	CEikonEnv::Static()->BusyMsgCancel();
}

void CFontSelector::HandleControlStateChangeL(TInt aControlId)
{
	if ((aControlId == cFontSelectorFontList) || (aControlId == cFontSelectorFontSize))
		SetPreviewWindowFont();
}

/*************************************************************
*
* Credits dialog
*
**************************************************************/

void CCreditsDialog::PreLayoutDynInitL()
{
	HBufC* iDataBuffer = CEikonEnv::Static()->AllocReadResourceL(iMessageResourceID);
	TInt i = 0, iLength;
	TPtrC iTextPointer;	
	while (TextUtils::ColumnText(iTextPointer, i, iDataBuffer, TChar(0x2a)) == KErrNone)	// 0x2a = *
	{
		iLength = static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->TextLength();
		static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->Text()->InsertL(iLength, iTextPointer);
		iLength = static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->TextLength();
		static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->Text()->InsertL(iLength, CEditableText::ELineBreak);
		i++;
	}
	delete iDataBuffer;
}

void CCreditsDialog::PostLayoutDynInitL()
{
	static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->CreateScrollBarFrameL();
	static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
}

TKeyResponse CCreditsDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->MoveDisplayL(TCursorPosition::EFLineDown);
			static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->MoveDisplayL(TCursorPosition::EFLineUp);
			static_cast<CEikGlobalTextEditor*>(Control(cCreditsText))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
}

/*************************************************************
*
* Dialog to edit group info, cell info, events
*
**************************************************************/

CEditCellDBDialog::CEditCellDBDialog(CDBHelper *iCellDBHelper)
{
	iLocalCellDBHelper = iCellDBHelper;
	iGroupIds = new (ELeave) CArrayFixFlat<TUint>(3);
	iGroupPriorities = new (ELeave) CArrayFixFlat<TUint>(3);
	iEventIds = new (ELeave) CArrayFixFlat<TUint>(3);
	iEventTypeIds = new (ELeave) CArrayFixFlat<TUint>(3);
	iEventTriggerIds = new (ELeave) CArrayFixFlat<TUint>(3);
	iEventPriorities = new (ELeave) CArrayFixFlat<TUint>(3);
	iEventDesc = new (ELeave) CDesCArrayFlat(3);
	iEventTriggerDesc = new (ELeave) CDesCArrayFlat(3);
	iGroupStates = new (ELeave) CArrayFixFlat<TInt8>(3);
	iGroupNames = new (ELeave) CDesCArrayFlat(3);
	iGroupInfoOnCellsChanged = EFalse;
	iGroupInfoOnEventsChanged = EFalse;
	iGroupInfoOnImportExportChanged = EFalse;
	
	// Read iEventsDesc from the resource file
	CDesCArrayFlat *iTempEventArray = CEikonEnv::Static()->ReadDesCArrayResourceL(R_EVENT_TYPE_ARRAY);
	TPtrC iTextColumn, iNumberColumn;
	TBuf<50> iTextBuffer;
	TBuf<10> iNumberBuffer;
	TLex iLexConverter;
	TInt iTempEventTypeNumber;
	for (int i = 0; i < iTempEventArray->MdcaCount(); i++)
	{
		iTextBuffer.Copy(iTempEventArray->MdcaPoint(i));
		TextUtils::ColumnText(iTextColumn, 0, &iTextBuffer, TChar(';'));
		TextUtils::ColumnText(iNumberColumn, 1, &iTextBuffer, TChar(';'));
		
		iNumberBuffer.Copy(iNumberColumn);
		iTextBuffer.Copy(iTextColumn);
		iLexConverter.Assign(iNumberBuffer);
		iLexConverter.Val(iTempEventTypeNumber);
		
		// At this point, iTextBuffer and iTempEventTypeNumber are both ready
		iEventTypeIds->AppendL(iTempEventTypeNumber);
		iEventDesc->AppendL(iTextBuffer);
	}
	delete iTempEventArray;

	// Read iEventsTriggerDesc from the resource file
	iTempEventArray = CEikonEnv::Static()->ReadDesCArrayResourceL(R_EVENT_TYPE_TRIGGER);
	for (int i = 0; i < iTempEventArray->MdcaCount(); i++)
	{
		iTextBuffer.Copy(iTempEventArray->MdcaPoint(i));
		TextUtils::ColumnText(iTextColumn, 0, &iTextBuffer, TChar(';'));
		TextUtils::ColumnText(iNumberColumn, 1, &iTextBuffer, TChar(';'));
		
		iNumberBuffer.Copy(iNumberColumn);
		iTextBuffer.Copy(iTextColumn);
		iLexConverter.Assign(iNumberBuffer);
		iLexConverter.Val(iTempEventTypeNumber);
		
		// At this point, iTextBuffer and iTempEventTypeNumber are both ready
		iEventTriggerIds->AppendL(iTempEventTypeNumber);
		iEventTriggerDesc->AppendL(iTextBuffer);
	}
	delete iTempEventArray;
}

CEditCellDBDialog::~CEditCellDBDialog()
{
	delete iGroupIds;
	delete iGroupPriorities;
	delete iGroupNames;
	delete iGroupStates;
	delete iEventIds;
	delete iEventDesc;
	delete iEventTypeIds;
	delete iEventPriorities;
	delete iEventTriggerDesc;
	delete iEventTriggerIds;
}

TBool CEditCellDBDialog::ReadGroupInfo()
{
	TBool iRetVal = EFalse;
	
	iLocalCellDBHelper->iSQLStatement.Copy(_L("SELECT Group_Id, Group_Name, State, Priority FROM Groups_List ORDER BY Priority"));
	iGroupIds->Reset();
	iGroupNames->Reset();
	iGroupStates->Reset();
	iGroupPriorities->Reset();
	
	if (iLocalCellDBHelper->ReadDB())
	{
		//for (iLocalCellDBHelper->iDBView.FirstL(); iLocalCellDBHelper->iDBView.AtRow(); iLocalCellDBHelper->iDBView.NextL())
		while (iLocalCellDBHelper->iDBView.NextL())
		{
			iLocalCellDBHelper->iDBView.GetL();
			iGroupIds->AppendL(iLocalCellDBHelper->iDBView.ColUint32(1));
			iGroupNames->AppendL(iLocalCellDBHelper->iDBView.ColDes(2));
			iGroupStates->AppendL(iLocalCellDBHelper->iDBView.ColInt8(3));
			iGroupPriorities->AppendL(iLocalCellDBHelper->iDBView.ColUint32(4));
		}
		iLocalCellDBHelper->iDBView.Close();
		iRetVal = ETrue;
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRREADGROUPS);
		
	iGroupIds->Compress();
	iGroupNames->Compress();
	iGroupStates->Compress();
	iGroupPriorities->Compress();

	iGroupInfoOnCellsChanged = iRetVal;
	iGroupInfoOnEventsChanged = iRetVal;
	iGroupInfoOnImportExportChanged = iRetVal;
	return iRetVal;
}

// aInitGroupList = Refresh the GroupList control's array?
// aCellId = if > 0, after the page is initialized, set the cell list control to focus on this cell
//
// aMaintainSelectionIndex = maintain the selection indexes?
// This parameter is only set to ETrue when moving cells from one group to another. During the
// move, items which get successfully moved across will be UNCHECKED. That means by the time
// execution reaches InitCellsPage(), all items in SelectionIndexes() are those which need to be
// maintained. This saves processing time when doing a search.
TBool CEditCellDBDialog::InitCellsPage(TBool aInitGroupList, TUint aCellId, TBool aMaintainSelectionIndex)
{
	TBool iRetVal = EFalse;
	TInt iIndexToFocusOn = 0;
	CArrayFixFlat<TUint> *iSelectedCells = NULL;
	CArrayFixFlat<TInt>* iNewSelectionIndex = NULL;
	TUint iCellId;
	TInt iDummy;
	TKeyArrayFix iArrayFix = TKeyArrayFix(0, ECmpTUint);
	
	CEikColumnListBox *iCellsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox));
	CDesCArray *iCellsListBoxArray = (CDesCArray *)iCellsListBox->Model()->ItemTextArray();

	if ((iCellsListBox->SelectionIndexes()->Count() > 0) && (aMaintainSelectionIndex))
	{
		TPtrC iCellRow, iCellIdText;
		TLex iLexConverter;

		iSelectedCells = new (ELeave) CArrayFixFlat<TUint>(1);
		iNewSelectionIndex = new (ELeave) CArrayFixFlat<TInt>(1);
		// We recycle the iIndexToFocusOn variable
		for (iIndexToFocusOn = 0; iIndexToFocusOn < iCellsListBox->SelectionIndexes()->Count(); iIndexToFocusOn++)
		{
			iCellRow.Set(iCellsListBoxArray->MdcaPoint(iCellsListBox->SelectionIndexes()->At(iIndexToFocusOn)));
			TextUtils::ColumnText(iCellIdText, 0, &iCellRow);
			iLexConverter.Assign(iCellIdText);
			iLexConverter.Val(iCellId);
			iSelectedCells->AppendL(iCellId);
		}
	}

	iCellsListBox->ClearSelection();
	iCellsListBoxArray->Reset();
	iCellsListBox->HandleItemRemovalL();
	
	CEikChoiceList *iGroupList = static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnCellsPage));
	if (aInitGroupList)
	{
		iGroupList->SetArrayL(iGroupNames);
		iGroupList->SetArrayExternalOwnership(ETrue);
		iGroupInfoOnCellsChanged = EFalse;
	}
	
	if (iGroupList->DesCArray()->Count() > 0)
	{
		// Find all cells that belong to this group
		iLocalCellDBHelper->iSQLStatement.Format(_L("SELECT Cell_Id, Country_Code, Network_Id, Description FROM Cells_List WHERE Group_Id = %u ORDER BY Cell_Id"), iGroupIds->At(iGroupList->CurrentItem()));
		if (iLocalCellDBHelper->ReadDB())
		{
			HBufC *iCellText = HBufC::NewL(10 + 10 + 8 + 4 + 2 + 50);
			while (iLocalCellDBHelper->iDBView.NextL())
			{
				iLocalCellDBHelper->iDBView.GetL();
				iCellText->Des().Copy(_L(""));
				iCellId = iLocalCellDBHelper->iDBView.ColUint32(1);
				iCellText->Des().AppendNum(iCellId, 10);
				iCellText->Des().Append(KColumnListSeparator);
				iCellText->Des().Append(iLocalCellDBHelper->iDBView.ColDes(2));
				iCellText->Des().Append(KColumnListSeparator);
				iCellText->Des().Append(iLocalCellDBHelper->iDBView.ColDes(3));
				iCellText->Des().Append(KColumnListSeparator);
				iCellText->Des().Append(iLocalCellDBHelper->iDBView.ColDes(4));
				iCellsListBoxArray->AppendL(*iCellText);
				if ((aCellId > 0) && (iLocalCellDBHelper->iDBView.ColUint32(1) == aCellId))
					iIndexToFocusOn = iCellsListBoxArray->Count() - 1;
				
				if ((iSelectedCells) && (aMaintainSelectionIndex))
				{
					if (iSelectedCells->Find(iCellId, iArrayFix, iDummy) == 0)
						iNewSelectionIndex->AppendL(iCellsListBoxArray->Count() - 1);
				}
			}
			iLocalCellDBHelper->iDBView.Close();
			delete iCellText;
			iRetVal = ETrue;
		}
	}
	iCellsListBox->HandleItemRemovalL();

	if (iGroupNames->MdcaCount() < 1)
		iGroupList->SetDimmed(ETrue);
	else
		iGroupList->SetDimmed(EFalse);
		
	if (iCellsListBoxArray->Count() > 0)
		iCellsListBox->SetDimmed(EFalse);
	else
		iCellsListBox->SetDimmed(ETrue);
		
	UpdatePageButtons_Cells();
	
	if (aCellId > 0)
		iCellsListBox->SetCurrentItemIndexAndDraw(iIndexToFocusOn);

	if (iSelectedCells)
	{
		iCellsListBox->SetSelectionIndexesL(iNewSelectionIndex);
		delete iSelectedCells;
		delete iNewSelectionIndex;
	}		
	return iRetVal;
}

void CEditCellDBDialog::InitImportExportPage(TBool aInitGroupList)
{
	CEikTextListBox *iGroupsListBox = static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox));
	iGroupsListBox->ClearSelection();
	iGroupsListBox->HandleItemRemovalL();
	
	if (aInitGroupList)
	{
		iGroupsListBox->Model()->SetItemTextArray(iGroupNames);
		iGroupsListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
		iGroupInfoOnImportExportChanged = EFalse;
	}	
	iGroupsListBox->HandleItemRemovalL();
	
	iGroupsListBox->UpdateScrollBarsL();
	
	if (iGroupNames->MdcaCount() < 1)
		iGroupsListBox->SetDimmed(ETrue);
	else
		iGroupsListBox->SetDimmed(EFalse);
		
	UpdatePageButtons_ImportExport();
}

TBool CEditCellDBDialog::InitEventsPage(TBool aInitGroupList)
{
	TBool iRetVal = EFalse;

	CEikColumnListBox *iEventsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
	CDesCArray *iEventsListBoxArray = (CDesCArray *)iEventsListBox->Model()->ItemTextArray();
	iEventsListBox->ClearSelection();
	iEventsListBoxArray->Reset();
	iEventsListBox->HandleItemRemovalL();
	iEventIds->Reset();
	iEventPriorities->Reset();

	CEikChoiceList *iGroupList = static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnEventsPage));
	if (aInitGroupList)
	{
		iGroupList->SetArrayL(iGroupNames);
		iGroupList->SetArrayExternalOwnership(ETrue);
		iGroupInfoOnEventsChanged = EFalse;
	}

	if (iGroupList->DesCArray()->Count() > 0)
	{
		// Find all events that belong to this group
		iLocalCellDBHelper->iSQLStatement.Format(_L("SELECT Event_Id, Event_Name, Event_Type, State, Priority, Type FROM Events_List WHERE Group_Id = %u ORDER BY Priority"), iGroupIds->At(iGroupList->CurrentItem()));
		if (iLocalCellDBHelper->ReadDB())
		{
			HBufC *iEventsText = HBufC::NewL(50 + 10 + 2);
			TKeyArrayFix iKeyArray(0, ECmpTUint);
			TUint iTempEventType;
			TInt iFoundEventIdIndex;
			while (iLocalCellDBHelper->iDBView.NextL())
			{
				iLocalCellDBHelper->iDBView.GetL();
				iEventIds->AppendL(iLocalCellDBHelper->iDBView.ColUint(1));
				iEventPriorities->AppendL(iLocalCellDBHelper->iDBView.ColUint(5));
				iEventsText->Des().SetLength(0);
				iEventsText->Des().AppendNum(iLocalCellDBHelper->iDBView.ColInt8(4));
				iEventsText->Des().Append(KColumnListSeparator);
				
				// Icon at position 4 is for unknown icon
				TUint iIconIndex = 4;
				iTempEventType = iLocalCellDBHelper->iDBView.ColUint(3);
				if (iEventTypeIds->Find(iTempEventType, iKeyArray, iFoundEventIdIndex) == 0)
				{
					switch (iTempEventType)
					{
						case 0 : iIconIndex = 2; break;
						case 1 : iIconIndex = 3; break;
					};
				}
				iEventsText->Des().AppendNum(iIconIndex);
				iEventsText->Des().Append(KColumnListSeparator);
				
				iIconIndex = 4;
				iTempEventType = iLocalCellDBHelper->iDBView.ColUint(6);
				if (iEventTriggerIds->Find(iTempEventType, iKeyArray, iFoundEventIdIndex) == 0)
				{
					switch (iTempEventType)
					{
						case 0 : iIconIndex = 5; break;
						case 1 : iIconIndex = 6; break;
						case 2 : iIconIndex = 7; break;
					};
				}
				iEventsText->Des().AppendNum(iIconIndex);
				iEventsText->Des().Append(KColumnListSeparator);
				
				iEventsText->Des().Append(iLocalCellDBHelper->iDBView.ColDes(2));
				iEventsText->Des().Append(KColumnListSeparator);
				iEventsListBoxArray->AppendL(*iEventsText);
			}
			iLocalCellDBHelper->iDBView.Close();
			delete iEventsText;
			iRetVal = ETrue;
		}
	}
	iEventsListBox->HandleItemRemovalL();
	
	if (iGroupNames->MdcaCount() < 1)
		iGroupList->SetDimmed(ETrue);
	else
		iGroupList->SetDimmed(EFalse);
	
	if (iEventsListBoxArray->Count() > 0)
		iEventsListBox->SetDimmed(EFalse);
	else
		iEventsListBox->SetDimmed(ETrue);

	UpdatePageButtons_Events();
	return iRetVal;
}

void CEditCellDBDialog::InitGroupsPage()
{
	HBufC *iOneGroup = HBufC::NewL(256 + 5);

	CEikColumnListBox *iGroupsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
	CDesCArray *iGroupsListBoxArray = (CDesCArray *)iGroupsListBox->Model()->ItemTextArray();
	iGroupsListBox->ClearSelection();
	iGroupsListBoxArray->Reset();
	iGroupsListBox->HandleItemRemovalL();

	if (ReadGroupInfo())
	{
		// Init group page
		for (int i = 0; i < iGroupNames->Count(); i++)
		{
			iOneGroup->Des().SetLength(0);
			iOneGroup->Des().AppendNum(iGroupStates->At(i));
			iOneGroup->Des().Append(KColumnListSeparator);
			iOneGroup->Des().Append(iGroupNames->MdcaPoint(i));
			iGroupsListBoxArray->AppendL(*iOneGroup);
		}
	}		
	delete iOneGroup;

	iGroupsListBox->HandleItemRemovalL();

	if (iGroupNames->MdcaCount() < 1)
		iGroupsListBox->SetDimmed(ETrue);
	else
		iGroupsListBox->SetDimmed(EFalse);
		
	UpdatePageButtons_Groups();
}

void CEditCellDBDialog::PreLayoutDynInitL()
{
}

void CEditCellDBDialog::PostLayoutDynInitL()
{
	// Prepare group enable/disable icons
	CArrayPtr<CGulIcon>* iIconArrayGroups = new CArrayPtrFlat<CGulIcon>(2);
	CFbsBitmap* iTempBitmapGroup = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapGroupMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapGroup->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCrossmark));
	User::LeaveIfError(iTempBitmapGroupMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCrossmarkmask));
	iIconArrayGroups->AppendL(CGulIcon::NewL(iTempBitmapGroup, iTempBitmapGroupMask));
	CFbsBitmap* iTempBitmapGroup2 = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapGroupMask2 = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapGroup2->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCheckmark));
	User::LeaveIfError(iTempBitmapGroupMask2->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCheckmarkmask));
	iIconArrayGroups->AppendL(CGulIcon::NewL(iTempBitmapGroup2, iTempBitmapGroupMask2));

	// Prepare events enable/disable icons
	CArrayPtr<CGulIcon>* iIconArrayEvents = new CArrayPtrFlat<CGulIcon>(2);
	CFbsBitmap* iTempBitmapEvent = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEvent->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCrossmark));
	User::LeaveIfError(iTempBitmapEventMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCrossmarkmask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEvent, iTempBitmapEventMask));
	CFbsBitmap* iTempBitmapEvent2 = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventMask2 = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEvent2->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCheckmark));
	User::LeaveIfError(iTempBitmapEventMask2->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderCheckmarkmask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEvent2, iTempBitmapEventMask2));

	// Prepare event type icons
	CFbsBitmap* iTempBitmapEventAudio = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventAudioMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEventAudio->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventaudio));
	User::LeaveIfError(iTempBitmapEventAudioMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventaudiomask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEventAudio, iTempBitmapEventAudioMask));
	CFbsBitmap* iTempBitmapEventVibrate = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventVibrateMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEventVibrate->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventvibrate));
	User::LeaveIfError(iTempBitmapEventVibrateMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventvibratemask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEventVibrate, iTempBitmapEventVibrateMask));
	CFbsBitmap* iTempBitmapEventUnknown = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventUnknownMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEventUnknown->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventunknown));
	User::LeaveIfError(iTempBitmapEventUnknownMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventunknownmask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEventUnknown, iTempBitmapEventUnknownMask));

	// Prepare event trigger icons
	CFbsBitmap* iTempBitmapEventEnter = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventEnterMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEventEnter->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEvententer));
	User::LeaveIfError(iTempBitmapEventEnterMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEvententermask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEventEnter, iTempBitmapEventEnterMask));
	CFbsBitmap* iTempBitmapEventLeave = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventLeaveMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEventLeave->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventleave));
	User::LeaveIfError(iTempBitmapEventLeaveMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEventleavemask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEventLeave, iTempBitmapEventLeaveMask));
	CFbsBitmap* iTempBitmapEventEnterLeave = new (ELeave) CFbsBitmap();
	CFbsBitmap* iTempBitmapEventEnterLeaveMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iTempBitmapEventEnterLeave->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEvententerleave));
	User::LeaveIfError(iTempBitmapEventEnterLeaveMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderEvententerleavemask));
	iIconArrayEvents->AppendL(CGulIcon::NewL(iTempBitmapEventEnterLeave, iTempBitmapEventEnterLeaveMask));

	CEikColumnListBox *iListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
	iListBox->CreateScrollBarFrameL();
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	CColumnListBoxData *iListBoxColumns = ((CColumnListBoxItemDrawer*)iListBox->ItemDrawer())->ColumnData();
	iListBoxColumns->SetGraphicsColumnL(0, ETrue);
	iListBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnWidthPixelL(0, 10);
	iListBoxColumns->SetColumnWidthPixelL(1, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 51);
	iListBoxColumns->SetIconArray(iIconArrayGroups);
	//iListBox->ItemDrawer()->SetMarkGutter(0);
	//iListBox->ItemDrawer()->SetMarkColumnWidth(10);
	
	iListBox = static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox));
	iListBox->CreateScrollBarFrameL();
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	iListBoxColumns = ((CColumnListBoxItemDrawer*)iListBox->ItemDrawer())->ColumnData();
	iListBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(2, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(3, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnWidthPixelL(0, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 6);
	iListBoxColumns->SetColumnWidthPixelL(1, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 5);
	iListBoxColumns->SetColumnWidthPixelL(2, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 9);
	iListBoxColumns->SetColumnWidthPixelL(3, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 51);
	//iListBox->ItemDrawer()->SetMarkGutter(0);
	//iListBox->ItemDrawer()->SetMarkColumnWidth(10);

	iListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
	iListBox->CreateScrollBarFrameL();
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	iListBoxColumns = ((CColumnListBoxItemDrawer*)iListBox->ItemDrawer())->ColumnData();
	iListBoxColumns->SetGraphicsColumnL(0, ETrue);
	iListBoxColumns->SetGraphicsColumnL(1, ETrue);
	iListBoxColumns->SetGraphicsColumnL(2, ETrue);
	iListBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(2, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(3, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnWidthPixelL(0, 10);
	iListBoxColumns->SetColumnWidthPixelL(1, 10);
	iListBoxColumns->SetColumnWidthPixelL(2, 10);
	iListBoxColumns->SetColumnWidthPixelL(3, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 51);
	//iListBox->ItemDrawer()->SetMarkGutter(0);
	//iListBox->ItemDrawer()->SetMarkColumnWidth(10);
	iListBoxColumns->SetIconArray(iIconArrayEvents);

	static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox))->CreateScrollBarFrameL();
	static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);

	InitGroupsPage();
	InitCellsPage(ETrue, 0, EFalse);
	InitEventsPage(ETrue);
	InitImportExportPage(ETrue);
	
	// Since this is the first page that is displayed, we have to set the button states
	UpdatePageButtons_Groups();
}

void CEditCellDBDialog::HandleControlStateChangeL(TInt aControlId)
{
	if (aControlId == cCellDBSelectedGroupOnCellsPage)
	{
		InitCellsPage(EFalse, 0, EFalse);
		static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
	}
	if (aControlId == cCellDBSelectedGroupOnEventsPage)
	{
		InitEventsPage(EFalse);
		static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
	}
}

TBool CEditCellDBDialog::SetGroupState(TInt aState)
{
	TBool iRetVal = EFalse;
	CEikColumnListBox *iGroupsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
	if (iGroupsListBox->SelectionIndexes()->Count() > 0)
	{
		TUint iGroupId;
		TBool iInTrans;
		
		CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
		for (int i = 0; i < iGroupsListBox->SelectionIndexes()->Count(); i++)
		{
			iInTrans = EFalse;
			iGroupId = iGroupIds->At(iGroupsListBox->SelectionIndexes()->At(i));
			if (iLocalCellDBHelper->BeginTransaction() == KErrNone)
			{
				iInTrans = ETrue;
				iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Groups_List SET State = %d WHERE Group_Id = %u"), aState, iGroupId);
				if (iLocalCellDBHelper->ExecuteSQL() == 1)
				{
					iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Cells_List SET Group_State = %d WHERE Group_Id = %u"), aState, iGroupId);
					if (iLocalCellDBHelper->ExecuteSQL() >= 0)
					{
						iLocalCellDBHelper->CommitTransaction();
						iInTrans = EFalse;
						iRetVal = ETrue;
					}
				}
				// If this block gets executed it means an error was encountered
				if (iInTrans)
				{
					iLocalCellDBHelper->RollbackTransaction();
					if (!iLocalCellDBHelper->RecoverDB())
						CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRRECOVERY);
				}
				if (!iRetVal)
				{
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSETGROUPSTATE);
					break;
				}
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSETGROUPSTATE);
		}
		CEikonEnv::Static()->BusyMsgCancel();
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);	
	return iRetVal;
}

TBool CEditCellDBDialog::SetEventState(TInt aState)
{
	TBool iRetVal = EFalse;
	CEikColumnListBox *iEventsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
	if (iEventsListBox->SelectionIndexes()->Count() > 0)
	{
		CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
		TUint iEventId = 0;
		for (int i = 0; i < iEventsListBox->SelectionIndexes()->Count(); i++)
		{
			iEventId = iEventIds->At(iEventsListBox->SelectionIndexes()->At(i));
			iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Events_List SET State = %d WHERE Event_Id = %u"), aState, iEventId);
			if (iLocalCellDBHelper->ExecuteSQL() < 0)
			{
				CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSETEVENTSTATE);
				break;
			}
			else
				iRetVal = ETrue;
		}
		CEikonEnv::Static()->BusyMsgCancel();
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
	return iRetVal;
}

void CEditCellDBDialog::RehighlightGroupsPage(TInt aIndex)
{
	CEikColumnListBox *iGroupsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
	if (aIndex < iGroupsListBox->Model()->ItemTextArray()->MdcaCount())
		iGroupsListBox->SetCurrentItemIndexAndDraw(aIndex);
	else
		iGroupsListBox->SetCurrentItemIndexAndDraw(0);
}

void CEditCellDBDialog::RehighlightEventsPage(TInt aIndex)
{
	CEikColumnListBox *iEventsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
	if (aIndex < iEventsListBox->Model()->ItemTextArray()->MdcaCount())
		iEventsListBox->SetCurrentItemIndexAndDraw(aIndex);
	else
		iEventsListBox->SetCurrentItemIndexAndDraw(0);
}

void CEditCellDBDialog::SetListBoxSelection(TBool aSelectState, CEikColumnListBox *aListBox)
{
	if (aSelectState)
	{
		for (int i = 0; i < aListBox->Model()->ItemTextArray()->MdcaCount(); i++)
			aListBox->View()->SelectItemL(i);
	}
	else
		aListBox->ClearSelection();
}

void CEditCellDBDialog::ProcessPageButtonCommandL(TInt aCommandId)
{
	TUint iCellId = 0;
	TBuf<8> iCountryCode;
	TBuf<16> iNetworkCode;
	TBuf<100> iCellDescription, iNewGroup, iEventDescription;
	TUint iGroupIndex, iGroupId, iGroupPriority, iEventId, iEventTypeId, iEventTriggerId;
	TInt iStatus, iIndex;
	TBool iChangesMade;
	TPtrC iCellRow, iCellIdText;
	TLex iLexConverter;
	TDes *iTemp;
	TBuf<KMaxPath> iImportExportPath;

	switch (aCommandId)
	{
		/***************
		 * Groups page
		 ***************/
		case EEikBidCellGroupSelectAll :
			{
				SetListBoxSelection(ETrue, static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox)));
				break;
			}
		case EEikBidCellGroupSelectNone :
			{
				SetListBoxSelection(EFalse, static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox)));
				break;
			}
		case EEikBidCellGroupNew :
			{
				// Double the group name schema size of 50 because the user could enter all 
				// single quotes and we need to convert that to double quotes for the DBMS
				// engine to properly insert the record
				CEditCellDBNewGroup *iEditDialog = new (ELeave) CEditCellDBNewGroup();
				iEditDialog->iTextLimit = 50;
				iEditDialog->iTheString = &iNewGroup;
				iEditDialog->iIsRequired = ETrue;
				iEditDialog->iStatus = &iStatus;
				if (iEditDialog->ExecuteLD(R_DIALOG_CELLID_NEWGROUP) == EEikBidYes)
				{
					if (CreateGroup(&iNewGroup, iStatus, &iGroupId, &iGroupPriority) == KErrNone)
					{
						InitGroupsPage();
						static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->SetCurrentItemIndexAndDraw(0);
						InitCellsPage(ETrue, 0, EFalse);
						static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
						InitEventsPage(ETrue);
						static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
						InitImportExportPage(ETrue);
						static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox))->SetCurrentItemIndexAndDraw(0);
					}
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRCREATEGROUP);
				}
				break;
			};
		case EEikBidCellGroupDel :
			{
				CEikColumnListBox *iGroupsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
				if (iGroupsListBox->SelectionIndexes()->Count() > 0)
				{
					HBufC *iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
					HBufC *iWarning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_DELETE);
					if (CEikonEnv::Static()->QueryWinL(*iTitle, *iWarning))
					{
						delete iTitle;
						delete iWarning;
						
						TBool iSQLStepOk;
						
						iChangesMade = EFalse;
						CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
						for (int i = 0; i < iGroupsListBox->SelectionIndexes()->Count(); i++)
						{
							iGroupId = iGroupIds->At(iGroupsListBox->SelectionIndexes()->At(i));
							if (iLocalCellDBHelper->BeginTransaction() == KErrNone)
							{
								iSQLStepOk = EFalse;
								iLocalCellDBHelper->iSQLStatement.Format(_L("DELETE FROM Cells_List WHERE Group_Id = %u"), iGroupId);
								if (iLocalCellDBHelper->ExecuteSQL() >= 0)
								{
									iLocalCellDBHelper->iSQLStatement.Format(_L("DELETE FROM Events_List WHERE Group_Id = %u"), iGroupId);
									if (iLocalCellDBHelper->ExecuteSQL() >= 0)
									{
										iLocalCellDBHelper->iSQLStatement.Format(_L("DELETE FROM Groups_List WHERE Group_Id = %u"), iGroupId);
										if (iLocalCellDBHelper->ExecuteSQL() >= 0)
											iSQLStepOk = ETrue;
									}
								}
								if (iSQLStepOk)
								{
									iLocalCellDBHelper->CommitTransaction();
									iChangesMade = ETrue;
								}
								else
								{
									CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRDELGROUP);
									iLocalCellDBHelper->RollbackTransaction();
									// Rolling back transactions leaves indexes in a damaged state
									if (!iLocalCellDBHelper->RecoverDB())
										CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRRECOVERY);
									break;
								}
							}
							else
							{
								CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRDELGROUP);
								break;
							}
						}
						CEikonEnv::Static()->BusyMsgCancel();
						if (iChangesMade)
						{
							InitGroupsPage();
							static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->SetCurrentItemIndexAndDraw(0);
							InitCellsPage(ETrue, 0, EFalse);
							static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
							InitEventsPage(ETrue);
							static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
							InitImportExportPage(ETrue);
							static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox))->SetCurrentItemIndexAndDraw(0);
						}
					}
					else
					{
						delete iTitle;
						delete iWarning;
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};			
		case EEikBidCellGroupEnable :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->CurrentItemIndex();
				if (SetGroupState(1))
				{
					InitGroupsPage();
					RehighlightGroupsPage(iIndex);
				}
				break;
			}
		case EEikBidCellGroupDisable :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->CurrentItemIndex();
				if (SetGroupState(0))
				{
					InitGroupsPage();
					RehighlightGroupsPage(iIndex);
				}
				break;
			}
		case EEikBidCellGroupShiftUp :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->CurrentItemIndex();
				if (ShiftGroup(-1))
				{
					InitGroupsPage();
					RehighlightGroupsPage(iIndex - 1);
				}
				break;
			}
		case EEikBidCellGroupShiftDown :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->CurrentItemIndex();
				if (ShiftGroup(1))
				{
					InitGroupsPage();
					RehighlightGroupsPage(iIndex + 1);
				}
				break;
			}
		case EEikBidCellGroupRen :
			{
				CEikColumnListBox *iGroupsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
				if (iGroupsListBox->SelectionIndexes()->Count() > 0)
				{
					iChangesMade = EFalse;
					for (int i = 0; i < iGroupsListBox->SelectionIndexes()->Count(); i++)
					{
						iGroupId = iGroupIds->At(iGroupsListBox->SelectionIndexes()->At(i));
						iNewGroup.Copy(iGroupNames->MdcaPoint(iGroupsListBox->SelectionIndexes()->At(i)));
						CEditCellDBGenericString *iEditDialog = new (ELeave) CEditCellDBGenericString();
						iEditDialog->iTextLimit = 50;
						iEditDialog->iTheString = &iNewGroup;
						iEditDialog->iIsRequired = ETrue;
						if (iEditDialog->ExecuteLD(R_DIALOG_MISC_EDITSTRING) == EEikBidYes)
						{
							iLocalCellDBHelper->ConvertTextStatement(&iNewGroup, _L("'"), _L("''"));
							iLocalCellDBHelper->iSQLStatement.Copy(_L("UPDATE Groups_List SET Group_Name = '"));
							iLocalCellDBHelper->iSQLStatement.Append(iNewGroup);
							iLocalCellDBHelper->iSQLStatement.Append(_L("' WHERE Group_Id = "));
							iLocalCellDBHelper->iSQLStatement.AppendNum(iGroupId, 10);
							CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
							if (iLocalCellDBHelper->ExecuteSQL() < 0)
							{
								CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRRENGROUP);
								break;
							}
							else
								iChangesMade = ETrue;
							CEikonEnv::Static()->BusyMsgCancel();
						}	
						else
							break;
					}
					if (iChangesMade)
					{
						InitGroupsPage();
						static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->SetCurrentItemIndexAndDraw(0);
						InitCellsPage(ETrue, 0, EFalse);
						static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
						InitEventsPage(ETrue);
						static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
						InitImportExportPage(ETrue);
						static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox))->SetCurrentItemIndexAndDraw(0);
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};
		/***************
		 * Cells page
		 ***************/
		case EEikBidCellCellsSelectAll :
			{
				SetListBoxSelection(ETrue, static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox)));
				break;
			}
		case EEikBidCellCellsSelectNone :
			{
				SetListBoxSelection(EFalse, static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox)));
				break;
			}
		case EEikBidCellCellsDel :
			{
				CEikChoiceList *iGroupsListBox = static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnCellsPage));
				TUint iGroupIndex = iGroupsListBox->CurrentItem();
				CEikColumnListBox *iCellsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox));
				if (iCellsListBox->SelectionIndexes()->Count() > 0)
				{
					HBufC *iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
					HBufC *iWarning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_DELETE);
					if (CEikonEnv::Static()->QueryWinL(*iTitle, *iWarning))
					{
						delete iTitle;
						delete iWarning;
				
						iChangesMade = EFalse;
						CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
						for (int i = 0; i < iCellsListBox->SelectionIndexes()->Count(); i++)
						{
							iCellRow.Set(iCellsListBox->Model()->ItemText(iCellsListBox->SelectionIndexes()->At(i)));
							TextUtils::ColumnText(iCellIdText, 0, &iCellRow);
							iLexConverter.Assign(iCellIdText);
							iLexConverter.Val(iCellId);
							iLocalCellDBHelper->iSQLStatement.Format(_L("DELETE FROM Cells_List WHERE Cell_Id = %u AND Group_Id = %u"), iCellId, iGroupIds->At(iGroupIndex));
							if (iLocalCellDBHelper->ExecuteSQL() >= 0)
								iChangesMade = ETrue;
							else
							{
								CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRDELCELL);
								break;
							}
						}
						CEikonEnv::Static()->BusyMsgCancel();
						if (iChangesMade)
						{
							InitCellsPage(EFalse, 0, EFalse);
							static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
						}
					}
					else
					{
						delete iTitle;
						delete iWarning;
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};
		case EEikBidCellCellsMove :
			{
				CEikColumnListBox *iCellsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox));
				if (iCellsListBox->SelectionIndexes()->Count() > 0)
				{
					iGroupId = static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnCellsPage))->CurrentItem();
					iChangesMade = EFalse;
					TUint iOldGroupId = iGroupId;
					
					CEditCellDBMoveCells *iMoveDialog = new (ELeave) CEditCellDBMoveCells();
					iMoveDialog->iGroupId = &iGroupId;
					iMoveDialog->iGroupNames = iGroupNames;
					if (iMoveDialog->ExecuteLD(R_DIALOG_CELLID_MOVECELL) == EEikBidYes)
					{
						CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
						TInt iDBResult;
						TInt i;
						CArrayFixFlat<TInt> *iSelectedItemIndexes = new (ELeave) CArrayFixFlat<TInt>(1);
						
						// Make a copy of the selection indexes since we could end up changing
						// it dynamically via the call to DeselectItem() below...
						for (i = 0; i < iCellsListBox->SelectionIndexes()->Count(); i++)
							iSelectedItemIndexes->AppendL(iCellsListBox->SelectionIndexes()->At(i));
							
						for (i = 0; i < iSelectedItemIndexes->Count(); i++)
						{
							iCellRow.Set(iCellsListBox->Model()->ItemText(iSelectedItemIndexes->At(i)));
							TextUtils::ColumnText(iCellIdText, 0, &iCellRow);
							iLexConverter.Assign(iCellIdText);
							iLexConverter.Val(iCellId);
							iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Cells_List SET Group_Id = %u, Group_Priority = %u, Group_State = %u WHERE Cell_Id = %u AND Group_Id = %u"), iGroupIds->At(iGroupId), iGroupPriorities->At(iGroupId), iGroupStates->At(iGroupId), iCellId, iGroupIds->At(iOldGroupId));
							iDBResult = iLocalCellDBHelper->ExecuteSQL();
							if (iDBResult < 0)
							{
								if (iDBResult != KErrAlreadyExists)
								{
									CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSAVECELL);
									break;
								}
							}
							else
							{
								// Uncheck the selected items as we move along so that
								// when we refresh the cells page, we are more efficient in
								// only marking down the items that didn't move over
								// successfully. See the InitCellsPage() member to see how
								// this affects it
								iCellsListBox->View()->DeselectItem(iSelectedItemIndexes->At(i));
								iChangesMade = ETrue;
							}
						}
						delete iSelectedItemIndexes;
						CEikonEnv::Static()->BusyMsgCancel();
					}
					if (iChangesMade)
					{
						InitCellsPage(EFalse, 0, ETrue);
						static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};
		case EEikBidCellCellsNew :
			{
				iGroupIndex = (static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnCellsPage)))->CurrentItem();
				iCountryCode.Copy(*iDefaultCountryCodeForNewCells);
				iNetworkCode.Copy(*iDefaultNetworkIdForNewCells);
				CEditCellDBNewCell *iEditDialog = new (ELeave) CEditCellDBNewCell();
				iEditDialog->iCellId = &iCellId;
				iEditDialog->iCountryCode = &iCountryCode;
				iEditDialog->iNetworkCode = &iNetworkCode;
				iEditDialog->iGroupId = &iGroupIndex;
				iEditDialog->iGroupNames = iGroupNames;
				iEditDialog->iCellDescription = &iCellDescription;
				if (iEditDialog->ExecuteLD(R_DIALOG_CELLID_NEWCELL) == EEikBidYes)
				{
					TInt iDBResult = CreateCell(iCellId, iGroupIds->At(iGroupIndex), iGroupPriorities->At(iGroupIndex), iGroupStates->At(iGroupIndex), &iCountryCode, &iNetworkCode, &iCellDescription);
					if (iDBResult >= 0)
					{
						InitCellsPage(EFalse, 0, EFalse);
						static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
					}
					else if (iDBResult == KErrAlreadyExists)
						CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRCELLEXISTS);
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRCREATECELL);
				}
				break;
			}
		case EEikBidCellCellsEdit :
			{
				CEikColumnListBox *iCellsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox));
				if (iCellsListBox->Model()->ItemTextArray()->MdcaCount() > 0)
				{
					TUint iOldCellId;

					iGroupIndex = (static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnCellsPage)))->CurrentItem();
					iCellRow.Set(iCellsListBox->Model()->ItemText(iCellsListBox->CurrentItemIndex()));
					TextUtils::ColumnText(iCellIdText, 0, &iCellRow);
					iLexConverter.Assign(iCellIdText);
					iLexConverter.Val(iCellId);
					iOldCellId = iCellId;
					
					TextUtils::ColumnText(iCellIdText, 1, &iCellRow);
					iCountryCode.Copy(iCellIdText);
					TextUtils::ColumnText(iCellIdText, 2, &iCellRow);
					iNetworkCode.Copy(iCellIdText);
					TextUtils::ColumnText(iCellIdText, 3, &iCellRow);
					iCellDescription.Copy(iCellIdText);

					CEditCellDBNewCell *iEditDialog = new (ELeave) CEditCellDBNewCell();
					iEditDialog->iCellId = &iCellId;
					iEditDialog->iCountryCode = &iCountryCode;
					iEditDialog->iNetworkCode = &iNetworkCode;
					iEditDialog->iCellDescription = &iCellDescription;
					iEditDialog->iGroupId = &iGroupIndex;
					iEditDialog->iGroupNames = iGroupNames;
					if (iEditDialog->ExecuteLD(R_DIALOG_CELLID_NEWCELL) == EEikBidYes)
					{
						iLocalCellDBHelper->ConvertTextStatement(&iCountryCode, _L("'"), _L("''"));
						iLocalCellDBHelper->ConvertTextStatement(&iNetworkCode, _L("'"), _L("''"));
						iLocalCellDBHelper->iSQLStatement.Copy(_L("UPDATE Cells_List SET Country_Code = '"));
						iLocalCellDBHelper->iSQLStatement.Append(iCountryCode);
						iLocalCellDBHelper->iSQLStatement.Append(_L("',Network_Id = '"));
						iLocalCellDBHelper->iSQLStatement.Append(iNetworkCode);
						iLocalCellDBHelper->iSQLStatement.Append(_L("',Group_Id = "));
						iLocalCellDBHelper->iSQLStatement.AppendNum(iGroupIds->At(iGroupIndex));
						iLocalCellDBHelper->iSQLStatement.Append(_L(",Group_Priority = "));
						iLocalCellDBHelper->iSQLStatement.AppendNum(iGroupPriorities->At(iGroupIndex));
						iLocalCellDBHelper->iSQLStatement.Append(_L(",Group_State = "));
						iLocalCellDBHelper->iSQLStatement.AppendNum(iGroupStates->At(iGroupIndex));
						iLocalCellDBHelper->iSQLStatement.Append(_L(",Description = '"));
						iLocalCellDBHelper->ConvertTextStatement(&iCellDescription, _L("'"), _L("''"));
						iLocalCellDBHelper->iSQLStatement.Append(iCellDescription);
						iLocalCellDBHelper->iSQLStatement.Append(_L("'"));
						
						if (iOldCellId != iCellId)
						{
							iLocalCellDBHelper->iSQLStatement.Append(_L(",Cell_Id = "));
							iLocalCellDBHelper->iSQLStatement.AppendNum(iCellId);
						}
						
						iLocalCellDBHelper->iSQLStatement.Append(_L(" WHERE Cell_Id = "));
						iLocalCellDBHelper->iSQLStatement.AppendNum(iOldCellId);
						CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
						if (iLocalCellDBHelper->ExecuteSQL() == 1)
						{
							InitCellsPage(EFalse, 0, EFalse);
							static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
						}
						else
							CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSAVECELL);
						CEikonEnv::Static()->BusyMsgCancel();
					}			
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMS);
				break;
			}
		case EEikBidCellCellsFind :
			{
				CEditCellDBFindCell *iFindCellDialog = new (ELeave) CEditCellDBFindCell();
				iFindCellDialog->iCellId = &iCellId;
				if (iFindCellDialog->ExecuteLD(R_DIALOG_CELLID_FINDCELL) == EEikBidYes)
				{
					// Table joins are not supported in symbian. :(
					iLocalCellDBHelper->iSQLStatement.Format(_L("SELECT Group_Id FROM Cells_List WHERE Cell_Id = %u"), iCellId);
					if (iLocalCellDBHelper->ReadDB())
					{
						if (iLocalCellDBHelper->iDBView.CountL() > 0)
						{
							CArrayFixFlat<TUint> *iFoundGroupIds;
							CDesCArray *iFoundGroupNames;

							iFoundGroupIds = new (ELeave) CArrayFixFlat<TUint>(3);
							iFoundGroupNames = new (ELeave) CDesCArrayFlat(3);
							
							TUint iTempGroupId;
							TInt iFoundGroupIdIndex = 0;
							TKeyArrayFix iKeyArray(0, ECmpTUint);

							while (iLocalCellDBHelper->iDBView.NextL())
							{
								// We do our own in-memory table joins :(
								iLocalCellDBHelper->iDBView.GetL();
								iTempGroupId = iLocalCellDBHelper->iDBView.ColUint32(1);
								if (iGroupIds->Find(iTempGroupId, iKeyArray, iFoundGroupIdIndex) == 0)
								{
									iFoundGroupIds->AppendL(iTempGroupId);
									iFoundGroupNames->AppendL(iGroupNames->MdcaPoint(iFoundGroupIdIndex));
								}
							}
							iLocalCellDBHelper->iDBView.Close();

							iGroupId = 0;
							// We can reuse this dialog
							CEditCellDBSelectGroup *iGroupsDialog = new (ELeave) CEditCellDBSelectGroup();
							iGroupsDialog->iGroupIdIndex = &iGroupId;
							iGroupsDialog->iGroupNames = iFoundGroupNames;
							if (iGroupsDialog->ExecuteLD(R_DIALOG_CELLID_FOUNDCELL) == EEikBidYes)
							{
								iGroupId = iFoundGroupIds->At(iGroupId);
								delete iFoundGroupNames;
								delete iFoundGroupIds;
								if (iGroupIds->Find(iGroupId, iKeyArray, iFoundGroupIdIndex) == 0)
								{
									static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnCellsPage))->SetCurrentItem(iFoundGroupIdIndex);
									InitCellsPage(EFalse, iCellId, EFalse);
								}
								else
									CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_GROUPNOTFOUND);
							}
							else
							{
								delete iFoundGroupNames;
								delete iFoundGroupIds;
							}
						}
						else
							CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_CELLNOTFOUND);
					}
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRREADDB);
				}
				break;
			};
		/***************
		 * Events page
		 ***************/
		case EEikBidCellEventsSelectAll :
			{
				SetListBoxSelection(ETrue, static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox)));
				break;
			}
		case EEikBidCellEventsSelectNone :
			{
				SetListBoxSelection(EFalse, static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox)));
				break;
			}
		case EEikBidCellEventsShiftUp :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->CurrentItemIndex();
				if (ShiftEvents(-1))
				{
					InitEventsPage(EFalse);
					RehighlightEventsPage(iIndex - 1);
				}
				break;
			};
		case EEikBidCellEventsShiftDown :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->CurrentItemIndex();
				if (ShiftEvents(1))
				{
					InitEventsPage(EFalse);
					RehighlightEventsPage(iIndex + 1);
				}
				break;
			};
		case EEikBidCellEventsEnable :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->CurrentItemIndex();
				if (SetEventState(1))
				{
					InitEventsPage(EFalse);
					RehighlightEventsPage(iIndex);
				}
				break;
			};
		case EEikBidCellEventsDisable :
			{
				iIndex = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->CurrentItemIndex();
				if (SetEventState(0))
				{
					InitEventsPage(EFalse);
					RehighlightEventsPage(iIndex);
				}
				break;
			};
		case EEikBidCellEventsDel :
			{
				iEventId = 0;
				CEikColumnListBox *iEventsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
				if (iEventsListBox->SelectionIndexes()->Count() > 0)
				{
					HBufC *iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
					HBufC *iWarning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_DELETE);
					if (CEikonEnv::Static()->QueryWinL(*iTitle, *iWarning))
					{
						delete iTitle;
						delete iWarning;
				
						TBool iChangesMade = EFalse;
						CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);		
						for (int i = 0; i < iEventsListBox->SelectionIndexes()->Count(); i++)
						{
							iEventId = iEventIds->At(iEventsListBox->SelectionIndexes()->At(i));
							iLocalCellDBHelper->iSQLStatement.Format(_L("DELETE FROM Events_List WHERE Event_Id = %u"), iEventId);
							if (iLocalCellDBHelper->ExecuteSQL() >= 0)
								iChangesMade = ETrue;
							else
							{
								CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRDELEVENTS);
								break;
							}
						}
						CEikonEnv::Static()->BusyMsgCancel();
						if (iChangesMade)
						{
							InitEventsPage(EFalse);
							static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
						}
					}
					else
					{
						delete iTitle;
						delete iWarning;
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};
		case EEikBidCellEventsMove :
			{
				CEikColumnListBox *iEventsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
				iGroupId = (static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnEventsPage)))->CurrentItem();
				if (iEventsListBox->SelectionIndexes()->Count() > 0)
				{
					iGroupId = static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnEventsPage))->CurrentItem();
					iChangesMade = EFalse;
					
					CEditCellDBMoveCells *iMoveDialog = new (ELeave) CEditCellDBMoveCells();
					iMoveDialog->iGroupId = &iGroupId;
					iMoveDialog->iGroupNames = iGroupNames;
					if (iMoveDialog->ExecuteLD(R_DIALOG_CELLID_MOVECELL) == EEikBidYes)
					{
						iGroupId = iGroupIds->At(iGroupId);
						CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
						for (int i = 0; i < iEventsListBox->SelectionIndexes()->Count(); i++)
						{
							iEventId = iEventIds->At(iEventsListBox->SelectionIndexes()->At(i));
							iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Events_List SET Group_Id = %u WHERE Event_Id = %u"), iGroupId, iEventId);
							if (iLocalCellDBHelper->ExecuteSQL() < 0)
							{
								CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSAVEEVENT);
								break;
							}
							else
								iChangesMade = ETrue;
						}
						CEikonEnv::Static()->BusyMsgCancel();
					}
					if (iChangesMade)
					{
						InitEventsPage(EFalse);
						static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};
		case EEikBidCellEventsNew :
			{
				iStatus = 0;
				iEventTypeId = 0;
				iGroupId = (static_cast<CEikChoiceList*>(Control(cCellDBSelectedGroupOnEventsPage)))->CurrentItem();
				CEditSelectEvent *iNewEventDialog = new (ELeave) CEditSelectEvent();
				iNewEventDialog->iEventTypeIdIndex = &iEventTypeId;
				iNewEventDialog->iGroupNames = iGroupNames;
				iNewEventDialog->iGroupIdIndex = &iGroupId;
				iNewEventDialog->iEventDescription = &iEventDescription;
				iNewEventDialog->iEventTypeDescriptions = iEventDesc;
				iNewEventDialog->iEventTriggerIdIndex = &iEventTriggerId;
				iNewEventDialog->iEventTriggerDescriptions = iEventTriggerDesc;
				iNewEventDialog->iState = &iStatus;
				if (iNewEventDialog->ExecuteLD(R_DIALOG_EVENT_TYPE_SELECTION) == EEikBidYes)
				{
					iGroupId = iGroupIds->At(iGroupId);
					iEventTypeId = iEventTypeIds->At(iEventTypeId);
					iEventTriggerId = iEventTriggerIds->At(iEventTriggerId);
					switch (iEventTypeId)
					{
						// Play Audio
						case 0 : 
							{
								HBufC *iAudioFileName = HBufC::NewL(KMaxFileName * 2);
								iAudioFileName->Des().SetLength(0);
								CEditCellNewAudioEvent *iNewAudioEventDialog = new (ELeave) CEditCellNewAudioEvent();
				#ifdef __WINS__
								iAudioFileName->Des().Copy(_L("C:\\APAMACAM.WAV"));
				#endif								
								iNewAudioEventDialog->iSelectedAudioFileName = iAudioFileName;
								if (iNewAudioEventDialog->ExecuteLD(R_DIALOG_EVENT_AUDIO) == EEikBidYes)
								{
									//Event_Id	Event_Name	Event_Type	Event_Data	Group_Id	Priority
									iLocalCellDBHelper->iSQLStatement.Copy(_L("INSERT INTO Events_List (Event_Name,Event_Type,Event_Data,Group_Id,State,Type) VALUES ('"));
									iLocalCellDBHelper->ConvertTextStatement(&iEventDescription, _L("'"), _L("''"));
									iLocalCellDBHelper->iSQLStatement.Append(iEventDescription);
									iLocalCellDBHelper->iSQLStatement.Append(_L("',"));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iEventTypeId);
									iLocalCellDBHelper->iSQLStatement.Append(_L(",'"));
									TPtr iTempPtr = iAudioFileName->Des();
									iTemp = &iTempPtr;
									iLocalCellDBHelper->ConvertTextStatement(iTemp, _L("'"), _L("''"));
									iLocalCellDBHelper->iSQLStatement.Append(*iAudioFileName);
									iLocalCellDBHelper->iSQLStatement.Append(_L("',"));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iGroupId);
									iLocalCellDBHelper->iSQLStatement.Append(_L(","));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iStatus);
									iLocalCellDBHelper->iSQLStatement.Append(_L(","));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iEventTriggerId);
									iLocalCellDBHelper->iSQLStatement.Append(_L(")"));
									if (iLocalCellDBHelper->ExecuteSQL() < 0)
										CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSAVEEVENT);
								}
								delete iAudioFileName;
								break;
							};
						// Vibrate
						case 1 :
							{
								// This event has no event data
								iLocalCellDBHelper->iSQLStatement.Copy(_L("INSERT INTO Events_List (Event_Name,Event_Type,Group_Id,State,Type) VALUES ('"));
								iLocalCellDBHelper->ConvertTextStatement(&iEventDescription, _L("'"), _L("''"));
								iLocalCellDBHelper->iSQLStatement.Append(iEventDescription);
								iLocalCellDBHelper->iSQLStatement.Append(_L("',"));
								iLocalCellDBHelper->iSQLStatement.AppendNum(iEventTypeId);
								iLocalCellDBHelper->iSQLStatement.Append(_L(","));
								iLocalCellDBHelper->iSQLStatement.AppendNum(iGroupId);
								iLocalCellDBHelper->iSQLStatement.Append(_L(","));
								iLocalCellDBHelper->iSQLStatement.AppendNum(iStatus);
								iLocalCellDBHelper->iSQLStatement.Append(_L(","));
								iLocalCellDBHelper->iSQLStatement.AppendNum(iEventTriggerId);
								iLocalCellDBHelper->iSQLStatement.Append(_L(")"));
								if (iLocalCellDBHelper->ExecuteSQL() < 0)
									CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSAVEEVENT);
								break;
							};
					};
				}
				InitEventsPage(EFalse);
				static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
				break;
			};
		case EEikBidCellEventsEdit :
			{
				CEikColumnListBox *iEventsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
				iEventId = iEventIds->At(iEventsListBox->CurrentItemIndex());
				iLocalCellDBHelper->iSQLStatement.Format(_L("SELECT Event_Name, Event_Data, Event_Type, Type FROM Events_List WHERE Event_Id = %u"), iEventId);
				if (iLocalCellDBHelper->ReadDB())
				{
					iLocalCellDBHelper->iDBView.NextL();
					iLocalCellDBHelper->iDBView.GetL();
					HBufC *iEventData = HBufC::NewL(KMaxFileName * 2);
					HBufC *iEventDescription = HBufC::NewL(100);
					iEventDescription->Des().Copy(iLocalCellDBHelper->iDBView.ColDes(1));
					iEventData->Des().Copy(iLocalCellDBHelper->iDBView.ColDes(2));
					iEventTypeId = iLocalCellDBHelper->iDBView.ColUint32(3);
					iEventTriggerId = iLocalCellDBHelper->iDBView.ColUint32(4);
					
					switch (iEventTypeId) 
					{
						// Play audio
						case 0 :
							{
								iLocalCellDBHelper->iDBView.Close();
								CEditCellEditAudioEvent *iEditAudioEvent = new (ELeave) CEditCellEditAudioEvent();
								iEditAudioEvent->iSelectedAudioFileName = iEventData;
								iEditAudioEvent->iEventName = iEventDescription;
								iEditAudioEvent->iEventTriggerIdIndex = &iEventTriggerId;
								iEditAudioEvent->iEventTriggerDescriptions = iEventTriggerDesc;
								if (iEditAudioEvent->ExecuteLD(R_DIALOG_EVENT_EDIT_AUDIO) == EEikBidYes)
								{
									iEventTriggerId = iEventTriggerIds->At(iEventTriggerId);
									TPtr iTempPtr = iEventDescription->Des();
									iTemp = &iTempPtr;
									iLocalCellDBHelper->ConvertTextStatement(iTemp, _L("'"), _L("''"));
									iLocalCellDBHelper->iSQLStatement.Copy(_L("UPDATE Events_List SET Event_Name='"));
									iLocalCellDBHelper->iSQLStatement.Append(*iEventDescription);
									iLocalCellDBHelper->iSQLStatement.Append(_L("',Event_Data='"));
									iTempPtr = iEventData->Des();
									iTemp = &iTempPtr;
									iLocalCellDBHelper->ConvertTextStatement(iTemp, _L("'"), _L("''"));
									iLocalCellDBHelper->iSQLStatement.Append(*iEventData);
									iLocalCellDBHelper->iSQLStatement.Append(_L("',Type="));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iEventTriggerId);
									iLocalCellDBHelper->iSQLStatement.Append(_L(" WHERE Event_Id="));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iEventId);
									CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
									if (iLocalCellDBHelper->ExecuteSQL() <= 0)
										CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSAVEEVENT);
									CEikonEnv::Static()->BusyMsgCancel();
								}
								break;
							};
						// Vibrate
						case 1 :
							{
								iLocalCellDBHelper->iDBView.Close();
								CEditCellEditVibrateEvent *iEditVibrateEvent = new (ELeave) CEditCellEditVibrateEvent();
								iEditVibrateEvent->iTextLimit = 50;
								iEditVibrateEvent->iIsRequired = ETrue;
								TPtr iTempPtr = iEventDescription->Des();
								iTemp = &iTempPtr;
								iEditVibrateEvent->iTheString = iTemp;
								iEditVibrateEvent->iEventTriggerIdIndex = &iEventTriggerId;
								iEditVibrateEvent->iEventTriggerDescriptions = iEventTriggerDesc;
								if (iEditVibrateEvent->ExecuteLD(R_DIALOG_EVENT_EDIT_VIBRATE) == EEikBidYes)
								{
									iEventTriggerId = iEventTriggerIds->At(iEventTriggerId);
									iLocalCellDBHelper->ConvertTextStatement(iTemp, _L("'"), _L("''"));
									iLocalCellDBHelper->iSQLStatement.Copy(_L("UPDATE Events_List SET Event_Name='"));
									iLocalCellDBHelper->iSQLStatement.Append(*iEventDescription);
									iLocalCellDBHelper->iSQLStatement.Append(_L("',Type="));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iEventTriggerId);
									iLocalCellDBHelper->iSQLStatement.Append(_L(" WHERE Event_Id="));
									iLocalCellDBHelper->iSQLStatement.AppendNum(iEventId);
									CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
									if (iLocalCellDBHelper->ExecuteSQL() <= 0)
										CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRSAVEEVENT);
									CEikonEnv::Static()->BusyMsgCancel();
								}
								break;
							};
						default :
							{
								iLocalCellDBHelper->iDBView.Close();
								CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRUNKNOWNEVENTTYPE);
								break;
							}
					};
					delete iEventData;
					delete iEventDescription;
				}
				InitEventsPage(EFalse);
				static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
				break;
			};
		/***************
		 * Share page
		 ***************/
		case EEikBidCellImportExportSelectAll :
			{
				SetListBoxSelection(ETrue, static_cast<CEikColumnListBox*>(Control(cCellDBImportExportGroupListBox)));
				break;
			}
		case EEikBidCellImportExportSelectNone :
			{
				SetListBoxSelection(EFalse, static_cast<CEikColumnListBox*>(Control(cCellDBImportExportGroupListBox)));
				break;
			}
		case EEikBidCellImport :
			{
				TBool iGroupsImported = EFalse;

				iImportExportPath.SetLength(0);
				CFolderBrowser *iFolderBrowser = new (ELeave) CFolderBrowser();
				iFolderBrowser->iPath = &iImportExportPath;
				iFolderBrowser->iFileSpec.Copy(_L("*.CSV"));
				iFolderBrowser->iAllowSelection = ETrue;
				CDesCArray *iFilesArray = new (ELeave) CDesCArrayFlat(2);
				iFolderBrowser->iFilesSelected = iFilesArray;
				if (iFolderBrowser->ExecuteLD(R_DIALOG_FOLDER_FILE_BROWSER) == EEikBidYes)
				{
					if (iFilesArray->Count() > 0)
					{
						HBufC *iReadBuffer = HBufC::NewL(256);
						TBool iProgressDialogActive = ETrue;
						RFile iCSVFile;
						CTextFileReader *iTextFileReader = new (ELeave) CTextFileReader();
						CCSVLineParser *iCSVParser = new (ELeave) CCSVLineParser();
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
							iFileMask->Des().Copy(iImportExportPath);
							iFileMask->Des().Append(iFilesArray->MdcaPoint(i));
							
							// Now we have the filename! Sooooooooo, we can start parsing
							// Btw, TFileText is crap!!

							if (iProgressDialogActive)
								iProgressDialog->AppendText(-1, iFileMask);

							// Prepare to inject group into database
							if (iLocalCellDBHelper->BeginTransaction() == KErrNone)
							{
								TBool iErrorEncountered = EFalse;

								// Insert group record
								iReadBuffer->Des().Copy(iFilesArray->MdcaPoint(i));
								// Remove .CSV file extension from group name
								iReadBuffer->Des().SetLength(iReadBuffer->Des().Length() - 4);
								TPtr iPtr = TPtr(iReadBuffer->Des());
								iTemp = &iPtr;				
								if (CreateGroup(iTemp, 0, &iGroupId, &iGroupPriority) == KErrNone)
								{
									if (iCSVFile.Open(CEikonEnv::Static()->FsSession(), iFileMask->Des(), EFileRead) == KErrNone)
									{
										iTextFileReader->Set(&iCSVFile);
										TInt iReadResult = KErrGeneral;
										TInt iCurrentCSVLine = 0;
										do 
										{
											TBuf<50> iCellDescription;
										
											iCurrentCSVLine++;
											iReadResult = iTextFileReader->ReadLine(iTemp);
											
											if (iProgressDialogActive)
												iProgressDialog->SetProgressValue(iTextFileReader->iPercentRead);
												
											if (iReadResult == KErrNone)
											{	
												// Read cellid
												if (iCSVParser->GetColumn(iTemp, &iCellDescription, 0) == KErrNone)
												{
													iLexConverter.Assign(iCellDescription);
													if (iLexConverter.Val(iCellId) == KErrNone)
													{
														// Read country code
														if (iCSVParser->GetColumn(iTemp, &iCountryCode, 1) == KErrNone)
														{
															iCellDescription.Copy(iCountryCode);
															iCellDescription.Trim();
															if (iCellDescription.Length() > 0)
															{
																// Read network id
																if (iCSVParser->GetColumn(iTemp, &iNetworkCode, 2) == KErrNone)
																{
																	iCellDescription.Copy(iNetworkCode);
																	iCellDescription.Trim();
																	if (iCellDescription.Length() > 0)
																	{
																		// Read cell description
																		if (iCSVParser->GetColumn(iTemp, &iCellDescription, 3) == KErrNone)
																		{
																			iCSVParser->ReplaceAll(&iCountryCode, _L("\"\""), _L("\""));
																			iCSVParser->ReplaceAll(&iNetworkCode, _L("\"\""), _L("\""));
																			iCSVParser->ReplaceAll(&iCellDescription, _L("\"\""), _L("\""));
																			TInt iResult = CreateCell(iCellId, iGroupId, iGroupPriority, 0, &iCountryCode, &iNetworkCode, &iCellDescription);
																			if (iResult != 1)
																			{
																				if (iProgressDialogActive)
																				{
																					HBufC *iErrorText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_IMPEXP_GENERALERROR);
																					iErrorText = iErrorText->ReAllocL(iErrorText->Length() + 11);
																					iErrorText->Des().AppendNum(iCurrentCSVLine);
																					HBufC *iDBErrText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_IMPEXP_GENERALDBERROR);
																					iDBErrText = iDBErrText->ReAllocL(iDBErrText->Length() + 11 + 3);
																					iDBErrText->Des().AppendNum(iResult);
																					iErrorText->Des().Append(_L(". "));
																					iErrorText->Des().Append(*iDBErrText);
																					delete iDBErrText;
																					iProgressDialog->AppendText(R_TBUF_CELLID_ERRCREATECELL, iErrorText);
																					delete iErrorText;
																				}
																				iErrorEncountered = ETrue;
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
																	else
																	{
																		if (iProgressDialogActive)
																		{
																			HBufC *iErrorText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_IMPEXP_GENERALERROR);
																			iErrorText = iErrorText->ReAllocL(iErrorText->Length() + 11);
																			iErrorText->Des().AppendNum(iCurrentCSVLine);
																			iProgressDialog->AppendText(R_TBUF_CELLID_REQUIRENETWORKID, iErrorText);
																			delete iErrorText;
																		}
																		iErrorEncountered = ETrue;
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
															else
															{
																HBufC *iErrorText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_IMPEXP_GENERALERROR);
																iErrorText = iErrorText->ReAllocL(iErrorText->Length() + 11);
																iErrorText->Des().AppendNum(iCurrentCSVLine);
																iProgressDialog->AppendText(R_TBUF_CELLID_REQUIRECOUNTRYCODE, iErrorText);
																delete iErrorText;
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
											} // if (iReadResult == KErrNone)
											else if (iReadResult != KErrEof)
											{
												if (iProgressDialogActive)
												{
													HBufC *iErrorText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_IMPEXP_GENERALERROR);
													iErrorText = iErrorText->ReAllocL(iErrorText->Length() + 11);
													iErrorText->Des().AppendNum(iCurrentCSVLine);												
													iProgressDialog->AppendText(R_TBUF_CELLID_ERRREADINGIMPORTFILE, iErrorText);
													delete iErrorText;
												}
												iErrorEncountered = ETrue;
											}
										} while ((iReadResult == KErrNone) && (!iErrorEncountered)); // end-do
									} // if (iCSVFile.Open(CEikonEnv::Static()->FsSession(), iFileMask->Des(), EFileRead) == KErrNone)
									else
									{
										if (iProgressDialogActive)
											iProgressDialog->AppendText(R_TBUF_CELLID_ERRREADINGIMPORTFILE, (HBufC *)(NULL));
										iErrorEncountered = ETrue;
									}
									iCSVFile.Close();
								} // if (CreateGroup(iReadBuffer, 0, &iGroupId) == KErrNone)
								else
								{
									if (iProgressDialogActive)
										iProgressDialog->AppendText(R_TBUF_CELLID_ERRCREATEGROUP, (HBufC *)(NULL));
									iErrorEncountered = ETrue;
								}
								
								if (iErrorEncountered)
								{
									iLocalCellDBHelper->RollbackTransaction();
									if (!iLocalCellDBHelper->RecoverDB())
									{
										if (iProgressDialogActive)
											iProgressDialog->AppendText(R_TBUF_MISC_ERRRECOVERY, (HBufC *)(NULL));
									}
									break;
								}
								else
								{
									iLocalCellDBHelper->CommitTransaction();
									iGroupsImported = ETrue;
								}
									
							} // if (iLocalCellDBHelper->BeginTransaction() == KErrNone)
							else
							{
								if (iProgressDialogActive)
									iProgressDialog->AppendText(R_TBUF_CELLID_ERRCREATEGROUP, (HBufC *)(NULL));
							}
						} // for (int i = 0; i < iFilesSelectionIndexes->Count(); i++)
						delete iCSVParser;
						delete iTextFileReader;
						delete iReadBuffer;
						if (iProgressDialogActive)
							iProgressDialog->SetButtonDim(EFalse);
					}
					else
						CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOIMPORTFILES);
				} // if (iFolderBrowser->ExecuteLD(R_DIALOG_FOLDER_FILE_BROWSER) == EEikBidYes)
				delete iFilesArray;
				if (iGroupsImported)
				{
					InitGroupsPage();
					static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox))->SetCurrentItemIndexAndDraw(0);
					InitCellsPage(ETrue, 0, EFalse);
					static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->SetCurrentItemIndexAndDraw(0);
					InitEventsPage(ETrue);
					static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->SetCurrentItemIndexAndDraw(0);
					InitImportExportPage(ETrue);
					static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox))->SetCurrentItemIndexAndDraw(0);
				}
				break;
			};
		case EEikBidCellExport :
			{
				TInt iNumExported = 0;
				
				CEikTextListBox *iGroupsListBox = static_cast<CEikTextListBox*>(Control(cCellDBImportExportGroupListBox));
				if (iGroupsListBox->SelectionIndexes()->Count() > 0)
				{
					// This creates a text file with the following fields from Cells_List
					// Cell_Id	Country_Code	Network_Id	Description
					// This is a CSV file. All text is qualified with double quotes

					iImportExportPath.SetLength(0);
					CFolderBrowser *iFolderSelector = new (ELeave) CFolderBrowser();
					iFolderSelector->iPath = &iImportExportPath;
					iFolderSelector->iAllowSelection = EFalse;
					if (iFolderSelector->ExecuteLD(R_DIALOG_FOLDER_FILE_BROWSER) == EEikBidYes)
					{
						RFile iBaseOutputFile;
						CTextFileReader *iTextOutputFile = new (ELeave) CTextFileReader();

						TBool iProgressDialogActive = ETrue;
						
						CProgressDialog *iProgressDialog = new (ELeave) CProgressDialog();
						iProgressDialog->iDialogStillActive = &iProgressDialogActive;
						iProgressDialog->ExecuteLD(R_DIALOG_CELLS_IMPORTEXPORT_PROGRESS);
						iProgressDialog->SetButtonDim(ETrue);
						iProgressDialog->DrawNow();
								
						// Give dialog time to draw itself
						User::After(300000);
				
						// Check to make sure target folder exists
						TParsePtr iParsePtr(iImportExportPath);
						ConeUtils::EnsurePathExistsL(iParsePtr.DriveAndPath());
						TFileName iTargetFileName;
						TBool iHasError = EFalse;
						HBufC *iOutputBuffer = HBufC::NewL(10 + 8 + 4 + 50 + 3);
						HBufC *iTempOutputBuffer = HBufC::NewL(50 + 2);
						for (int i = 0; i < iGroupsListBox->SelectionIndexes()->Count(); i++)
						{
							if (iHasError)
								break;
								
							iHasError = EFalse;
							
							// Generate target file name
							iTargetFileName.Copy(iGroupsListBox->Model()->ItemTextArray()->MdcaPoint(iGroupsListBox->SelectionIndexes()->At(i)));
							
							if (iProgressDialogActive)
							{
								HBufC *iProcText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_PROCESSING);
								iProcText = iProcText->ReAllocL(iProcText->Length() + iTargetFileName.Length());
								iProcText->Des().Append(iTargetFileName);
								iProgressDialog->AppendText(-1, iProcText);
								delete iProcText;
							}
							
							TText iBadChar;
							TBuf<2> iBadDesc;
							while (!CEikonEnv::Static()->FsSession().IsValidName(iTargetFileName, iBadChar))
							{
								iBadDesc.Format(_L("%c"), iBadChar);
								iLocalCellDBHelper->ConvertTextStatement(&iTargetFileName, iBadDesc, _L("_"));
							}
							iTargetFileName.Insert(0, iImportExportPath);
							iTargetFileName.Append(_L(".CSV"));
							
							if (iProgressDialogActive)
							{
								HBufC *iProcText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_EXPORTINGAS);
								iProcText = iProcText->ReAllocL(iProcText->Length() + iTargetFileName.Length());
								iProcText->Des().Append(iTargetFileName);
								iProgressDialog->AppendText(-1, iProcText);
								delete iProcText;
							}
							
							// Initialize the text file
							if (iBaseOutputFile.Replace(CEikonEnv::Static()->FsSession(), iTargetFileName, EFileWrite | EFileStreamText) == KErrNone)
							{
								iTextOutputFile->Set(&iBaseOutputFile);
								
								// Query the database and begin dumping output values
								iLocalCellDBHelper->iSQLStatement.Format(_L("SELECT Cell_Id, Country_Code, Network_Id, Description FROM Cells_List WHERE Group_Id = %u ORDER BY Cell_Id"), iGroupIds->At(iGroupsListBox->SelectionIndexes()->At(i)));
								if (iLocalCellDBHelper->ReadDB())
								{
									TInt iNumRecords = iLocalCellDBHelper->iDBView.CountL();
									TInt iCurrentRecord = 0;
									
									while (iLocalCellDBHelper->iDBView.NextL())
									{
										iCurrentRecord++;
										iOutputBuffer->Des().Copy(_L("\""));
										iLocalCellDBHelper->iDBView.GetL();
										iOutputBuffer->Des().AppendNum(iLocalCellDBHelper->iDBView.ColUint32(1), 10);
										iOutputBuffer->Des().Append(_L("\","));
										
										iTempOutputBuffer->Des().Copy(iLocalCellDBHelper->iDBView.ColDes(2));
										CreateQualifiedTextField(iTempOutputBuffer);
										iOutputBuffer->Des().Append(*iTempOutputBuffer);
										iOutputBuffer->Des().Append(_L(","));

										iTempOutputBuffer->Des().Copy(iLocalCellDBHelper->iDBView.ColDes(3));
										CreateQualifiedTextField(iTempOutputBuffer);
										iOutputBuffer->Des().Append(*iTempOutputBuffer);
										iOutputBuffer->Des().Append(_L(","));
										
										iTempOutputBuffer->Des().Copy(iLocalCellDBHelper->iDBView.ColDes(4));
										CreateQualifiedTextField(iTempOutputBuffer);
										iOutputBuffer->Des().Append(*iTempOutputBuffer);
									
										if (iProgressDialogActive)
											iProgressDialog->SetProgressValue(TInt(TReal(iCurrentRecord) / TReal(iNumRecords) * TReal(100)));
									
										TPtr iPtr = TPtr(iOutputBuffer->Des());
										iTemp = &iPtr;
										if (iTextOutputFile->WriteLine(iTemp) != KErrNone)
										{
											if (iProgressDialogActive)
												iProgressDialog->AppendText(R_TBUF_CELLID_ERRCREATEOUTPUT, (HBufC *)(NULL));
											iHasError = ETrue;
											break;
										}
									}
									iLocalCellDBHelper->iDBView.Close();
								}
								iBaseOutputFile.Close();
								iNumExported++;
							}
							else
								CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRCREATEOUTPUT);
						}
						delete iOutputBuffer;
						delete iTempOutputBuffer;
						delete iTextOutputFile;
						
						if ((iProgressDialogActive) && (!iHasError))
						{
							HBufC *iProcText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_CELLID_NUMEXPORTED);
							iProcText = iProcText->ReAllocL(iProcText->Length() + 11);
							iProcText->Des().AppendNum(iNumExported);
							iProgressDialog->AppendText(-1, iProcText);
							delete iProcText;
						}
						
						if (iProgressDialogActive)
							iProgressDialog->SetButtonDim(EFalse);
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};
		case EEikBidOk :
			{
				TryExitL(EEikBidOk);
				break;
			};
	};
}

// Create a new cell
TInt CEditCellDBDialog::CreateCell(TUint aCellId, TUint aGroupId, TUint aGroupPriority, TUint aGroupState, TDes *aCountryCode, TDes *aNetworkCode, TDes *aCellDescription)
{
	iLocalCellDBHelper->iSQLStatement.Copy(_L("INSERT INTO Cells_List (Cell_Id,Country_Code,Network_Id,Description,Group_Id,Group_Priority,Group_State) VALUES ("));
	iLocalCellDBHelper->iSQLStatement.AppendNum(aCellId, 10);
	iLocalCellDBHelper->iSQLStatement.Append(_L(",'"));
	iLocalCellDBHelper->ConvertTextStatement(aCountryCode, _L("'"), _L("''"));
	iLocalCellDBHelper->iSQLStatement.Append(*aCountryCode);
	iLocalCellDBHelper->iSQLStatement.Append(_L("','"));
	iLocalCellDBHelper->ConvertTextStatement(aNetworkCode, _L("'"), _L("''"));
	iLocalCellDBHelper->iSQLStatement.Append(*aNetworkCode);
	iLocalCellDBHelper->iSQLStatement.Append(_L("','"));
	iLocalCellDBHelper->ConvertTextStatement(aCellDescription, _L("'"), _L("''"));
	iLocalCellDBHelper->iSQLStatement.Append(*aCellDescription);
	iLocalCellDBHelper->iSQLStatement.Append(_L("',"));
	iLocalCellDBHelper->iSQLStatement.AppendNum(aGroupId, 10);
	iLocalCellDBHelper->iSQLStatement.Append(_L(","));
	iLocalCellDBHelper->iSQLStatement.AppendNum(aGroupPriority, 10);
	iLocalCellDBHelper->iSQLStatement.Append(_L(","));
	iLocalCellDBHelper->iSQLStatement.AppendNum(aGroupState, 10);
	iLocalCellDBHelper->iSQLStatement.Append(_L(")"));

	return (iLocalCellDBHelper->ExecuteSQL());
}

// Given a group name, and state, 
//	create a new group and 
// return the group id and priority
TInt CEditCellDBDialog::CreateGroup(TDes *aGroupName, TInt aState, TUint *aGroupId, TUint *aPriority)
{
	TInt iRetVal = KErrGeneral;
	
	iLocalCellDBHelper->iSQLStatement.Copy(_L("INSERT INTO Groups_List (Group_Name, State) VALUES ('"));
	iLocalCellDBHelper->ConvertTextStatement(aGroupName, _L("'"), _L("''"));
	iLocalCellDBHelper->iSQLStatement.Append(*aGroupName);
	iLocalCellDBHelper->iSQLStatement.Append(_L("',"));
	iLocalCellDBHelper->iSQLStatement.AppendNum(aState);
	iLocalCellDBHelper->iSQLStatement.Append(_L(")"));

	iRetVal = iLocalCellDBHelper->ExecuteSQL();
	if (iRetVal == 1)
	{
		// Now that the group is inserted, we need to extract its group id so we can tie the cells back
		// to this group. The best way to do this is to read the table sorting by priority in descending
		// order. The first record should be the group we just created
		iLocalCellDBHelper->iSQLStatement.Copy(_L("SELECT Group_Id, Priority FROM Groups_List ORDER BY Priority DESC"));
		if (iLocalCellDBHelper->ReadDB())
		{
			iLocalCellDBHelper->iDBView.NextL();
			iLocalCellDBHelper->iDBView.GetL();
			*aGroupId = iLocalCellDBHelper->iDBView.ColUint32(1);
			*aPriority = iLocalCellDBHelper->iDBView.ColUint32(2);
			iRetVal = KErrNone;
		}
		iLocalCellDBHelper->iDBView.Close();
	}
	return iRetVal;
}

// This parses a field and inserts appropriate double quote characters into the field
// to form text that is ready to be written to the text file
void CEditCellDBDialog::CreateQualifiedTextField(HBufC *iBuffer)
{
	TPtr iPtr = TPtr(iBuffer->Des());
	TDes *iTempDes = &iPtr;
	iLocalCellDBHelper->ConvertTextStatement(iTempDes, _L("\""), _L("\"\""));
	iBuffer->Des().Insert(0, _L("\""));
	iBuffer->Des().Append(_L("\""));
}

TBool CEditCellDBDialog::ShiftGroup(TInt aIncrement)
{
	TBool iRetVal = EFalse;
	
	// Get currently focused item
	CEikColumnListBox *iGroupsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
	TInt iGroupIndex = iGroupsListBox->CurrentItemIndex();	
	if ((iGroupIndex + aIncrement >= 0) && (iGroupIndex + aIncrement < iGroupsListBox->Model()->ItemTextArray()->MdcaCount()))
	{
		// Do DB reordering first
		TUint iOldGroupId = iGroupIds->At(iGroupsListBox->CurrentItemIndex());
		TUint iNewGroupId = iGroupIds->At(iGroupsListBox->CurrentItemIndex() + aIncrement);
		
		TUint iOldPriority = iGroupPriorities->At(iGroupsListBox->CurrentItemIndex());
		TUint iNewPriority = iGroupPriorities->At(iGroupsListBox->CurrentItemIndex() + aIncrement);
		
		TUint iOldState = iGroupStates->At(iGroupsListBox->CurrentItemIndex());
		TUint iNewState = iGroupStates->At(iGroupsListBox->CurrentItemIndex() + aIncrement);
		
		CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
		if (iLocalCellDBHelper->BeginTransaction() == KErrNone)
		{
			iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Groups_List SET Priority = %u WHERE Group_Id = %u"), iNewPriority, iOldGroupId);
			if (iLocalCellDBHelper->ExecuteSQL() >= 0)
			{
				iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Groups_List SET Priority = %u WHERE Group_Id = %u"), iOldPriority, iNewGroupId);
				if (iLocalCellDBHelper->ExecuteSQL() >= 0)
				{
					iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Cells_List SET Group_Priority = %u, Group_State = %u WHERE Group_Id = %u"), iOldPriority, iOldState, iNewGroupId);
					if (iLocalCellDBHelper->ExecuteSQL() >= 0)
					{
						iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Cells_List SET Group_Priority = %u, Group_State = %u WHERE Group_Id = %u"), iNewPriority, iNewState, iOldGroupId);
						if (iLocalCellDBHelper->ExecuteSQL() >= 0)
						{
							iRetVal = ETrue;
							iLocalCellDBHelper->CommitTransaction();
						}
					}
				}
			}
			if (iRetVal == EFalse)
			{
				iLocalCellDBHelper->RollbackTransaction();
				// Rolling back transactions leaves indexes in a damaged state
				if (!iLocalCellDBHelper->RecoverDB())
					CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRRECOVERY);
				CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRMOVEGROUP);
			}
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRMOVEGROUP);
		CEikonEnv::Static()->BusyMsgCancel();
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_CANTMOVE);
	return iRetVal;
}

TBool CEditCellDBDialog::ShiftEvents(TInt aIncrement)
{
	TBool iRetVal = EFalse;
	
	// Get currently focused item
	CEikColumnListBox *iEventsListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
	TInt iEventIndex = iEventsListBox->CurrentItemIndex();
	if ((iEventIndex + aIncrement >= 0) && (iEventIndex + aIncrement < iEventsListBox->Model()->ItemTextArray()->MdcaCount()))
	{
		TUint iOldEventId = iEventIds->At(iEventsListBox->CurrentItemIndex());
		TUint iNewEventId = iEventIds->At(iEventsListBox->CurrentItemIndex() + aIncrement);
		
		TUint iOldPriority = iEventPriorities->At(iEventsListBox->CurrentItemIndex());
		TUint iNewPriority = iEventPriorities->At(iEventsListBox->CurrentItemIndex() + aIncrement);
		
		CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
		if (iLocalCellDBHelper->BeginTransaction() == KErrNone)
		{
			iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Events_List SET Priority = %u WHERE Event_Id = %u"), iNewPriority, iOldEventId);
			if (iLocalCellDBHelper->ExecuteSQL() >= 0)
			{
				iLocalCellDBHelper->iSQLStatement.Format(_L("UPDATE Events_List SET Priority = %u WHERE Event_Id = %u"), iOldPriority, iNewEventId);
				if (iLocalCellDBHelper->ExecuteSQL() >= 0)
				{
					iRetVal = ETrue;
					iLocalCellDBHelper->CommitTransaction();
				}
			}
			if (iRetVal == EFalse)
			{
				iLocalCellDBHelper->RollbackTransaction();
				// Rolling back transactions leaves indexes in a damaged state
				if (!iLocalCellDBHelper->RecoverDB())
					CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRRECOVERY);
				CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRMOVEEVENT);
			}
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_ERRMOVEEVENT);
		CEikonEnv::Static()->BusyMsgCancel();
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_CANTMOVE);
	return iRetVal;
}

void CEditCellDBDialog::UpdatePageButtons_ImportExport()
{
	if (ActivePageId() == idCellIdPage_ImportExport)
	{
		TBool iDimState = EFalse;
		if (iGroupNames->MdcaCount() < 1)
			iDimState = ETrue;
		PageButtonGroupContainer(idCellIdPage_ImportExport).DimCommand(EEikBidCellExport, iDimState);
	}
}

void CEditCellDBDialog::UpdatePageButtons_Events()
{
	if (ActivePageId() == idCellIdPage_Events)
	{
		TBool iDimState = EFalse;
		if ((iGroupNames->MdcaCount() < 1) || (static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox))->Model()->ItemTextArray()->MdcaCount() < 1))
			iDimState = ETrue;
			
		PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsEnable, iDimState);
		PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsDisable, iDimState);
		PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsShiftUp, iDimState);
		PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsShiftDown, iDimState);
		PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsDel, iDimState);
		PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsMove, iDimState);
		PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsEdit, iDimState);
		
		if (iGroupNames->MdcaCount() < 1)
			PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsNew, ETrue);
		else
			PageButtonGroupContainer(idCellIdPage_Events).DimCommand(EEikBidCellEventsNew, EFalse);		
	}
}

void CEditCellDBDialog::UpdatePageButtons_Cells()
{
	if (ActivePageId() == idCellIdPage_Cells)
	{
		TBool iDimState = EFalse;
		if ((iGroupNames->MdcaCount() < 1) || (static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox))->Model()->ItemTextArray()->MdcaCount() < 1))
			iDimState = ETrue;

		PageButtonGroupContainer(idCellIdPage_Cells).DimCommand(EEikBidCellCellsDel, iDimState);
		PageButtonGroupContainer(idCellIdPage_Cells).DimCommand(EEikBidCellCellsMove, iDimState);
		PageButtonGroupContainer(idCellIdPage_Cells).DimCommand(EEikBidCellCellsEdit, iDimState);
		
		if (iGroupNames->MdcaCount() < 1)
		{
			PageButtonGroupContainer(idCellIdPage_Cells).DimCommand(EEikBidCellCellsNew, ETrue);
			PageButtonGroupContainer(idCellIdPage_Cells).DimCommand(EEikBidCellCellsFind, ETrue);
		}
		else
		{
			PageButtonGroupContainer(idCellIdPage_Cells).DimCommand(EEikBidCellCellsNew, EFalse);
			PageButtonGroupContainer(idCellIdPage_Cells).DimCommand(EEikBidCellCellsFind, EFalse);
		}
	}
}

void CEditCellDBDialog::UpdatePageButtons_Groups()
{
	if (ActivePageId() == idCellIdPage_Groups)
	{
		TBool iDimState = EFalse;
		if (iGroupNames->MdcaCount() < 1)
			iDimState = ETrue;
			
		PageButtonGroupContainer(idCellIdPage_Groups).DimCommand(EEikBidCellGroupShiftUp, iDimState);
		PageButtonGroupContainer(idCellIdPage_Groups).DimCommand(EEikBidCellGroupShiftDown, iDimState);
		PageButtonGroupContainer(idCellIdPage_Groups).DimCommand(EEikBidCellGroupEnable, iDimState);
		PageButtonGroupContainer(idCellIdPage_Groups).DimCommand(EEikBidCellGroupDisable, iDimState);
		PageButtonGroupContainer(idCellIdPage_Groups).DimCommand(EEikBidCellGroupDel, iDimState);
		PageButtonGroupContainer(idCellIdPage_Groups).DimCommand(EEikBidCellGroupRen, iDimState);
	}
}

void CEditCellDBDialog::PageChangedL(TInt aPageId)
{
	if (aPageId == idCellIdPage_Groups)
		UpdatePageButtons_Groups();
	else if (aPageId == idCellIdPage_Events)
	{
		if (iGroupInfoOnEventsChanged)
			InitEventsPage(ETrue);
		UpdatePageButtons_Events();
	}
	else if (aPageId == idCellIdPage_Cells)
	{
		if (iGroupInfoOnCellsChanged)
			InitCellsPage(ETrue, 0, EFalse);
		UpdatePageButtons_Cells();
	}
	else if (aPageId == idCellIdPage_ImportExport)
	{
		if (iGroupInfoOnImportExportChanged)
			InitImportExportPage(ETrue);
		UpdatePageButtons_ImportExport();
	}
}

TKeyResponse CEditCellDBDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	TKeyResponse iRetVal = EKeyWasNotConsumed;
	
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyConfirm)
		{
			CEikColumnListBox *iListBox = NULL;
			switch (ActivePageId())
			{
				case idCellIdPage_Groups:
					{
						iListBox = static_cast<CEikColumnListBox*>(Control(cCellDBGroupsListBox));
						break;
					}
				case idCellIdPage_Cells:
					{
						iListBox = static_cast<CEikColumnListBox*>(Control(cCellDBCellsListBox));
						break;
					}
				case idCellIdPage_Events:
					{
						iListBox = static_cast<CEikColumnListBox*>(Control(cCellDBEventsListBox));
						break;
					}
				case idCellIdPage_ImportExport:
					{
						iListBox = static_cast<CEikColumnListBox*>(Control(cCellDBImportExportGroupListBox));
						break;
					}
			}
			if (iListBox->Model()->NumberOfItems() > 0)
			{
				iListBox->View()->ToggleItemL(iListBox->View()->CurrentItemIndex());
				iRetVal = EKeyWasConsumed;
			}
		}
	}
	
	if (iRetVal == EKeyWasNotConsumed)
		return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
	else
		return EKeyWasConsumed;
}

/*************************************************************
*
* Generic dialog that will allow the user to enter a new string
* or edit an existing string. Used by cell db editor dialog
*
**************************************************************/

TBool CEditCellDBGenericString::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		if ((iIsRequired) && ((static_cast<CEikGlobalTextEditor*>(Control(cString)))->TextLength() < 1))
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_GENERICEDITORDIALOG_STRREQUIRED);
			return EFalse;
		}
		else
		{
			static_cast<CEikGlobalTextEditor*>(Control(cString))->GetText(*iTheString);
			return ETrue;
		}
	}
	return ETrue;
}

void CEditCellDBGenericString::PreLayoutDynInitL()
{
	CEikGlobalTextEditor *iStringControl = static_cast<CEikGlobalTextEditor*>(Control(cString));
	iStringControl->SetTextLimit(iTextLimit);
	iStringControl->SetTextL(iTheString);
	iStringControl->CreateScrollBarFrameL();
	iStringControl->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
}

/*************************************************************
*
* Create new group dialog
*
**************************************************************/

TBool CEditCellDBNewGroup::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		if ((iIsRequired) && ((static_cast<CEikGlobalTextEditor*>(Control(cString)))->TextLength() < 1))
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_GENERICEDITORDIALOG_STRREQUIRED);
			return EFalse;
		}
		else
		{
			static_cast<CEikGlobalTextEditor*>(Control(cString))->GetText(*iTheString);
			if (static_cast<CEikCheckBox*>(Control(cCheckBox))->State() == CEikButtonBase::ESet)
				*iStatus = 1;
			else 
				*iStatus = 0;
			return ETrue;
		}
	}
	return ETrue;
}

void CEditCellDBNewGroup::PreLayoutDynInitL()
{
	CEikGlobalTextEditor *iStringControl = static_cast<CEikGlobalTextEditor*>(Control(cString));
	iStringControl->SetTextLimit(iTextLimit);
	iStringControl->SetTextL(iTheString);
	iStringControl->CreateScrollBarFrameL();
	iStringControl->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
}

/*************************************************************
*
* Create new cell dialog
*
**************************************************************/

TBool CEditCellDBNewCell::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		if (static_cast<CEikGlobalTextEditor*>(Control(cCountryCode))->TextLength() < 1)
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_REQUIRECOUNTRYCODE);
			return EFalse;
		}
		else if (static_cast<CEikGlobalTextEditor*>(Control(cNetworkCode))->TextLength() < 1)
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_REQUIRENETWORKID);
			return EFalse;
		}
		else
		{
			*iCellId = static_cast<CQikNumberEditor*>(Control(cCellId))->Value();
			static_cast<CEikGlobalTextEditor*>(Control(cCountryCode))->GetText(*iCountryCode);
			static_cast<CEikGlobalTextEditor*>(Control(cNetworkCode))->GetText(*iNetworkCode);
			static_cast<CEikGlobalTextEditor*>(Control(cCellDescription))->GetText(*iCellDescription);
			*iGroupId = static_cast<CEikChoiceList*>(Control(cCellGroups))->CurrentItem();
			return ETrue;
		}
	}
	return ETrue;
}

void CEditCellDBNewCell::PreLayoutDynInitL()
{
	static_cast<CQikNumberEditor*>(Control(cCellId))->SetValueL(*iCellId);
	static_cast<CEikGlobalTextEditor*>(Control(cCountryCode))->SetTextL(iCountryCode);
	static_cast<CEikGlobalTextEditor*>(Control(cNetworkCode))->SetTextL(iNetworkCode);
	static_cast<CEikGlobalTextEditor*>(Control(cCellDescription))->SetTextL(iCellDescription);
	static_cast<CEikGlobalTextEditor*>(Control(cCellDescription))->CreateScrollBarFrameL();
	static_cast<CEikGlobalTextEditor*>(Control(cCellDescription))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
		
	CEikChoiceList *iChoiceListControl = static_cast<CEikChoiceList*>(Control(cCellGroups));
	iChoiceListControl->SetArrayL(iGroupNames);
	iChoiceListControl->SetArrayExternalOwnership(ETrue);

	static_cast<CEikChoiceList*>(Control(cCellGroups))->SetCurrentItem(*iGroupId);
}

/*************************************************************
*
* Select group dialog. Used for moving a bunch of cells
*
**************************************************************/

TBool CEditCellDBMoveCells::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
		*iGroupId = static_cast<CEikChoiceList*>(Control(cCellGroups))->CurrentItem();
	return ETrue;
}

void CEditCellDBMoveCells::PreLayoutDynInitL()
{
	CEikChoiceList *iChoiceListControl = static_cast<CEikChoiceList*>(Control(cCellGroups));
	iChoiceListControl->SetArrayL(iGroupNames);
	iChoiceListControl->SetArrayExternalOwnership(ETrue);

	static_cast<CEikChoiceList*>(Control(cCellGroups))->SetCurrentItem(*iGroupId);
};

/*************************************************************
*
* Prompt cell id to find
*
**************************************************************/

TBool CEditCellDBFindCell::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
		*iCellId = static_cast<CQikNumberEditor*>(Control(cCellId))->Value();
	return ETrue;
}

void CEditCellDBFindCell::PreLayoutDynInitL()
{
	static_cast<CQikNumberEditor*>(Control(cCellId))->SetValueL(*iCellId);
}

/*************************************************************
*
* Found cells. Now, select your group
*
**************************************************************/

TBool CEditCellDBSelectGroup::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
		*iGroupIdIndex = static_cast<CEikTextListBox*>(Control(cCellGroups))->CurrentItemIndex();
	return ETrue;
}

void CEditCellDBSelectGroup::PreLayoutDynInitL()
{
	CEikTextListBox *iChoiceListControl = static_cast<CEikTextListBox*>(Control(cCellGroups));
	iChoiceListControl->CreateScrollBarFrameL();
	iChoiceListControl->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	iChoiceListControl->Model()->SetItemTextArray(iGroupNames);
	iChoiceListControl->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
}

/*************************************************************
*
* Select new event type to create
*
**************************************************************/

TBool CEditSelectEvent::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		static_cast<CEikGlobalTextEditor*>(Control(cString))->GetText(*iEventDescription);
		*iEventTypeIdIndex = static_cast<CEikChoiceList*>(Control(cChoiceList))->CurrentItem();
		*iGroupIdIndex = static_cast<CEikChoiceList*>(Control(cCellGroups))->CurrentItem();
		*iEventTriggerIdIndex = static_cast<CEikChoiceList*>(Control(cTriggerList))->CurrentItem();
		if (static_cast<CEikCheckBox*>(Control(cCheckBox))->State() == CEikButtonBase::ESet)
			*iState = 1;
		else 
			*iState = 0;
	}
	return ETrue;
}

void CEditSelectEvent::PreLayoutDynInitL()
{
	CEikChoiceList *iChoiceListControl = static_cast<CEikChoiceList*>(Control(cCellGroups));
	iChoiceListControl->SetArrayL(iGroupNames);
	iChoiceListControl->SetArrayExternalOwnership(ETrue);

	iChoiceListControl = static_cast<CEikChoiceList*>(Control(cChoiceList));
	iChoiceListControl->SetArrayL(iEventTypeDescriptions);
	iChoiceListControl->SetArrayExternalOwnership(ETrue);
	iChoiceListControl->SetCurrentItem(*iEventTypeIdIndex);	

	iChoiceListControl = static_cast<CEikChoiceList*>(Control(cTriggerList));
	iChoiceListControl->SetArrayL(iEventTriggerDescriptions);
	iChoiceListControl->SetArrayExternalOwnership(ETrue);
	
	static_cast<CEikChoiceList*>(Control(cCellGroups))->SetCurrentItem(*iGroupIdIndex);
	static_cast<CEikGlobalTextEditor*>(Control(cString))->SetTextL(iEventDescription);
	static_cast<CEikGlobalTextEditor*>(Control(cString))->CreateScrollBarFrameL();
	static_cast<CEikGlobalTextEditor*>(Control(cString))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	if (*iState == 1)
		static_cast<CEikCheckBox*>(Control(cCheckBox))->SetState(CEikButtonBase::ESet);
	else
		static_cast<CEikCheckBox*>(Control(cCheckBox))->SetState(CEikButtonBase::EClear);
};

/*************************************************************
*
* Create / edit audio event
*
**************************************************************/

void CEditCellNewAudioEvent::HandleControlStateChangeL(TInt aControlId)
{
	if (aControlId == cChoiceList)
	{
		CEikChoiceList *iAudioClipControl = STATIC_CAST(CEikChoiceList*, Control(cChoiceList));
		if (iAudioClipControl->CurrentItem() == 1)
			PickAudioFile();
	}
}

void CEditCellNewAudioEvent::PreLayoutDynInitL()
{
	CEikChoiceList *iAudioClipControl = STATIC_CAST(CEikChoiceList*, Control(cChoiceList));
	if (iSelectedAudioFileName->Length() == 0)
		iAudioClipControl->SetCurrentItem(0);
	else
	{
		TPtr iTempPtr = iSelectedAudioFileName->Des();
		TDes *iTemp = &iTempPtr;
		TParsePtr iParsedAudioFileName(*iTemp);
		iAudioClipControl->DesCArray()->AppendL(iParsedAudioFileName.NameAndExt());
		iAudioClipControl->SetCurrentItem(2);
	}
}

TBool CEditCellNewAudioEvent::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		CEikChoiceList *iAudioClipControl = STATIC_CAST(CEikChoiceList*, Control(cChoiceList));
		if (iAudioClipControl->CurrentItem() != 2)
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOAUDIOFILE);
			iSelectedAudioFileName->Des().SetLength(0);
			return EFalse;
		}
	}
	return ETrue;
}

void CEditCellNewAudioEvent::PickAudioFile(void)
{
	CDesCArray *iAudioMimeArray = new(ELeave) CDesCArrayFlat(2);
	CEikChoiceList *iAudioClipControl = STATIC_CAST(CEikChoiceList*, Control(cChoiceList));
	iAudioMimeArray->AppendL(_L("audio/x-wav"));		// WAV
	iAudioMimeArray->AppendL(_L("audio/x-midi"));		// MID
	iAudioMimeArray->AppendL(_L("audio/x-rmf"));		// RMF
	TFileName iTemp;
	iTemp.Copy(iSelectedAudioFileName->Des());
	if (CQikSelectMediaFileDlg::RunDlgLD(*iAudioMimeArray, iTemp))
	{
		if (iAudioClipControl->DesCArray()->MdcaCount() > 2)
			iAudioClipControl->DesCArray()->Delete(2);
		TParsePtr iParsedAudioFileName(iTemp);
		iAudioClipControl->DesCArray()->AppendL(iParsedAudioFileName.NameAndExt());
		iAudioClipControl->SetCurrentItem(2);
		iSelectedAudioFileName->Des().Copy(iTemp);
	}
	else
	{
		iAudioClipControl->SetCurrentItem(0);
		iSelectedAudioFileName->Des().SetLength(0);
	}
	iAudioClipControl->DesCArray()->Compress();
	delete iAudioMimeArray;	
}

/*************************************************************
*
* Edit audio event
*
**************************************************************/

void CEditCellEditAudioEvent::PreLayoutDynInitL()
{
	CEditCellNewAudioEvent::PreLayoutDynInitL();
	static_cast<CEikGlobalTextEditor*>(Control(cString))->SetTextL(iEventName);
	static_cast<CEikGlobalTextEditor*>(Control(cString))->CreateScrollBarFrameL();
	static_cast<CEikGlobalTextEditor*>(Control(cString))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

	static_cast<CEikChoiceList*>(Control(cTriggerList))->SetArrayL(iEventTriggerDescriptions);
	static_cast<CEikChoiceList*>(Control(cTriggerList))->SetArrayExternalOwnership(ETrue);	
	static_cast<CEikChoiceList*>(Control(cTriggerList))->SetCurrentItem(*iEventTriggerIdIndex);
}

TBool CEditCellEditAudioEvent::OkToExitL(TInt aButtonId)
{
	if (CEditCellNewAudioEvent::OkToExitL(aButtonId))
	{
		TPtr iTempPtr = iEventName->Des();
		TDes *iTemp = &iTempPtr;
		static_cast<CEikGlobalTextEditor*>(Control(cString))->GetText(*iTemp);
		*iEventTriggerIdIndex = static_cast<CEikChoiceList*>(Control(cTriggerList))->CurrentItem();
		return ETrue;
	}
	else
		return EFalse;
}

/*************************************************************
*
* Edit vibrate event
*
**************************************************************/

TBool CEditCellEditVibrateEvent::OkToExitL(TInt aButtonId)
{
	if (CEditCellDBGenericString::OkToExitL(aButtonId))
	{
		*iEventTriggerIdIndex = static_cast<CEikChoiceList*>(Control(cTriggerList))->CurrentItem();
		return ETrue;
	}
	else
		return EFalse;
}

void CEditCellEditVibrateEvent::PreLayoutDynInitL()
{
	CEditCellDBGenericString::PreLayoutDynInitL();
	
	static_cast<CEikChoiceList*>(Control(cTriggerList))->SetArrayL(iEventTriggerDescriptions);
	static_cast<CEikChoiceList*>(Control(cTriggerList))->SetArrayExternalOwnership(ETrue);	
	static_cast<CEikChoiceList*>(Control(cTriggerList))->SetCurrentItem(*iEventTriggerIdIndex);
}

/*************************************************************
*
* Select drive
*
**************************************************************/

CSelectDrive::CSelectDrive()
{
	iDriveLetters = new (ELeave) CDesCArrayFlat(2);
}

CSelectDrive::~CSelectDrive()
{
	delete iDriveLetters;
}

void CSelectDrive::PreLayoutDynInitL()
{
	// Enumerate all RAM drives.
	
	TVolumeInfo iVolInfo;
	TInt iError;
	TBuf<1> iDrive;

	for (int i = 0; i < KMaxDrives; i++)
	{
		iError = CEikonEnv::Static()->FsSession().Volume(iVolInfo, EDriveA + i);
		if (iError == KErrNone)
		{
			if (iVolInfo.iDrive.iType != EMediaRom)
			{
				iDrive.Format(_L("%c"), 0x41 + i);
				iDriveLetters->AppendL(iDrive);
			}
		}
	}	
	static_cast<CEikChoiceList*>(Control(cChoiceList))->SetArrayL(iDriveLetters);
	static_cast<CEikChoiceList*>(Control(cChoiceList))->SetArrayExternalOwnership(ETrue);		
}

TBool CSelectDrive::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
		iDriveLetter->Copy(iDriveLetters->MdcaPoint(static_cast<CEikChoiceList*>(Control(cChoiceList))->CurrentItem()));
	else
		iDriveLetter->SetLength(0);
	return ETrue;
}

/*************************************************************
*
* Given a listbox, return an array of selection indexes
*
**************************************************************/
	
void CSelectMultiListBox::PreLayoutDynInitL()
{
	CEikTextListBox *iListBox = static_cast<CEikTextListBox*>(Control(cCellDBImportExportFileListBox));
	iListBox->CreateScrollBarFrameL();
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	iListBox->Model()->SetItemTextArray(iListBoxItems);
	iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
}

TBool CSelectMultiListBox::OkToExitL(TInt aButtonId)
{
	iSelectionIndexes->Reset();
	if (aButtonId == EEikBidYes)
	{
		const CListBoxView::CSelectionIndexArray *iSelectionArray = static_cast<CEikTextListBox*>(Control(cCellDBImportExportFileListBox))->SelectionIndexes();
		if (iSelectionArray->Count() > 0)
		{
			for (int i = 0; i < iSelectionArray->Count(); i++)
				iSelectionIndexes->AppendL(iSelectionArray->At(i));
			return ETrue;
		}
		else
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
			return EFalse;
		}
	}
	return ETrue;
}

/*************************************************************
*
* Progress for cell import / export 
*
**************************************************************/

void CProgressDialog::PreLayoutDynInitL()
{
	static_cast<CEikProgressInfo*>(Control(cProgressInfo))->SetFinalValue(100);
	static_cast<CEikProgressInfo*>(Control(cProgressInfo))->SetAndDraw(0);
	static_cast<CEikGlobalTextEditor*>(Control(cString))->CreateScrollBarFrameL();
	static_cast<CEikGlobalTextEditor*>(Control(cString))->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
}

TKeyResponse CProgressDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cString))->MoveDisplayL(TCursorPosition::EFPageDown);
			static_cast<CEikGlobalTextEditor*>(Control(cString))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			static_cast<CEikGlobalTextEditor*>(Control(cString))->MoveDisplayL(TCursorPosition::EFPageUp);
			static_cast<CEikGlobalTextEditor*>(Control(cString))->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
}

void CProgressDialog::SetButtonDim(TBool aDim)
{
	ButtonGroupContainer().DimCommand(EEikBidOk, aDim);
}

void CProgressDialog::SetProgressValue(TInt aValue)
{
	static_cast<CEikProgressInfo*>(Control(cProgressInfo))->SetAndDraw(aValue);
}

void CProgressDialog::AppendText(TInt aResourceId, HBufC *aText)
{
	if (aText)
	{
		TPtr iPtr = TPtr(aText->Des());
		TDes *iTempDes = &iPtr;
		AppendText(aResourceId, iTempDes);
	}
	else
	{
		TBuf<1> iDummy;
		iDummy.Zero();
		AppendText(aResourceId, &iDummy);
	}
}

void CProgressDialog::AppendText(TInt aResourceId, TDes *aText)
{
	TBuf<2> iNewLine;
	
	CEikGlobalTextEditor *iTextEditor = static_cast<CEikGlobalTextEditor*>(Control(cString));
	iNewLine.Append(CEditableText::ELineBreak);
	iNewLine.Append(CEditableText::ELineBreak);
	if (aResourceId > -1)
	{
		HBufC *iResourceText = CEikonEnv::Static()->AllocReadResourceL(aResourceId);
		iTextEditor->InsertDeleteCharsL(iTextEditor->TextLength(), *iResourceText, TCursorSelection(0, 0));
		delete iResourceText;
		iTextEditor->InsertDeleteCharsL(iTextEditor->TextLength(), iNewLine, TCursorSelection(0, 0));
	}
	if (aText)
	{
		iTextEditor->InsertDeleteCharsL(iTextEditor->TextLength(), *aText, TCursorSelection(0, 0));
		iTextEditor->SetCursorPosL(iTextEditor->TextLength(), 0);
		iTextEditor->InsertDeleteCharsL(iTextEditor->TextLength(), iNewLine, TCursorSelection(0, 0));
	}
	iTextEditor->DrawNow();
	iTextEditor->UpdateScrollBarsL();
}

TBool CProgressDialog::OkToExitL(TInt /*aButtonId*/)
{
	*iDialogStillActive = EFalse;
	return ETrue;
}

/*************************************************************
*
* Select phone model to fool sman. This is the debug dialog
*
**************************************************************/
#ifdef __DEBUG_BUILD__
void CDebugDialog::PreLayoutDynInitL()
{
	CEikChoiceList *iChoiceList = static_cast<CEikChoiceList*>(Control(cChoiceList));
	switch (*iRunningPhoneModel)
	{
		case CSMan2AppUi::EPhoneModel_P800 : iChoiceList->SetCurrentItem(0); break;
		case CSMan2AppUi::EPhoneModel_P900 : iChoiceList->SetCurrentItem(1); break;
		case CSMan2AppUi::EPhoneModel_P910 : iChoiceList->SetCurrentItem(2); break;
		case CSMan2AppUi::EPhoneModel_A920 : iChoiceList->SetCurrentItem(3); break;
		case CSMan2AppUi::EPhoneModel_A920_C : iChoiceList->SetCurrentItem(4); break;
		case CSMan2AppUi::EPhoneModel_A925 : iChoiceList->SetCurrentItem(5); break;
		case CSMan2AppUi::EPhoneModel_A925_C : iChoiceList->SetCurrentItem(6); break;
		case CSMan2AppUi::EPhoneModel_P30 : iChoiceList->SetCurrentItem(7); break;
		case CSMan2AppUi::EPhoneModel_Unknown : iChoiceList->SetCurrentItem(8); break;
	}
}

TBool CDebugDialog::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		TInt iSelection = static_cast<CEikChoiceList*>(Control(cChoiceList))->CurrentItem();
		switch (iSelection)
		{
			case 0 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_P800; break;
			case 1 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_P900; break;
			case 2 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_P910; break;
			case 3 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_A920; break;
			case 4 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_A920_C; break;
			case 5 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_A925; break;
			case 6 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_A925_C; break;
			case 7 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_P30; break;
			case 8 : *iRunningPhoneModel = CSMan2AppUi::EPhoneModel_Unknown; break;
		}			
	}
	return ETrue;	
}
#endif

/*************************************************************
*
* Edit bluetooth exclusion list
*
**************************************************************/

void CEditBTExclusionList::PostLayoutDynInitL()
{
	CEikColumnListBox *iListBox = static_cast<CEikColumnListBox*>(Control(cListBox));
	iListBox->CreateScrollBarFrameL();
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	CColumnListBoxData *iListBoxColumns = ((CColumnListBoxItemDrawer*)iListBox->ItemDrawer())->ColumnData();
	iListBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ELeft);
	iListBoxColumns->SetColumnWidthPixelL(0, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 20);
	iListBoxColumns->SetColumnWidthPixelL(1, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * 12);
	
	RefreshListBox();
}

void CEditBTExclusionList::RefreshListBox()
{
	CEikColumnListBox *iListBox = static_cast<CEikColumnListBox*>(Control(cListBox));
	CDesCArray *iListBoxArray = (CDesCArray *)iListBox->Model()->ItemTextArray();
	iListBoxArray->Reset();
	iListBox->HandleItemRemovalL();
	
	if (iBTDeviceArray->Count() > 0)
	{
		HBufC *iTextBuffer = HBufC::NewL(iBTDeviceArray->At(0).iName.MaxLength() + 12 + 2);
		TBuf<15> iAddrHexBuffer;
		for (int i = 0; i < iBTDeviceArray->Count(); i++)
		{
			iTextBuffer->Des().Copy(iBTDeviceArray->At(i).iName);
			iTextBuffer->Des().Append(KColumnListSeparator);
			iAddrHexBuffer.Format(_L("0x%02x%02x%02x%02x%02x%02x"), iBTDeviceArray->At(i).iAddress[0], 
				iBTDeviceArray->At(i).iAddress[1], iBTDeviceArray->At(i).iAddress[2], iBTDeviceArray->At(i).iAddress[3], 
				iBTDeviceArray->At(i).iAddress[4], iBTDeviceArray->At(i).iAddress[5]);
			iTextBuffer->Des().Append(iAddrHexBuffer);
			iListBoxArray->AppendL(*iTextBuffer);
		}
		delete iTextBuffer;
	}
	iListBox->HandleItemRemovalL();
}

TBool CEditBTExclusionList::OkToExitL(TInt aButtonId)
{
	TBool iRetVal = EFalse;
	CEikColumnListBox *iListBox = static_cast<CEikColumnListBox*>(Control(cListBox));
	CDesCArray *iListBoxArray = (CDesCArray *)iListBox->Model()->ItemTextArray();
	switch (aButtonId)
	{
		case EEikBidAdd :
			{
				CBTDeviceArray *iDevices = new (ELeave) CBTDeviceArray(1);
				TInt iServiceFilter = -1;

				CQBTUISelectDialog *iBTDialog = new (ELeave)CQBTUISelectDialog(iDevices, CQBTUISelectDialog::EQBTDeviceFilterAll, iServiceFilter);
				if (iBTDialog->LaunchMultipleSelectDialogLD())
				{
					CScanner::TBTDevice iOneDevice;
					for (int i = 0; i < iDevices->Count(); i++)
					{
						if (iDevices->At(i)->DeviceName().Length() > KMaxDeviceName)
							iOneDevice.iName.Copy(iDevices->At(i)->DeviceName().Left(KMaxDeviceName));
						else
							iOneDevice.iName.Copy(iDevices->At(i)->DeviceName());
						iOneDevice.iAddress.Des().Copy(iDevices->At(i)->BDAddr().Des());
						
						// Is this a duplicate device?
						TBool iIsDuplicate = EFalse;
						for (int j = 0; j < iBTDeviceArray->Count(); j++)
						{
							if (iBTDeviceArray->At(j).iAddress == iOneDevice.iAddress)
							{
								iIsDuplicate = ETrue;
								break;
							}
						}
						if (!iIsDuplicate)
							iBTDeviceArray->AppendL(iOneDevice);
					}
					RefreshListBox();
					iListBox->UpdateScrollBarsL();
					iListBox->DrawDeferred();
				}
				delete iDevices;
				break;
			};
		case EEikBidDel :
			{
				if (iListBox->SelectionIndexes()->Count() > 0)
				{
					HBufC *iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
					HBufC *iWarning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_DELETE);
					if (CEikonEnv::Static()->QueryWinL(*iTitle, *iWarning))
					{
						delete iTitle;
						delete iWarning;
				
						CDesCArray *iDeleteList = new (ELeave) CDesCArraySeg(iListBox->SelectionIndexes()->Count());

						// Make a list of items to be deleted
						for (int i = 0; i < iListBox->SelectionIndexes()->Count(); i++)
							iDeleteList->AppendL(iListBoxArray->MdcaPoint(iListBox->SelectionIndexes()->At(i)));
							
						// Now do deletion
						TInt iIndex;
						for (int i = 0; i < iDeleteList->MdcaCount(); i++)
						{
							if (iListBoxArray->Find(iDeleteList->MdcaPoint(i), iIndex) == 0)
							{
								iListBoxArray->Delete(iIndex);
								iBTDeviceArray->Delete(iIndex);
							}
						}
						iListBox->HandleItemRemovalL();
						iListBox->UpdateScrollBarsL();
						iListBox->DrawDeferred();
					}
					else
					{
						delete iTitle;
						delete iWarning;
					}
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMSSELECTED);
				break;
			};
		case EEikBidEdit :
			{
				// Extract the device name. This is the only thing you can modify.
				if (iListBoxArray->MdcaCount() > 0)
				{
					TPtrC iPtr;
					TBTDeviceName iDeviceName;
					HBufC *iBuffer = HBufC::NewL(KMaxDeviceName + 15);
					TInt iCurrentIndex = iListBox->CurrentItemIndex();
					
					iBuffer->Des().Copy(iListBoxArray->MdcaPoint(iCurrentIndex));
					TextUtils::ColumnText(iPtr, 0, iBuffer);
					iDeviceName.Copy(iPtr);

					CEditBTDevice *iEditDialog = new (ELeave) CEditBTDevice();
					iEditDialog->iTextLimit = KMaxDeviceName;
					iEditDialog->iTheString = &iDeviceName;
					iEditDialog->iIsRequired = ETrue;
					if (iEditDialog->ExecuteLD(R_DIALOG_BLUEJACK_EDITDEVICE) == EEikBidYes)
					{
						TextUtils::ColumnText(iPtr, 1, iBuffer);
						HBufC *iBuffer2 = HBufC::NewL(KMaxDeviceName + 15);
						iBuffer2->Des().Copy(iDeviceName);
						iBuffer2->Des().Append(KColumnListSeparator);
						iBuffer2->Des().Append(iPtr);
						iListBoxArray->Delete(iCurrentIndex);
						iListBoxArray->InsertL(iCurrentIndex, *iBuffer2);
						iListBox->DrawDeferred();
						
						// Update the bluetooth device array
						iBTDeviceArray->At(iCurrentIndex).iName.Copy(iDeviceName);						
					}
					delete iBuffer;
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_NOITEMS);
				break;
			};
		case EEikBidOk :
			{
				// try to save it...saving is done by the caller
				iRetVal = ETrue;
				break;
			};
	}
	return iRetVal;
}

/*************************************************************
*
* Edit one BT device
*
**************************************************************/

TBool CEditBTDevice::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		if ((iIsRequired) && ((static_cast<CEikGlobalTextEditor*>(Control(cString)))->TextLength() < 1))
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_GENERICEDITORDIALOG_STRREQUIRED);
			return EFalse;
		}
		else
		{
			static_cast<CEikGlobalTextEditor*>(Control(cString))->GetText(*iTheString);
			return ETrue;
		}
	}
	else
		return ETrue;
}

/*************************************************************
*
* Specify agenda todo priority filter
*
**************************************************************/

TBool CAgendaMaxTodoPriority::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		TInt iCurrentItem = static_cast<CEikChoiceList*>(Control(cChoiceList))->CurrentItem();
		if (iCurrentItem == 0)
			*iMaxPriority = KMaxTUint;
		else
			*iMaxPriority = static_cast<CEikChoiceList*>(Control(cChoiceList))->CurrentItem();
	}
	return ETrue;
}

void CAgendaMaxTodoPriority::PreLayoutDynInitL()
{
	CEikChoiceList *iChoiceList = static_cast<CEikChoiceList*>(Control(cChoiceList));
	
	if (*iMaxPriority == KMaxTUint)
		iChoiceList->SetCurrentItem(0);
	else
		iChoiceList->SetCurrentItem(*iMaxPriority);
}

/*************************************************************
*
* Folder selector
*
**************************************************************/

/*
I know there is a CEikFileNameSelector but i've decided not to use it here because you cannot do
disjointed selections if your file is beyond the current pop-up listbox window (i.e. you need
to scroll the list). This is because files are selected using the drag event.

The other problem is the lack of display space to show filenames that are too long. The listbox
that i use solves this problem.

Quite a pity actually. The default controls make it so easy to do everything. :(
*/
/*
TBool CFolderSelector::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
	{
		CEikFolderNameSelector *iFolderSelector = static_cast<CEikFolderNameSelector*>(Control(cFolderSelector));
		iFolder->Copy(iFolderSelector->FullName());
	}
	return ETrue;
}

void CFolderSelector::PreLayoutDynInitL()
{
	CEikFolderNameSelector *iFolderSelector = static_cast<CEikFolderNameSelector*>(Control(cFolderSelector));
	CEikDriveNameSelector *iDriveSelector = static_cast<CEikDriveNameSelector*>(Control(cDriveSelector));

	iFolderSelector->SetShowSystem(EFalse);	

	iFolderSelector->SetFullNameL(_L("C:\\"));
	iDriveSelector->SetFullNameL(_L("C:\\"));
	
	iDriveSelector->SetFileSelectionObserver(iFolderSelector);
	
	iDriveSelector->ShowRom(EFalse);
	iDriveSelector->AllowCorruptDisk(EFalse);
}
*/

/*************************************************************
*
* Cell area event firing delay
*
**************************************************************/

TBool CCellEventDelay::OkToExitL(TInt aButtonId)
{
	if (aButtonId == EEikBidYes)
		*iDelay = static_cast<CQikNumberEditor*>(Control(cNumberEditor))->Value();
	return ETrue;
}

void CCellEventDelay::PreLayoutDynInitL()
{
	static_cast<CQikNumberEditor*>(Control(cNumberEditor))->SetValueL(*iDelay);	
}

/*************************************************************
*
* Folder browser
*
**************************************************************/

// This is a class that inherits from CEikColumnListBox. Its primary purpose is to trap
// pointer events and key presses for custom handling. This class is used in CFolderBrowser
void CColumnListBoxWithPointerHandler::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	TInt iItemTapped;
	TBool iPointerValid = View()->XYPosToItemIndex(aPointerEvent.iPosition, iItemTapped);
	TInt iEntryType = 2;
	TBool iRequireFolderNavigation = EFalse;
	
	if ((aPointerEvent.iType == TPointerEvent::EButton1Down) || (aPointerEvent.iType == TPointerEvent::EDrag))
	{
		if (iPointerValid)
		{
			iEntryType = GetEntryType(iItemTapped);
			if (iDialogContainer->iAllowSelection)
			{
				// This is not a file
				if (iEntryType != 2)
				{
					if (aPointerEvent.iPosition.iX <= View()->ItemDrawer()->MarkColumn())
					{
						if (aPointerEvent.iType == TPointerEvent::EButton1Down)
							CEikonEnv::Static()->InfoMsg(R_TBUF_FOLDERFILE_BROWSER_ONLYSELECTFILE);
					}
					else if (aPointerEvent.iType == TPointerEvent::EButton1Down)
					{
						iRequireFolderNavigation = ETrue;
						View()->SetCurrentItemIndex(iItemTapped);
					}
					iPointerValid = EFalse;
				}
			}
			else if (aPointerEvent.iType != TPointerEvent::EDrag)
			{
				iRequireFolderNavigation = ETrue;
				View()->SetCurrentItemIndex(iItemTapped);
				iPointerValid = EFalse;
			}
		}
		else
			iPointerValid = EFalse;
	}
		
	if (iPointerValid)
		CEikTextListBox::HandlePointerEventL(aPointerEvent);
	
	if (iRequireFolderNavigation)
		iDialogContainer->ProcessFolderNavigation();
}

CColumnListBoxWithPointerHandler::CColumnListBoxWithPointerHandler()
{
	iSelectedEntry = HBufC::NewL(KMaxFileName + 4);
}

CColumnListBoxWithPointerHandler::~CColumnListBoxWithPointerHandler()
{
	delete iSelectedEntry;
}

TInt CColumnListBoxWithPointerHandler::GetEntryType(TInt aItem)
{
	TPtrC iTempText;
	TInt iEntryType;
	TLex iLexConverter;
	
	iSelectedEntry->Des().Copy(Model()->ItemText(aItem));
	TPtr iPtr = TPtr(iSelectedEntry->Des());
	TDes *iTemp = &iPtr;
	TextUtils::ColumnText(iTempText, 0, iTemp, KColumnListSeparator);
	iLexConverter.Assign(iTempText);
	iLexConverter.Val(iEntryType);
	
	return iEntryType;
}

TKeyResponse CColumnListBoxWithPointerHandler::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyConfirm)
		{
			if (iDialogContainer->iAllowSelection)
			{
				if (GetEntryType(CurrentItemIndex()) == 2)
				{
					View()->SelectItemL(CurrentItemIndex());
					DrawDeferred();
				}
				else
					iDialogContainer->ProcessFolderNavigation();
			}
			else
				iDialogContainer->ProcessFolderNavigation();
			return EKeyWasConsumed;
		}
	}
	return CEikColumnListBox::OfferKeyEventL(aKeyEvent, aType);
}

void CColumnListBoxWithPointerHandler::ConstructFromResourceL(TResourceReader& aReader)
{
	CEikTextListBox::ConstructFromResourceL(aReader);
}

/***********************************/

SEikControlInfo CFolderBrowser::CreateCustomControlL(TInt aControlType)
{
	SEikControlInfo iControlInfo;
	
	iControlInfo.iControl = NULL;
	iControlInfo.iTrailerTextId = 0;
	iControlInfo.iFlags = 0;
	
	switch (aControlType)
	{
		case EEikTextListBoxWithPointerHandler :
			{
				iControlInfo.iControl = new (ELeave) CColumnListBoxWithPointerHandler;
				static_cast<CColumnListBoxWithPointerHandler*>(iControlInfo.iControl)->iDialogContainer = this;
				iControlInfo.iControl->SetObserver(this);
				break;
			};
		default : break;
	}
	return iControlInfo;
}

TKeyResponse CFolderBrowser::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	TKeyResponse iRetVal = static_cast<CColumnListBoxWithPointerHandler*>(Control(cListBox))->OfferKeyEventL(aKeyEvent, aType);
	if (iRetVal == EKeyWasNotConsumed)
		return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
	else
		return EKeyWasConsumed;
}

CFolderBrowser::CFolderBrowser()
{
	iFilesSelected = NULL;
	iAllowSelection = EFalse;
}

void CFolderBrowser::ProcessFolderNavigation()
{
	TBool iFolderChanged = EFalse;
	CColumnListBoxWithPointerHandler *iTextListBox = static_cast<CColumnListBoxWithPointerHandler*>(Control(cListBox));
	if (iTextListBox->Model()->NumberOfItems() > 0)
	{
		iSelectedEntry.Copy(iTextListBox->Model()->ItemText(iTextListBox->CurrentItemIndex()));
		TextUtils::ColumnText(iTempText, 1, &iSelectedEntry, KColumnListSeparator);
		iSelectedPath.Copy(iTempText);
		TextUtils::ColumnText(iTempText, 0, &iSelectedEntry, KColumnListSeparator);
		iLexConverter.Assign(iTempText);
		iLexConverter.Val(iEntryType);

		if (iEntryType != 2)
		{
			if (iSelectedPath.Compare(_L("..")) == 0)
			{
				iCurrentPath.SetLength(iCurrentPath.Length() - 1); // Remove '\' at the end
				iCurrentPath.SetLength(iCurrentPath.LocateReverse(TChar('\\')));
			}
			else if ((iSelectedPath.Length() == 2) && (iSelectedPath.Right(1).Compare(_L(":")) == 0))
				iCurrentPath.Copy(iSelectedPath);
			else
				iCurrentPath.Append(iSelectedPath);
			iCurrentPath.Append(_L("\\"));
			iFolderChanged = ETrue;
		}
	}
	if (iFolderChanged)
	{
		ReadFolder();
	}
}

void CFolderBrowser::SetItemSelected(TBool aIsSelected)
{
	if (iAllowSelection)
	{
		CColumnListBoxWithPointerHandler *iTextListBox = static_cast<CColumnListBoxWithPointerHandler*>(Control(cListBox));
		CDesCArray *iTextArray = (CDesCArray *)(iTextListBox->Model()->ItemTextArray());
		for (int i = 0; i < iTextArray->Count(); i++)
		{
			iSelectedEntry.Copy(iTextArray->MdcaPoint(i));
			TextUtils::ColumnText(iTempText, 0, &iSelectedEntry, KColumnListSeparator);
			iLexConverter.Assign(iTempText);
			iLexConverter.Val(iEntryType);
			if (iEntryType == 2)
			{
				if (aIsSelected)
					iTextListBox->View()->SelectItemL(i);
				else
					iTextListBox->View()->DeselectItem(i);
			}
		}
	}
}

TBool CFolderBrowser::OkToExitL(TInt aButtonId)
{
	TBool iRetVal = ETrue;
	switch (aButtonId)
	{
		case EEikBidYes :
			{
				iPath->Copy(iCurrentPath);
				if (iFilesSelected)
				{
					CColumnListBoxWithPointerHandler *iTextListBox = static_cast<CColumnListBoxWithPointerHandler*>(Control(cListBox));
					for (int i = 0; i < iTextListBox->SelectionIndexes()->Count(); i++)
					{
						iSelectedEntry.Copy(iTextListBox->Model()->ItemText(iTextListBox->SelectionIndexes()->At(i)));
						TextUtils::ColumnText(iTempText, 1, &iSelectedEntry, KColumnListSeparator);
						iFilesSelected->AppendL(iTempText);
					}
				}
				break;
			};
		case EEikBidSelectAll :
			{
				SetItemSelected(ETrue);
				iRetVal = EFalse;
				break;
			};
		case EEikBidSelectNone :
			{
				SetItemSelected(EFalse);
				iRetVal = EFalse;
				break;
			};
		default : break;
	}
	return iRetVal;
}

void CFolderBrowser::ReadFolder()
{
	CColumnListBoxWithPointerHandler *iTextListBox = static_cast<CColumnListBoxWithPointerHandler*>(Control(cListBox));
	CDir *iDirList = NULL;
	HBufC *iPathFilter = HBufC::NewL(KMaxFileName + 4);
	TInt iAttribFilter;
	TInt i, iMaxEntryLength = 3;

	// Clear the list box in preparation for repopulation
	CDesCArray *iTextArray = (CDesCArray *)(iTextListBox->Model()->ItemTextArray());
	iTextListBox->ClearSelection();
	iTextArray->Reset();
	iTextListBox->HandleItemRemovalL();

	iPathFilter->Des().Copy(iCurrentPath);
	iPathFilter->Des().Append(_L("*.*"));
	// Get folders first
	if (CEikonEnv::Static()->FsSession().GetDir(*iPathFilter, KEntryAttDir | KEntryAttMatchExclusive, EDirsFirst | EAscending | ESortByName, iDirList) == KErrNone)
	{
		// Repopulate the listbox
		if (iCurrentPath.Length() > 3)
		{
			iPathFilter->Des().Copy(_L("1"));
			iPathFilter->Des().Append(KColumnListSeparator);
			iPathFilter->Des().Append(_L(".."));
			iTextArray->AppendL(*iPathFilter);
		}
		for (i = 0; i < iDirList->Count(); i++)
		{
			if ((*iDirList)[i].IsDir())
				iPathFilter->Des().Copy(_L("1"));
			else
				iPathFilter->Des().Copy(_L("2"));
			iPathFilter->Des().Append(KColumnListSeparator);
			iPathFilter->Des().Append((*iDirList)[i].iName);
			iTextArray->AppendL(*iPathFilter);
			if ((*iDirList)[i].iName.Length() > iMaxEntryLength)
				iMaxEntryLength = (*iDirList)[i].iName.Length();
		}
		
		// Now, get files if necessary
		if (iAllowSelection)
		{
			iPathFilter->Des().Copy(iCurrentPath);
			iPathFilter->Des().Append(iFileSpec);
			iAttribFilter = KEntryAttMaskSupported;
			
			if (CEikonEnv::Static()->FsSession().GetDir(*iPathFilter, KEntryAttDir | KEntryAttMatchExclude, EDirsFirst | EAscending | ESortByName, iDirList) == KErrNone)
			{
				for (i = 0; i < iDirList->Count(); i++)
				{
					iPathFilter->Des().Copy(_L("2"));
					iPathFilter->Des().Append(KColumnListSeparator);
					iPathFilter->Des().Append((*iDirList)[i].iName);
					iTextArray->AppendL(*iPathFilter);
					if ((*iDirList)[i].iName.Length() > iMaxEntryLength)
						iMaxEntryLength = (*iDirList)[i].iName.Length();
				}
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRREADFOLDER);
		}
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRREADFOLDER);
		
	// Add drives			
	TVolumeInfo iVolumeInfo;	
	for (i = 0; i < KMaxDrives; i++)
	{
		if (CEikonEnv::Static()->FsSession().Volume(iVolumeInfo, EDriveA + i) == KErrNone)
		{
			iPathFilter->Des().Format(_L("%d%c%c:"), 0, KColumnListSeparator, 0x41 + i);
			iTextArray->AppendL(*iPathFilter);
		}
	}		
	iTextListBox->HandleItemRemovalL();

	if (iDirList)
		delete iDirList;
	delete iPathFilter;

	//::Static()->NormalFont()->TextWidthInPixels(iSelectedEntry));
	iTextListBox->ItemDrawer()->ColumnData()->SetColumnWidthPixelL(1, CEikonEnv::Static()->NormalFont()->MaxCharWidthInPixels() * iMaxEntryLength);
	iTextListBox->DrawNow();
}

void CFolderBrowser::PostLayoutDynInitL()
{
	if (iAllowSelection)
		SetTitleL(R_TBUF_ALT_TITLE_FILE_FOLDER_BROWSER_DIALOG);
}

void CFolderBrowser::PreLayoutDynInitL()
{
	CColumnListBoxWithPointerHandler *iTextListBox = static_cast<CColumnListBoxWithPointerHandler*>(Control(cListBox));
	iTextListBox->CreateScrollBarFrameL();
	iTextListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	iCurrentPath.Copy(*iPath);
	if (iCurrentPath.Length() < 1)
		iCurrentPath.Copy(_L("C:\\"));
	if (iAllowSelection)
	{
		if (iFileSpec.Length() < 1)
			iFileSpec.Copy(_L("*.*"));
	}
	else
		iTextListBox->ItemDrawer()->SetDrawMark(EFalse);

	CArrayPtr<CGulIcon>* iIconArray = new CArrayPtrFlat<CGulIcon>(2);
	CFbsBitmap *iBitmapDrive = new (ELeave) CFbsBitmap();
	CFbsBitmap *iBitmapDriveMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmapDrive->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderDrive));
	User::LeaveIfError(iBitmapDriveMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderDrivemask));
	iIconArray->AppendL(CGulIcon::NewL(iBitmapDrive, iBitmapDriveMask));
	CFbsBitmap *iBitmapFolder = new (ELeave) CFbsBitmap();
	CFbsBitmap *iBitmapFolderMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmapFolder->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderFolder));
	User::LeaveIfError(iBitmapFolderMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderFoldermask));
	iIconArray->AppendL(CGulIcon::NewL(iBitmapFolder, iBitmapFolderMask));
	CFbsBitmap *iBitmapFile = new (ELeave) CFbsBitmap();
	CFbsBitmap *iBitmapFileMask = new (ELeave) CFbsBitmap();
	User::LeaveIfError(iBitmapFile->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderFile));
	User::LeaveIfError(iBitmapFileMask->Load(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderFilemask));
	iIconArray->AppendL(CGulIcon::NewL(iBitmapFile, iBitmapFileMask));	
	iTextListBox->ItemDrawer()->ColumnData()->SetIconArray(iIconArray);
	
	iTextListBox->ItemDrawer()->ColumnData()->SetGraphicsColumnL(0, ETrue);
	iTextListBox->ItemDrawer()->ColumnData()->SetColumnWidthPixelL(0, 16);

	if (!iAllowSelection)
	{
		ButtonGroupContainer().MakeCommandVisible(EEikBidSelectAll, EFalse);
		ButtonGroupContainer().MakeCommandVisible(EEikBidSelectNone, EFalse);
	}
	
	ReadFolder();
}