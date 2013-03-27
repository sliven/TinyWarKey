#ifndef _CONFIG_HEADER_
#define _CONFIG_HEADER_

//�ļ�ͷ����Ϣ
#define REC_SIZE  (256)   //ÿ����¼�ĳ���
#define NAME_LEN  (64)	  //����������󳤶�
#define  FILE_NAME  (_T("KeyConfig.dat"))  //�ļ���
#define  CURRENT_MAJOR_VER  1
#define  CURRENT_MINOR_VER  0

struct HEAD_INFO
{
	short  ver_major;
	short  ver_minor;
	wchar_t   mark[16];		//����Ƿ�����ȷ���ļ�
	unsigned int   count;			//�ܷ�������
	unsigned int   current;	//������ʹ�õķ������

	HEAD_INFO()
	{
		ver_major = 1;
		ver_minor = 0;
		wcscpy(mark,TEXT("TinyWarKey"));
		count = 0;
		current = 0;
	}
};
//ÿ�������ļ�¼��Ϣ
struct RECORD_INFO
{
	unsigned int	number;   //���
	wchar_t			name[NAME_LEN];
	CKeyInfo		keyInfo;
	RECORD_INFO():number(0),keyInfo(CKeyInfo())
	{
		name[0]='\0';
	}

	//�����͸�ֵ
	RECORD_INFO& operator=(const RECORD_INFO& info)
	{
		number = info.number;
		wcsncpy(name,info.name,NAME_LEN);
		keyInfo = info.keyInfo;
		return *this;
	}
	RECORD_INFO(const RECORD_INFO& info)
	{
		number = info.number;
		wcsncpy(name,info.name,NAME_LEN);
		keyInfo = info.keyInfo;
	}
};

bool FileExist(const wchar_t * FileName);//ȫ�ֺ������ж��ļ��Ƿ����
#endif