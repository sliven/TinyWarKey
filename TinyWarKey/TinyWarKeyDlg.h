// TinyWarKeyDlg.h : header file
//

#pragma once
#include "KeyInfo.h"
#include "afxwin.h"
#include<vector>
#include"ConfigHeader.h"

// CTinyWarKeyDlg dialog
class CTinyWarKeyDlg : public CDialog
{
// Construction
public:
	CTinyWarKeyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TINYWARKEY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	bool	m_bHooked;		//标记是否已经设定了钩子
	HWND    m_hWnd;			//魔兽窗口句柄
	CKeyInfo m_keyInfo;
	std::vector<RECORD_INFO>  m_vRecord;   //保存读取的方案信息
	int		m_nCurrentIndex;				//当前方案序号
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChange();
	afx_msg void OnOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	CString m_cOldKey1;
	CString m_cOldKey2;
	CString m_cOldKey3;
	CString m_cOldKey4;
	CString m_cOldKey5;
	CString m_cOldKey6;
	CString m_cNewKey1;
	CString m_cNewKey2;
	CString m_cNewKey3;
	CString m_cNewKey4;
	CString m_cNewKey5;
	CString m_cNewKey6;
	CString m_strCheckInfo;
	CButton m_btnOk;
	CString m_cNumNew1;
	CString m_cNumNew2;
	CString m_cNumNew3;
	CString m_cNumNew4;
	CString m_cNumNew5;
	CString m_cNumNew6;

private:
	void ChangeCtlState(bool bEnable);
	void UpDateParam(void);
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSaveConfig();
	CComboBox m_cmbCurSelect;
	bool ReadConfig(void);
	void UpdateKeyInfo(CKeyInfo& keyInfo);
	afx_msg void OnConfigChange();
	afx_msg void OnDeleteSelected();
	afx_msg void OnAbout();
};
