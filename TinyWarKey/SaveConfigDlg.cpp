// SaveConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TinyWarKey.h"
#include "SaveConfigDlg.h"
#include <sys/stat.h>
#include<string>
#include"ConfigHeader.h"
using namespace std;

	

//����ļ��Ƿ����
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
		MessageBox(_T("�����뷽�����ƣ�"),_T("����"),MB_OK);
		return;
	}
	if(! FileExist(FILE_NAME) )  //�ļ������ڣ������µ��ļ�
	{
		if(!CreateNewFile())
		{
			MessageBox(_T("�����������ļ�ʧ��!�޷�����!\n\n"));
			return ;
		}
	}

	//������Ϣ
	CFile f;
	if(! f.Open(FILE_NAME,CFile::modeReadWrite))
	{
		MessageBox(_T("�������ļ�ʧ��!�޷�����!\n\n"));
		return ;
	}

	try
	{
		f.SeekToBegin();
		//���ȶ�ȡ����ͷ����Ϣ
		HEAD_INFO head;
		f.Read(&head, sizeof(HEAD_INFO));
		if(head.ver_major != CURRENT_MAJOR_VER
			|| head.ver_minor!=CURRENT_MINOR_VER
			|| wcscmp(head.mark,_T("TinyWarKey"))!=0 )
		{
			MessageBox(_T("�ļ����ʹ���!�޷�����!\n\n"));
			f.Close();
			return ;
		}
		unsigned count = head.count;
		unsigned current= head.current;
		//��ȡ���еķ�������
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
			f.Seek(REC_SIZE*i,CFile::begin);  //��λ
			f.Read(&info,sizeof(RECORD_INFO));
			if(wcscmp(info.name,wcCurName)==0)  //�ҵ�������ͬ��
			{
				bFind = true;
				index = i;
				break;
			}
		}

		BYTE buf[REC_SIZE];
		memset(buf,REC_SIZE,'\0');

		if(bFind) //�ҵ�ͬ���ģ�����
		{
			//�¼�¼��Ϣ
			RECORD_INFO NewRecordInfo;
			NewRecordInfo.keyInfo = m_keyInfo;
			NewRecordInfo.number = index;
			wcscpy(NewRecordInfo.name,wcCurName);
			
			f.Seek(REC_SIZE*index,CFile::begin);
			f.Write(&NewRecordInfo,sizeof(RECORD_INFO));
			f.Write(buf,REC_SIZE-sizeof(RECORD_INFO));

			//����ͷ����Ϣ
			head.current = index;
			f.SeekToBegin();
			f.Write(&head,sizeof(HEAD_INFO));
			f.Write(buf,REC_SIZE-sizeof(HEAD_INFO));
		}
		else  //û���ҵ������
		{
			//�¼�¼��Ϣ
			RECORD_INFO NewRecordInfo;
			NewRecordInfo.keyInfo = m_keyInfo;
			NewRecordInfo.number = count+1;     //�����µļ�¼
			wcscpy(NewRecordInfo.name,wcCurName);
			f.Seek(REC_SIZE*(count+1),CFile::begin);
			f.Write(&NewRecordInfo,sizeof(RECORD_INFO));
			f.Write(buf,REC_SIZE-sizeof(RECORD_INFO));

			//����ͷ����Ϣ
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
		MessageBox(_T("д���ļ������쳣������ʧ�ܣ�"));
	}
	f.Close();

	MessageBox(_T("��������ɹ�!\t\t\t\n\n"),_T("����ɹ�"),MB_OK);
	OnOK();
}
//�����ļ������ڣ��򴴽��µ��ļ�
bool CSaveConfigDlg::CreateNewFile()
{
	CFile f;
	CFileException e;
	if(!f.Open(FILE_NAME, CFile::modeCreate | CFile::modeReadWrite, &e))
	{
	   //MessageBox(_T("�����ļ���ʧ���ڴ��������ļ�ʱʧ��\n\n"));
	   return false;;
	}
	try
	{
		HEAD_INFO head;
		BYTE buf[REC_SIZE];
		memset(buf,'\0',REC_SIZE);
	
		//ͷ����Ϣ
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

//�������ڴ��ݲ�����Ϣ
void CSaveConfigDlg::SetKeyInfo(CKeyInfo& info)
{
	m_keyInfo = info;
}
