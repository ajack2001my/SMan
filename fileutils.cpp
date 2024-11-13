#include "fileutils.h"
#include "textutils.h"
#include <eikenv.h>
#include <sman.rsg>

/*************************************************************
*
* Provides synchronous file deletion with exclude list
*
**************************************************************/

CRecursiveDelete::CRecursiveDelete()
{
	iFileMan = CFileMan::NewL(CEikonEnv::Static()->FsSession(), this);
	iExcludeFiles = new (ELeave) CDesCArraySeg(1);
	iExcludePaths = new (ELeave) CDesCArraySeg(1);
}

CRecursiveDelete::~CRecursiveDelete()
{
	delete iFileMan;
	delete iExcludeFiles;
	delete iExcludePaths;
}

// aFileManRecursiveSwitch = CFileMan::ERecurse or 0
TInt CRecursiveDelete::DoDelete(TUint aFileManRecursiveSwitch)
{
	TInt iResult = iFileMan->Delete(iPathToDelete, aFileManRecursiveSwitch);
	
	// You can get a KErrNotFound if iPathToDelete is empty. For example, C:\TODELETE\*.* is
	// in iPathToDelete. Even though C:\TODELETE exists, you can still get KErrNotFound if
	// C:\TODELETE does not contain any files. The following code fixes that by returning
	// KErrNone if this scenario occurs
	if (iResult == KErrNotFound)
	{
		iParse.Set(iPathToDelete, NULL, NULL);
		TBool iFolderExists = EikFileUtils::FolderExists(iParse.DriveAndPath());
		if (iFolderExists)
			iResult = KErrNone;
	}
	
	return iResult;
}

MFileManObserver::TControl CRecursiveDelete::NotifyFileManStarted()
{
	MFileManObserver::TControl iRetVal = MFileManObserver::EContinue;
	TInt iDummy;
	TFileName iFileName;
	TPath iPath;
	
	iFileMan->GetCurrentSource(iFileName);
	iFileName.UpperCase();
	iParse.Set(iFileName, NULL, NULL);
	iPath.Copy(iParse.DriveAndPath());
	
	// The caller is responsible for ensuring iExcludeFiles and iExcludePaths are all in 
	// upper case!
	if ((iExcludeFiles->Find(iFileName, iDummy) == 0) || (iExcludePaths->Find(iPath, iDummy) == 0))
		iRetVal = MFileManObserver::ECancel;
	else
	{
		// Remove the read-only attribute from the file
		if (CEikonEnv::Static()->FsSession().SetAtt(iFileName, 0, KEntryAttReadOnly) != KErrNone)
			iRetVal = MFileManObserver::EAbort;
	}
	return iRetVal;
}

/*************************************************************
*
* Provides synchronous un-zipping of a zipfile
*
* Note: Adapted from:
* http://www3.symbian.com/faq.nsf/0/9203211F39FBA7A380256CA700560E21?OpenDocument
*
**************************************************************/

TInt CUnzip::UnZipOneFile(TFileName *aZipFileName, TFileName *aFileToExtract, TPath *aExtractPath, TBool iShowErrors)
{
	TInt iError = KErrGeneral;
	TBool iFileToExtractFound = EFalse;
	TFileName iFileToExtract;
	
	// Check if the zip file is valid. Doesn't seem to work very well. :( some valid zips
	// are rejected as invalid. i think it's because this library is old :(
	//if (EZGZipFile::IsGzipFile(CEikonEnv::Static()->FsSession(), *aZipFileName))
	CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
	if (ConeUtils::FileExists(*aZipFileName))
	{
		CZipFile* iZipFile = NULL;
		iFileToExtract.Copy(*aFileToExtract);
		iFileToExtract.UpperCase();
		TRAP(iError, iZipFile = CZipFile::NewL(CEikonEnv::Static()->FsSession(), *aZipFileName));
		if (iError == KErrNone)
		{
			TRAP(iError, iZipFile->OpenL());
			if (iError == KErrNone)
			{
				CZipFileMemberIterator* iZipFileMembers = NULL;
				TRAP(iError, iZipFileMembers = iZipFile->GetMembersL());
				if (iError == KErrNone)
				{
					TFileName iZipMemberName;
					TParse iParse;
					CZipFileMember* iZipFileMember = NULL;
					CCSVLineParser *iCSVParser = new (ELeave) CCSVLineParser;
					while (iError == KErrNone)
					{
						TRAP(iError, iZipFileMember = iZipFileMembers->NextL());
						if ((iError == KErrNone) && (iZipFileMember))
						{
							iZipMemberName.Copy(*(iZipFileMember->Name()));
							// Zip stores folder backslash as forwardslash. We need to
							// translate it back
							iCSVParser->ReplaceAll(&iZipMemberName, _L("/"), _L("\\"));
							iParse.Set(iZipMemberName, NULL, NULL);
							iZipMemberName.Copy(iParse.NameAndExt());
							iZipMemberName.UpperCase();
							if (iZipMemberName.Compare(iFileToExtract) == 0)
							{
								iFileToExtractFound = ETrue;
								break;
							}
						}
						else
						{
							iError = KErrCorrupt;
							if (iShowErrors)
								CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRREADZIP);
						}
					}
					delete iCSVParser;					
					if (iFileToExtractFound)
						iError = ExtractOneFile(iZipFileMember, iZipFile, aFileToExtract, aExtractPath, ETrue);
					delete iZipFileMember;
				}
				else
				{
					if (iShowErrors)
						CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRREADZIP);
					iError = KErrCorrupt;
				}
				delete iZipFileMembers;
				iZipFile->Close();
			}
			else
			{
				iError = KErrNotReady;
				if (iShowErrors)
					CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERROPENZIP);
			}
		}
		else
		{
			iError = KErrNotReady;
			if (iShowErrors)
				CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRINITZIP);
		}
		delete iZipFile;
	}
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRZIPMISSING);
	CEikonEnv::Static()->BusyMsgCancel();
/*
	else
	{
		if (iShowErrors)
			CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_CORRUPTZIP);
		iError = KErrCorrupt;	
	}
*/
	return iError;
}

// Note: aExtractPath MUST be terminated with a backslash!!
TInt CUnzip::ExtractOneFile(CZipFileMember* aZipFileMember, CZipFile* aZipFile, TFileName* aOutputFileName, TPath *aExtractPath, TBool iShowErrors)
{
	TInt iError;

	TFileName iOutputFileWithPath;
	iOutputFileWithPath.Copy(*aExtractPath);
	// Note: aExtractPath MUST be terminated with a backslash!!
	iOutputFileWithPath.Append(*aOutputFileName);
	
	iError = CEikonEnv::Static()->FsSession().MkDirAll(*aExtractPath);
	if ((iError == KErrNone) || (iError = KErrAlreadyExists))
	{	
		RZipFileMemberReaderStream *iFileStream;
		TRAP(iError, aZipFile->GetInputStreamL(aZipFileMember, iFileStream));
		if (iError == KErrNone)
		{
			HBufC8 *iReadBuffer = HBufC8::NewL(FILE_BUFFER_SIZE);
			TUint32 iFileSize = aZipFileMember->UncompressedSize();
			RFile iOutputFile;
			iError = iOutputFile.Replace(CEikonEnv::Static()->FsSession(), iOutputFileWithPath, EFileShareAny | EFileWrite);
			if (iError == KErrNone)
			{
				while (iFileSize > 0)
				{
					if (iFileSize > FILE_BUFFER_SIZE)
					{
						TPtr8 iPtr = TPtr8(iReadBuffer->Des());
						TDes8 *iDes = &iPtr;
						TRAP(iError, iFileStream->ReadL(*iDes, FILE_BUFFER_SIZE));
					}
					else
					{
						TPtr8 iPtr = TPtr8(iReadBuffer->Des());
						TDes8 *iDes = &iPtr;
						TRAP(iError, iFileStream->ReadL(*iDes, iFileSize));
					}
					if (iError == KErrNone)
					{
						iError = iOutputFile.Write(iReadBuffer->Des());
						if (iError != KErrNone)
						{
							if (iShowErrors)
								CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_ERRORWRITEFILE);
							break;
						}
						iFileSize -= iReadBuffer->Size();
					}
					else
					{
						if (iShowErrors)
							CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRREADZIP);
						EikFileUtils::DeleteFile(iOutputFileWithPath);
						break;
					}
				}
				if (iFileSize == 0)
					iError = KErrNone;
					
				delete iReadBuffer;
				iOutputFile.Close();
			}
			else
			{
				if (iShowErrors)
					CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRCREATEFILE);
			}
			delete iFileStream;
		}
		else
		{
			if (iShowErrors)
				CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_ERRREADZIP);
		}		
	}
	else
	{
		if (iShowErrors)
			CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_ERRMKDIR);
	}
	return iError;
}
