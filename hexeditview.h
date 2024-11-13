#include "systemicon.h"
#include "viewbase.h"

#if !defined(__HEXEDITVIEW_H__)
#define __HEXEDITVIEW_H__

/*************************************************************
*
* HexEditor View
*
* The hexeditor view is a little different from the other views.
* In all respects, it is a standalone view like all the others.
* However, we only allow it to be activated from the file manager.
* And you can only return back to the file manager from the hex editor
* Thus, the hexeditor view does not have a "Fn" menu option
*
**************************************************************/

#define READ_BUFFER_SIZE 128
#define PAGE_SIZE READ_BUFFER_SIZE * 2

class CHexEditorControl;
class CASCIIDisplayControl;

class CSMan2HexEditorView : public MCoeControlObserver, public MCoeView, public CViewBase
{		
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	CSMan2HexEditorView(CConfig *cData);
	~CSMan2HexEditorView();
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	void ConstructL(const TRect& aRect);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);
	void ModifyHexEditorSelection();
	void SetHexControlIsReadOnly(TBool aIsReadOnly);
	TBool SetHexAndASCIIControlsExtent();
	TInt FileIsInROM();
	TBool FileHasReadOnlyAttribute();
	TBool OpenFile();
	TInt ReadPage(TInt aPageOffsetToRead);
	TInt WritePage();
	TBool DoEditFile();
	TBool IsAtLastPage();
	TBool HasSelection();
	void ReadNextPage();
	void ReadPrevPage();
	void ReadBeginPage();
	void ReadEndPage();
	void GotoOffset();
	void ReleaseFileAndBuffer();
	void DeleteBytes();
	void DoReload();
	void DoFileSave();
	TBool ToggleReadOnly();

	TFileName iFileName;
	TBool iFileIsOpen;
	TInt iCurrentFilePageOffset;
	
private:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void UpdateOffsetAndFileSizeText();
	void PopulateHexControl();
	void SetHexControlExtent();
	void SetASCIIControlExtent();

	TRect iClientRect;
	RFile iFile;
	CEikLabel *iOffsetAndFileSize;
	// The backup buffer is used like so:
	// 1. User made changes to the current page
	// 2. User wants to move to next page
	// 3. User chooses to discard changes in current page
	// 4. Next page is unavailable as this is the last page
	// 5. iBackupBuffer contents are recopied into the current page to reflect that the user
	//    chose to discard changes
	HBufC8 *iDataBuffer, *iBackupBuffer;
	TBuf<50> iOffsetAndFileSizeText;
	CFbsBitmap *iLineCursorBitmap;
	TBool iIsPageModified;	
	TInt iFileSize;
	CHexEditorControl *iHexEditControl;
	CASCIIDisplayControl *iASCIIControl;
};

/*************************************************************
*
* HexEditor control
*
* See comments for ASCII control
**************************************************************/

class CHexEditorControl : public CEikGlobalTextEditor
{
public:
	CHexEditorControl();
	~CHexEditorControl();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void CopyByteDataToDisplay();
	void InitState(CFbsBitmap *iLineCursorBitmap);
	void ModifySelection();
	void SetAllowInsertDelete(TBool aAllowInsert, TBool aAllowDelete);
	void MoveCursorToRelativeOffset(TInt aOffset);
	void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
	void ScrollDisplayByPixels(TInt aDeltaY);
	void SetAllowEdit(TBool aAllowEdit);
	void CopyDisplayToByteData(HBufC8 *aBuffer);
	void DoDelete();
	
	// The window that holds the ASCII characters. This is only a pointer to the control
	// owned by the view. This class is NOT responsible for destroying this object
	CASCIIDisplayControl *iASCIIWindow;
	void SynchronizeWithASCIIWindow(TBool aIsDelete, TBool aIsChunkUpdate);
	// This class is NOT responsible for destroying this object i.e. ownership is not transferred
	HBufC8* iByteData;

private:
	char iAllowableChars[100];
	TInt iNumAllowableChars;
	void UpdateASCIIWindowScrollPosition();
	TBool IsDisplayableChar(char iChar);
	// iAllowEdit takes precedence over iAllowInsert and iAllowDelete
	TBool iAllowInsert, iAllowDelete, iAllowEdit;
};

/*************************************************************
*
* ASCII control
*
* This control goes hand-in-hand with the hexeditor control
* The hexeditor control stores a reference to this control
* and this control stores a reference to the hexeditor control
**************************************************************/

class CASCIIDisplayControl : public CEikGlobalTextEditor, public MFormCustomWrap
{
public:
	CASCIIDisplayControl();
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	void HandleScrollEventL(CEikScrollBar* aScrollBar,TEikScrollEvent aEventType);
	void InitState();
	void ScrollDisplayByPixels(TInt aDeltaY);
	void DoDelete();
	
	// Reference to the hexeditor control
	// This class is NOT responsible for destroying this object
	CHexEditorControl *iHexWindow;

	// Implements MFormCustomWrap
	TBool LineBreakPossible(TUint aPrevClass,TUint aNextClass,TBool aHaveSpaces) const;
};

#endif