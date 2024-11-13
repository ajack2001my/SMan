#include <coeview.h>
#include <eikappui.h>
#include <eikhlbx.h>		// CHierchicalListBox
#include <eikhlbm.h>		// CHierchicalListBoxModel and CHierListItem
#include <eikhlbi.h>		// CHierarchicalListItemDrawer
#include <agmcallb.h>
#include <eikcal.h>			// calendar controls

#include "uid.h"
#include "systemicon.h"
#include "viewbase.h"

#if !defined(__AGENDAVIEW_H__)
#define __AGENDAVIEW_H__

class CHierModel;
class CHierItemDrawer;
class CAgendaFileReader;

class CSMan2AgendaView : public CViewBase, public MCoeView, public MEikCalendarObserver
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	CSMan2AgendaView(CConfig *cData);
	~CSMan2AgendaView();
	void ConstructL(const TRect& aRect);
	void SetZoomLevel(TInt zoomLevel);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void ReadAgenda(TBool dateIsSet);
	void FixSynchTimeZone();
	void ViewOtherDate();
	TBool SetMaxTodoPriority();
	//void DoAutoRefresh();
		
private:
	TBool IsSameDate(TTime *aDate1, TTime *aDate2);
	void FetchEntry();
	void SetDateFromCalendarAndDrawNow(const TTime& aDate);
	void GetMinimumAndMaximumAndInitialDatesForCalendarL(TTime& aMinimumDate, TTime& aMaximumDate, TTime& aInitialDate) const;
	CAgendaFileReader* reader;
	CHierModel* hierModel;
	CHierItemDrawer* hierDrawer;
	CEikHierarchicalListBox* cHierListBox;
	CHierModel* internalHierModel;
};

class CHListItem : public CHierListItem
{
public:
	CHListItem(TInt aFlags);
	TInt iDayListIndex;
	TInt iFullHierListModelIndex;
	TBool iComplete;
};

class CHierModel : public CHierarchicalListBoxModel
{
public:
	void ExpandItemL(TInt aItemIndex);
	CHierModel* fullHierModel;
};

class CHierItemDrawer : public CHierarchicalListItemDrawer
{
public:
	CHierItemDrawer(CHierModel* aModel, const CFont* aFont, CArrayPtrFlat<CGulIcon>* aBitmaps);
	CGulIcon* ItemIcon(TInt aItemIndex) const;
	CHierModel* fullHierModel;
};

/******************************************************
* CAgendaFileReader
*
* The main class for the example. CAgendaFileReader 
* contains all the functions for reading the Agenda document
*******************************************************/

class CAgendaFileReader : public CBase, public MAgnModelStateCallBack, public MAgnProgressCallBack
{
public:
	CAgendaFileReader(CConfig *cData);
	~CAgendaFileReader();
	void ExecuteL(CHierarchicalListBoxModel *hlModel);
	void ReadDayListL();
	void ReadEntriesL();
	void FindNextEntryL();
	TBool OpenAgendaFile();
	void CloseAgendaFile();
	void UpdateInstance(CAgnEntry *aAgnEntry);
	
	void StateCallBack(CAgnEntryModel::TState aState); // inherited from MAgnModelStateCallBack
	void Progress(TInt aPercentageCompleted); // inherited from MAgnProgressCallBack
	void Completed(TInt aError) ; // inherited from MAgnProgressCallBack	

	CAgnEntry* GetOneEntry(TInt aIndex);
	TTime iDay;
	TInt todoRootOffset;
	TBool activeSchedulerStarted;
private:
	RAgendaServ *iAgnServ;
	CConfig *configData;
	CHierarchicalListBoxModel *hierListModel;
	CAgnModel* agnModel;
	CAgnDayList<TAgnInstanceId>* iDayList;
	TInt iCount;
	TBuf<50> iText;
};

#endif