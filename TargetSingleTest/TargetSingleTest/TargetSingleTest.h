
// TargetSingleTest.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once


#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号

#include "SerialPort/serialport.h"
#include "Ini/Ini.h"

#include <vector>
#include <string>
using std::string;
using std::vector;

typedef struct _BarParamStru
{
//	int type;
	double Left;
	double Bottom;
	double Width;
	double Height;

	double X1,X2,X3,X4,X5,X6,X7;
	double Y1,Y2,Y3,Y4,Y5,Y6,Y7;
	double Z1,Z2,Z3,Z4,Z5,Z6,Z7;
}BarParamStru;


// CTargetSingleTestApp:
// 有关此类的实现，请参阅 TargetSingleTest.cpp
//

class CTargetSingleTestApp : public CWinApp
{
public:
	CTargetSingleTestApp();


//靶的参数设置
	int m_BarCount;
	int m_BarType;
	int m_BarID;
	CIni m_IniBarParam;
	vector<BarParamStru> m_BarParamStru;
	void GetIniBarParam();
	void SetIniBarParam();
    void SetIniBarParam(int TaretID);
	CString char_to_CStr(char* pchar);
	char* CStr_to_char(CString cstr);

	CSerialPort m_MsComm;
	char strAlgType[40];


// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CTargetSingleTestApp theApp;