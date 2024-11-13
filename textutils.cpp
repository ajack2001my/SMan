#include "textutils.h"

/*************************************************************
*
* My own implementation of TFileText. This properly supports 8bit
* and 16bit conversion!! This class also does not have the 
* 256 chars per line limitation. This class can read a maximum of
* 100 characters at a time until the end of line. The caller is
* responsible for ensuring a buffer of sufficient size.
*
* It also exposes one property that contains the percentage of
* the text file that has been read. This property is only valid
* after a call to ReadLine.
*
* The string it returns is DEVOID of the \r\n!!!!
*
**************************************************************/

TInt CTextFileReader::ReadLine(TDes *aReadBuffer)
{
	TInt iRetVal = KErrGeneral;
	
	if (iFileSize > -1)
	{
		iInternalBuffer.Zero();
		aReadBuffer->Zero();

		while (iFile->Read(iInternalBuffer, iInternalBuffer.MaxLength()) == KErrNone)
		{
			// Did we reach the end of the file?
			if (iInternalBuffer.Length() == 0)
			{
				iRetVal = KErrEof;
				break;
			}
				
			TInt iLineTerminatorPos = iInternalBuffer.Find(_L8("\r\n"));
			if (iLineTerminatorPos >= 0)
			{
				if ((aReadBuffer->Length() + iLineTerminatorPos) <= aReadBuffer->MaxLength())
				{
					iInternalConverterBuffer.Copy(iInternalBuffer.Left(iLineTerminatorPos));
					aReadBuffer->Append(iInternalConverterBuffer);
					iLineTerminatorPos = -(iInternalBuffer.Length() - iLineTerminatorPos) + 2;
					if (iFile->Seek(ESeekCurrent, iLineTerminatorPos) == KErrNone)
					{
						iCurrentCursorOffset = iLineTerminatorPos;
						iRetVal = KErrNone;
					}
				}
				else
					iRetVal = KErrOverflow;
				break;
			}
			else
			{
				// Couldn't find the terminator. Keep reading.
				if ((aReadBuffer->Length() + iInternalBuffer.Length()) <= aReadBuffer->MaxLength())
				{
					iInternalConverterBuffer.Copy(iInternalBuffer);
					aReadBuffer->Append(iInternalConverterBuffer);
					iCurrentCursorOffset += iInternalBuffer.Length();
				}
				else
					iRetVal = KErrOverflow;
			}
		}
		//iCurrentCursorOffset = 0;
		//if (iFile->Seek(ESeekCurrent, iCurrentCursorOffset) == KErrNone)
		iPercentRead = TInt((TReal(iCurrentCursorOffset) / TReal(iFileSize) * TReal(100) + 0.5));
	}
	
	return iRetVal;
}

TInt CTextFileReader::WriteLine(TDes *aWriteBuffer)
{
	TInt iRetVal = KErrGeneral;
	
	HBufC8 *iConvBuffer = HBufC8::NewL(aWriteBuffer->Length() * 2);
	iConvBuffer->Des().Copy(*aWriteBuffer);
	TPtr8 iPtr = iConvBuffer->Des();
	if (iFile->Write(iPtr) == KErrNone)
	{
		if (iFile->Write(_L8("\r\n")) == KErrNone)
			iRetVal = KErrNone;
	}
	delete iConvBuffer;
	
	return iRetVal;
}

void CTextFileReader::Set(RFile *aFile)
{
	iFile = aFile;
	if (iFile->Size(iFileSize) != KErrNone)
		iFileSize = -1;
}

/*************************************************************
*
* My CSV parser. :)
*
* Note: iColumnToExtract starts from 0 (as per C/C++ convention)
* 
*************************************************************/

// Checks if a given character is a single quote. Returns:
// 0 = yes, is single quote
// 1 = no, current pos is quote but has quote before
// 2 = no, current pos is quote but has quote after
// 3 = no, current pos is not quote
// 4 = no, current pos is quote but has quote before and after
TInt CCSVLineParser::IsSingleQuote(TInt aOffsetLocation, TDesC *aBuffer)
{
	TInt iRetVal = 3;
	TBool iCurrentIsQuote = EFalse, iPrevIsQuote = EFalse, iNextIsQuote = EFalse;
		
	if (aBuffer->Length() > 0)
	{
		// Check current char pos
		if (aBuffer->Mid(aOffsetLocation, 1).Compare(_L("\"")) == 0)
			iCurrentIsQuote = ETrue;
			
		// Check next char pos
		if (aOffsetLocation < (aBuffer->Length() - 1))
		{
			if (aBuffer->Mid(aOffsetLocation + 1, 1).Compare(_L("\"")) == 0)
				iNextIsQuote = ETrue;
		}
		
		// Check prev char pos
		if (aOffsetLocation > 0)
		{
			if (aBuffer->Mid(aOffsetLocation - 1, 1).Compare(_L("\"")) == 0)
				iPrevIsQuote = ETrue;
		}
		
		if (iCurrentIsQuote)
		{
			if ((iPrevIsQuote) && (iNextIsQuote))
				iRetVal = 4;
			else if (iPrevIsQuote)
				iRetVal = 1;
			else if (iNextIsQuote)
				iRetVal = 2;
			else
				iRetVal = 0;
		}
	}

	return iRetVal;
}

// This function assumes the caller has already done all the necessary trimming to remove
// leading spaces. It will always assume the FIRST character MUST be a double quote
// It returns aEndQuotePos denoting the end char position of the quote. 
// This char position is inclusive of the quote location
TInt CCSVLineParser::FindStartEndQuotes(TInt *aEndQuotePos, TDesC *aBuffer)
{
	TInt iRetVal = KErrNotFound;
	
	if (aBuffer->Left(1).Compare(_L("\"")) == 0)
	{
		TInt iCharToInspect = 1;
		TInt iQuoteSearchResult;
		
		while (iCharToInspect < aBuffer->Length())
		{
			iQuoteSearchResult = IsSingleQuote(iCharToInspect, aBuffer);
			if ((iQuoteSearchResult == 2) || (iQuoteSearchResult == 4))
				iCharToInspect++;
			else if ((iQuoteSearchResult == 0) || (iQuoteSearchResult == 1))
			{
				*aEndQuotePos = iCharToInspect;
				iRetVal = KErrNone;
				break;
			}
			iCharToInspect++;
		}
	}
	return iRetVal;
}

// Returns the text for a particular column given a line of CSV text. The text returned is
// stripped of the enclosing double quotes.
// It is assumed there is no \r\n at the end of the text line
TInt CCSVLineParser::GetColumn(TDesC *aInputBuffer, TDes *aOutputBuffer, TInt iColumnToExtract)
{
	TInt iRetVal = KErrGeneral;
	
	if (iColumnToExtract < 0)
		iRetVal = KErrArgument;
	else
	{
		if (aInputBuffer->Length() > 0)
		{
			HBufC *iTempBuffer = HBufC::NewL(aInputBuffer->Length());
			
			// Trim leading and trailing spaces
			iTempBuffer->Des().Copy(*aInputBuffer);
			iTempBuffer->Des().Trim();
			
			TPtr iPtr = iTempBuffer->Des();
			TDes *iTempDes = &iPtr;
			TInt iEndPos = 0;
			TBool iFoundText = ETrue;
			
			do
			{
				if (FindStartEndQuotes(&iEndPos, iTempDes) == KErrNone)
				{
					if (iColumnToExtract > 0)
						iTempDes->Delete(0, iEndPos + 1);
				}
				else
				{
					iFoundText = EFalse;
					break;
				}
				
				// Check for comma
				if (iColumnToExtract > 0)
				{
					iTempDes->Trim();
					if (iTempDes->Length() > 0)
					{
						if (iTempDes->Left(1).Compare(_L(",")) == 0)
						{
							iTempDes->Delete(0, 1);
							iTempDes->Trim();
						}
						else
						{
							iFoundText = EFalse;
							break;
						}
					}
				}
				iColumnToExtract--;
			} while (iColumnToExtract >= 0);

			if (iFoundText)
			{
				iRetVal = KErrNone;
				// -2 to compensate for the double quote at the beginning and at the end
				// of the string
				if ((iEndPos - 1 + aOutputBuffer->Length() - 2) <= aOutputBuffer->MaxLength())
					aOutputBuffer->Copy(iTempBuffer->Des().Mid(1, iEndPos - 1));
				else
					iRetVal = KErrOverflow;
			}
			delete iTempBuffer;
		}
		else
			iRetVal = KErrNotFound;
	}
	
	return iRetVal;
}

// Searches a string and replaces all occurrences of aStrToReplace with aReplacementStr
void CCSVLineParser::ReplaceAll(TDes *aTheString, TPtrC aStrToReplace, TPtrC aReplacementStr)
{
	HBufC *iTempBuffer = HBufC::NewL(aTheString->Length() * 2);
	iTempBuffer->Des().Zero();
	for (int i = 0; i < aTheString->Length(); i++)
	{
		if ((i + aStrToReplace.Length() - 1) < aTheString->Length())
		{
			if (aTheString->Mid(i, aStrToReplace.Length()).Compare(aStrToReplace) == 0)
			{
				iTempBuffer->Des().Append(aReplacementStr);
				i += aStrToReplace.Length() - 1;
			}
			else
				iTempBuffer->Des().Append(aTheString->Mid(i, 1));
		}
		else
			iTempBuffer->Des().Append(aTheString->Mid(i, 1));
	}
	aTheString->Copy(*iTempBuffer);
	delete iTempBuffer;
	
	/* Even though this code below is shorter, it is both more inefficient and riskier (it has
	// the possibility of getting stuck in an infinite loop until you run out of space on 
	// aTheString)
	if (aTheString->Length() > 0)
	{
		TInt iStartLocation = aTheString->Find(aStrToReplace);
		while (iStartLocation > 0)
		{
			aTheString->Replace(iStartLocation, aStrToReplace.Length(), aReplacementStr);
			iStartLocation = aTheString->Find(aStrToReplace);
		}
	}
	*/
}
