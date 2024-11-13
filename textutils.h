#include <f32file.h>
#include <e32std.h>

#if !defined(__TEXTUTILS_H__)
#define __TEXTUTILS_H__

/*************************************************************
*
* My own implementation of TFileText. This properly supports 8bit
* and 16bit conversion!! This class also does not have the 
* 256 chars per line limitation
*
* Internally, this class reads the text file in chunks of 100  
* bytes at a time. Unlike Symbian's TFileText, this class does
* not have the 256 line length limitation. As long as you supply
* a read / write buffer that is large enough, it will process
* everything accordingly
*
**************************************************************/

class CTextFileReader
{
public:
	TInt ReadLine(TDes *aReadBuffer);
	TInt WriteLine(TDes *aWriteBuffer);
	void Set(RFile *aFile);
	TInt iPercentRead;
	
private:
	TInt iCurrentCursorOffset;
	TInt iFileSize;
	RFile *iFile;
	TBuf8<100> iInternalBuffer;
	TBuf<100> iInternalConverterBuffer;
};

/*************************************************************
*
* My CSV parser. :)
* 
* Given a line of text, you tell it to extract whichever column
* you want. The CSV file is assumed to be delimited by commas.
* Fields are qualified by double-quotes. A double-quote in the
* data itself is represented by two double-quotes.
* 
* This class isn't fussy about spaces. They are ignored when 
* parsing delimiters (double quotes and commas).
*
* Any \r\n at the end of the file is ignored
*
* Note: iColumnToExtract starts from 0 (as per C/C++ convention)
*
**************************************************************/

class CCSVLineParser
{
public:
	TInt GetColumn(TDesC *aInputBuffer, TDes *aOutputBuffer, TInt iColumnToExtract);
	void ReplaceAll(TDes *aTheString, TPtrC aStrToReplace, TPtrC aReplacementStr);
	
private:
	TInt IsSingleQuote(TInt aOffsetLocation, TDesC *aBuffer);
	TInt FindStartEndQuotes(TInt *aEndQuotePos, TDesC *aBuffer);

};

#endif