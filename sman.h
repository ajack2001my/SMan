#include <qikapplication.h>		// CQikApp
#include <qikappui.h>			// CQikAppUi
#include <quartzkeys.h>			// EQuartzKey....Confirm, TwoWayDown, TwoWayUp
#include <qikzoomdlg.h>			// CQikZoomDialog
#include <QPhoneAppExternalInterface.h>	// KUidPhoneApp
#include <qikdocument.h>		// CQikDocument
#include <eikdll.h>				// StartExeL

//#define __DEBUG_BUILD__

#include "dbview.h"
#include "bjackview.h"
#include "dialogs.h"
#include "config.h"
#include "viewbase.h"
#include "tasklistview.h"
#include "filelistview.h"
#include "sysinfoview.h"
#include "agendaview.h"
#include "dtmfview.h"
#include "hexeditview.h"
#include "findfileview.h"
#include "cellareaview.h"
#include "fc_cellareaview.h"
#include "fc_bjackview.h"

#if !defined(__SMAN_H__)
#define __SMAN_H__

#define ZOOM_INCREMENT 500
// These 3 literals are used by the utilities to fix app ordering
_LIT(KAppConfigFileBackup, "C:\\SYSTEM\\APPS\\ALAUNCH\\Alaunch.bak");
_LIT(KAppConfigFile, "C:\\SYSTEM\\APPS\\ALAUNCH\\Alaunch.ini");
_LIT(KAppTaskName_P80X, "Applications");
_LIT(KAppTaskName_A92X, "Launcher");

static const TUint sCaptureKeyCodes[] = 
{
	0,						// Disable - unused. Place holder so indexing is correct.
	EKeyApplication1,		// Camera
	EKeyApplication0,		// Internet
	0						// Flip open - unused. Place holder so indexing is correct.
};
// This is the output keycode for the call to CaptureLongKey
// Hopefully, this key doesn't exist in the real keyboard! It shouldn't though....no idea
// what key maps to 0xffff :)
#define CAPTURE_LONG_HOTKEY_REPLACEMENT 0xffff

/*************************************************************
*
* Document class
*
**************************************************************/

class CSMan2Document : public CQikDocument
{
public:
	static CSMan2Document* NewL(CEikApplication& aApp);
	~CSMan2Document();
	CSMan2Document(CEikApplication& aApp);
	void ConstructL();
	CConfig configData;
	void saveConfig(void);
	TInt loadConfig(void);	
	TFileName configFileName;

private: // from CEikDocument
	CQikAppUi* CreateAppUiL();
	CVerInfo verData;
};

/*************************************************************
*
* App UI class
*
**************************************************************/

class CSMan2AppUi : public CQikAppUi
{	
public:
	enum EViewType
	{
		EViewTask,
		EViewFile,
		EViewBJack,
		EViewDB,
		EViewSysInfo,
		EViewAgenda,
		EViewDTMFDialer,
		EViewCellArea,
		EViewFCBJack,
		EViewHexEditor,
		EViewFindFile,
		EViewFCCellArea
	};

	enum EPhoneModels
	{
		EPhoneModel_P800 = 1,
		EPhoneModel_P900 = 2,
		EPhoneModel_P910 = 3,
		EPhoneModel_A920 = 4,
		EPhoneModel_A920_C = 5,
		EPhoneModel_A925 = 6,
		EPhoneModel_A925_C = 7,
		EPhoneModel_P30 = 8,
		EPhoneModel_Unknown = 99
	};
	
	enum EPhoneCapabilities
	{
		EPhone_HasIRDA = 1,
		EPhone_HasBluetooth,
		EPhone_HasFlip,
		EPhone_CanChangeFont,
		EPhone_CanAutoStartInD,
		EPhone_HasFixedWidthFont,
		EPhone_HasIRDACPL,
		EPhone_HasBluetoothCPL,
		EPhone_HasPhoneCPL,
		EPhone_HasAppListBug,
		EPhone_HasTimeSynchZoneBug,
		EPhone_HasNetworkInfoTSY,
		EPhone_CanFixAppOrder,
		EPhone_CanSetZoomFont,
		EPhone_CanAutoStart,
		EPhone_CanSetHotkey
	};
	
	enum EIconId
	{
		EIconBluejack = 1,
		EIconNetworkInfoRecording
	};
	
	CSMan2AppUi(CConfig *cData);
	void ConstructL();
	~CSMan2AppUi();
	void HandleForegroundEventL(TBool aForeground);
	void flushTasks(void);
	void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
	void switchView(TInt viewId);
	TErrorHandlerResponse HandleError(TInt aError, const SExtendedError& aExtErr, TDes& aErrorText, TDes& aContextText);
	void FindAndLaunchAppNoUid(const TDesC *appPath);
	TBool FindApp(TUid appUid, TApaAppInfo *aInfo);
	void SetAppPosition(TBool foreground);
	void focusPhoneApp();
	void UpdateSystemIcons(TInt aIconId, TBool iDoDraw);
	void doCancelMenu(void);
	void doProcessCommand(TInt aCommand);
	void DeInitTemporaryViews(TInt aCurrentViewId);
	void InitTemporaryViews(EViewType aViewIdToInit);
	TBool PhoneIsCapable(TUint iCapability);

	// When SMan fires up, it sets this variable to the appropriate value depending
	// on what phone it is running on.
	// This is used to control feature availability on different models
	// See the enum EPhoneModels for values.
	TUint iRunningPhoneModel;
	TInt viewId;
	TInt bjackIconLabelCurrentImage;
	TUid iCurrentFCViewUid, iCurrentFOViewUid;
	TBool iFCViewSwitchIsInternal;
	TFileName iZipFile;	// Contains name of sman's zip archive

	CSMan2TaskListView* iTaskView;
	CSMan2FileListView* iFileView;
	CSMan2FCBJackView* iFCBJackView;
	CSMan2BJackView* iBJackView;
	CSMan2SysInfoView* iSysInfoView;
	CSMan2DBView* iDBView;
	CSMan2AgendaView* iAgendaView;
	CSMan2DTMFDialerView* iDTMFDialerView;
	CSMan2HexEditorView* iHexEditorView;
	CSMan2FindFileView* iFindFileView;
	CSMan2CellAreaView* iCellAreaView;
	CSMan2FCCellAreaView* iFCCellAreaView;

private:
	void InfoPrintFromResource(TInt aResourceId);
	void DoShortCuts(TInt aShortcutOperation, TInt aScreenMode);
	void doFileMan(TInt op);
	void updateSnapShot(void);
	void sizeToText(TInt64 size, TDes* buffer);
	void launchControlPanel(TPtrC appletName);
	void launchApp(TUid appUid, TFileName docName, TBool putHistory);
	void SetZoom(TInt zoomLevel);
	TBool IsHexEditorEditingFile(TFileName *iFileName);
	TBool PromptAppFixOrder(TInt aPromptResourceId);
	void FixIfContextErrorIsFileName(TDes *iContextError, TFileName *iSManResourceFileName);

	TBool appInForeground;
	TRect iScreenRect;
	TUint oldScreenMode;
	CArrayFixFlat<TUid> *startUidList, *historyUidList;
	CConfig *configData;
	TInt iOldMenuBarYPos;
	TBool iIsProcessingShortcut;
	
	//struct SGlobalVars iZlibVars;
	
protected:
	void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);
	void HandleCommandL(TInt aCommand);
};

/*************************************************************
*
* Application class
*
**************************************************************/

class CSMan2Application : public CQikApplication
{
/*
public:
	CSMan2Application();
	~CSMan2Application();
*/
	
private: // from CApaApplication
	CApaDocument* CreateDocumentL();
	TUid AppDllUid() const;
	//CSManActiveScheduler *iSManActiveScheduler;
};

// to launch control panel CTL files. Thanks to Phil Spencer of symbian for the tip.
typedef void (*TLoadDll)(const TDesC& aDllFullPathName);

#endif
