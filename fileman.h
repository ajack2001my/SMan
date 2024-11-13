#include <eikfutil.h>		// EikFileUtils
#include <f32file.h>
#include <eikgted.h>
#include "dialogs.h"

#if !defined(__FILEMAN_H__)
#define __FILEMAN_H__

/*************************************************************
*
* Our actual file manager
*
**************************************************************/
class CSManFileMan : public CBase, public MFileManObserver
{
public:
	enum
	{
		operationCopy,
		operationCut,
		isPaste,
		isDelete,
		isRename,
		isCopyAs	// This is a special case that is handled separately
	};

	CSManFileMan(CConfig *cData);
	~CSManFileMan();
	
	CArrayFixSeg<TFileName> *fileList;
	TInt fileManOperation;
	//CArrayFixSeg<TInt> *operationList;
	TPath targetPath;
	TUint currentOperation;
	void doWork();
	void updateText(CEikGlobalTextEditor *textEdit);
	void updateErrorMsg(CEikGlobalTextEditor *textEdit);
	CFileManLogDialog *progressDialog;
	CFileMan *fileMan;
		
private:
	TInt prevOverwriteStatus;
	void setupDialog();
	TInt currentFile;
	TUint opSwitch;
	TInt userRet;
	CLogger *logObj;
	CEikGlobalTextEditor *progressText;
	CEikProgressInfo *progressBar;
	TInt totalBytes, currentBytes;
	void displayPromptDialog(TBool showOverwrite);

	TControl NotifyFileManStarted();
	TControl NotifyFileManOperation();
	TControl NotifyFileManEnded();
		
	RFs fileServer;
};

#endif