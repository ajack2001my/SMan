#if !defined(__CELLAREAVIEW_H__)
#define __CELLAREAVIEW_H__

#include <etelmm.h>
#include "systemicon.h"
#include "dialogs.h"
#include "viewbase.h"
#include "dbhelper.h"

class CSMan2SignalStrengthRefresh;
class CSMan2CellAreaView;

/*************************************************************
*
* Network info notification handler
*
**************************************************************/

class CSMan2NetworkInfoRefresh : public CActive
{
public:
	CSMan2NetworkInfoRefresh(RMobilePhone *aPhoneServer, CSMan2CellAreaView *aView);
	~CSMan2NetworkInfoRefresh();
	void RunL();
	void DoCancel();
	void UpdateNetworkInfo();
	void ActivateNotification();
	void DeactivateNotification();
	void InfoChanged();
	
	enum EOperationMode
	{
		EGettingFirstReading,
		EGettingSubsequentReading
	};	
	
	typedef TBuf<50> TGroupName;
	typedef TBuf<50> TCellDescription;
	struct sCellInfo
	{
		TUint iCellId;
		TGroupName iGroupName;
		TCellDescription iCellDesc;
		TTime iLogonTime;
		TBuf<4> iCountryCode;
		TBuf<8> iNetworkId;
	};

private:
	RMobilePhone::TMobilePhoneNetworkInfoV1 iNetworkInfo;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg *iNetworkInfoPackage;
	RMobilePhone::TMobilePhoneLocationAreaV1 iCellInfo;
	TUint iOldCurrentCellId;
	sCellInfo iPastCellId[3];
	TInt iCurrentOperation;
	RMobilePhone *iLocalPhoneServer;
	CSMan2CellAreaView *iLocalView;
	TFileName iGroupNamesStore;
};

/*************************************************************
*
* Cell area view
*
**************************************************************/

class CSMan2CellAreaView : public MCoeView, public CViewBase, public MMdaAudioPlayerCallback
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	CSMan2CellAreaView(CConfig *cData);
	~CSMan2CellAreaView();
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void ConstructL(const TRect& aRect);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void AlwaysRunFlagChanged(TBool aViewIsInFocus);
	TBool IsStorageOK();
	TBool ReInitStorage();
	void ToggleRecording();
	TBool IsRecording();
	void DoEdit();
	void DoCompress();
	void DoUpdateStats();
	void ForceCellInfoUpdate();
	TBool DoChangeEventDelay();

	TBool iProcessEvents;

private:
	friend class CSMan2NetworkInfoRefresh;
	friend class CSMan2SignalStrengthRefresh;
	friend class CSMan2FCCellAreaView;

	void UpdateSignalStrength(TInt32 aSignalStrength);
	void UpdateNetworkInfo(CSMan2NetworkInfoRefresh::sCellInfo* aCellId, TDesC *aShortName, TDesC *aLongName);
	void StopAsynchRequests();
	void StartAsynchRequests();
	void InitRClasses();
	void DeInitRClasses();
	void HandleSwitchToFlightMode();
	void DeInitDBHelper();
	void InitDBHelper();
	void DoEventPlayAudio(HBufC *iEventData);
	void DoEventVibrate();
	void DoEvents(TUint aGroupId, TUint aEventFiringType);
	
	// Implements MMdaAudioPlayerCallback
	void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	void MapcPlayComplete(TInt aError);
	
	#define NUM_EVENT_FIRE_TIME_HISTORY 10
	struct SEventFireTimeHistory
	{
		TUint32 iEventId;
		TTime iFireTime;
	};
	CArrayFixFlat<SEventFireTimeHistory> *iEventFireTimeHistory;
	TInt iEventFireTimeHistoryCurrentIndex;
	
	CArrayFixFlat<TUint> *iEventTriggerIds;
	CMdaAudioPlayerUtility *iAudioPlayer;
	// Interface to our ECOM plugin for doing vibration
	CSManEComInterfaceDefinition *iSManECom;
	CEikLabel *iNetworkInfoLabel;
	CEikLabel *iSignalStrengthLabel;
	CSMan2NetworkInfoRefresh *iNetworkInfoRefresher;
	CSMan2SignalStrengthRefresh *iSignalStrengthRefresher;	
	TBool iRClassesInitialized;
	RTelServer iTelServer; 
	RTelServer::TPhoneInfo iPhoneInfo;
	RMobilePhone iMobilePhone;
	CDBHelper *iDBHelper;
	TBool iIsRecording;
	TUint iCurrentAutoRecordingGroupId, iCurrentAutoRecordingGroupPriority;
	TBuf<4> iCurrentCountryCode;
	TBuf<8> iCurrentNetworkId;
	HBufC *iShortNameText, *iLongNameText, *iCountryCodeText, *iNetworkIdText,
			*iCellText, *iCellIdText, *iGroupText, *iMultiGroupText, *iSignalStrengthText,
			*iEventDataBuffer, *iTSYName;
	HBufC *iBuffer, *iBuffer2, *iTempTime;
	TBool iOldGroupIdNotInitialized;
	TUint iOldGroupId;
};

/*************************************************************
*
* Signal strength notification handler
*
**************************************************************/

class CSMan2SignalStrengthRefresh : public CActive
{
public:
	CSMan2SignalStrengthRefresh(RMobilePhone *aPhoneServer, CSMan2CellAreaView *aView);
	~CSMan2SignalStrengthRefresh();
	void RunL();
	void DoCancel();
	void UpdateSignalStrength();
	void ActivateNotification();
	void DeactivateNotification();
	
	enum EOperationMode
	{
		EGettingFirstReading,
		EGettingSubsequentReading
	};	

private:
	// Allow this class to be updated by the view class. Signal strength doesn't seem 
	// to change (to zero) after switching phone from normal mode to flight mode
	// The view class is called by the network info class to update this class.
	// See "HandleSwitchToFlightMode()" member in view class
	friend class CSMan2CellAreaView;

	TInt32 iSignalStrengthdB;
	TInt8 iSignalStrengthBar;
	TInt iCurrentOperation;
	RMobilePhone *iLocalPhoneServer;
	CSMan2CellAreaView *iLocalView;
};

#endif