// SaveConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TinyWarKey.h"
#include "SaveConfigDlg.h"
#include <sys/stat.h>
#include<string>
#include"ConfigHeader.h"
using namespace std;

	

//检查文件是否存在
bool FileExist(const wchar_t * FileName)
{
	return   (INVALID_FILE_ATTRIBUTES   !=   GetFileAttributes(FileName)); 
}



// CSaveConfigDlg dialog

IMPLEMENT_DYNAMIC(CSaveConfigDlg, CDialog)

CSaveConfigDlg::CSaveConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveConfigDlg::IDD, pParent)
	, m_strConfigName(_T(""))
{

}

CSaveConfigDlg::~CSaveConfigDlg()
{
}

void CSaveConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CONFIG_NAME, m_strConfigName);
}


BEGIN_MESSAGE_MAP(CSaveConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSaveConfigDlg::OnSave)
END_MESSAGE_MAP()


// CSaveConfigDlg message handlers

void CSaveConfigDlg::OnSave()
{
	UpdateData();
	if(m_strConfigName.IsEmpty())
	{
		MessageBox(_T("请输入方案名称！"),_T("错误"),MB_OK);
		return;
	}
	if(! FileExist(FILE_NAME) )  //文件不存在，创建新的文件
	{
		if(!CreateNewFile())
		{
			MessageBox(_T("创建新配置文件失败!无法保存!\n\n"));
			return ;
		}
	}

	//保存信息
	CFile f;
	if(! f.Open(FILE_NAME,CFile::modeReadWrite))
	{
		MessageBox(_T("打开配置文件失败!无法保存!\n\n"));
		return ;
	}

	try
	{
		f.SeekToBegin();
		//首先读取配置头部信息
		HEAD_INFO head;
		f.Read(&head, sizeof(HEAD_INFO));
		if(head.ver_major != CURRENT_MAJOR_VER
			|| head.ver_minor!=CURRENT_MINOR_VER
			|| wcscmp(head.mark,_T("TinyWarKey"))!=0 )
		{
			MessageBox(_T("文件类型错误!无法保存!\n\n"));
			f.Close();
			return ;
		}
		unsigned count = head.count;
		unsigned current= head.current;
		//读取所有的方案名称
		wchar_t	wcCurName[NAME_LEN];
		int i=0;
		for(i=0;i<m_strConfigName.GetLength();i++)
		{
			wcCurName[i] = (wchar_t)m_strConfigName[i];
		}
		wcCurName[i]='\0';
		bool bFind = false;
		int  index=0;
		for(i=1;i<=count;i++)
		{
			RECORD_INFO info;
			f.Seek(REC_SIZE*i,CFile::begin);  //定位
			f.Read(&info,sizeof(RECORD_INFO));
			if(wcscmp(info.name,wcCurName)==0)  //找到名字相同的
			{
				bFind = true;
				index = i;
				break;
			}
		}

		BYTE buf[REC_SIZE];
		memset(buf,REC_SIZE,'\0');

		if(bFind) //找到同名的，覆盖
		{
			//新纪录信息
			RECORD_INFO NewRecordInfo;
			NewRecordInfo.keyInfo = m_keyInfo;
			NewRecordInfo.number = index;
			wcscpy(NewRecordInfo.name,wcCurName);
			
			f.Seek(REC_SIZE*index,CFile::begin);
			f.Write(&NewRecordInfo,sizeof(RECORD_INFO));
			f.Write(buf,REC_SIZE-sizeof(RECORD_INFO));

			//更新头部信息
			head.current = index;
			f.SeekToBegin();
			f.Write(&head,sizeof(HEAD_INFO));
			f.Write(buf,REC_SIZE-sizeof(HEAD_INFO));
		}
		else  //没有找到则添加
		{
			//新纪录信息
			RECORD_INFO NewRecordInfo;
			NewRecordInfo.keyInfo = m_keyInfo;
			NewRecordInfo.number = count+1;     //增添新的记录
			wcscpy(NewRecordInfo.name,wcCurName);
			f.Seek(REC_SIZE*(count+1),CFile::begin);
			f.Write(&NewRecordInfo,sizeof(RECORD_INFO));
			f.Write(buf,REC_SIZE-sizeof(RECORD_INFO));

			//更新头部信息
			head.current = count+1;
			head.count++;
			f.SeekToBegin();
			f.Write(&head,sizeof(HEAD_INFO));
			f.Write(buf,REC_SIZE-sizeof(HEAD_INFO));
		}

	}
	catch(CFileException e)
	{
		f.Close();
		MessageBox(_T("写入文件发生异常，保存失败！"));
	}
	f.Close();

	MessageBox(_T("方案保存成功!\t\t\t\n\n"),_T("保存成功"),MB_OK);
	OnOK();
}
//配置文件不存在，则创建新的文件
bool CSaveConfigDlg::CreateNewFile()
{
	CFile f;
	CFileException e;
	if(!f.Open(FILE_NAME, CFile::modeCreate | CFile::modeReadWrite, &e))
	{
	   //MessageBox(_T("配置文件丢失，在创建配置文件时失败\n\n"));
	   return false;;
	}
	try
	{
		HEAD_INFO head;
		BYTE buf[REC_SIZE];
		memset(buf,'\0',REC_SIZE);
	
		//头部信息
		f.SeekToBegin();
		f.Write(&head,sizeof(HEAD_INFO));
		f.Write(buf,REC_SIZE-sizeof(HEAD_INFO));
	}
	catch(CFileException e)
	{
		f.Close();
		return false;
	}
	f.Close();
	return true;
}

//由主窗口传递参数信息
void CSaveConfigDlg::SetKeyInfo(CKeyInfo& info)
{
	m_keyInfo = info;
}
