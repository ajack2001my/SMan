#include <qikscrollablecontainer.h>	// CQikScrollableCOntainer
#include <baclipb.h>				// CClipboard
#include <eikcmbox.h>				// CEikComboBox
#include <eikcmbut.h>				// CEikCommandButton
#include <d32dbms.h>				// RDbStoreDatabase
#include <s32file.h>				// CPermanentFileStore
#include <eikpword.h>				// CEikSetPasswordDialog

#include "systemicon.h"
#include "dialogs.h"
#include "viewbase.h"

#if !defined(__DBVIEW_H__)
#define __DBVIEW_H__

#define DEFAULT_PASSWORD _L("0000")

#define NUM_DATA_FIELDS 5

class CComboBox;
class CGlobalTextEditor;

/*************************************************************
*
* DB View
*
**************************************************************/

class CSMan2DBView : public CViewBase, public MCoeControlObserver, public MCoeView
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void SetZoomLevel(TInt zoomLevel);

public:
	CSMan2DBView(CConfig *cData);
	~CSMan2DBView();
	void ConstructL(const TRect& aRect);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);	
	void swapFocus();
	void ShowRecordsSingle(TUint32 aIndex, TBool aIsNewRecord);
	void ShowRecordsSingleReadOnly(TUint32 aIndex);
	void ShowRecordsList(TBool aRefresh, TBool showToolbar);
	TBool SaveRecord(TUint32 aIndex);
	TBool DeleteRecord(TUint32 aIndex);
	void DoEditCommands(TInt aCommand);
	void doDisplayOneRecord(TBool aIsReadOnly);
	TBool doLongDBOp(TInt stepNumber);
	TBool doCompact();
	TBool doUpdateStats();
	TBool doDelete();
	void doChangePassword();
	void doReadDB();
	void doReInitDB();
	void doCloseDB(TBool aDrawToolbar);
	void ExportDB();
	void ImportDB();
	
	CEikColumnListBox *cIndexListBox;
	TUint32 currentRecordIndex;
	// I needed a flag here because currentRecordIndex is not a TInt else I would've indicated an invalid
	// index by setting currentRecordIndex to -1. I couldn't use zero either because the COUNTER field
	// in the DB starts at zero. This variables holds the COUNTER field for a particular record. This is the
	// key to the particular record we are working on
	TBool currentRecordIndexIsValid;
	TBool dbIsOpened;

	enum
	{
		DBView_Single_ReadOnly,
		DBView_Single_Editable,
		DBView_List
	};
	TInt currentDBView;
	
	enum
	{
		DBOp_Browsing,
		DBOp_Editing,
		DBOp_Adding,
		DBOp_Viewing,
	};
	TInt currentDBOp;

private:
	void doCreateTable();
	void doCreateIndex();
	TBool doWriteDB();
	TBool InitDB();
	void HideRecordsList();
	void HideRecordsSingle();
	void HideRecordsSingleReadOnly();
	TBool ReadDB(TPtrC sqlStatement);
	TBool ReadRecordsList();
	void DeInitDB();
	void HideAllControls();
	void MoveEditCursors(TCursorPosition::TMovementType aCursorDirection);
	void ClipboardAction(CEikEdwin *aEdwin, TInt aCommand);
	
	RDbView dbView;
	RDbStoreDatabase dbStore;
	CFileStore *fileStore;
	TDbWindow *dbWindow;
	RDbIncremental incDB;
	CSecurityBase *securityBase;
	CSecurityEncryptBase *encryptor;
	CSecurityDecryptBase *decryptor;
	TPassword dbPassword;
	TFileName dbFileName;
	
	CQikScrollableContainer* scrollableContainer;
	// I prefer using these now instead of dynamic arrays because
	// 1) access is cleaner and faster since it doesn't go through a class
	// 2) i have better control over what happens
	// 3) memory for the pointers are preallocated (ok, small excuse :))
	CComboBox* fieldLabels[NUM_DATA_FIELDS];
	CGlobalTextEditor* fieldValues[NUM_DATA_FIELDS];
	CGlobalTextEditor* fieldDisplayName;
	CEikLabel* fieldLabelsNumbering[NUM_DATA_FIELDS];
	CDesCArrayFlat *fieldLabelsArray;
	CGlobalTextEditor *readOnlyDisplay;
	void PositionRecordsSingleControls();
	//CArrayFixSeg<CComboBox>* fieldLabels;
	//CArrayFixSeg<CGlobalTextEditor>* fieldValues;
	
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	TInt oldItem;
};

/*************************************************************
*
* Behaves like normal except when focus is lost it will bring the cursor of the edwin
* to the front
*
**************************************************************/

class CComboBox : public CEikComboBox
{
public:
	CComboBox();
	~CComboBox();
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void SetTextListBoxZoom();
	void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEvent);
	CFont *theFont;
	
private:
	TBool popoutNew;
};

/*************************************************************
*
* As per the CComboBox
*
**************************************************************/

class CGlobalTextEditor : public CEikGlobalTextEditor
{
public:
	CGlobalTextEditor();
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
};

#endif