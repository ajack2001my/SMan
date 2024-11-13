#include "marquee.h"

/*************************************************************
*
* Marquee class
*
**************************************************************/

CMarquee::CMarquee() : CTimer(EPriorityNormal)
{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
}

void CMarquee::RunL()
{
	TPtrC *tempText;
	TBuf<KMaxPath + 8> tempPath;
	
	tempText = (TPtrC*)(localLabel->Label()->Text());
	if (localText->Length() > 20)
	{
		tempPath.Copy(tempText->Mid(1, tempText->Length() - 1));
		if (tempText->Length() == localText->Length())
			tempPath.Append(_L("        "));
		tempPath.Append(tempText->Left(1));
		localLabel->SetTextL(tempPath);
		localLabel->DrawNow();
	}
	After(400000);
}

void CMarquee::startMarquee(CEikTextButton *marqueeLabel, TPath *scrollText)
{
	localLabel = marqueeLabel;
	localText = scrollText;
	After(400000);
}

