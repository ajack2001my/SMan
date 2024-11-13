#include <e32base.h>

#if !defined(__REFRESHTASKLIST_H__)
#define __REFRESHTASKLIST_H__

// Misc settings
#define REFRESH_DELAY_IF_NOT_FORCE_ENDTASK 1

/*************************************************************
*
* This class refreshes the task list after a pre-determined
* time. This is used by the gentle termination algorithm
*
**************************************************************/

class CRefreshTaskList : public CTimer
{
public:
	CRefreshTaskList();
	void RunL();
	void startRefresh(TInt delay);
protected:
	TInt localDelay;
 	TTime startTime;
};

#endif