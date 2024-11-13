#include "refreshtasklist.h"
#include "config.h"
#include "sman.h"

/*************************************************************
*
* Task refresher class
*
**************************************************************/

CRefreshTaskList::CRefreshTaskList() : CTimer(EPriorityNormal)
{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
}

void CRefreshTaskList::RunL()
{
	TTime currentTime;
	TTimeIntervalSeconds interval;
	
	currentTime.HomeTime();
	if (currentTime.SecondsFrom(startTime, interval) != 0)
		interval = CONFIG_NO_DEVICE_PAUSE + 1;
	if (interval.Int() >= localDelay)
		static_cast<CSMan2AppUi*>(CEikonEnv::Static()->EikAppUi())->iTaskView->refreshTaskList(TODO_REFRESH);
	else
		After(800000);
}

void CRefreshTaskList::startRefresh(TInt delay)
{
	localDelay = delay;
	startTime.HomeTime();
	After(800000);
}

