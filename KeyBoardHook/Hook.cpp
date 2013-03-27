// Hook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include<Tlhelp32.h>
#include<wchar.h>
//#include<fstream>
//#include<iostream>
//using namespace std;

#pragma data_seg(".Shared")
HHOOK  g_hKeyHook = NULL;
HWND   g_hwnd = NULL;
DWORD  g_dwThreadID=0;
#pragma data_seg()

#pragma comment(linker,"/section:.Shared,rws")



const int  KEY_COUNT = 6;

//用于传递键值信息
struct CKeyInfo
{
	wchar_t OldKey[KEY_COUNT];
	wchar_t NewKey[KEY_COUNT];
	wchar_t Num[KEY_COUNT]; 
	CKeyInfo()
	{
		for(int i=0;i<KEY_COUNT;++i)
		{
			OldKey[i]='\0';
			NewKey[i]='\0';
		}
	}

	CKeyInfo(CKeyInfo& info)
	{
		for(int i=0;i<KEY_COUNT; i++)
		{
			OldKey[i] = info.OldKey[i];
			NewKey[i] = info.NewKey[i];
			Num[i] = info.Num[i];
		}
	}

	CKeyInfo& operator=(const CKeyInfo& info)
	{
		for(int i=0;i<KEY_COUNT; i++)
		{
			OldKey[i] = info.OldKey[i];
			NewKey[i] = info.NewKey[i];
			Num[i] = info.Num[i];
		}
		return *this;
	}
};

#define VK_NUM1	0x61
#define VK_NUM2	0x62
#define VK_NUM4	0x64
#define VK_NUM5	0x65
#define VK_NUM7	0x67
#define VK_NUM8	0x68


HINSTANCE g_hinstDll=NULL;		//DLL句柄
CKeyInfo  g_keyInfo;		//参数信息
bool	  g_bPrivilegeImproved=false;  //标记特权是否已提升

//提高特权，使本进程具备debug特权
bool ImprovePrivilege()
{
	HANDLE  hTokenHandle=NULL;
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hTokenHandle);
	bool bChangePrivSuc=false;
	if(hTokenHandle)
	{
		TOKEN_PRIVILEGES tp, oldTp;
		LUID luid;
		bool bRes = LookupPrivilegeValue(NULL, TEXT("SeDebugPrivilege"), &luid);
		if(bRes)  //读取luid成功
		{
			tp.PrivilegeCount=1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			bChangePrivSuc = AdjustTokenPrivileges(hTokenHandle,false,&tp,
										sizeof(tp), NULL, NULL);

		}
	}
	if(bChangePrivSuc)
		g_bPrivilegeImproved = true;
	else
		g_bPrivilegeImproved=false;

	return bChangePrivSuc;
}

#define  CHAT_ADDRESS_24E  (0x6FAE8450)    //1.24E 版本聊天状态的内存地址
//读取魔兽内存，判断是否处于聊天状态
bool  ReadChatFlag(bool& isChatting)
{
	isChatting = false;
	if(g_hwnd==NULL)
		return false;
	DWORD pid;								//魔兽进程ID
	GetWindowThreadProcessId(g_hwnd,&pid);  //获取进程的ID

	//如果特权没有提升，则提升权限，否则无法读取魔兽内存
	if(!g_bPrivilegeImproved)
	{
		ImprovePrivilege();
	}

	HANDLE hProcess;			//魔兽进程句柄
	if(g_bPrivilegeImproved)  //权限提高成功
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS,false,pid);  //打开进程
	}
	int res=0;
	BYTE  buf = 0;		//存储读取的内存地址
	if(hProcess)
	{
		res = ReadProcessMemory(hProcess,(LPVOID)CHAT_ADDRESS_24E , &buf,1,NULL);  //读取内存

		if(res!=0)  //读取成功
		{
			isChatting = buf==1 ? true : false;
			return true;
		}
		else
		{
			isChatting = false;
			return true;
		}
	}
	else
		return false;

	return true;
}

bool  OpenMap()
{
	if(g_hwnd==NULL)
		return false;
	DWORD pid;								//魔兽进程ID
	GetWindowThreadProcessId(g_hwnd,&pid);  //获取进程的ID

	//如果特权没有提升，则提升权限，否则无法读取魔兽内存
	if(!g_bPrivilegeImproved)
	{
		ImprovePrivilege();
	}

	HANDLE hProcess;			//魔兽进程句柄
	if(g_bPrivilegeImproved)  //权限提高成功
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS,false,pid);  //打开进程
	}
	int res=0;
	int nMapType = 1;  //总是可见
	if(hProcess)
	{
//		res = ReadProcessMemory(hProcess,(LPVOID)CHAT_ADDRESS_24E , &buf,1,NULL);  //读取内存
		res = WriteProcessMemory(hProcess,(LPVOID)0x09A50478,&nMapType,4,NULL);
		if(res!=0)  //写入成功
		{
			return true;
		}
		else
		{
			return true;
		}
	}
	else
		return false;

	return true;
}


//钩子过程
static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//首先判断魔兽是否处于激活状态，不处于激活状态则不进行改建
	HWND ahwnd = GetForegroundWindow(); //当前激活的窗口
	if(ahwnd && ahwnd!=g_hwnd)
		return CallNextHookEx(NULL,nCode,wParam,lParam);

	if(nCode<0)
		CallNextHookEx(NULL,nCode,wParam,lParam);
	if(nCode==HC_ACTION)
	{
		//检查是否处于聊天状态
		bool isChat=false;
		bool bSuc = ReadChatFlag(isChat);
		if(bSuc && isChat)  //聊天,不进行改建
			return CallNextHookEx(NULL,nCode,wParam,lParam);

		KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
		
		//按下空格则开图
		if( kb && kb->vkCode==VK_SPACE )
		{
			OpenMap();
			return 1;
		}

		//------ 英雄快捷键 --------------
		for(int i=0;i<KEY_COUNT;i++)
		{
			DWORD key = (DWORD)(g_keyInfo.NewKey[i]);
			if(key>='a' && key<='z') //小写转换成大写
				key -= 32;
			if( kb && kb->vkCode==key )
			{	
				BYTE bVk = (BYTE)(g_keyInfo.OldKey[i]);
				if(bVk>='a' && key<='z') //小写转换成大写
					bVk -= 32;
				if(wParam==WM_KEYDOWN)
					keybd_event( bVk ,0,0,0);
				if(wParam==WM_KEYUP)
					keybd_event(bVk,0,0x2,0);
				return 1;
			}
		}

		//物品栏
		for(int i=0;i<KEY_COUNT;i++)
		{
			DWORD key = (DWORD)(g_keyInfo.Num[i]);
			if(key>='a' && key<='z') //小写转换成大写
				key -= 32;
			if( kb && kb->vkCode==key )
			{	
				switch(i)
				{
				case 0:		//小键盘7
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM7,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM7,0,0x2,0);
					return 1;
				case 1:		//小键盘4
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM4,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM4,0,0x2,0);
					return 1;
					
				case 2:		//小键盘1
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM1,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM1,0,0x2,0);
					return 1;
				case 3:		//小键盘8
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM8,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM8,0,0x2,0);
					return 1;
				case 4:		//小键盘5
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM5,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM5,0,0x2,0);
					return 1;
				case 5:		//小键盘2
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM2,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM2,0,0x2,0);
					return 1;
				}
				
			}
		}	
	}

	return CallNextHookEx(NULL,nCode,wParam,lParam);
}



//安装说表钩子过程的函数
bool SetHook(HWND  hWnd,DWORD dwThreadID,CKeyInfo keyInfo)
{
	if(g_hwnd!=NULL)
		return false;

	g_hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL,				//底层键盘钩子 
								(HOOKPROC)KeyboardProc,				//过程函数入口
								g_hinstDll,					//获得Hook.dll的句柄
								0);						//0表示安装的钩子过程和运行在同一桌面的
														//所有过程相关联
	if(g_hKeyHook)
	{
		g_hwnd = hWnd;
		g_dwThreadID = dwThreadID;
		g_keyInfo = keyInfo;    //保留参数信息
	//	PostThreadMessage(dwThreadID,WM_NULL,0,0);
		return true;
	}

	return false;
}


BOOL UnHook(HWND hWnd)
{
//	if(hWnd!=g_hwnd || hWnd==NULL)
//		return FALSE;
	if(g_hKeyHook==NULL)
		return false;
	BOOL unHooked = UnhookWindowsHookEx(g_hKeyHook);
	if(unHooked)
	{
		g_hwnd = NULL;
		g_hKeyHook = NULL;
	}
	return unHooked;
}