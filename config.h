#if !defined(__CONFIG_H__)
#define __CONFIG_H__

// Stuff for the "Start" menu
#define NUM_SHORTCUTS 7

// Number of last run history
#define NUM_LASTRUN_HISTORY 3

// Default config values and config file version information
#define VER_MAJOR 1
#define VER_MINOR 1
#define CONFIG_AUTOSAVE ETrue
#define CONFIG_HOTKEY_DEFAULT 2	// Internet button. See sCaptureKeyCodes in sman.h
#define CONFIG_HOTKEY_ENABLED ETrue
#define CONFIG_NO_DEVICE_PAUSE 60
#define CONFIG_NO_DEVICE_ATTEMPTS 3
#define CONFIG_RESOLVE_HOSTNAME ETrue
#define CONFIG_SHOW_BLUEJACK_SYSMSG ETrue
#define CONFIG_VIBRA_NOTIFY EFalse
#define CONFIG_PROX_SWEEP_ONCE ETrue
#define CONFIG_FLIPFLUSH EFalse
#define CONFIG_FORCE_ENDTASK EFalse
#define CONFIG_SHOW_HIDDEN_TASKS EFalse
#define CONFIG_DECAY_TIME 30
#define CONFIG_OBEX_TIMEOUT 10
#define CONFIG_PERSIST EFalse
#define CONFIG_BEAM_PROTOCOL CBlueBeam::protocolBluetooth		// enum in bjackview.h
#define CONFIG_ZOOM_MAIN ZOOM_INCREMENT * 2
#define CONFIG_ZOOM_FILE ZOOM_INCREMENT
#define CONFIG_ZOOM_BJACK ZOOM_INCREMENT
#define CONFIG_ZOOM_DB ZOOM_INCREMENT * 2
#define CONFIG_ZOOM_SYSINFO ZOOM_INCREMENT
#define CONFIG_FILEMAN_SHOWHIDDEN EFalse
#define CONFIG_AUTO_RUNAPP EFalse
#define CONFIG_AUTO_OPENFILE EFalse
#define CONFIG_FN_ACTIVATED_MENUS 0xFFFF
#define CONFIG_DB_SORT_ASCENDING ETrue
#define CONFIG_ZOOM_AGENDA ZOOM_INCREMENT * 2
#define CONFIG_FLUSH_EXCLUDE EFalse
// These two mean: by default do you want to see items that have not been completed?
// they don't mean "do you want to see the COMPLETE calendar/todo?" :)
#define CONFIG_AGENDA_SHOW_COMPLETE_CALENDAR EFalse
#define CONFIG_AGENDA_SHOW_COMPLETE_TODO EFalse

#define CONFIG_AGENDA_AUTOEXPAND_CALENDAR ETrue
#define CONFIG_AGENDA_AUTOEXPAND_TODO ETrue
#define CONFIG_AGENDA_TODO_MAXPRIORITY KMaxTUint
#define CONFIG_FN_DEFAULT_VIEW CSMan2AppUi::EViewTask
#define CONFIG_FILEMAN_SORTBY ESortByName
#define CONFIG_FILEMAN_SORTORDER EAscending
#define CONFIG_DTMF_INPUT EFalse
#define CONFIG_HEXEDIT_READONLY ETrue
#define CONFIG_ZOOM_FINDFILE ZOOM_INCREMENT
#define CONFIG_HEXEDIT_PRESERVEDATE EFalse
#define CONFIG_CELLAREA_ALWAYS_RUN EFalse
#define CONFIG_CELLAREA_EVENT_FIRING_DELAY_IN_MINUTES 0
#define CONFIG_LAST_UNKNOWN_PHONE_CHECK_DATE TTime(0)

// 0 = <disabled>
#define CONFIG_HOTKEY_INFOCUS_SHORT_FO 20	// Task Manager view
#define CONFIG_HOTKEY_INFOCUS_LONG_FO 16	// File Manager view
#define CONFIG_HOTKEY_EXFOCUS_SHORT_FO 9	// Navigate
#define CONFIG_HOTKEY_EXFOCUS_LONG_FO 11	// Fix applist

#define CONFIG_HOTKEY_INFOCUS_SHORT_FC 14	// Bluejack view
#define CONFIG_HOTKEY_INFOCUS_LONG_FC 18	// Network info view
#define CONFIG_HOTKEY_EXFOCUS_SHORT_FC 9	// Navigate
#define CONFIG_HOTKEY_EXFOCUS_LONG_FC 11	// Fix applist

// This value is ultimately divided by 10 before being multiplied into the height of the
// screen
#define CONFIG_HEXEDIT_WINDOWSIZEPROPORTION 6

/*************************************************************
*
* Config class - holds user configurable options
*
**************************************************************/

class CConfig
{
public:
	TInt iHotkeyDialogResult;
	TUid shortCutUid[NUM_SHORTCUTS];
	TBool autoSave;
	TBool hotkeyEnabled;
	TBool resolveName;
	TBool showBluejackSysMsg;
	TBool proxSweepOnce;
	TBool flipFlush;
	TBool forceEndTask;
	TInt noDevicePause;
	TBool showHiddenTasks;
	TInt noDeviceAttempts;
	TInt decayTime;
	TUint logFlags;
	TInt obexTimeout;
	TBool vibraNotify;
	TFileName audioFileName;
	TBool persist;
	TUint beamProtocol;
	TBool showHiddenFiles;
	TInt zoomMain;
	TInt zoomFile;
	TInt zoomBJack;
	TInt zoomDB;
	TInt zoomSysInfo;
	TBool autoRunApp;
	TBool autoOpenFile;
	TUint fnActivatedMenus;		// Each bit represents each menu item
	TBool dbSortAscending;		// True = sort ascending in list view, False = sort descending
	TInt zoomAgenda;
	TBool agendaShowCompleteCalendar;
	TBool agendaShowCompleteTodo;
	TBool agendaAutoExpandCalendar;
	TBool agendaAutoExpandTodo;
	TInt defaultView;
	TInt filemanSortBy;
	TInt filemanSortOrder;
	TBool iDTMFInput;
	TBool iHexEditIsReadOnly;
	TInt iHexWindowSizeProportion;
	TInt zoomFindFile;
	TBool iHexEditPreserveDate;
	TBool iCellAreaAlwaysRun;
	TInt iHotkeyInFocusShort_FO;
	TInt iHotkeyInFocusLong_FO;
	TInt iHotkeyExFocusShort_FO;
	TInt iHotkeyExFocusLong_FO;
	TInt iHotkeyInFocusShort_FC;
	TInt iHotkeyInFocusLong_FC;
	TInt iHotkeyExFocusShort_FC;
	TInt iHotkeyExFocusLong_FC;
	TUint iTodoMaxPriority;
	TBool iFlushExclude;
	TInt iCellEventFiringDelayInMinutes;
	TTime iLastUnknownPhoneCheckDate;
};

/*************************************************************
*
* Version of config file
*
**************************************************************/

class CVerInfo
{
public:
	TInt majorVersion;
	TInt minorVersion;
};

#endif