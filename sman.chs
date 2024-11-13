#include <eikon.rh>
#include <uikon.hrh>
#include <eikon.rsg>
#include <qikon.rh>
#include "sman.hrh"
#include <quartz.mbg>

//#define __DEBUG_BUILD__

NAME sman

CHARACTER_SET UTF8

RESOURCE RSS_SIGNATURE { }

RESOURCE TBUF { buf=""; }

RESOURCE EIK_APP_INFO
{
	menubar=r_memory_menubar;
}

RESOURCE MENU_PANE r_options_autostart_menu
{
	items=
	{
		MENU_ITEM { command=cmdAutostartSMan; txt="SMan"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdAutostartOthers; txt="其它程序"; flags = EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdAutostartActive; txt="启用"; }
	};
}

RESOURCE MENU_PANE r_options_bluejack_menu
{
	items=
	{
		MENU_ITEM { command=cmdNoDevicePause; txt="时限设置"; },
		MENU_ITEM { command=cmdLogFlags; txt="扫描记录设置"; },
		MENU_ITEM { command=cmdBluejackSysMsg; txt="通知方式设置";},
		MENU_ITEM { command=cmdBluejackMsg; txt="信息设置"; },
		MENU_ITEM { command=cmdBluejackExcludeList; txt="忽略清单设置"; flags=EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdResolveName; txt="解析设备名称"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdBluejackOnce; txt="单次扫描"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdBluejackAutoSave; txt="记录到磁盘"; flags=EEikMenuItemCheckBox | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdZoom; txt = "缩放"; }
	};
}

RESOURCE MENU_PANE r_runapplist_menu
{
	items=
	{
	};
}
	
RESOURCE MENU_PANE r_shortcuts_menu
{
	items=
	{
		MENU_ITEM { cascade=r_runapplist_menu; txt="运行"; },
		MENU_ITEM { command=cmdShortcuts; txt="设置开始快捷方式";  flags = EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdShort1; txt="<未指定>"; flags = EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdShort2; txt="<未指定>"; flags = EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdShort3; txt="<未指定>"; flags = EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdShort4; txt="<未指定>"; flags = EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdShort5; txt="<未指定>"; flags = EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdShort6; txt="<未指定>"; flags = EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdShort7; txt="<未指定>"; flags = EEikMenuItemDimmed; }
	};
}	

RESOURCE ARRAY r_fn_array
{
	items=
	{	
		LBUF { txt="任务管理"; },
		LBUF { txt="文件管理"; },
		LBUF { txt="蓝牙接口"; },
		LBUF { txt="信息存储"; },
		LBUF { txt="系统信息"; },
		LBUF { txt="今日视图"; },
		LBUF { txt="DTMF 拨号器"; },
		LBUF { txt="网络信息"; }
	};	
}

RESOURCE DIALOG r_dialog_fn_options
{
	title = "功能设置";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			prompt = "使用模组：";
			type = EEikCtListBox;
			id = cFnModuleList;
			control = LISTBOX
			{
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				array_id = r_fn_array;
			};
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "开始模组：";
			id = cFnStartupView;
			control = CHOICELIST { array_id = r_fn_array; };
		}
	};
}	

RESOURCE MENU_PANE r_modules_menu
{
	items =
	{
		MENU_ITEM { command=cmdSwitchMemory; txt="任务管理"; },
		MENU_ITEM { command=cmdSwitchFile; txt="文件管理"; },
		MENU_ITEM { command=cmdSwitchSysInfo; txt="系统信息"; },
		MENU_ITEM { command=cmdSwitchBJack; txt="蓝牙接口"; },
		MENU_ITEM { command=cmdSwitchDB; txt="信息存储"; },
		MENU_ITEM { command=cmdSwitchAgenda; txt="今日视图"; },
		MENU_ITEM { command=cmdSwitchDTMFDialer; txt="DTMF 拨号器"; },
		MENU_ITEM { command=cmdSwitchCellArea; txt="网络信息"; }
	};
}
	
RESOURCE MENU_PANE r_generic_fn
{
	items =
	{
	   	MENU_ITEM { cascade=r_shortcuts_menu; txt="开始"; },
	   	MENU_ITEM { cascade=r_modules_menu; txt="模组"; },
		MENU_ITEM { cascade=r_fn_utilities_menu; txt="工具"; },
		MENU_ITEM { cascade=r_controlpanel_menu; txt="控制面板"; flags=EEikMenuItemSeparatorAfter; },
		MENU_ITEM { cascade=r_fn_options_menu; txt="选项"; },
		MENU_ITEM { command=cmdAbout; txt="关于"; },
		MENU_ITEM { command=cmdQuit; txt="离开"; }
	};
}

RESOURCE MENU_PANE r_fn_options_menu
{
	items = 
	{
		MENU_ITEM { command=cmdSwitchOptions; txt="设置模组"; flags=EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdHotkey; txt="热键"; },
		MENU_ITEM { command=cmdNoHotkey; txt="关闭热键"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdPersist; txt="忽略 \"自杀要求\""; flags=EEikMenuItemCheckBox; }
	};
}

RESOURCE MENU_PANE r_sysinfo_options
{
	items=
	{
		MENU_ITEM { command=cmdSysInfoRefresh; txt="刷新"; },
		MENU_ITEM { command=cmdZoom; txt="缩放"; }
	};
}

RESOURCE MENU_PANE r_controlpanel_menu
{
	items =
	{
		MENU_ITEM { command=cmdCPBluetooth; txt="蓝牙"; },
		MENU_ITEM { command=cmdCPIrda; txt="红外线"; },
		MENU_ITEM { command=cmdCPPhone; txt="电话"; },
		MENU_ITEM { command=cmdControlPanel; txt="其它"; }
	};
}

RESOURCE MENU_PANE r_bluejacklog_menu
{
	items=
	{
		MENU_ITEM { command=cmdBluejackClearLog; txt="清除内存记录"; },
		MENU_ITEM { cascade=r_bluejack_filelog_menu; txt="文件记录"; }
	};
}

RESOURCE MENU_PANE r_bluejack_filelog_menu
{
	items =
	{
		MENU_ITEM { command=cmdBluejackViewFileLog; txt="查看"; },
		MENU_ITEM { command=cmdBluejackDelFileLog; txt="删除"; }
	};
}


RESOURCE MENU_PANE r_extras_menu
{
	items=
	{
		MENU_ITEM { command=cmdFlipFlush; txt="合盖退出程序"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdForceEndTask; txt="强制结束任务"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdFlushExclude; txt = "忽略清除内存"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdShowHiddenTask; txt="显示隐藏任务"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdZoom; txt = "缩放"; }
	};
}

RESOURCE MENU_BAR r_db_menubar
{
	titles=
   	{
		MENU_TITLE { menu_pane=r_db_options; txt="数据选项"; },
		MENU_TITLE { menu_pane=r_edit; txt="编辑"; },
		MENU_TITLE { menu_pane=r_db_data; txt="资料"; },
		MENU_TITLE { menu_pane=r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE MENU_BAR r_sysinfo_menubar
{
	titles=
   	{
		MENU_TITLE { menu_pane=r_sysinfo_options; txt="选项"; },
		MENU_TITLE { menu_pane=r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE MENU_PANE r_edit
{
	items=
	{
		MENU_ITEM { command=cmdEditCut; txt="剪切"; flags=EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdEditCopy; txt="复制"; flags=EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdEditPaste; txt="粘贴"; flags=EEikMenuItemDimmed; }
	};
}

RESOURCE MENU_PANE r_db_options
{
	items=
	{
		MENU_ITEM { command=cmdDBReadDB; txt="打开数据库"; },
		MENU_ITEM { command=cmdDBCloseDB; txt="关闭数据库"; flags=EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdDBChangePassword; txt="修改密码"; },
		MENU_ITEM { command=cmdDBReInit; txt="重建数据库"; },
		MENU_ITEM { command=cmdDBUpdateStats; txt="更新统计信息"; },
		MENU_ITEM { command=cmdDBCompact; txt="压缩"; flags=EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdDBSortAsc; txt = "升序排序"; flags = EEikMenuItemRadioStart; },
		MENU_ITEM { command=cmdDBSortDes; txt = "降序排序"; flags = EEikMenuItemRadioEnd | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdZoom; txt="缩放"; }
	};
}

RESOURCE MENU_PANE r_db_data
{
	items=
	{
		MENU_ITEM { command=cmdDBExportDB; txt="导出数据库"; },
		MENU_ITEM { command=cmdDBImportDB; txt="导入数据库"; }
	};
}

RESOURCE MENU_PANE r_fc_bluejack_menu
{
	items=
	{
		MENU_ITEM { cascade=r_fc_moduleswitch_menu; command=cmdFCSwitchMenu; txt="模组"; },
		MENU_ITEM { command=cmdFlushOut; txt="退出程序"; },
		MENU_ITEM { command=cmdUtilsFixApplist; txt="修复程序列表";  flags=EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdBluejack; txt="开始扫描"; },
		MENU_ITEM { command=cmdBluejackClearLog; txt="清除内存记录"; }
	};	
}

RESOURCE MENU_BAR r_bluejack_menubar
{
	titles=
   	{
	   	MENU_TITLE { menu_pane=r_options_bluejack_menu; txt="选项"; },
	   	MENU_TITLE { menu_pane=r_bluejacklog_menu; txt="记录"; },
		MENU_TITLE { menu_pane=r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE MENU_BAR r_memory_menubar
{
	titles=
   	{
	   	MENU_TITLE { menu_pane=r_extras_menu; txt="选项"; },
		MENU_TITLE { menu_pane=r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE DIALOG r_dialog_autostart
{
	title = "自动启动设置";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "程序 1：";
			id = cAutoStart1;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "程序 2：";
			id = cAutoStart2;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "程序 3：";
			id = cAutoStart3;
			control = CHOICELIST { };
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_nodevicepause
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "默认"; }; },
		DLG_BUTTON { id = EEikBidCancel; button = CMBUT { txt = "取消"; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "确定"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

RESOURCE DIALOG r_dialog_loglevels
{
	title = "扫描记录设置";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait | EEikDialogFlagButtonsRight;
	items=
	{
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "蓝牙接口记录：";
			id = cLogBluejack;
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "OBEX 记录：";
			id = cLogOBEX;
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "扫描记录：";
			id = cLogScanning;
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "衰减记录：";
			id = cLogDecays;
		}
	};
}

RESOURCE ARRAY r_array_audionotify
{
	items=
	{	
		LBUF { txt="<无>"; },
		LBUF { txt="选择文件"; }
	};	
}

RESOURCE DIALOG r_dialog_notification
{
	title = "蓝牙接口通知";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items=
	{
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "信息：";
			id = cSystemMsgNotify;
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "震动提示：";
			id = cVibraNotify;
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "声响：";
			id = cAudioFileName;
			control = CHOICELIST { };
		}			
	};
}

RESOURCE DIALOG r_dialog_nodevicepause
{
	title = "蓝牙接口时限";
	buttons = r_buttons_nodevicepause;
	flags = EEikDialogFlagWait;
	items=
	{
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "OBEX 超时 (秒)：";
			id = cOBEXTimeout;
			control = QIK_NUMBER_EDITOR { min = 10; max = 30; };
		},
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "衰减 (秒)：";
			id = cDecayTime;
			control = QIK_NUMBER_EDITOR { min = 5; max = 1800; };
		},
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "平静 (秒)：";
			id = cNoDevicePause;
			control = QIK_NUMBER_EDITOR { min = 1; max = 1800; };
		},
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "失败重试：";
			id = cNoDeviceAttempts;
			control = QIK_NUMBER_EDITOR { min = 1; max = 10; };
		}			
	};
}
			
RESOURCE DIALOG r_dialog_shortcut
{
	title = "开始";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait | EEikDialogFlagButtonsRight;
	items=
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "1：";
			id = cShortcut1;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "2：";
			id = cShortcut2;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "3：";
			id = cShortcut3;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "4：";
			id = cShortcut4;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "5：";
			id = cShortcut5;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "6：";
			id = cShortcut6;
			control = CHOICELIST { };
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "7：";
			id = cShortcut7;
			control = CHOICELIST { };
		}
	};
}

RESOURCE ARRAY r_hotkey_array_p800_p900
{
	items=
	{	
		LBUF { txt="<关闭>"; },
		LBUF { txt="摄像头键"; },
		LBUF { txt="互联网键"; },
		LBUF { txt="开盖"; }
	};	
}

RESOURCE ARRAY r_hotkey_array_a920_a925
{
	items=
	{	
		LBUF { txt="<关闭>"; },
		LBUF { txt="快捷方式"; },
		LBUF { txt="互联网键"; },
		LBUF { txt="<未使用>"; }
	};	
}

// The number after the semi-colon is the shortcut number that is stored in sman's config file
// By doing it this way, we can insert any arbitrary item in the middle and have it sorted in
// alphabetical order for the user to easily locate whatever he / she wants and still not worry
// about the value stored in the config file being wrong
RESOURCE ARRAY r_hotkey_choices_fo
{
	items =
	{
		LBUF { txt = "<关闭>;0"; },
		LBUF { txt = "自动运行 -- 其它;1"; },
		LBUF { txt = "自动运行 -- SMan 开关;2"; },
		LBUF { txt = "自动运行 -- 开关;3"; },
		LBUF { txt = "控制面板 -- 蓝牙;4"; },
		LBUF { txt = "控制面板 -- 红外线;5"; },
		LBUF { txt = "控制面板 -- 控制面板;6"; },
		LBUF { txt = "控制面板 -- 电话;7"; },
		LBUF { txt = "其它 -- 退出程序;8"; },
		LBUF { txt = "其它 -- 展开选单;9"; },
		LBUF { txt = "工具 -- 压缩联系人;10"; },
		LBUF { txt = "工具 -- 修复程序列表;11"; },
		LBUF { txt = "工具 -- 修复同步时区;12"; },
		LBUF { txt = "工具 -- 设置系统字体;13"; },
		LBUF { txt = "查看 -- 蓝牙接口;14"; },
		LBUF { txt = "查看 -- DTMF 拨号器;15"; },
		LBUF { txt = "查看 -- 文件管理;16"; },
		LBUF { txt = "查看 -- 数据管理;17"; },
		LBUF { txt = "查看 -- 网络信息;18"; },
		LBUF { txt = "查看 -- 系统信息;19"; },
		LBUF { txt = "查看 -- 任务管理;20"; },
		LBUF { txt = "查看 -- 今日视图;21"; }
	};
}

// Note: These numbers MUST match the flip open counterpart. Only if there is specific 
// functionality to either FO or FC mode will there be differences; in that event, the numbers
// MUST be UNIQUE across the FO + FC modes
RESOURCE ARRAY r_hotkey_choices_fc
{
	items =
	{
		LBUF { txt = "<关闭>;0"; },
		LBUF { txt = "自动启动 -- SMan 开关;2"; },
		LBUF { txt = "自动启动 -- 开关;3"; },
		LBUF { txt = "其它 -- 退出程序;8"; },
		LBUF { txt = "其它 -- 导航;9"; },
		LBUF { txt = "工具 -- 压缩联系人;10"; },
		LBUF { txt = "工具 -- 修复程序列表;11"; },
		LBUF { txt = "查看 -- 蓝牙接口;14"; },
		LBUF { txt = "查看 -- 网络信息;18"; }
	};
}

RESOURCE ARRAY r_dialog_hotkey_pages_key
{
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "热键：";
			id = cHotkeyControl;
			control = CHOICELIST 
			{
				array_id = r_hotkey_array_p800_p900;
			};
		}
	};
}

RESOURCE ARRAY r_dialog_hotkey_pages_fo
{
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "前景 (短按)：";
			id = cHotkeyInFocusShortFO;
			control = CHOICELIST {};
		},		
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "前景 (长按)：";
			id = cHotkeyInFocusLongFO;
			control = CHOICELIST {};
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "背景 (短按)：";
			id = cHotkeyExFocusShortFO;
			control = CHOICELIST {};
		},		
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "背景 (长按)：";
			id = cHotkeyExFocusLongFO;
			control = CHOICELIST {};
		}
	};
}

RESOURCE ARRAY r_dialog_hotkey_pages_fc
{
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "前景 (短按)：";
			id = cHotkeyInFocusShortFC;
			control = CHOICELIST {};
		},		
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "前景 (长按)：";
			id = cHotkeyInFocusLongFC;
			control = CHOICELIST {};
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "背景 (短按)：";
			id = cHotkeyExFocusShortFC;
			control = CHOICELIST {};
		},		
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "背景 (长按)：";
			id = cHotkeyExFocusLongFC;
			control = CHOICELIST {};
		}
	};
}

RESOURCE ARRAY r_dialog_hotkey_pages
{
	items = 
	{	
		PAGE
		{
			id = idHotkeyPage_Hotkey;
			text = "热键";
			lines = r_dialog_hotkey_pages_key;
		},
		PAGE
		{
			id = idHotkeyPage_FO;
			text = "开盖";
			lines = r_dialog_hotkey_pages_fo;
		},
		PAGE
		{
			id = idHotkeyPage_FC;
			text = "合盖";
			lines = r_dialog_hotkey_pages_fc;
		}
	};
}

RESOURCE DIALOG r_dialog_hotkey
{
	title = "热键设置";
	flags = EEikDialogFlagWait;
	pages = r_dialog_hotkey_pages;
	buttons = R_EIK_BUTTONS_CANCEL_OK;
}

RESOURCE DLG_BUTTONS r_buttons_continue_credits
{
	buttons =
	{
		#ifdef __DEBUG_BUILD__
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "除错"; }; },
		#endif
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "信用"; }; },
		DLG_BUTTON { id = EEikBidCancel; button = CMBUT { txt = "继续"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

RESOURCE DIALOG r_dialog_about
{
	title = "About";
	buttons = r_buttons_continue_credits;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtLabel;
			control = LABEL 
			{
				txt="The S Manager (c) 2003,2004";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			type = EEikCtLabel;
			control = LABEL 
			{
				txt="v1.3c, Build 202";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			type = EEikCtLabel;
			control = LABEL 
			{
				txt="程序制作：Yip Je Sum / droll";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			type = EEikCtLabel;
			control = LABEL 
			{
				txt="程序设计：Adrian Chiang / ajack";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			type = EEikCtLabel;
			control = LABEL 
			{
				txt="http://renegade.w3xs.com";
				horiz_align=EEikLabelAlignHLeft;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_snapshotconfirm
{
	title = "确认快照";
	buttons = R_EIK_BUTTONS_NO_YES;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtLabel;
			control = LABEL
			{
				txt = "你确定要为系统拍摄快照？";
			};
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_bluebeamlog
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "取消"; }; },
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "确定"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

RESOURCE DIALOG r_dialog_bluebeamlog
{
	title = "文件传送记录";
	buttons = r_buttons_bluebeamlog;
	items=
	{
		DLG_LINE
		{
			type = EEikCtRichTextEditor;
			id = cBluebeamLog;
			control = RTXTED 
			{ 
				width = 150; height = 170; numlines = 10; flags = EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinJustAutoCurEnd | EEikEdwinDisplayOnly;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_bluejacklog
{
	title = "蓝牙接口记录";
	buttons = R_EIK_BUTTONS_OK;
	flags=EEikDialogFlagWait;
	items=
	{
		DLG_LINE
		{
			type = EEikCtRichTextEditor;
			id = cBluejackLog;
			control = RTXTED 
			{ 
				width = 150; height = 170; numlines = 10; flags = EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinJustAutoCurEnd | EEikEdwinDisplayOnly;
			};
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_filemanprogress
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidOverwrite; button = CMBUT { txt = "覆盖"; }; },
		DLG_BUTTON { id = EEikBidOverwriteAll; button = CMBUT { txt = "全部覆盖"; }; },
		DLG_BUTTON { id = EEikBidSkip; button = CMBUT { txt = "忽略"; }; flags=EEikLabeledButtonIsDefault; },
		DLG_BUTTON { id = EEikBidSkipAll; button = CMBUT { txt = "全部忽略"; }; },
		DLG_BUTTON { id = EEikBidAbort; button = CMBUT { txt = "放弃"; }; }
	};
}

RESOURCE DIALOG r_dialog_filemanprogress_nowait
{
	title = "文件管理";
	buttons = r_buttons_filemanprogress;
	flags = EEikDialogFlagNoDrag;
	items=
	{
		DLG_LINE
		{
			type = EEikCtRichTextEditor;
			id = cFileManProgressText;
			control = RTXTED 
			{ 
				width = 150; height = 100; numlines = 10; flags = EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinJustAutoCurEnd | EEikEdwinDisplayOnly;
			};
		},
		DLG_LINE
		{
			prompt = "当前 ";
			type = EEikCtProgInfo;
			id = cFileManProgressBar;
			control = PROGRESSINFO
			{
				text_type = EEikProgressTextPercentage;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_filemanprogress_wait
{
	title = "文件管理";
	buttons = r_buttons_filemanprogress;
	flags = EEikDialogFlagWait | EEikDialogFlagNoDrag;
	items =
	{
		DLG_LINE
		{
			type = EEikCtRichTextEditor;
			id = cFileManProgressText;
			control = RTXTED 
			{ 
				width = 150; height = 100; numlines = 10; flags = EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinJustAutoCurEnd | EEikEdwinDisplayOnly;
			};
		},
		DLG_LINE
		{
			prompt = "当前 ";
			type = EEikCtProgInfo;
			id = cFileManProgressBar;
			control = PROGRESSINFO
			{
				text_type = EEikProgressTextPercentage;
			};
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_filemanrename
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidAbort; button = CMBUT { txt = "取消"; }; },
		DLG_BUTTON { id = EEikBidSkip; button = CMBUT { txt = "忽略"; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "确定"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

RESOURCE DIALOG r_dialog_filemanrename
{
	title = "重命名";
	buttons = r_buttons_filemanrename;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cFileManNewName;
			control = GTXTED 
			{ 
				height = 22; width = 21; numlines = 1; flags = EEikEdwinJustAutoCurEnd; textlimit = 256;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_filemannewfolder
{
	title = "新建文件夹";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cFileManNewName;
			control = GTXTED 
			{ 
				height = 22; width = 21; numlines = 1; flags = EEikEdwinJustAutoCurEnd;
			};
		}
	};
}

RESOURCE DIALOG r_bluejack_vcard
{
	title = "蓝牙接口信息";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cBluejackMessage;
			control = GTXTED 
			{ 
				textlimit = 20; height = 22; width = 21; numlines = 1; flags = EEikEdwinJustAutoCurEnd;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_filemanatt
{
	title = "文件属性";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cFileAttName;
			control = GTXTED 
			{ 
				height = 70; width = 150; numlines = 10; flags = EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinDisplayOnly;
			};
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "只读：";
			id = cFileAttReadOnly;
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "保存：";
			id = cFileAttArchive;
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "系统：";
			id = cFileAttSystem;
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "隐藏：";
			id = cFileAttHidden;
		},
/*		
		DLG_LINE
		{
			type = EQikCtDateEditor;
			prompt = "日期：";
			id = cFileModifiedDateTime;
			control = QIK_DATE_EDITOR
			{
				minDate = DATE
				{
					day = 0;
					month = 0;
					year = 1900;
				};
				maxDate = DATE
				{
					day = 30;
					month = 11;
					year = 2999;
				};
			};
		}
*/		
		DLG_LINE
		{
			type = EQikCtTimeAndDateEditor;
			prompt = "修改日期：";
			id = cFileModifiedDateTime;
			control = QIK_TIME_AND_DATE_EDITOR
			{
				minTimeAndDate = TIME_AND_DATE
				{
					minute = 0;
					hour = 0;
					day = 0;
					month = 0;
					year = 1900;
				};
				maxTimeAndDate = TIME_AND_DATE
				{
					minute = 59;
					hour = 23;
					day = 30;
					month = 11;
					year = 2999;
				};
			};
		}
	};
}

RESOURCE DIALOG r_dialog_fileman_crc
{
	title = "CRC 计算器";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	items=
	{
		DLG_LINE
		{
			prompt = "文件：";
			type = EEikCtLabel;
			id = cCRCFileName;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "CRC：";
			type = EEikCtLabel;
			id = cCRCValue;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "进度：";
			type = EEikCtProgInfo;
			id = cCRCProgress;
			control = PROGRESSINFO
			{
				text_type = EEikProgressTextPercentage;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_db_getpassword
{
	title = "必须密码校验";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtSecretEd;
			prompt = "密码：";
			id = cDBPassword;
			control = SECRETED
			{
				num_letters = 16;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_db_changepassword
{
	title = "修改密码";
	buttons = R_EIK_BUTTONS_CANCEL_OK;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtSecretEd;
			prompt = "输入密码：";
			id = cDBPassword;
			control = SECRETED
			{
				num_letters = 16;
			};
		},
		DLG_LINE
		{
			type = EEikCtSecretEd;
			prompt = "确认密码：";
			id = cDBPasswordReEnter;
			control = SECRETED
			{
				num_letters = 16;
			};
		}		
	};
}

RESOURCE DIALOG r_dialog_db_compress
{
	title = "请稍候";
	items=
	{
		DLG_LINE
		{
			prompt = "进度：";
			type = EEikCtProgInfo;
			id = cDBCompactProgress;
			control = PROGRESSINFO
			{
				text_type = EEikProgressTextPercentage;
			};
		}
	};
}

RESOURCE MENU_BAR r_file_menubar
{
	titles=
   	{
	   	MENU_TITLE { menu_pane = r_file_options_menu; txt="选项";},
	   	MENU_TITLE { menu_pane = r_file_drives_menu; txt="磁盘";},
		MENU_TITLE { menu_pane = r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE MENU_PANE r_file_options_menu
{
	items=
	{
		MENU_ITEM { command=cmdFileMan_Bluetooth; txt="以蓝牙传送"; flags=EEikMenuItemRadioStart; },
		MENU_ITEM { command=cmdFileMan_IrDA; txt="以红外线传送"; flags=EEikMenuItemRadioEnd | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { cascade=r_file_sort_menu; txt="排序"; },
		MENU_ITEM { command=cmdFileManToggleHiddenView; txt="显示隐藏文件和文件夹"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdFileManToggleRunApp; txt="自动运行程序"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdFileManToggleOpenFile; txt="自动打开文件"; flags=EEikMenuItemCheckBox | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdZoom; txt="缩放"; }
	};
}

RESOURCE MENU_PANE r_file_sort_menu
{
	items=
	{
		MENU_ITEM { command=cmdFileMan_SortName; txt="按名称排序"; flags=EEikMenuItemRadioStart; },
		MENU_ITEM { command=cmdFileMan_SortDate; txt="按日期排序"; flags=EEikMenuItemRadioMiddle; },
		MENU_ITEM { command=cmdFileMan_SortSize; txt="按大小排序"; flags=EEikMenuItemRadioEnd | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdFileMan_SortAsc; txt="按升序排序"; flags=EEikMenuItemRadioStart; },
		MENU_ITEM { command=cmdFileMan_SortDesc; txt="按降序排序"; flags=EEikMenuItemRadioEnd; }
	};
}

RESOURCE MENU_PANE r_file_drives_menu
{
	items=
	{
	};
}

RESOURCE MENU_PANE r_fileman_dobutton
{
	items=
	{
		MENU_ITEM { command=cmdFileManBeam; txt="传送"; },
		MENU_ITEM { command=cmdFileManCut; txt="剪切"; },
		MENU_ITEM { command=cmdFileManCopy; txt="复制"; },
		MENU_ITEM { command=cmdFileManPaste; txt="粘贴"; },
		MENU_ITEM { command=cmdFileManRename; txt="重命名"; },
		MENU_ITEM { command=cmdFileManDelete; txt="删除"; flags = EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdFileManCRC; txt="获取 CRC"; },
		MENU_ITEM { command=cmdFileManProperties; txt="属性"; },
		MENU_ITEM { command=cmdFileManOpenOverride; txt="打开"; },
		MENU_ITEM { command=cmdFileManCopyAs; txt="建立复制"; },
		MENU_ITEM { command=cmdFileManHexEdit; txt="编辑 HEX"; }
	};
}

RESOURCE MENU_PANE r_db_dobutton
{
	items=
	{
		MENU_ITEM { command=cmdDBDel; txt="删除"; flags = EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdDBAdd; txt="新建"; },
		MENU_ITEM { command=cmdDBEdit; txt="编辑"; },
		MENU_ITEM { command=cmdDBView; txt="视图"; },
		MENU_ITEM { command=cmdDBSave; txt="保存"; }	
	};
}

RESOURCE QIK_TOOLBAR r_toolbar_db_list
{
	height = EQikToolbarHeight - 4;
	controls = 
	{
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdDBDo;
			bmpfile = "*";
			bmpid = EMbmMyHeaderDobutton;
			bmpmask = EMbmMyHeaderDobuttonmask;
			alignment = EQikToolbarRight;
		}
	};
}

RESOURCE QIK_TOOLBAR r_toolbar_db_single
{
	height = EQikToolbarHeight - 4;
	controls = 
	{
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdDBBack;
			bmpfile = "Z:\\SYSTEM\\DATA\\QUARTZ.MBM";
			bmpid = EMbmQuartzBackarrow;
			bmpmask = EMbmQuartzBackarrowmask;
		},		
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdDBDo;
			bmpfile = "*";
			bmpid = EMbmMyHeaderDobutton;
			bmpmask = EMbmMyHeaderDobuttonmask;
			alignment = EQikToolbarRight;
		}
	};
}

RESOURCE QIK_TOOLBAR r_toolbar_bluejack
{
	controls = 
	{
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdBluejack;
			bmpfile = "*";
			bmpid = EMbmMyheaderBjackproximity;
			bmpmask = EMbmMyheaderBjackproximity;
		},
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdBluejackOne;
			bmpfile = "*";
			bmpid = EMbmMyheaderBjackindividual;
			bmpmask = EMbmMyheaderBjackindividual;
		},
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdBluejackStop;
			bmpfile = "*";
			bmpid = EMbmMyheaderBjackstop;
			bmpmask = EMbmMyheaderBjackstopmask;
		}
	};
}

RESOURCE QIK_TOOLBAR r_toolbar_fileman
{
	height = EQikToolbarHeight - 4;
	controls = 
	{
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdFileManAll;
			bmpfile = "*";
			bmpid = EMbmMyheaderAll;
			bmpmask = EMbmMyheaderAllmask;
		},
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdFileManNone;
			bmpfile = "*";
			bmpid = EMbmMyheaderNone;
			bmpmask = EMbmMyheaderNonemask;
		},
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdFileManNewFolder;
			bmpfile = "*";
			bmpid = EMbmMyheaderNewfolder;
			bmpmask = EMbmMyheaderNewfoldermask;
		},
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdFileManReload;
			bmpfile = "*";
			bmpid = EMbmMyheaderReload;
			bmpmask = EMbmMyheaderReloadmask;
		},
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdFileManFindFile;
			bmpfile = "*";
			bmpid = EMbmMyheaderFindfile;
			bmpmask = EMbmMyheaderFindfilemask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdDoButton;
			bmpfile = "*";
			bmpid = EMbmMyHeaderDobutton;
			bmpmask = EMbmMyHeaderDobuttonmask;
			alignment = EQikToolbarRight;
		}
	};
}

RESOURCE ARRAY r_array_db_fieldname
{
	items=
	{
		LBUF { txt="使用者名称"; },
		LBUF { txt="密码"; },
		LBUF { txt="PIN"; },
		LBUF { txt="信用卡号码"; },
		LBUF { txt="信用卡型式"; },
		LBUF { txt="使用期限"; }
	};
}

RESOURCE TBUF r_tbuf_applistfixed { buf = "已修复程序列表"; }
RESOURCE TBUF r_tbuf_errlaunchapp { buf = "启动程序时发生错误"; }
RESOURCE TBUF r_tbuf_errsaveconfig { buf = "储存设置时发生错误"; }
RESOURCE TBUF r_tbuf_errsavesnapshot { buf = "储存快照时发生错误";}
RESOURCE TBUF r_tbuf_hotkeydisabled { buf = "已禁用热键"; }
RESOURCE TBUF r_tbuf_hotkeyenabled { buf = "已启用热键"; }
RESOURCE TBUF r_tbuf_nomarkedtasks { buf = "没有选择的任务";}
RESOURCE TBUF r_tbuf_noprevsnapshot { buf = "没有快照可载入";}
RESOURCE TBUF r_tbuf_nosnapshot { buf = "没有载入任何的快照";}
RESOURCE TBUF r_tbuf_scannerstopping { buf = "正在停止扫描器. 请稍候"; }
RESOURCE TBUF r_tbuf_snapshotloaded { buf = "已读取快照";}
RESOURCE TBUF r_tbuf_snapshotupdated { buf = "已更新快照";}
RESOURCE TBUF r_tbuf_stopbeam { buf = "传输中, 请稍候"; }
RESOURCE TBUF r_tbuf_stopscanner { buf = "请取消扫描器"; }
RESOURCE TBUF r_tbuf_tasksflushed { buf = "已退出不使用的程序";}
RESOURCE TBUF r_tbuf_validating { buf = "确认中..."; }

//RESOURCE TBUF r_tbuf_beamer_busy { buf = "传送文件忙碌，请稍候"; }
RESOURCE TBUF r_tbuf_beamer_cancelled { buf = "已取消文件传送"; }
RESOURCE TBUF r_tbuf_beamer_fileopenerr { buf = "无法打开文件"; }
RESOURCE TBUF r_tbuf_beamer_irdahrerr { buf = "红外线主控制器系统错误！"; }
RESOURCE TBUF r_tbuf_beamer_irdamissing { buf = "找不到红外线驱动程序！"; }
RESOURCE TBUF r_tbuf_beamer_irdanodevice { buf = "没有发现远程设备。"; }
RESOURCE TBUF r_tbuf_beamer_obexconnect { buf = "正在连接..."; }
RESOURCE TBUF r_tbuf_beamer_obexconnecterr { buf = "OBEX 连接失败。"; }
RESOURCE TBUF r_tbuf_beamer_obexdisconnect { buf = "已完成. 正在断开连接..."; }
RESOURCE TBUF r_tbuf_beamer_obexdisconnecterr { buf = "断开连接出错. 已中止。"; }
RESOURCE TBUF r_tbuf_beamer_obexputerr { buf = "OBEX PUT 失败. 正在断开连接..."; }
RESOURCE TBUF r_tbuf_beamer_obexretry { buf = "OBEX 搜索失败. 正在重试。"; }
RESOURCE TBUF r_tbuf_beamer_obexsearch { buf = "OBEX 查找"; }
RESOURCE TBUF r_tbuf_beamer_obexsearcherr { buf = "OBEX 查找失败. 正在放弃。"; }
RESOURCE TBUF r_tbuf_beamer_sending { buf = "传送中 "; }
RESOURCE TBUF r_tbuf_beamer_skipfolder { buf = "正在略过文件夹 "; }

RESOURCE TBUF r_tbuf_bjackbtdisabled { buf = "蓝牙尚未启动！"; }
RESOURCE TBUF r_tbuf_bjackbtlinkmissing { buf = "找不到蓝牙连线管理器！"; }
RESOURCE TBUF r_tbuf_bjackbusy { buf = "蓝牙接口模块忙碌中"; }
RESOURCE TBUF r_tbuf_bjackcached { buf = "找到缓存中的设备。"; }
RESOURCE TBUF r_tbuf_bjackcachedecay { buf = "找到缓存中的设备. 正等待被遗弃。"; }
RESOURCE TBUF r_tbuf_bjackcachefull { buf = "发现设备 (缓存区已满)"; }
RESOURCE TBUF r_tbuf_bjackcachefullskip { buf = "设备缓存已满. 正在略过."; }
RESOURCE TBUF r_tbuf_bjackcooloff { buf = "堆栈区平复"; }
RESOURCE TBUF r_tbuf_bjackerr { buf = "蓝牙接口发生错误 "; }
RESOURCE TBUF r_tbuf_bjackerrsavelog { buf = "保存扫描记录时发生错误"; }
RESOURCE TBUF r_tbuf_bjackerrvcard { buf = "无法打开电子名片"; }
RESOURCE TBUF r_tbuf_bjackfounddevice { buf = "已发现设备"; }
RESOURCE TBUF r_tbuf_bjacklogmissing { buf = "找不到记录文件"; }
RESOURCE TBUF r_tbuf_bjackmsgsaved { buf = "已保存信息"; }
RESOURCE TBUF r_tbuf_bjackmsgtoolong { buf = "最多 20 个字符"; }
RESOURCE TBUF r_tbuf_bjacknodevices { buf = "找不到其他设备"; }
RESOURCE TBUF r_tbuf_bjackobexconnerr { buf = "OBEX 连接失败 "; }
RESOURCE TBUF r_tbuf_bjackobexfound { buf = "OBEX 已找到"; }
RESOURCE TBUF r_tbuf_bjackobexput { buf = "OBEX Put"; }
RESOURCE TBUF r_tbuf_bjackobexsearcherr { buf = "OBEX 检索错误 "; }
RESOURCE TBUF r_tbuf_bjackscannerstarted { buf = "扫描器已启动"; }
RESOURCE TBUF r_tbuf_bjackscannerterminated { buf = "扫描器已停止"; }
RESOURCE TBUF r_tbuf_bjackscannerterminating { buf = "正在停止扫描器. 请稍候。"; }
RESOURCE TBUF r_tbuf_bjackscanning { buf = "扫描中..."; }
RESOURCE TBUF r_tbuf_bjacksuccess { buf = "蓝牙接口已成功！"; }
RESOURCE TBUF r_tbuf_bjacktimeout { buf = "超时..."; }

RESOURCE TBUF r_tbuf_fileman_confirm_delete { buf = "你确定要删除这些项目吗？"; }
RESOURCE TBUF r_tbuf_fileman_copy { buf = "已复制 %d 个项目"; }
RESOURCE TBUF r_tbuf_fileman_crccancelled { buf = "CRC 操作已取消"; }
RESOURCE TBUF r_tbuf_fileman_cut { buf = "已剪切 %d 个项目"; }
RESOURCE TBUF r_tbuf_fileman_errmkdir { buf = "创建文件夹出错"; }
RESOURCE TBUF r_tbuf_fileman_errreadattr { buf = "读取文件属性时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_errreadfolder { buf = "读取文件夹时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_errrename { buf = "重命名时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_errsetattribs { buf = "设置文件属性时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_invalidfilename { buf = "错误的名称"; }
RESOURCE TBUF r_tbuf_fileman_openerr { buf = "打开文件时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_sametargetsource { buf = "目标与来源相同，请改用建立复制功能。"; }
RESOURCE TBUF r_tbuf_fileman_wrongselect { buf = "不能选择磁盘或上层文件夹"; }
RESOURCE TBUF r_tbuf_fileman_appnotregistered { buf = "应用程序未注册"; }
RESOURCE TBUF r_tbuf_fileman_copying { buf = "复制中 "; }
RESOURCE TBUF r_tbuf_fileman_moving { buf = "移动中 "; }
RESOURCE TBUF r_tbuf_fileman_deleting { buf = "删除中 "; }
RESOURCE TBUF r_tbuf_fileman_renaming { buf = "重命名中 "; }
RESOURCE TBUF r_tbuf_fileman_initfailure { buf = "文件管理初始失败"; }
RESOURCE TBUF r_tbuf_fileman_errscansubfolder { buf = "扫描子文件夹时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_erropensource { buf = "打开来源文件时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_samelocation { buf = "目标是来源的子文件夹"; }
RESOURCE TBUF r_tbuf_fileman_cantwritetarget { buf = "目标已存在或是无法创建目标"; }
RESOURCE TBUF r_tbuf_fileman_errsourcelock { buf = "来源正在使用中"; }
RESOURCE TBUF r_tbuf_fileman_errdirfull { buf = "文件夹已满"; }
RESOURCE TBUF r_tbuf_fileman_errdevdismount { buf = "设备已卸载"; }
RESOURCE TBUF r_tbuf_fileman_errfilelock { buf = "文件已锁定"; }
RESOURCE TBUF r_tbuf_fileman_errdevnotready { buf = "设备未加载"; }
RESOURCE TBUF r_tbuf_fileman_erraccessdenied { buf = "存取失败. 文件是否为只读？"; }
RESOURCE TBUF r_tbuf_fileman_errdiskfull { buf = "磁盘已满"; }
RESOURCE TBUF r_tbuf_fileman_errwrite { buf = "写入错误"; }
RESOURCE TBUF r_tbuf_fileman_errpathnotfound { buf = "找不到路径"; }
RESOURCE TBUF r_tbuf_fileman_errgeneral { buf = "一般错误 "; }
RESOURCE TBUF r_tbuf_fileman_errreadfile { buf = "读取文件时发生错误"; }

RESOURCE TBUF r_tbuf_misc_done { buf = "完成"; }
RESOURCE TBUF r_tbuf_misc_errors { buf = "发生错误"; }
RESOURCE TBUF r_tbuf_misc_busy { buf = "忙碌中..."; }
RESOURCE TBUF r_tbuf_misc_fc_bjack_sweep { buf = "停止区域扫描"; }
RESOURCE TBUF r_tbuf_misc_info_title { buf = "信息"; }
RESOURCE TBUF r_tbuf_misc_delete_title { buf = "删除"; }

RESOURCE TBUF r_tbuf_db_recordmissing { buf = "无法找到记录，数据库有错误？"; }
RESOURCE TBUF r_tbuf_db_confirm_delete { buf = "确认删除动作？"; }
RESOURCE TBUF r_tbuf_db_err_delete { buf = "删除记录时发生错误"; }
RESOURCE TBUF r_tbuf_db_err_mass_delete { buf = "错误。部分记录未删除"; }
RESOURCE TBUF r_tbuf_db_err_save { buf = "保存记录时发生错误，数据库有错误？"; }
RESOURCE TBUF r_tbuf_db_err_read { buf = "读取数据库时发生错误"; }
RESOURCE TBUF r_tbuf_db_err_create { buf = "创建数据库时发生错误"; }
RESOURCE TBUF r_tbuf_db_err_compact { buf = "压缩数据库时发生错误"; }
RESOURCE TBUF r_tbuf_db_err_updatestats { buf = "更新统计资料时发生错误"; }
RESOURCE TBUF r_tbuf_db_err_norecords { buf = "没有选取的记录"; }
RESOURCE TBUF r_tbuf_db_confirm_cancel { buf = "取消变更？"; }
RESOURCE TBUF r_tbuf_db_confirm_cancel_title { buf = "确认"; }
RESOURCE TBUF r_tbuf_db_err_changepassword { buf = "更改密码时发生错误"; }
RESOURCE TBUF r_tbuf_db_err_passwordnomatch { buf = "密码不一致"; }
RESOURCE TBUF r_tbuf_db_msg_encrypting { buf = "加密数据库存取金钥"; }
RESOURCE TBUF r_tbuf_db_err_reinit { buf = "删除数据库时发生错误"; }
RESOURCE TBUF r_tbuf_db_confirm_reinit { buf = "这将会清除掉所有的东西，你确定吗？"; }
RESOURCE TBUF r_tbuf_db_err_nodb { buf = "找不到数据库文件"; }
RESOURCE TBUF r_tbuf_db_no_displayname { buf = "显示名称为空白，记录未创建。"; }

// Added Sept 18th
RESOURCE TBUF r_tbuf_db_msg_deleting { buf = "删除中...请稍候"; }

RESOURCE MENU_PANE r_agenda_options
{
	items=
	{
		MENU_ITEM { cascade=r_agenda_completed_menu; txt="显示已完成项目"; },
		MENU_ITEM { cascade=r_agenda_autoexpand_menu; txt="自动展开"; },
		MENU_ITEM { command=cmdAgendaSetTodoMaxPriority; txt="指定工作优先级"; flags = EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdZoom; txt="缩放"; }
	};
}

RESOURCE MENU_PANE r_agenda_completed_menu
{
	items=
	{
		MENU_ITEM { command=cmdAgendaCompleteCalendar; txt="行程历"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdAgendaCompleteTodo; txt="工作项目"; flags=EEikMenuItemCheckBox; }
	};
}

RESOURCE MENU_PANE r_agenda_autoexpand_menu
{
	items=
	{
		MENU_ITEM { command=cmdAgendaAutoExpandCalendar; txt="行程历"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdAgendaAutoExpandTodo; txt="工作项目"; flags=EEikMenuItemCheckBox; }
	};
}

RESOURCE MENU_BAR r_agenda_menubar
{
	titles=
   	{
		MENU_TITLE { menu_pane=r_agenda_options; txt="今日视图"; },
		MENU_TITLE { menu_pane=r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE TBUF r_tbuf_agenda_nofile { buf = "找不到行程历文件"; }
RESOURCE TBUF r_tbuf_agenda_operr { buf = "行程历运作错误 #"; }
RESOURCE TBUF r_tbuf_agenda_openerror { buf = "打开行程历时发生错误"; }
RESOURCE TBUF r_tbuf_agenda_busy { buf = "读取行程历..."; }
RESOURCE TBUF r_tbuf_agenda_errupdatetodo { buf = "更新项目时发生错误"; }
RESOURCE TBUF r_tbuf_agenda_notfound_or_error { buf = "项目遗失或是无法读取行程历文件"; }
RESOURCE TBUF r_tbuf_agenda_none { buf = "<无>"; }
RESOURCE TBUF r_tbuf_agenda_notimplemented { buf = "尚未创建功能"; }

RESOURCE MENU_PANE r_fn_utilities_menu
{
	items =
	{
		MENU_ITEM { command=cmdUtilsFixApplist; txt="修复程序列表"; },
		MENU_ITEM { command=cmdUtilsCompContacts; txt="压缩联系人"; },
		MENU_ITEM { command=cmdUtilsAgendaFixSynchTimeZone; txt="修复同步时区"; },
		MENU_ITEM { command=cmdUtilsFontSelector; txt="设置系统字型"; },
		MENU_ITEM { command=cmdUtilsP900toP800; txt="P900 改 P800"; },
		MENU_ITEM { cascade=r_fixapporder_menu; command=cmdCascadeFixAppOrder; txt="修复程序排列"; },
		MENU_ITEM { command=cmdAutoStartCascade; cascade=r_options_autostart_menu; txt="自动启动"; }
	};
}

RESOURCE MENU_PANE r_fixapporder_menu
{
	items=
	{
		MENU_ITEM { command=cmdUtilsFixAppOrderReset; txt="重设"; },
		MENU_ITEM { command=cmdUtilsFixAppOrderRestore; txt="恢复"; },
		MENU_ITEM { command=cmdUtilsFixAppOrderBackup; txt="备份"; }
	};
}

RESOURCE TBUF r_tbuf_util_errcompactcontacts { buf = "压缩联系人时发生错误"; }

RESOURCE QIK_TOOLBAR r_toolbar_agenda
{
	height = EQikToolbarHeight - 4;
	controls = 
	{
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdAgendaRead;
			bmpfile = "*";
			bmpid = EMbmMyheaderReload;
			bmpmask = EMbmMyheaderReloadmask;
		},
		QIK_TBAR_BUTTON 
		{ 
			ypos = -3;
			id = cmdAgendaViewOtherDate;
			bmpfile = "*";
			bmpid = EMbmMyheaderTodayviewwarp;
			bmpmask = EMbmMyheaderTodayviewwarpmask;
		}
	};
}

RESOURCE DIALOG r_dialog_agenda_calendar_detailed
{
	title = "行程历资讯";
	buttons = R_EIK_BUTTONS_OK;
	flags = EEikDialogFlagWait;
	pages = r_dialog_agenda_calendar_detailed_pages;
}

RESOURCE ARRAY r_dialog_agenda_calendar_detailed_pages
{
	items = 
	{	
		PAGE
		{
			id = idAgendaPage_Summary;		
			text = "概要";
			lines = r_dialog_agenda_calendar_summary_page;
		},
		PAGE
		{
			id = idAgendaPage_Notes;
			text = "注释";
			lines = r_dialog_agenda_calendar_notes_page;
		}
	};
}

RESOURCE ARRAY r_dialog_agenda_calendar_summary_page
{
	items = 
	{
		DLG_LINE
		{
			prompt = "开始：";
			type = EEikCtLabel;
			id = cAgendaDetailStartDateTime;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "结束：";
			type = EEikCtLabel;
			id = cAgendaDetailEndDateTime;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "闹铃：";
			type = EEikCtLabel;
			id = cAgendaDetailAlarmDateTime;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "地点：";
			type = EEikCtLabel;
			id = cAgendaDetailLocation;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		}
	};
}

RESOURCE ARRAY r_dialog_agenda_calendar_notes_page
{
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cAgendaDetailNotes;
			control = GTXTED 
			{ 
				height = 22 * 4 + 40; width = 200; numlines = 4; flags = EEikEdwinReadOnly | EEikEdwinDisplayOnly;
			};
		}		
	};
}

RESOURCE DIALOG r_dialog_agenda_todo_detailed
{
	title = "工作项目资讯";
	buttons = R_EIK_BUTTONS_OK;
	flags = EEikDialogFlagWait;
	pages = r_dialog_agenda_todo_detailed_pages;
}

RESOURCE ARRAY r_dialog_agenda_todo_detailed_pages
{
	items = 
	{	
		PAGE
		{
			id = idAgendaPage_Summary;		
			text = "概要";
			lines = r_dialog_agenda_todo_summary_page;
		},
		PAGE
		{
			id = idAgendaPage_Notes;
			text = "注释";
			lines = r_dialog_agenda_todo_notes_page;
		}
	};
}

RESOURCE ARRAY r_dialog_agenda_todo_summary_page
{
	items = 
	{
		DLG_LINE
		{
			prompt = "到期日：";
			type = EEikCtLabel;
			id = cAgendaDetailEndDateTime;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "闹铃：";
			type = EEikCtLabel;
			id = cAgendaDetailAlarmDateTime;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "优先级：";
			type = EEikCtLabel;
			id = cAgendaDetailPriority;
			control = LABEL 
			{
				txt="";
				horiz_align=EEikLabelAlignHLeft;
			};
		},
		DLG_LINE
		{
			prompt = "已完成：";
			type = EEikCtCheckBox;
			id = cAgendaDetailTodoDone;
		}
	};
}

RESOURCE ARRAY r_dialog_agenda_todo_notes_page
{
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cAgendaDetailNotes;
			control = GTXTED 
			{ 
				height = 22 * 4 + 40; width = 200; numlines = 4; flags = EEikEdwinReadOnly | EEikEdwinDisplayOnly;
			};
		}		
	};
}

RESOURCE TBUF r_tbuf_fileman_prop_isfolder { buf = "是一个文件夹"; }
RESOURCE TBUF r_tbuf_fileman_prop_size { buf = "大小为 "; }
RESOURCE TBUF r_tbuf_fileman_prop_byte { buf = " 位元"; }
RESOURCE TBUF r_tbuf_fileman_prop_lastmod { buf = "最后一次修改日期： "; }
RESOURCE TBUF r_tbuf_unassigned { buf = "<未指定>%c%u"; }
RESOURCE TBUF r_tbuf_unassigned2 { buf = "<未指定>%c%c%c%c"; }

//--------- v1.2b --------------

RESOURCE TBUF r_tbuf_utils_fixapporder_backupfail { buf = "创建备份时出错。"; }
RESOURCE TBUF r_tbuf_utils_fixapporder_failed { buf = "从备份中恢复时发生错误，修复失败。"; }
RESOURCE TBUF r_tbuf_utils_fixapporder_confirmrestore { buf = "这将导致从上次备份后所安装的程序被归至\"未分类\"中，其他程序则将依照备份资料去归类。你确定吗？\n\r上次的备份日期为：\n"; }
RESOURCE TBUF r_tbuf_utils_fixapporder_confirmreset { buf = "这将导致所有用户自行安装的程序被归至\"未分类\"中，而所有预安装的程序按默认设定归类。你确定吗？"; }
RESOURCE TBUF r_tbuf_utils_fixapporder_confirmbackup { buf = "备份文件已存在，进行新的备份将替换它。你确定吗？\n\r上次备份日期为：\n"; }
RESOURCE TBUF r_tbuf_utils_fixapporder_waitawhile { buf = "重新启动应用程序。手机反应可能会暂时变慢"; }

//--------- P900 fix -----------
RESOURCE TBUF r_tbuf_fc_bluejacktxt { buf = "SMan - 蓝牙接口记录"; }

//--------- v1.3 ---------------
RESOURCE TBUF r_tbuf_edit_notextselected { buf = "尚未选取任何文字"; }
RESOURCE TBUF r_tbuf_utils_compresscontacts_line1 { buf = "压缩前空间： "; }
RESOURCE TBUF r_tbuf_utils_compresscontacts_line2 { buf = "压缩后空间： "; }
RESOURCE TBUF r_tbuf_utils_compresscontacts_saved { buf = "节省空间： "; }
RESOURCE TBUF r_tbuf_utils_fixsynchtimezone_confirm { buf = "确认动作？"; }
RESOURCE TBUF r_tbuf_fileman_toomanydupes { buf = "太多文件复制"; }
RESOURCE TBUF r_tbuf_fileman_findingslot { buf = "侦测复制文件名中..."; }
RESOURCE TBUF r_tbuf_fileman_duplicating { buf = "制作复制文件中..."; }
RESOURCE TBUF r_tbuf_fileman_errordupe { buf = "制作复制文件时发生错误"; }
RESOURCE TBUF r_tbuf_fileman_dupedone { buf = "复制成 "; }
RESOURCE TBUF r_tbuf_fileman_errsetmodified { buf = "设置日期/时间时发生错误"; }
RESOURCE TBUF r_tbuf_dtmf_generating { buf = "产生 DTMF 音讯中..."; }
RESOURCE TBUF r_tbuf_dtmf_nonumber { buf = "没有号码"; }
RESOURCE TBUF r_tbuf_hexedit_notready { buf = "取得文件大小时发生错误"; }
RESOURCE TBUF r_tbuf_hexedit_buffull { buf = "请保存文件。缓存区已满。"; }
RESOURCE TBUF r_tbuf_hexedit_errorwritefile { buf = "写入文件时发生错误"; }
RESOURCE TBUF r_tbuf_hexedit_endoffile { buf = "已达文件结尾"; }
RESOURCE TBUF r_tbuf_hexedit_beginningoffile { buf = "已达文件开头"; }
RESOURCE TBUF r_tbuf_hexedit_readonly { buf = "只读"; }
RESOURCE TBUF r_tbuf_hexedit_fileinrom { buf = "文件保存于只读存储器！"; }
RESOURCE TBUF r_tbuf_hexedit_fileisreadonly { buf = "文件是只读的"; }
RESOURCE TBUF r_tbuf_hexedit_smanfile { buf = "无法编辑 - SMan 需要此文件"; }
RESOURCE TBUF r_tbuf_hexedit_writeerror { buf = "写入错误"; }
RESOURCE TBUF r_tbuf_hexedit_sizeerror { buf = "已保存。读取文件大小时发生错误"; }
RESOURCE TBUF r_tbuf_hexedit_truncerror { buf = "已保存。调整文件大小时发生错误"; }
RESOURCE TBUF r_tbuf_hexedit_seekerror { buf = "未保存。搜寻错误"; }
RESOURCE TBUF r_tbuf_hexedit_flusherror { buf = "未保存。退出错误"; }
RESOURCE TBUF r_tbuf_hexedit_writeok { buf = "已保存"; }
RESOURCE TBUF r_tbuf_hexedit_nochanges { buf = "没有任何变更"; }
RESOURCE TBUF r_tbuf_hexedit_getdateerror { buf = "取得日期时发生错误"; }
RESOURCE TBUF r_tbuf_findfile_topreceedsfrom { buf = "开始日期早于结束日期"; }
RESOURCE TBUF r_tbuf_findfile_nolocation { buf = "尚未指定搜寻位置"; }
RESOURCE TBUF r_tbuf_findfile_invalidsearchname { buf = "错误的搜寻名称"; }
RESOURCE TBUF r_tbuf_findfile_errorsinsearch { buf = "发生错误，搜寻未完成"; }
RESOURCE TBUF r_tbuf_findfile_sorting { buf = "排序中..."; }
RESOURCE TBUF r_tbuf_findfile_done { buf = "已找到 %d 个文件"; }
RESOURCE TBUF r_tbuf_utils_misc_reboot { buf = "请重新启动手机来查看变更"; }
RESOURCE TBUF r_tbuf_fc_cellareatxt { buf = "SMan - 网络信息"; }
RESOURCE TBUF r_tbuf_autostart_errdelete { buf = "移除识别器时发生错误"; }
RESOURCE TBUF r_tbuf_autostart_errcopy { buf = "复制识别器时发生错误"; }
RESOURCE TBUF r_tbuf_autostart_enabled { buf = "已启用自动启动功能"; }
RESOURCE TBUF r_tbuf_autostart_disabled { buf = "已停用自动启动功能"; }
RESOURCE TBUF r_tbuf_autostart_deactivate { buf = "停用"; }
RESOURCE TBUF r_tbuf_shortcut_fcinvalid { buf = "错误的快捷方式于阖盖模式"; }
RESOURCE TBUF r_tbuf_misc_unsupported { buf = "未支援"; }
RESOURCE TBUF r_tbuf_cellid_errinitstore { buf = "加载 cell 数据库时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errreadgroups { buf = "读取群组资讯时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errcreategroup { buf = "创建群组时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_autorecordok { buf = "自动计录已开始"; }
RESOURCE TBUF r_tbuf_cellid_errautosave { buf = "自动计录已停止。保存 cell 信息时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_autorecordoff { buf = "自动记录已停止"; }
RESOURCE TBUF r_tbuf_cellid_genericeditordialog_strrequired { buf = "你必需指定某个事件！"; }
RESOURCE TBUF r_tbuf_cellid_errdelgroup { buf = "删除群组时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errsetgroupstate { buf = "设置群组状态时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errrengroup { buf = "更改群组名时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_noitemsselected { buf = "尚未选取任何项目"; }
RESOURCE TBUF r_tbuf_cellid_errdelcell { buf = "删除 cell 时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errcreatecell { buf = "创建 cell 时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errsavecell { buf = "保存 cell 时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_requirecountrycode { buf = "需要国家编号"; }
RESOURCE TBUF r_tbuf_cellid_requirenetworkid { buf = "需要网络编号"; }
RESOURCE TBUF r_tbuf_cellid_noitems { buf = "没有项目！"; }
RESOURCE TBUF r_tbuf_cellid_nogroups { buf = "没有群组！"; }
RESOURCE TBUF r_tbuf_cellid_disableautorec { buf = "请先取消自动记录"; }
RESOURCE TBUF r_tbuf_cellid_errcellexists { buf = "Cell 已存在于这个群组"; }
RESOURCE TBUF r_tbuf_cellid_errdbcompress { buf = "压缩数据库时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errdbupdatestats { buf = "更新统计资料时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errreaddb { buf = "读取数据库时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_cellnotfound { buf = "找不到 Cell"; }
RESOURCE TBUF r_tbuf_cellid_groupnotfound { buf = "找不到群组"; }
RESOURCE TBUF r_tbuf_cellid_errmovegroup { buf = "移动群组时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errmoveevent { buf = "移动事件时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_cantmove { buf = "无法往那个方向移动"; }
RESOURCE TBUF r_tbuf_cellid_noaudiofile { buf = "尚未指定音讯文件"; }
RESOURCE TBUF r_tbuf_cellid_errsaveevent { buf = "保存事件时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errdelevents { buf = "删除事件时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errseteventstate { buf = "设置事件状态时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errunknowneventtype { buf = "未知的事件类型"; }
RESOURCE TBUF r_tbuf_cellid_errcreateoutput { buf = "创建输出文件时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_numexported { buf = "已导出群组："; }
//RESOURCE TBUF r_tbuf_cellid_errimportlistfiles { buf = "列出导入文件时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_noimportfiles { buf = "没有文件可导入"; }
RESOURCE TBUF r_tbuf_cellid_errparsingimportfile { buf = "分析导入文件时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errreadingimportfile { buf = "读取导入文件时发生错误"; }
RESOURCE TBUF r_tbuf_cellid_errmisc { buf = "错误发生在行数："; }
RESOURCE TBUF r_tbuf_cellid_impexp_generalerror { buf = "错误发生在行数 "; }
RESOURCE TBUF r_tbuf_cellid_impexp_generaldberror { buf = "数据库回复错误 "; }
RESOURCE TBUF r_tbuf_misc_dbrecovery { buf = "恢复索引值"; }
RESOURCE TBUF r_tbuf_misc_dbupdatestats { buf = "更新统计资料"; }
RESOURCE TBUF r_tbuf_misc_errrecovery { buf = "恢复数据库时发生错误"; }
RESOURCE TBUF r_tbuf_misc_erropendb { buf = "打开数据库时发生错误"; }
RESOURCE TBUF r_tbuf_misc_processing { buf = "处理中 "; }
RESOURCE TBUF r_tbuf_cellid_exportingas { buf = "导出成 "; }
RESOURCE TBUF r_tbuf_cellid_shortname { buf = "简称："; }
RESOURCE TBUF r_tbuf_cellid_longname { buf = "全名："; }
RESOURCE TBUF r_tbuf_cellid_countrycode { buf = "国家编号："; }
RESOURCE TBUF r_tbuf_cellid_networkid { buf = "网络编号："; }
RESOURCE TBUF r_tbuf_cellid_cell { buf = "Cell："; }
RESOURCE TBUF r_tbuf_cellid_cellid { buf = "Cell 编号："; }
RESOURCE TBUF r_tbuf_cellid_group { buf = "群组："; }
RESOURCE TBUF r_tbuf_cellid_multigroup { buf = "群组[*]："; }
RESOURCE TBUF r_tbuf_cellid_signalstrength { buf = "讯号长度："; }
RESOURCE TBUF r_tbuf_bjack_errloadexlist { buf = "无法加载忽略清单"; }
RESOURCE TBUF r_tbuf_bjack_errsaveexlist { buf = "无法保存忽略清单"; }
RESOURCE TBUF r_tbuf_bjack_exclude { buf = "在忽略清单中发现设备。忽略中。"; }
RESOURCE TBUF r_tbuf_misc_warning { buf = "警告"; }
RESOURCE TBUF r_tbuf_recog_warning { buf = "自动启动模组需要将会存放至 C: 磁盘。如果这个模组发生错误，它可能会造成你的手机 \
无法使用直到你将它送至客服重设。\n\r你确定你要继续吗？"; }
RESOURCE TBUF r_tbuf_db_export { buf = "导出至："; }
RESOURCE TBUF r_tbuf_db_export_erroutput { buf = "创建输出文件时发生错误"; }
RESOURCE TBUF r_tbuf_db_createrecord { buf = "插入记录时发生错误"; }
RESOURCE TBUF r_tbuf_misc_navigation_noselection { buf = "尚未选取任何模组"; }
RESOURCE TBUF r_tbuf_utils_p900top800_errdelete { buf = "删除部分文件时发生错误"; }
RESOURCE TBUF r_tbuf_utils_p900top800_errcopy { buf = "复制 INI 文件时发生错误"; }
RESOURCE TBUF r_tbuf_misc_errcreatefile { buf = "创建文件时发生错误"; }
RESOURCE TBUF r_tbuf_misc_errzipmissing { buf = "Zip 文件遗失"; }
RESOURCE TBUF r_tbuf_misc_errreadzip { buf = "读取 zip 时发生错误。已损坏？"; }
RESOURCE TBUF r_tbuf_misc_errinitzip { buf = "启始 zip 程序库时发生错误"; }
RESOURCE TBUF r_tbuf_misc_erropenzip { buf = "打开 zip 文件时发生错误"; }
RESOURCE TBUF r_tbuf_utils_p900top800_confirm { buf = "这将会套用标准布景的颜色设置，请顺便运行位在\"控制面板\"的\"铃声与音效\" \
并重新检视你的设置，你将仍然可以听到事件 (使用默认音效) 但该设置可能会指定到已被 SMan 删除的文件上。\n\r继续这个动作吗？"; }
RESOURCE TBUF r_tbuf_folderfile_browser_onlyselectfile { buf = "只可以选取文件"; }
RESOURCE TBUF r_tbuf_misc_fatalerror { buf = "严重错误"; }
RESOURCE TBUF r_tbuf_utils_p900top800_failed { buf = "取代位于 C:\\SYSTEM\\DATA\\THEMES 的 .INI 文件时发生错误。\n\r请马上套用一个布景不然你的设备可能无法再启动！！！"; }
RESOURCE TBUF r_tbuf_alt_title_file_folder_browser_dialog { buf = "选择文件"; }

RESOURCE MENU_PANE r_dtmf_edit
{
	items=
	{
		MENU_ITEM { command=cmdDTMFEditCut; txt="剪切"; flags=EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdDTMFEditCopy; txt="复制"; flags=EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdDTMFEditPaste; txt="粘贴"; flags=EEikMenuItemDimmed | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdDTMFEditClear; txt="清除"; flags=EEikMenuItemDimmed; }
	};
}

RESOURCE MENU_BAR r_dtmfdialer_menubar
{
	titles=
   	{
   		MENU_TITLE { menu_pane=r_dtmfdialer_dial; txt="拨号"; },
   		MENU_TITLE { menu_pane=r_dtmf_edit; txt="编辑"; },
		MENU_TITLE { menu_pane=r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE MENU_PANE r_dtmfdialer_dial
{
	items=
	{
		MENU_ITEM { command=cmdDTMFDialerInput; txt="使用数字键"; flags=EEikMenuItemCheckBox; }
	};
}

RESOURCE MENU_PANE r_hexeditor_file
{
	items=
	{
		MENU_ITEM { command=cmdHexEditorFileSave; txt="保存"; },
		MENU_ITEM { command=cmdHexEditorFileReload; txt="重新读取"; },
		MENU_ITEM { command=cmdHexEditorFilePreserveDate; txt="保留日期"; flags = EEikMenuItemCheckBox; }
	};
}

RESOURCE MENU_PANE r_hexeditor_edit
{
	items=
	{
		MENU_ITEM { command=cmdHexEditorEditDeleteBytes; txt = "删除"; flags=EEikMenuItemDimmed; },
		MENU_ITEM { command=cmdHexEditorEditToggleReadOnly; txt="只读"; flags=EEikMenuItemCheckBox | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdHexEditorEditResize; txt="更改大小"; }
	};
}

RESOURCE MENU_BAR r_hexeditor_menubar
{
	titles=
	{
		MENU_TITLE { menu_pane=r_hexeditor_file; txt="文件"; },
		MENU_TITLE { menu_pane=r_hexeditor_edit; txt="编辑"; }
	};
}

RESOURCE QIK_TOOLBAR r_toolbar_hexeditor
{
	height = EQikToolbarHeight - 4;
	controls = 
	{
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdHexEditorBeginPage;
			bmpfile = "*";
			bmpid = EMbmMyHeaderHexeditbeginpage;
			bmpmask = EMbmMyHeaderHexeditbeginpagemask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdHexEditorPrevPage;
			bmpfile = "*";
			bmpid = EMbmMyHeaderHexeditprevpage;
			bmpmask = EMbmMyHeaderHexeditprevpagemask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdHexEditorGoto;
			bmpfile = "*";
			bmpid = EMbmMyHeaderHexeditgoto;
			bmpmask = EMbmMyHeaderHexeditgotomask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdHexEditorNextPage;
			bmpfile = "*";
			bmpid = EMbmMyHeaderHexeditnextpage;
			bmpmask = EMbmMyHeaderHexeditnextpagemask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdHexEditorEndPage;
			bmpfile = "*";
			bmpid = EMbmMyHeaderHexeditendpage;
			bmpmask = EMbmMyHeaderHexeditendpagemask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdHexEditorBack;
			bmpfile = "Z:\\SYSTEM\\DATA\\QUARTZ.MBM";
			bmpid = EMbmQuartzBackarrow;
			bmpmask = EMbmQuartzBackarrowmask;
			alignment = EQikToolbarRight;
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_ok_cancel
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidCancel; button = CMBUT { txt = "取消"; }; },
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "确定"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

RESOURCE DIALOG r_dialog_hexedit_gotooffset
{
	title = "输入 Offset";
	buttons = r_buttons_ok_cancel;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			prompt = "Hex Offset：";
			type = EEikCtEdwin;
			id = cHexEditHexOffset;
			control = EDWIN 
			{ 
				lines = 1; maxlength = 7; width = 8;
			};
		},
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "Dec Offset：";
			id = cHexEditDecOffset;
			control = QIK_NUMBER_EDITOR 
			{ 
				min = 0; max = 0x7fffffff;
			};
		},
		DLG_LINE
		{
			type = EQikCtSlider;
			prompt = "范围：";
			id = cHexEditSliderOffset;
			control = QIK_SLIDER
			{
				min_value = 0;
				// Something odd about this control. In the RSS definition, max_value is a WORD
				// which only takes $FFFF as the max value. However, in the class, the max value
				// is a TInt which goes up to 32 bits. hmmmmmmm.......
				max_value = 0;
			};
		}
	};
}

RESOURCE DIALOG r_dialog_hexedit_resize
{
	title = "更改视窗大小";
	buttons = r_buttons_ok_cancel;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EQikCtSlider;
			prompt = "Hex 视窗大小：";
			id = cHexEditSliderResize;
			control = QIK_SLIDER
			{
				min_value = 3;
				max_value = 7;
				flags = EQikSliderSnapToMarker;
				num_markers = 5;
			};
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_yes_no_cancel
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "确定"; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "放弃"; }; flags=EEikLabeledButtonIsDefault; },
		DLG_BUTTON { id = EEikBidSpace; button = CMBUT { txt = "取消"; }; }
	};
}

RESOURCE DIALOG r_dialog_hexedit_confirmchanges
{
	title = "变更确认";
	buttons = r_buttons_yes_no_cancel;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtLabel;
			control = LABEL 
			{
				txt="你确定要变更本页的内容吗？";
				horiz_align=EEikLabelAlignHLeft;
			};
		}
	};
}

RESOURCE QIK_TOOLBAR r_toolbar_findfile
{
	height = EQikToolbarHeight - 4;
	controls = 
	{
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdFindFileStart;
			bmpfile = "*";
			bmpid = EMbmMyheaderFindfilestart;
			bmpmask = EMbmMyheaderFindfilestartmask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdFindFileStop;
			bmpfile = "*";
			bmpid = EMbmMyheaderFindfilestop;
			bmpmask = EMbmMyheaderFindfilestopmask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdFindFileCriteria;
			bmpfile = "*";
			bmpid = EMbmMyheaderFindfilecriteria;
			bmpmask = EMbmMyheaderFindfilecriteriamask;
		},
		QIK_TBAR_BUTTON
		{
			ypos = -3;
			id = cmdFindFileBack;
			bmpfile = "Z:\\SYSTEM\\DATA\\QUARTZ.MBM";
			bmpid = EMbmQuartzBackarrow;
			bmpmask = EMbmQuartzBackarrowmask;
			alignment = EQikToolbarRight;
		}
	};
}

RESOURCE MENU_BAR r_findfile_menubar
{
	titles=
   	{
   		MENU_TITLE { menu_pane=r_findfile_options; txt="设置"; }
	};
}

RESOURCE MENU_PANE r_findfile_options
{
	items=
	{
		MENU_ITEM { command=cmdZoom; txt = "缩放"; }
	};
}

RESOURCE ARRAY r_findfile_modified_option_choices
{
	items =
	{
		LBUF { txt = "忽略"; },
		LBUF { txt = "今天"; },
		LBUF { txt = "昨天"; },
		LBUF { txt = "其它"; }
	};
}

RESOURCE ARRAY r_empty_array
{
	items =
	{
		// Do not change this. Does not require translation. 'W' chosen to maximize width
		// of control that uses this. Used in PreLayoutDynInitL() of dialogs.
		LBUF { txt = "WWWWWWWWWWWWWWWWWWW"; } 
	};
}

RESOURCE DIALOG r_dialog_findfile_criteria
{
	title = "查找条件";
	buttons = r_buttons_ok_cancel;
	flags = EEikDialogFlagWait;
	pages = r_dialog_findfile_criteria_pages;
}

RESOURCE ARRAY r_dialog_findfile_criteria_pages
{
	items = 
	{	
		PAGE
		{
			text = "常规";
			lines = r_dialog_findfile_criteria_general_page;
		},
		PAGE
		{
			text = "日期";
			lines = r_dialog_findfile_criteria_date_page;
		}
	};
}

RESOURCE ARRAY r_findfile_criteria_array
{
	items=
	{	
		LBUF { txt="*.*"; },
		LBUF { txt="*.sis"; },
		LBUF { txt="*.gif"; },
		LBUF { txt="*.jpg"; },
		LBUF { txt="*.zip"; }
	};	
}

RESOURCE ARRAY r_dialog_findfile_criteria_general_page
{
	items = 
	{
		DLG_LINE
		{
			prompt = "名称：";
			type = EEikCtComboBox;
			id = cFindFileSearchName;
			control = COMBOBOX
			{
				maxarraysize = 5;
				maxlength = 256;
				width = 99;
			};
		},
		DLG_LINE
		{
			prompt = "位置：";
			type = EEikCtListBox;
			id = cFindFileLocation;
			control = LISTBOX
			{
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				array_id = r_empty_array;
			};
		}
	};
}

RESOURCE ARRAY r_dialog_findfile_criteria_date_page
{
	items = 
	{
		DLG_LINE
		{
			prompt = "修改日期：";
			type = EEikCtChoiceList;
			id = cFindFileModifiedOption;
			control = CHOICELIST
			{
				array_id = r_findfile_modified_option_choices;
			};
		},
		DLG_LINE
		{
			type = EQikCtDateEditor;
			prompt = "从：";
			id = cFindFileModifiedFromDate;
			control = QIK_DATE_EDITOR
			{
				minDate = DATE
				{
					day = 0;
					month = 0;
					year = 1900;
				};
				maxDate = DATE
				{
					day = 30;
					month = 11;
					year = 2999;
				};
			};
		},
		DLG_LINE
		{
			type = EQikCtDateEditor;
			prompt = "到：";
			id = cFindFileModifiedToDate;
			control = QIK_DATE_EDITOR
			{
				minDate = DATE
				{
					day = 0;
					month = 0;
					year = 1900;
				};
				maxDate = DATE
				{
					day = 30;
					month = 11;
					year = 2999;
				};
			};
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_fontselector
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "重设"; }; },
		DLG_BUTTON { id = EEikBidCancel; button = CMBUT { txt = "取消"; }; },
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "确定"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

RESOURCE DIALOG r_dialog_fontselector
{
	title = "选择字型";
	buttons = r_buttons_fontselector;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			prompt = "字型：";
			type = EEikCtChoiceList;
			id = cFontSelectorFontList;
			control = CHOICELIST
			{
				array_id = r_empty_array;
			};
		},
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "大小：";
			id = cFontSelectorFontSize;
			control = QIK_NUMBER_EDITOR { min = 5; max = 30; };
		},
		DLG_LINE
		{
			prompt = "反锯齿状：";
			type = EEikCtCheckBox;
			id = cFontSelectorAntiAlias;
		},
		DLG_LINE
		{
			prompt = "预览：";
			type = EEikCtRichTextEditor;
			id = cFontSelectorPreview;
			control = RTXTED
			{ 
				width = 150; height = 60; numlines = 20; flags = EEikEdwinNoHorizScrolling | EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinDisplayOnly;
			};
		}
	};
}

RESOURCE MENU_PANE r_cellarea_edit
{
	items =
	{
		MENU_ITEM { command=cmdCellAreaEdit; txt="编辑"; flags = EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdCellAreaInitStore; txt="重新创建数据库"; },
		MENU_ITEM { command=cmdCellAreaCompress; txt="压缩"; },
		MENU_ITEM { command=cmdCellAreaUpdateStats; txt="更新统计信息"; }
	};
}

RESOURCE MENU_PANE r_cellarea_options
{
	items=
	{
		MENU_ITEM { command=cmdCellAreaAlwaysRun; txt="总是运行"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdCellAreaToggleRecording; txt="自动记录 Cell"; flags=EEikMenuItemCheckBox | EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdCellAreaEventDelay; txt="事件延迟"; }
	};	
}

RESOURCE MENU_BAR r_cellarea_menubar
{
	titles=
   	{
		MENU_TITLE { menu_pane = r_cellarea_options; txt="网络信息"; },
		MENU_TITLE { menu_pane = r_cellarea_edit; txt="数据库"; },
		MENU_TITLE { menu_pane = r_generic_fn; txt="功能"; flags = EEikMenuTitleRightAlign; }
	};
}

RESOURCE MENU_PANE r_fc_cellarea_menu
{
	items=
	{
		MENU_ITEM { cascade=r_fc_moduleswitch_menu; command=cmdFCSwitchMenu; txt="模组"; },
		MENU_ITEM { command=cmdFlushOut; txt="退出程序"; },
		MENU_ITEM { command=cmdUtilsFixApplist; txt="修复程序列表"; flags = EEikMenuItemSeparatorAfter; },
		MENU_ITEM { command=cmdCellAreaAlwaysRun; txt="总是运行"; flags=EEikMenuItemCheckBox; },
		MENU_ITEM { command=cmdCellAreaToggleRecording; txt="自动记录"; flags=EEikMenuItemCheckBox; }
	};	
}

RESOURCE MENU_PANE r_fc_moduleswitch_menu
{
	items=
	{
		MENU_ITEM { command=cmdSwitchFCBJackView; txt="蓝牙接口";},
		MENU_ITEM { command=cmdSwitchFCCellAreaView; txt="网络信息"; }
	};	
}

RESOURCE TBUF r_tbuf_credits { buf = "SMan 开发团队特别感谢 \
下列所有的人员以及组织：**\
Symbian*- Phil Spencer*- David Mery**\
Sony Ericsson*- James Lowerence*- Eric Goh**\
Motorola*- Peter Ho**\
Meero in My-Symbian.com's forum for the system font information**\
所有翻译 SMan 的人员**\
所有赞助 SMan 的人员"; }

RESOURCE DIALOG r_dialog_credits
{
	title = "感谢";
	buttons = R_EIK_BUTTONS_DONE;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cCreditsText;
			control = GTXTED
			{ 
				width = 150; height = 150; numlines = 20; flags = EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinDisplayOnly;
			};
		}
	};
}

RESOURCE ARRAY r_edit_cells_dialog_groups_page
{
	items = 
	{
		DLG_LINE
		{
			type = EEikCtColListBox;
			id = cCellDBGroupsListBox;
			control = LISTBOX
			{
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				array_id = r_empty_array;
				height = 6;
			};
		}
	};
}

RESOURCE ARRAY r_edit_cells_dialog_cells_page
{
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "群组：";
			id = cCellDBSelectedGroupOnCellsPage;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array; 
			};
		},
		DLG_LINE
		{
			prompt = "Cell：";
			type = EEikCtColListBox;
			id = cCellDBCellsListBox;
			control = LISTBOX
			{
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				array_id = r_empty_array;
				height = 5;
			};
		}
	};
}

RESOURCE ARRAY r_edit_cells_dialog_events_page
{
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "群组：";
			id = cCellDBSelectedGroupOnEventsPage;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array;
			};
		},
		DLG_LINE
		{
			prompt = "事件：";
			type = EEikCtColListBox;
			id = cCellDBEventsListBox;
			control = LISTBOX
			{
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				array_id = r_empty_array;
				height = 5;
			};
		}
	};
}

RESOURCE ARRAY r_edit_cells_dialog_importexport_page
{
	items = 
	{
		DLG_LINE
		{
			type = EEikCtListBox;
			id = cCellDBImportExportGroupListBox;
			control = LISTBOX 
			{ 
				array_id = r_empty_array;
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				height = 7;
			};
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_edit_cells_dialog_groups_page
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidCellGroupSelectAll; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderAll; bmpmask = EMbmMyheaderAllmask; }; },
		DLG_BUTTON { id = EEikBidCellGroupSelectNone; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderNone; bmpmask = EMbmMyheaderNonemask; }; },
		DLG_BUTTON { id = EEikBidCellGroupShiftUp; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderUparrow; bmpmask = EMbmMyHeaderUparrowmask; }; },
		DLG_BUTTON { id = EEikBidCellGroupShiftDown; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderDownarrow; bmpmask = EMbmMyHeaderDownarrowmask; }; },
		DLG_BUTTON { id = EEikBidCellGroupEnable; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCheckmark; bmpmask = EMbmMyHeaderCheckmarkmask; }; },
		DLG_BUTTON { id = EEikBidCellGroupDisable; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCrossmark; bmpmask = EMbmMyHeaderCrossmarkmask; }; },
		DLG_BUTTON { id = EEikBidCellGroupDel; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditdelete; bmpmask = EMbmMyHeaderCelleditdeletemask; }; },
		DLG_BUTTON { id = EEikBidCellGroupNew; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditnew; bmpmask = EMbmMyHeaderCelleditnewmask; }; },
		DLG_BUTTON { id = EEikBidCellGroupRen; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditedit; bmpmask = EMbmMyHeaderCellediteditmask; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "确定"; }; }
	};
}

RESOURCE DLG_BUTTONS r_buttons_edit_cells_dialog_cells_page
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidCellCellsSelectAll; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderAll; bmpmask = EMbmMyheaderAllmask; }; },
		DLG_BUTTON { id = EEikBidCellCellsSelectNone; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderNone; bmpmask = EMbmMyheaderNonemask; }; },
		DLG_BUTTON { id = EEikBidCellCellsFind; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderFindfile; bmpmask = EMbmMyheaderFindfilemask; }; },
		DLG_BUTTON { id = EEikBidCellCellsDel; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditdelete; bmpmask = EMbmMyHeaderCelleditdeletemask; }; },
		DLG_BUTTON { id = EEikBidCellCellsMove; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditmove; bmpmask = EMbmMyHeaderCelleditmovemask; }; },
		DLG_BUTTON { id = EEikBidCellCellsNew; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditnew; bmpmask = EMbmMyHeaderCelleditnewmask; }; },
		DLG_BUTTON { id = EEikBidCellCellsEdit; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditedit; bmpmask = EMbmMyHeaderCellediteditmask; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "确定"; }; }
	};
}

RESOURCE DLG_BUTTONS r_buttons_edit_cells_dialog_events_page
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidCellEventsSelectAll; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderAll; bmpmask = EMbmMyheaderAllmask; }; },
		DLG_BUTTON { id = EEikBidCellEventsSelectNone; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderNone; bmpmask = EMbmMyheaderNonemask; }; },
		DLG_BUTTON { id = EEikBidCellEventsShiftUp; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderUparrow; bmpmask = EMbmMyHeaderUparrowmask; }; },
		DLG_BUTTON { id = EEikBidCellEventsShiftDown; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderDownarrow; bmpmask = EMbmMyHeaderDownarrowmask; }; },
		DLG_BUTTON { id = EEikBidCellEventsEnable; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCheckmark; bmpmask = EMbmMyHeaderCheckmarkmask; }; },
		DLG_BUTTON { id = EEikBidCellEventsDisable; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCrossmark; bmpmask = EMbmMyHeaderCrossmarkmask; }; },
		DLG_BUTTON { id = EEikBidCellEventsDel; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditdelete; bmpmask = EMbmMyHeaderCelleditdeletemask; }; },
		DLG_BUTTON { id = EEikBidCellEventsMove; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditmove; bmpmask = EMbmMyHeaderCelleditmovemask; }; },
		DLG_BUTTON { id = EEikBidCellEventsNew; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditnew; bmpmask = EMbmMyHeaderCelleditnewmask; }; },
		DLG_BUTTON { id = EEikBidCellEventsEdit; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditedit; bmpmask = EMbmMyHeaderCellediteditmask; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "确定"; }; }
	};
}

RESOURCE DLG_BUTTONS r_buttons_edit_cells_dialog_importexport_page
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidCellImport; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditimport; bmpmask = EMbmMyHeaderCelleditimportmask; }; },
		DLG_BUTTON { id = EEikBidCellExport; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyHeaderCelleditexport; bmpmask = EMbmMyHeaderCelleditexportmask; }; },
		DLG_BUTTON { id = EEikBidCellImportExportSelectAll; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderAll; bmpmask = EMbmMyheaderAllmask; }; },
		DLG_BUTTON { id = EEikBidCellImportExportSelectNone; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderNone; bmpmask = EMbmMyheaderNonemask; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "确定"; }; }
	};
}

RESOURCE ARRAY r_edit_cells_dialog_pages
{
	items = 
	{	
		PAGE
		{
			id = idCellIdPage_Groups;
			buttons = r_buttons_edit_cells_dialog_groups_page;
			text = "群组";
			lines = r_edit_cells_dialog_groups_page;
		},
		PAGE
		{
			id = idCellIdPage_Cells;
			buttons = r_buttons_edit_cells_dialog_cells_page;
			text = "Cell";
			lines = r_edit_cells_dialog_cells_page;
		},
		PAGE
		{
			id = idCellIdPage_Events;
			buttons = r_buttons_edit_cells_dialog_events_page;
			text = "事件";
			lines = r_edit_cells_dialog_events_page;
		},
		PAGE
		{
			id = idCellIdPage_ImportExport;
			buttons = r_buttons_edit_cells_dialog_importexport_page;
			text = "共享";
			lines = r_edit_cells_dialog_importexport_page;
		}
	};
}

RESOURCE DIALOG r_edit_cells_dialog
{
	title = "Cell 信息数据库";
	flags = EEikDialogFlagWait;
	pages = r_edit_cells_dialog_pages;
	//buttons = R_EIK_BUTTONS_OK;
}

// Generic dialog for cell db editor
RESOURCE DIALOG r_dialog_misc_editstring
{
	title = "输入名称";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cString;
			control = GTXTED 
			{ 
				height = 55; width = 150; numlines = 1; textlimit = 255; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd;
			};
		}
	};
}

// Dialog for creating a new group
RESOURCE DIALOG r_dialog_cellid_newgroup
{
	title = "新建群组";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cString;
			control = GTXTED 
			{ 
				height = 55; width = 150; numlines = 1; textlimit = 255; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd;
			};
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "启动：";
			id = cCheckBox;
		}		
	};
}

// Dialog for creating a new cell
RESOURCE DIALOG r_dialog_cellid_newcell
{
	title = "Cell 说明";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "Cell 编号：";
			id = cCellId;
			control = QIK_NUMBER_EDITOR { min = 1; max = 0x7fffffff; };
		},
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			prompt = "国家编号：";
			id = cCountryCode;
			control = GTXTED 
			{ 
				height = 22; width = 150; numlines = 1; flags = EEikEdwinJustAutoCurEnd; textlimit = 4;
			};
		},
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			prompt = "网络编号：";
			id = cNetworkCode;
			control = GTXTED 
			{ 
				height = 22; width = 150; numlines = 1; flags = EEikEdwinJustAutoCurEnd; textlimit = 8;
			};
		},
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			prompt = "说明：";
			id = cCellDescription;
			control = GTXTED
			{ 
				height = 55; width = 150; numlines = 1; textlimit = 50; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd;
			};
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "群组：";
			id = cCellGroups;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array; 
			};
		}
	};
}

// Dialog for moving a bunch of cells to another group
RESOURCE DIALOG r_dialog_cellid_movecell
{
	title = "选择群组";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "群组：";
			id = cCellGroups;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array; 
			};
		}
	};
}

// Dialog for finding a new cell
RESOURCE DIALOG r_dialog_cellid_findcell
{
	title = "找到 Cell";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "编号：";
			id = cCellId;
			control = QIK_NUMBER_EDITOR { min = 1; max = 0x7fffffff; };
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_foundcells
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "切换到"; }; flags=EEikLabeledButtonIsDefault; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "关闭"; }; }
	};
}

// Dialog to display found cells
RESOURCE DIALOG r_dialog_cellid_foundcell
{
	title = "查找结果";
	buttons = r_buttons_foundcells;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			prompt = "群组：";
			type = EEikCtListBox;
			id = cCellGroups;
			control = LISTBOX
			{
				array_id = r_empty_array;
				height = 6;
			};
		}
	};
}

// The numbers at the end of the array represent the event_id that is stored in the
// cell database. Do not change！！ Note: number 4 cannot be used because it is reserved for
// an unknown event type. The reason for this is because in our icon array for the columnlistbox,
// icon index #4 (i.e. the 5th icon) is a special icon to indicate an unknown event type.
// The 1st icon is for checkmark
// The 2nd icon is for crossmark
// The 3rd icon is for audio event
// The 4th icon is for vibrate event
// The 5th icon is for unknown event
RESOURCE ARRAY r_event_type_array
{
	items=
	{	
		LBUF { txt="播放音效;0"; },
		LBUF { txt="震动;1"; }
	};	
}

// Number 4 cannot be used.
//
// The numbers in this array are the actual numbers that are stored in the database. However,
// for indexing into the icon array, they are a continuation of the above list.
//
// The 6th icon is for enter trigger
// The 7th icon is for leave trigger
// The 8th icon is for both trigger
RESOURCE ARRAY r_event_type_trigger
{
	items=
	{	
		LBUF { txt="进入;0"; },
		LBUF { txt="离开;1"; },
		LBUF { txt="进入与离开;2"; }
	};	
}

// Dialog to select type of event to create
RESOURCE DIALOG r_dialog_event_type_selection
{
	title = "指定事件";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "型式：";
			id = cChoiceList;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array;
			};
		},
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			prompt = "说明：";
			id = cString;
			control = GTXTED
			{ 
				height = 55; width = 150; numlines = 1; textlimit = 50; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd;
			};
		},
		DLG_LINE
		{
			type = EEikCtCheckBox;
			prompt = "启动：";
			id = cCheckBox;
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "触发：";
			id = cTriggerList;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array;
			};
		},		
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "群组：";
			id = cCellGroups;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array;
			};
		}
	};
}

// Dialog to create new audio event
RESOURCE DIALOG r_dialog_event_audio
{
	title = "事件 - 播放音讯";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "音讯：";
			id = cChoiceList;
			control = CHOICELIST 
			{ 
				array_id = r_array_audionotify;
			};
		}
	};
}

// Dialog to edit audio event
RESOURCE DIALOG r_dialog_event_edit_audio
{
	title = "事件 - 播放音讯";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			prompt = "说明：";
			id = cString;
			control = GTXTED
			{ 
				height = 55; width = 150; numlines = 1; textlimit = 50; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd;
			};
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "触发：";
			id = cTriggerList;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array;
			};
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "音讯：";
			id = cChoiceList;
			control = CHOICELIST 
			{ 
				array_id = r_array_audionotify;
			};
		}
	};
}

// Dialog to edit vibration event
RESOURCE DIALOG r_dialog_event_edit_vibrate
{
	title = "事件 - 震动";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			prompt = "说明：";
			id = cString;
			control = GTXTED
			{ 
				height = 55; width = 150; numlines = 1; textlimit = 50; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd;
			};
		},
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "触发：";
			id = cTriggerList;
			control = CHOICELIST 
			{ 
				array_id = r_event_type_trigger;
			};
		}
	};
}

/*
// Generic dialog to prompt for drive letter (RAM only)
RESOURCE DIALOG r_dialog_prompt_drive_letter
{
	title = "选择磁盘";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "磁盘：";
			id = cChoiceList;
			control = CHOICELIST 
			{ 
				array_id = r_empty_array;
			};
		}
	};
}
*/

/*
// List files to import
RESOURCE DIALOG r_edit_cells_dialog_import
{
	title = "选择文件";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtListBox;
			prompt = "导入：";
			id = cCellDBImportExportFileListBox;
			control = LISTBOX 
			{ 
				array_id = r_empty_array;
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
			};
		}
	};
}
*/

RESOURCE DIALOG r_dialog_cells_importexport_progress
{
	title = "进度";
	buttons = R_EIK_BUTTONS_OK;
	items=
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			prompt = "目前：";
			id = cString;
			control = GTXTED
			{ 
				height = 100; width = 150; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd | EEikEdwinReadOnly;
			};
		},
		DLG_LINE
		{
			prompt = "进度：";
			type = EEikCtProgInfo;
			id = cProgressInfo;
			control = PROGRESSINFO
			{
				text_type = EEikProgressTextPercentage;
			};
		}
	};
}

// Debug dialog to fool sman into thinking it is running on a particular phone model
#ifdef __DEBUG_BUILD__
RESOURCE DIALOG r_dialog_debug
{
	title = "选择手机型号";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "手机：";
			id = cChoiceList;
			control = CHOICELIST 
			{ 
				array_id = r_phone_model_array;
			};
		}
	};
}
#endif

// For debug menu
#ifdef __DEBUG_BUILD__
RESOURCE ARRAY r_phone_model_array
{
	items=
	{	
		LBUF { txt="SonyEricsson P800"; },
		LBUF { txt="SonyEricsson P900"; },
		LBUF { txt="SonyEricsson P910"; },
		LBUF { txt="Motorola A920"; },
		LBUF { txt="Motorola A920(C)"; },
		LBUF { txt="Motorola A925"; },
		LBUF { txt="Motorola A925(C)"; },
		LBUF { txt="BenQ P30"; },
		LBUF { txt="<未知>"; }
	};	
}
#endif

RESOURCE DLG_BUTTONS r_buttons_bluejack_exclusionlist
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidAdd; button = CMBUT { txt = "新建"; }; },
		DLG_BUTTON { id = EEikBidDel; button = CMBUT { txt = "删除"; }; },
		DLG_BUTTON { id = EEikBidEdit; button = CMBUT { txt = "编辑"; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "确定"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

// Edit bluejack exclusion list
// List files to import
RESOURCE DIALOG r_edit_bluejack_exclusionlist
{
	title = "蓝牙设备";
	buttons = r_buttons_bluejack_exclusionlist;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtColListBox;
			id = cListBox;
			control = LISTBOX 
			{ 
				array_id = r_empty_array;
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				height = 6;
			};
		}
	};
}

// Edit bt exclusion list device
RESOURCE DIALOG r_dialog_bluejack_editdevice
{
	title = "编辑项目";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cString;
			control = GTXTED 
			{ 
				height = 55; width = 150; numlines = 1; textlimit = 255; flags = EEikEdwinNoHorizScrolling | EEikEdwinJustAutoCurEnd;
			};
		}
	};
}

RESOURCE ARRAY r_maxtodopriority_array
{
	items=
	{	
		LBUF { txt="全部"; },
		LBUF { txt="1"; },
		LBUF { txt="2"; },
		LBUF { txt="3"; },
		LBUF { txt="4"; },
		LBUF { txt="5"; },
		LBUF { txt="6"; },
		LBUF { txt="7"; },
		LBUF { txt="8"; },
		LBUF { txt="9"; }
	};	
}

// Set max todo priority to display in today view
RESOURCE DIALOG r_dialog_agenda_maxtodopriority
{
	title = "优先级过滤";
	buttons = r_buttons_ok_cancel;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtChoiceList;
			prompt = "优先级：";
			id = cChoiceList;
			control = CHOICELIST 
			{ 
				array_id = r_maxtodopriority_array;
			};
		}
	};
}

/*
RESOURCE DIALOG r_dialog_folder_selector
{
	title = "选择文件夹";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items =
	{
		DLG_LINE
		{
			type = EEikCtFolderNameSel;
			prompt = "文件夹：";
			id = cFolderSelector;
			control = FILENAMESELECTOR 
			{ 
			};
		},
		DLG_LINE
		{
			type = EEikCtDriveNameSel;
			prompt = "磁盘：";
			id = cDriveSelector;
			control = DRIVENAMESELECTOR 
			{ 
			};
		}
	};
}
*/

RESOURCE DIALOG r_dialog_cellarea_event_firing_delay
{
	title = "事件延迟";
	buttons = r_buttons_bluebeamlog;
	flags = EEikDialogFlagWait;
	items=
	{
		DLG_LINE
		{
			type = EQikCtNumberEditor;
			prompt = "延迟 (分钟)：";
			id = cNumberEditor;
			control = QIK_NUMBER_EDITOR { min = 0; max = 60; };
		}
	};
}

RESOURCE DLG_BUTTONS r_buttons_folder_file_browser
{
	buttons =
	{
		DLG_BUTTON { id = EEikBidSelectAll; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderAll; bmpmask = EMbmMyheaderAllmask; }; },
		DLG_BUTTON { id = EEikBidSelectNone; button = CMBUT { bmpfile = "*"; bmpid = EMbmMyheaderNone; bmpmask = EMbmMyheaderNonemask; }; },
		DLG_BUTTON { id = EEikBidOk; button = CMBUT { txt = "取消"; }; },
		DLG_BUTTON { id = EEikBidYes; button = CMBUT { txt = "确定"; }; flags=EEikLabeledButtonIsDefault; }
	};
}

RESOURCE DIALOG r_dialog_folder_file_browser
{
	title = "选择文件夹";
	buttons = r_buttons_folder_file_browser;
	flags = EEikDialogFlagWait;
	items=
	{
		DLG_LINE
		{
			type = EEikTextListBoxWithPointerHandler;
			id = cListBox;
			control = LISTBOX
			{
				array_id = r_empty_array;
				flags = EEikListBoxMultipleSelection | EEikListBoxNoExtendedSelection;
				height = 6;
			};
		}
	};
}

RESOURCE TBUF r_tbuf_unknownphonemessage { buf = "SMan has detected that it is running on a \
phone it does not recognize. Please send the following information to*\
- ajack (ajack2001my@yahoo.com); or*\
- drollercoaster (drollercoaster@hotmail.com):**\
1. Machine UID*\
2. Manufacturer UID**\
You can obtain this information from the SysInfo module in SMan.**\
Thank you."; }

RESOURCE DIALOG r_dialog_unknownphonedialog
{
	title = "Information";
	buttons = R_EIK_BUTTONS_DONE;
	flags = EEikDialogFlagWait;
	items = 
	{
		DLG_LINE
		{
			type = EEikCtGlobalTextEditor;
			id = cCreditsText;
			control = GTXTED
			{ 
				width = 150; height = 150; numlines = 20; flags = EEikEdwinReadOnly | EEikEdwinNoAutoSelection | EEikEdwinDisplayOnly;
			};
		}
	};
}
