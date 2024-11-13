#include <apgwgnam.h>		// CApaWindowGroupName
#include <eikcmbut.h>		// CEikBitmapButton
#include <eikclbd.h>		// CEikColumnListBoxData
#include <eiktxlbm.h>		// CEikTextListBoxModel
#include <apgtask.h>		// TApaTaskList
#include <apgcli.h>			// RApaLsSession
#include <hal.h>			// hardware abstraction layer
#include <hal_data.h>		// memory stats
#include <gulutil.h>		// TextUtils::ColumnText
#include <eikclb.h>			// CEikColumnListBox
#include <apgicnfl.h>		// CApaMaskedBitmap
#include <eikmenub.h>		// CEikMenuBar
#include <eikimage.h>		// CEikImage

#include "systemicon.h"
#include "dialogs.h"
#include "config.h"
#include "refreshtasklist.h"
#include "viewbase.h"

#if !defined(__TASKLISTVIEW_H__)
#define __TASKLISTVIEW_H__

// Actions to perform in refreshTaskList()
#define TODO_NOTHING 0
#define TODO_FOCUS 1
#define TODO_TERMINATE 2
#define TODO_SNAPSHOT 3
#define TODO_FLUSH 4
#define TODO_REFRESH 5

/*************************************************************
*
* Default view. This is also the task list view
*
**************************************************************/

class CSMan2TaskListView : public CViewBase, public MCoeControlObserver, public MCoeView
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void SetZoomLevel(TInt zoomLevel);

	CSMan2TaskListView(CConfig *cData);
	~CSMan2TaskListView();
	void ConstructL(const TRect& aRect);
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void refreshTaskList(TInt intTodo);
	int saveSnapShot(void);
	int saveConfig(void);
	void GentleEndTask(TPtrC iTaskCaption, TUid iTaskUid);
	void ForceEndTask(TPtrC iTaskCaption, TUid iTaskUid);
	void ToggleHotKey(void);
	void SetIgnoreHarakiriState();
	
	TInt32 iCapturedHotkeyShortHandle, iCapturedHotkeyLongHandle;
	CEikColumnListBox *cTaskListBox;
	CArrayFixSeg<TApaAppCaption> *iSnapshotTaskCaption;
	CArrayFixFlat<TUid> *iSnapshotTaskUid;
 	TFileName snapshotFileName;
	// Buttons at the bottom
	CEikBitmapButton *flushButton, *endTaskButton, *refreshButton, *snapshotButton,
					 *compHeapButton;
	// Memory status
	CEikProgressInfo *iMemoryBar;

private:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	int loadSnapShot(void);
	void InitToolBarButton(CEikBitmapButton **aButton, TPoint aPosition, TFileName *bitmapFile, TInt aBitmapId, TInt aBitmapMaskId);
	TInt oldItem;
	CRefreshTaskList *taskRefresher;
	CArrayFixFlat<TUid> *iWindowUidList;
};

#endif