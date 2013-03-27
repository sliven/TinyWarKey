#ifndef _CONFIG_HEADER_
#define _CONFIG_HEADER_

//文件头部信息
#define REC_SIZE  (256)   //每个记录的长度
#define NAME_LEN  (64)	  //方案名字最大长度
#define  FILE_NAME  (_T("KeyConfig.dat"))  //文件名
#define  CURRENT_MAJOR_VER  1
#define  CURRENT_MINOR_VER  0

struct HEAD_INFO
{
	short  ver_major;
	short  ver_minor;
	wchar_t   mark[16];		//标记是否是正确的文件
	unsigned int   count;			//总方案个数
	unsigned int   current;	//标记最后使用的方案编号

	HEAD_INFO()
	{
		ver_major = 1;
		ver_minor = 0;
		wcscpy(mark,TEXT("TinyWarKey"));
		count = 0;
		current = 0;
	}
};
//每个方案的记录信息
struct RECORD_INFO
{
	unsigned int	number;   //编号
	wchar_t			name[NAME_LEN];
	CKeyInfo		keyInfo;
	RECORD_INFO():number(0),keyInfo(CKeyInfo())
	{
		name[0]='\0';
	}

	//拷贝和赋值
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

bool FileExist(const wchar_t * FileName);//全局函数，判断文件是否存在
#endif