// TinyWarKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include<atlbase.h>
#include<atlwin.h>
#include "TinyWarKey.h"
#include "TinyWarKeyDlg.h"
#include "SaveConfigDlg.h"
#include"ConfigHeader.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//导入DLL
#pragma comment(lib,"Hook.lib")
_declspec(dllimport)  bool SetHook(HWND hWnd,DWORD dwThreadID,CKeyInfo keyInfo);
_declspec(dllimport)  BOOL UnHook(HWND hWnd);

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTinyWarKeyDlg dialog




CTinyWarKeyDlg::CTinyWarKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTinyWarKeyDlg::IDD, pParent)
	, m_cOldKey1(_T(""))
	, m_cOldKey2(_T(""))
	, m_cOldKey3(_T(""))
	, m_cOldKey4(_T(""))
	, m_cOldKey5(_T(""))
	, m_cOldKey6(_T(""))
	, m_cNewKey1(_T(""))
	, m_cNewKey2(_T(""))
	, m_cNewKey3(_T(""))
	, m_cNewKey4(_T(""))
	, m_cNewKey5(_T(""))
	, m_cNewKey6(_T(""))
	, m_strCheckInfo(_T(""))
	, m_cNumNew1(_T(""))
	, m_cNumNew2(_T(""))
	, m_cNumNew3(_T(""))
	, m_cNumNew4(_T(""))
	, m_cNumNew5(_T(""))
	, m_cNumNew6(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bHooked = false;
	m_hWnd = NULL;
	m_nCurrentIndex = 0;
}

void CTinyWarKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_KEY_OLD1, m_cOldKey1);
	DDV_MaxChars(pDX, m_cOldKey1, 1);
	DDX_Text(pDX, IDC_KEY_OLD2, m_cOldKey2);
	DDV_MaxChars(pDX, m_cOldKey2, 1);
	DDX_Text(pDX, IDC_KEY_OLD3, m_cOldKey3);
	DDV_MaxChars(pDX, m_cOldKey3, 1);
	DDX_Text(pDX, IDC_KEY_OLD4, m_cOldKey4);
	DDV_MaxChars(pDX, m_cOldKey4, 1);
	DDX_Text(pDX, IDC_KEY_OLD5, m_cOldKey5);
	DDV_MaxChars(pDX, m_cOldKey5, 1);
	DDX_Text(pDX, IDC_KEY_OLD6, m_cOldKey6);
	DDV_MaxChars(pDX, m_cOldKey6, 1);
	DDX_Text(pDX, IDC_KEY_NEW1, m_cNewKey1);
	DDV_MaxChars(pDX, m_cNewKey1, 1);
	DDX_Text(pDX, IDC_KEY_NEW2, m_cNewKey2);
	DDV_MaxChars(pDX, m_cNewKey2, 1);
	DDX_Text(pDX, IDC_KEY_NEW3, m_cNewKey3);
	DDV_MaxChars(pDX, m_cNewKey3, 1);
	DDX_Text(pDX, IDC_KEY_NEW4, m_cNewKey4);
	DDV_MaxChars(pDX, m_cNewKey4, 1);
	DDX_Text(pDX, IDC_KEY_NEW5, m_cNewKey5);
	DDV_MaxChars(pDX, m_cNewKey5, 1);
	DDX_Text(pDX, IDC_KEY_NEW6, m_cNewKey6);
	DDV_MaxChars(pDX, m_cNewKey6, 1);
	DDX_Text(pDX, IDC_CHECK_INFO, m_strCheckInfo);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Text(pDX, IDC_NUM_NEW1, m_cNumNew1);
	DDV_MaxChars(pDX, m_cNumNew1, 1);
	DDX_Text(pDX, IDC_NUM_NEW2, m_cNumNew2);
	DDV_MaxChars(pDX, m_cNumNew2, 1);
	DDX_Text(pDX, IDC_NUM_NEW3, m_cNumNew3);
	DDV_MaxChars(pDX, m_cNumNew3, 1);
	DDX_Text(pDX, IDC_NUM_NEW4, m_cNumNew4);
	DDV_MaxChars(pDX, m_cNumNew4, 1);
	DDX_Text(pDX, IDC_NUM_NEW5, m_cNumNew5);
	DDV_MaxChars(pDX, m_cNumNew5, 1);
	DDX_Text(pDX, IDC_NUM_NEW6, m_cNumNew6);
	DDV_MaxChars(pDX, m_cNumNew6, 1);
	DDX_Control(pDX, IDC_CUR_CONFIG, m_cmbCurSelect);
}

BEGIN_MESSAGE_MAP(CTinyWarKeyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CTinyWarKeyDlg::OnOk)
	ON_BN_CLICKED(ID_CHANGE, &CTinyWarKeyDlg::OnChange)
	ON_BN_CLICKED(IDCANCEL, &CTinyWarKeyDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CTinyWarKeyDlg::OnSaveConfig)
	ON_CBN_SELCHANGE(IDC_CUR_CONFIG, &CTinyWarKeyDlg::OnConfigChange)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CTinyWarKeyDlg::OnDeleteSelected)
	ON_BN_CLICKED(IDC_BTN_ABOUT, &CTinyWarKeyDlg::OnAbout)
END_MESSAGE_MAP()



// CTinyWarKeyDlg message handlers

BOOL CTinyWarKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetTimer(1,1000,NULL);  //定时检测魔兽运行状态
	ReadConfig();  //读取保存的改建方案，并显示最后一次使用的方案

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTinyWarKeyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTinyWarKeyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTinyWarKeyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTinyWarKeyDlg::OnTimer(UINT_PTR nIDEvent)
{
	UpdateData();
	m_hWnd = ::FindWindow(TEXT("Warcraft III"),NULL);
	if(m_hWnd==NULL)
	{
		m_strCheckInfo = TEXT("Warcraft没有启动!");
		m_btnOk.SetWindowText(_T("确定"));
		m_btnOk.EnableWindow(TRUE);
		ChangeCtlState(true);
		UnHook(m_hWnd);
		UpdateData(FALSE);
		CDialog::OnTimer(nIDEvent);
		return;
	}
	if(m_bHooked)
	{
		m_strCheckInfo = TEXT("设定成功,正在运行!");
		UpdateData(FALSE);
		CDialog::OnTimer(nIDEvent);
		return;
	}
	else
	{
		m_strCheckInfo = TEXT("魔兽争霸正在运行，点击确定进行设定!");
		UpdateData(FALSE);
		CDialog::OnTimer(nIDEvent);
		return;
	}
	
}

void CTinyWarKeyDlg::OnDestroy()
{
	CDialog::OnDestroy();

	UnHook(m_hWnd);
}

//更新参数信息
void CTinyWarKeyDlg::UpDateParam(void)
{
	UpdateData();
	for(int i=0;i<KEY_COUNT;i++)
	{
		m_keyInfo.OldKey[i]='\0';
		m_keyInfo.NewKey[i]='\0';
		m_keyInfo.Num[i]='\0';
	}
	if(!m_cOldKey1.IsEmpty())
		m_keyInfo.OldKey[0] = m_cOldKey1[0];
	if(!m_cOldKey2.IsEmpty())
		m_keyInfo.OldKey[1] = m_cOldKey2[0];
	if(!m_cOldKey3.IsEmpty())
		m_keyInfo.OldKey[2] = m_cOldKey3[0];
	if(!m_cOldKey4.IsEmpty())
		m_keyInfo.OldKey[3] = m_cOldKey4[0];
	if(!m_cOldKey5.IsEmpty())
		m_keyInfo.OldKey[4] = m_cOldKey5[0];
	if(!m_cOldKey6.IsEmpty())
		m_keyInfo.OldKey[5] = m_cOldKey6[0];

	if(!m_cNewKey1.IsEmpty())
		m_keyInfo.NewKey[0] = m_cNewKey1[0];
	if(!m_cNewKey2.IsEmpty())
		m_keyInfo.NewKey[1] = m_cNewKey2[0];
	if(!m_cNewKey3.IsEmpty())
		m_keyInfo.NewKey[2] = m_cNewKey3[0];
	if(!m_cNewKey4.IsEmpty())
		m_keyInfo.NewKey[3] = m_cNewKey4[0];
	if(!m_cNewKey5.IsEmpty())
		m_keyInfo.NewKey[4] = m_cNewKey5[0];
	if(!m_cNewKey6.IsEmpty())
		m_keyInfo.NewKey[5] = m_cNewKey6[0];

	if(!m_cNumNew1.IsEmpty())
		m_keyInfo.Num[0] = m_cNumNew1[0];
	if(!m_cNumNew2.IsEmpty())
		m_keyInfo.Num[1] = m_cNumNew2[0];
	if(!m_cNumNew3.IsEmpty())
		m_keyInfo.Num[2] = m_cNumNew3[0];
	if(!m_cNumNew4.IsEmpty())
		m_keyInfo.Num[3] = m_cNumNew4[0];
	if(!m_cNumNew5.IsEmpty())
		m_keyInfo.Num[4] = m_cNumNew5[0];
	if(!m_cNumNew6.IsEmpty())
		m_keyInfo.Num[5] = m_cNumNew6[0];

	UpdateData(FALSE);
}

//设定
void CTinyWarKeyDlg::OnOk()
{
	if(m_hWnd==NULL)
		m_hWnd = ::FindWindow(TEXT("Warcraft III"),NULL);

	if(m_hWnd)
	{
		UpDateParam();
		CWindow myWindow;
		myWindow.Attach(m_hWnd);
		DWORD dwID = myWindow.GetWindowThreadID();
		bool res = SetHook(m_hWnd,dwID,m_keyInfo);
		if(res==false)
		{
			MessageBox(TEXT("软件设定失败！\n\n"),TEXT("提示"),MB_ICONEXCLAMATION | MB_OK);
		}
		else
		{
			m_bHooked = true;
			m_btnOk.SetWindowText(TEXT("正在运行"));
			m_btnOk.EnableWindow(FALSE);
			ChangeCtlState(false);
		}
	}
	else
	{
		MessageBox(TEXT("Warcraft III 没有运行！请运行该软件！\n\n"),TEXT("提示"),MB_ICONEXCLAMATION | MB_OK);
	}
}


void CTinyWarKeyDlg::OnChange()
{
	UnHook(m_hWnd);
	
		UpdateData();
		m_hWnd=NULL;
		m_bHooked=false;
		m_btnOk.SetWindowText(TEXT("确定"));
		m_btnOk.EnableWindow(true);
		m_strCheckInfo = TEXT("点击确定进行设定!");
		ChangeCtlState(true);
		UpdateData(FALSE);
	
}

//改变编辑框的状态
//bEnable==true  可编辑
void CTinyWarKeyDlg::ChangeCtlState(bool bEnable)
{	
	GetDlgItem(IDC_KEY_OLD1)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_OLD2)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_OLD3)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_OLD4)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_OLD5)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_OLD6)->EnableWindow(bEnable);

	GetDlgItem(IDC_KEY_NEW1)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_NEW2)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_NEW3)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_NEW4)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_NEW5)->EnableWindow(bEnable);
	GetDlgItem(IDC_KEY_NEW6)->EnableWindow(bEnable);

	GetDlgItem(IDC_NUM_NEW1)->EnableWindow(bEnable);
	GetDlgItem(IDC_NUM_NEW2)->EnableWindow(bEnable);
	GetDlgItem(IDC_NUM_NEW3)->EnableWindow(bEnable);
	GetDlgItem(IDC_NUM_NEW4)->EnableWindow(bEnable);
	GetDlgItem(IDC_NUM_NEW5)->EnableWindow(bEnable);
	GetDlgItem(IDC_NUM_NEW6)->EnableWindow(bEnable);

	m_cmbCurSelect.EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_SAVE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BTN_DELETE)->EnableWindow(bEnable);
	UpdateData(FALSE);
}

void CTinyWarKeyDlg::OnBnClickedCancel()
{
	UnHook(m_hWnd);
	
	CFile f;
	if(!f.Open(FILE_NAME,CFile::modeReadWrite))
	{
		return;	//打开文件失败
	}
	
	f.SeekToBegin();
	HEAD_INFO  headInfo;
	f.Read(&headInfo,sizeof(HEAD_INFO));
	//判断是否是我们的文件
	if(headInfo.ver_major != CURRENT_MAJOR_VER 
		|| headInfo.ver_minor != CURRENT_MINOR_VER 
		|| wcscmp(headInfo.mark,_T("TinyWarKey"))!=0)
	{
		MessageBox(_T("文件类型错误!无法读取数据\n\n"));
		f.Close();
		return ;
	}

	try
	{
		headInfo.current = m_nCurrentIndex+1;
		f.SeekToBegin();
		f.Write(&headInfo,sizeof(HEAD_INFO));
	}
	catch(CFileException e)
	{
		f.Close();
		return ;
	}
	f.Close();

	OnCancel();
}

//保存方案
void CTinyWarKeyDlg::OnSaveConfig()
{
	UpDateParam();  //更新当前配置参数
	CSaveConfigDlg  dlg;
	dlg.SetKeyInfo(m_keyInfo);
	dlg.DoModal();
	//保存之后重新读取文件，以更新信息
	ReadConfig();
}

//读取保存的改建方案，并显示最后一次使用的方案
bool CTinyWarKeyDlg::ReadConfig(void)
{
	UpdateData();
	CFile f;
	if(!f.Open(FILE_NAME,CFile::modeRead))
	{
		return false;	//打开文件失败
	}
	
	f.SeekToBegin();
	HEAD_INFO  headInfo;
	f.Read(&headInfo,sizeof(HEAD_INFO));
	//判断是否是我们的文件
	if(headInfo.ver_major != CURRENT_MAJOR_VER 
		|| headInfo.ver_minor != CURRENT_MINOR_VER 
		|| wcscmp(headInfo.mark,_T("TinyWarKey"))!=0)
	{
		MessageBox(_T("文件类型错误!无法读取数据\n\n"));
		f.Close();
		return false;
	}

	//首先清空原有信息
	m_vRecord.clear();

	//读取所有的方案信息
	RECORD_INFO  recordInfo;
	try
	{
		for(int i=1; i<=headInfo.count; i++)
		{
			f.Seek(REC_SIZE*i,CFile::begin);
			f.Read(&recordInfo,sizeof(RECORD_INFO));
			m_vRecord.push_back(recordInfo);
		}
	}
	catch(CFileException e)
	{
		f.Close();
		return false;
	}
	f.Close();

	//初始化列表框，显示当前方案
	//先清除列表框信息
	//m_cmbCurSelect.SetEditSel(0,-1);
	//m_cmbCurSelect.Clear();
	for (int i = m_cmbCurSelect.GetCount() - 1; i >= 0; i--)
	{
		m_cmbCurSelect.DeleteString(i);
	}

	UpdateData(FALSE);

	for(int i=0;i<headInfo.count;++i)
	{
		m_cmbCurSelect.AddString(m_vRecord[i].name);
	}
	m_cmbCurSelect.SetCurSel(headInfo.current-1);
	m_nCurrentIndex = headInfo.current-1;
	//更新键值
	if(headInfo.current-1 <= m_vRecord.size())
		UpdateKeyInfo(m_vRecord[headInfo.current-1].keyInfo);
	return true;
}

//更新键值
void CTinyWarKeyDlg::UpdateKeyInfo(CKeyInfo& keyInfo)
{
	m_cNewKey1 = keyInfo.NewKey[0];
	m_cNewKey2 = keyInfo.NewKey[1];
	m_cNewKey3 = keyInfo.NewKey[2];
	m_cNewKey4 = keyInfo.NewKey[3];
	m_cNewKey5 = keyInfo.NewKey[4];
	m_cNewKey6 = keyInfo.NewKey[5];

	m_cOldKey1 = keyInfo.OldKey[0];
	m_cOldKey2 = keyInfo.OldKey[1];
	m_cOldKey3 = keyInfo.OldKey[2];
	m_cOldKey4 = keyInfo.OldKey[3];
	m_cOldKey5 = keyInfo.OldKey[4];
	m_cOldKey6 = keyInfo.OldKey[5];

	m_cNumNew1 = keyInfo.Num[0];
	m_cNumNew2 = keyInfo.Num[1];
	m_cNumNew3 = keyInfo.Num[2];
	m_cNumNew4 = keyInfo.Num[3];
	m_cNumNew5 = keyInfo.Num[4];
	m_cNumNew6 = keyInfo.Num[5];
	UpdateData(FALSE);
}

//用户选择其他方案时，更新
void CTinyWarKeyDlg::OnConfigChange()
{
	UpdateData();
	const int sel = m_cmbCurSelect.GetCurSel();
	m_nCurrentIndex = sel;
	if(sel>m_vRecord.size())  //越界
		return;
	UpdateKeyInfo(m_vRecord[sel].keyInfo);
}

//删除当前选中的方案
void CTinyWarKeyDlg::OnDeleteSelected()
{
	CFile f;
	if(!f.Open(FILE_NAME,CFile::modeReadWrite))
	{
		return;	//打开文件失败
	}
	
	f.SeekToBegin();
	HEAD_INFO  headInfo;
	f.Read(&headInfo,sizeof(HEAD_INFO));
	//判断是否是我们的文件
	if(headInfo.ver_major != CURRENT_MAJOR_VER 
		|| headInfo.ver_minor != CURRENT_MINOR_VER 
		|| wcscmp(headInfo.mark,_T("TinyWarKey"))!=0)
	{
		MessageBox(_T("文件类型错误!无法读取数据\n\n"));
		f.Close();
		return ;
	}

	//读取所有的方案信息
	RECORD_INFO  recordInfo;
	try
	{
		for(int i=m_nCurrentIndex+1; i<headInfo.count; i++)
		{
			f.Seek(REC_SIZE*(i+1),CFile::begin);  //下一个记录
			f.Read(&recordInfo,sizeof(RECORD_INFO));
			recordInfo.number--;
			f.Seek(REC_SIZE*i,CFile::begin);
			f.Write(&recordInfo,sizeof(RECORD_INFO));  //覆盖前一个
		}
		headInfo.count--;
		headInfo.current = m_nCurrentIndex;
		if(headInfo.current<1)
			headInfo.current=1;
		f.SeekToBegin();
		f.Write(&headInfo,sizeof(HEAD_INFO));
	}
	catch(CFileException e)
	{
		f.Close();
		return ;
	}
	f.Close();
/*
	//更新内存信息数组
	vector<RECORD_INFO>::iterator pos = m_vRecord.begin();
	for(int i=0;i<m_nCurrentIndex && pos!=m_vRecord.end() ;i++)
		pos++;
	if(pos!=m_vRecord.end())
		m_vRecord.erase(pos);
	OnConfigChange();
	*/
	ReadConfig();
}

void CTinyWarKeyDlg::OnAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}
