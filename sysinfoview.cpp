#include "sysinfoview.h"
#include "sman.h"
#include <SMan.rsg>
#include <plpvariant.h>		// PlpVariant

/*************************************************************
*
* Sysinfo view.
*
**************************************************************/

CSMan2SysInfoView::CSMan2SysInfoView(CConfig *cData) : CViewBase(cData)
{
}

TBool CSMan2SysInfoView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2SysInfoView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2SysInfoView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
		cSysInfoListBox->Reset();
		((CDesCArray *) cSysInfoListBox->Model()->ItemTextArray())->Compress();
		cSysInfoListBox->HandleItemAdditionL();
	}
}

void CSMan2SysInfoView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
	refreshSysInfo();
}

TVwsViewId CSMan2SysInfoView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidSysInfoView);
}

CSMan2SysInfoView::~CSMan2SysInfoView()
{
	// DEBUG
	releaseFont();
}

void CSMan2SysInfoView::showDetail()
{
	// Pop up the detail view for that item
	HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_INFO_TITLE);
	TPtrC currentItem, itemName, itemValue;
	
	currentItem.Set(cSysInfoListBox->Model()->ItemTextArray()->MdcaPoint(cSysInfoListBox->View()->CurrentItemIndex()));
	TextUtils::ColumnText(itemName, 0, &currentItem);
	TextUtils::ColumnText(itemValue, 1, &currentItem);

	HBufC *message = HBufC::NewL(itemName.Length() + itemValue.Length() + 5);
	message->Des().Copy(itemName);
	message->Des().Append(CEditableText::ELineBreak);
	message->Des().Append(itemValue);
	CEikonEnv::Static()->InfoWinL(*dataBuffer, *message);

	delete dataBuffer;
	delete message;
}

TKeyResponse CSMan2SysInfoView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	TKeyResponse retVal = cSysInfoListBox->OfferKeyEventL(aKeyEvent, aType);
	if ((retVal == EKeyWasConsumed) && (aType == EEventKey) && (aKeyEvent.iCode == EQuartzKeyConfirm))
		showDetail();
	return retVal;
}

void CSMan2SysInfoView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	CCoeControl::HandlePointerEventL(aPointerEvent);
	if (aPointerEvent.iType == TPointerEvent::EButton1Up)
	{
		TInt theItem;
		TBool pointValid = cSysInfoListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, theItem);
		if (pointValid)
			showDetail();
	}
}

void CSMan2SysInfoView::refreshSysInfo()
{
	CDesCArray *iArray;
	TInt halInt;
	TBuf<256> tempText;
	TBuf<10> generalBuffer;
	TBuf<15> hexConvertBuffer;
	TLocale iLocale;

	CEikonEnv::Static()->BusyMsgL(_L("Busy..."), TGulAlignment(EHRightVTop));
	
	cSysInfoListBox->ClearSelection();
	iArray = ((CDesCArray *) cSysInfoListBox->Model()->ItemTextArray());
	iArray->Reset();
	iArray->Compress();
	
	cSysInfoListBox->HandleItemRemovalL();

	// CPU Speed
	HAL::Get(HALData::ECPUSpeed, halInt);
	tempText.Format(_L("CPU Speed%c%dMhz"), KColumnListSeparator, TInt(halInt / 1000));
	iArray->AppendL(tempText);

	// Manufacturer
	tempText.Format(_L("Manufacturer UID%c"), KColumnListSeparator);
	HAL::Get(HALData::EManufacturer, halInt);
	switch (halInt)
	{
		case HALData::EManufacturer_Ericsson : tempText.Append(_L("Ericsson")); break;
		case HALData::EManufacturer_Motorola : tempText.Append(_L("Motorola")); break;
		case HALData::EManufacturer_Nokia : tempText.Append(_L("Nokia")); break;
		case HALData::EManufacturer_Psion : tempText.Append(_L("Psion")); break;
		case HALData::EManufacturer_Intel : tempText.Append(_L("Intel")); break;
		case 0x101F6CED : tempText.Append(_L("SonyEricsson")); break;
		case 0x101fd278 : tempText.Append(_L("BenQ")); break;
		default : 
			{
				hexConvertBuffer.Format(_L("0x%x"), halInt);
				tempText.Append(hexConvertBuffer);
				break;
			}
	}
	iArray->AppendL(tempText);
	
	// Machine UID
	tempText.Format(_L("Machine UID%c"), KColumnListSeparator);
	HAL::Get(HALData::EMachineUid, halInt);
	switch (halInt)
	{
		case HALData::EMachineUid_Win32Emulator : tempText.Append(_L("Win32 Emulator")); break;
		case 0x101F408B : tempText.Append(_L("P800")); break;
		case 0x101FB2AE : tempText.Append(_L("P900")); break;
		case 0x10200AC6 : tempText.Append(_L("P910")); break;
		case 0x101F6B26 : tempText.Append(_L("A920")); break;
		case 0x101F6B27 : tempText.Append(_L("A925")); break;
		case 0x101FD279 : tempText.Append(_L("P30")); break;
		default : 
			{
				hexConvertBuffer.Format(_L("0x%x"), halInt);
				tempText.Append(hexConvertBuffer);
				break;
			}
	}
	iArray->AppendL(tempText);

	// Device family
	tempText.Format(_L("Device family%c"), KColumnListSeparator);
	HAL::Get(HALData::EDeviceFamily, halInt);
	switch (halInt)
	{
		case HALData::EDeviceFamily_Crystal : tempText.Append(_L("Crystal")); break;
		case HALData::EDeviceFamily_Quartz : tempText.Append(_L("Quartz")); break;
		case HALData::EDeviceFamily_Pearl : tempText.Append(_L("Pearl")); break;
		default :
			{
				hexConvertBuffer.Format(_L("0x%x"), halInt);
				tempText.Append(hexConvertBuffer);
				break;
			}
	}
	iArray->AppendL(tempText);
	
	// Device family revision
	HAL::Get(HALData::EDeviceFamilyRev, halInt);
	tempText.Format(_L("Device family revision%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);
	
	// CPU type
	tempText.Format(_L("CPU type%c"), KColumnListSeparator);
	HAL::Get(HALData::ECPU, halInt);
	switch (halInt)
	{
		case HALData::ECPU_ARM : tempText.Append(_L("ARM")); break;
		case HALData::ECPU_MCORE : tempText.Append(_L("MCORE")); break;
		case HALData::ECPU_X86 : tempText.Append(_L("X86")); break;
		default : 
			{
				hexConvertBuffer.Format(_L("0x%x"), halInt);
				tempText.Append(hexConvertBuffer);
				break;
			}
	}
	iArray->AppendL(tempText);

	// CPU ABI...whatever ABI stands for. :)
	tempText.Format(_L("CPU ABI%c"), KColumnListSeparator);
	HAL::Get(HALData::ECPUABI, halInt);
	switch (halInt)
	{
		case HALData::ECPUABI_ARM4 : tempText.Append(_L("ARM4")); break;
		case HALData::ECPUABI_ARMI : tempText.Append(_L("ARMI")); break;
		case HALData::ECPUABI_THUMB : tempText.Append(_L("THUMB")); break;
		case HALData::ECPUABI_ARM5T : tempText.Append(_L("ARM5T")); break;
		default :
			{
				hexConvertBuffer.Format(_L("0x%x"), halInt);
				tempText.Append(hexConvertBuffer);
				break;
			}
	}
	iArray->AppendL(tempText);
	
	// Startup reason
	tempText.Format(_L("Startup reason%c"), KColumnListSeparator);
	HAL::Get(HALData::ESystemStartupReason, halInt);
	switch (halInt)
	{
		case HALData::ESystemStartupReason_Cold : tempText.Append(_L("Cold boot")); break;
		case HALData::ESystemStartupReason_Warm : tempText.Append(_L("Warm boot")); break;
		case HALData::ESystemStartupReason_Fault : tempText.Append(_L("Fault")); break;
		default :
			{
				hexConvertBuffer.Format(_L("0x%x"), halInt);
				tempText.Append(hexConvertBuffer);
				break;
			}
	}
	iArray->AppendL(tempText);
	
	// Keyboard type
	tempText.Format(_L("Keyboard type%c"), KColumnListSeparator);
	HAL::Get(HALData::EKeyboard, halInt);
	switch (halInt)
	{
		case HALData::EKeyboard_Keypad : tempText.Append(_L("Keypad")); break;
		case HALData::EKeyboard_Full : tempText.Append(_L("Full")); break;
		default : 
			{
				hexConvertBuffer.Format(_L("0x%x"), halInt);
				tempText.Append(hexConvertBuffer);
				break;
			}
	}
	iArray->AppendL(tempText);
	
	// Ticks
	HAL::Get(HALData::ESystemTickPeriod, halInt);
	tempText.Format(_L("Tick period%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);

	// ROM size
	HAL::Get(HALData::EMemoryROM, halInt);
	tempText.Format(_L("ROM size%c%dMB"), KColumnListSeparator, TInt(halInt / (1024 * 1024)));
	iArray->AppendL(tempText);

	// RAM size
	HAL::Get(HALData::EMemoryRAM, halInt);
	tempText.Format(_L("RAM size%c%dMB"), KColumnListSeparator, TInt(halInt / (1024 * 1024)));
	iArray->AppendL(tempText);

	// Display X pixels
	HAL::Get(HALData::EDisplayXPixels, halInt);
	tempText.Format(_L("Display X-pixels%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);

	// Display Y pixels
	HAL::Get(HALData::EDisplayYPixels, halInt);
	tempText.Format(_L("Display Y-pixels%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);

	// Display X twips
	HAL::Get(HALData::EDisplayXTwips, halInt);
	tempText.Format(_L("Display X-twips%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);

	// Display Y twips
	HAL::Get(HALData::EDisplayYTwips, halInt);
	tempText.Format(_L("Display Y-twips%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);
	
	// Colors
	HAL::Get(HALData::EDisplayColors, halInt);
	tempText.Format(_L("Colors%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);

	// Num drives
	RFs fs;
	if (fs.Connect() == KErrNone)
	{
		TDriveList aList;
		TInt numDrives;
		TVolumeInfo volInfo;
		TInt err;
		
		if (fs.DriveList(aList) == KErrNone)
		{
			numDrives = 0;
			for (halInt = 0; halInt < KMaxDrives; halInt++)
			{
				if (aList[halInt] != 0)
				{
					err = CEikonEnv::Static()->FsSession().Volume(volInfo, EDriveA + halInt);
					numDrives++;
					if (err == KErrNone)
						tempText.Format(_L("%c: size%c%dMB"), 0x41 + halInt, KColumnListSeparator, TInt64(volInfo.iSize / 1024 / 1024).GetTInt());
					else
						tempText.Format(_L("%c: size%cUnknown"), 0x41 + halInt, KColumnListSeparator);
					iArray->AppendL(tempText);
				}
			}
			generalBuffer.Num(numDrives);
			tempText.Format(_L("Total drives%c"), KColumnListSeparator);
			tempText.Append(generalBuffer);
		}
		else
		{
			tempText.Format(_L("Total drives%cError querying"), KColumnListSeparator);
		}
		iArray->AppendL(tempText);
		fs.Close();
	}
	else
		tempText.Append(_L("Error querying"));
	
	// Apps installed
	tempText.Format(_L("Installed apps%c"), KColumnListSeparator);
	
	RApaLsSession mySession;
	TApaAppCapabilityBuf aCapabilityBuf;
	TApaAppInfo aInfo;
			
	// Bounce the app server
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->FindApp(KUidControlPanel, &aInfo);
	
	halInt = 0;
	if (mySession.Connect() == KErrNone)
	{
		if (mySession.GetAllApps() == KErrNone)
		{
			while (mySession.GetNextApp(aInfo) == KErrNone)
			{
				if (mySession.GetAppCapability(aCapabilityBuf, aInfo.iUid) == KErrNone)
				{
					if (aCapabilityBuf().iAppIsHidden == EFalse)
						halInt++;
				}				
			}
			mySession.Close();
			generalBuffer.Num(halInt);
			tempText.Append(generalBuffer);
		}
		else
			tempText.Append(_L("Error querying"));
	}
	else
		tempText.Append(_L("Error querying"));
	iArray->AppendL(tempText);

	// Locale - Country code
	halInt = iLocale.CountryCode();
	tempText.Format(_L("Country Code%c%d"), KColumnListSeparator, halInt);
	iArray->AppendL(tempText);

	// Locale - Clock type
	tempText.Format(_L("Clock type%c"), KColumnListSeparator); 
	TClockFormat cFormat = iLocale.ClockFormat();
	switch (cFormat)
	{
		case EClockAnalog : tempText.Append(_L("Analog")); break;
		case EClockDigital : tempText.Append(_L("Digital")); break;
		default : tempText.Append(_L("Unknown")); 
	}
	iArray->AppendL(tempText);

	// Locale - Daylight saving at home
	tempText.Format(_L("Home daylight saving%c"), KColumnListSeparator);
	if (iLocale.QueryHomeHasDaylightSavingOn())
		tempText.Append(_L("Enabled"));
	else
		tempText.Append(_L("Disabled"));
	iArray->AppendL(tempText);
	
	// Locale - Date format
	tempText.Format(_L("Date format%c"), KColumnListSeparator);
	TDateFormat dtFormat = iLocale.DateFormat();
	switch (dtFormat)
	{
		case EDateAmerican : tempText.Append(_L("mm/dd/yyyy")); break;
		case EDateEuropean : tempText.Append(_L("dd/mm/yyyy")); break;
		case EDateJapanese : tempText.Append(_L("yyyy/mm/dd")); break;
	}
	iArray->AppendL(tempText);

	// Locale - Time format
	tempText.Format(_L("Time format%c"), KColumnListSeparator);
	TTimeFormat tmFormat = iLocale.TimeFormat();
	switch (tmFormat)
	{
		case ETime12 : tempText.Append(_L("12 hour")); break;
		case ETime24 : tempText.Append(_L("24 hour")); break;
	}
	iArray->AppendL(tempText);

	// IMEI number
	tempText.Format(_L("IMEI%c"), KColumnListSeparator);
	TPlpVariantMachineId machineId;
	PlpVariant::GetMachineIdL(machineId);
	tempText.Append(machineId);
	iArray->AppendL(tempText);

	CEikonEnv::Static()->BusyMsgCancel();
	
	// Cleanup and display
	cSysInfoListBox->HandleItemAdditionL();
	cSysInfoListBox->SetFocus(ETrue, EDrawNow);
}

void CSMan2SysInfoView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	// Initialize listbox
	cSysInfoListBox = new(ELeave) CEikColumnListBox;
	cSysInfoListBox->ConstructL(this, 0);
	CColumnListBoxData* listBoxColumns = ((CColumnListBoxItemDrawer*)cSysInfoListBox->ItemDrawer())->ColumnData();
	listBoxColumns->SetColumnWidthPixelL(0, int(Size().iWidth * 0.48));
	listBoxColumns->SetColumnWidthPixelL(1, int(Size().iWidth * 0.40));
	listBoxColumns->SetColumnAlignmentL(0, CGraphicsContext::ELeft);
	listBoxColumns->SetColumnAlignmentL(1, CGraphicsContext::ERight);
	cSysInfoListBox->CreateScrollBarFrameL();
	cSysInfoListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	cSysInfoListBox->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, aRect.Size().iHeight - 10));
	controlsArray->AppendL(cSysInfoListBox);

	viewId = CSMan2AppUi::EViewSysInfo;
	iBJackIconPosition = TPoint(2, cSysInfoListBox->Position().iY + cSysInfoListBox->Size().iHeight);
	InitBJackIcon();
	
	EnableDragEvents();
	
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
		SetZoomLevel(configData->zoomSysInfo);

	MakeVisible(EFalse);
	ActivateL();
}

void CSMan2SysInfoView::SetZoomLevel(TInt zoomLevel)
{
	// DEBUG
	releaseFont();
	generateZoomFont(zoomLevel);
	CTextListItemDrawer* itemDrawer = ((CTextListItemDrawer*)cSysInfoListBox->View()->ItemDrawer());
	itemDrawer->SetFont(font);
	cSysInfoListBox->SetItemHeightL(font->HeightInPixels() + cSysInfoListBox->VerticalInterItemGap());
	configData->zoomSysInfo = zoomLevel;
	//DEBUG releaseFont();
}

void CSMan2SysInfoView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}