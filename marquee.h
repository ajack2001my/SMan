#include <e32base.h>
#include <eikcmbut.h>
#include <eiklabel.h>

#if !defined(__MARQUEE_H__)
#define __MARQUEE_H__

/*************************************************************
*
* This is the marquee that will scroll the current folder path
* display in the file browser view
*
**************************************************************/

class CMarquee : public CTimer
{
public:
	CMarquee();
	void RunL();
	void startMarquee(CEikTextButton *marqueeLabel, TPath *scrollText);
protected:
	CEikTextButton *localLabel;
	TPath *localText;
};

#endif