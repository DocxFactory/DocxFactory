
/**
 * slibwinprop.i -
 *
 * (c) Copyright ABC Alon Blich Consulting Tech, Ltd.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Contact information
 *  Email: alonblich@gmail.com
 *  Phone: +972-54-218-8086
 */



/* Generic Values */
&global INVALID_HANDLE_VALUE -1

/* GetExitProcess exitcode */
&global STILL_ACTIVE 259



/* GetVersionEx VersionInfo wProductType */
&global VER_NT_WORKSTATION                  1
&global VER_NT_DOMAIN_CONTROLLER            2
&global VER_NT_SERVER                       3

/* GetVersionEx VersionInfo wSuiteMask */
&global VER_SUITE_SMALLBUSINESS             1
&global VER_SUITE_ENTERPRISE                2
&global VER_SUITE_BACKOFFICE                4
&global VER_SUITE_COMMUNICATIONS            8
&global VER_SUITE_TERMINAL                  16
&global VER_SUITE_SMALLBUSINESS_RESTRICTED  32
&global VER_SUITE_EMBEDDEDNT                64
&global VER_SUITE_DATACENTER                128
&global VER_SUITE_SINGLEUSERTS              256
&global VER_SUITE_PERSONAL                  512
&global VER_SUITE_BLADE                     1024



/* CreateFile dwDesiredAccess */
&global GENERIC_EXECUTE     536870912
&global GENERIC_WRITE       1073741824
&global GENERIC_READ       -2147483648

/* CreateFile dwShareMode */
&global FILE_SHARE_READ     1
&global FILE_SHARE_WRITE    2
&global FILE_SHARE_DELETE   4

/* CreateFile dwCreationDisposition */
&global CREATE_ALWAYS       2
&global CREATE_NEW          1
&global OPEN_ALWAYS         4
&global OPEN_EXISTING       3
&global TRUNCATE_EXISTING   5

/* CreateFile dwFlagsandAttributes */
&global FILE_ATTRIBUTE_NORMAL       128
&global FILE_FLAG_DELETE_ON_CLOSE   67108864



/* GetDriveType return values */
&global DRIVE_UNKNOWN       0
&global DRIVE_NO_ROOT_DIR   1
&global DRIVE_REMOVABLE     2
&global DRIVE_FIXED         3
&global DRIVE_REMOTE        4
&global DRIVE_CDROM         5
&global DRIVE_RAMDISK       6



/* SHGetSpecialFolderLocation nFolder */
&global CSIDL_ADMINTOOLS                48
&global CSIDL_ALTSTARTUP                29
&global CSIDL_APPDATA                   26
&global CSIDL_BITBUCKET                 10
&global CSIDL_COMMON_ADMINTOOLS         47
&global CSIDL_COMMON_ALTSTARTUP         30
&global CSIDL_COMMON_DESKTOPDIRECTORY   25
&global CSIDL_COMMON_DOCUMENTS          46
&global CSIDL_COMMON_FAVORITES          31
&global CSIDL_COMMON_PROGRAMS           23
&global CSIDL_COMMON_STARTMENU          22
&global CSIDL_COMMON_STARTUP            24
&global CSIDL_COMMON_TEMPLATES          45
&global CSIDL_CONTROLS                  3
&global CSIDL_COOKIES                   33
&global CSIDL_DESKTOP                   0
&global CSIDL_DESKTOPDIRECTORY          16
&global CSIDL_DRIVES                    17
&global CSIDL_FAVORITES                 6
&global CSIDL_FONTS                     20
&global CSIDL_HISTORY                   34
&global CSIDL_INTERNET                  1
&global CSIDL_INTERNET_CACHE            32
&global CSIDL_LOCAL_APPDATA             28
&global CSIDL_MYPICTURES                39
&global CSIDL_NETHOOD                   19
&global CSIDL_NETWORK                   18
&global CSIDL_PERSONAL                  5
&global CSIDL_PRINTERS                  4
&global CSIDL_PRINTHOOD                 27
&global CSIDL_PROFILE                   40
&global CSIDL_PROGRAMS                  2
&global CSIDL_PROGRAM_FILES             38
&global CSIDL_PROGRAM_FILESX86          42
&global CSIDL_PROGRAM_FILES_COMMON      43
&global CSIDL_PROGRAM_FILES_COMMONX86   44
&global CSIDL_RECENT                    8
&global CSIDL_SENDTO                    9
&global CSIDL_STARTMENU                 11
&global CSIDL_STARTUP                   7
&global CSIDL_SYSTEM                    37
&global CSIDL_SYSTEMX86                 41
&global CSIDL_TEMPLATES                 21
&global CSIDL_WINDOWS                   36
&global CSIDL_FLAG_CREATE               32768
&global CSIDL_FLAG_DONT_VERIFY          16384
&global CSIDL_FLAG_MASK                 65280



/* OpenProcess dwDesiredAccess */
&global PROCESS_CREATE_PROCESS              128
&global PROCESS_CREATE_THREAD               2
&global PROCESS_DUP_HANDLE                  64
&global PROCESS_QUERY_INFORMATION           1024
&global PROCESS_QUERY_LIMITED_INFORMATION   4096
&global PROCESS_SET_INFORMATION             512
&global PROCESS_SET_QUOTA                   256
&global PROCESS_SUSPEND_RESUME              2048
&global PROCESS_TERMINATE                   1
&global PROCESS_VM_OPERATION                8
&global PROCESS_VM_READ                     16
&global PROCESS_VM_WRITE                    32



/* reateToolhelp32Snapshot dwFlags */
&global TH32CS_INHERIT      2147483648
&global TH32CS_SNAPHEAPLIST 1
&global TH32CS_SNAPMODULE   8
&global TH32CS_SNAPMODULE32 16
&global TH32CS_SNAPPROCESS  2
&global TH32CS_SNAPTHREAD   4
 


/* ShellExecuteEx fMask */
&global SEE_MASK_FLAG_NOCLOSEPROCESS    64
&global SEE_MASK_FLAG_DDEWAIT           256
&global SEE_MASK_FLAG_NO_UI             1024

/* ShellExecuteEx nShow */
&global SW_HIDE             0
&global SW_MAXIMIZE         3
&global SW_MINIMIZE         6
&global SW_RESTORE          1
&global SW_SHOW             5
&global SW_SHOWDEFAULT      10
&global SW_SHOWMAXIMIZED    3
&global SW_SHOWMINIMIZED    2
&global SW_SHOWMINNOACTIVE  7
&global SW_SHOWNA           8
&global SW_SHOWNOACTIVATE   4
&global SW_SHOWNORMAL       1

/* ShellExecuteEx hInstApp */
&global SE_ERR_FNF              2
&global SE_ERR_PNF              3
&global SE_ERR_ACCESSDENIED     5
&global SE_ERR_OOM              8
&global SE_ERR_DLLNOTFOUND      32
&global SE_ERR_SHARE            26
&global SE_ERR_ASSOCINCOMPLETE  27
&global SE_ERR_DDETIMEOUT       28
&global SE_ERR_DDEFAIL          29
&global SE_ERR_DDEBUSY          30
&global SE_ERR_NOASSOC          31

/* CreateProcess lpStartInfo dwFlags */
&global STARTF_FORCEONFEEDBACK  64
&global STARTF_FORCEOFFFEEDBACK 128
&global STARTF_RUNFULLSCREEN    32
&global STARTF_USEFILLATTRIBUTE 16
&global STARTF_USECOUNTCHARS    8
&global STARTF_USEPOSITION      4
&global STARTF_USESHOWWINDOW    1
&global STARTF_USESIZE          2
&global STARTF_USESTDHANDLES    256



/* SendInput input type */
&global INPUT_MOUSE     0
&global INPUT_KEYBOARD  1
&global INPUT_HARDWARE  2

/* SendInput virtual key codes */
&global VK_LBUTTON      01 /* left mouse button */
&global VK_RBUTTON      02 /* right mouse button */
&global VK_CANCEL       03 /* ctrll-break processing */
&global VK_MBUTTON      04 /* middle mouse button */
&global VK_BACK         08
&global VK_TAB          09
&global VK_CLEAR        12
&global VK_RETURN       13
&global VK_SHIFT        16
&global VK_CONTROL      17
&global VK_MENU         18 /* alt */
&global VK_PAUSE        19
&global VK_CAPITAL      20
&global VK_ESCAPE       27
&global VK_SPACE        32
&global VK_PRIOR        33 /* page up */
&global VK_NEXT         34 /* page down */
&global VK_END          35
&global VK_HOME         36
&global VK_LEFT         37
&global VK_UP           38
&global VK_RIGHT        39
&global VK_DOWN         40
&global VK_SELECT       41
&global VK_EXECUTE      43
&global VK_SNAPSHOT     44 /* print screen */
&global VK_INSERT       45
&global VK_DELETE       46
&global VK_HELP         47
&global VK_LWIN         91
&global VK_RWIN         92
&global VK_APPS         93
&global VK_NUMPAD0      96
&global VK_NUMPAD1      97
&global VK_NUMPAD2      98
&global VK_NUMPAD3      99
&global VK_NUMPAD4      100
&global VK_NUMPAD5      101
&global VK_NUMPAD6      102
&global VK_NUMPAD7      103
&global VK_NUMPAD8      104
&global VK_NUMPAD9      105
&global VK_MULTIPLY     106
&global VK_ADD          107
&global VK_SEPARATOR    108
&global VK_SUBTRACT     109
&global VK_DECIMAL      110
&global VK_DIVIDE       111
&global VK_F1           112
&global VK_F2           113
&global VK_F3           114
&global VK_F4           115
&global VK_F5           116
&global VK_F6           117
&global VK_F7           118
&global VK_F8           119
&global VK_F9           120
&global VK_F10          121
&global VK_F11          122
&global VK_F12          123
&global VK_F13          124
&global VK_F14          125
&global VK_F15          126
&global VK_F16          127
&global VK_F17          128
&global VK_F18          129
&global VK_F19          130
&global VK_F20          131
&global VK_F21          132
&global VK_F22          133 
&global VK_F23          134
&global VK_F24          135
&global VK_NUMLOCK      144
&global VK_SCROLL       145
&global VK_LSHIFT       160
&global VK_RSHIFT       161
&global VK_LCONTROL     162
&global VK_RCONTROL     163
&global VK_LMENU        164
&global VK_RMENU        165
&global VK_PACKET       231
&global VK_ATTN         246
&global VK_CRSEL        247
&global VK_EXSEL        248
&global VK_EREOF        249
&global VK_PLAY         250
&global VK_ZOOM         251
&global VK_PA1          253
&global VK_OEM_CLEAR    254

/* SendInput keyboard dwFlags */
&global KEYEVENTF_EXTENDEDKEY   1
&global KEYEVENTF_KEYUP         2
&global KEYEVENTF_UNICODE       4
&global KEYEVENTF_SCANCODE      8

/* SendInput mouse dwFlags */
&global MOUSEEVENTF_MOVE        1   
&global MOUSEEVENTF_LEFTDOWN    2
&global MOUSEEVENTF_LEFTUP      4
&global MOUSEEVENTF_RIGHTDOWN   8
&global MOUSEEVENTF_RIGHTUP     16
&global MOUSEEVENTF_MIDDLEDOWN  32
&global MOUSEEVENTF_MIDDLEUP    64
&global MOUSEEVENTF_XDOWN       128 /* 2000 or higher */
&global MOUSEEVENTF_XUP         256 /* 2000 or higher */
&global MOUSEEVENTF_WHEEL       2048
&global MOUSEEVENTF_HWHEEL      4096
&global MOUSEEVENTF_VIRTUALDESK 16384
&global MOUSEEVENTF_ABSOLUTE    32768



/* GetSystemMetrics nIndex */
&global SM_ARRANGE                      56
&global SM_CLEANBOOT                    67
&global SM_CMONITORS                    80
&global SM_CMOUSEBUTTONS                43
&global SM_CXBORDER                     5
&global SM_CXCURSOR                     13
&global SM_CXDLGFRAME                   7
&global SM_CXDOUBLECLK                  36
&global SM_CXDRAG                       68
&global SM_CXEDGE                       45
&global SM_CXFIXEDFRAME                 7
&global SM_CXFOCUSBORDER                83
&global SM_CXFRAME                      32
&global SM_CXFULLSCREEN                 16
&global SM_CXHSCROLL                    21
&global SM_CXHTHUMB                     10
&global SM_CXICON                       11
&global SM_CXICONSPACING                38
&global SM_CXMAXIMIZED                  61
&global SM_CXMAXTRACK                   59
&global SM_CXMENUCHECK                  71
&global SM_CXMENUSIZE                   54
&global SM_CXMIN                        28
&global SM_CXMINIMIZED                  57
&global SM_CXMINSPACING                 47
&global SM_CXMINTRACK                   34
&global SM_CXPADDEDBORDER               92
&global SM_CXSCREEN                     0
&global SM_CXSIZE                       30
&global SM_CXSIZEFRAME                  32
&global SM_CXSMICON                     49
&global SM_CXSMSIZE                     52
&global SM_CXVIRTUALSCREEN              78
&global SM_CXVSCROLL                    2
&global SM_CYBORDER                     6
&global SM_CYCAPTION                    4
&global SM_CYCURSOR                     14
&global SM_CYDLGFRAME                   8
&global SM_CYDOUBLECLK                  37
&global SM_CYDRAG                       69
&global SM_CYEDGE                       46
&global SM_CYFIXEDFRAME                 8
&global SM_CYFOCUSBORDER                84
&global SM_CYFRAME                      33
&global SM_CYFULLSCREEN                 17
&global SM_CYHSCROLL                    3
&global SM_CYICON                       12
&global SM_CYICONSPACING                39
&global SM_CYKANJIWINDOW                18
&global SM_CYMAXIMIZED                  62
&global SM_CYMAXTRACK                   60
&global SM_CYMENU                       15
&global SM_CYMENUCHECK                  72
&global SM_CYMENUSIZE                   55
&global SM_CYMIN                        29
&global SM_CYMINIMIZED                  58
&global SM_CYMINSPACING                 48
&global SM_CYMINTRACK                   35
&global SM_CYSCREEN                     1
&global SM_CYSIZE                       31
&global SM_CYSIZEFRAME                  33
&global SM_CYSMCAPTION                  51
&global SM_CYSMICON                     50
&global SM_CYSMSIZE                     53
&global SM_CYVIRTUALSCREEN              79
&global SM_CYVSCROLL                    20
&global SM_CYVTHUMB                     9
&global SM_DBCSENABLED                  42
&global SM_DEBUG                        22
&global SM_IMMENABLED                   82
&global SM_MEDIACENTER                  87
&global SM_MENUDROPALIGNMENT            40
&global SM_MIDEASTENABLED               74
&global SM_MOUSEPRESENT                 19
&global SM_MOUSEHORIZONTALWHEELPRESENT  91
&global SM_MOUSEWHEELPRESENT            75
&global SM_NETWORK                      63
&global SM_PENWINDOWS                   41
&global SM_REMOTECONTROL                8193
&global SM_REMOTESESSION                4096
&global SM_SAMEDISPLAYFORMAT            81
&global SM_SECURE                       44
&global SM_SERVERR2                     89
&global SM_SHOWSOUNDS                   70
&global SM_SHUTTINGDOWN                 8192
&global SM_SLOWMACHINE                  73
&global SM_STARTER                      88
&global SM_SWAPBUTTON                   23
&global SM_TABLETPC                     86
&global SM_XVIRTUALSCREEN               76
&global SM_YVIRTUALSCREEN               77
 


/* SetWindowLongA nIndex */
&global GWL_STYLE               -16
&global GWL_EXSTYLE             -20

/* SetWindowLongA dwNewLong */
&global ES_LEFT                 0
&global ES_CENTER               1
&global ES_RIGHT                2
&global ES_MULTILINE            4
&global ES_UPPERCASE            8
&global ES_LOWERCASE            16
&global ES_PASSWORD             32
&global ES_AUTOVSCROLL          64
&global ES_AUTOHSCROLL          128
&global ES_NOHIDESEL            256
&global ES_OEMCONVERT           1024
&global ES_READONLY             2048
&global ES_WANTRETURN           4096

&global WS_EX_DLGMODALFRAME     1
&global WS_EX_NOPARENTNOTIFY    4
&global WS_EX_TOPMOST           8
&global WS_EX_ACCEPTFILES       16
&global WS_EX_TRANSPARENT       32
&global WS_EX_MDICHILD          64
&global WS_EX_TOOLWINDOW        128
&global WS_EX_WINDOWEDGE        256
&global WS_EX_CLIENTEDGE        512
&global WS_EX_CONTEXTHELP       1024
&global WS_EX_RIGHT             4096
&global WS_EX_LEFT              0
&global WS_EX_RTLREADING        8192
&global WS_EX_LTRREADING        0
&global WS_EX_LEFTSCROLLBAR     16384
&global WS_EX_RIGHTSCROLLBAR    0
&global WS_EX_CONTROLPARENT     65536
&global WS_EX_STATICEDGE        131072
&global WS_EX_APPWINDOW         262144
&global WS_EX_OVERLAPPEDWINDOW  768
&global WS_EX_PALETTEWINDOW     392
&global WS_EX_LAYERED           524288
&global WS_EX_NOINHERITLAYOUT   1048576
&global WS_EX_LAYOUTRTL         4194304
&global WS_EX_NOACTIVATE        134217728

/* GetKeyboardLayoutName pwszKLID. for reference look at Locale Identifier Constants and Strings http://msdn2.microsoft.com/en-us/library/ms776260.aspx */
&global LANG_EN_US "00000409"
&global LANG_HE_IL "0000040D"



/* scan codes are the actual hardware specific key code. virtual keys are non hardware specific
   codes. the keyboard driver takes care of the mapping. unless theres a specific use don't
   bother with the scan codes only use the virtual keys. */

define temp-table win_ttInput no-undo

    field iSeq          as int
    field dwType        as int

    /* keyboard input */

    field wVk           as int
    field wScan         as int 

    /* mouse input */

    /* the ( x, y ) coordinates are 1 based. win_sendInput does this and other conversion. */

    field dX            as int
    field dY            as int
    field dwMouseData   as int

    /* keyboard and mouse shared fields */

    field dwFlags       as int
    field dwTime        as int
    field lpExtraInfo   as int

    /* hardware input */

    field uMsg          as int
    field wParamL       as int
    field wParamH       as int

    index iSeq is primary unique iSeq.



define temp-table win_ttProcess no-undo

    field cPName    as char
    field iPid      as int
    index iPid      is primary unique iPid.



/* VersionInfoEx */

define new global shared var win_cVersionStr        as char no-undo.
define new global shared var win_cVersionName       as char no-undo.
define new global shared var win_iMajorVersion      as int no-undo.
define new global shared var win_iMinorVersion      as int no-undo.
define new global shared var win_iBuildNumber       as int no-undo.
define new global shared var win_iPlatformId        as int no-undo.
define new global shared var win_cCSDVersion        as char no-undo.
define new global shared var win_iServicePackMajor  as int no-undo.
define new global shared var win_iServicePackMinor  as int no-undo.
define new global shared var win_iSuiteMask         as int no-undo.
define new global shared var win_iProductType       as int no-undo.
define new global shared var win_iReserved          as int no-undo.



/* win_batch */

define new global shared var win_iErrorLevel        as int no-undo.



/* win_deleteTempFiles */

define new global shared var win_tDeleteTempFiles   as date no-undo init ?.
