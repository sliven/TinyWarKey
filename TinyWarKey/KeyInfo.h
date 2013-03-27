
//KeyInfo.h

#ifndef _KEY_INFO_
#define _KEY_INFO_
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

	CKeyInfo(const CKeyInfo& info)
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



#endif