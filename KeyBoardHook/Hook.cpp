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

//���ڴ��ݼ�ֵ��Ϣ
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


HINSTANCE g_hinstDll=NULL;		//DLL���
CKeyInfo  g_keyInfo;		//������Ϣ
bool	  g_bPrivilegeImproved=false;  //�����Ȩ�Ƿ�������

//�����Ȩ��ʹ�����̾߱�debug��Ȩ
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
		if(bRes)  //��ȡluid�ɹ�
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

#define  CHAT_ADDRESS_24E  (0x6FAE8450)    //1.24E �汾����״̬���ڴ��ַ
//��ȡħ���ڴ棬�ж��Ƿ�������״̬
bool  ReadChatFlag(bool& isChatting)
{
	isChatting = false;
	if(g_hwnd==NULL)
		return false;
	DWORD pid;								//ħ�޽���ID
	GetWindowThreadProcessId(g_hwnd,&pid);  //��ȡ���̵�ID

	//�����Ȩû��������������Ȩ�ޣ������޷���ȡħ���ڴ�
	if(!g_bPrivilegeImproved)
	{
		ImprovePrivilege();
	}

	HANDLE hProcess;			//ħ�޽��̾��
	if(g_bPrivilegeImproved)  //Ȩ����߳ɹ�
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS,false,pid);  //�򿪽���
	}
	int res=0;
	BYTE  buf = 0;		//�洢��ȡ���ڴ��ַ
	if(hProcess)
	{
		res = ReadProcessMemory(hProcess,(LPVOID)CHAT_ADDRESS_24E , &buf,1,NULL);  //��ȡ�ڴ�

		if(res!=0)  //��ȡ�ɹ�
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
	DWORD pid;								//ħ�޽���ID
	GetWindowThreadProcessId(g_hwnd,&pid);  //��ȡ���̵�ID

	//�����Ȩû��������������Ȩ�ޣ������޷���ȡħ���ڴ�
	if(!g_bPrivilegeImproved)
	{
		ImprovePrivilege();
	}

	HANDLE hProcess;			//ħ�޽��̾��
	if(g_bPrivilegeImproved)  //Ȩ����߳ɹ�
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS,false,pid);  //�򿪽���
	}
	int res=0;
	int nMapType = 1;  //���ǿɼ�
	if(hProcess)
	{
//		res = ReadProcessMemory(hProcess,(LPVOID)CHAT_ADDRESS_24E , &buf,1,NULL);  //��ȡ�ڴ�
		res = WriteProcessMemory(hProcess,(LPVOID)0x09A50478,&nMapType,4,NULL);
		if(res!=0)  //д��ɹ�
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


//���ӹ���
static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//�����ж�ħ���Ƿ��ڼ���״̬�������ڼ���״̬�򲻽��иĽ�
	HWND ahwnd = GetForegroundWindow(); //��ǰ����Ĵ���
	if(ahwnd && ahwnd!=g_hwnd)
		return CallNextHookEx(NULL,nCode,wParam,lParam);

	if(nCode<0)
		CallNextHookEx(NULL,nCode,wParam,lParam);
	if(nCode==HC_ACTION)
	{
		//����Ƿ�������״̬
		bool isChat=false;
		bool bSuc = ReadChatFlag(isChat);
		if(bSuc && isChat)  //����,�����иĽ�
			return CallNextHookEx(NULL,nCode,wParam,lParam);

		KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
		
		//���¿ո���ͼ
		if( kb && kb->vkCode==VK_SPACE )
		{
			OpenMap();
			return 1;
		}

		//------ Ӣ�ۿ�ݼ� --------------
		for(int i=0;i<KEY_COUNT;i++)
		{
			DWORD key = (DWORD)(g_keyInfo.NewKey[i]);
			if(key>='a' && key<='z') //Сдת���ɴ�д
				key -= 32;
			if( kb && kb->vkCode==key )
			{	
				BYTE bVk = (BYTE)(g_keyInfo.OldKey[i]);
				if(bVk>='a' && key<='z') //Сдת���ɴ�д
					bVk -= 32;
				if(wParam==WM_KEYDOWN)
					keybd_event( bVk ,0,0,0);
				if(wParam==WM_KEYUP)
					keybd_event(bVk,0,0x2,0);
				return 1;
			}
		}

		//��Ʒ��
		for(int i=0;i<KEY_COUNT;i++)
		{
			DWORD key = (DWORD)(g_keyInfo.Num[i]);
			if(key>='a' && key<='z') //Сдת���ɴ�д
				key -= 32;
			if( kb && kb->vkCode==key )
			{	
				switch(i)
				{
				case 0:		//С����7
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM7,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM7,0,0x2,0);
					return 1;
				case 1:		//С����4
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM4,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM4,0,0x2,0);
					return 1;
					
				case 2:		//С����1
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM1,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM1,0,0x2,0);
					return 1;
				case 3:		//С����8
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM8,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM8,0,0x2,0);
					return 1;
				case 4:		//С����5
					if(wParam==WM_KEYDOWN)
						keybd_event(VK_NUM5,0,0,0);
					if(wParam==WM_KEYUP)
						keybd_event(VK_NUM5,0,0x2,0);
					return 1;
				case 5:		//С����2
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



//��װ˵���ӹ��̵ĺ���
bool SetHook(HWND  hWnd,DWORD dwThreadID,CKeyInfo keyInfo)
{
	if(g_hwnd!=NULL)
		return false;

	g_hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL,				//�ײ���̹��� 
								(HOOKPROC)KeyboardProc,				//���̺������
								g_hinstDll,					//���Hook.dll�ľ��
								0);						//0��ʾ��װ�Ĺ��ӹ��̺�������ͬһ�����
														//���й��������
	if(g_hKeyHook)
	{
		g_hwnd = hWnd;
		g_dwThreadID = dwThreadID;
		g_keyInfo = keyInfo;    //����������Ϣ
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