#if !defined(__BJACKVIEW_H__)
#define __BJACKVIEW_H__

#include <es_sock.h>
#include <bt_sock.h>
#include <obex.h>
#include <btsdp.h>
#include <btextnotifiers.h>
#include <eikgted.h>		// CEikGlobalTextEditor
#include <eikdialg.h>		// CEikDialog
#include <txtrich.h>		// CRichText
#include <eikappui.h>		// CEikAppUi
#include <eikapp.h>			// CEikApplication
#include <QBTselectdlg.h>	// CQBTUISelectDialog
#include <eiklbv.h>			// CSelectionIndexArray
#include <eikimage.h>		// CEikImage
#include <mdaaudiosampleplayer.h>	// MMdaAudioPlayerCallback
#include "EComInterfaceDefinition.h"

#include "systemicon.h"
#include "sman.hrh"
#include "viewbase.h"

#define MAX_VCARD_FULLNAME 20	// Maximum length of N: field in vcard
#define MAX_LOG_LINES 13			// Stricly speaking, there's no such thing as lines
#define MAX_LOG_LINE_SIZE 80		// Everything is one buffer the size of these 2 multiplied
#define MAX_VCARD_SIZE 60		// Max file size for the vcard. Enuf for 20 char name
#define MAX_VCARD 1				// Max number of vcards (for bluejacking)
#define MAX_BTDEV_ADDR 32		// Max entries in bluetooth device cache
#define OBEX_RETRIES 5			// Obex retries. Usually for -6004 hci error = page timeout
#define STACK_COOLOFF_COUNTDOWN 5	// Stack cool off delay (seconds) after bluejacking each device
#define TIMER_GRANULARITY 800000	// Granularity of CTimer::After() calls

class CBluebeamLogDialog;
class CScanner;

/*************************************************************
*
* Bluejack view
*
*************************************************************/
class CSMan2BJackView : public CViewBase, public MCoeControlObserver, public MCoeView
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);
public:
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);	
	void SetZoomLevel(TInt zoomLevel);
	
public:
	CSMan2BJackView(CConfig *cData);
	~CSMan2BJackView();
	void ConstructL(const TRect& aRect);

	CScanner *oScanner;
	CEikGlobalTextEditor *bluejackLog;

private:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
};

/*************************************************************
*
* Generic logging class
*
* It needs these things:
* - a pointer to the dialog box where it will display the log
* - the control id of the control in the dialog box that will display the log
* - a pointer to a buffer to hold the log (this is displayed in the dialog box)
* - a pointer to a boolean value to indicate whether to log to disk
*
* 3 classes using logging class
* - scanner 
* - bluebeam
* - bluejackone
* 
* scanner 
* - maintains its own log buffer
* - dialog should point to type CBluejackLogDialog
* - dialog owned by tasklistview
*
* bluebeam 
* - filelist view maintains the buffer.
* - dialog should point to type CBluebeamLogDialog
* - dialog owned by self (bluebeam)
*
* bluejackone
* - uses scanner's log buffer
* - dialog should point to type CBluejackLogDialog
* - dialog owned by tasklistview
*
* In all instances, the logging class has a pointer in it called logDialog. this pointer
* is initialised to NULL when the 3 classes above start their work. the pointer is set
* to the appropriate instance of the log dialog only when the dialog is created (in AppUi
* or in CBluebeam when doing a bluebeam - as opposed to bluejackone)
*
* Update: 3rd August 2003
* Scanner and bluejack one now have their own dedicated view. This class modified
* to do logging to a dialog or to a global text editor
*
**************************************************************/

class CLogger
{
public:
	enum enumLogType
	{
		logBluejack = 0x01,
		logOBEX = 0x02,
		logScanning = 0x04,
		logDecays = 0x08,
		logAll = 0xff
	};
	enum enumLogTarget
	{
		logToDialog,
		logToTextEditor
	};
	
	CLogger(CConfig *cData);
	~CLogger();
	// Write to log. numExtraLines = number of extra line breaks to insert into log on disk
	void updateLog(const TDesC *dialogLine, const TInt numExtraLines, TUint logType);
	// Save log to disk. This method appends *buffer to log file
	TBool saveLog(TBool overwrite, TDesC *buffer);
	// Filename of the log
	TFileName logFilename;
	// Used for logging the results to the log dialog. The log buffer is provided by 
	// someone else. The logging class does not own it!
	TDes *logBuffer;
	// Pointer to a log dialog. Using polymorphism, this can either point to
	// CBluebeamLogDialog or CBluejackLogDialog. We are only interested in 2 base
	// class members - ExecuteLD() and Control() and since we don't override any of them, it's ok
	CEikDialog *logDialog;
	// Logging can happen to this control instead of *logDialog. Which to log to depends on
	// the TInt logTarget member. See enumLogTarget for possible values.
	CEikGlobalTextEditor *logTextBox;
	// Log to logDialog or logTextBox?
	TInt logTarget;
	// Auto save the log?
	TBool *localAutoSave;
	TUint controlId;
private:
	CConfig *configData;	
};

/*************************************************************
*
* SDP OBEX searcher class
*
**************************************************************/

class CFindOBEX : public MSdpAgentNotifier, public MSdpAttributeValueVisitor
{
public:
	CFindOBEX();
	void start(TRequestStatus *theStatus, const TBTDevAddr *theAddress);
	TInt RFCOMMPort;
	void releaseObjects(void);

// Specific implementation for MSdpAttributeValueVisitor
public:
	void VisitAttributeValueL(CSdpAttrValue& aValue, TSdpElementType aType);
	void StartListL(CSdpAttrValueList& aList);    
	void EndListL();
    
// Specific implementation for MsdpAgentNotifier
public:
	void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
	void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue);
	void AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError);
		
protected:
	void searchDone(TUint errorNumber);
	TUint foundOBEX;
    CSdpSearchPattern *iSdpSearchPattern;
	CSdpAttrIdMatchList *iMatchList;
	TRequestStatus *localStatus;
	CSdpAgent *iAgent;
};

/*************************************************************
*
* Generic timeout class
*
**************************************************************/

class CTimeout : CTimer
{
public:
	CTimeout();
	void RunL(void);
	void startTimer(CScanner *theScanObj, TInt obexTimeout);
	void stopTimer(void);

protected:
	TInt localObexTimeout;
	TTime timeoutStart;
	CScanner *scanObj;
};

/*************************************************************
*
* Bluetooth scanner class. This does the grunt of the work
* for bluesweeping
*
**************************************************************/

class CScanner : public CTimer, public MMdaAudioPlayerCallback
{
public:
	CScanner(TFileName *appPath, CConfig *cData);
	~CScanner();
	
	enum scanStatus
	{
		isRest,					// Waiting for BT stack to cool-off
		isShutdown,				// Scanner is shutting down
		isOBEXConnect,			// Connecting to OBEX on remote device
		isOBEXDisconnect,		// Disconnecting
		isOBEXSearch,			// Searching for OBEX
		isOBEXSend,				// Sending file
		isIdle,					// Scanner is idle
		isInquiring				// Scanner is searching for devices
	};
	
	// Current status of the CScanner object. Refer to enum scanStatus for values
	TUint scannerStatus;

	// Although the array structure below is more memory intensive, it offers better
	// memory handling because it allocates everything at run-time.

	// Array of vcard filenames in the current directory
	TName vCardNames[MAX_VCARD];
	
	enum btDeviceStatus
	{
		statusNoDevice,				// No device
		statusValidNotBluejacked,	// Valid device, not bluejacked
		statusValidBluejacked,		// Valid device, bluejacked
		statusCached				// Cached device
	};
	
	// Array of bluetooth device addresses that were successfully discovered
	struct 
	{
		TBTDevAddr btDevAddr;
		THostName btNames;
		TTime lastBluejackedTime;
		// Values in isBluejacked. Refer to enum btDeviceStatus for values
		TInt isBluejacked;
		// This port is only valid if isBluejacked is set to either statusValidNotBluejacked
		// or statusValidBluejacked
		TInt obexPort;
	} btDevice[MAX_BTDEV_ADDR];

	// Start scanning
	void startScan(void);
	
	// Stop scanning
	void stopScan(void);
	
	// Current index in btDevice of the device we're working on
	TInt currentDeviceIndex;

	// Handle timeout
	void timeoutAbort(void);
	
	// Path to our app
	TFileName appPath;
	
	// Log stuff
	//HBufC *logBuffer;
	//TPtr *iLogPointer;
	TBuf<(MAX_LOG_LINES + 1) * MAX_LOG_LINE_SIZE> logBuffer;
	
	enum scannerBehaviour
	{
		behaviourProximitySweep,	// Do proximity sweep
		behaviourBluejackOne		// Bluejack one device only
	};
	// Behaviour of scanner object. Refer to enum scannerBehaviour for values
	TInt iScannerBehaviour;

	// Class used for logging
	CLogger *logHelper;

	void SetToolbarButtonState();

	// Interface to our ECOM plugin for doing vibration
	CSManEComInterfaceDefinition *iSManECom;
	
	// This struct is cheaper than maintaining an array of CBTDeviceArray
	struct SBluetoothDevice
	{
		TBTDevAddr iAddress;
		//TBTDeviceName iName;
		TBuf<KMaxDeviceName> iName;
	};
	typedef SBluetoothDevice TBTDevice;
	// Array of devices that should not be bluejacked
	CArrayFixFlat<TBTDevice> *iBTExclusionList;

	TFileName iBTExclusionListFileName;
	TBool LoadBTExclusionList();
	TBool SaveBTExclusionList();
	void EditBTExclusionList();
	
private:
	TBool IsFriendDevice(TBTDevice *iBTDevice);
	void LogFriendExclusion(TDes *aDeviceName, TBTDevAddr *aDeviceAddr);
	TBool initFile(void);

	TInt selectDevice(void);
	TBool forcedFlagTrue;

	// Specific implementation for CActive
	void DoCancel();
	void RunL();
	
	// Scan the next device
	void bluejackNextDevice(void);

	// BT stack cool off
	void rest(void);
	
	// Various objects used to scan for BT devices
	TBool socketServOpen;
	TProtocolDesc protoDesc;
	RHostResolver hostResolver;
	TBool hostResolverOpen;
	TInquirySockAddr sockAddr;
	TNameEntry nameEntry;
	void doRealScan(void);
	TUint resolveAction;
	TBool insertAndUpdateBTList(TBTDevAddr btDevAddr, TBTDeviceName btDevName);
	TBool getNextBTDeviceIndex(void);
	
	// Flintstones
	TUint obexRetries;

	// Log related stuff
	TBool *localShowSysMsg;
	void logWithLookup(TPtrC theText, TUint logType, TBool aIsFriend);

	// Sweep only once?
	TBool *localProxSweepOnce;

	// Pause if no devices found
	TInt *localNoDevicePause;

	TFileName *localAudioFile;

	// Vibrate if bluejacked?
	TBool *localVibrate;

	// Number of times no devices were found before we do a stack cool off
	TInt *localNoDeviceAttempts;
	TInt currentNoDeviceAttempts;	

	TInt currentNoDevicePause;
	TTime startRestTime;
	TBool firstRest;

	void playAudio(void);
	CMdaAudioPlayerUtility *audioPlayerUtil;
public:
	// Only one instance of these objects exist. They operate on each and every address
	// in the btDevAddr via a loop. Each call is asynchronously processed.
	RSocketServ socketServ;
	CFindOBEX obexSearcher;
	CObexClient *obexClient;
	TObexBluetoothProtocolInfo obexBTProtoInfo;
	CObexFileObject *obexFile;
	CTimeout timeoutCounter;

// MMdaAudioPlayerCallback interfaces
public:			
	virtual void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
	virtual void MapcPlayComplete(TInt aError);
	
private:
	CConfig *configData;	
};

/*************************************************************
*
* Class used for blue beaming
*
* Note: This class was originally written to do bluetooth 
* beaming ONLY. It has now been adapted to do IrDA. Don't get
* confused by the class' name. Rightfully, it should be
* PurpleBeam. :)
*
**************************************************************/

class CBlueBeam : public CTimer
{
public:
	CBlueBeam(CConfig *cData);
	~CBlueBeam();
	void startBeam(void);

	enum beamStatus
	{
		isOBEXNextFile,
		isOBEXConnect,			// Connecting to OBEX on remote device
		isOBEXDisconnect,		// Disconnecting
		isOBEXSearch,			// Searching for OBEX
		isOBEXSend,				// Sending file
		isIdle,					// Idle
		isStarting				// Is starting to do work...
	};
	
	// Current status of this object. Refer to enum beamStatus for values
	TUint beamerStatus;

	// Log stuff
	CLogger *logHelper;
	TDes *logBuffer;
	CBluebeamLogDialog *bluebeamLogDialog;
	// Number of files selected
	TInt selectedFiles;
	TFileName currentFileName;
	
	enum protocolToUse
	{
		protocolBluetooth,
		protocolIrDA
	};
	
	// Protocol to use for beaming files. Refer to enum protocolToUse for values
	TInt beamerProtocol;
	
	void stopBeam();
protected:
	void DoCancel();
	CListBoxView::CSelectionIndexArray* selArray;

	// Uses standard UIQ BT device dialog
	TInt selectDevice(void);
	// Do the actual OBEX work
	void doBeam(void);
	void RunL();
	
	TBTDevAddr devAddress;
	TBTDeviceName devName;
	TBTDeviceClass devClass;
	
	void logWithName(const TDesC *buffer);
	
	CObexClient *obexClient;
	CObexFileObject *obexFile;
	CFindOBEX obexSearcher;	
	TUint obexRetries;
	TObexBluetoothProtocolInfo obexBTProtoInfo;
	TObexIrProtocolInfo obexIRProtoInfo;
	TNameEntry nameEntry;	
	void sendOBEXFile(void);

	// For the logger class. Bluebeaming should not auto log to disk.
	TBool fakeAutoSaveFlag;
};

#endif