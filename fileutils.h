#include "dbhelper.h"
#include <f32file.h>
#include <e32base.h>
#include <e32std.h>
#include <badesca.h>
#include <zipfile.h>
#include <zipfilemember.h>
#include <zipfilememberiterator.h>

#if !defined(__FILEUTILS_H__)
#define __FILEUTILS_H__

/*************************************************************
*
* Provides synchronous file deletion with exclude list
*
**************************************************************/

class CRecursiveDelete : public CBase, public MFileManObserver
{
public:
	TPath iPathToDelete;
	CDesCArray *iExcludeFiles;
	CDesCArray *iExcludePaths;

	CRecursiveDelete();
	~CRecursiveDelete();
	TInt DoDelete(TUint aFileManRecursiveSwitch);

protected:
	TControl NotifyFileManStarted();

private:
	CFileMan *iFileMan;
	TParse iParse;
};

/*************************************************************
*
* Provides synchronous un-zip of a zipfile
*
**************************************************************/

// This is the size of the buffer to be filled when deflating a file. It is also the size
// of the buffer that is written to the disk progressively as a file is deflated.
#define FILE_BUFFER_SIZE 2048
class CUnzip
{
public:
	TInt UnZipOneFile(TFileName *aZipFileName, TFileName *aFileToExtract, TPath *aExtractPath, TBool iShowErrors);
private:
	TInt ExtractOneFile(CZipFileMember* aZipFileMember, CZipFile* aZipFile, TFileName* aOutputFileName, TPath *aExtractPath, TBool iShowErrors);
};

#endif
