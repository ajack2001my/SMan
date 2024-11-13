#include <qiknumbereditor.h>		// CQikNumberEditor
#include <eikcapc.h>			// CEikCaptionedControl
#include <eikchkbx.h>			// CEikCheckBox
#include <QikSelectMediaFileDlg.h>	// CQikSelectMediaFileDlg
#include <qikutils.h>			// QikFileUtils
#include <qikzoomdlg.h>			// CQikZoomDialog
#include <eikseced.h>			// CEikSecretEditor
#include <qikslider.h>			// CQikSlider
#include <eikfsel.h>			// CEikFolderSelector

#include "bjackview.h"
#include "sman.hrh"

#if !defined(__DIALOGS_H__)
#define __DIALOGS_H__

//#define __DEBUG_BUILD__

#include "dbhelper.h"

// Note: this is always ONE more than the controls defined in the resource file because
// item #4 will be for SMan's autostart
#define MAX_AUTOSTART 4

/*************************************************************
*
* Logging level dialog
*
**************************************************************/

class CLogLevelDialog : public CEikDialog
{
public:
	CLogLevelDialog(TUint *logFlags);
	~CLogLevelDialog();
protected:
//	TBool ShutL() { return EFalse; };
	TUint *localLogFlags;
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Various bluejack timer options dialog
*
**************************************************************/

class CBluejackTimerDialog : public CEikDialog
{
public:
	CBluejackTimerDialog(TInt *noDevicePause, TInt *noDeviceAttempts, TInt *decayTime, TInt *obexTimeout);
	~CBluejackTimerDialog();
protected:
//	TBool ShutL() { return EFalse; };
	TInt *localNoDevicePause;
	TInt *localNoDeviceAttempts;
	TInt *localDecayTime;
	TInt *localObexTimeout;
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Hotkey dialog
*
**************************************************************/

class CHotkeyDialog : public CEikDialog
{
public:
	TInt *iHotkey;
	TInt *iInFocusShort_FO, *iInFocusLong_FO, *iExFocusShort_FO, *iExFocusLong_FO,
		*iInFocusShort_FC, *iInFocusLong_FC, *iExFocusShort_FC, *iExFocusLong_FC;
	CHotkeyDialog();
	~CHotkeyDialog();
	
protected:
	void HandleControlStateChangeL(TInt aControlId);
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
	
private:
	void InitFOPage_Controls();
	void InitFCPage_Controls();
	void RemoveFOAction(TInt aActionNumber_FO);
	void RemoveFCAction(TInt aActionNumber_FC);
	
	CArrayFixFlat<TInt> *iShortcutNumbers_FO;
	CArrayFixFlat<TInt> *iShortcutNumbers_FC;
	CDesCArray* iFOShortcutArray;
	CDesCArray* iFCShortcutArray;
	CDesCArray* iHotkeyArray;
};

/*************************************************************
*
* Shortcuts dialog
*
**************************************************************/

class CShortcutDialog : public CEikDialog
{
public:
	CShortcutDialog(CArrayFixFlat<TUid> *shortCut);
	~CShortcutDialog();
	CDesCArray *appListArray;
	CArrayFixFlat<TUid> *appListUid;
	void populateAppArray(void);
protected:
//	TBool ShutL() { return EFalse; };
	CArrayFixFlat<TUid> *tempUid;
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Autostart dialog
*
**************************************************************/

class CAutoStartConfig
{
public:
	TFileName autoStartApp[MAX_AUTOSTART];
	TUid autoStartUid[MAX_AUTOSTART];
	TFileName autoStartDefaultDoc[MAX_AUTOSTART];
	TInt placeHolder;
};

class CAutostartDialog : public CEikDialog
{
public:
	CAutostartDialog();
	~CAutostartDialog();
	CAutoStartConfig autoStartConfig;
	TUint loadBootFile(void);
	TUint saveBootFile(void);
protected:
//	TBool ShutL() { return EFalse; };
	CDesCArray *appListCaption;
	CArrayFixSeg<TFileName> *appListPath;
	CArrayFixFlat<TUid> *appListUid;
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
	void populateAppArray(void);
};

/*************************************************************
*
* Bluejack notifications dialog
*
**************************************************************/

class CBluejackNotifications : public CEikDialog
{
public:
	CBluejackNotifications(CConfig *cData);
	~CBluejackNotifications();
protected:
//	TBool ShutL() { return EFalse; };
	void HandleControlStateChangeL(TInt aControlId);
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	void pickAudioFile(void);
private:
	TFileName selectedAudioFileName;
	CConfig *configData;
};

/*************************************************************
*
* Bluebeam log dialog
*
**************************************************************/

class CBluebeamLogDialog : public CEikDialog
{
public:
	CBluebeamLogDialog(CLogger *logObj);
	~CBluebeamLogDialog();
protected:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void PostLayoutDynInitL();	
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
private:
	CLogger *localLogObj;
};

/*************************************************************
*
* Class for dialog for editing bluejack vcard
*
**************************************************************/

class CBluejackMessageDialog : public CEikDialog
{
public:
	CBluejackMessageDialog(CScanner *scannerObj);
	~CBluejackMessageDialog();
protected:
//	TBool ShutL() { return EFalse; };
	void makeVCard(void);
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
	CScanner *localScannerObj;
	TBuf<MAX_VCARD_SIZE> currBJackMsg;
};

/*************************************************************
*
* Fileman log dialog
*
**************************************************************/

class CFileManLogDialog : public CEikDialog
{
public:
	TInt progressValue;
	TPtrC text;
	TBool showOverwrite;
protected:
	TBool ShutL() { return EFalse; };
	void PostLayoutDynInitL();
};

/*************************************************************
*
* Fileman rename
*
**************************************************************/

class CFileManRenDialog : public CEikDialog
{
public:
	TFileName fileName;
	TPath filePath;
private:
	TFileName newName;
protected:
//	TBool ShutL() { return EFalse; };
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Fileman new folder
*
**************************************************************/

class CFileManNewDirDialog : public CEikDialog
{
public:
	TPath currentPath;
protected:
//	TBool ShutL() { return EFalse; };
	TBool OkToExitL(TInt aButtonId);
};

/*************************************************************
*
* Fileman attribute dialog
*
**************************************************************/

class CFileManAttrDialog : public CEikDialog
{
public:
	TFileName theFile;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

protected:
//	TBool ShutL() { return EFalse; };
	TBool OkToExitL(TInt aButtonId);
	void PostLayoutDynInitL();
};

/*************************************************************
*
* Fn menu toggling
*
**************************************************************/

class CFnMenuToggle : public CEikDialog
{
public:
	CFnMenuToggle(TUint *bits, TInt *defaultView);
	~CFnMenuToggle();
	
protected:
	void PostLayoutDynInitL();
	void SynchronizeControls();
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();	
	void HandleControlStateChangeL(TInt aControlId);
	
private:
	TUint *toggleBits;
	TInt *theDefaultView;
	CArrayFixFlat<TInt> *iDefaultViewIndex, *iViewIndexes;
};

/*************************************************************
*
* CRC progress
*
**************************************************************/

class CCRCDialog : public CEikDialog
{
public:
	CCRCDialog(TPtrC fileName);
	~CCRCDialog();
	void updateProgress(TInt aProgress);
	TPtrC theFileName;

protected:
	void stopCRC();
	void PreLayoutDynInitL();
};

/*************************************************************
*
* DB progress
*
**************************************************************/

class CDBProgressDialog : public CEikDialog
{
public:
	CDBProgressDialog();
	~CDBProgressDialog();
	void updateProgress(TInt aProgress);
	void setProgressMaxValue(TInt aMaxValue);

protected:
	TBool ShutL() { return EFalse; };		// Cannot be dispatched
	void PostLayoutDynInitL();
};

/*************************************************************
*
* Generic dialog to get password
*
**************************************************************/

class CGetPasswordDialog : public CEikDialog
{
public:
	CGetPasswordDialog(TPassword *thePassword);
	~CGetPasswordDialog();

protected:
	TBool OkToExitL(TInt aButtonId);

private:
	TPassword *localPassword;
};

/*************************************************************
*
* Generic dialog to set password
*
**************************************************************/

class CSetPasswordDialog : public CEikDialog
{
public:
	CSetPasswordDialog(TPassword *thePassword);
	~CSetPasswordDialog();

protected:
	TBool OkToExitL(TInt aButtonId);

private:
	TPassword *localPassword;
};

/*************************************************************
*
* Detailed calendar dialog
*
**************************************************************/

class CAgendaDetail : public CEikDialog
{
public:
	TDesC *agendaDate, *startDateTime, *endDateTime, *alarmDateTime, *location;
	TPtrC notes;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	
protected:
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
};

/*************************************************************
*
* Detailed todo dialog
*
**************************************************************/

class CTodoDetail : public CEikDialog
{
public:
	TDesC *dueDateTime, *alarmDateTime, *priority;
	TPtrC notes;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	TTime *iCrossedOutDate;

protected:
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
};

/*************************************************************
*
* Hexeditor GOTO OFFSET dialog
*
**************************************************************/

class CHexEditGotoOffset : public CEikDialog
{
public:
	CHexEditGotoOffset(TInt aMaxOffset, TInt *aSelectedOffset, TInt aCurrentOffset);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void HandleControlStateChangeL(TInt aControlId);

protected:
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	void PrepareForFocusTransitionL();
	
private:
	void SetHexControlValue(TInt aValue);
	void SetDecControlValue(TInt aValue);
	void SetSliderControlValue(TInt aValue);

	TInt iMaxOffset, iCurrentOffset;
	TInt *iSelectedOffset;
};

/*************************************************************
*
* Hexeditor window resize dialog
*
**************************************************************/

class CHexEditResize : public CEikDialog
{
public:
	CHexEditResize(TInt *aCurrentSize);

protected:
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);

private:
	TInt *iCurrentSize;	
};

/*************************************************************
*
* FindFile criteria dialog
*
**************************************************************/

class CFindFileCriteria : public CEikDialog
{
public:
	CFindFileCriteria(CDesCArray *aFoldersToScan, TInt *aModifiedDateOption, TTime *aFromTime, TTime *aToTime, TFileName *aSearchWildCard, TFileName *aHighlightedFolder);

protected:
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	
private:
	CDesCArray *iLocalFoldersToScan;
	TInt *iLocalModifiedDateOption;
	TTime *iLocalFromTime, *iLocalToTime;
	TFileName *iLocalSearchWildCard, *iLocalHighlightedFolder;
	void AutoCompleteOtherLocation();
	void HandleControlStateChangeL(TInt aControlId);
};

/*************************************************************
*
* Font selector dialog
*
**************************************************************/

#define SYSTEMFONT_INIFILE _L("C:\\System\\Data\\SysFnt.ini")
class CFontSelector : public CEikDialog
{
public:
	CFontSelector();
	~CFontSelector();

protected:
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	void HandleControlStateChangeL(TInt aControlId);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

private:
	void SetPreviewWindowFont();
	CDictionaryStore* OpenFontIniFileLC(TBool aResetFile);
	void WriteAntiAliasingStateToIniFileL(TBool aUseAntiAliasing);
	void WriteSystemFontNameAndSizeToIniFileL(const TDesC& aFontName, TInt aSizeInPixel);

	CRichText *iRichText;
	TInt i;
	TBuf<KMaxTypefaceNameLength> iFontName;	
};

/*************************************************************
*
* Credits dialog
*
**************************************************************/

class CCreditsDialog : public CEikDialog
{
public:
	TInt iMessageResourceID;
	
protected:
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
};

/*************************************************************
*
* Dialog to edit group info, cell info, events
*
**************************************************************/

class CEditCellDBDialog : public CEikDialog
{
public:
	CEditCellDBDialog(CDBHelper *iCellDBHelper);
	~CEditCellDBDialog();
	TDes *iDefaultNetworkIdForNewCells;
	TDes *iDefaultCountryCodeForNewCells;
	
protected:
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
	void HandleControlStateChangeL(TInt aControlId);
	void ProcessPageButtonCommandL(TInt aCommandId);
	void PageChangedL(TInt aPageId);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	
private:
	CDBHelper *iLocalCellDBHelper;
	CArrayFixFlat<TUint> *iGroupIds, *iGroupPriorities, 
		*iEventIds, *iEventTypeIds, *iEventPriorities, *iEventTriggerIds;
	CArrayFixFlat<TInt8> *iGroupStates;
	CDesCArray *iGroupNames, *iEventDesc, *iEventTriggerDesc;
	TBool iGroupInfoOnCellsChanged, iGroupInfoOnEventsChanged, iGroupInfoOnImportExportChanged;
	
	TBool ReadGroupInfo();
	void InitGroupsPage();
	void InitImportExportPage(TBool aInitGroupList);
	TBool InitCellsPage(TBool aInitGroupList, TUint aCellId, TBool aMaintainSelectionIndex);
	TBool InitEventsPage(TBool aInitGroupList);
	void UpdatePageButtons_ImportExport();
	void UpdatePageButtons_Groups();
	void UpdatePageButtons_Cells();
	void UpdatePageButtons_Events();
	TBool SetGroupState(TInt aState);
	TBool SetEventState(TInt aState);
	TBool ShiftGroup(TInt aIncrement);
	TBool ShiftEvents(TInt aIncrement);
	void RehighlightGroupsPage(TInt aIndex);
	void RehighlightEventsPage(TInt aIndex);
	void CreateQualifiedTextField(HBufC *iBuffer);
	TInt CreateCell(TUint aCellId, TUint aGroupId, TUint aGroupPriority, TUint aGroupState, TDes *aCountryCode, TDes *aNetworkCode, TDes *aCellDescription);
	TInt CreateGroup(TDes *aGroupName, TInt aState, TUint *aGroupId, TUint *aPriority);
	void SetListBoxSelection(TBool aSelectState, CEikColumnListBox *aListBox);
};

/*************************************************************
*
* Generic dialog that will allow the user to enter a new string
* or edit an existing string. Used by cell db editor dialog
*
**************************************************************/

class CEditCellDBGenericString : public CEikDialog
{
public:
	TDes *iTheString;
	TInt iTextLimit;
	TBool iIsRequired;
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* New group dialog
*
**************************************************************/

class CEditCellDBNewGroup : public CEikDialog
{
public:
	TDes *iTheString;
	TInt iTextLimit;
	TBool iIsRequired;
	TInt *iStatus;
protected:
	virtual TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* New cell dialog. Can also be used to edit a cell
*
**************************************************************/

class CEditCellDBNewCell : public CEikDialog
{
public:
	TUint *iCellId;
	TDes *iCountryCode, *iNetworkCode, *iCellDescription;
	TUint *iGroupId;
	CDesCArray *iGroupNames;
	
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Select group dialog. Used for moving a bunch of cells
*
**************************************************************/

class CEditCellDBMoveCells : public CEikDialog
{
public:
	TUint *iGroupId;
	CDesCArray *iGroupNames;
	
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Prompt cell id to find
*
**************************************************************/

class CEditCellDBFindCell : public CEikDialog
{
public:
	TUint *iCellId;
	
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Found cells. Now, select your group
*
**************************************************************/

class CEditCellDBSelectGroup : public CEikDialog
{
public:
	TUint *iGroupIdIndex;
	CDesCArray *iGroupNames;
	
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Select new event type to create
*
**************************************************************/

class CEditSelectEvent : public CEikDialog
{
public:
	TUint *iGroupIdIndex, *iEventTypeIdIndex, *iEventTriggerIdIndex;
	CDesCArray *iGroupNames, *iEventTypeDescriptions, *iEventTriggerDescriptions;
	TDes *iEventDescription;
	TInt *iState;
	
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Create audio event
*
**************************************************************/

class CEditCellNewAudioEvent : public CEikDialog
{
public:
	HBufC *iSelectedAudioFileName;
	
protected:
	void HandleControlStateChangeL(TInt aControlId);
	virtual void PreLayoutDynInitL();
	virtual TBool OkToExitL(TInt aButtonId);
	void PickAudioFile(void);
};

/*************************************************************
*
* Edit audio event
*
**************************************************************/

class CEditCellEditAudioEvent : public CEditCellNewAudioEvent
{
public:
	HBufC *iEventName;
	TUint *iEventTriggerIdIndex;
	CDesCArray *iEventTriggerDescriptions;
	
protected:
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
};

/*************************************************************
*
* Edit vibrate event
*
**************************************************************/

class CEditCellEditVibrateEvent : public CEditCellDBGenericString
{
public:
	TUint *iEventTriggerIdIndex;
	CDesCArray *iEventTriggerDescriptions;
	
protected:
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
};

/*************************************************************
*
* Select drive
*
**************************************************************/

class CSelectDrive : public CEikDialog
{
public:
	CSelectDrive();
	~CSelectDrive();
	TDes *iDriveLetter;
	
protected:
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	
private:
	CDesCArray *iDriveLetters;
};

/*************************************************************
*
* Given a listbox, return an array of selection indexes
*
**************************************************************/

class CSelectMultiListBox : public CEikDialog
{
public:
	CListBoxView::CSelectionIndexArray *iSelectionIndexes;
	CDesCArray *iListBoxItems;
	
protected:
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
	
};

/*************************************************************
*
* Progress for cell import / export 
*
**************************************************************/

class CProgressDialog : public CEikDialog
{
public:
	TBool *iDialogStillActive;
	void SetButtonDim(TBool aDim);
	void AppendText(TInt aResourceId, HBufC *aText);
	void AppendText(TInt aResourceId, TDes *aText);
	void SetProgressValue(TInt aValue);
	
protected:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
};

/*************************************************************
*
* Select phone model to fool sman. This is the debug dialog
*
**************************************************************/
#ifdef __DEBUG_BUILD__
class CDebugDialog : public CEikDialog
{
public:
	TUint *iRunningPhoneModel;
	
protected:
	void PreLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
};
#endif

/*************************************************************
*
* Edit bluetooth exclusion list
*
**************************************************************/

class CEditBTExclusionList : public CEikDialog
{
public:
	CArrayFixFlat<CScanner::TBTDevice> *iBTDeviceArray;
	
protected:
	void RefreshListBox();
	void PostLayoutDynInitL();
	TBool OkToExitL(TInt aButtonId);
};

/*************************************************************
*
* Edit one BT device
*
**************************************************************/

class CEditBTDevice : public CEditCellDBNewGroup
{
protected:
	TBool OkToExitL(TInt aButtonId);
};

/*************************************************************
*
* Specify agenda todo priority filter
*
**************************************************************/

class CAgendaMaxTodoPriority : public CEikDialog
{
public:
	TUint *iMaxPriority;
	
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Cell area event firing delay
*
**************************************************************/

class CCellEventDelay : public CEikDialog
{
public:
	TInt *iDelay;
	
protected:
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
};

/*************************************************************
*
* Folder browser
*
**************************************************************/
class CFolderBrowser;

class CColumnListBoxWithPointerHandler : public CEikColumnListBox
{
public:
	CFolderBrowser *iDialogContainer;
	CColumnListBoxWithPointerHandler();
	~CColumnListBoxWithPointerHandler();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	
protected:
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void ConstructFromResourceL(TResourceReader& aReader);
	TInt GetEntryType(TInt aItem);
	
private:
	HBufC *iSelectedEntry;
};

class CFolderBrowser : public CEikDialog
{
public:
	CFolderBrowser();
	
	/*
	When this dialog is first called, this contains the initial path that will be focused to
	If blank, it will default to C:\
	When the dialog quits, this contains the path the user wants
	*/
	TPath *iPath;
	// The file mask for files to display
	TFileName iFileSpec;
	// If this flag is set to true, this class displays files that match iFileSpec in the
	// current folder and allows the user to select these files. Else, it only displays
	// folders
	TBool iAllowSelection;
	// Contains the list of files selected if iAllowSelection is ETrue. This array contains only
	// filenames. To get the fully qualified path to the file, you must combine each entry in
	// this array with the value in iPath. This is the responsibility of the caller. The caller
	// is responsible for providing a reference iFilesSelected. This class does not own this
	// array
	CDesCArray *iFilesSelected;
	// Returns true if folder changed
	void ProcessFolderNavigation();
	
protected:
	SEikControlInfo CreateCustomControlL(TInt aControlType);
	TBool OkToExitL(TInt aButtonId);
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	
private:
	void ReadFolder();
	void SetItemSelected(TBool aIsSelected);

	TPath iCurrentPath;
	TBuf<KMaxFileName + 4> iSelectedEntry;
	TPtrC iTempText;
	TPath iSelectedPath;
	TInt iEntryType;
	TLex iLexConverter;
};

#endif