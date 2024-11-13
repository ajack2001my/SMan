#include <eikgted.h>		// CEikGlobalTextEditor
#include <eikimage.h>		// CEikImage
#include <eikhkeyt.h>		// CEikHotKeyTable
#include <eikmover.h>		// CEikMover

#include "systemicon.h"
#include "dialogs.h"
#include "viewbase.h"

#if !defined(__FC_BJACKVIEW_H__)
#define __FC_BJACKVIEW_H__

/*************************************************************
*
* Flip closed view
*
**************************************************************/

class CSMan2FCBJackView : public CViewBase, public MCoeControlObserver, public MCoeView
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);
public:
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	
public:
	CSMan2FCBJackView(CConfig *cData);
	~CSMan2FCBJackView();
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);
	void ConstructL(const TRect& aRect);
	CEikGlobalTextEditor *bluejackLog;
	
private:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	TKeyResponse HandleKey(TInt iScanCode);
	CEikMover *iTitleBar;
};

#endif