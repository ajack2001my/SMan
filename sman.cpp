#include "SMan.h"
#include "fileutils.h"
#include <SMan.rsg>
#include <cntdb.h>
#include "textutils.h"

/*************************************************************
*
* Application object
*
**************************************************************/

CApaDocument* CSMan2Application::CreateDocumentL()
{
	CSMan2Document* doc = new (ELeave) CSMan2Document(*this);
	CleanupStack::PushL(doc);
	doc->ConstructL();
	CleanupStack::Pop();
	return doc;
}

/*************************************************************
*
* Document object
*
**************************************************************/

CSMan2Document* CSMan2Document::NewL(CEikApplication& aApp)
{
	CSMan2Document* self = new (ELeave) CSMan2Document(aApp);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CSMan2Document::CSMan2Document(CEikApplication& aApp)
		: CQikDocument(aApp)
{
}

CSMan2Document::~CSMan2Document()
{
}

TInt CSMan2Document::loadConfig(void)
{
	RFile inStream;
	TInt retVal;
	
	retVal = KErrGeneral;
	if (inStream.Open(CEikonEnv::Static()->FsSession(), configFileName, EFileRead) == KErrNone)
	{
		TInt fileSize;
		
		inStream.Size(fileSize);
		if (fileSize >= (TInt)sizeof(verData))
		{
			CVerInfo verData;
			
			// Check config version. If correct, load it else ignore it
			
			TPckg<CVerInfo> pkgVerData(verData);
			if (inStream.Read(pkgVerData) == KErrNone)
			{
				if ((verData.majorVersion == VER_MAJOR) && (verData.minorVersion == VER_MINOR))
				{
					TPckg<CConfig> pkgConfigData(configData);
					if (inStream.Read(pkgConfigData) == KErrNone)
					{
						retVal = KErrNone;
						configData.hotkeyEnabled = CONFIG_HOTKEY_ENABLED;
					}
				}
			}
		}
		inStream.Close();
	}
	return retVal;
}

void CSMan2Document::saveConfig(void)
{
	RFile outStream;
	TInt retVal;
	
	retVal = KErrGeneral;
	if (outStream.Replace(CEikonEnv::Static()->FsSession(), configFileName, EFileRead) == KErrNone)
	{
		verData.majorVersion = VER_MAJOR;
		verData.minorVersion = VER_MINOR;
		TPckg<CVerInfo> pkgVerData(verData);
		if (outStream.Write(pkgVerData) == KErrNone)
		{
			TPckg<CConfig> pkgConfigData(configData);
			if (outStream.Write(pkgConfigData) == KErrNone)
			{
				if (outStream.Flush() == KErrNone)
					retVal = KErrNone;
			}
		}
		outStream.Close();
	}
	if (retVal != KErrNone)
		CEikonEnv::Static()->InfoMsg(R_TBUF_ERRSAVECONFIG);
}

void CSMan2Document::ConstructL()
{
	for (int i = 0; i < NUM_SHORTCUTS; i++)
		configData.shortCutUid[i] = KNullUid;
	configData.iHotkeyDialogResult = CONFIG_HOTKEY_DEFAULT;
	configData.autoSave = CONFIG_AUTOSAVE;
	configData.hotkeyEnabled = CONFIG_HOTKEY_ENABLED;
	configData.resolveName = CONFIG_RESOLVE_HOSTNAME;
	configData.showBluejackSysMsg = CONFIG_SHOW_BLUEJACK_SYSMSG;
	configData.proxSweepOnce = CONFIG_PROX_SWEEP_ONCE;
	configData.flipFlush = CONFIG_FLIPFLUSH;
	configData.forceEndTask = CONFIG_FORCE_ENDTASK;
	configData.noDevicePause = CONFIG_NO_DEVICE_PAUSE;
	configData.noDeviceAttempts = CONFIG_NO_DEVICE_ATTEMPTS;
	configData.showHiddenTasks = CONFIG_SHOW_HIDDEN_TASKS;
	configData.decayTime = CONFIG_DECAY_TIME;
	configData.logFlags = CLogger::logAll;
	configData.obexTimeout = CONFIG_OBEX_TIMEOUT;
	configData.vibraNotify = CONFIG_VIBRA_NOTIFY;
	configData.audioFileName.Copy(_L(""));
	configData.persist = CONFIG_PERSIST;
	configData.beamProtocol = CONFIG_BEAM_PROTOCOL;
	configData.showHiddenFiles = CONFIG_FILEMAN_SHOWHIDDEN;
	configData.zoomMain = CONFIG_ZOOM_MAIN;
	configData.zoomFile = CONFIG_ZOOM_FILE;
	configData.zoomBJack = CONFIG_ZOOM_BJACK;
	configData.zoomDB = CONFIG_ZOOM_DB;
	configData.zoomSysInfo = CONFIG_ZOOM_SYSINFO;
	configData.autoRunApp = CONFIG_AUTO_RUNAPP;
	configData.autoOpenFile = CONFIG_AUTO_OPENFILE;
	configData.fnActivatedMenus = CONFIG_FN_ACTIVATED_MENUS;
	configData.dbSortAscending = CONFIG_DB_SORT_ASCENDING;
	configData.zoomAgenda = CONFIG_ZOOM_AGENDA;
	configData.agendaShowCompleteCalendar = CONFIG_AGENDA_SHOW_COMPLETE_CALENDAR;
	configData.agendaShowCompleteTodo = CONFIG_AGENDA_SHOW_COMPLETE_TODO;
	configData.agendaAutoExpandCalendar = CONFIG_AGENDA_AUTOEXPAND_CALENDAR;
	configData.agendaAutoExpandTodo = CONFIG_AGENDA_AUTOEXPAND_TODO;
	configData.defaultView = CONFIG_FN_DEFAULT_VIEW;
	configData.filemanSortBy = CONFIG_FILEMAN_SORTBY;
	configData.filemanSortOrder = CONFIG_FILEMAN_SORTORDER;
	configData.iDTMFInput = CONFIG_DTMF_INPUT;
	configData.iHexEditIsReadOnly = CONFIG_HEXEDIT_READONLY;
	configData.iHexWindowSizeProportion = CONFIG_HEXEDIT_WINDOWSIZEPROPORTION;
	configData.zoomFindFile = CONFIG_ZOOM_FINDFILE;
	configData.iHexEditPreserveDate = CONFIG_HEXEDIT_PRESERVEDATE;
	configData.iCellAreaAlwaysRun = CONFIG_CELLAREA_ALWAYS_RUN;
	configData.iHotkeyInFocusShort_FO = CONFIG_HOTKEY_INFOCUS_SHORT_FO;
	configData.iHotkeyInFocusLong_FO = CONFIG_HOTKEY_INFOCUS_LONG_FO;
	configData.iHotkeyExFocusShort_FO = CONFIG_HOTKEY_EXFOCUS_SHORT_FO;
	configData.iHotkeyExFocusLong_FO = CONFIG_HOTKEY_EXFOCUS_LONG_FO;
	configData.iHotkeyInFocusShort_FC = CONFIG_HOTKEY_INFOCUS_SHORT_FC;
	configData.iHotkeyInFocusLong_FC = CONFIG_HOTKEY_INFOCUS_LONG_FC;
	configData.iHotkeyExFocusShort_FC = CONFIG_HOTKEY_EXFOCUS_SHORT_FC;
	configData.iHotkeyExFocusLong_FC = CONFIG_HOTKEY_EXFOCUS_LONG_FC;
	configData.iTodoMaxPriority = CONFIG_AGENDA_TODO_MAXPRIORITY;
	configData.iFlushExclude = CONFIG_FLUSH_EXCLUDE;
	configData.iCellEventFiringDelayInMinutes = CONFIG_CELLAREA_EVENT_FIRING_DELAY_IN_MINUTES;
	configData.iLastUnknownPhoneCheckDate = CONFIG_LAST_UNKNOWN_PHONE_CHECK_DATE;

	// Config cannot be loaded now because we need the full path of the application and
	// this is only available after the application is constructed. Therefore, config
	// loading happens in the App class's ConstructL() member
}

CQikAppUi* CSMan2Document::CreateAppUiL()
{
	return new (ELeave) CSMan2AppUi(&configData);
}

/*************************************************************
*
* AppUi object
*
**************************************************************/

CSMan2AppUi::CSMan2AppUi(CConfig *cData)
{
	configData = cData;
}

CSMan2AppUi::~CSMan2AppUi()
{
	configData->hotkeyEnabled = EFalse;
	iTaskView->ToggleHotKey();

	if (iSysInfoView)
	{
		DeregisterView(*iSysInfoView);
		delete iSysInfoView;
		iSysInfoView = NULL;
	}
	if (iDBView)
	{
		DeregisterView(*iDBView);
		delete iDBView;
		iDBView = NULL;
	}
	if (iDTMFDialerView)
	{
		DeregisterView(*iDTMFDialerView);
		delete iDTMFDialerView;
		iDTMFDialerView = NULL;
	}
	if (iHexEditorView)
	{
		DeregisterView(*iHexEditorView);
		delete iHexEditorView;
		iHexEditorView = NULL;
	}
	if (iAgendaView)
	{
		DeregisterView(*iAgendaView);
		delete iAgendaView;
		iAgendaView = NULL;
	}
	if (iFindFileView)
	{
		DeregisterView(*iFindFileView);
		delete iFindFileView;
		iFindFileView = NULL;
	}
	if (iBJackView)
	{
		DeregisterView(*iBJackView);
		delete iBJackView;
		iBJackView = NULL;
	}
	//DeregisterView(*iAgendaView);
	//DeregisterView(*iFindFileView);
	//DeregisterView(*iBJackView);

	DeregisterView(*iTaskView);
	DeregisterView(*iFileView);
	DeregisterView(*iFCBJackView);
	DeregisterView(*iCellAreaView);
	DeregisterView(*iFCCellAreaView);
	
	//delete iAgendaView;
	//delete iFindFileView;
	//delete iBJackView;

	delete iTaskView;
	delete iFileView;
	delete iFCBJackView;
	delete iCellAreaView;
	delete iFCCellAreaView;
	
	delete startUidList;
	delete historyUidList;
}

void CSMan2AppUi::HandleForegroundEventL(TBool aForeground)
{
	CQikAppUi::HandleForegroundEventL(aForeground);
	appInForeground = aForeground;
	iFCViewSwitchIsInternal = EFalse;
	// Workaround for SetDefaultViewL() bug in FC mode :(
	if ((iCoeEnv->ScreenDevice()->CurrentScreenMode() == 1) && (appInForeground))
	{
		/* 
		The bluejack view in flip open is instantiated only when needed.
		This call with a custom message id and message data (although this is not used)
		tells the ViewActivatedL handler of the bluejack flip closed view that
		WE WANT THE FLIP OPEN BLUEJACK VIEW TO BE INSTANTIATED.
		If the flipclosed bluejack view never sees this uid number, it will never instantiate
		the flipopen bluejack view because it knows this is a temporary system request
		*/
		if (iCurrentFCViewUid != KUidFCBJackView)
			ActivateViewL(TVwsViewId(KUidSMan2App, iCurrentFCViewUid), KUidSMan2App, _L8(""));
	}
	
	/*
	If we had a temporary view as the default view, we must reinitialize it when sman comes
	back to the foreground. Only the temporary views that call SetDefaultViewL() on themselves
	will require this special processing. If we don't do this special processing, the UI
	framework will spit out a "Not Found" error dialog box and sman's view will become 
	inconsistent (blank)
	*/
	TVwsViewId iDefaultViewId;
	iDefaultViewId.iAppUid = KUidSMan2App;
	if (GetDefaultViewId(iDefaultViewId) == KErrNone)
	{
		if (iDefaultViewId.iViewUid == KUidBJackView)
			InitTemporaryViews(EViewBJack);
		else if (iDefaultViewId.iViewUid == KUidDTMFDialerView)
			InitTemporaryViews(EViewDTMFDialer);
	}
	else
	{
		// If an error occurs, forcefully switch back to task manager
		InitTemporaryViews(EViewTask);
	}
}

void CSMan2AppUi::InfoPrintFromResource(TInt aResourceId)
{
	HBufC *iTextBuffer = CEikonEnv::Static()->AllocReadResourceL(aResourceId);
	User::InfoPrint(*iTextBuffer);
	delete iTextBuffer;
}

void CSMan2AppUi::DoShortCuts(TInt aShortcutOperation, TInt aScreenMode)
{
	// Refer to the RSS file under
	// r_shortcut_infocus_choices for the exact case number 

	// Do manufacturer specific checks first
	if (!PhoneIsCapable(EPhone_CanSetHotkey))
	{
		return;
	}
	else if (!PhoneIsCapable(EPhone_HasIRDA))
	{
		if (aShortcutOperation == 5)
			return;
	}
	else if (!PhoneIsCapable(EPhone_HasPhoneCPL))
	{
		if (aShortcutOperation == 7)
			return;
	}
	else if (!PhoneIsCapable(EPhone_HasAppListBug))
	{
		if (aShortcutOperation == 11)
			return;
	}
	else if (!PhoneIsCapable(EPhone_HasTimeSynchZoneBug))
	{
		if (aShortcutOperation == 12)
			return;
	}
	else if (!PhoneIsCapable(EPhone_CanChangeFont))
	{
		if (aShortcutOperation == 13)
			return;
	}
	else if (!PhoneIsCapable(EPhone_HasNetworkInfoTSY))
	{
		if (aShortcutOperation == 18)
			return;
	}
	else if (!PhoneIsCapable(EPhone_HasBluetooth))
	{
		if ((aShortcutOperation == 4) || (aShortcutOperation == 14))
			return;
	}
	else if (!PhoneIsCapable(EPhone_CanAutoStart))
	{
		if ((aShortcutOperation == 1) || (aShortcutOperation == 2) || (aShortcutOperation == 3))
			return;
	}
	
	switch (aShortcutOperation)
	{
		case 0 :
			{
				 // disabled do nothing! 
				break;
			}
		case 1 :
			{
				// Autostart others
				if (aScreenMode == 0)
				{
					SetAppPosition(ETrue);
					ProcessCommandL(cmdAutostartOthers);
				}
				break;
			}
		case 2 :
			{
				// Toggle sman autostart
				SetAppPosition(ETrue);
				ProcessCommandL(cmdAutostartSMan);
				break;
			}
		case 3 :
			{
				// Toggle autostart
				SetAppPosition(ETrue);
				ProcessCommandL(cmdAutostartActive);
				break;
			}
		case 4 :
			{
				// Bluetooth control panel
				if (aScreenMode == 0)
				{
					SetAppPosition(ETrue);
					ProcessCommandL(cmdCPBluetooth);
				}
				break;
			}
		case 5 :
			{
				// Irda control panel
				if (aScreenMode == 0)
				{
					SetAppPosition(ETrue);
					ProcessCommandL(cmdCPIrda);
				}
				break;
			}
		case 6 :
			{
				// Control panel
				if (aScreenMode == 0)
					ProcessCommandL(cmdControlPanel);
				break;
			}
		case 7 :
			{
				// Phone control panel
				if (aScreenMode == 0)
				{
					//SetAppPosition(ETrue);
					ProcessCommandL(cmdCPPhone);
				}
				break;
			}
		case 8 :
			{
				// Flush procseses
				ProcessCommandL(cmdFlushOut);
				break;
			}
		case 9 :
			{
				// Navigate
				if (aScreenMode == 1)
				{
					if (appInForeground)
						InfoPrintFromResource(R_TBUF_SHORTCUT_FCINVALID);
					else
						ActivateViewL(TVwsViewId(KUidSMan2App, iCurrentFCViewUid));
				}
				else
				{
					TVwsViewId currentView;
					
					if (GetActiveViewId(currentView) != KErrNone)
						SetAppPosition(ETrue);
					else
					{
						if ((!CEikonEnv::Static()->AppUiFactory()->MenuBar()->DrawableWindow()->IsFaded()) && (appInForeground))
						{
							// Hotkey is pressed while we have focus
							TInt selectedTitle;

							if (CEikonEnv::Static()->AppUiFactory()->MenuBar()->IsFocused())
							{
								selectedTitle = CEikonEnv::Static()->AppUiFactory()->MenuBar()->SelectedTitle();
								if (selectedTitle == (CEikonEnv::Static()->AppUiFactory()->MenuBar()->MenuTitleArray()->Count() - 2))
									ProcessCommandL(EEikCmdCanceled);
								else
								{
									if (selectedTitle == (CEikonEnv::Static()->AppUiFactory()->MenuBar()->MenuTitleArray()->Count() - 1))
										selectedTitle = -1;
									CEikonEnv::Static()->AppUiFactory()->MenuBar()->MoveHighlightToL(selectedTitle + 1, -1);
								}
							}
							else
							{
								CEikonEnv::Static()->AppUiFactory()->MenuBar()->MoveHighlightToL(CEikonEnv::Static()->AppUiFactory()->MenuBar()->MenuTitleArray()->Count() - 1,  -1);
							}
						}
						else
							SetAppPosition(ETrue);
					}
				}
				break;		
			}
		case 10:
			{
				// Compress contacts
				if (aScreenMode == 0)
					SetAppPosition(ETrue);
				else
					ActivateViewL(TVwsViewId(KUidSMan2App, iCurrentFCViewUid));
				ProcessCommandL(cmdUtilsCompContacts);
				break;
			}

		case 11:
			{
				// Fix applist
				ProcessCommandL(cmdUtilsFixApplist);
				break;
			}
		case 12:
			{
				// Fix synch timezone
				if (aScreenMode == 0)
					SetAppPosition(ETrue);
				else
					ActivateViewL(TVwsViewId(KUidSMan2App, iCurrentFCViewUid));
				ProcessCommandL(cmdUtilsAgendaFixSynchTimeZone);
				break;
			}
		case 13:
			{
				// Set system font
				if (aScreenMode == 0)
				{
					SetAppPosition(ETrue);
					ProcessCommandL(cmdUtilsFontSelector);
				}
				break;
			}
		case 14:
			{
				// Bluejack
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchBJack);
				else
					ProcessCommandL(cmdSwitchFCBJackView);
				break;
			}
		case 15:
			{
				// DTMF Dialer
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchDTMFDialer);
				break;
			}
		case 16:
			{
				// File manager
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchFile);
				break;
			}
		case 17:
			{
				// Infostore
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchDB);
				break;
			}
		case 18:
			{
				// Network info
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchCellArea);
				else
					ProcessCommandL(cmdSwitchFCCellAreaView);
				break;
			}
		case 19:
			{
				// Sysinfo
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchSysInfo);
				break;
			}
		case 20:
			{
				// Task manager
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchMemory);
				break;
			}
		case 21:
			{
				// Today view
				if (aScreenMode == 0)
					ProcessCommandL(cmdSwitchAgenda);
				break;
			}
	};
	CEikonEnv::Static()->BusyMsgCancel();
}

void CSMan2AppUi::switchView(TInt viewId)
{
	// Save battery
	if (iFileView->currentPathScroller->IsActive())
		iFileView->currentPathScroller->Cancel();

	if (viewId == EViewAgenda)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_AGENDA_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));
		
		SetToolbarL(R_TOOLBAR_AGENDA);
		SetDefaultViewL(*iAgendaView);
		iCurrentFOViewUid = KUidAgendaView;
	}
	if (viewId == EViewTask)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_MEMORY_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		RemoveToolbarL();
		SetDefaultViewL(*iTaskView);
		iCurrentFOViewUid = KUidTaskView;
	}
	else if (viewId == EViewBJack)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_BLUEJACK_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		SetToolbarL(R_TOOLBAR_BLUEJACK);
		SetDefaultViewL(*iBJackView);
		iCurrentFOViewUid = KUidBJackView;

		iBJackView->oScanner->SetToolbarButtonState();
	}
	else if (viewId == EViewDB)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_DB_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		if (iDBView->currentDBView == CSMan2DBView::DBView_List)
			SetToolbarL(R_TOOLBAR_DB_LIST);
		else
			SetToolbarL(R_TOOLBAR_DB_SINGLE);
	}
	else if (viewId == EViewSysInfo)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_SYSINFO_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		RemoveToolbarL();
	}
	else if (viewId == EViewFile)
	{
		iFileView->StartScroller();

		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_FILE_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));
			
		SetToolbarL(R_TOOLBAR_FILEMAN);
		SetDefaultViewL(*iFileView);
		iCurrentFOViewUid = KUidFileView;
	}
	else if (viewId == EViewDTMFDialer)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_DTMFDIALER_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		RemoveToolbarL();
		SetDefaultViewL(*iDTMFDialerView);
		iCurrentFOViewUid = KUidDTMFDialerView;
	}
	else if (viewId == EViewHexEditor)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_HEXEDITOR_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		SetToolbarL(R_TOOLBAR_HEXEDITOR);
	}
	else if (viewId == EViewFindFile)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_FINDFILE_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		SetToolbarL(R_TOOLBAR_FINDFILE);
	}
	else if (viewId == EViewCellArea)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->ChangeMenuBarL(0, R_CELLAREA_MENUBAR, EFalse);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(ETrue);
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetPosition(TPoint(0, iOldMenuBarYPos));

		RemoveToolbarL();
		SetDefaultViewL(*iCellAreaView);
		iCurrentFOViewUid = KUidCellAreaView;
	}
	else if (viewId == EViewFCBJack)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(EFalse);
		RemoveToolbarL();
		// SetDefaultViewL() doesn't work in FC mode :(:(
		// Instead, the view that gets activated in FC mode is the first FC mode that appears
		// in the application information file (aif)
		// SetDefaultViewL(*iFCBJackView);
		if (iFCViewSwitchIsInternal)
			iCurrentFCViewUid = KUidFCBJackView;
	}
	else if (viewId == EViewFCCellArea)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->MakeVisible(EFalse);
		RemoveToolbarL();
		// SetDefaultViewL() doesn't work in FC mode :(:(
		// Instead, the view that gets activated in FC mode is the first FC mode that appears
		// in the application information file (aif)
		// SetDefaultViewL(*iFCCellAreaView);
		iCurrentFCViewUid = KUidFCCellAreaView;
	}
}

void CSMan2AppUi::UpdateSystemIcons(TInt aIconId, TBool iDoDraw)
{
	// To be sure we get this right, update ALL views.
	iTaskView->DrawSystemIcon(aIconId, iDoDraw);
	iFileView->DrawSystemIcon(aIconId, iDoDraw);
	iFCBJackView->DrawSystemIcon(aIconId, iDoDraw);
	iCellAreaView->DrawSystemIcon(aIconId, iDoDraw);
	iFCCellAreaView->DrawSystemIcon(aIconId, iDoDraw);
	
	if (iAgendaView)
		iAgendaView->DrawSystemIcon(aIconId, iDoDraw);
	if (iFindFileView)
		iFindFileView->DrawSystemIcon(aIconId, iDoDraw);
	if (iDBView)
		iDBView->DrawSystemIcon(aIconId, iDoDraw);
	if (iSysInfoView)
		iSysInfoView->DrawSystemIcon(aIconId, iDoDraw);
	if (iDTMFDialerView)
		iDTMFDialerView->DrawSystemIcon(aIconId, iDoDraw);
	if (iHexEditorView)
		iHexEditorView->DrawSystemIcon(aIconId, iDoDraw);
	if (iBJackView)
		iBJackView->DrawSystemIcon(aIconId, iDoDraw);
}

TBool CSMan2AppUi::PhoneIsCapable(TUint iCapability)
{
	TBool iIsCapable = EFalse;
	switch (iCapability)
	{
		case EPhone_HasIRDA : 
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_A925) ||
									(iRunningPhoneModel == EPhoneModel_A925_C) ||
									(iRunningPhoneModel == EPhoneModel_P910) ||
									(iRunningPhoneModel == EPhoneModel_P30))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasBluetooth :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_A925) ||
									(iRunningPhoneModel == EPhoneModel_A925_C) ||
									(iRunningPhoneModel == EPhoneModel_P910) ||
									(iRunningPhoneModel == EPhoneModel_P30))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasFlip :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_CanChangeFont :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_A920) ||
									(iRunningPhoneModel == EPhoneModel_A925) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_CanAutoStartInD :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasFixedWidthFont :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasIRDACPL :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910) ||
									(iRunningPhoneModel == EPhoneModel_P30))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasBluetoothCPL :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910) ||
									(iRunningPhoneModel == EPhoneModel_P30) ||
									(iRunningPhoneModel == EPhoneModel_A925_C) ||
									(iRunningPhoneModel == EPhoneModel_A925))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasPhoneCPL :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasAppListBug :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasTimeSynchZoneBug :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_HasNetworkInfoTSY :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_CanFixAppOrder :
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_A925) ||
									(iRunningPhoneModel == EPhoneModel_A925_C) ||
									(iRunningPhoneModel == EPhoneModel_A920) || 
									(iRunningPhoneModel == EPhoneModel_A920_C) || 
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_CanSetZoomFont : 
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_CanSetHotkey : 
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_A925) ||
									(iRunningPhoneModel == EPhoneModel_A925_C) ||
									(iRunningPhoneModel == EPhoneModel_A920) || 
									(iRunningPhoneModel == EPhoneModel_A920_C) || 
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
		case EPhone_CanAutoStart : 
							{
								if ((iRunningPhoneModel == EPhoneModel_P800) || 
									(iRunningPhoneModel == EPhoneModel_P900) ||
									(iRunningPhoneModel == EPhoneModel_A925) ||
									(iRunningPhoneModel == EPhoneModel_A925_C) ||
									(iRunningPhoneModel == EPhoneModel_A920) || 
									(iRunningPhoneModel == EPhoneModel_A920_C) || 
									(iRunningPhoneModel == EPhoneModel_P910))
									iIsCapable = ETrue;
								break;
							}
	};
	return iIsCapable;
}

void CSMan2AppUi::ConstructL()
{
	BaseConstructL();

	appInForeground = EFalse;
	iCurrentFCViewUid = KUidFCBJackView;
	iFCViewSwitchIsInternal = EFalse;
	iIsProcessingShortcut = EFalse;

	iZipFile.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName());
	iZipFile.SetLength(iZipFile.Length() - 3);
	iZipFile.Append(_L("ZIP"));
	
	// Initialize phone model number
	TInt iMachineUid;
	HAL::Get(HALData::EMachineUid, iMachineUid);
	switch (iMachineUid)
	{
		case 0x101f408b : iRunningPhoneModel = EPhoneModel_P800; break;
		case 0x101fb2ae : iRunningPhoneModel = EPhoneModel_P900; break;
		case 0x101F6B26 : iRunningPhoneModel = EPhoneModel_A920; break;
		case 0x101F6B27 : iRunningPhoneModel = EPhoneModel_A925; break;
		case 0x10200AC6 : iRunningPhoneModel = EPhoneModel_P910; break;
		case 0x101FD279 : iRunningPhoneModel = EPhoneModel_P30; break;
		case HALData::EMachineUid_Win32Emulator : iRunningPhoneModel = EPhoneModel_P900; break;
		default : iRunningPhoneModel = EPhoneModel_Unknown; break;
	}
	
	// Check if this is a chinese version of the A920. The chinese version cannot set
	// fonts...it screws up the display :(
	if ((iRunningPhoneModel == EPhoneModel_A920) || (iRunningPhoneModel == EPhoneModel_A925))
	{
		_LIT(KVerFile, "Z:\\System\\Data\\version.txt");
		RFile iVerFile;
		if (iVerFile.Open(CEikonEnv::Static()->FsSession(), KVerFile, EFileRead) == KErrNone)
		{
			CTextFileReader *iFileReader = new (ELeave) CTextFileReader;
			iFileReader->Set(&iVerFile);
			HBufC *iBuffer = HBufC::NewL(50);
			TPtr iPtr = TPtr(iBuffer->Des());
			TDes *iDes = &iPtr;

			if (iFileReader->ReadLine(iDes) == KErrNone)
			{
				if ((iBuffer->Des().Right(1).Compare(_L("0")) < 0) ||
					(iBuffer->Des().Right(1).Compare(_L("9")) > 0))
				{
					if (iRunningPhoneModel == EPhoneModel_A920)
						iRunningPhoneModel = EPhoneModel_A920_C;
					else
						iRunningPhoneModel = EPhoneModel_A925_C;
				}
			}
			delete iBuffer;
			delete iFileReader;
			iVerFile.Close();
		}
	}
	
	if ((!PhoneIsCapable(EPhone_HasBluetooth)) && (PhoneIsCapable(EPhone_HasIRDA)))
		configData->beamProtocol = CBlueBeam::protocolIrDA;

	// Note: You need to load this first because some of the views created rely on the
	// loaded values for initialization
	static_cast<CSMan2Document*>(Document())->configFileName.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName());
	static_cast<CSMan2Document*>(Document())->configFileName.SetLength(static_cast<CSMan2Document*>(Document())->configFileName.Length() - 3);
	static_cast<CSMan2Document*>(Document())->configFileName.Append(_L("cfg"));

	// Load config and initialize vars based on config
	static_cast<CSMan2Document*>(Document())->loadConfig();

	oldScreenMode = iCoeEnv->ScreenDevice()->CurrentScreenMode();
	bjackIconLabelCurrentImage = 0;

	iScreenRect = ClientRect();

	if (iRunningPhoneModel == EPhoneModel_Unknown)
	{
		TTime iCurrentTime;
		TInt iDaysDifferent;
		
		iCurrentTime.HomeTime();
		iDaysDifferent = configData->iLastUnknownPhoneCheckDate.DaysFrom(iCurrentTime).Int();
		iDaysDifferent = iDaysDifferent < 0 ? -iDaysDifferent : iDaysDifferent;
		if (iDaysDifferent > 3)
		{
			configData->iLastUnknownPhoneCheckDate.HomeTime();
			static_cast<CSMan2Document*>(Document())->saveConfig();
			CCreditsDialog *iCreditsDialog = new (ELeave) CCreditsDialog();
			iCreditsDialog->iMessageResourceID = R_TBUF_UNKNOWNPHONEMESSAGE;
			iCreditsDialog->ExecuteLD(R_DIALOG_UNKNOWNPHONEDIALOG);
		}
	}

	iTaskView = new (ELeave) CSMan2TaskListView(configData);
	iTaskView->ConstructL(ClientRect());

	iFCCellAreaView = new (ELeave) CSMan2FCCellAreaView(configData);
	iFCCellAreaView->ConstructL(ClientRect());

	iFCBJackView = new (ELeave) CSMan2FCBJackView(configData);
	iFCBJackView->ConstructL(ClientRect());

	iFileView = new (ELeave) CSMan2FileListView(configData);
	iFileView->ConstructL(ClientRect());

	iCellAreaView = new (ELeave) CSMan2CellAreaView(configData);
	iCellAreaView->ConstructL(ClientRect());

	// These views are initialized only when they are used for the first time
	/*
	iAgendaView = new (ELeave) CSMan2AgendaView(configData);
	iAgendaView->ConstructL(ClientRect());
	iFindFileView = new (ELeave) CSMan2FindFileView(configData);
	iFindFileView->ConstructL(ClientRect());
	iBJackView = new (ELeave) CSMan2BJackView(configData);
	iBJackView->ConstructL(ClientRect());
	*/

	iAgendaView = NULL;
	iFindFileView = NULL;
	iBJackView = NULL;
	iDBView = NULL;
	iSysInfoView = NULL;
	iDTMFDialerView = NULL;
	iHexEditorView = NULL;
	
	//RegisterViewL(*iAgendaView);
	//RegisterViewL(*iFindFileView);
	//RegisterViewL(*iBJackView);
	
	RegisterViewL(*iFileView);
	RegisterViewL(*iTaskView);
	RegisterViewL(*iCellAreaView);
	RegisterViewL(*iFCCellAreaView);
	RegisterViewL(*iFCBJackView);
	
	startUidList = new (ELeave) CArrayFixFlat<TUid>(3);
	historyUidList = new (ELeave) CArrayFixFlat<TUid>(NUM_LASTRUN_HISTORY);
	historyUidList->AppendL(KNullUid);
	historyUidList->AppendL(KNullUid);
	historyUidList->AppendL(KNullUid);
	
	// Set this state
	iTaskView->SetIgnoreHarakiriState();
		
	// This variable shouldn't be initialized here. It gets initialized in base class of each view
	//iViewId = configData->defaultView;

	// This is needed because in FC mode we reposition the menubar to the top of the screen
	iOldMenuBarYPos = CEikonEnv::Static()->AppUiFactory()->MenuBar()->Position().iY;
	
	switch (configData->defaultView)
	{
		case EViewTask : 
			{
				iCurrentFOViewUid = KUidTaskView;
				SetDefaultViewL(*iTaskView);
				break;
			}
		case EViewFile :
			{
				iCurrentFOViewUid = KUidFileView;
				SetDefaultViewL(*iFileView);
				iFileView->DoSwitchDrive(TChar('C'));
				break;
			}
		case EViewBJack :
			{
				if (PhoneIsCapable(EPhone_HasBluetooth))
				{
					if (iBJackView == NULL)
					{
						iBJackView = new (ELeave) CSMan2BJackView(configData);
						iBJackView->ConstructL(ClientRect());
						RegisterViewL(*iBJackView);
					}
					iCurrentFOViewUid = KUidBJackView;
					iCurrentFCViewUid = KUidFCBJackView;
					SetDefaultViewL(*iBJackView);
				}
				break;
			}
		case EViewDB :
			{
				iCurrentFOViewUid = KUidDBView;
				iDBView = new (ELeave) CSMan2DBView(configData);
				iDBView->ConstructL(ClientRect());
				RegisterViewL(*iDBView);
				
				SetDefaultViewL(*iDBView);
				break;
			}
		case EViewSysInfo :
			{
				iCurrentFOViewUid = KUidSysInfoView;
				iSysInfoView = new (ELeave) CSMan2SysInfoView(configData);
				iSysInfoView->ConstructL(ClientRect());
				RegisterViewL(*iSysInfoView);
				
				SetDefaultViewL(*iSysInfoView);
				// The ViewActivatedL member of this view already calls refreshSysInfo
				//iSysInfoView->refreshSysInfo();
				break;
			}
		case EViewAgenda :
			{
				if (iAgendaView == NULL)
				{
					iAgendaView = new (ELeave) CSMan2AgendaView(configData);
					iAgendaView->ConstructL(ClientRect());
					RegisterViewL(*iAgendaView);
				}
				iCurrentFOViewUid = KUidAgendaView;
				SetDefaultViewL(*iAgendaView);
				iAgendaView->ReadAgenda(EFalse);
				break;
			}
		case EViewDTMFDialer :
			{
				iCurrentFOViewUid = KUidDTMFDialerView;
				iDTMFDialerView = new (ELeave) CSMan2DTMFDialerView(configData);
				iDTMFDialerView->ConstructL(ClientRect());
				RegisterViewL(*iDTMFDialerView);
				
				SetDefaultViewL(*iDTMFDialerView);
				break;
			}
		case EViewCellArea:
			{
				if (PhoneIsCapable(EPhone_HasNetworkInfoTSY))
				{
					iCurrentFOViewUid = KUidCellAreaView;
					iCurrentFCViewUid = KUidFCCellAreaView;
					SetDefaultViewL(*iCellAreaView);
				}
				break;
			}
		// These views are activated from the file manager. They will never be a default view
		// and they always have a back button to return to the file manager
		// Hexeditor is not a normal view! So, no initialization for it
		// Findfile is not a normal view! So, no initialization for it
		default:
			{
				iCurrentFOViewUid = KUidTaskView;
				iCurrentFCViewUid = KUidFCBJackView;
				SetDefaultViewL(*iTaskView);
				break;
			}
	};
}

LOCAL_C void CloseLibrary(TAny* aLibrary)
{
	((RLibrary*)aLibrary)->Close();
}

void CSMan2AppUi::launchControlPanel(TPtrC appletName)
{
	RLibrary control;
	CleanupStack::PushL(TCleanupItem(CloseLibrary, &control));
	User::LeaveIfError(control.Load(appletName));
	(*(TLoadDll)control.Lookup(1))(appletName);
	CleanupStack::PopAndDestroy(); // control
}

void CSMan2AppUi::SetZoom(TInt zoomLevel)
{
	if (viewId == EViewTask)
		iTaskView->SetZoomLevel(zoomLevel);
	else if (viewId == EViewBJack)
		iBJackView->SetZoomLevel(zoomLevel);
	else if (viewId == EViewDB)
		iDBView->SetZoomLevel(zoomLevel);
	else if (viewId == EViewFile)
		iFileView->SetZoomLevel(zoomLevel);
	else if (viewId == EViewSysInfo)
		iSysInfoView->SetZoomLevel(zoomLevel);
	else if (viewId == EViewAgenda)
		iAgendaView->SetZoomLevel(zoomLevel);
	else if (viewId == EViewFindFile)
		iFindFileView->SetZoomLevel(zoomLevel);		
	// DTMF Dialer does not support zooming
	// HexEditor does not support zooming
	// Cell Area does not support zooming
	static_cast<CSMan2Document*>(Document())->saveConfig();
}

void CSMan2AppUi::doProcessCommand(TInt aCommand)
{
	ProcessCommandL(aCommand);
}

void CSMan2AppUi::doCancelMenu(void)
{
	doProcessCommand(EEikCmdCanceled);
}

TBool CSMan2AppUi::PromptAppFixOrder(TInt aPromptResourceId)
{
	if (!ConeUtils::FileExists(KAppConfigFileBackup))
		return ETrue;
	
	TBool retVal = EFalse;
	
	HBufC *title = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
	HBufC *warning = CEikonEnv::Static()->AllocReadResourceL(aPromptResourceId);
	
	TTime backupFileDateTime;
	CEikonEnv::Static()->FsSession().Modified(KAppConfigFileBackup, backupFileDateTime);
	TBuf<30> backupFileDateTimeText;
	backupFileDateTime.FormatL(backupFileDateTimeText, _L("%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B  "));
	warning = warning->ReAllocL(warning->Length() + backupFileDateTimeText.Length());
	warning->Des().Append(backupFileDateTimeText);
	
	if (CEikonEnv::Static()->QueryWinL(*title, *warning))
		retVal = ETrue;

	delete title;
	delete warning;
	return retVal;
}

void CSMan2AppUi::DeInitTemporaryViews(TInt aCurrentViewId)
{
	if ((iDBView) && (aCurrentViewId != EViewDB))
	{
		DeregisterView(*iDBView);
		delete iDBView;
		iDBView = NULL;
	}
	else if ((iSysInfoView) && (aCurrentViewId != EViewSysInfo))
	{
		DeregisterView(*iSysInfoView);
		delete iSysInfoView;
		iSysInfoView = NULL;
	}
	else if ((iHexEditorView) && (aCurrentViewId != EViewHexEditor))
	{
		DeregisterView(*iHexEditorView);
		delete iHexEditorView;
		iHexEditorView = NULL;
	}
	else if ((iDTMFDialerView) && (aCurrentViewId != EViewDTMFDialer))
	{
		DeregisterView(*iDTMFDialerView);
		delete iDTMFDialerView;
		iDTMFDialerView = NULL;
	}
	else if ((iBJackView) && (aCurrentViewId != EViewBJack))
	{
		// We will only deregister this view if there is no entry in the log. Since the log
		// can exist either in the flip open or flip closed bluejack views, we be a little 
		// smart and check the log helper class instead of the individual views. The log helper
		// class will always point to the correct global text editor log buffer of either view
		// - depending on which view is currently active
		if ((!iBJackView->oScanner->IsActive()) && (iBJackView->oScanner->logHelper->logTextBox->TextLength() < 1))
		{
			DeregisterView(*iBJackView);
			delete iBJackView;
			iBJackView = NULL;
		}
	}
}

void CSMan2AppUi::InitTemporaryViews(EViewType aViewIdToInit)
{
	switch (aViewIdToInit)
	{
		case EViewBJack :
						{
							if (!iBJackView)
							{
								iBJackView = new (ELeave) CSMan2BJackView(configData);
								iBJackView->ConstructL(iScreenRect);
								RegisterViewL(*iBJackView);		
							}
							break;
						};
		case EViewDB :
						{
							if (!iDBView)
							{
								iDBView = new (ELeave) CSMan2DBView(configData);
								iDBView->ConstructL(iScreenRect);
								RegisterViewL(*iDBView);		
							}
							break;
						};
		case EViewSysInfo : 
						{
							if (!iSysInfoView)
							{
								iSysInfoView = new (ELeave) CSMan2SysInfoView(configData);
								iSysInfoView->ConstructL(iScreenRect);
								RegisterViewL(*iSysInfoView);
							}
							break;
						};
		case EViewHexEditor :
						{
							if (!iHexEditorView)
							{
								iHexEditorView = new (ELeave) CSMan2HexEditorView(configData);
								iHexEditorView->ConstructL(iScreenRect);
								RegisterViewL(*iHexEditorView);
							}
							break;
						};
		case EViewDTMFDialer :
						{
							if (!iDTMFDialerView)
							{
								iDTMFDialerView = new (ELeave) CSMan2DTMFDialerView(configData);
								iDTMFDialerView->ConstructL(iScreenRect);
								RegisterViewL(*iDTMFDialerView);
							}
							break;
						}
		default : { break; } // Do nothing
	}
}

void CSMan2AppUi::HandleCommandL(TInt aCommand)
{
	TInt i;

	/*************************************************************
	*
	* Zoom command appears everywhere
	*
	**************************************************************/
	if (aCommand == cmdZoom)
	{
		TInt zoomState = 0;
		
		// This is faster than calling APIs....
		if (viewId == EViewTask)
			zoomState = configData->zoomMain;
		else if (viewId == EViewBJack)
			zoomState = configData->zoomBJack;
		else if (viewId == EViewDB)
			zoomState = configData->zoomDB;
		else if (viewId == EViewFile)
			zoomState = configData->zoomFile;
		else if (viewId == EViewSysInfo)
			zoomState = configData->zoomSysInfo;
		else if (viewId == EViewAgenda)
			zoomState = configData->zoomAgenda;
		else if (viewId == EViewFindFile)
			zoomState = configData->zoomFindFile;
		CQikZoomDialog::RunDlgLD((zoomState / ZOOM_INCREMENT) + EQikCmdZoomLevel1 - 1);
	}
	else if ((aCommand == EQikCmdZoomLevel1) || (aCommand == EQikCmdZoomLevel2) || (aCommand == EQikCmdZoomLevel3))
	{
		SetZoom(ZOOM_INCREMENT * (aCommand - EQikCmdZoomLevel1 + 1));
	}
	/*************************************************************
	*
	* Control panel shortcuts. Thanks to Phil Spencer of Symbian
	*
	**************************************************************/
	if (aCommand == cmdCPIrda)
	{
		if (iRunningPhoneModel == EPhoneModel_P30)
			launchControlPanel(_L("Z:\\System\\Controls\\irdasettingdlg.ctl"));
		else
			launchControlPanel(_L("Z:\\System\\Controls\\irdase.ctl"));
	}
	else if (aCommand == cmdCPBluetooth)
	{
		launchControlPanel(_L("Z:\\System\\Controls\\btconfig.ctl"));
	}
	else if (aCommand == cmdCPPhone)
	{
		if ((iRunningPhoneModel == EPhoneModel_P900) ||
			(iRunningPhoneModel == EPhoneModel_P910))
		{
			launchControlPanel(_L("Z:\\System\\Controls\\telegsmnetworkscp.ctl"));
		}
		else if (iRunningPhoneModel == EPhoneModel_P800)
			launchControlPanel(_L("Z:\\System\\Controls\\telepreferencesCP.ctl"));
	}
	/*************************************************************
	*
	* FN menu commands. Exists in all views.
	*
	**************************************************************/
	else if (aCommand == cmdSwitchMemory)
	{
		if (iTaskView->firstTime)
		{
			iTaskView->refreshTaskList(TODO_REFRESH);
			iTaskView->firstTime = EFalse;
		}
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidTaskView));
		DeInitTemporaryViews(EViewTask);
	}
	else if (aCommand == cmdSwitchFile)
	{
		if (iFileView->firstTime)
		{
			iFileView->DoSwitchDrive(TChar('C'));
			iFileView->firstTime = EFalse;
		}
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidFileView));
		DeInitTemporaryViews(EViewFile);
	}
	else if (aCommand == cmdSwitchBJack)
	{
		InitTemporaryViews(EViewBJack);
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidBJackView));
		DeInitTemporaryViews(EViewBJack);
	}
	else if (aCommand == cmdSwitchDB)
	{
		InitTemporaryViews(EViewDB);
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidDBView));
		DeInitTemporaryViews(EViewDB);
	}
	else if (aCommand == cmdSwitchSysInfo)
	{
		InitTemporaryViews(EViewSysInfo);
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidSysInfoView));
		DeInitTemporaryViews(EViewSysInfo);
	}
	else if (aCommand == cmdSwitchAgenda)
	{
		if (iAgendaView == NULL)
		{
			iAgendaView = new (ELeave) CSMan2AgendaView(configData);
			iAgendaView->ConstructL(ClientRect());
			RegisterViewL(*iAgendaView);
			iAgendaView->ReadAgenda(ETrue);
		}
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidAgendaView));
		DeInitTemporaryViews(EViewAgenda);
	}
	else if (aCommand == cmdSwitchDTMFDialer)
	{
		InitTemporaryViews(EViewDTMFDialer);
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidDTMFDialerView));
		DeInitTemporaryViews(EViewDTMFDialer);
	}
	else if (aCommand == cmdSwitchCellArea)
	{
		InitTemporaryViews(EViewCellArea);
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidCellAreaView));
		DeInitTemporaryViews(EViewCellArea);
	}
	else if (aCommand == cmdSwitchOptions)
	{
		CFnMenuToggle *switchDialog = new (ELeave) CFnMenuToggle(&(configData->fnActivatedMenus), &(configData->defaultView));
		if (switchDialog->ExecuteLD(R_DIALOG_FN_OPTIONS) == EEikBidOk)
			static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdControlPanel)
	{
		launchApp(KUidControlPanel, _L(""), EFalse);
	}
	/*************************************************************
	*
	* Commands for switching modules in FC view
	*
	**************************************************************/
	else if (aCommand == cmdSwitchFCBJackView)
	{
		iFCViewSwitchIsInternal = ETrue;
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidFCBJackView), KUidSMan2App, _L8(""));
		DeInitTemporaryViews(EViewFCBJack);
	}
	else if (aCommand == cmdSwitchFCCellAreaView)
	{
		iFCViewSwitchIsInternal = ETrue;
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidFCCellAreaView));
		DeInitTemporaryViews(EViewFCCellArea);
	}
	/*************************************************************
	*
	* Commands for Agenda View.
	*
	**************************************************************/
	else if (aCommand == cmdAgendaSetTodoMaxPriority)
	{
		if (iAgendaView->SetMaxTodoPriority())
		{
			static_cast<CSMan2Document*>(Document())->saveConfig();
			iAgendaView->ReadAgenda(ETrue);
		}
	}
	else if (aCommand == cmdAgendaViewOtherDate)
	{
		iAgendaView->ViewOtherDate();
	}
	else if (aCommand == cmdAgendaRead)
	{
		iAgendaView->ReadAgenda(ETrue);
	}
	else if (aCommand == cmdAgendaCompleteCalendar)
	{
		configData->agendaShowCompleteCalendar = !configData->agendaShowCompleteCalendar;
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iAgendaView->ReadAgenda(ETrue);
	}
	else if (aCommand == cmdAgendaCompleteTodo)
	{
		configData->agendaShowCompleteTodo = !configData->agendaShowCompleteTodo;
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iAgendaView->ReadAgenda(ETrue);
	}
	else if (aCommand == cmdAgendaAutoExpandCalendar)
	{
		configData->agendaAutoExpandCalendar = !configData->agendaAutoExpandCalendar;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdAgendaAutoExpandTodo)
	{
		configData->agendaAutoExpandTodo = !configData->agendaAutoExpandTodo;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	/*************************************************************
	*
	* Commands for Sysinfo View
	*
	**************************************************************/
	else if (aCommand == cmdSysInfoRefresh)
	{
		iSysInfoView->refreshSysInfo();
	}
	/*************************************************************
	*
	* Commands for Database View
	*
	**************************************************************/
	else if ((aCommand == cmdEditCut) || (aCommand == cmdEditCopy) || (aCommand == cmdEditPaste))
	{
		iDBView->DoEditCommands(aCommand);
	}
	else if (aCommand == cmdDBExportDB)
	{
		iDBView->ExportDB();
	}
	else if (aCommand == cmdDBImportDB)
	{
		iDBView->ImportDB();
	}
	else if (aCommand == cmdDBCloseDB)
	{
		if (iDBView->dbIsOpened)
			iDBView->doCloseDB(ETrue);
	}
	else if (aCommand == cmdDBSortAsc)
	{
		configData->dbSortAscending = ETrue;
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iDBView->ShowRecordsList(ETrue, EFalse);
	}
	else if (aCommand == cmdDBSortDes)
	{
		configData->dbSortAscending = EFalse;
		static_cast<CSMan2Document*>(Document())->saveConfig();		
		iDBView->ShowRecordsList(ETrue, EFalse);
	}
	else if (aCommand == cmdDBReInit)
	{
		iDBView->doReInitDB();
	}
	else if (aCommand == cmdDBChangePassword)
	{
		iDBView->doChangePassword();
	}
	else if (aCommand == cmdDBReadDB)
	{
		iDBView->doReadDB();
	}
	else if (aCommand == cmdDBDo)
	{
		LaunchPopupMenuL(R_DB_DOBUTTON, TPoint(iDBView->Size().iWidth, iDBView->PositionRelativeToScreen().iY + iDBView->Size().iHeight - 2), EPopupTargetBottomRight);
	}
	else if (aCommand == cmdDBBack)
	{
		if ((iDBView->currentDBOp == CSMan2DBView::DBOp_Editing) || (iDBView->currentDBOp == CSMan2DBView::DBOp_Adding))
		{
			HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
			HBufC* dataBuffer2 = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL);
			if (CEikonEnv::Static()->QueryWinL(*dataBuffer, *dataBuffer2))
				iDBView->ShowRecordsList(EFalse, ETrue);
			delete dataBuffer;
			delete dataBuffer2;
		}
		else
			iDBView->ShowRecordsList(EFalse, ETrue);
	}
	else if (aCommand == cmdDBSave)
	{
		if (iDBView->SaveRecord(iDBView->currentRecordIndex))
			iDBView->ShowRecordsList(ETrue, ETrue);
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_SAVE);
	}
	else if (aCommand == cmdDBView)
	{
		iDBView->doDisplayOneRecord(ETrue);
	}
	else if (aCommand == cmdDBDel)
	{
		if (!iDBView->doDelete())
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_MASS_DELETE);
		else
			iDBView->ShowRecordsList(ETrue, EFalse);
	}
	else if (aCommand == cmdDBAdd)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetTitleDimmed(1, EFalse);
		iDBView->ShowRecordsSingle(0, ETrue);
	}
	else if (aCommand == cmdDBEdit)
	{
		CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetTitleDimmed(1, EFalse);
		iDBView->doDisplayOneRecord(EFalse);
	}
	else if (aCommand == cmdDBCompact)
	{
		if (!iDBView->doCompact())
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_COMPACT);
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
	}
	else if (aCommand == cmdDBUpdateStats)
	{
		if (!iDBView->doUpdateStats())
			CEikonEnv::Static()->InfoMsg(R_TBUF_DB_ERR_UPDATESTATS);
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
	}
	/*************************************************************
	*
	* Commands for File View.
	*
	**************************************************************/
	else if (aCommand == cmdDoButton)
	{
		LaunchPopupMenuL(R_FILEMAN_DOBUTTON, TPoint(iFileView->Size().iWidth, iFileView->cFileListBox->PositionRelativeToScreen().iY + iFileView->cFileListBox->Size().iHeight + 6), EPopupTargetBottomRight);
	}
	else if (aCommand == cmdFileManFindFile)
	{
		if (iFindFileView == NULL)
		{
			iFindFileView = new (ELeave) CSMan2FindFileView(configData);
			iFindFileView->ConstructL(ClientRect());
			RegisterViewL(*iFindFileView);
		}
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidFindFileView));
	}
	else if ((aCommand == cmdFileMan_SortName) || (aCommand == cmdFileMan_SortSize) 
		|| (aCommand == cmdFileMan_SortDate) || (aCommand == cmdFileMan_SortAsc)
		|| (aCommand == cmdFileMan_SortDesc))
	{
		switch (aCommand)
		{
			case cmdFileMan_SortName : configData->filemanSortBy = ESortByName; break;
			case cmdFileMan_SortSize : configData->filemanSortBy = ESortBySize; break;
			case cmdFileMan_SortDate : configData->filemanSortBy = ESortByDate; break;
			case cmdFileMan_SortAsc  : configData->filemanSortOrder = EAscending; break;
			case cmdFileMan_SortDesc  : configData->filemanSortOrder = EDescending; break;
		}
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iFileView->ReadPath();
	}
	else if (aCommand == cmdFileMan_IrDA)
	{
		configData->beamProtocol = CBlueBeam::protocolIrDA;
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iFileView->SetHighlightColor(KRgbRed, KRgbWhite);
	}
	else if (aCommand == cmdFileMan_Bluetooth)
	{
		configData->beamProtocol = CBlueBeam::protocolBluetooth;
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iFileView->SetHighlightColor(KRgbBlue, KRgbWhite);
	}
	else if (aCommand == cmdFileManReload)
	{
		iFileView->ReadPath();
	}
	else if ((aCommand == cmdFileManOpen) || (aCommand == cmdFileManOpenOverride))
	{
		iFileView->DoOpenFile(aCommand);
	}
	else if (aCommand == cmdFileManToggleHiddenView)
	{
		configData->showHiddenFiles = !configData->showHiddenFiles;
		iFileView->ReadPath();
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdFileManToggleRunApp)
	{
		configData->autoRunApp = !configData->autoRunApp;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdFileManCRC)
	{
		iFileView->CalculateCRC();
	}
	else if (aCommand == cmdFileManToggleOpenFile)
	{
		configData->autoOpenFile = !configData->autoOpenFile;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdFileManProperties)
	{
		iFileView->DoShowProperties();
	}
	else if (aCommand == cmdFileManAll)
	{
		iFileView->SetAllSelectionStatus(ETrue);
	}
	else if (aCommand == cmdFileManNone)
	{
		iFileView->SetAllSelectionStatus(EFalse);
	}
	else if (aCommand == cmdFileManBeam)
	{
		iFileView->DoBeamFile();
	}
	else if (aCommand == cmdFileManCopyAs)
	{
		iFileView->DoCopyAs();
	}
	else if ((aCommand == cmdFileManCopy) || (aCommand == cmdFileManCut))
	{
		iFileView->DoCopyOrCut(aCommand);
	}
	else if (aCommand == cmdFileManPaste)
	{
		iFileView->DoFileManagerOperation(CSManFileMan::isPaste);
	}
	else if (aCommand == cmdFileManDelete)
	{
		iFileView->ClearClipBoard();
		iFileView->DoFileManagerOperation(CSManFileMan::isDelete);
	}
	else if (aCommand == cmdFileManRename)
	{
		iFileView->ClearClipBoard();
		iFileView->DoFileManagerOperation(CSManFileMan::isRename);
	}	
	else if (aCommand == cmdFileManNewFolder)
	{
		iFileView->DoCreateNewFolder();
	}
	else if (aCommand == cmdFileManHexEdit)
	{
		InitTemporaryViews(EViewHexEditor);
		iFileView->DoPrepareHexEditor(iHexEditorView);
		if (iHexEditorView->DoEditFile())
			ActivateViewL(TVwsViewId(KUidSMan2App, KUidHexEditorView));
		DeInitTemporaryViews(EViewHexEditor);
	}
	else if (aCommand == cmdFileManFindFile)
	{
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidFindFileView));
	}
	// File manager sub-commands: Hexeditor
	else if (aCommand == cmdHexEditorEditDeleteBytes)
	{
		iHexEditorView->DeleteBytes();
	}
	else if (aCommand == cmdHexEditorEditToggleReadOnly)
	{
		if (iHexEditorView->ToggleReadOnly())
			static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdHexEditorFileSave)
	{
		iHexEditorView->DoFileSave();
	}
	else if (aCommand == cmdHexEditorFileReload)
	{
		iHexEditorView->DoReload();
	}
	else if (aCommand == cmdHexEditorFilePreserveDate)
	{
		configData->iHexEditPreserveDate = !configData->iHexEditPreserveDate;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdHexEditorBack)
	{
		TInt iWriteResult = iHexEditorView->WritePage();
		if ((iWriteResult == 0) || (iWriteResult == 1) || (iWriteResult == -1))
		{
			ActivateViewL(TVwsViewId(KUidSMan2App, KUidFileView));
			iFileView->ReadPath();
		}
	}
	else if (aCommand == cmdHexEditorBeginPage)
	{
		iHexEditorView->ReadBeginPage();
	}
	else if (aCommand == cmdHexEditorEndPage)
	{
		iHexEditorView->ReadEndPage();
	}
	else if (aCommand == cmdHexEditorNextPage)
	{
		iHexEditorView->ReadNextPage();
	}
	else if (aCommand == cmdHexEditorPrevPage)
	{
		iHexEditorView->ReadPrevPage();
	}
	else if (aCommand == cmdHexEditorGoto)
	{
		iHexEditorView->GotoOffset();
	}
	else if (aCommand == cmdHexEditorEditResize)
	{
		if (iHexEditorView->SetHexAndASCIIControlsExtent())
			static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	// File manager sub-commands: Find file
	else if (aCommand == cmdFindFileCriteria)
	{
		TFileName iCriteria;
		iFileView->DoPrepareFindFileCriteria(&iCriteria);
		iFindFileView->SetCriteria(&iCriteria);
	}
	else if (aCommand == cmdFindFileBack)
	{
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidFileView));
	}
	else if (aCommand == cmdFindFileStop)
	{
		iFindFileView->StopFileFind();
	}
	else if (aCommand == cmdFindFileStart)
	{
		iFindFileView->StartFileFind();
	}
	// File manager main commands...unfortunately, these need to sit right at the back of
	// the list...
	else if ((aCommand >= cmdFileMan_Drive_A) && (aCommand <= cmdFileMan_Drive_Z))
	{
		iFileView->DoSwitchDrive(aCommand - cmdFileMan_Drive_A + 0x41);
	}
	/*************************************************************
	*
	* Commands for Utilities
	*
	**************************************************************/
	else if (aCommand == cmdUtilsP900toP800)
	{
		if (iRunningPhoneModel == EPhoneModel_P900)
		{
			TBool iDoConversion = EFalse;
			HBufC* iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
			HBufC* iMessage = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_P900TOP800_CONFIRM);
			iDoConversion = CEikonEnv::Static()->QueryWinL(*iTitle, *iMessage);
			delete iTitle;
			delete iMessage;
		
			if (iDoConversion)
			{
				CRecursiveDelete *iRecursiveDelete = new (ELeave) CRecursiveDelete();
				CDesCArray *iINIFiles = new (ELeave) CDesCArraySeg(3);
				TInt i;
				iINIFiles->AppendL(_L("C:\\SYSTEM\\DATA\\THEMES\\LAUNCHERWALLPAPER.INI"));
				iINIFiles->AppendL(_L("C:\\SYSTEM\\DATA\\THEMES\\STANDBYWALLPAPER.INI"));
				iINIFiles->AppendL(_L("C:\\SYSTEM\\DATA\\THEMES\\THEME.INI"));

				// Make sure everything is in upper case because CRecursiveDelete uses upper case 
				// to do comparisons against the array
				for (i = 0; i < iINIFiles->MdcaCount(); i++)
					iRecursiveDelete->iExcludeFiles->AppendL(iINIFiles->MdcaPoint(i));
				iRecursiveDelete->iExcludePaths->AppendL(_L("C:\\SYSTEM\\DATA\\THEMES\\SKINS\\STANDBY\\"));
				iRecursiveDelete->iPathToDelete.Copy(_L("C:\\SYSTEM\\DATA\\THEMES\\*.*"));
				CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
				
				TInt iResult = iRecursiveDelete->DoDelete(CFileMan::ERecurse);
				
				if (iResult == KErrNone)
				{
					// Copy 3 files across.....themes.ini, standbywallpaper.ini, launcherwallpaper.ini
					// To reduce chances of corrupted file, these 3 files have a .BLANK extension added
					// to the end of them. We unzip them first. Once all 3 have been unzipped
					// successfully, we overwrite the original INI files with these.
					CUnzip *iUnzipHandler = new (ELeave) CUnzip;
					TParse iParse;
					TFileName iFileName;
					TPath iPath;
					for (i = 0; i < iINIFiles->MdcaCount(); i++)
					{
						iParse.Set(iINIFiles->MdcaPoint(i), NULL, NULL);
						iFileName.Copy(iParse.NameAndExt());
						iPath.Copy(iParse.DriveAndPath());
						
						// We don't want SHOUTING entries. :)
						iFileName.LowerCase();
						iFileName.Append(_L(".blank"));
						iPath.LowerCase();
						
						iResult = iUnzipHandler->UnZipOneFile(&iZipFile, &iFileName, &iPath, ETrue);
						if (iResult != KErrNone)
							break;
					}
					
					// Now, copy the color scheme
					if (iResult == KErrNone)
					{
						iFileName.Copy(_L("Colorscm.dat"));
						iPath.Copy(_L("C:\\SYSTEM\\DATA\\"));
						iResult = iUnzipHandler->UnZipOneFile(&iZipFile, &iFileName, &iPath, ETrue);
					}
					delete iUnzipHandler;
					
					// Now, use the .BLANK files to overwrite the original .INI files
					if (iResult == KErrNone)
					{
						CFileMan *iFileMan = CFileMan::NewL(CEikonEnv::Static()->FsSession());
						for (i = 0; ((i < iINIFiles->MdcaCount()) && (iResult == KErrNone)); i++)
						{
							iFileName.Copy(iINIFiles->MdcaPoint(i));
							iFileName.Append(_L(".blank"));
							iPath.Copy(iINIFiles->MdcaPoint(i));
							iPath.LowerCase();
							iResult = iFileMan->Move(iFileName, iPath);
							//->MdcaPoint(i));
						}
						delete iFileMan;
						if (iResult != KErrNone)
						{
							// remove the .blank files since unzip failed
							for (i = 0; i < iINIFiles->MdcaCount(); i++)
							{
								iFileName.Copy(iINIFiles->MdcaPoint(i));
								iFileName.Append(_L(".blank"));
								EikFileUtils::DeleteFile(iFileName);
							}
							
							iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_FATALERROR);
							iMessage = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_P900TOP800_FAILED);
							CEikonEnv::Static()->InfoWinL(*iTitle, *iMessage);
							delete iTitle;
							delete iMessage;
						}
					}
					// We do not need to display any error messages here if any error occurred
					// because the CUnzip class already does this for us.
				}
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_P900TOP800_ERRDELETE);					
				CEikonEnv::Static()->BusyMsgCancel();
				delete iRecursiveDelete;
				delete iINIFiles;
				if (iResult == KErrNone)
					CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_MISC_REBOOT);
			}
		}
	}
	else if (aCommand == cmdUtilsFontSelector)
	{
		CFontSelector *iSelectorDialog = new (ELeave) CFontSelector;
		iSelectorDialog->ExecuteLD(R_DIALOG_FONTSELECTOR);
	}
	else if (aCommand == cmdUtilsAgendaFixSynchTimeZone)
	{
		HBufC *title = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
		HBufC *warning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_FIXSYNCHTIMEZONE_CONFIRM);
		if (CEikonEnv::Static()->QueryWinL(*title, *warning))
		{
			// Just being paranoid... :)
			delete title;
			delete warning;
			
			iAgendaView->FixSynchTimeZone();
		}
		else
		{
			delete title;
			delete warning;
		}
	}
	else if (aCommand == cmdUtilsFixApplist)
	{
		TApaAppInfo dummy;
		
		static_cast<CSMan2Document*>(Document())->saveConfig();
		FindApp(KUidSMan2App, &dummy);
		if (appInForeground)
			CEikonEnv::Static()->InfoMsg(R_TBUF_APPLISTFIXED);
		else
			InfoPrintFromResource(R_TBUF_APPLISTFIXED);
	}
	else if (aCommand == cmdUtilsCompContacts)
	{
		CEikonEnv::Static()->BusyMsgL(R_TBUF_MISC_BUSY);
		CContactDatabase* cdb = CContactDatabase::OpenL();
		TInt iOldSize = cdb->FileSize();
		TRAPD(error, cdb->CompactL());
		TInt iNewSize = cdb->FileSize();
		delete cdb;
		CEikonEnv::Static()->BusyMsgCancel();
		if (error != KErrNone)
			CEikonEnv::Static()->InfoMsg(R_TBUF_UTIL_ERRCOMPACTCONTACTS);
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
				
		TInt comma = 0;
		TBuf<20> textNumericOldSize;
		
		if (iOldSize < 1024)
			textNumericOldSize.Format(_L("%db"), iOldSize);
		else
			textNumericOldSize.Format(_L("%dk"), iOldSize /= 1024);
		if (iOldSize > 999)
		{
			textNumericOldSize.Insert(textNumericOldSize.Length() - 4, _L(","));
			comma++;
		}
		if (iOldSize > 999999)
			textNumericOldSize.Insert(textNumericOldSize.Length() - 4 - comma - 3, _L(","));

		TBuf<20> textNumericNewSize;
		if (iNewSize < 1024)
			textNumericNewSize.Format(_L("%dk"), iNewSize);
		else
			textNumericNewSize.Format(_L("%dk"), iNewSize /= 1024);
		if (iNewSize > 999)
		{
			textNumericNewSize.Insert(textNumericNewSize.Length() - 4, _L(","));
			comma++;
		}
		if (iNewSize > 999999)
			textNumericNewSize.Insert(textNumericNewSize.Length() - 4 - comma - 3, _L(","));
		
		// I could have done this with TDes::Format() using the %S conversion specifier. However,
		// that would mean SMan can only be compiled in Unicode mode :(
		HBufC* title = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_INFO_TITLE);
		HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_COMPRESSCONTACTS_LINE1);
		dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + textNumericOldSize.Length() + 2);
		dataBuffer->Des().Append(textNumericOldSize);
		dataBuffer->Des().Append(_L("\n"));
		HBufC* dataBuffer2 = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_COMPRESSCONTACTS_LINE2);
		dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + textNumericNewSize.Length() + dataBuffer2->Length() + 2);
		dataBuffer->Des().Append(*dataBuffer2);
		dataBuffer->Des().Append(textNumericNewSize);
		dataBuffer->Des().Append(_L("\n"));
		delete dataBuffer2;
		dataBuffer2 = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_COMPRESSCONTACTS_SAVED);
		TBuf<10> iPercentText;
		TReal pSize = ((TReal(iOldSize) - TReal(iNewSize)) / TReal(iOldSize)) * 100;
		//(TReal(TInt((TReal(TReal(iOldSize) - TReal(iNewSize)) / TReal(iOldSize)) * 10000)) / TReal(100));
		iPercentText.Format(_L16("%6.2f%%"), pSize);
		dataBuffer = dataBuffer->ReAllocL(dataBuffer->Length() + dataBuffer2->Length() + iPercentText.Length());
		dataBuffer->Des().Append(*dataBuffer2);
		dataBuffer->Des().Append(iPercentText);
		delete dataBuffer2;

		//HBufC* tempLine = HBufC::NewL(dataBuffer->Length() + textNumericOldSize.Length());
		//tempLine->Des().Format(*dataBuffer, &textNumericOldSize, &textNumericNewSize);

		CEikonEnv::Static()->InfoWinL(*title, *dataBuffer);
		delete dataBuffer;
		delete title;
	}
	else if (aCommand == cmdUtilsFixAppOrderReset)
	{
		HBufC *title = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
		HBufC *warning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_FIXAPPORDER_CONFIRMRESET);
		if (CEikonEnv::Static()->QueryWinL(*title, *warning))
		{
			EikFileUtils::DeleteFile(KAppConfigFile);
			
			TBuf<30> iAppName;
			if ((iRunningPhoneModel == EPhoneModel_P800) || (iRunningPhoneModel == EPhoneModel_P900)
				|| (iRunningPhoneModel == EPhoneModel_P910))
			{
				iAppName.Copy(KAppTaskName_P80X);
				iTaskView->GentleEndTask(iAppName, KNullUid);
			}
			else if ((iRunningPhoneModel == EPhoneModel_A920) 
				|| (iRunningPhoneModel == EPhoneModel_A925)
				|| (iRunningPhoneModel == EPhoneModel_A925_C)
				|| (iRunningPhoneModel == EPhoneModel_A920_C))
			{
				iAppName.Copy(KAppTaskName_A92X);
				iTaskView->ForceEndTask(iAppName, KNullUid);
			}
			CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_FIXAPPORDER_WAITAWHILE);
		}
		delete title;
		delete warning;
	}
	else if (aCommand == cmdUtilsFixAppOrderRestore)
	{
		if (PromptAppFixOrder(R_TBUF_UTILS_FIXAPPORDER_CONFIRMRESTORE))
		{
			if (EikFileUtils::CopyFile(KAppConfigFileBackup, KAppConfigFile, CFileMan::EOverWrite) == KErrNone)
			{
				TBuf<30> iAppName;
				if ((iRunningPhoneModel == EPhoneModel_P800) || (iRunningPhoneModel == EPhoneModel_P900)
					|| (iRunningPhoneModel == EPhoneModel_P910))
				{
					iAppName.Copy(KAppTaskName_P80X);
					iTaskView->GentleEndTask(iAppName, KNullUid);
				}
				else if ((iRunningPhoneModel == EPhoneModel_A920) 
					|| (iRunningPhoneModel == EPhoneModel_A925)
					|| (iRunningPhoneModel == EPhoneModel_A925_C)
					|| (iRunningPhoneModel == EPhoneModel_A920_C))
				{
					iAppName.Copy(KAppTaskName_A92X);
					iTaskView->ForceEndTask(iAppName, KNullUid);
				}
				CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_FIXAPPORDER_WAITAWHILE);
			}
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_FIXAPPORDER_FAILED);
		}
	}
	else if (aCommand == cmdUtilsFixAppOrderBackup)
	{
		if (PromptAppFixOrder(R_TBUF_UTILS_FIXAPPORDER_CONFIRMBACKUP))
		{
			if (EikFileUtils::CopyFile(KAppConfigFile, KAppConfigFileBackup, CFileMan::EOverWrite) == KErrNone)
				CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
			else
				CEikonEnv::Static()->InfoMsg(R_TBUF_UTILS_FIXAPPORDER_BACKUPFAIL);
		}
	}
	/*************************************************************
	*
	* Commands for Task View
	*
	**************************************************************/
	else if (aCommand == cmdShortcuts)
	{
		CArrayFixFlat<TUid> *tempUid;

		tempUid = new (ELeave) CArrayFixFlat<TUid>(NUM_SHORTCUTS);
		for (i = 0; i < NUM_SHORTCUTS; i++)
		{
			tempUid->AppendL(configData->shortCutUid[i]);
		}
		CShortcutDialog* shortcutDialog = new (ELeave) CShortcutDialog(tempUid);
		if (shortcutDialog->ExecuteLD(R_DIALOG_SHORTCUT) == EEikBidOk)
		{
			for (i = 0; i < NUM_SHORTCUTS; i++)
				configData->shortCutUid[i] = tempUid->At(i);
			static_cast<CSMan2Document*>(Document())->saveConfig();
		}
		delete tempUid;
	}
	else if ((aCommand >= cmdShort1) && (aCommand <= cmdShort7))
	{
		for (i = 0; i < NUM_SHORTCUTS; i++)
		{
			if (aCommand == (cmdShort1 + i))
			{
				launchApp(configData->shortCutUid[i], _L(""), EFalse);
				break;
			}
		}
	}
	else if ((aCommand >= cmdLastRunBase) && ((aCommand < cmdLastRunBase + NUM_LASTRUN_HISTORY)))
	{
		launchApp(historyUidList->At(aCommand - cmdLastRunBase), _L(""), ETrue);
	}
	else if (aCommand >= cmdRunAppBase)
	{
		TBool addToHistory = ETrue;
		// Check if the entry is in the shortcut list. If so, do not add it to history
		for (int i = 0; i < NUM_SHORTCUTS; i++)
			if (configData->shortCutUid[i] == startUidList->At(aCommand - cmdRunAppBase))
			{
				addToHistory = EFalse;
				break;
			}
		
		launchApp(startUidList->At(aCommand - cmdRunAppBase), _L(""), addToHistory);
		startUidList->Reset();
	}
	else if (aCommand == cmdPersist)
	{
		configData->persist = !configData->persist;
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iTaskView->SetIgnoreHarakiriState();
	}
	else if (aCommand == cmdAutostartActive)
	{
		TBuf<50> iRecognizer;
		
		iRecognizer.Copy(_L("D:\\System\\Recogs\\SManRecog.mdl"));
		if (!PhoneIsCapable(EPhone_CanAutoStartInD))
			iRecognizer[0] = TChar('C');
			
		if (ConeUtils::FileExists(iRecognizer))
		{
			if (EikFileUtils::DeleteFile(iRecognizer) != KErrNone)
			{
				if (appInForeground)
					CEikonEnv::Static()->InfoMsg(R_TBUF_AUTOSTART_ERRDELETE);
				else
					InfoPrintFromResource(R_TBUF_AUTOSTART_ERRDELETE);
			}
			else
			{
				if (appInForeground)
					CEikonEnv::Static()->InfoMsg(R_TBUF_AUTOSTART_DISABLED);
				else
					InfoPrintFromResource(R_TBUF_AUTOSTART_DISABLED);
			}
		}
		else
		{
			TBool iDoAutoStart = ETrue;
			
			if (!PhoneIsCapable(EPhone_CanAutoStartInD))
			{
				HBufC *iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_WARNING);
				HBufC *iText = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_RECOG_WARNING);
				iDoAutoStart = CEikonEnv::Static()->QueryWinL(*iTitle, *iText);
				delete iTitle;
				delete iText;
			}
			
			if (iDoAutoStart)
			{
				TFileName iMDLSource;
				iMDLSource.Copy(iRecognizer);
				TParsePtr iParsePtr(iMDLSource);
				
				// Check to make sure target folder exists
				TRAPD(iError, ConeUtils::EnsurePathExistsL(iParsePtr.DriveAndPath()));
				
				if (iError == KErrNone)
				{
					iMDLSource.Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName());
					iMDLSource.SetLength(iMDLSource.Length() - 4);
					iMDLSource.Append(_L("Recog.mdl")); // this gives SManRecog.mdl including full path
					
					// Now to copy file from iMDLSource to iRecognizer
					iError = EikFileUtils::CopyFile(iMDLSource, iRecognizer);
					if (iError == KErrNone)
					{
						if (appInForeground)
							CEikonEnv::Static()->InfoMsg(R_TBUF_AUTOSTART_ENABLED);
						else
							InfoPrintFromResource(R_TBUF_AUTOSTART_ENABLED);
					}
					
					/* ZIP / UNZIP code can't be used because A92x doesn't have gzip libraries
					built in! :( Why did Motorola remove them? :(
					
					TFileName iMDLName;
					TPath iMDLPath;
					TParse iParse;
					TInt iResult;
					
					iParse.Set(iRecognizer, NULL, NULL);
					iMDLPath.Copy(iParse.DriveAndPath());
					iMDLName.Copy(iParse.NameAndExt());
										
					CUnzip *iUnzipHandler = new (ELeave) CUnzip;
					iResult = iUnzipHandler->UnZipOneFile(&iZipFile, &iMDLName, &iMDLPath, ETrue);
					delete iUnzipHandler;
					if (iResult == KErrNone)
					{
						if (appInForeground)
							CEikonEnv::Static()->InfoMsg(R_TBUF_AUTOSTART_ENABLED);
						else
							InfoPrintFromResource(R_TBUF_AUTOSTART_ENABLED);
					}
					*/
				}
				if (iError != KErrNone)
					CEikonEnv::Static()->InfoMsg(R_TBUF_AUTOSTART_ERRCOPY);
			}
		}
	}
	else if (aCommand == cmdAutostartOthers)
	{
		CAutostartDialog* autoStartDialog = new (ELeave) CAutostartDialog();
		autoStartDialog->ExecuteLD(R_DIALOG_AUTOSTART);
	}
	else if (aCommand == cmdAutostartSMan)
	{
		CAutostartDialog* autoStartDialog = new (ELeave) CAutostartDialog();
		autoStartDialog->loadBootFile();		
		if (autoStartDialog->autoStartConfig.autoStartUid[MAX_AUTOSTART - 1] != KUidSMan2App)
		{
			// Ensure we get upper case stuff only!
			autoStartDialog->autoStartConfig.autoStartApp[MAX_AUTOSTART - 1].Copy(CEikonEnv::Static()->EikAppUi()->Application()->AppFullName().Left(2));
			autoStartDialog->autoStartConfig.autoStartApp[MAX_AUTOSTART - 1].Append(_L("\\SYSTEM\\APPS\\SMAN\\SMAN.APP"));
			autoStartDialog->autoStartConfig.autoStartUid[MAX_AUTOSTART - 1] = KUidSMan2App;
			
			if (appInForeground)
				CEikonEnv::Static()->InfoMsg(R_TBUF_AUTOSTART_ENABLED);
			else
				InfoPrintFromResource(R_TBUF_AUTOSTART_ENABLED);
		}
		else
		{
			autoStartDialog->autoStartConfig.autoStartApp[MAX_AUTOSTART - 1].Copy(_L(""));
			autoStartDialog->autoStartConfig.autoStartUid[MAX_AUTOSTART - 1] = KNullUid;

			if (appInForeground)
				CEikonEnv::Static()->InfoMsg(R_TBUF_AUTOSTART_DISABLED);
			else
				InfoPrintFromResource(R_TBUF_AUTOSTART_DISABLED);
		}
		if (autoStartDialog->saveBootFile() != 0)
		{
			if (appInForeground)
				CEikonEnv::Static()->InfoMsg(R_TBUF_ERRSAVECONFIG);
			else
				InfoPrintFromResource(R_TBUF_ERRSAVECONFIG);
		}
		delete autoStartDialog;
	}
	else if (aCommand == cmdLogFlags)
	{
		CLogLevelDialog* logLevelDialog = new (ELeave) CLogLevelDialog(&(configData->logFlags));
		if (logLevelDialog->ExecuteLD(R_DIALOG_LOGLEVELS) == EEikBidOk)
			static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdShowHiddenTask)
	{
		configData->showHiddenTasks = !configData->showHiddenTasks;
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iTaskView->refreshTaskList(TODO_REFRESH);
	}
	else if (aCommand == cmdNoDevicePause)
	{
		CEikDialog* noDevicePauseDialog = new(ELeave) CBluejackTimerDialog(&(configData->noDevicePause), &(configData->noDeviceAttempts), &(configData->decayTime), &(configData->obexTimeout));
		if (noDevicePauseDialog->ExecuteLD(R_DIALOG_NODEVICEPAUSE) == EEikBidOk)
			static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdForceEndTask)
	{
		configData->forceEndTask = !configData->forceEndTask;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdFlipFlush)
	{
		if (!configData->flipFlush)
		{
			if (iTaskView->iSnapshotTaskCaption->Count() <= 0)
				CEikonEnv::Static()->InfoMsg(R_TBUF_NOSNAPSHOT);
			else
				configData->flipFlush = !configData->flipFlush;
		}
		else
			configData->flipFlush = !configData->flipFlush;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdEndTask)
	{
		if (iTaskView->cTaskListBox->SelectionIndexes()->Count() > 0)
		{
			iTaskView->refreshTaskList(TODO_TERMINATE);			
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_NOMARKEDTASKS);
	}
	else if (aCommand == cmdBluejackAutoSave)
	{
		configData->autoSave = !configData->autoSave;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdFlushOut)
	{
		flushTasks();
	}
	else if (aCommand == cmdFlushExclude)
	{
		configData->iFlushExclude = !configData->iFlushExclude;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}	
	else if (aCommand == cmdSnapshot)
	{
		CEikDialog* snapshotDialog = new(ELeave) CEikDialog;
		if (snapshotDialog->ExecuteLD(R_DIALOG_SNAPSHOTCONFIRM) == EEikBidYes)
			updateSnapShot();
	}
	else if (aCommand == cmdHotkey)
	{
		CHotkeyDialog* iHotkeyDialog = new (ELeave) CHotkeyDialog();
		
		iHotkeyDialog->iHotkey = &(configData->iHotkeyDialogResult);

		// Set FO keys
		iHotkeyDialog->iInFocusShort_FO = &(configData->iHotkeyInFocusShort_FO);
		iHotkeyDialog->iInFocusLong_FO = &(configData->iHotkeyInFocusLong_FO);
		iHotkeyDialog->iExFocusShort_FO = &(configData->iHotkeyExFocusShort_FO);
		iHotkeyDialog->iExFocusLong_FO = &(configData->iHotkeyExFocusLong_FO);

		// Set FC keys
		iHotkeyDialog->iInFocusShort_FC = &(configData->iHotkeyInFocusShort_FC);
		iHotkeyDialog->iInFocusLong_FC = &(configData->iHotkeyInFocusLong_FC);
		iHotkeyDialog->iExFocusShort_FC = &(configData->iHotkeyExFocusShort_FC);
		iHotkeyDialog->iExFocusLong_FC = &(configData->iHotkeyExFocusLong_FC);
		
		if (iHotkeyDialog->ExecuteLD(R_DIALOG_HOTKEY) == EEikBidOk)
		{
			iTaskView->ToggleHotKey();
			static_cast<CSMan2Document*>(Document())->saveConfig();
		}
	}
	else if ((aCommand == cmdQuit) || (aCommand == EEikCmdExit))
	{
		if ((aCommand == EEikCmdExit) && (configData->persist))
		{
			// Do nothing
		}
		else
		{
			TBool iBJackActive = EFalse;
			
			if (iBJackView)
				iBJackActive = iBJackView->oScanner->IsActive();
			
			if (iBJackActive)
			{
				if (iBJackView->oScanner->scannerStatus == CScanner::isShutdown)
					CEikonEnv::Static()->InfoMsg(R_TBUF_SCANNERSTOPPING);
				else
					CEikonEnv::Static()->InfoMsg(R_TBUF_STOPSCANNER);
			}
			else if (iFileView->blueBeam->IsActive())
				CEikonEnv::Static()->InfoMsg(R_TBUF_STOPBEAM);
			else
			{
				if (aCommand == cmdQuit)
				{
					HBufC *title = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
					HBufC *warning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_FIXSYNCHTIMEZONE_CONFIRM);
					if (CEikonEnv::Static()->QueryWinL(*title, *warning))
					{
						// Just being paranoid... :)
						delete title;
						delete warning;					
						Exit();
					}
					else
					{
						delete title;
						delete warning;
					}
				}
				else	// Has to be EEikCmdExit. The OS wants us to quit so we should honour it
					Exit();
			}
		}
	}
	else if (aCommand == cmdCompressHeap)
	{
		User::CompressAllHeaps();
		User::Heap().Compress();
		iTaskView->refreshTaskList(TODO_REFRESH);
	}
	else if (aCommand == cmdRefresh)
	{
		iTaskView->refreshTaskList(TODO_REFRESH);
	}
	else if (aCommand == cmdNoHotkey)
	{
		configData->hotkeyEnabled = !configData->hotkeyEnabled;
		//static_cast<CSMan2Document*>(Document())->saveConfig();
		iTaskView->ToggleHotKey();
	}
	else if (aCommand == cmdResolveName)
	{
		configData->resolveName = !configData->resolveName;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	/*************************************************************
	*
	* Commands for Bluejack view
	*
	**************************************************************/
	// For these commands, we don't have to check if iBJackView is initialized because
	// these commands are in the menubar of that view
	else if (aCommand == cmdBluejackClearLog)
	{
		// The FC bluejack view can get activated even though the FO bluejack view isn't yet
		// initialized. So, we have to check if iBJackView is valid before doing any log clearing.
		// If it isn't valid, we are sure the user has never used bluejacking since starting
		// SMan this time around. Therefore, there are no logs to clear.
		if (iBJackView)
		{
			iBJackView->oScanner->logHelper->logBuffer->Zero();
			//iBJackView->oScanner->logHelper->updateLog(NULL, 0, CLogger::logAll);
			// Clear both flipclosed and flipopened logs
			iBJackView->bluejackLog->SetTextL(iBJackView->oScanner->logHelper->logBuffer);
			iBJackView->bluejackLog->DrawDeferred();
			iFCBJackView->bluejackLog->SetTextL(iBJackView->oScanner->logHelper->logBuffer);
			iFCBJackView->bluejackLog->DrawDeferred();
		}
	}
	else if (aCommand == cmdBluejackStop)
	{
		iBJackView->oScanner->stopScan();
	}
	else if (aCommand == cmdBluejackOne)
	{
		if (!iBJackView->oScanner->IsActive())
		{
			// bluejack one
			iBJackView->oScanner->iScannerBehaviour = CScanner::behaviourBluejackOne;
			iBJackView->oScanner->startScan();
		}
	}
	else if (aCommand == cmdBluejackDelFileLog)
	{
		EikFileUtils::DeleteFile(iBJackView->oScanner->logHelper->logFilename);
		CEikonEnv::Static()->InfoMsg(R_TBUF_MISC_DONE);
	}
	else if (aCommand == cmdBluejackViewFileLog)
	{
		if (ConeUtils::FileExists(iBJackView->oScanner->logHelper->logFilename))
			QikFileUtils::StartAppL(iBJackView->oScanner->logHelper->logFilename);
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_BJACKLOGMISSING);
	}
	else if (aCommand == cmdBluejackSysMsg)
	{
		CBluejackNotifications* notificationDialog = new (ELeave) CBluejackNotifications(configData);
		if (notificationDialog->ExecuteLD(R_DIALOG_NOTIFICATION) == EEikBidOk)
			static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdBluejackOnce)
	{
		configData->proxSweepOnce = !configData->proxSweepOnce;
		static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdAbout)
	{
	
/*
gzFile outFile;
outFile = gzopen("c:\\qfile.gz", "wb6 ");
RFile iInputFile;
iInputFile.Open(CEikonEnv::Static()->FsSession(), _L("C:\\qfileman.sis"), EFileRead);
TBuf8<8192> iInputBuffer;
do
{
	iInputFile.Read(iInputBuffer);
	gzwrite(outFile, iInputBuffer.Ptr(), iInputBuffer.Size());
} while (iInputBuffer.Length() != 0);
iInputFile.Close();
gzclose(outFile);

// See http://www3.symbian.com/faq.nsf/0/055490199FDB4D1580256A570051B8C8?OpenDocument
CloseSTDLIB();
*/
		CEikDialog* aboutDialog = new (ELeave) CEikDialog;
		TInt iButtonRes = aboutDialog->ExecuteLD(R_DIALOG_ABOUT);
		switch (iButtonRes)
		{
			case EEikBidYes :
				{
					CCreditsDialog *iCreditsDialog = new (ELeave) CCreditsDialog();
					iCreditsDialog->iMessageResourceID = R_TBUF_CREDITS;
					iCreditsDialog->ExecuteLD(R_DIALOG_CREDITS);
					break;
				}
			#ifdef __DEBUG_BUILD__
			case EEikBidOk :
				{
					CDebugDialog *iDebugDialog = new (ELeave) CDebugDialog();
					iDebugDialog->iRunningPhoneModel = &iRunningPhoneModel;
					iDebugDialog->ExecuteLD(R_DIALOG_DEBUG);
					break;
				}
			#endif
			default: break;
		}

/*
#include <msvapi.h>
#include <mtclreg.h>
#include <msvstd.h>
LIB - msgs.lib

CMsvSession* iMsvSession = CMsvSession::OpenSyncL(*this);
// Create client registry object
CClientMtmRegistry* iClientMtmRegistry =
CClientMtmRegistry::NewL(*iMsvSession);

//handle also the invisible entries
TMsvSelectionOrdering sort;
sort.SetShowInvisibleEntries(ETrue);

// Search MMSs in the INBOX
CMsvEntry* parentEntry;
parentEntry = CMsvEntry::NewL* (iMsvSession,KMsvDraftEntryId, sort);
CleanupStack::PushL(parentEntry);

CMsvEntrySelection* entries = parentEntry->ChildrenWithMtmL(KUidMsgTypeMMS);
CleanupStack::PushL(entries);

entries->Count();
*/

//CEikonEnv::Static()->AppUiFactory()->MenuBar()->OverrideColorL(EColorMenuPaneDimmedText, KRgbBlack);
//CEikonEnv::Static()->AppUiFactory()->MenuBar()->SetTitleDimmed(0, ETrue);

	}
	else if (aCommand == cmdBluejackMsg)
	{
		if (!iBJackView->oScanner->IsActive())
		{
			CBluejackMessageDialog* bluejackMsgDialog = new (ELeave) CBluejackMessageDialog(iBJackView->oScanner);
			bluejackMsgDialog->ExecuteLD(R_BLUEJACK_VCARD);
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_STOPSCANNER);
	}
	else if (aCommand == cmdBluejackExcludeList)
	{
		iBJackView->oScanner->EditBTExclusionList();
	}
	else if (aCommand == cmdBluejack)
	{
		if (!iBJackView)
			InitTemporaryViews(CSMan2AppUi::EViewBJack);
		
		// This command was called from the flipclosed bluejack view? If so, reroute the log
		// to the flipclosed bluejack view
		if (iCoeEnv->ScreenDevice()->CurrentScreenMode() == 1)
		{
			iBJackView->oScanner->logHelper->logTextBox = iFCBJackView->bluejackLog;
			iBJackView->oScanner->logHelper->updateLog(NULL, 0, CLogger::logAll);
		}
		
		if (!iBJackView->oScanner->IsActive())
		{
			iBJackView->oScanner->iScannerBehaviour = CScanner::behaviourProximitySweep;
			iBJackView->oScanner->startScan();
		}
		else
			/* Note, this part of the else block will only get executed if
			1. You are in flip closed mode
			2. You chose to stop bluejack
			3. Bluejack was running
			This is because we disable this command in the fullscreen mode's toolbar if bluejack is already
			active i.e. you will not be able to press the button and issue this command in that scenario
			*/
			iBJackView->oScanner->stopScan();
	}
	/*************************************************************
	*
	* Commands for Cell Area
	*
	**************************************************************/
	else if (aCommand == cmdCellAreaEventDelay)
	{
		if (iCellAreaView->DoChangeEventDelay())
			static_cast<CSMan2Document*>(Document())->saveConfig();
	}
	else if (aCommand == cmdCellAreaAlwaysRun)
	{
		if (!iCellAreaView->IsRecording())
		{
			configData->iCellAreaAlwaysRun = !(configData->iCellAreaAlwaysRun);
			iCellAreaView->AlwaysRunFlagChanged(ETrue);
			static_cast<CSMan2Document*>(Document())->saveConfig();
		}
		else
			CEikonEnv::Static()->InfoMsg(R_TBUF_CELLID_DISABLEAUTOREC);
	}
	else if (aCommand == cmdCellAreaInitStore)
	{
		HBufC *iTitle = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_DB_CONFIRM_CANCEL_TITLE);
		HBufC *iWarning = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_UTILS_FIXSYNCHTIMEZONE_CONFIRM);
		TBool iConfirm = CEikonEnv::Static()->QueryWinL(*iTitle, *iWarning);
		delete iTitle;
		delete iWarning;
		if (iConfirm)
			iCellAreaView->ReInitStorage();
		iCellAreaView->ForceCellInfoUpdate();
	}
	else if (aCommand == cmdCellAreaToggleRecording)
	{
		iCellAreaView->ToggleRecording();
		iCellAreaView->ForceCellInfoUpdate();
	}
	else if (aCommand == cmdCellAreaEdit)
	{
		iCellAreaView->iProcessEvents = EFalse;
		iCellAreaView->DoEdit();
		iCellAreaView->ForceCellInfoUpdate();
		iCellAreaView->iProcessEvents = ETrue;
	}
	else if (aCommand == cmdCellAreaUpdateStats)
	{
		iCellAreaView->DoUpdateStats();
	}
	else if (aCommand == cmdCellAreaCompress)
	{
		iCellAreaView->DoCompress();
	}
	/*************************************************************
	*
	* Commands for DTMF Dialer
	*
	**************************************************************/
	else if (aCommand == cmdDTMFDialerInput)
	{
		configData->iDTMFInput = !(configData->iDTMFInput);
		iDTMFDialerView->StopAndResetDialer();
		static_cast<CSMan2Document*>(Document())->saveConfig();
		iDTMFDialerView->InitDialerState();
	}
	else if ((aCommand == cmdDTMFEditCut) || (aCommand == cmdDTMFEditCopy) || (aCommand == cmdDTMFEditPaste))
	{
		iDTMFDialerView->DoEditCommands(aCommand);
	}
	else if (aCommand == cmdDTMFEditClear)
	{
		TBuf<1> iBlank;
		iBlank.Copy(_L(""));
		iDTMFDialerView->iDTMFNumbers->SetTextL(&iBlank);
		iDTMFDialerView->iDTMFNumbers->DrawNow();
	}
}

void CSMan2AppUi::launchApp(TUid appUid, TFileName docName, TBool putHistory)
{
	RApaLsSession mySession;
	TInt runResult;
	TApaAppInfo aInfo;		
	
	// Don't launch if another copy is already running.

	runResult = KErrGeneral;
	TApaTaskList taskList(CEikonEnv::Static()->WsSession());
	TApaTask theTask = taskList.FindApp(appUid);
	if (theTask.Exists())
	{
		// Why the *toot* did I even put this here????? This line causes problems!!
		//SetAppPosition(EFalse);
		
		theTask.BringToForeground();
		runResult = KErrNone;
	}
	else
	{
		if (FindApp(appUid, &aInfo) == 0)
		{
			if (docName.Compare(_L("")) == 0)
			{
				TRAPD(err, QikFileUtils::MakeAppDocumentFileNameL(docName, appUid));
				if (err != KErrNone)
					docName.Copy(_L(""));
			}
			CApaCommandLine* cmdln = CApaCommandLine::NewLC();
			cmdln->SetLibraryNameL(aInfo.iFullName);
			cmdln->SetCommandL(EApaCommandRun);
			cmdln->SetDocumentNameL(docName);
			mySession.Connect();
			runResult = mySession.StartApp(*cmdln);
			CleanupStack::PopAndDestroy(cmdln);
			mySession.Close();
		}
	}
	if (runResult != KErrNone)
		CEikonEnv::Static()->InfoMsg(R_TBUF_ERRLAUNCHAPP);
	else
	{
		if (putHistory)
		{
			TInt i, j;

			for (i = NUM_LASTRUN_HISTORY - 1; i > -1; i--)
				if (historyUidList->At(i) == appUid)
					break;
				
			if (i > -1)
				j = i;
			else
				j = NUM_LASTRUN_HISTORY - 1;

			for (; j > 0; j--)
				historyUidList->At(j) = historyUidList->At(j - 1);
			historyUidList->At(0) = appUid;
		}
	}
}

void CSMan2AppUi::SetAppPosition(TBool foreground)
{
	RWsSession& oWindowSession = CEikonEnv::Static()->WsSession();
	RWindowGroup& oWindowGroup = CEikonEnv::Static()->RootWin();
	
	int windowID = oWindowGroup.Identifier();
	TApaTask oTApaTask(oWindowSession);
	oTApaTask.SetWgId(windowID);
	if (foreground)
		oTApaTask.BringToForeground();
	else
		oTApaTask.SendToBackground();
}

void CSMan2AppUi::focusPhoneApp()
{
	TVwsViewId viewId;
	viewId.iAppUid = KUidPhoneApp;
	if (GetDefaultViewId(viewId) == KErrNone)
	{
		TRAPD(err, ActivateViewL(viewId));
		if (!err)
			iCoeEnv->RootWin().SetOrdinalPosition(-1);
	}
}

void CSMan2AppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
{
	if ((aEvent.Type() == EEventKey) && (iIsProcessingShortcut == EFalse))
	{
		iIsProcessingShortcut = ETrue;

		TInt iScreenMode = CEikonEnv::Static()->ScreenDevice()->CurrentScreenMode();
		if (aEvent.Key()->iCode == CAPTURE_LONG_HOTKEY_REPLACEMENT)
		{
			if (appInForeground)
			{
				if (iScreenMode == 0)
					DoShortCuts(configData->iHotkeyInFocusLong_FO, iScreenMode);
				else
					DoShortCuts(configData ->iHotkeyInFocusLong_FC, iScreenMode);
			}
			else
			{
				if (iScreenMode == 0)
					DoShortCuts(configData->iHotkeyExFocusLong_FO, iScreenMode);
				else
					DoShortCuts(configData ->iHotkeyExFocusLong_FC, iScreenMode);
			}
		}
		if (aEvent.Key()->iCode == sCaptureKeyCodes[configData->iHotkeyDialogResult])
		{
			if (appInForeground)
			{
				if (iScreenMode == 0)
					DoShortCuts(configData->iHotkeyInFocusShort_FO, iScreenMode);
				else
					DoShortCuts(configData->iHotkeyInFocusShort_FC, iScreenMode);
			}
			else
			{
				if (iScreenMode == 0)
					DoShortCuts(configData->iHotkeyExFocusShort_FO, iScreenMode);
				else
					DoShortCuts(configData->iHotkeyExFocusShort_FC, iScreenMode);
			}
		}
		iIsProcessingShortcut = EFalse;
	} 
	else if (aEvent.Type() == EEventScreenDeviceChanged)
	{
		// For another strange reason, i found that this event is fired off by the
		// phone when you launch your program in flipclosed mode. I didn't even open/close
		// the flip and yet it is fired....why?!?!? my workaround is to use "oldScreenMode"
		if (CEikonEnv::Static()->ScreenDevice()->CurrentScreenMode() == 0) // 0 = flip open
		{
			if (PhoneIsCapable(EPhone_CanSetHotkey) && PhoneIsCapable(EPhone_HasFlip))
			{
				if (configData->iHotkeyDialogResult == 3)
					SetAppPosition(ETrue);
			}
			oldScreenMode = 0;
		}
		else if (PhoneIsCapable(EPhone_HasFlip))
		{
			if (oldScreenMode != 1)
			{
				if (configData->flipFlush)
				{
					flushTasks();
					User::CompressAllHeaps();
					User::Heap().Compress();
				}
				SetAppPosition(EFalse);
				oldScreenMode = 1;
			}
		}
	}
	// To modify selection in hexedit view when virtual keyboard FEP is deactivated. 
	// Very, very bad hack :(
	else if (aEvent.Type() == EEventMessageReady)
	{
		TVwsViewId currentView;
		if (GetActiveViewId(currentView) == KErrNone)
		{
			if ((currentView.iAppUid == KUidSMan2App) && (currentView.iViewUid == KUidHexEditorView))
				iHexEditorView->ModifyHexEditorSelection();
		}
	}
	CQikAppUi::HandleWsEventL(aEvent, aDestination);
}

void CSMan2AppUi::flushTasks(void)
{
	if (iTaskView->iSnapshotTaskCaption->Count() <= 0)
	{
		if (appInForeground)
			CEikonEnv::Static()->InfoMsg(R_TBUF_NOSNAPSHOT);
		else
			InfoPrintFromResource(R_TBUF_NOSNAPSHOT);
	}
	else
	{
		iTaskView->refreshTaskList(TODO_FLUSH);
		if (appInForeground)
			CEikonEnv::Static()->InfoMsg(R_TBUF_TASKSFLUSHED);
		else
			InfoPrintFromResource(R_TBUF_TASKSFLUSHED);
	}
}

void CSMan2AppUi::updateSnapShot(void)
{
	iTaskView->iSnapshotTaskCaption->Reset();
	iTaskView->iSnapshotTaskUid->Reset();
	iTaskView->refreshTaskList(TODO_SNAPSHOT);
	if (iTaskView->saveSnapShot() == 0)
		CEikonEnv::Static()->InfoMsg(R_TBUF_SNAPSHOTUPDATED);
	else
		CEikonEnv::Static()->InfoMsg(R_TBUF_ERRSAVESNAPSHOT);
}

void CSMan2AppUi::FindAndLaunchAppNoUid(const TDesC *appPath)
{
	// Given the path to an .APP or .EXE file, find the UID of the app and launch it
	RApaLsSession mySession;
	TApaAppCapabilityBuf aCapabilityBuf;
	TApaAppInfo aInfo;
	TBool appRegistered = EFalse;

	TFullName temp;
	temp.Copy(*appPath);
	temp.UpperCase();
	TParsePtr toFind(temp);
	
	// Bounce the app server
	FindApp(KUidControlPanel, &aInfo);
	
	mySession.Connect();
	mySession.GetAllApps();
	while (mySession.GetNextApp(aInfo) == KErrNone)
	{
		if (mySession.GetAppCapability(aCapabilityBuf, aInfo.iUid) == KErrNone)
		{
			// Only scan applications that are not hidden
			if (aCapabilityBuf().iAppIsHidden == EFalse)
			{
				aInfo.iFullName.UpperCase();
				if (aInfo.iFullName == toFind.FullName())
				{
					launchApp(aInfo.iUid, _L(""), EFalse);
					appRegistered = ETrue;
					break;
				}
			}
		}
	}
	mySession.Close();
	if (!appRegistered)
		CEikonEnv::Static()->InfoMsg(R_TBUF_FILEMAN_APPNOTREGISTERED);
}

TInt CSMan2AppUi::FindApp(TUid appUid, TApaAppInfo *aInfo)
{
	// Given an app UID, find if it is installed and get its app info

	RApaLsSession mySession;
	TApaAppInfo theAppInfo;
	TApaAppCapabilityBuf aCapabilityBuf;
	TInt errRes, iTimeout = 50;
	TTimeIntervalMicroSeconds32 iDelay = 100;
	
	// For some strange reason when I have just issued a command to write to the
	// disk (via saving snapshot or saving shortcuts), the app server always returns
	// an invalid application list (corrupted). I have to wait a while before the query
	// runs correct again....?????

	CEikonEnv::Static()->BusyMsgL(R_TBUF_VALIDATING);
	mySession.Connect();
	mySession.GetAllApps();
	errRes = 2;
	while ((errRes != 0) & (iTimeout > 0))
	{
		while (mySession.GetNextApp(theAppInfo) == KErrNone)
		{
			errRes = 1;
			if (mySession.GetAppCapability(aCapabilityBuf, theAppInfo.iUid) == KErrNone)
			{
				// Only scan applications that are not hidden
				if (aCapabilityBuf().iAppIsHidden == EFalse)
				{
					if (theAppInfo.iUid == appUid)
					{
						aInfo->iUid = theAppInfo.iUid;
						aInfo->iFullName = theAppInfo.iFullName;
						aInfo->iCaption = theAppInfo.iCaption;
						errRes = 0;
						break;
					}
				}
			}
		}
		User::After(iDelay);
		iTimeout--;
	} 
	mySession.Close();
	CEikonEnv::Static()->BusyMsgCancel();
	return errRes;
}

void CSMan2AppUi::sizeToText(TInt64 size, TDes* desc)
{
	TInt64 origSize, decimals;
	TBuf<3> sizeName;
	
	origSize = size;
	if (size < 1024)
	{
		sizeName = _L("b)");
		decimals = 0;
	}
	else if (size < (1024 * 1024))
	{
		size = size / 1024;
		decimals = (origSize - (size * 1024)) / 1024;
		sizeName = _L("KB)");
	}
	else
	{
		size = size / (1024 * 1024);
		decimals = (origSize - (size * 1024 * 1024)) / 1024;
		sizeName = _L("MB)");
	}
	
	desc->Num(size);
	desc->Append(_L("."));
	desc->AppendFormat(_L("%2d"), decimals);
	if (size > 999)
		desc->Insert(desc->Length() - 6, _L(","));
	desc->Append(sizeName);
	desc->Insert(0, _L("]("));
}

void CSMan2AppUi::DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane)
{
	// Generic menus (Fn menu) - those that don't belong to any particular view
	if (aMenuId == R_FN_OPTIONS_MENU)
	{
		if (PhoneIsCapable(EPhone_CanSetHotkey))
		{
			if (!configData->hotkeyEnabled)
				aMenuPane->SetItemButtonState(cmdNoHotkey, EEikMenuItemSymbolOn);
		}
		else
		{
			aMenuPane->SetItemDimmed(cmdNoHotkey, ETrue);
			aMenuPane->SetItemDimmed(cmdHotkey, ETrue);
		}
			
		if (configData->persist)
			aMenuPane->SetItemButtonState(cmdPersist, EEikMenuItemSymbolOn);
	}
	if (aMenuId == R_MODULES_MENU)
	{							
		// Handle manufacturer specific changes
		if (!PhoneIsCapable(EPhone_HasNetworkInfoTSY))
			aMenuPane->SetItemDimmed(cmdSwitchCellArea, ETrue);
		if (!PhoneIsCapable(EPhone_HasBluetooth))
			aMenuPane->SetItemDimmed(cmdSwitchBJack, ETrue);
		
		if ((configData->fnActivatedMenus & 1) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchMemory);
		if ((configData->fnActivatedMenus & 2) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchFile);
		if ((configData->fnActivatedMenus & 4) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchBJack);
		if ((configData->fnActivatedMenus & 8) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchDB);
		if ((configData->fnActivatedMenus & 16) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchSysInfo);
		if ((configData->fnActivatedMenus & 32) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchAgenda);
		if ((configData->fnActivatedMenus & 64) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchDTMFDialer);
		if ((configData->fnActivatedMenus & 128) == 0)
			aMenuPane->DeleteMenuItem(cmdSwitchCellArea);
	}
	else if (aMenuId == R_RUNAPPLIST_MENU)
	{
		CEikMenuPaneItem::SData itemData;

		CShortcutDialog* shortcutDialog = new (ELeave) CShortcutDialog(NULL);
		shortcutDialog->populateAppArray();
		startUidList->Reset();
		startUidList->Compress();
		for (int i = 0; i < shortcutDialog->appListArray->MdcaCount(); i++)
		{
			itemData.iText.Copy(shortcutDialog->appListArray->MdcaPoint(i));
			itemData.iCommandId = cmdRunAppBase + i;
			startUidList->AppendL(shortcutDialog->appListUid->At(i));
			itemData.iFlags = 0;
			itemData.iCascadeId = 0;
			aMenuPane->AddMenuItemL(itemData);
		}
		delete shortcutDialog;

	}
	else if (aMenuId == R_SHORTCUTS_MENU)
	{				
		// This is faster than NUM_SHORTCUTS calls to FindApp() as long as NUM_SHORTCUTS is less
		// than the number of apps u have installed. However, this is bulkier in code.
		RApaLsSession mySession;
		TApaAppCapabilityBuf aCapabilityBuf;
		TApaAppInfo aInfo;
		TInt i;
		CEikMenuPaneItem::SData itemData;
		
		// Bounce the app server
		FindApp(KUidControlPanel, &aInfo);
		
		mySession.Connect();
		mySession.GetAllApps();
		while (mySession.GetNextApp(aInfo) == KErrNone)
		{
			if (mySession.GetAppCapability(aCapabilityBuf, aInfo.iUid) == KErrNone)
			{
				// Only scan applications that are not hidden
				if (aCapabilityBuf().iAppIsHidden == EFalse)
				{
					for (i = 0; i < NUM_SHORTCUTS; i++)
					{
						if (aInfo.iUid == configData->shortCutUid[i])
						{
							aMenuPane->SetItemDimmed(cmdShort1 + i,EFalse);
							aMenuPane->SetItemTextL(cmdShort1 + i, aInfo.iCaption);
						}
					}
				}
			}
		}
		mySession.Close();

		// Dynamically initialize the history menu
		TBool hasHistory = EFalse;
		TInt iMenuPos;
		
		for (int i = 0; i < NUM_LASTRUN_HISTORY; i++)
		{
			if (historyUidList->At(i) != KNullUid)
			{
				if (FindApp(historyUidList->At(i), &aInfo) == 0)
					itemData.iText.Copy(aInfo.iCaption);
				else
					itemData.iText=_L("<none>");

				itemData.iCommandId = cmdLastRunBase + i;
				itemData.iFlags = 0;
				itemData.iCascadeId = 0;
				// add the item
				aMenuPane->AddMenuItemL(itemData);
				hasHistory = ETrue;
			}
		}
		
		// As per Phil Spencer's rant, I'm now in compliance with UIQ's style guide. LOL. :)
		if (hasHistory)
		{
			CEikMenuPaneItem *theMenuItem = aMenuPane->ItemAndPos(cmdShort7, iMenuPos);
			theMenuItem->iData.iFlags = theMenuItem->iData.iFlags | EEikMenuItemSeparatorAfter;
		}
	}
	else if (aMenuId == R_OPTIONS_AUTOSTART_MENU)
	{
		TBuf<50> iRecognizer;
		
		iRecognizer.Copy(_L("D:\\SYSTEM\\RECOGS\\SMANRECOG.MDL"));
		if (!PhoneIsCapable(EPhone_CanAutoStartInD))
			iRecognizer[0] = TChar('C');
		if (!ConeUtils::FileExists(iRecognizer))
		{
			aMenuPane->SetItemDimmed(cmdAutostartSMan, ETrue);
			aMenuPane->SetItemDimmed(cmdAutostartOthers, ETrue);
		}
		else
		{
			HBufC* iBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_AUTOSTART_DEACTIVATE);
			aMenuPane->SetItemTextL(cmdAutostartActive, *iBuffer);
			delete iBuffer;
			
			CAutostartDialog* autoStartDialog = new (ELeave) CAutostartDialog();
			autoStartDialog->loadBootFile();
			if (autoStartDialog->autoStartConfig.autoStartUid[MAX_AUTOSTART - 1] == KUidSMan2App)
				aMenuPane->SetItemButtonState(cmdAutostartSMan, EEikMenuItemSymbolOn);
			delete autoStartDialog;
		}
	}
	else if (aMenuId == R_FIXAPPORDER_MENU)
	{
		if (!ConeUtils::FileExists(KAppConfigFileBackup))
			aMenuPane->SetItemDimmed(cmdUtilsFixAppOrderRestore, ETrue);
	}
	else if (viewId == EViewAgenda)
	{
		if (aMenuId == R_AGENDA_COMPLETED_MENU)
		{
			if (configData->agendaShowCompleteCalendar)
				aMenuPane->SetItemButtonState(cmdAgendaCompleteCalendar, EEikMenuItemSymbolOn);
			if (configData->agendaShowCompleteTodo)
				aMenuPane->SetItemButtonState(cmdAgendaCompleteTodo, EEikMenuItemSymbolOn);
		}
		else if (aMenuId == R_AGENDA_AUTOEXPAND_MENU)
		{
			if (configData->agendaAutoExpandCalendar)
				aMenuPane->SetItemButtonState(cmdAgendaAutoExpandCalendar, EEikMenuItemSymbolOn);
			if (configData->agendaAutoExpandTodo)
				aMenuPane->SetItemButtonState(cmdAgendaAutoExpandTodo, EEikMenuItemSymbolOn);
		}
	}
	else if (viewId == EViewDB)
	{
		if (aMenuId == R_EDIT)
		{
			if (iDBView->dbIsOpened)
			{
				if ((iDBView->currentDBOp == CSMan2DBView::DBOp_Editing) || 
					(iDBView->currentDBOp == CSMan2DBView::DBOp_Adding))
				{
					aMenuPane->SetItemDimmed(cmdEditCut, EFalse);
					aMenuPane->SetItemDimmed(cmdEditCopy, EFalse);
					aMenuPane->SetItemDimmed(cmdEditPaste, EFalse);
				}
			}
		}
		if (aMenuId == R_DB_OPTIONS)
		{
			if (!iDBView->dbIsOpened)
			{
				aMenuPane->SetItemDimmed(cmdDBUpdateStats, ETrue);
				aMenuPane->SetItemDimmed(cmdDBCompact, ETrue);
				aMenuPane->SetItemDimmed(cmdDBChangePassword, ETrue);
				aMenuPane->SetItemDimmed(cmdDBSortAsc, ETrue);
				aMenuPane->SetItemDimmed(cmdDBSortDes, ETrue);
				aMenuPane->SetItemDimmed(cmdDBCloseDB, ETrue);
			}
			else if (iDBView->dbIsOpened)
			{
				aMenuPane->SetItemDimmed(cmdDBReadDB, ETrue);
				if (iDBView->currentDBView != CSMan2DBView::DBView_List)
				{
					aMenuPane->SetItemDimmed(cmdDBChangePassword, ETrue);
					aMenuPane->SetItemDimmed(cmdDBReadDB, ETrue);
					aMenuPane->SetItemDimmed(cmdDBSortAsc, ETrue);
					aMenuPane->SetItemDimmed(cmdDBSortDes, ETrue);
					aMenuPane->SetItemDimmed(cmdDBUpdateStats, ETrue);
					aMenuPane->SetItemDimmed(cmdDBCompact, ETrue);
				}
			}
			if (configData->dbSortAscending)
				aMenuPane->SetItemButtonState(cmdDBSortAsc, EEikMenuItemSymbolOn);
			else
				aMenuPane->SetItemButtonState(cmdDBSortDes, EEikMenuItemSymbolOn);
		}
		else if (aMenuId == R_DB_DOBUTTON)
		{
			if (!iDBView->dbIsOpened)
			{
				aMenuPane->SetItemDimmed(cmdDBAdd, ETrue);
				aMenuPane->SetItemDimmed(cmdDBEdit, ETrue);	
				aMenuPane->SetItemDimmed(cmdDBView, ETrue);
				aMenuPane->SetItemDimmed(cmdDBDel, ETrue);
				aMenuPane->SetItemDimmed(cmdDBSave, ETrue);
			}
			else
			{
				switch (iDBView->currentDBOp)
				{
					case CSMan2DBView::DBOp_Browsing:
					{
						aMenuPane->SetItemDimmed(cmdDBSave, ETrue);
						break;
					}
					case CSMan2DBView::DBOp_Editing:
					{
						aMenuPane->SetItemDimmed(cmdDBAdd, ETrue);
						aMenuPane->SetItemDimmed(cmdDBEdit, ETrue);	
						aMenuPane->SetItemDimmed(cmdDBDel, ETrue);
						aMenuPane->SetItemDimmed(cmdDBView, ETrue);
						break;
					}
					case CSMan2DBView::DBOp_Adding:
					{
						aMenuPane->SetItemDimmed(cmdDBAdd, ETrue);	
						aMenuPane->SetItemDimmed(cmdDBEdit, ETrue);	
						aMenuPane->SetItemDimmed(cmdDBDel, ETrue);
						aMenuPane->SetItemDimmed(cmdDBView, ETrue);
						break;
					}
					case CSMan2DBView::DBOp_Viewing:
					{
						aMenuPane->SetItemDimmed(cmdDBAdd, ETrue);	
						aMenuPane->SetItemDimmed(cmdDBDel, ETrue);
						aMenuPane->SetItemDimmed(cmdDBView, ETrue);
						aMenuPane->SetItemDimmed(cmdDBSave, ETrue);
						break;
					}
				}
			}
		}
	}
	else if (viewId == EViewFile)
	{
		if (aMenuId == R_FILEMAN_DOBUTTON)
		{
			iFileView->GetFile();
			
			if (iFileView->cFileListBox->SelectionIndexes()->Count() == 0)
			{
				aMenuPane->SetItemDimmed(cmdFileManCut, ETrue);
				aMenuPane->SetItemDimmed(cmdFileManCopy, ETrue);
			}
			
			if ((iFileView->iEntryType.Compare(_L("<drv>")) == 0) || (iFileView->iEntryName.Compare(_L("..")) == 0) || (iFileView->iEntryType.Compare(_L("<dir>")) == 0))
			{
				// This command exists in the hrh file but is not associated with any
				// menu pane. It is programmatically issued by the file view when you tap
				// on a file
				//aMenuPane->SetItemDimmed(cmdFileManOpen, ETrue);
				
				aMenuPane->SetItemDimmed(cmdFileManOpenOverride, ETrue);
				aMenuPane->SetItemDimmed(cmdFileManCRC, ETrue);
				aMenuPane->SetItemDimmed(cmdFileManHexEdit, ETrue);
			}
			
			if ((iFileView->iEntryType.Compare(_L("<drv>")) == 0) || (iFileView->iEntryName.Compare(_L("..")) == 0))
			{
				aMenuPane->SetItemDimmed(cmdFileManProperties, ETrue);
				aMenuPane->SetItemDimmed(cmdFileManCopyAs, ETrue);
			}
			
			TBool iBJackScannerActive = EFalse;
			if (iBJackView)
				if (iBJackView->oScanner->IsActive())
					iBJackScannerActive = ETrue;
			if ((iBJackScannerActive) || (iFileView->blueBeam->IsActive()))
				aMenuPane->SetItemDimmed(cmdFileManBeam, ETrue);
			else
			{
				TBool hasFiles = EFalse;
				TPtrC entryType, tempText;
				
				for (int i = 0; i < iFileView->cFileListBox->SelectionIndexes()->Count(); i++)
				{
					tempText.Set(iFileView->cFileListBox->Model()->ItemTextArray()->MdcaPoint((iFileView->cFileListBox->SelectionIndexes()->At(i))));
					TextUtils::ColumnText(entryType, 1, &tempText);
					if (entryType.Compare(_L("<dir>")) != 0)
					{
						hasFiles = ETrue;
						break;
					}
				}
				if (!hasFiles)
					aMenuPane->SetItemDimmed(cmdFileManBeam, ETrue);
			}
			
			if (iFileView->NumItemsInClipboard() < 1)
				aMenuPane->SetItemDimmed(cmdFileManPaste, ETrue);
			
			if (iFileView->cFileListBox->SelectionIndexes()->Count() < 1)
			{
				aMenuPane->SetItemDimmed(cmdFileManDelete, ETrue);
				aMenuPane->SetItemDimmed(cmdFileManRename, ETrue);
			}
			
			// Handle manufacturer specific functionality
			if (!PhoneIsCapable(EPhone_HasFixedWidthFont))
				aMenuPane->SetItemDimmed(cmdFileManHexEdit, ETrue);
				
			// No irda + bluetooth for this bugger
			if (!PhoneIsCapable(EPhone_HasIRDA) && !PhoneIsCapable(EPhone_HasBluetooth))
				aMenuPane->SetItemDimmed(cmdFileManBeam, ETrue);
		}
		else if (aMenuId == R_FILE_DRIVES_MENU)
		{
			// Get volume + free space information for all the drives.
			
			TVolumeInfo volInfo;
			TInt err, i;
			TBuf<KMaxFileName + 30> desc;
			TBuf<3> drivePrefix;
			CEikMenuPaneItem::SData itemData;
			
			for (i = 0; i < KMaxDrives; i++)
			{
				err = CEikonEnv::Static()->FsSession().Volume(volInfo, EDriveA + i);
				if (err == KErrNone)
				{
					if (volInfo.iDrive.iType != EMediaRom)
						sizeToText(volInfo.iFree, &desc);
					else
						desc.Copy(_L("](ROM)"));
						
					desc.Insert(0, volInfo.iName);
					drivePrefix.Format(_L("%c:["), 0x41 + i);
					desc.Insert(0, drivePrefix);
					 
					itemData.iText.Copy(desc);
					itemData.iCommandId = cmdFileMan_Drive_A + i;
					itemData.iFlags = 0;
					itemData.iCascadeId = 0;
					aMenuPane->AddMenuItemL(itemData);					
				}				
			}
		}
		else if (aMenuId == R_FILE_OPTIONS_MENU)
		{
			if (configData->beamProtocol == CBlueBeam::protocolIrDA)
				aMenuPane->SetItemButtonState(cmdFileMan_IrDA, EEikMenuItemSymbolOn);
			else if (configData->beamProtocol == CBlueBeam::protocolBluetooth)
				aMenuPane->SetItemButtonState(cmdFileMan_Bluetooth, EEikMenuItemSymbolOn);
				
			if (configData->showHiddenFiles)
				aMenuPane->SetItemButtonState(cmdFileManToggleHiddenView, EEikMenuItemSymbolOn);
			if (configData->autoRunApp)
				aMenuPane->SetItemButtonState(cmdFileManToggleRunApp, EEikMenuItemSymbolOn);
			if (configData->autoOpenFile)
				aMenuPane->SetItemButtonState(cmdFileManToggleOpenFile, EEikMenuItemSymbolOn);
				
			if (!PhoneIsCapable(EPhone_HasIRDA))
				aMenuPane->SetItemDimmed(cmdFileMan_IrDA, ETrue);
				
			if (!PhoneIsCapable(EPhone_HasBluetooth))
				aMenuPane->SetItemDimmed(cmdFileMan_Bluetooth, ETrue);
		}
		else if (aMenuId == R_FILE_SORT_MENU)
		{
			if (configData->filemanSortBy == ESortByName)
				aMenuPane->SetItemButtonState(cmdFileMan_SortName, EEikMenuItemSymbolOn);
			else if (configData->filemanSortBy == ESortBySize)
				aMenuPane->SetItemButtonState(cmdFileMan_SortSize, EEikMenuItemSymbolOn);
			else if (configData->filemanSortBy == ESortByDate)
				aMenuPane->SetItemButtonState(cmdFileMan_SortDate, EEikMenuItemSymbolOn);
				
			if (configData->filemanSortOrder == EAscending)
				aMenuPane->SetItemButtonState(cmdFileMan_SortAsc, EEikMenuItemSymbolOn);
			else if (configData->filemanSortOrder == EDescending)
				aMenuPane->SetItemButtonState(cmdFileMan_SortDesc, EEikMenuItemSymbolOn);
		}
	}
	else if (viewId == EViewHexEditor)
	{
		if (aMenuId == R_HEXEDITOR_EDIT)
		{
			// If the file is in ROM, the setting in configData is ignored and the hexeditor
			// will always be in readonly mode.
			if ((iHexEditorView->FileIsInROM()) || (configData->iHexEditIsReadOnly))
				aMenuPane->SetItemButtonState(cmdHexEditorEditToggleReadOnly, EEikMenuItemSymbolOn);
			if ((iHexEditorView->IsAtLastPage()) && (!iHexEditorView->FileIsInROM()) && (!configData->iHexEditIsReadOnly)
				&& (iHexEditorView->HasSelection()))
				aMenuPane->SetItemDimmed(cmdHexEditorEditDeleteBytes, EFalse);				
		}
		else if (aMenuId == R_HEXEDITOR_FILE)
		{
			if (configData->iHexEditPreserveDate)
				aMenuPane->SetItemButtonState(cmdHexEditorFilePreserveDate, EEikMenuItemSymbolOn);
		}
	}
	else if (viewId == EViewBJack)
	{
		if (aMenuId == R_OPTIONS_BLUEJACK_MENU)
		{
			if (configData->showBluejackSysMsg)
				aMenuPane->SetItemButtonState(cmdBluejackSysMsg, EEikMenuItemSymbolOn);
			if (configData->resolveName)
				aMenuPane->SetItemButtonState(cmdResolveName, EEikMenuItemSymbolOn);
			if (configData->autoSave)
				aMenuPane->SetItemButtonState(cmdBluejackAutoSave, EEikMenuItemSymbolOn);
			if (configData->proxSweepOnce)
				aMenuPane->SetItemButtonState(cmdBluejackOnce, EEikMenuItemSymbolOn);
			if (iBJackView->oScanner->IsActive())
				aMenuPane->SetItemDimmed(cmdBluejackExcludeList, ETrue);
		}
		else if (aMenuId == R_BLUEJACK_FILELOG_MENU)
		{
			if (iBJackView->oScanner->IsActive())
			{
				aMenuPane->SetItemDimmed(cmdBluejackDelFileLog, ETrue);
				aMenuPane->SetItemDimmed(cmdBluejackViewFileLog, ETrue);
			}
		}
	}
	else if (viewId == EViewTask)
	{
		if (aMenuId == R_EXTRAS_MENU)
		{
			if (configData->flipFlush)
				aMenuPane->SetItemButtonState(cmdFlipFlush, EEikMenuItemSymbolOn);
			if (configData->forceEndTask)
				aMenuPane->SetItemButtonState(cmdForceEndTask, EEikMenuItemSymbolOn);
			if (configData->showHiddenTasks)
				aMenuPane->SetItemButtonState(cmdShowHiddenTask, EEikMenuItemSymbolOn);
			if (configData->iFlushExclude)
				aMenuPane->SetItemButtonState(cmdFlushExclude, EEikMenuItemSymbolOn);
		}
	}
	else if (viewId == EViewFCBJack)
	{
		if (aMenuId == R_FC_BLUEJACK_MENU)
		{
			if (iBJackView)
			{
				if (iBJackView->oScanner->IsActive())
				{
					HBufC* dataBuffer = CEikonEnv::Static()->AllocReadResourceL(R_TBUF_MISC_FC_BJACK_SWEEP);
					aMenuPane->SetItemTextL(cmdBluejack, *dataBuffer);
					delete dataBuffer;
				}
			}
		}
	}
	else if (viewId == EViewDTMFDialer)
	{
		if ((aMenuId == R_DTMF_EDIT) && (configData->iDTMFInput))
		{
			aMenuPane->SetItemDimmed(cmdDTMFEditCut, EFalse);
			aMenuPane->SetItemDimmed(cmdDTMFEditCopy, EFalse);
			aMenuPane->SetItemDimmed(cmdDTMFEditPaste, EFalse);
			aMenuPane->SetItemDimmed(cmdDTMFEditClear, EFalse);
		}
		
		if (aMenuId == R_DTMFDIALER_DIAL)
		{
			if (!configData->iDTMFInput)
				aMenuPane->SetItemButtonState(cmdDTMFDialerInput, EEikMenuItemSymbolOn);
		}
	}
	else if (viewId == EViewCellArea)
	{
		if (aMenuId == R_CELLAREA_OPTIONS)
		{
			if (configData->iCellAreaAlwaysRun)
				aMenuPane->SetItemButtonState(cmdCellAreaAlwaysRun, EEikMenuItemSymbolOn);
			else
				aMenuPane->SetItemDimmed(cmdCellAreaToggleRecording, ETrue);
			if (iCellAreaView->IsRecording())
				aMenuPane->SetItemButtonState(cmdCellAreaToggleRecording, EEikMenuItemSymbolOn);
		}
		else if (aMenuId == R_CELLAREA_EDIT)
		{
			if (iCellAreaView->IsRecording())
			{
				aMenuPane->SetItemDimmed(cmdCellAreaEdit, ETrue);
				aMenuPane->SetItemDimmed(cmdCellAreaCompress, ETrue);
				aMenuPane->SetItemDimmed(cmdCellAreaUpdateStats, ETrue);
				aMenuPane->SetItemDimmed(cmdCellAreaInitStore, ETrue);
			}
		}
	}
	else if (viewId == EViewFCCellArea)
	{
		if (aMenuId == R_FC_CELLAREA_MENU)
		{
			if (configData->iCellAreaAlwaysRun)
				aMenuPane->SetItemButtonState(cmdCellAreaAlwaysRun, EEikMenuItemSymbolOn);
			else
				aMenuPane->SetItemDimmed(cmdCellAreaToggleRecording, ETrue);
			if (iCellAreaView->IsRecording())
				aMenuPane->SetItemButtonState(cmdCellAreaToggleRecording, EEikMenuItemSymbolOn);
		}
	}

	// Disable manufacturer specific menus. Some of this code can be found above this
	// member
	if (aMenuId == R_FN_UTILITIES_MENU)
	{
		if (iRunningPhoneModel != EPhoneModel_P900)
			aMenuPane->SetItemDimmed(cmdUtilsP900toP800, ETrue);
		if (!PhoneIsCapable(EPhone_HasAppListBug))
			aMenuPane->SetItemDimmed(cmdUtilsFixApplist, ETrue);
		if (!PhoneIsCapable(EPhone_HasTimeSynchZoneBug))
			aMenuPane->SetItemDimmed(cmdUtilsAgendaFixSynchTimeZone, ETrue);
		if (!PhoneIsCapable(EPhone_CanChangeFont))
			aMenuPane->SetItemDimmed(cmdUtilsFontSelector, ETrue);
		if (!PhoneIsCapable(EPhone_CanFixAppOrder))
			aMenuPane->SetItemDimmed(cmdCascadeFixAppOrder, ETrue);
		if (!PhoneIsCapable(EPhone_CanAutoStart))
			aMenuPane->SetItemDimmed(cmdAutoStartCascade, ETrue);		
	}
	else if (aMenuId == R_CONTROLPANEL_MENU)
	{
		if (!PhoneIsCapable(EPhone_HasIRDACPL))
			aMenuPane->SetItemDimmed(cmdCPIrda, ETrue);
		if (!PhoneIsCapable(EPhone_HasPhoneCPL))
			aMenuPane->SetItemDimmed(cmdCPPhone, ETrue);
		if (!PhoneIsCapable(EPhone_HasBluetoothCPL))
			aMenuPane->SetItemDimmed(cmdCPBluetooth, ETrue);
	}
	else if (aMenuId == R_FIXAPPORDER_MENU)
	{
		if (!PhoneIsCapable(EPhone_CanFixAppOrder))
		{
			aMenuPane->SetItemDimmed(cmdUtilsFixAppOrderReset, ETrue);
			aMenuPane->SetItemDimmed(cmdUtilsFixAppOrderRestore, ETrue);
			aMenuPane->SetItemDimmed(cmdUtilsFixAppOrderBackup, ETrue);
		}
	}
	else if ((aMenuId == R_FC_CELLAREA_MENU) || (aMenuId == R_FC_BLUEJACK_MENU))
	{
		if (!PhoneIsCapable(EPhone_HasFlip))
		{
			aMenuPane->SetItemDimmed(cmdFCSwitchMenu, ETrue);
			aMenuPane->SetItemDimmed(cmdUtilsFixApplist, ETrue);
			aMenuPane->SetItemDimmed(cmdBluejack, ETrue);
			aMenuPane->SetItemDimmed(cmdBluejackClearLog, ETrue);
		}
	}
	else if (aMenuId == R_EXTRAS_MENU)
	{
		if (!PhoneIsCapable(EPhone_HasFlip))
			aMenuPane->SetItemDimmed(cmdFlipFlush, ETrue);
		if (!PhoneIsCapable(EPhone_CanSetZoomFont))
			aMenuPane->SetItemDimmed(cmdZoom, ETrue);
	}
	else if ((aMenuId == R_OPTIONS_BLUEJACK_MENU) ||
		(aMenuId == R_SYSINFO_OPTIONS) || (aMenuId == R_DB_OPTIONS) ||
		(aMenuId == R_FILE_OPTIONS_MENU) || (aMenuId == R_AGENDA_OPTIONS) ||
		(aMenuId == R_FINDFILE_OPTIONS))
	{
		if (!PhoneIsCapable(EPhone_CanSetZoomFont))
			aMenuPane->SetItemDimmed(cmdZoom, ETrue);
	}
}

TBool CSMan2AppUi::IsHexEditorEditingFile(TFileName *iFileName)
{
	TBool iRetVal = EFalse;
	TFileName iUpCaseHexEditorFileName;
	
	iUpCaseHexEditorFileName.Copy(iHexEditorView->iFileName);
	iUpCaseHexEditorFileName.UpperCase();
	
	if ((iUpCaseHexEditorFileName.Compare(*iFileName) == 0) && (iHexEditorView->iFileIsOpen))
		iRetVal = ETrue;

	return iRetVal;

}

void CSMan2AppUi::FixIfContextErrorIsFileName(TDes *iContextError, TFileName *iSManResourceFileName)
{
	HBufC *iTempBuffer = HBufC::NewL(iContextError->Length());
	iTempBuffer->Des().Copy(*iContextError);
	iTempBuffer->Des().UpperCase();
	if (iTempBuffer->Des().Compare(*iSManResourceFileName) == 0)
	{
		// Not strictly necessary but I'm paranoid :)
		delete iTempBuffer;
		
		// At this point, the call to ActivateViewL for the hex editor did not complete
		// successfully. 
		iHexEditorView->ReleaseFileAndBuffer();
		ActivateViewL(TVwsViewId(KUidSMan2App, KUidFileView));
		switchView(EViewFile);
		CEikonEnv::Static()->InfoMsg(R_TBUF_HEXEDIT_SMANFILE);
	}
	else
		delete iTempBuffer;
}

TErrorHandlerResponse CSMan2AppUi::HandleError(TInt aError, const SExtendedError& /*aExtErr*/, TDes& /*aErrorText*/, TDes& aContextText)
{
	// Check if our hex editor is causing a lock on our bitmap file or resource file
	// The problem is: when hexeditor opens the MBM file, it acquires a lock on it. While switching
	// view to the hexeditor, UIQ's framework needs to read the MBM file for the toolbar icons. This
	// will fail and the current view will be in an inconsistent state (the menubar, toolbar etc
	// will be wrong). To be safe, I have also included checks for the resource file. I haven't seen
	// this as being necessary but who knows....
	//
	// When there is such an error, the following parameters of this method are set as:
	//
	//	acontexttext = name of file
	//	aerrortext = "In use"
	//	aerror = -1001
	//
	// I don't think it's safe to check aerrortext as symbian may change this text. aerror is 
	// likely a better way to check for the "In use" error

	if (aError == -1001)
	{
		TFileName iFileName;
		iFileName.Copy(CEikonEnv::Static()->EikAppUi()->Application()->BitmapStoreName());
		iFileName.UpperCase();
		if (IsHexEditorEditingFile(&iFileName))
		{
			FixIfContextErrorIsFileName(&aContextText, &iFileName);
		}
		else
		{
			iFileName.Copy(CEikonEnv::Static()->EikAppUi()->Application()->ResourceFileName());
			iFileName.UpperCase();
			if (IsHexEditorEditingFile(&iFileName))
			{
				FixIfContextErrorIsFileName(&aContextText, &iFileName);
			}
		}
	}
	return EErrorNotHandled;
}

/*************************************************************
*
* External interface
*
**************************************************************/

/*
CSMan2Application::CSMan2Application()
{
	iSManActiveScheduler = new (ELeave) CSManActiveScheduler();
	delete (CActiveScheduler::Replace(iSManActiveScheduler));
}

CSMan2Application::~CSMan2Application()
{
	delete iSManActiveScheduler;
}
*/

TUid CSMan2Application::AppDllUid() const
{
	return KUidSMan2App;
}

GLDEF_C TInt E32Dll(TDllReason)
{
	return KErrNone;
}

EXPORT_C CApaApplication* NewApplication()
{
	return new CSMan2Application;
}