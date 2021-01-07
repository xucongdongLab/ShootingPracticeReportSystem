// SetSerialPort.cpp : implementation file
//

#include "stdafx.h"
#include "TargetSingleTest.h"
#include "SetSerialPort.h"

#include "TargetSingleTest.h"
extern CTargetSingleTestApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetSerialPort dialog


CSetSerialPort::CSetSerialPort(CWnd* pParent /*=NULL*/)
	: CDialog(CSetSerialPort::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetSerialPort)
	m_str1 = _T("");
	m_str2 = _T("");
	m_str3 = _T("");
	m_str4 = _T("");
	m_str5 = _T("");
	//}}AFX_DATA_INIT
}


void CSetSerialPort::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetSerialPort)
	DDX_Control(pDX, ID_SERIAL_COM1, m_conCOM1);
	DDX_CBString(pDX, ID_SERIAL_COM1, m_str1);
	DDX_CBString(pDX, ID_SERIAL_COM2, m_str2);
	DDX_CBString(pDX, ID_SERIAL_COM3, m_str3);
	DDX_CBString(pDX, ID_SERIAL_COM4, m_str4);
	DDX_CBString(pDX, ID_SERIAL_COM5, m_str5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetSerialPort, CDialog)
	//{{AFX_MSG_MAP(CSetSerialPort)
	ON_BN_CLICKED(ID_SERIAL_OFF, OnSerialOff)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSetSerialPort::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetSerialPort message handlers

BOOL CSetSerialPort::OnInitDialog() 
{
	CDialog::OnInitDialog();
	portNum = searchCom(strPort);
	InitCOM1();

		if(portNum>0)
		{
			m_str1 = strPort[0];
		}

	{
//		m_str1 = "COM6";
		m_str2 = "9600";
		m_str3 = "8";
		m_str4 = "0";
		m_str5 = "0";
	}

	UpdateData(false);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetSerialPort::OnOK() 
{
 //   UpdateData(true);
	//if((m_str1=="")||(m_str2=="")||(m_str3=="")||(m_str4=="")||(m_str5==""))
	//{
	//	AfxMessageBox(_T("请设置串口参数！"));
	//	return;
	//}

	//char mystr[10];
	//strcpy(mystr,(LPSTR)(LPCTSTR)m_str1);
	//int1 = atoi(mystr+3);
	//swscanf(m_str2,_T("%d"),&int2);
	//swscanf(m_str3,_T("%d"),&int3);
	//swscanf(m_str4,_T("%d"),&int4);
	//swscanf(m_str5,_T("%d"),&int5);

	//CDialog::OnOK();
}

void CSetSerialPort::OnSerialOff() 
{
	
}

int CSetSerialPort::searchCom(CString *strPort)
{
	
	//////////////从注册表中读取用户的串口信息/////////
	int Port_Num=0;//串口数
	HKEY hKEY;
	LPCTSTR data_Set=_T("HARDWARE\\DEVICEMAP\\SERIALCOMM"); 
	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,data_Set, 0, KEY_READ, &hKEY))	//打开注册表
	{
		AfxMessageBox(_T("注册表错误: 无法打开有关的hKEY!")); 
		return 0;
	} 
	long i;
	LPBYTE Data_Get=new BYTE [10];
	for (i=0; ;i++) //中間為空,沒有進行i值的比較
	{
		TCHAR szValueName[_MAX_PATH + 1];
		DWORD dwValueName = sizeof(szValueName);
		LPDWORD lpType = 0;
		
		DWORD cbData = 10;
		long ret = (RegEnumValue(hKEY,i,szValueName,&dwValueName,
			NULL,lpType,Data_Get,&cbData));
		if (ret)	  
			break;
		for (int j=0; j<10; j++)	
		{
			strPort[i].Format(_T("%s"),Data_Get);
			
		}
		
		Port_Num = i+1;
		
	}
	delete[] Data_Get;
	::RegCloseKey(hKEY);	//关闭注册表
	return Port_Num;
	////////////////////////////////////////////////////////
	
}
void CSetSerialPort::InitCOM1()
{
	m_conCOM1.ResetContent();
	for(int i=0;i<portNum;i++)
	{
		m_conCOM1.AddString(strPort[i]);
	}
}


void CSetSerialPort::OnBnClickedOk()
{
   UpdateData(true);
	if((m_str1=="")||(m_str2=="")||(m_str3=="")||(m_str4=="")||(m_str5==""))
	{
		AfxMessageBox(_T("请设置串口参数！"));
		return;
	}
//	char mystr[10];
	//char *p;
	//p = (LPSTR)(LPCTSTR)m_str1;
	////strcpy(mystr,(LPSTR)(LPCTSTR)m_str1);
	//int1 = atoi((p+3));
	sscanf(m_str1,_T("COM%d"),&int1);
	sscanf(m_str2,_T("%d"),&int2);
	sscanf(m_str3,_T("%d"),&int3);
	sscanf(m_str4,_T("%d"),&int4);
	sscanf(m_str5,_T("%d"),&int5);
	CDialog::OnOK();
}
