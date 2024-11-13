#include "viewbase.h"
#include "sman.h"
#include <e32math.h>
#include <sman.rsg>

/*************************************************************
*
* DTMF Dialer view.
*
**************************************************************/

CSMan2DTMFDialerView::CSMan2DTMFDialerView(CConfig *cData) : CViewBase(cData), iWAVDataDescriptor(0,0,0)
{
	iPhoneNumber = NULL;
	iPhoneNumberPosition = -1;
	iSilenceCounter = -1;
}

TBool CSMan2DTMFDialerView::ViewScreenModeCompatible(TInt aScreenMode)
{
	return (aScreenMode == 0);
}

TVwsViewIdAndMessage CSMan2DTMFDialerView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2DTMFDialerView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
		DeallocateWAVData();
	}
}

void CSMan2DTMFDialerView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	doViewActivated();
	GenerateWAVData();
	InitDialerState();
}

TVwsViewId CSMan2DTMFDialerView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidDTMFDialerView);
}

CSMan2DTMFDialerView::~CSMan2DTMFDialerView()
{
	if (iAudioPlayer)
	{
		//iAudioPlayer->Stop();
		delete iAudioPlayer;
	}

	DeallocateWAVData();
}

TKeyResponse CSMan2DTMFDialerView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	// Jogdial allows you to scroll the number text box up and down
	if (aType == EEventKey)
	{
		if (aKeyEvent.iCode == EQuartzKeyTwoWayDown)
		{
			iDTMFNumbers->MoveDisplayL(TCursorPosition::EFLineDown);
			return EKeyWasConsumed;
		}
		else if (aKeyEvent.iCode == EQuartzKeyTwoWayUp)
		{
			iDTMFNumbers->MoveDisplayL(TCursorPosition::EFLineUp);
			return EKeyWasConsumed;
		}
	}
	
	if (iPhoneNumberPosition == -1)
	{
		// Handle default cut, copy, paste key combos
		if (aKeyEvent.iModifiers == 262272)
		{
			// 3 = copy, 24 = cut, 22 = paste
			if ((aKeyEvent.iCode == 3) || (aKeyEvent.iCode == 24) || (aKeyEvent.iCode == 22))
				return iDTMFNumbers->OfferKeyEventL(aKeyEvent, aType);
		}
		
		// Only accept the following keys
		// 0-9 = 48-57, * = 42, # = 35, P = 80, p = 112, backspace = 8
		if (((aKeyEvent.iCode >= 48) && (aKeyEvent.iCode <= 57)) || (aKeyEvent.iCode == 42) || 
			(aKeyEvent.iCode == 35) || (aKeyEvent.iCode == 80) || (aKeyEvent.iCode == 112) ||
			(aKeyEvent.iCode == 8))
			return iDTMFNumbers->OfferKeyEventL(aKeyEvent, aType);
	}

	return EKeyWasNotConsumed;
}

void CSMan2DTMFDialerView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
	CCoeControl::HandlePointerEventL(aPointerEvent);
}

void CSMan2DTMFDialerView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());
	
	// Initialize controls	
	iDTMFDialButton = new (ELeave) CEikBitmapButton();
	iDTMFDialButton->SetPictureFromFileL(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName(), EMbmMyHeaderDial, EMbmMyHeaderDialmask, CEikCommandButtonBase::EFirst);
	iDTMFDialButton->SetContainerWindowL(*this);
	iDTMFDialButton->SetObserver(this);
	iDTMFDialButton->SetExtent(TPoint(aRect.Size().iWidth - 42, 0), TSize(42, 42));
	controlsArray->AppendL(iDTMFDialButton);
	
	iDTMFNumbers = new (ELeave) CEikGlobalTextEditor;
	iDTMFNumbers->ConstructL(this, 20, 256, CEikEdwin::ENoAutoSelection | CEikEdwin::EOnlyASCIIChars, EGulFontControlAll, EGulAllFonts);
	iDTMFNumbers->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth - iDTMFDialButton->Size().iWidth, iDTMFDialButton->Size().iHeight));
	controlsArray->AppendL(iDTMFNumbers);

	TInt iButWidth = aRect.Size().iWidth / 3;
	TInt iButHeight = (aRect.Size().iHeight - iDTMFDialButton->Size().iHeight - 10) / 4;
	
	TInt iTempIndex;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			iTempIndex = y * 3 + x;
			iDTMFButtons[iTempIndex] = new (ELeave) CEikCommandButton();
			iDTMFButtons[iTempIndex]->SetTextL(_L("9"));
			iDTMFButtons[iTempIndex]->SetContainerWindowL(*this);
			iDTMFButtons[iTempIndex]->SetObserver(this);
			iDTMFButtons[iTempIndex]->SetExtent(TPoint(iButWidth * x, iButHeight * y + iDTMFDialButton->Size().iHeight), TSize(iButWidth, iButHeight));
			iDTMFButtons[iTempIndex]->SetBehavior(EEikButtonReportsOnPointerDown);
			controlsArray->AppendL(iDTMFButtons[iTempIndex]);
		}
	}
	
	TBuf<2> iTempNum;
	for (iTempIndex = 0; iTempIndex < 10; iTempIndex++)
	{
		iTempNum.Format(_L("%d"), iTempIndex + 1);
		iDTMFButtons[iTempIndex]->SetTextL(iTempNum);
	}
	iDTMFButtons[9]->SetTextL(_L("*"));
	iDTMFButtons[10]->SetTextL(_L("0"));
	iDTMFButtons[11]->SetTextL(_L("#"));
	
	// To indicate the WAV data buffers do not have the necessary sound data
	iWAVDataReady = EFalse;
	
	// Enlarge dialer font
	/*
	TInt lineSpaceTwips[3] = {150, 200, 300};
	TCharFormat charFormat;
	TCharFormatMask charFormatMask;
	CParaFormat paraFormat;
	TParaFormatMask paraFormatMask;
	charFormat.iFontSpec.iHeight = ((3 - ZOOM_INCREMENT) / ZOOM_INCREMENT) * 150 + 100;
	charFormat.iFontSpec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
	charFormatMask.SetAttrib(EAttFontHeight);
	charFormatMask.SetAttrib(EAttFontStrokeWeight);
	paraFormat.iLineSpacingInTwips = lineSpaceTwips[(zoomLevel / ZOOM_INCREMENT) - 1];
	paraFormatMask.SetAttrib(EAttLineSpacing);
	iDTMFNumbers->ApplyCharFormatL(charFormat, charFormatMask);
	iDTMFNumbers->ApplyParaFormatL(&paraFormat, paraFormatMask);
	*/
	
	iAudioPlayer = NULL;
	
	// Finish up view initialisation
	viewId = CSMan2AppUi::EViewDTMFDialer;
	iBJackIconPosition = TPoint(2, iDTMFButtons[9]->Position().iY + iDTMFButtons[9]->Size().iHeight + 2);
	InitBJackIcon();
	this->EnableDragEvents();
		
	MakeVisible(EFalse);
	ActivateL();
	
	InitDialerState();	
}

void CSMan2DTMFDialerView::InitDialerState(void)
{
	if (iPhoneNumberPosition == -1)
		iDTMFNumbers->SetFocus(configData->iDTMFInput, EDrawNow);
	else
		iDTMFNumbers->SetFocus(EFalse);
	iDTMFDialButton->SetDimmed(!configData->iDTMFInput);
	iDTMFDialButton->DrawNow();
	for (int i = 0; i < 12; i++)
	{
		iDTMFButtons[i]->SetDimmed(configData->iDTMFInput);
		iDTMFButtons[i]->DrawNow();
	}
}

void CSMan2DTMFDialerView::HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType)
{
	// This if check is redundant but I've included it because it's safer
	if (aEventType != MCoeControlObserver::EEventInteractionRefused)
	{
		if (!configData->iDTMFInput)
		{
			if ((Index(aControl) >= 2) && (Index(aControl) <= 13))
			{
				iPhoneNumberPosition = -1;
				DoDialPad(Index(aControl) - 2);
			}
		}
		else
		{
			if (iPhoneNumberPosition == -1)
				DoDialNumbers();
			else
			{
				delete iAudioPlayer;
				iAudioPlayer = NULL;
				iPhoneNumberPosition = -1;
				iDTMFNumbers->SetFocus(ETrue);
			}
		}
	}
}

void CSMan2DTMFDialerView::GetNextValidDigit(void)
{
	if (iSilenceCounter > -1)
	{
		DoDialPad(0);
	}
	else
	{
		while (iPhoneNumberPosition < iPhoneNumber->Length())
		{
			// 1-9
			if ((((iPhoneNumber->Des())[iPhoneNumberPosition]) >= 49) && (((iPhoneNumber->Des())[iPhoneNumberPosition]) <= 57))
			{
				DoDialPad((iPhoneNumber->Des()[iPhoneNumberPosition]) - 49);
				iPhoneNumberPosition++;
				break;
			}
			// *
			else if (((iPhoneNumber->Des())[iPhoneNumberPosition]) == 0x2a)
			{
				DoDialPad(9);
				iPhoneNumberPosition++;
				break;
			}
			// 0
			else if (((iPhoneNumber->Des())[iPhoneNumberPosition]) == 48)
			{
				DoDialPad(10);
				iPhoneNumberPosition++;
				break;
			}
			// # 
			else if (((iPhoneNumber->Des())[iPhoneNumberPosition]) == 0x23)
			{
				DoDialPad(11);
				iPhoneNumberPosition++;
				break;
			}
			// p or P
			else if ((((iPhoneNumber->Des())[iPhoneNumberPosition]) == 0x50) || (((iPhoneNumber->Des())[iPhoneNumberPosition]) == 0x70))
			{
				// Since each sample is 0.2secs, looping it 10 times produces a 2 sec playback silence
				// That's theory! Unfortunately, there is a slight overheard in initializing the media player
				// utility class. A loop of 10 ends up playing the silence for 4 seconds! So the magic
				// number is about 5. Ok, this is bad coding but I'm just plain lazy. For god's sake, it's a
				// DTMF DIALER!!!! not some life-supporting system!
				iSilenceCounter = 5;
				DoDialPad(0);
				iPhoneNumberPosition++;
				break;
			}
			// Invalid char
			else
				iPhoneNumberPosition++;
		}
		if (iPhoneNumberPosition >= iPhoneNumber->Length())
		{
			iPhoneNumberPosition = -1;
			delete iPhoneNumber;
			iPhoneNumber = NULL;
			iDTMFNumbers->SetFocus(ETrue);
		}
	}
}

// Read the number input area and dial!
// This function will ignore any invalid characters. It will not delete them. This is useful if
// you have a number embedded in an SMS message. You cut and paste the entire SMS into the dialer
// and it will only dial the number!
void CSMan2DTMFDialerView::DoDialNumbers(void)
{
	if (iPhoneNumber)
	{
		delete iPhoneNumber;
		iPhoneNumber = NULL;
	}
		
	iPhoneNumber = iDTMFNumbers->GetTextInHBufL();
	if (iPhoneNumber)
	{
		iPhoneNumber->Des().Trim();
		if (iPhoneNumber->Length() > 0)
		{
			iPhoneNumberPosition = 0;
			iDTMFNumbers->SetFocus(EFalse);
			GetNextValidDigit();
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_DTMF_NONUMBER);
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_DTMF_NONUMBER);
}

// Respond to keypad by playing the appropriate WAV descriptor
void CSMan2DTMFDialerView::DoDialPad(TInt aNumber)
{
	//GenerateWAVData();
	if (iAudioPlayer)
	{
		delete iAudioPlayer;
		iAudioPlayer = NULL;
	}
	
	iWAVDataDescriptor.Set(iWAVData[aNumber]->Des());
	TRAPD(err, iAudioPlayer = CMdaAudioPlayerUtility::NewDesPlayerReadOnlyL(iWAVDataDescriptor, *this));
	if (err)
	{
		iSilenceCounter = -1;
		MapcPlayComplete(err);
	}
}

void CSMan2DTMFDialerView::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
	if (aError)
		MapcPlayComplete(aError);
	else
	{
		if (iSilenceCounter != -1)
		{
			iAudioPlayer->SetVolume(0);
			iSilenceCounter--;
		}
		else
			iAudioPlayer->SetVolume(iAudioPlayer->MaxVolume());
		iAudioPlayer->Play();
	}
}

void CSMan2DTMFDialerView::MapcPlayComplete(TInt /*aError*/)
{
	if (iAudioPlayer)
	{
		delete iAudioPlayer;
		iAudioPlayer = NULL;
	}
	if (iPhoneNumberPosition != -1)
		GetNextValidDigit();
}

// Stop and resets the dialer
void CSMan2DTMFDialerView::StopAndResetDialer(void)
{
	if (iAudioPlayer)
	{
		delete iAudioPlayer;
		iAudioPlayer = NULL;
	}
	iPhoneNumberPosition = -1;
	iSilenceCounter = -1;
	if (iPhoneNumber)
	{
		delete iPhoneNumber;
		iPhoneNumber = NULL;
	}
}

void CSMan2DTMFDialerView::DeallocateWAVData(void)
{
	if (iWAVDataReady)
	{		
		// To prevent any complications, terminate all playing audio and reset the dialer to an
		// idle state
		StopAndResetDialer();
		
		// Now we can remove the WAV data
		for (int iTempIndex = 0; iTempIndex < 12; iTempIndex++)
			delete iWAVData[iTempIndex];
		iWAVDataReady = EFalse;
		User::Heap().Compress();
	}
}

// Generate WAV data
void CSMan2DTMFDialerView::GenerateWAVData(void)
{	
	if (!iWAVDataReady)
	{
		// Whenever you see xxxLo, xxxHi, and a whole bunch of Mem::Copy commands AND the iInt32Temp
		// variable, this is to avoid the word alignment feature of ARM processors for 32 bit
		// values
		struct sWAVHeader
		{
			char iRIFFText[4];
			TUint16 iPackageLengthLo;
			TUint16 iPackageLengthHi;
			char iWAVEText[4];
			
			char iFMTText[4];
			TUint16 iFormatLengthLo;
			TUint16 iFormatLengthHi;
			TUint16 iAudioFormat;
			TUint16 iNumChannels;
			TUint16 iSampleRateLo;
			TUint16 iSampleRateHi;
			TUint16 iBytesPerSecondLo;
			TUint16 iBytesPerSecondHi;
			TUint16 iBytesPerSample;
			TUint16 iBitsPerSample;
			TUint16 iUnknown;
			
			char iFACTText[4];
			TUint16 iChunkSizeLo;
			TUint16 iChunkSizeHi;
			TUint16 iSamplesPerChannelLo;
			TUint16 iSamplesPerChannelHi;
			
			char iDATAText[4];
			TUint16 iDataLengthLo;
			TUint16 iDataLengthHi;
		};
		
		sWAVHeader iWAVHeader;
		
		TUint32 iInt32Temp;
		
		// Initialize RIFF chunk
		iWAVHeader.iRIFFText[0] = 'R';
		iWAVHeader.iRIFFText[1] = 'I';
		iWAVHeader.iRIFFText[2] = 'F';
		iWAVHeader.iRIFFText[3] = 'F';
		iInt32Temp = WAV_BUFFER_SIZE + WAV_HEADER_SIZE - 8;
		Mem::Copy(&iWAVHeader.iPackageLengthLo, &iInt32Temp, sizeof(TUint32));
		iWAVHeader.iWAVEText[0] = 'W';
		iWAVHeader.iWAVEText[1] = 'A';
		iWAVHeader.iWAVEText[2] = 'V';
		iWAVHeader.iWAVEText[3] = 'E';
		
		// Initialize FORMAT chunk
		iWAVHeader.iFMTText[0] = 'f';
		iWAVHeader.iFMTText[1] = 'm';
		iWAVHeader.iFMTText[2] = 't';
		iWAVHeader.iFMTText[3] = ' ';
		iInt32Temp = 0x12;
		Mem::Copy(&iWAVHeader.iFormatLengthLo, &iInt32Temp, sizeof(TUint32));
		iWAVHeader.iAudioFormat = 0x1;	// PCM, Linear quantization
		iWAVHeader.iNumChannels = 0x1;	// 1 = mono, 2 = stereo
		iInt32Temp = 8000;	// 8Khz
		Mem::Copy(&iWAVHeader.iSampleRateLo, &iInt32Temp, sizeof(TUint32));
		iInt32Temp = 8000;
		Mem::Copy(&iWAVHeader.iBytesPerSecondLo, &iInt32Temp, sizeof(TUint32));
		iWAVHeader.iBytesPerSample = 1;	// 1 = 8bit mono
		iWAVHeader.iBitsPerSample = 8;
		iWAVHeader.iUnknown = 0;
		
		// Initialize FACT chunk
		iWAVHeader.iFACTText[0] = 'f';
		iWAVHeader.iFACTText[1] = 'a';
		iWAVHeader.iFACTText[2] = 'c';
		iWAVHeader.iFACTText[3] = 't';
		iInt32Temp = 4;
		Mem::Copy(&iWAVHeader.iChunkSizeLo, &iInt32Temp, sizeof(TUint32));
		iInt32Temp = WAV_BUFFER_SIZE;
		Mem::Copy(&iWAVHeader.iSamplesPerChannelLo, &iInt32Temp, sizeof(TUint32));
		
		// Initialize DATA chunk
		iWAVHeader.iDATAText[0] = 'd';
		iWAVHeader.iDATAText[1] = 'a';
		iWAVHeader.iDATAText[2] = 't';
		iWAVHeader.iDATAText[3] = 'a';
		iInt32Temp = WAV_BUFFER_SIZE;
		Mem::Copy(&iWAVHeader.iDataLengthLo, &iInt32Temp, sizeof(TUint32));		

		// Setup the frequency table
		TInt xFreq[3] = {1209, 1336, 1477};
		TInt yFreq[4] = {697, 770, 852, 941};
		
		// Formula: sample(n) = 128 + 63*sin(n*2*pi*f1/8000) + 63*sin(n*2*pi*f2/8000)
		
		TReal iPiConst = 3.1415926535897 * 2 / 8000;
		TReal iSinResult1, iSinResult2;
		CEikonEnv::Static()->BusyMsgL(R_TBUF_DTMF_GENERATING, 0);
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 3; x++)
			{
				iWAVData[x + y * 3] = HBufC8::NewL(WAV_BUFFER_SIZE + WAV_HEADER_SIZE);
				HBufC8 *iTempWAVDataPtr = iWAVData[x + y * 3];
				iTempWAVDataPtr->Des().FillZ(WAV_BUFFER_SIZE + WAV_HEADER_SIZE);
				unsigned char *iPointerToData = (unsigned char*)(iTempWAVDataPtr->Des().Ptr());
				Mem::Copy(iPointerToData, &iWAVHeader, WAV_HEADER_SIZE);
				iPointerToData += WAV_HEADER_SIZE;
				for (int iSampleNumber = 0; iSampleNumber < WAV_BUFFER_SIZE; iSampleNumber++)
				{
					Math::Sin(iSinResult1, iSampleNumber * iPiConst * xFreq[x]);
					Math::Sin(iSinResult2, iSampleNumber * iPiConst * yFreq[y]);
					iPointerToData[iSampleNumber] = TInt8(128 + 63 * iSinResult1 + 63 * iSinResult2);
				}
			}
		}

/*
	TBuf8<1658> dataFile;
	RFile inStream;
	inStream.Open(CEikonEnv::Static()->FsSession(), _L("D:\\DTMF-0-PCM.WAV"), EFileRead);
	inStream.Read(0, dataFile);
	inStream.Close();
	iWAVData[0]->Des().Copy(dataFile);
	RFile inStream;
	inStream.Open(CEikonEnv::Static()->FsSession(), _L("D:\\test.WAV"), EFileWrite);
	inStream.Write(iWAVData[0]->Des());
	inStream.Close();
*/

		CEikonEnv::Static()->BusyMsgCancel();
		iWAVDataReady = ETrue;
	}
}

void CSMan2DTMFDialerView::DoEditCommands(TInt aCommand)
{
	if (iPhoneNumberPosition == -1)
	{
		CClipboard *oClipboard;

		if ((aCommand == cmdDTMFEditCopy) || (aCommand == cmdDTMFEditCut))
		{
			if (iDTMFNumbers->SelectionLength() > 0)
			{
				oClipboard = CClipboard::NewForWritingLC(CEikonEnv::Static()->FsSession());
				iDTMFNumbers->CopyToStoreL(oClipboard->Store(), oClipboard->StreamDictionary());
				oClipboard->CommitL();			
				if (aCommand == cmdDTMFEditCut)
				{
					iDTMFNumbers->InsertDeleteCharsL(0, _L(""), iDTMFNumbers->Selection());
					iDTMFNumbers->ClearSelectionL();
				}
				CleanupStack::PopAndDestroy(); // oClipboard
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_EDIT_NOTEXTSELECTED);
		}
		else if (aCommand == cmdDTMFEditPaste)
		{
			oClipboard = CClipboard::NewForReadingLC(CEikonEnv::Static()->FsSession());
			iDTMFNumbers->PasteFromStoreL(oClipboard->Store(), oClipboard->StreamDictionary());
			CleanupStack::PopAndDestroy(); // oClipboard
		}
	}
}
