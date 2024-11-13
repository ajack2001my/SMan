#include "bjackview.h"
#include "dialogs.h"
#include "sman.h"
#include <sman.rsg>

/*************************************************************
*
* BJack view
*
**************************************************************/

CSMan2BJackView::CSMan2BJackView(CConfig *cData)  : CViewBase(cData)
{
}

TBool CSMan2BJackView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2BJackView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2BJackView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
	}
}

void CSMan2BJackView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	if (activateCount <= 0)
	{
		doViewActivated();
		// Did the log get rerouted?
		if (oScanner->logHelper->logTextBox != bluejackLog)
		{
			oScanner->logHelper->logTextBox = bluejackLog;
			oScanner->logHelper->updateLog(NULL, 0, CLogger::logAll);
		}
	}
}

TVwsViewId CSMan2BJackView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidBJackView);
}

void CSMan2BJackView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}

TKeyResponse CSMan2BJackView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			bluejackLog->MoveDisplayL(TCursorPosition::EFLineDown);
			bluejackLog->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			bluejackLog->MoveDisplayL(TCursorPosition::EFLineUp);
			bluejackLog->UpdateScrollBarsL();
			return EKeyWasConsumed;
		}
	}
	return bluejackLog->OfferKeyEventL(aKeyEvent, aType);
}

CSMan2BJackView::~CSMan2BJackView()
{
	delete oScanner;
}

void CSMan2BJackView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	bluejackLog = new (ELeave) CEikGlobalTextEditor;
	bluejackLog->ConstructL(this, 18, MAX_LOG_LINES * MAX_LOG_LINE_SIZE, CEikEdwin::EReadOnly, EGulFontControlAll, EGulAllFonts);
	bluejackLog->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, aRect.Size().iHeight - EQikToolbarHeight - 10));
	bluejackLog->CreateScrollBarFrameL();
	bluejackLog->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);
	controlsArray->AppendL(bluejackLog);

	TFileName appPath;
	
	appPath.Copy(appPathNoExt);
	appPath.SetLength(appPath.Length() - 5);

	oScanner = new (ELeave) CScanner(&appPath, configData);

	TCharFormat charFormat;
	TCharFormatMask charFormatMask;

	charFormat.iFontPresentation.iTextColor = KRgbDarkBlue;
	charFormatMask.SetAttrib(EAttColor);
	bluejackLog->ApplyCharFormatL(charFormat, charFormatMask);

	viewId = CSMan2AppUi::EViewBJack;
	iBJackIconPosition = TPoint(2, bluejackLog->Position().iY + bluejackLog->Size().iHeight + 2);
	InitBJackIcon();

	MakeVisible(EFalse);

	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_CanSetZoomFont))
		SetZoomLevel(configData->zoomBJack);

	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;

	paraFormat.iLineSpacingControl = CParaFormat::ELineSpacingAtLeastInTwips;
	paraFormatMask.SetAttrib(EAttLineSpacingControl);
	bluejackLog->ApplyParaFormatL(&paraFormat, paraFormatMask);
	ActivateL();
}

void CSMan2BJackView::SetZoomLevel(TInt zoomLevel)
{
	TInt lineSpaceTwips[3] = {150, 200, 300};
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	
	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;

	generateZoomFont(zoomLevel);
	charFormat.iFontSpec.iHeight = font->FontSpecInTwips().iHeight;
	//((zoomLevel - ZOOM_INCREMENT) / ZOOM_INCREMENT) * 150 + 100;
	charFormatMask.SetAttrib(EAttFontHeight);
	bluejackLog->ApplyCharFormatL(charFormat, charFormatMask);
	
	paraFormat.iLineSpacingInTwips = lineSpaceTwips[(zoomLevel / ZOOM_INCREMENT) - 1];
	paraFormatMask.SetAttrib(EAttLineSpacing);
	bluejackLog->ApplyParaFormatL(&paraFormat, paraFormatMask);
	configData->zoomBJack = zoomLevel;
}

/*************************************************************
*
* Logging class
*
**************************************************************/

CLogger::CLogger(CConfig *cData)
{
	configData = cData;
	logFilename.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName());
	logFilename.SetLength(logFilename.Length() - 3);
	logFilename.Append(_L("log.txt"));
}

CLogger::~CLogger()
{
}

void CLogger::updateLog(const TDesC *dialogLine, const TInt numExtraLines, TUint logType)
{
	if ((configData->logFlags & logType) == 0)
		return;

	TBuf<MAX_LOG_LINE_SIZE * MAX_LOG_LINES> tempLine;
	TTime tTime;

	if (dialogLine != NULL)
	{
		tTime.HomeTime();
		tTime.FormatL(tempLine, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B  "));
		tempLine.Append(*dialogLine);
		
		if (*localAutoSave)
		{
			TBuf<MAX_LOG_LINE_SIZE * MAX_LOG_LINES> fileOutput;
			
			fileOutput.Copy(tempLine);
			fileOutput.Append(_L("\r\n"));
			for (int i = 0; i < numExtraLines;i ++)
				fileOutput.Append(_L("\r\n"));
			if (!saveLog(EFalse, &fileOutput))
				CEikonEnv::Static()->InfoMsgWithAlignment(TGulAlignment(EHRightVCenter), R_TBUF_BJACKERRSAVELOG);
		}
		tempLine.Append(CEditableText::ELineBreak);
		tempLine.Append(CEditableText::ELineBreak);

		// Are we overflowing the buffer?
		if ((logBuffer->Length() + tempLine.Length()) >= (MAX_LOG_LINES * MAX_LOG_LINE_SIZE))
		{
			// Yes. Delete the older characters
			logBuffer->Delete(0, tempLine.Length());
		}
		logBuffer->Append(tempLine);
	}
	if (logTarget == logToDialog)
	{
		if (logDialog != NULL)
		{
			CEikGlobalTextEditor *logEditor;
			logEditor = STATIC_CAST(CEikGlobalTextEditor*, logDialog->Control(controlId));
			logEditor->SetTextL(logBuffer);
			logEditor->SetCursorPosL(logEditor->TextLength(), 0);
			logEditor->DrawDeferred();
		}
	} 
	else if (logTarget == logToTextEditor)
	{
		logTextBox->SetTextL(logBuffer);
		logTextBox->SetCursorPosL(logTextBox->TextLength(), 0);
		logTextBox->DrawDeferred();
	}
}

TBool CLogger::saveLog(TBool overwrite, TDesC *buffer)
{
	RFile outStream;
	TBuf8<((MAX_LOG_LINES + 1) * MAX_LOG_LINE_SIZE) * 2> buffer8bit;
	TInt openResult;

	// Use copy to convert from unicode to non-unicode
	buffer8bit.Copy(*buffer);

	if (overwrite)
		openResult = outStream.Replace(CEikonEnv::Static()->FsSession(), logFilename, EFileWrite);
	else
	{
		if ((openResult = outStream.Open(CEikonEnv::Static()->FsSession(), logFilename, EFileWrite)) != KErrNone)
			openResult = outStream.Replace(CEikonEnv::Static()->FsSession(), logFilename, EFileWrite);
	}
		
	TBool iSaveOk = EFalse;
	if (openResult == KErrNone)
	{
		TInt aPos;
		
		aPos = 0;
		outStream.Seek(ESeekEnd, aPos);
		outStream.Write(buffer8bit);
		if (outStream.Flush() == KErrNone)
			iSaveOk = ETrue;
		outStream.Close();
	}
	return iSaveOk;
}

/*************************************************************
*
* Scanner class
*
**************************************************************/

TBool CScanner::LoadBTExclusionList()
{	
	RFile iFile;
	TInt iResult;
	
	iResult = KErrGeneral;
	if (iFile.Open(CEikonEnv::Static()->FsSession(), iBTExclusionListFileName, EFileRead) == KErrNone)
	{
		TBTDevice iOneDevice;
		TPckg<TBTDevice> iPackageDeviceList(iOneDevice);
		iResult = iFile.Read(iPackageDeviceList);
		while ((iResult == KErrNone) && (iPackageDeviceList.Size() > 0))
		{
			iBTExclusionList->AppendL(iOneDevice);
			iResult = iFile.Read(iPackageDeviceList);
		}
		iFile.Close();
	}
	
	if (iResult == KErrNone)
		return ETrue;
	else
		return EFalse;
}

TBool CScanner::SaveBTExclusionList()
{
	RFile iFile;
	TInt iResult = KErrGeneral;
	
	TInt iDoh = iFile.Replace(CEikonEnv::Static()->FsSession(), iBTExclusionListFileName, EFileWrite);
	if (iDoh == KErrNone)
	{
		if (iBTExclusionList->Count() > 0)
		{
			TBTDevice iOneDevice;
			TPckg<TBTDevice> iPackageDeviceList(iOneDevice);
			TInt i = 0;
			do
			{
				iOneDevice.iAddress.Des().Copy(iBTExclusionList->At(i).iAddress.Des());
				iOneDevice.iName.Copy(iBTExclusionList->At(i).iName);
				iResult = iFile.Write(iPackageDeviceList);
				i++;
			} while ((iResult == KErrNone) && (i < iBTExclusionList->Count()));
		}
		else
			iResult = KErrNone;
			
		iResult = iFile.Flush();
		iFile.Close();
	}
	
	if (iResult == KErrNone)
		return ETrue;
	else
		return EFalse;
}

void CScanner::EditBTExclusionList()
{
	CEditBTExclusionList *iEditListDialog = new (ELeave) CEditBTExclusionList();
	iEditListDialog->iBTDeviceArray = iBTExclusionList;
	if (iEditListDialog->ExecuteLD(R_EDIT_BLUEJACK_EXCLUSIONLIST) == EEikBidOk)
	{
		if (!SaveBTExclusionList())
			CEikonEnv::Static()->InfoMsg(R_TBUF_BJACK_ERRSAVEEXLIST);
	}
}

TBool CScanner::IsFriendDevice(TBTDevice *iBTDevice)
{
	TBool iResult = EFalse;
	
	for (int i = 0; i < iBTExclusionList->Count(); i++)
	{
		if (iBTExclusionList->At(i).iAddress == iBTDevice->iAddress)
		{
			iResult = ETrue;
			break;
		}
	}
	return iResult;
}

CScanner::~CScanner()
{
	if (hostResolverOpen)
	{
		hostResolver.Close();
		hostResolverOpen = EFalse;
	}
	if (socketServOpen)
	{
		socketServ.Close();
		socketServOpen = EFalse;
	}
	delete logHelper;
	
	if (iSManECom)
	{
		iSManECom->DoVibrateOff();
		delete iSManECom;
	}
	
	if (audioPlayerUtil)
	{
		//audioPlayerUtil->Stop();
		delete audioPlayerUtil;
	}
	
	if (iBTExclusionList)
		delete iBTExclusionList;		
}

CScanner::CScanner(TFileName *appPath, CConfig *cData) : CTimer(EPriorityNormal)
{	
	firstRest = EFalse;
	configData = cData;
	audioPlayerUtil = NULL;

	iBTExclusionList = new (ELeave) CArrayFixFlat<TBTDevice>(1);
	
	vCardNames[0].Copy(*appPath);
	vCardNames[0].Append(_L("SMAN.VCF"));
			
	iBTExclusionListFileName.Copy(*appPath);
	iBTExclusionListFileName.Append(_L("SMAN.BJE"));

	iBTExclusionList = new (ELeave) CArrayFixFlat<TBTDevice>(1);
	LoadBTExclusionList();

	scannerStatus = isIdle;
	hostResolverOpen = EFalse;
	socketServOpen = EFalse;

	TRAPD(iError, iSManECom = CSManEComInterfaceDefinition::NewL(_L8("DoVibrate")));
	if (iError)
		iSManECom = NULL;
	
	logHelper = new (ELeave) CLogger(cData);
	logHelper->logBuffer = &logBuffer;
	logHelper->logTarget = CLogger::logToTextEditor;
	logHelper->logTextBox = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->bluejackLog;

	socketServ.Connect();
	socketServOpen = ETrue;
	// L2CAP, SDP, RFCOMM, BTLinkManager
	if ((socketServ.FindProtocol(_L("BTLinkManager"), protoDesc)) != KErrNone)
	{
		socketServ.Close();
		socketServOpen = EFalse;
		CEikonEnv::Static()->InfoMsg(R_TBUF_BJACKBTLINKMISSING);
		User::Leave(0);
	}
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
}

TBool CScanner::initFile(void)
{
	if (obexFile)
	{
		delete obexFile;
		obexFile = NULL;
	}
	obexFile = CObexFileObject::NewL(TPtrC(NULL, 0));
	TInt err;
	TRAP(err, obexFile->InitFromFileL(vCardNames[0]));
	if (err)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKERRVCARD);
		logHelper->updateLog(dataBuffer, 0, CLogger::logAll);
		delete dataBuffer;
		
		return EFalse;
	}
	return ETrue;
}

void CScanner::doRealScan(void)
{
	HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKSCANNING);
	logHelper->updateLog(dataBuffer, 0, CLogger::logScanning);
	delete dataBuffer;

	// Check if user switched off bluetooth
	if (hostResolverOpen)
		hostResolver.Close();
	if (hostResolver.Open(socketServ, protoDesc.iAddrFamily, protoDesc.iProtocol) != KErrNone)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKBTDISABLED);
		//TBuf<50> txt;		
		//CEikonEnv::Static()->ReadResource(txt, R_TBUF_BJACKBTDISABLED);
		hostResolverOpen = EFalse;
		CEikonEnv::Static()->InfoMsg(*dataBuffer);
		logHelper->updateLog(dataBuffer, 0, CLogger::logAll);
		delete dataBuffer;
		
		scannerStatus = isIdle;
		return;
	}
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->UpdateSystemIcons(CSMan2AppUi::EIconBluejack, ETrue);
	hostResolverOpen = ETrue;
	if (obexClient)
	{
		delete obexClient;
		obexClient = NULL;
	}
	if (initFile())
	{
		// Start the discovery inquiry
		sockAddr.SetIAC(KGIAC);
		sockAddr.SetAction(resolveAction);
		iStatus = KRequestPending;
		scannerStatus = isInquiring;
		hostResolver.GetByAddress(sockAddr, nameEntry, iStatus);
		SetActive();
	}
	else
		stopScan();
	SetToolbarButtonState();
}

void CScanner::timeoutAbort(void)
{
	if (obexClient)
	{
		delete obexClient;
		obexClient = NULL;
	}
	if (IsActive())
		Cancel();
	
	if (scannerStatus != isShutdown)
	{
		startRestTime.HomeTime();
		currentNoDevicePause = STACK_COOLOFF_COUNTDOWN;
		firstRest = ETrue;
		rest();
	}
	else
		stopScan();
}

void CScanner::startScan(void)
{	
	logHelper->localAutoSave = &(configData->autoSave);
	localShowSysMsg = &(configData->showBluejackSysMsg);
	localProxSweepOnce = &(configData->proxSweepOnce);
	localNoDeviceAttempts = &(configData->noDeviceAttempts);
	localNoDevicePause = &(configData->noDevicePause);
	localVibrate = &(configData->vibraNotify);
	localAudioFile = &(configData->audioFileName);
	currentNoDeviceAttempts = 0;
	currentDeviceIndex = MAX_BTDEV_ADDR;
	for (int i = 0 ; i < MAX_BTDEV_ADDR; i++)
	{
		btDevice[i].isBluejacked = statusNoDevice;
		btDevice[i].btDevAddr[0] = 0;
		btDevice[i].btDevAddr[1] = 0;
		btDevice[i].btDevAddr[2] = 0;
		btDevice[i].btDevAddr[3] = 0;
		btDevice[i].btDevAddr[4] = 0;
		btDevice[i].btDevAddr[5] = 0;
		btDevice[i].obexPort = -1;										
	}
	if (configData->resolveName)
		resolveAction = KHostResInquiry + KHostResName;
	else
		resolveAction = KHostResInquiry;
		
	if (iScannerBehaviour == behaviourBluejackOne)
	{
		forcedFlagTrue = ETrue;
		localProxSweepOnce = &forcedFlagTrue;
		if (selectDevice() == KErrNone)
		{
			// Skip scanning. Bluejack immediately
			static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->UpdateSystemIcons(CSMan2AppUi::EIconBluejack, ETrue);
			currentDeviceIndex = -1;
			CEikonEnv::Static()->InfoMsg(R_TBUF_BJACKSCANNERSTARTED);
			if (initFile())
			{
				bluejackNextDevice();
				SetToolbarButtonState();
			}
			else
				stopScan();
		}
	}
	else
	{
		CEikonEnv::Static()->InfoMsg(R_TBUF_BJACKSCANNERSTARTED);
		doRealScan();
	}
}

void CScanner::stopScan(void)
{
	if (IsActive())
	{
		if (scannerStatus == isRest)
		{
			Cancel();
			After(1);
		}
		scannerStatus = isShutdown;
		CEikonEnv::Static()->InfoMsg(R_TBUF_BJACKSCANNERTERMINATING);
	}
	else
	{
		timeoutCounter.stopTimer();
		if (obexFile)
		{
			delete obexFile;
			obexFile = NULL;
		}
		if (obexClient)
		{
			delete obexClient;
			obexClient = NULL;
		}
		
		scannerStatus = isIdle;
		if (hostResolverOpen)
		{
			hostResolver.Close();
			hostResolverOpen = EFalse;
		}
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKSCANNERTERMINATED);
		logHelper->updateLog(dataBuffer, 2, CLogger::logScanning);
		CEikonEnv::Static()->InfoMsg(*dataBuffer);
		delete dataBuffer;
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->UpdateSystemIcons(CSMan2AppUi::EIconBluejack, EFalse);
		SetToolbarButtonState();
		
		if (audioPlayerUtil)
			audioPlayerUtil->Stop();
	}
}

void CScanner::logWithLookup(TPtrC theText, TUint logType, TBool aIsFriend)
{		
	if (resolveAction == KHostResInquiry)
	{
		TBuf<30> temp;
		if (!aIsFriend)
		{
			temp.Format(_L("%x:%x:%x:%x:%x:%x - "), btDevice[currentDeviceIndex].btDevAddr[0], 
				btDevice[currentDeviceIndex].btDevAddr[1], btDevice[currentDeviceIndex].btDevAddr[2], 
				btDevice[currentDeviceIndex].btDevAddr[3], btDevice[currentDeviceIndex].btDevAddr[4], 
				btDevice[currentDeviceIndex].btDevAddr[5]);
		}
		else
			temp.Copy(_L(" - "));
		HBufC* dataBuffer = HBufC::NewL(theText.Length() + temp.Length());
		dataBuffer->Des().Copy(temp);
		dataBuffer->Des().Append(theText);
		logHelper->updateLog(dataBuffer, 0, logType);
		delete dataBuffer;
	}
	else
	{
		HBufC *dataBuffer = HBufC::NewL(theText.Length() + 256 + 10);
		if (!aIsFriend)
		{
			dataBuffer->Des().Copy(btDevice[currentDeviceIndex].btNames);
			dataBuffer->Des().Append(_L(" - "));
		}
		dataBuffer->Des().Append(theText);
		logHelper->updateLog(dataBuffer, 0, logType);
		delete dataBuffer;
	}
}

void CScanner::rest(void)
{
	if (firstRest)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKCOOLOFF);
		logHelper->updateLog(dataBuffer, 0, CLogger::logDecays);
		delete dataBuffer;
		//TBuf<50> txt;
		//CEikonEnv::Static()->ReadResource(txt, R_TBUF_BJACKCOOLOFF);
		//logHelper->updateLog(&txt, 0, CLogger::logDecays);
		
		scannerStatus = isRest;
		firstRest = EFalse;
	}
	After(TIMER_GRANULARITY);
}

void CScanner::LogFriendExclusion(TDes *aDeviceName, TBTDevAddr *aDeviceAddr)
{
	HBufC* iText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACK_EXCLUDE);
	if (resolveAction != KHostResInquiry)
	{
		iText = iText->ReAllocL(iText->Length() + aDeviceName->Length() + 3);
		iText->Des().Insert(0, _L(" - "));
		iText->Des().Insert(0, *aDeviceName);
	}
	else
	{
		iText = iText->ReAllocL(iText->Length() + 30 + 3);
		TBuf<30> iTempBuffer;
		iTempBuffer.Format(_L("%x:%x:%x:%x:%x:%x - "), aDeviceAddr[0], aDeviceAddr[1], 
			aDeviceAddr[2], aDeviceAddr[3], aDeviceAddr[4], aDeviceAddr[5]);
		iText->Des().Insert(0, iTempBuffer);
	}
	logWithLookup(*iText, CLogger::logScanning, ETrue);
	delete iText;
}

void CScanner::bluejackNextDevice(void)
{
	if (getNextBTDeviceIndex())
	{
		TBTDevice iOneDevice;
		iOneDevice.iAddress.Des().Copy(btDevice[currentDeviceIndex].btDevAddr.Des());
		if (IsFriendDevice(&iOneDevice))
			LogFriendExclusion(&(btDevice[currentDeviceIndex].btNames), &(btDevice[currentDeviceIndex].btDevAddr));
		else
		{
			if (obexClient)
			{
				delete obexClient;
				obexClient = NULL;
			}
			iStatus = KRequestPending;
			if (btDevice[currentDeviceIndex].obexPort == -1)
			{
				scannerStatus = isOBEXSearch;
				obexRetries = OBEX_RETRIES;
				obexSearcher.start(&iStatus, &(btDevice[currentDeviceIndex].btDevAddr));
				SetActive();
			}
			else
			{
				// This is a hack... :(
				obexSearcher.RFCOMMPort = btDevice[currentDeviceIndex].obexPort;
				scannerStatus = isOBEXSearch;
				RunL();
			}
		}
	}
	else
	{
		currentNoDeviceAttempts++;
		if (*localProxSweepOnce == EFalse)
		{
			if (currentNoDeviceAttempts > *localNoDeviceAttempts)
			{
				currentNoDeviceAttempts = 0;
				startRestTime.HomeTime();
				currentNoDevicePause = *localNoDevicePause;
				firstRest = ETrue;
				rest();
			}
			else
				doRealScan();
		}
		else
		{
			stopScan();
			SetToolbarButtonState();
		}
	}
}

void CScanner::SetToolbarButtonState()
{
	CEikButtonGroupContainer* toolbar =	CEikonEnv::Static()->AppUiFactory()->ToolBar();
	if (toolbar != NULL)
	{
		CEikCommandButton* button = (CEikCommandButton*)(toolbar->ControlOrNull(cmdBluejackStop));
		if (button != NULL)
		{
			if (!IsActive())
			{
				button->SetDimmed(ETrue);

				button = (CEikCommandButton*)(toolbar->ControlOrNull(cmdBluejack));
				button->SetDimmed(EFalse);
				button = (CEikCommandButton*)(toolbar->ControlOrNull(cmdBluejackOne));
				button->SetDimmed(EFalse);
			}
			else
			{
				button->SetDimmed(EFalse);

				button = (CEikCommandButton*)(toolbar->ControlOrNull(cmdBluejack));
				button->SetDimmed(ETrue);
				button = (CEikCommandButton*)(toolbar->ControlOrNull(cmdBluejackOne));
				button->SetDimmed(ETrue);
			}
			CEikonEnv::Static()->AppUiFactory()->ToolBar()->DrawNow();
		}
	}
}

void CScanner::RunL()
{
	static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->UpdateSystemIcons(CSMan2AppUi::EIconBluejack, ETrue);
	if (scannerStatus == isRest)
	{
		TTime currentTime;
		TTimeIntervalSeconds interval;
		
		currentTime.HomeTime();
		if (currentTime.SecondsFrom(startRestTime, interval) != 0)
			interval = CONFIG_NO_DEVICE_PAUSE + 1;

		if (interval.Int() >= currentNoDevicePause)
			bluejackNextDevice();
		else
			rest();
	} 
	else if (scannerStatus == isOBEXDisconnect)
	{
		timeoutCounter.stopTimer();
		startRestTime.HomeTime();
		currentNoDevicePause = STACK_COOLOFF_COUNTDOWN;
		firstRest = ETrue;
		rest();
	}
	else if (scannerStatus == isOBEXSend)
	{
		timeoutCounter.stopTimer();
		if (iStatus == KErrNone)
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKSUCCESS);
			logWithLookup(*dataBuffer, CLogger::logBluejack, EFalse);
			if (*localShowSysMsg)
				User::InfoPrint(*dataBuffer);
			delete dataBuffer;
			// Update the device's status and timer
			btDevice[currentDeviceIndex].isBluejacked = statusValidBluejacked;
			//btDevice[currentDeviceIndex].lastBluejackedTime.HomeTime();
			// Vibrate phone
			if ((*localVibrate) && (iSManECom))
			{
				iSManECom->DoVibrateOn(1, 10, 3);
			}
			// Audio?
			if (localAudioFile->Length() > 0)
				playAudio();
		}
		else
		{
			TBuf<15> errNum;
			errNum.Format(_L("%d"), iStatus);
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKERR);
			dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + errNum.Length());
			dataBuffer->Des().Append(errNum);
			logWithLookup(*dataBuffer, CLogger::logBluejack, EFalse);
			delete dataBuffer;
			// If the bluejack failed, marked as a cached item
			// and timestamp it
			btDevice[currentDeviceIndex].isBluejacked = statusCached;
			//btDevice[currentDeviceIndex].lastBluejackedTime.HomeTime();
		}
		btDevice[currentDeviceIndex].lastBluejackedTime.HomeTime();
		// Start the timeout counter
		timeoutCounter.startTimer(this, configData->obexTimeout);
		// Disconnect and restart
		scannerStatus = isOBEXDisconnect;
		iStatus = KRequestPending;
		obexClient->Disconnect(iStatus);
		SetActive();
	}
	else if (scannerStatus == isOBEXConnect)
	{
		timeoutCounter.stopTimer();
		if (iStatus == KErrNone)
		{
			scannerStatus = isOBEXSend;

			// Start the timeout counter
			timeoutCounter.startTimer(this, configData->obexTimeout);
			
			// Start OBEX Put
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKOBEXPUT);
			logWithLookup(*dataBuffer, CLogger::logOBEX, EFalse);
			delete dataBuffer;
			iStatus = KRequestPending;
			obexClient->Put(*obexFile, iStatus);
			SetActive();
		}
		else
		{
			TBuf<15> errNum;
			errNum.Format(_L("%d"), iStatus);
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKOBEXCONNERR);
			dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + errNum.Length());
			dataBuffer->Des().Append(errNum);
			logWithLookup(*dataBuffer, CLogger::logOBEX, EFalse);
			delete dataBuffer;
			
			startRestTime.HomeTime();
			currentNoDevicePause = STACK_COOLOFF_COUNTDOWN;
			firstRest = ETrue;
			
			// Mark device as cached and timestamp it
			btDevice[currentDeviceIndex].isBluejacked = statusCached;
			btDevice[currentDeviceIndex].lastBluejackedTime.HomeTime();				
			rest();
		}
	}
	else if (scannerStatus == isOBEXSearch)
	{
		// I could have passed this via iStatus too.
		if (obexSearcher.RFCOMMPort > -1)
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKOBEXFOUND);
			logHelper->updateLog(dataBuffer, 0, CLogger::logOBEX);
			delete dataBuffer;

			btDevice[currentDeviceIndex].obexPort = obexSearcher.RFCOMMPort;
			obexBTProtoInfo.iTransport.Copy(_L("RFCOMM"));
			obexBTProtoInfo.iAddr.SetBTAddr(btDevice[currentDeviceIndex].btDevAddr);
			obexBTProtoInfo.iAddr.SetPort(obexSearcher.RFCOMMPort);
			if (obexClient)
			{
				delete obexClient;
				obexClient = NULL;
			}
			obexClient = CObexClient::NewL(obexBTProtoInfo);
			scannerStatus = isOBEXConnect;
			iStatus = KRequestPending;
		    
			// Start the timeout counter
			timeoutCounter.startTimer(this, configData->obexTimeout);

		    // Start OBEX connect
			obexClient->Connect(iStatus);
			SetActive();
		}
		else
		{
			TBuf<15> errNum;
			errNum.Format(_L("%d"), iStatus);
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKOBEXSEARCHERR);
			dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + errNum.Length());
			dataBuffer->Des().Append(errNum);
			logWithLookup(*dataBuffer, CLogger::logOBEX, EFalse);
			delete dataBuffer;
			
			if (obexRetries > 0)
			{
				obexRetries--;
				iStatus = KRequestPending;
				scannerStatus = isOBEXSearch;
				obexSearcher.start(&iStatus, &(btDevice[currentDeviceIndex].btDevAddr));
				SetActive();
			}
			else
			{
				startRestTime.HomeTime();
				currentNoDevicePause = STACK_COOLOFF_COUNTDOWN;
				firstRest = ETrue;
			
				// Remove device from list
				btDevice[currentDeviceIndex].isBluejacked = statusNoDevice;
				rest();
			}
		}			
	}
	else if (scannerStatus == isShutdown)
	{
		stopScan();
	}
	else if (scannerStatus == isInquiring)
	{
		// Found a device?
		if (iStatus == KErrNone)
		{
			TBTSockAddr iBTSockAddr(nameEntry().iAddr);
			insertAndUpdateBTList(iBTSockAddr.BTAddr(), nameEntry().iName);
			iStatus = KRequestPending;
			hostResolver.Next(nameEntry, iStatus);
			SetActive();
		}
		else
		{
			if (hostResolverOpen)
			{
				hostResolver.Close();
				hostResolverOpen = EFalse;
			}
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKNODEVICES);
			logHelper->updateLog(dataBuffer, 0, CLogger::logScanning);
			delete dataBuffer;
			currentDeviceIndex = -1;
			bluejackNextDevice();
		}
	}
}

TBool CScanner::getNextBTDeviceIndex(void)
{
	for (int i = (currentDeviceIndex + 1); i < MAX_BTDEV_ADDR; i++)
	{
		if (btDevice[i].isBluejacked == statusValidNotBluejacked)
		{
			currentDeviceIndex = i;
			return ETrue;
		}		
	}
	currentDeviceIndex = -1;
	return EFalse;
}

// Returns TRUE if this device was inserted into the list (i.e. new device) else FALSE
TBool CScanner::insertAndUpdateBTList(TBTDevAddr btDevAddr, TBTDeviceName btDevName)
{
	nameEntry().iName.Copy(btDevName);
	
	// Is this a friendly device?
	TBTDevice iOneDevice;
	iOneDevice.iAddress.Des().Copy(btDevAddr.Des());
	if (IsFriendDevice(&iOneDevice))
	{
		LogFriendExclusion(&btDevName, &btDevAddr);
		return EFalse;
	}
	else
	{
		TInt cacheIndexToOverwrite = -1;
		TBool isExist = EFalse;
		TBool isInserted = EFalse;
		TTime currentTime;
		TTimeIntervalSeconds interval, oldInterval;

		// Need to be fair to all devices. So take a snapshot of time here rather than in each
		// loop iteration
		currentTime.HomeTime();
		
		// First, make sure this device does not exist in the list.
		for (int i = 0; i < MAX_BTDEV_ADDR; i++)
		{
			// Check if we found the device before. This means the device entry is either:
			// - a non-bluejacked device
			// - bluejacked but waiting for decay; OR
			// - a cached entry
			// In either case, it means we shouldn't insert anything into the array
			if (btDevice[i].btDevAddr == btDevAddr)
			{
				// Is this a device that was found before?
				if (btDevice[i].isBluejacked == statusValidNotBluejacked)
				{
					isExist = ETrue;
					currentDeviceIndex = i;
					HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKCACHED);
					logWithLookup(*dataBuffer, CLogger::logScanning, EFalse);
					delete dataBuffer;

					break;
					// We can leave the obex port in its unknown state. getNextBluejackDevice()
					// will take care of it
				}
				// Check if it was bluejacked but waiting for decay
				else if (btDevice[i].isBluejacked == statusValidBluejacked)
				{
					if (currentTime.SecondsFrom(btDevice[i].lastBluejackedTime, interval) != 0)
						interval = -1;
					// Decay still not expired?
					if (interval.Int() < configData->decayTime)
					{
						isExist = ETrue;
						
						// Generate a log entry
						currentDeviceIndex = i;
						
						HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKCACHEDECAY);
						logWithLookup(*dataBuffer, CLogger::logScanning, EFalse);
						delete dataBuffer;

						break;
					}
					// This means its decay time is up!
					else
					{
						isExist = ETrue;
						currentDeviceIndex = i;
						HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKCACHED);
						logWithLookup(*dataBuffer, CLogger::logScanning, EFalse);
						delete dataBuffer;
						
						btDevice[i].isBluejacked = statusValidNotBluejacked;
						break;
					}
				}
				// A cached entry?
				else if (btDevice[i].isBluejacked == statusCached)
				{
					isExist = ETrue;
					
					// Generate a log entry
					currentDeviceIndex = i;
					HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKCACHED);
					logWithLookup(*dataBuffer, CLogger::logScanning, EFalse);
					delete dataBuffer;
					
					// Reset the item's status to be a newly found item
					btDevice[i].isBluejacked = statusValidNotBluejacked;
					
					break;
				}
			}
		}
		
		// Ok, device wasn't found. We should insert it. It may be possible our array is
		// full (with cache and decaying entries). In that case, we should issue a message
		// to the user informing thus
		if (!isExist)
		{
			// Only the following types of array entries can be overwritten.
			// - empty entries (isBluejacked == statusNoDevice)
			// - decaying entries who's decay time is up
			// - cached entries

			oldInterval = -1;
			for (int i = 0; i < MAX_BTDEV_ADDR; i++)
			{
				if (currentTime.SecondsFrom(btDevice[i].lastBluejackedTime, interval) != 0)
					interval = -1;

				// Is this a decaying item? If so, check if its time is up
				if (btDevice[i].isBluejacked == statusValidBluejacked)
				{
					if (interval.Int() >= configData->decayTime)
					{
						// This entry's decay time is up!
						isInserted = ETrue;
						btDevice[i].btDevAddr = btDevAddr;
						if (resolveAction != KHostResInquiry)
							btDevice[i].btNames = nameEntry().iName;
						btDevice[i].isBluejacked = statusValidNotBluejacked;
						// Since this is a new entry, we haven't done a SDP. Set port to -1
						btDevice[i].obexPort = -1;
						currentDeviceIndex = i;
						break;
					}
				}
				// Is this an empty entry? If so insert immediately!
				else if (btDevice[i].isBluejacked == statusNoDevice)
				{
					isInserted = ETrue;
					btDevice[i].btDevAddr = btDevAddr;
					if (resolveAction != KHostResInquiry)
						btDevice[i].btNames = nameEntry().iName;
					btDevice[i].isBluejacked = statusValidNotBluejacked;
					// Since this is a new entry, we haven't done a SDP. Set port to -1
					btDevice[i].obexPort = -1;
					
					currentDeviceIndex = i;
					break;
				}
				// Is this a cached entry? If so take the one with the oldest timestamp
				else if (btDevice[i].isBluejacked == statusCached)
				{
					if (currentTime.SecondsFrom(btDevice[i].lastBluejackedTime, interval) != 0)
						interval = -1;
					if (interval > oldInterval)
						cacheIndexToOverwrite = i;
				}
			}
			
			// Did we insert?
			if (isInserted)
			{
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKFOUNDDEVICE);
				logWithLookup(*dataBuffer, CLogger::logScanning, EFalse);
				delete dataBuffer;
			}
			// Did we find a suitable cached entry to overwrite?
			else if (cacheIndexToOverwrite > -1)
			{
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKCACHEFULL);
				logWithLookup(*dataBuffer, CLogger::logScanning, EFalse);
				delete dataBuffer;

				btDevice[cacheIndexToOverwrite].btDevAddr = btDevAddr;
				if (resolveAction != KHostResInquiry)
					btDevice[cacheIndexToOverwrite].btNames = nameEntry().iName;
				btDevice[cacheIndexToOverwrite].isBluejacked = statusValidNotBluejacked;
				// Since this is a new entry, we haven't done a SDP. Set port to -1
				btDevice[cacheIndexToOverwrite].obexPort = -1;
			}
			// None of the above. This means buffer is full :(
			else
			{
				//CEikonEnv::Static()->ReadResource(txt, R_TBUF_BJACKCACHEFULLSKIP);
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKCACHEFULLSKIP);
				dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + 0x100 + 3);
				
				if (resolveAction == KHostResInquiry)
				{
					TBuf<30> temp;
					temp.Format(_L("%x:%x:%x:%x:%x:%x - "), btDevAddr[0], btDevAddr[1], 
					btDevAddr[2], btDevAddr[3], btDevAddr[4], btDevAddr[5]);
					dataBuffer->Des().Insert(0, temp);
					logHelper->updateLog(dataBuffer, 0, CLogger::logScanning);
					
				/*
					TBuf<65> temp;
					temp.Format(_L("%x:%x:%x:%x:%x:%x - "), btDevAddr[0], btDevAddr[1], 
					btDevAddr[2], btDevAddr[3], btDevAddr[4], btDevAddr[5]);
					temp.Append(txt);
					logHelper->updateLog(&temp, 0, CLogger::logScanning);
				*/
				}
				else
				{
					dataBuffer->Des().Insert(0, _L(" - "));
					dataBuffer->Des().Insert(0, nameEntry().iName);
				
				/*			
					TBuf<0x100> temp;
					temp.Copy(nameEntry().iName);
					temp.Append(_L(" - "));
					temp.Append(txt);
					logHelper->updateLog(&temp, 0, CLogger::logScanning);
				*/
				}
				delete dataBuffer;
			}
		}	
		currentNoDeviceAttempts = 0;
		return isInserted;
	}
}

void CScanner::DoCancel()
{
	if (obexClient)
	{
		//obexClient->Abort();
		delete obexClient;
		obexClient = NULL;
	}
/*
	if (obexFile)
	{
		delete obexFile;
		obexFile = NULL;
	}
*/
	if (hostResolverOpen)
	{
		hostResolver.Close();
		hostResolverOpen = EFalse;
	}
}

TInt CScanner::selectDevice(void)
{
	TBTDevAddr devAddress;
	TBTDeviceName devName;
	TBTDeviceClass devClass;

	CQBTUISelectDialog* selectDialog = new (ELeave) CQBTUISelectDialog(devAddress, devName, devClass, CQBTUISelectDialog::EQBTDeviceFilterAll, EMajorServiceObjectTransfer, EFalse);
	if (selectDialog->LaunchSingleSelectDialogLD() == 3)
	{
		insertAndUpdateBTList(devAddress, devName);
		return KErrNone;
	}
	else
		return KErrCancel;
}

void CScanner::playAudio(void)
{
	if (audioPlayerUtil)
	{
		delete audioPlayerUtil;
		audioPlayerUtil = NULL;
	}
	TPtrC audioFileNamePtr(*localAudioFile);
	if (localAudioFile->Length() > 0)
	{
		TRAPD(err, audioPlayerUtil = CMdaAudioPlayerUtility::NewFilePlayerL(audioFileNamePtr, *this));
		if (err)
		{
			MapcPlayComplete(err);
		}
	}
}

void CScanner::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
	if (aError)
		MapcPlayComplete(aError);
	else
		audioPlayerUtil->Play();
}

void CScanner::MapcPlayComplete(TInt /*aError*/)
{
	if (audioPlayerUtil)
	{
		delete audioPlayerUtil;
		audioPlayerUtil = NULL;
	}
}

/*************************************************************
*
* Search for OBEX and find its RFCOMM port
*
**************************************************************/

CFindOBEX::CFindOBEX()
{
	iSdpSearchPattern = NULL;
	iMatchList = NULL;
	iAgent = NULL;
}

void CFindOBEX::StartListL(CSdpAttrValueList& /*aList*/)
{
}

void CFindOBEX::EndListL()
{
}

void CFindOBEX::releaseObjects(void)
{
	if (iAgent)
	{
		delete iAgent;
		iAgent = NULL;
	}
	if (iSdpSearchPattern)
	{
		delete iSdpSearchPattern;
		iSdpSearchPattern = NULL;
	}
	if (iMatchList)
	{
		delete iMatchList;
		iMatchList = NULL;
	}
}

void CFindOBEX::start(TRequestStatus* theStatus, const TBTDevAddr* theAddress)
{
	releaseObjects();
	localStatus = theStatus;
	
    iSdpSearchPattern = CSdpSearchPattern::NewL();
    iSdpSearchPattern->AddL(KL2CAP);
    iMatchList = CSdpAttrIdMatchList::NewL();
    iMatchList->AddL(TAttrRange(0x0000, 0x0005));
    iAgent = CSdpAgent::NewL(*this, *theAddress);
	iAgent->SetRecordFilterL(*iSdpSearchPattern);
	RFCOMMPort = -1;

	// This is an asynchronous call. On completion it calls
	// MSdpAgentNotifier::NextRecordRequestComplete() 
    iAgent->NextRecordRequestL();
}

void CFindOBEX::searchDone(TUint errorNumber)
{
	releaseObjects();
	User::RequestComplete(localStatus, errorNumber);
}

// Called when an service record request (CSdpAgent::NextRecordRequestL()) 
// operation completes.
void CFindOBEX::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
{
	if (foundOBEX == 8)
	{
		searchDone(0);
		return;
	}
	foundOBEX = 0;
	if (aError != KErrNone)
	{
		searchDone(aError);
		return;
	}
	if (aTotalRecordsCount > 0)
	{
		/*
		Gets the specified attribute for a remote service. The function is asynchronous: 
		on completion, it calls MSdpAgentNotifier::AttributeRequestComplete(). 
		If the attribute is found, the function passes it by also calling 
		MSdpAgentNotifier::AttributeRequestResult().
		*/
        iAgent->AttributeRequestL(aHandle, *iMatchList);
	}
	else
	{
		searchDone(KErrEof);
		return;
	}
}

// Called when an attribute request (CSdpAgent::AttributeRequestL()) finds 
// that there are no more attributes to be returned.
void CFindOBEX::AttributeRequestComplete(TSdpServRecordHandle /*aHandle*/, TInt aError)
{
	if (aError == KErrNone)
	{
		if (foundOBEX != 8)
			iAgent->NextRecordRequestL();
		else
		{
			searchDone(aError);
			return;
		}
	}
	else
	{
		searchDone(aError);
		return;
	}
}

// Called by the attribute request function (CSdpAgent::AttributeRequestL()) to pass the 
// results of a successful attribute request.
void CFindOBEX::AttributeRequestResult(TSdpServRecordHandle /*aHandle*/, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
{

	// We're past the protocol descriptor list
	if (aAttrID > 0x4)
		foundOBEX = 0;
		
	if (aAttrID == 0x4)
	{
		// Did we previously discover a service class attribute?
		if (foundOBEX == 3)
		{
			foundOBEX = 4;
		}
	}
	else if (aAttrID == 0x1)
	{
		foundOBEX = 1;
	}
	
	// When called on attributes that are not lists (not DEA or DES), then this calls 
	// MSdpAttributeValueVisitor::VisitAttributeValueL(), passing the attribute value
	// object itself (i.e. *this), and the value attribute type.
	if ((foundOBEX != 8) && (aAttrID <= 0x4))
	{
		aAttrValue->AcceptVisitorL(*this);
    
	}
    // Ownership has been transferred
    delete aAttrValue;
}

void CFindOBEX::VisitAttributeValueL(CSdpAttrValue& aValue, TSdpElementType aType)
{
	if (foundOBEX == 8)
		return;
		
	// Looking for service class attribute
	if (foundOBEX == 1)
	{
		if (aType == ETypeDES)
			foundOBEX = 2;
	}
	// Found service class attribute. Is it OBEX?
	else if (foundOBEX == 2)
	{
		if (aType == ETypeUUID)
		{
			if (aValue.UUID() == 0x1105)
				foundOBEX = 3;
		}
	}
	// Found OBEX service class and Protocol descriptor list. Is this RFCOMM?
	else if (foundOBEX == 4)
	{
		if (aType == ETypeDES)
			foundOBEX = 5;
	}
	else if (foundOBEX == 5)
	{
		if (aType == ETypeDES)
			foundOBEX = 6;
	}
	else if (foundOBEX == 6)
	{
		if (aType == ETypeUUID)
		{
			// Is it RFCOMM?
			if (aValue.UUID() == 0x0003)
				foundOBEX = 7;
			else
				foundOBEX = 5;
		}
		else if (foundOBEX != 8)
			foundOBEX = 5;
	}
	else if (foundOBEX == 7)
	{
		if (aType == ETypeUint)
		{
			RFCOMMPort = aValue.Uint();
			foundOBEX = 8;
		}
	}
}

/*************************************************************
*
* Timeout handler
*
**************************************************************/

CTimeout::CTimeout() : CTimer(EPriorityNormal)
{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
}

void CTimeout::RunL(void)
{
	TTime currentTime;
	TTimeIntervalSeconds interval;
			
	currentTime.HomeTime();
	if (currentTime.SecondsFrom(timeoutStart, interval) != 0)
		interval = localObexTimeout;
	if (interval.Int() >= localObexTimeout)
	{
		// Timed out. Is the scanner object still waiting?
		if (scanObj->IsActive())
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BJACKTIMEOUT);
			scanObj->logHelper->updateLog(dataBuffer, 0, CLogger::logOBEX);
			delete dataBuffer;
			scanObj->timeoutAbort();
		}
	}
	else
		After(TIMER_GRANULARITY);
}

void CTimeout::startTimer(CScanner *theScanObj, TInt obexTimeout)
{
	scanObj = theScanObj;
	localObexTimeout = obexTimeout;
	timeoutStart.HomeTime();
	stopTimer();	
	After(TIMER_GRANULARITY);
}

void CTimeout::stopTimer(void)
{
	if (IsActive())
		Cancel();
}

/*************************************************************
*
* Bluebeamer. Quite similar to scanner class
*
**************************************************************/

CBlueBeam::CBlueBeam(CConfig *cData) : CTimer(EPriorityNormal)
{
	beamerProtocol = -1;	// Undefined default protocol
	fakeAutoSaveFlag = EFalse;
	beamerStatus = isIdle;
	selectedFiles = 0;
	logHelper = new (ELeave) CLogger(cData);
	logHelper->controlId = cBluebeamLog;
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
}

CBlueBeam::~CBlueBeam()
{
	delete logHelper;
}

void CBlueBeam::startBeam(void)
{
	CSMan2AppUi *tempUiObj = STATIC_CAST(CSMan2AppUi*, CEikonEnv::Static()->EikAppUi());
	if (selArray)
	{
		delete selArray;
		selArray = NULL;
	}
	// Make a copy of the selection indexes so we can track the bluebeam progress
	selArray = new (ELeave) CArrayFixFlat<TInt>(tempUiObj->iFileView->cFileListBox->SelectionIndexes()->Count());
	tempUiObj->iFileView->cFileListBox->View()->GetSelectionIndexesL(selArray);

	logHelper->localAutoSave = &fakeAutoSaveFlag;
	beamerStatus = isStarting;
	if (selectDevice() == KErrNone)
		doBeam();
	else
		beamerStatus = isIdle;
}

TInt CBlueBeam::selectDevice(void)
{
	if (beamerProtocol == protocolBluetooth)
	{
		// Service filter changed because some users were reporting problems of SMan not being able to find their OBEX server.
		// I think the OBEX server's implementation is the issue here.
		
		//CQBTUISelectDialog* selectDialog = new (ELeave) CQBTUISelectDialog(devAddress, devName, devClass, CQBTUISelectDialog::EQBTDeviceFilterAll, EMajorServiceObjectTransfer, EFalse);
		CQBTUISelectDialog* selectDialog = new (ELeave) CQBTUISelectDialog(devAddress, devName, devClass, CQBTUISelectDialog::EQBTDeviceFilterAll, -1, EFalse);
		if (selectDialog->LaunchSingleSelectDialogLD() == 3)
		{
			return KErrNone;
		}
		else
		{
			return KErrCancel;
		}
	} 
	else if (beamerProtocol == protocolIrDA)
	{
		RSocketServ rSocketServ;
		TProtocolDesc protoDesc;
		
		rSocketServ.Connect();
		if ((rSocketServ.FindProtocol(_L("IrTinyTP"), protoDesc)) != KErrNone)
		{
			rSocketServ.Close();
			CEikonEnv::Static()->InfoMsg(R_TBUF_BEAMER_IRDAMISSING);
			User::Leave(0);
		}
		RHostResolver hostResolver;
		if (hostResolver.Open(rSocketServ,protoDesc.iAddrFamily,protoDesc.iProtocol) != KErrNone)
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_BEAMER_IRDAHRERR);
			rSocketServ.Close();
			return KErrCancel;
		}
				
		THostName hostName;
		TInt ret;
		ret = hostResolver.GetByName(hostName, nameEntry); // Finds all IrLAP stations.
		hostResolver.Close();
		rSocketServ.Close();
		if (ret == KErrNone)
		{
			devName.Copy(nameEntry().iName);
			return KErrNone;
		}
		else
		{
			CEikonEnv::Static()->InfoMsg(R_TBUF_BEAMER_IRDANODEVICE);
			return KErrCancel;
		}
	}
	else
		return KErrCancel;
}

void CBlueBeam::doBeam(void)
{
	if (beamerProtocol == protocolBluetooth)
	{
		// Find OBEX
		iStatus = KRequestPending;
		beamerStatus = isOBEXSearch;
		obexRetries = OBEX_RETRIES;
		obexSearcher.start(&iStatus, &devAddress);
	}
	bluebeamLogDialog = new (ELeave) CBluebeamLogDialog(logHelper);
	bluebeamLogDialog->ExecuteLD(R_DIALOG_BLUEBEAMLOG);
	selectedFiles = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->cFileListBox->SelectionIndexes()->Count();
	logHelper->logBuffer = logBuffer;
	
	if (beamerProtocol == protocolBluetooth)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXSEARCH);
		logWithName(dataBuffer);
		delete dataBuffer;

		/*
		TBuf<30> txt;
		CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXSEARCH);
		logWithName(&txt);
		*/
		SetActive();
	}
	else if (beamerProtocol == protocolIrDA)
	{
		// IrDA doesn't need OBEX search so we can start beaming files immediately.
		// However, the obex client is not ready yet so we must manually initialize it
		// first
		
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXCONNECT);
		logWithName(dataBuffer);
		delete dataBuffer;
		
		/*
		TBuf<30> txt;
		CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXCONNECT);
		logWithName(&txt);
		*/
		
		obexIRProtoInfo.iAttributeName.Copy(_L("IrDA:TinyTP:LsapSel"));
		obexIRProtoInfo.iClassName.Copy(_L("OBEX"));
		obexIRProtoInfo.iAddr.Cast(nameEntry().iAddr);
		obexIRProtoInfo.iTransport.Copy(_L("IrTinyTP"));
		if (obexClient)
		{
			delete obexClient;
			obexClient = NULL;
		}
		obexClient = CObexClient::NewL(obexIRProtoInfo);
		beamerStatus = isOBEXConnect;
		iStatus = KRequestPending;

		// Start OBEX connect
		obexClient->Connect(iStatus);
		SetActive();
	}
}

void CBlueBeam::sendOBEXFile(void)
{
	TPtrC tempText, selectedFile, fileType;
	
	CSMan2AppUi *tempUiObj = STATIC_CAST(CSMan2AppUi*, CEikonEnv::Static()->EikAppUi());
	tempText.Set(tempUiObj->iFileView->cFileListBox->Model()->ItemTextArray()->MdcaPoint(tempUiObj->iFileView->cFileListBox->SelectionIndexes()->At(selectedFiles)));
	
	// Is this a folder? We don't beam folders. Sorry!
	TextUtils::ColumnText(fileType, 1, &tempText);
	if (fileType.Compare(_L("<dir>")) == 0)
	{
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_SKIPFOLDER);
		dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + KMaxFileName);
		
		//TBuf<KMaxFileName + 20> txt;
		//CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_SKIPFOLDER);
		
		TextUtils::ColumnText(selectedFile, 0, &tempText);
		currentFileName.Copy(tempUiObj->iFileView->currentPath);
		currentFileName.Append(selectedFile);

		//txt.Append(currentFileName);
		//logHelper->updateLog(&txt, 0, CLogger::logAll);
		dataBuffer->Des().Append(currentFileName);
		logHelper->updateLog(dataBuffer, 0, CLogger::logAll);
		
		delete dataBuffer;

		iStatus = KErrNone;
		beamerStatus = isOBEXNextFile;
		// Get next item
		After(1);
		return;
	}

	if (obexFile)
	{
		delete obexFile;
		obexFile = NULL;
	}
	obexFile = CObexFileObject::NewL(TPtrC(NULL, 0));
	
	TextUtils::ColumnText(selectedFile, 0, &tempText);
	currentFileName.Copy(tempUiObj->iFileView->currentPath);
	currentFileName.Append(selectedFile);
	
	TRAPD(err, obexFile->InitFromFileL(currentFileName));
	if (err)
	{	
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_FILEOPENERR);
		dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + KMaxFileName);
		CEikonEnv::Static()->InfoMsg(*dataBuffer);
		dataBuffer->Des().Append(currentFileName);
		logHelper->updateLog(dataBuffer, 0, CLogger::logAll);
		delete dataBuffer;

		/*
		TBuf<KMaxFileName + 20> txt;
		CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_FILEOPENERR);
		CEikonEnv::Static()->InfoMsg(txt);
		txt.Append(currentFileName);
		logHelper->updateLog(&txt, 0, CLogger::logAll);
		*/
		
		iStatus = KErrNone;
		beamerStatus = isOBEXNextFile;
		delete obexFile;
		obexFile = NULL;
		// On with the next file
		After(1);
	}
	else
	{	
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_SENDING);
		dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + KMaxFileName);
		dataBuffer->Des().Append(currentFileName);
		logWithName(dataBuffer);
		delete dataBuffer;

		/*
		TBuf<KMaxFileName + 11> txt;
		CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_SENDING);
		txt.Append(currentFileName);
		logWithName(&txt);
		*/
		
		iStatus = KRequestPending;
		beamerStatus = isOBEXSend;
		obexClient->Put(*obexFile, iStatus);
		SetActive();
	}
}

void CBlueBeam::logWithName(const TDesC *buffer)
{
	HBufC *dataBuffer = HBufC::NewL(buffer->Length() + devName.Length() + KMaxFileName);
	dataBuffer->Des().Copy(devName);
	dataBuffer->Des().Append(_L(" - "));
	dataBuffer->Des().Append(*buffer);
	logHelper->updateLog(dataBuffer, 0, CLogger::logAll);
	delete dataBuffer;
	
	/*
	TBuf<KMaxFileName + 0x100 + 30> txt;
	txt.Copy(devName);
	txt.Append(_L(" - "));
	txt.Append(*buffer);
	logHelper->updateLog(&txt, 0, CLogger::logAll);
	*/
}

void CBlueBeam::RunL()
{
	//TBuf<50> txt;
	
	if (beamerStatus == isOBEXSearch)
	{
		// I could have passed this via iStatus too.
		if (obexSearcher.RFCOMMPort > -1)
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXCONNECT);
			logWithName(dataBuffer);
			delete dataBuffer;
			
			/*
			CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXCONNECT);
			logWithName(&txt);
			*/
			
			obexBTProtoInfo.iTransport.Copy(_L("RFCOMM"));
			obexBTProtoInfo.iAddr.SetBTAddr(devAddress);
			obexBTProtoInfo.iAddr.SetPort(obexSearcher.RFCOMMPort);
			if (obexClient)
			{
				delete obexClient;
				obexClient = NULL;
			}
			obexClient = CObexClient::NewL(obexBTProtoInfo);
			beamerStatus = isOBEXConnect;
			iStatus = KRequestPending;

			// Start OBEX connect
			obexClient->Connect(iStatus);
			SetActive();
		}
		else
		{
			if (obexRetries > 0)
			{
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXRETRY);
				logWithName(dataBuffer);
				delete dataBuffer;
				
				/*
				CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXRETRY);
				logWithName(&txt);
				*/
				
				obexRetries--;
				iStatus = KRequestPending;
				beamerStatus = isOBEXSearch;
				obexSearcher.start(&iStatus, &devAddress);
				SetActive();
			}
			else
			{
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXSEARCHERR);
				logWithName(dataBuffer);
				delete dataBuffer;

				/*
				CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXSEARCHERR);
				logWithName(&txt);
				*/
				
				beamerStatus = isIdle;
			}
		}			
	}
	else if ((beamerStatus == isOBEXConnect) || (beamerStatus == isOBEXNextFile))
	{
		if (iStatus == KErrNone)
		{
			selectedFiles--;
			if (selectedFiles >= 0)
				sendOBEXFile();
			else
			{
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXDISCONNECT);
				logWithName(dataBuffer);
				delete dataBuffer;

				/*
				CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXDISCONNECT);
				logWithName(&txt);
				*/

				iStatus = KRequestPending;
				beamerStatus = isOBEXDisconnect;
				obexClient->Disconnect(iStatus);
				SetActive();
			}
		}
		else
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXCONNECTERR);
			logWithName(dataBuffer);
			delete dataBuffer;
		
			/*
			CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXCONNECTERR);
			logWithName(&txt);
			*/
			beamerStatus = isIdle;
		}
	}
	else if (beamerStatus == isOBEXSend)
	{
		if (iStatus == KErrNone)
		{
			// Unmark the file in the filebrowser
			static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iFileView->cFileListBox->View()->VerticalMoveToItemL(selArray->At(selectedFiles), CListBoxView::EDisjointSelection);
			selectedFiles--;
			if (selectedFiles >= 0)
			{
				sendOBEXFile();
				return;
			}
			else
			{
				HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXDISCONNECT);
				logWithName(dataBuffer);
				delete dataBuffer;

				/*
				CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXDISCONNECT);
				logWithName(&txt);
				*/
				
				if (obexFile)
				{
					delete obexFile;
					obexFile = NULL;
				}
			}
		}
		else
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXPUTERR);
			logWithName(dataBuffer);
			delete dataBuffer;

			/*
			CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXPUTERR);
			logWithName(&txt);
			*/
		}
		iStatus = KRequestPending;
		beamerStatus = isOBEXDisconnect;
		obexClient->Disconnect(iStatus);
		SetActive();
	}
	else if (beamerStatus == isOBEXDisconnect)
	{
		if (iStatus == KErrNone)
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_DONE);
			logWithName(dataBuffer);
			delete dataBuffer;

			/*
			CEikonEnv::Static()->ReadResource(txt, R_TBUF_MISC_DONE);
			logWithName(&txt);
			*/
		}
		else
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_OBEXDISCONNECTERR);
			logWithName(dataBuffer);
			delete dataBuffer;
		
			/*
			CEikonEnv::Static()->ReadResource(txt, R_TBUF_BEAMER_OBEXDISCONNECTERR);
			logWithName(&txt);
			*/
		}
		beamerStatus = isIdle;
	}

	// Cleanup!
	if (beamerStatus == isIdle)
	{
		if (logHelper->logDialog)
		{
			bluebeamLogDialog->ButtonGroupContainer().DimCommand(EEikBidYes, EFalse);
			bluebeamLogDialog->ButtonGroupContainer().DimCommand(EEikBidOk, ETrue);	
		}
		if (selArray)
		{
			delete selArray;
			selArray = NULL;
		}
		if (obexFile)
		{
			delete obexFile;
			obexFile = NULL;
		}
		if (obexClient)
		{
			delete obexClient;
			obexClient = NULL;
		}
	}
}

void CBlueBeam::stopBeam()
{
	if (IsActive())
	{
		if (obexClient)
			obexClient->Abort();			

		if (beamerStatus != isOBEXNextFile)
		{
			if (obexClient)
			{
				delete obexClient;
				obexClient = NULL;
			}
			if (obexFile)
			{
				delete obexFile;
				obexFile = NULL;
			}
		}		

		beamerStatus = isIdle;

		// This is kludgy but it works.

		TRequestStatus* tempStatus;		
		tempStatus = &iStatus;
		User::RequestComplete(tempStatus, KErrNone);
		User::WaitForRequest(iStatus);

		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_BEAMER_CANCELLED);
		logWithName(dataBuffer);
		delete dataBuffer;
	}
	else
		beamerStatus = isIdle;
}

void CBlueBeam::DoCancel()
{
	stopBeam();
}

