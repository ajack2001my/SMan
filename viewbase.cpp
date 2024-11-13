#include "viewbase.h"
#include "sman.h"
#include <sman.rsg>

/*
These are the view server panics. Couldn't find them in the SDK :(

enum TVwsServerPanic
{
    EVwsServerCreate=1,
    EVwsServerStart,
    EVwsBadRequest,
    EVwsBadDescriptor,
    EVwsMainSchedulerError,
    EVwsViewNotFound,
    EVwsInvalidViewUid,
    EVwsCustomMessageNotCollected,
    EVwsViewActive,
    EVwsViewEventRequestAlreadyPending,
    EVwsViewEventTimeOut
}
*/

CViewBase::CViewBase(CConfig *cData)
{
	configData = cData;	
	activateCount = 0;
	appPathNoExt.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName());
	appPathNoExt.SetLength(appPathNoExt.Length() - 3);
	controlsArray = new (ELeave) CArrayPtrSeg<CCoeControl>(1);
	firstTime = ETrue;
	font = NULL;	
}

CViewBase::~CViewBase()
{
	controlsArray->ResetAndDestroy();
	delete controlsArray;
}

TUid CViewBase::CurrentViewUid()
{
	// By right, this should be a if check comparing against 1, not 0. The only reason
	// this works here is because the screen mode has already switched by the time this
	// function is called
	if (iCoeEnv->ScreenDevice()->CurrentScreenMode() == 0)
		return static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iCurrentFOViewUid;
	else
		return static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iCurrentFCViewUid;
}

void CViewBase::doViewActivated()
{
	if (activateCount <= 0)
	{
		TPixelsAndRotation sizeAndRotation;
		TRect rect;
		
		CEikonEnv::Static()->ScreenDevice()->GetDefaultScreenSizeAndRotation(sizeAndRotation);
		CEikonEnv::Static()->ScreenDevice()->SetScreenSizeAndRotation(sizeAndRotation);
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->switchView(viewId);
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->viewId = viewId;
		MakeVisible(ETrue);
		static_cast<CEikAppUi*>(CEikonEnv::Static()->AppUi())->AddToStackL(this);
		if (iCoeEnv->ScreenDevice()->CurrentScreenMode() == 1)
		{
			rect.iTl = TPoint(0, 0);
			rect.iBr = TPoint((CEikonEnv::Static()->ScreenDevice())->SizeInPixels().iWidth, (CEikonEnv::Static()->ScreenDevice())->SizeInPixels().iHeight);
		}
		else if (iCoeEnv->ScreenDevice()->CurrentScreenMode() == 0)
		{
			rect.iTl = static_cast<CQikAppUi*>(CEikonEnv::Static()->AppUi())->ClientRect().iTl;
			rect.iBr = static_cast<CQikAppUi*>(CEikonEnv::Static()->AppUi())->ClientRect().iBr;
		}
		SetRect(rect);
		activateCount++;
		if (static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView)
			DrawSystemIcon(CSMan2AppUi::EIconBluejack, static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iBJackView->oScanner->IsActive());
		DrawSystemIcon(CSMan2AppUi::EIconNetworkInfoRecording, static_cast<CSMan2AppUi*>(CEikonEnv::Static()->AppUi())->iCellAreaView->IsRecording());
	}
}

void CViewBase::DrawSystemIcon(TInt aIconId, TBool iDoDraw)
{
	switch (aIconId)
	{
		case CSMan2AppUi::EIconBluejack :
			{
				iBJackIconLabel[iBJackIconLabelCurrentImage]->MakeVisible(EFalse);
				if (iDoDraw)
				{
					iBJackIconLabelCurrentImage++;
					if (iBJackIconLabelCurrentImage >= NUM_BJACK_ICONS)
						iBJackIconLabelCurrentImage = 0;
					iBJackIconLabel[iBJackIconLabelCurrentImage]->MakeVisible(ETrue);
				}
				else
				{
					for (int i = 0; i < NUM_BJACK_ICONS; i++)
						iBJackIconLabel[i]->MakeVisible(EFalse);
				}
				break;
			}
		case CSMan2AppUi::EIconNetworkInfoRecording :
			{
				iCellAreaIconLabel[iCellAreaIconLabelCurrentImage]->MakeVisible(EFalse);
				if (iDoDraw)
				{
					iCellAreaIconLabelCurrentImage++;
					if (iCellAreaIconLabelCurrentImage >= NUM_CELLAREA_ICONS)
						iCellAreaIconLabelCurrentImage = 0;
					iCellAreaIconLabel[iCellAreaIconLabelCurrentImage]->MakeVisible(ETrue);
				}
				else
				{
					for (int i = 0; i < NUM_CELLAREA_ICONS; i++)
						iCellAreaIconLabel[i]->MakeVisible(EFalse);
				}
				break;
			}
	}
}

void CViewBase::InitBJackIcon()
{
	// Bluejack icon.
	TFileName iBitmapFile;
	iBitmapFile.Copy(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName());

	for (int i = 0; i < NUM_BJACK_ICONS; i++)
	{
		iBJackIconLabel[i] = new (ELeave) CEikImage();
		iBJackIconLabel[i]->CreatePictureFromFileL(iBitmapFile, EMbmMyheaderBjackframe1 + i, EMbmMyheaderBjackframe1 + i);
		iBJackIconLabel[i]->SetContainerWindowL(*this);
		iBJackIconLabel[i]->SetExtent(iBJackIconPosition, TSize(16, 6));
		iBJackIconLabel[i]->MakeVisible(EFalse);
		controlsArray->AppendL(iBJackIconLabel[i]);
	}
	
	// Once the bjack icon's position is established, the other icons' positions are all
	// relatively easy to derive
	iCellAreaIconPosition = TPoint(iBJackIconLabel[0]->Size().iWidth + iBJackIconPosition.iX + 2, iBJackIconPosition.iY);
	InitCellAreaIcon();
}

void CViewBase::InitCellAreaIcon()
{
	TFileName iBitmapFile;
	iBitmapFile.Copy(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName());
	
	for (int i = 0; i < NUM_CELLAREA_ICONS; i++)
	{
		iCellAreaIconLabel[i] = new (ELeave) CEikImage();
		iCellAreaIconLabel[i]->CreatePictureFromFileL(iBitmapFile, EMbmMyHeaderCellAreaframe1 + i, EMbmMyHeaderCellAreaframe1 + i);
		iCellAreaIconLabel[i]->SetContainerWindowL(*this);
		iCellAreaIconLabel[i]->SetExtent(iCellAreaIconPosition, TSize(16, 6));
		iCellAreaIconLabel[i]->MakeVisible(EFalse);
		controlsArray->AppendL(iCellAreaIconLabel[i]);
	}
}

void CViewBase::releaseFont()
{
	if (font)
	{
		CEikonEnv::Static()->ScreenDevice()->ReleaseFont(font);
		font = NULL;
	}
}

void CViewBase::generateZoomFont(TInt zoomLevel)
{
	// Size = 750, 1000, 1250
	TZoomFactor zoomFactor;
	zoomFactor.SetZoomFactor(500 + (zoomLevel / 2));
	const CFont* normalFont = CEikonEnv::Static()->NormalFont();
	TFontSpec fontSpec = normalFont->FontSpecInTwips();
	if(!zoomFactor.GraphicsDeviceMap())
		zoomFactor.SetGraphicsDeviceMap(CEikonEnv::Static()->ScreenDevice());
	User::LeaveIfError(zoomFactor.GetNearestFontInTwips(font, fontSpec));
}

void CViewBase::Draw(const TRect& aRect) const
{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect);
}

int CViewBase::CountComponentControls() const
{
	return controlsArray->Count();
}

CCoeControl* CViewBase::ComponentControl(int aIndex) const
{
	if (aIndex < controlsArray->Count())
		return controlsArray->At(aIndex);
	else
		return NULL;
}
