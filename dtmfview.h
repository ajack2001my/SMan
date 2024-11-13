#include "systemicon.h"
#include "viewbase.h"

#if !defined(__DTMFVIEW_H__)
#define __DTMFVIEW_H__

/*************************************************************
*
* DTMF Dialer View
*
**************************************************************/

#define WAV_BUFFER_SIZE 1600
#define WAV_HEADER_SIZE 58

class CSMan2DTMFDialerView : public MCoeControlObserver, public MCoeView, public MMdaAudioPlayerCallback, public CViewBase
{		
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	CSMan2DTMFDialerView(CConfig *cData);
	~CSMan2DTMFDialerView();
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void ConstructL(const TRect& aRect);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);	

	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);

	void InitDialerState(void);
	void StopAndResetDialer(void);

	CEikGlobalTextEditor* iDTMFNumbers;
	CEikCommandButton* iDTMFButtons[12];
	CEikBitmapButton *iDTMFDialButton;
	
	void DoEditCommands(TInt aCommand);

private:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	HBufC8 *iWAVData[12];
	HBufC *iPhoneNumber;
	TInt iPhoneNumberPosition;
	TInt iSilenceCounter;	// used to play 'p'. plays tone 0 at volume 0 for 2 seconds
	TPtr8 iWAVDataDescriptor;

	void DoDialPad(TInt aNumber);
	void DoDialNumbers(void);
	void GenerateWAVData(void);
	void DeallocateWAVData(void);
	void GetNextValidDigit(void);
	
	TBool iWAVDataReady;
	CMdaAudioPlayerUtility *iAudioPlayer;
};

#endif