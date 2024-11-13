#include "systemicon.h"
#include "dialogs.h"
#include "viewbase.h"

#if !defined(__SYSINFOVIEW_H__)
#define __SYSINFOVIEW_H__


/*************************************************************
*
* Sysinfo view
*
**************************************************************/

class CSMan2SysInfoView : public MCoeControlObserver, public MCoeView, public CViewBase
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	CSMan2SysInfoView(CConfig *cData);
	~CSMan2SysInfoView();
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void SetZoomLevel(TInt zoomLevel);
	void ConstructL(const TRect& aRect);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	CEikColumnListBox *cSysInfoListBox;
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);	
	void refreshSysInfo();

private:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void showDetail();
};

#endif