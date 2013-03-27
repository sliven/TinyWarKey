#pragma once

#include"KeyInfo.h"




// CSaveConfigDlg dialog

class CSaveConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CSaveConfigDlg)

public:
	CSaveConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSaveConfigDlg();

// Dialog Data
	enum { IDD = IDD_SAVE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strConfigName;
	afx_msg void OnSave();

	void SetKeyInfo(CKeyInfo& info);
private:
	CKeyInfo m_keyInfo;

private:
	bool CreateNewFile();
};
