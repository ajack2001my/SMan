#include <d32dbms.h>				// RDbStoreDatabase
#include <s32file.h>				// CPermanentFileStore
#include <coeutils.h>				// ConeUtils
#include <eikenv.h>					// CEikonEnv
#include <eikfutil.h>				// EikFileUtils
#include "dialogs.h"

/*************************************************************
*
* Database helper class. This class provides utility members to
* access a database. This is a generic storage class that can
* be used by any other class. It provides these basic functionalities
*
* Db open
* DB close
* Execute SQL statement
* Populate view with SELECT statement
* Compact DB
* Update stats
*
* Note: This class does not support encryption / decryption
*
*************************************************************/

#if !defined(__DBHELPER_H__)
#define __DBHELPER_H__

class CDBHelper
{
public:
	CDBHelper();
	~CDBHelper();
	TBool InitDB();
	void DeInitDB();
	TBool ReadDB();
	TBool PrepareReadDBLongField(TInt aColumnNumber);
	TBool PrepareWriteDBLongField(TInt aColumnNumber);
	TInt ExecuteSQL();
	TBool CompactDB();
	TBool UpdateDBStats();
	TBool CreateDB();
	TBool DeleteDB();
	void ConvertTextStatement(TDes *aTheString, TPtrC aStrToReplace, TPtrC aReplacementStr);
	TInt BeginTransaction();
	void RollbackTransaction();
	TInt CommitTransaction();
	TBool RecoverDB();

	RDbColReadStream iDBReadStream;
	RDbColWriteStream iDBWriteStream;
	TBool iDBIsOpened;	
	RDbView iDBView;
	TFileName iDBFileName;
	TBuf<512> iSQLStatement;

private:
	TBool DoLongDBOp(TInt iStepNumber);

	RDbStoreDatabase iDBStore;
	CFileStore *iFileStore;
	TDbWindow *iDBWindow;
	RDbIncremental iIncrementalDBOp;
};

#endif