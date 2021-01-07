
// TargetSingleTest.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once


#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

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
// �йش����ʵ�֣������ TargetSingleTest.cpp
//

class CTargetSingleTestApp : public CWinApp
{
public:
	CTargetSingleTestApp();


//�еĲ�������
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


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTargetSingleTestApp theApp;