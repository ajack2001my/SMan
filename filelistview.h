#include <eikcmbut.h>
#include <qiktoolbar.h>

#include "systemicon.h"
#include "dialogs.h"
#include "marquee.h"
#include "viewbase.h"
#include "fileman.h"
#include "hexeditview.h"

#if !defined(__FILELISTVIEW_H__)
#define __FILELISTVIEW_H__

class CCRCCalculator;

/*************************************************************
*
* File browser view. This is the second view
*
**************************************************************/

class CSMan2FileListView : public CViewBase, public MCoeControlObserver, public MCoeView
{
protected:		// implements MCoeView
	virtual void ViewDeactivated();
	virtual void ViewActivatedL(const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/);

public:
	CSMan2FileListView(CConfig *cData);
	~CSMan2FileListView();
	void ConstructL(const TRect& aRect);
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	virtual TVwsViewId ViewId() const;
	virtual TVwsViewIdAndMessage ViewScreenDeviceChangedL();
	virtual TBool ViewScreenModeCompatible(TInt aScreenMode);
	
	void SetZoomLevel(TInt zoomLevel);
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);	
	void doCRC(TPtrC fileName);
	void updateFileDateTime(void);
	void DoOpenFile(TInt aCommand);
	void CalculateCRC();
	void DoShowProperties();
	void SetAllSelectionStatus(TBool aSelectionState);
	void DoBeamFile();
	void ReadPath();
	TBool ReadAlternatePath(TPath *aPath);
	void DoCopyAs();
	void DoCopyOrCut(TInt aCommand);
	void ClearClipBoard();
	void DoCreateNewFolder();
	void DoPrepareHexEditor(CSMan2HexEditorView *aHexEditView);
	void DoPrepareFindFileCriteria(TFileName *aCriteria);
	void DoSwitchDrive(TChar aDriveLetter);
	void SetHighlightColor(TRgb aRgbHiLiteColor, TRgb aRgbTextColor);
	void StartScroller();
	void DoFileManagerOperation(TInt aOperation);
	void GetFile();
	TInt NumItemsInClipboard();
	
	TBuf<(MAX_LOG_LINES + 1) * MAX_LOG_LINE_SIZE> beamLog;
	CMarquee *currentPathScroller;
	CEikColumnListBox *cFileListBox;
	TPath currentPath;
	CBlueBeam *blueBeam;
	CCRCCalculator *crcCalculator;
	TPtrC iEntryName, iEntryType;

private:
	TInt GetDirHighlightIndex(TPath *aPath);
	void SetDirHighlightIndex(TPath *aPath, TInt aIndex);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void getFileAttrib(TDes *attribText, TInt index);
	TInt populateClipboard(TInt op);
	TBool fillListBox(TPath thePath);	
	
	CArrayFixFlat<TInt> *iDirHighlightIndex[KMaxDrives];
	CEikTextButton *labelCurrentFolder;
	TBuf<50> textFileDateTime;
	CEikTextButton *labelFileDateTime;
	CArrayFixSeg<TFileName> *clipBoard;
	TPath iOldPath;
	CSManFileMan *iFileManager;
	HBufC* driveOldPath[KMaxDrives];
	TInt oldItem;
	TFileName iFileName;
	CDir *fileList;
	// "Cut" or "Copy" ??
	TInt clipBoardOperation;
};

/*************************************************************
*
* Provides asynchronous CRC calculation
*
**************************************************************/

#define CRC_BUFFER_SIZE 1024

class CCRCCalculator : public CActive
{
public:
	CCRCCalculator();
	~CCRCCalculator();
	enum
	{
		CRCCancelled,
		CRCError
	};
	
	void doCRC(TPtrC fileName);
	void DoCancel();
	void RunL();

	TBool crcDialogActive;
	
	// This table is exposed so that others may use it for doing custom crc calculations
	unsigned int crcTable[256];

private:
	TUint crcValue;
	TBuf8<CRC_BUFFER_SIZE> inputBuffer;
	RFile inputStream;
	CCRCDialog* crcDialog;
	void stopCRC(TInt aReason);
	void finishCRC();
	TInt fileSize;
	TInt numBytesRead;
};

#endif