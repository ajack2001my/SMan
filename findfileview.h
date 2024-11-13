#include "systemicon.h"
#include "dialogs.h"
#include "viewbase.h"

#if !defined(__FINDFILEVIEW_H__)
#define __FINDFILEVIEW_H__

class CSMan2FindFileView;

/*************************************************************
*
* File scanner - simply because Symbian doesn't support multiple
* inheritence (which is a good thing, I'm told :))
*
**************************************************************/

class CSMan2FindFileScanner : public CActive
{
public:
	CSMan2FindFileScanner(CSMan2FindFileView *aFindFileView);
	
	void DoCancel();
	void RunL();
	void ScanDir(TInt aNextOperation);
	void InitDateCriteria();
	
	enum eCurrentOperation
	{
		ScanningFolders,
		ScanningFiles
	};
	TInt iCurrentOperation;

private:
	TTime iTargetDate;
	TInt iDateCriteriaType;	// 0 = ignore; 1 = today, yesterday; 2 = other
	TEntryArray iEntriesFiltered;
	TBool iSearchSpecNoExt;
	CSMan2FindFileView *iLocalFindFileView;
};

/*************************************************************
*
* Find file view
*
**************************************************************/

class CSMan2FindFileView : public MCoeControlObserver, public MCoeView, public CViewBase
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	CSMan2FindFileView(CConfig *cData);
	~CSMan2FindFileView();
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void SetZoomLevel(TInt zoomLevel);
	void ConstructL(const TRect& aRect);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);	
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

	CEikTextListBox *iResultList;

	void StartFileFind();
	void StopFileFind();
	void SetCriteria(TFileName *aHighlightedFolder);
	
private:
	friend class CSMan2FindFileScanner;

	void SetToolbarButtonState(TBool aFindFileIsRunning);
	void ScanningEnded();
	void GotoFile();
	
	RDir *iDirScanner;
	TBool iErrorInScan;
	TInt iModifiedDateOption;
	TTime iFromTime, iToTime;
	TFileName iSearchWildCard;
	CSMan2FindFileScanner *iFindFileScanner;
	
	CDesCArray *iFoldersToScan;
	TInt iAnchorFoldersToScan, iIndexToFoldersToScan;
};

#endif