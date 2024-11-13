#include "fc_cellareaview.h"
#include "sman.h"
#include <SMan.rsg>

/*************************************************************
*
* Cell area flip closed view
*
**************************************************************/

CSMan2FCCellAreaView::CSMan2FCCellAreaView(CConfig *cData) : CViewBase(cData)
{
}

TBool CSMan2FCCellAreaView::ViewScreenModeCompatible(TInt aScreenMode)
{
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->PhoneIsCapable(CSMan2AppUi::EPhone_HasFlip))
		return (aScreenMode == 1);
	else
		return EFalse;
}

TVwsViewIdAndMessage CSMan2FCCellAreaView::ViewScreenDeviceChangedL()
{
	return TVwsViewIdAndMessage(TVwsViewId(KUidSMan2App, CurrentViewUid()));
}

void CSMan2FCCellAreaView::ViewDeactivated()
{
	activateCount--;
	if (activateCount <= 0)
	{
		if (!(configData->iCellAreaAlwaysRun))
			static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iCellAreaView->StopAsynchRequests();
		MakeVisible(EFalse);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->RemoveFromStack(this);
	}
}

void CSMan2FCCellAreaView::ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
{
	if (activateCount <= 0)
	{
		doViewActivated();

		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iCellAreaView->StartAsynchRequests();		
		iNetworkInfoLabel->MakeVisible(EFalse);
		iNetworkInfoLabel->MakeVisible(ETrue);
		iSignalStrengthLabel->MakeVisible(EFalse);
		iSignalStrengthLabel->MakeVisible(ETrue);		
	}
}

TVwsViewId CSMan2FCCellAreaView::ViewId() const
{
	return TVwsViewId(KUidSMan2App, KUidFCCellAreaView);
}

TKeyResponse CSMan2FCCellAreaView::HandleKey(TInt iScanCode)
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

		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->LaunchPopupMenuL(R_FC_CELLAREA_MENU, 
			TPoint(iSignalStrengthLabel->Size().iWidth, iSignalStrengthLabel->Size().iHeight + 10 + iSignalStrengthLabel->Position().iY), EPopupTargetBottomRight);
		return EKeyWasConsumed;
	}
	return EKeyWasNotConsumed;
}

TKeyResponse CSMan2FCCellAreaView::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType == EEventKey)
		return HandleKey(aKeyEvent.iScanCode);
	else
		return EKeyWasNotConsumed;
}

CSMan2FCCellAreaView::~CSMan2FCCellAreaView()
{
}

void CSMan2FCCellAreaView::ConstructL(const TRect& aRect)
{
	CreateWindowL();
	SetExtent(aRect.iTl, aRect.Size());

	iTitleBar = new (ELeave) CEikMover;
	iTitleBar->SetExtent(TPoint(0, 0), TSize(aRect.Size().iWidth, 25));
	iTitleBar->SetContainerWindowL(*this);
	HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_FC_CELLAREATXT);
	iTitleBar->SetText(dataBuffer);
	controlsArray->AppendL(iTitleBar);

	TInt iHeight;
	if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iRunningPhoneModel == CSMan2AppUi::EPhoneModel_P800)
		iHeight = 144;
	else if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iRunningPhoneModel == CSMan2AppUi::EPhoneModel_P900)
		iHeight = 208;
	else
		iHeight = 144;

	TBuf<1> temp;
	temp.Copy(_L(""));
	iNetworkInfoLabel = new (ELeave) CEikLabel;
	iNetworkInfoLabel->SetTextL(temp);
	iNetworkInfoLabel->SetContainerWindowL(*this);
	iNetworkInfoLabel->SetExtent(TPoint(0, iTitleBar->Size().iHeight), TSize(Size().iWidth, TInt((iHeight - iTitleBar->Size().iHeight) * 0.7)));
	iNetworkInfoLabel->SetAlignment(EHCenterVCenter);
	controlsArray->AppendL(iNetworkInfoLabel);
	
	iSignalStrengthLabel = new (ELeave) CEikLabel;
	iSignalStrengthLabel->SetTextL(temp);
	iSignalStrengthLabel->SetContainerWindowL(*this);
	iSignalStrengthLabel->SetExtent(TPoint(0, iNetworkInfoLabel->Size().iHeight + iNetworkInfoLabel->Position().iY), 
		TSize(Size().iWidth, iHeight - 10 - iNetworkInfoLabel->Size().iHeight - iNetworkInfoLabel->Position().iY));
	iSignalStrengthLabel->SetAlignment(EHCenterVCenter);
	controlsArray->AppendL(iSignalStrengthLabel);

	viewId = CSMan2AppUi::EViewFCCellArea;
	iBJackIconPosition = TPoint(2, iSignalStrengthLabel->Position().iY + iSignalStrengthLabel->Size().iHeight + 2);
	InitBJackIcon();

	MakeVisible(EFalse);
	ActivateL();
}

void CSMan2FCCellAreaView::HandleControlEventL(CCoeControl* /*aControl*/, TCoeEvent /*aEventType*/)
{
}
