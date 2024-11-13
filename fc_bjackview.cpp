#include "fc_bjackview.h"
#include "sman.h"
#include <SMan.rsg>

/*************************************************************
*
* Flip closed view
*
**************************************************************/

CSMan2FCBJackView::CSMan2FCBJackView(CConfig *cData) : CViewBase(cData)
{
}

TBool CSMan2FCBJackView::ViewScreenModeCompatible(TInt aScreenMode)
{
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasFlip))
		return (aScreenMode == 1);
	else
		return EFalse;
}

TVwsViewIdAndMessage CSMan2FCBJackView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2FCBJackView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		MakeVisible(EFalse);
		CSMan2AppUi *iSManAppUi = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi());
		iSManAppUi->RemoveFromStack(this);
		iSManAppUi->DeInitTemporaryViews(CSMan2AppUi::EViewBJack);

		// I think i found a bug in SE's flipclosed mode implementation. Doing this line of
		// code here will corrupt the graphic of the scrollbar for iBJackView.
		
		// Route logging back to original location
		//static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->oScanner->logHelper->logTextBox = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->bluejackLog;
		//static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->bluejackLog->SetTextL(&(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->oScanner->logBuffer));
		//static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->bluejackLog->SetCursorPosL(static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->bluejackLog->TextLength(), 0);
	}
}

void CSMan2FCBJackView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid aCustomMessageId, const TDesC8& /*aCustomMessage*/)
{
	if (activateCount <= 0)
	{
		doViewActivated();

		/* 
		The bluejack view in flip open is instantiated only when needed.
		This call with a custom message id and message data (although this is not used)
		tells the ViewActivatedL handler of the bluejack flip closed view that
		WE WANT THE FLIP OPEN BLUEJACK VIEW TO BE INSTANTIATED.
		If the flipclosed bluejack view never sees this uid number, it will never instantiate
		the flipopen bluejack view because it knows this is a temporary system request
		*/		
		CSMan2AppUi *iSManAppUi = static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi());
		if (aCustomMessageId == KUidSMan2App)
		{
			if (!(iSManAppUi->iBJackView))
				iSManAppUi->InitTemporaryViews(CSMan2AppUi::EViewBJack);
		}
		
		if (iSManAppUi->iBJackView)
		{
			iSManAppUi->iBJackView->oScanner->logHelper->logTextBox = bluejackLog;
			iSManAppUi->iBJackView->oScanner->logHelper->updateLog(NULL, 0, CLogger::logAll);
		}
	}
}

TVwsViewId CSMan2FCBJackView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidFCBJackView);
}

TKeyResponse CSMan2FCBJackView::HandleKey(TInt iScanCode)
{
	if ((iScanCode == EStdKeyDevice3) || (iScanCode == EStdKeyDeviceE))
	{
		// Back key. Device3 = back button, DeviceE = jogdial push
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->SetAppPosition(EFalse);
		return EKeyWasConsumed;
	}
	else if ((iScanCode == EStdKeyDevice9) || (iScanCode == EStdKeyDeviceD))
	{
		// application key. Device9 = option button, DeviceD = jogdial pull

		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->LaunchPopupMenuL(R_FC_BLUEJACK_MENU, 
			TPoint(bluejackLog->Size().iWidth, bluejackLog->Size().iHeight + 10 + bluejackLog->Position().iY), EPopupTargetBottomRight);
		return EKeyWasConsumed;
	}
	return EKeyWasNotConsumed;
}

TKeyResponse CSMan2FCBJackView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
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
		TKeyResponse aKey = HandleKey(aKeyEvent.iScanCode);
		if (aKey == EKeyWasConsumed)
			return EKeyWasConsumed;
	}
	
	return EKeyWasNotConsumed;
}

CSMan2FCBJackView::~CSMan2FCBJackView()
{
}

void CSMan2FCBJackView::ConstructL(const TRect& aRect)
{
	//popupActive = EFalse;
	
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	// CEikEdwin::EReadOnly;
	bluejackLog = new (ELeave) CEikGlobalTextEditor;
	bluejackLog->ConstructL(this, 18, MAX_LOG_LINES * MAX_LOG_LINE_SIZE, EEikEdwinInclusiveSizeFixed | CEikEdwin::EReadOnly, EGulFontControlAll, EGulAllFonts);

	TInt iHeight;
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iRunningPhoneModel == CSMan2AppUi::EPhoneModel_P800)
		iHeight = 144;
	else if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iRunningPhoneModel == CSMan2AppUi::EPhoneModel_P900)
		iHeight = 208;
	else
		iHeight = 144;
	
	iTitleBar = new (ELeave) CEikMover;
	iTitleBar->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, 25));
	iTitleBar->SetContainerWindowL(*this);
	HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FC_BLUEJACKTXT);
	iTitleBar->SetText(dataBuffer);
	controlsArray->AppendL(iTitleBar);

	bluejackLog->SetExtent(TPoint(0, iTitleBar->Size().iHeight), TSize(aRect.Size().iWidth, iHeight - 10 - iTitleBar->Size().iHeight)); //aRect.Size().iHeight - EQikToolbarHeight - 10));
	bluejackLog->CreateScrollBarFrameL();
	bluejackLog->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	controlsArray->AppendL(bluejackLog);

	viewId = CSMan2AppUi::EViewFCBJack;
	iBJackIconPosition = TPoint(2, bluejackLog->Position().iY + bluejackLog->Size().iHeight + 2);
	InitBJackIcon();

	MakeVisible(EFalse);
	ActivateL();
}

void CSMan2FCBJackView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}
