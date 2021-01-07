
// TargetSingleTest.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TargetSingleTest.h"
#include "TargetSingleTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CTargetSingleTestApp

BEGIN_MESSAGE_MAP(CTargetSingleTestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTargetSingleTestApp ����

CTargetSingleTestApp::CTargetSingleTestApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTargetSingleTestApp ����

CTargetSingleTestApp theApp;

// CTargetSingleTestApp ��ʼ��

BOOL CTargetSingleTestApp::InitInstance()
{


	CWinApp::InitInstance();
	GetIniBarParam();
	memset(strAlgType,0,40);

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


	CTargetSingleTestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


void CTargetSingleTestApp::GetIniBarParam()
{
	CString CurrentPath;
	CString szCurPath(_T(""));
	GetModuleFileName(NULL,szCurPath.GetBuffer(MAX_PATH),MAX_PATH);	
	szCurPath.ReleaseBuffer();
	CurrentPath = szCurPath.Left(szCurPath.ReverseFind(_T('\\')) + 1);
	CString strSettingFileName = CurrentPath+_T("BarParamSet.ini");
	m_IniBarParam.SetPathName(strSettingFileName);

	int i=0,j=0;
	CFileStatus fileStatus;
	CString strSection;
	CString strTemp;

	strSection.Format(_T("ProgSetting"));
	strTemp.Format(_T("BarCount"));
	m_BarCount = m_IniBarParam.GetInt(strSection,strTemp,10);
	strTemp.Format(_T("BarType"));
	m_BarType = m_IniBarParam.GetInt(strSection,strTemp,10);
	m_BarID = m_BarCount;

	if(CFile::GetStatus(strSettingFileName,fileStatus))
	{
		m_BarParamStru.clear();
		for (i=0; i<m_BarCount; i++)
		{
			strSection.Format(_T("BarID%d"),i+1);
			BarParamStru Tmp;

			Tmp.Left = m_IniBarParam.GetDouble(strSection,_T("Left"),-1900);
			Tmp.Bottom = m_IniBarParam.GetDouble(strSection,_T("Bottom"),260);
			Tmp.Width = m_IniBarParam.GetDouble(strSection,_T("Width"),3800);
			Tmp.Height = m_IniBarParam.GetDouble(strSection,_T("Height"),2500);

			Tmp.X1 = m_IniBarParam.GetDouble(strSection,_T("X1"),-1800);
			Tmp.X2 = m_IniBarParam.GetDouble(strSection,_T("X2"),1800);
			Tmp.X3 = m_IniBarParam.GetDouble(strSection,_T("X3"),-1800);
			Tmp.X4 = m_IniBarParam.GetDouble(strSection,_T("X4"),0);
			Tmp.X5 = m_IniBarParam.GetDouble(strSection,_T("X5"),1800);
			Tmp.X6 = m_IniBarParam.GetDouble(strSection,_T("X6"),0);
			Tmp.X7 = m_IniBarParam.GetDouble(strSection,_T("X7"),0);

			Tmp.Y1 = m_IniBarParam.GetDouble(strSection,_T("Y1"),0);
			Tmp.Y2 = m_IniBarParam.GetDouble(strSection,_T("Y2"),0);
			Tmp.Y3 = m_IniBarParam.GetDouble(strSection,_T("Y3"),0);
			Tmp.Y4 = m_IniBarParam.GetDouble(strSection,_T("Y4"),0);
			Tmp.Y5 = m_IniBarParam.GetDouble(strSection,_T("Y5"),0);
			Tmp.Y6 = m_IniBarParam.GetDouble(strSection,_T("Y6"),0);
			Tmp.Y7 = m_IniBarParam.GetDouble(strSection,_T("Y7"),0);

			Tmp.Z1 = m_IniBarParam.GetDouble(strSection,_T("Z1"),500);
			Tmp.Z2 = m_IniBarParam.GetDouble(strSection,_T("Z2"),500);
			Tmp.Z3 = m_IniBarParam.GetDouble(strSection,_T("Z3"),0);
			Tmp.Z4 = m_IniBarParam.GetDouble(strSection,_T("Z4"),0);
			Tmp.Z5 = m_IniBarParam.GetDouble(strSection,_T("Z5"),0);
			Tmp.Z6 = m_IniBarParam.GetDouble(strSection,_T("Z6"),0);
			Tmp.Z7 = m_IniBarParam.GetDouble(strSection,_T("Z7"),0);

			m_BarParamStru.push_back(Tmp);
		}
	}
}

void CTargetSingleTestApp::SetIniBarParam()
{
	CString CurrentPath;
	CString szCurPath(_T(""));
	GetModuleFileName(NULL,szCurPath.GetBuffer(MAX_PATH),MAX_PATH);	
	szCurPath.ReleaseBuffer();
	CurrentPath = szCurPath.Left(szCurPath.ReverseFind(_T('\\')) + 1);

	CString strSettingFileName = CurrentPath+_T("BarParamSet.ini");
	m_IniBarParam.SetPathName(strSettingFileName);

	CString strSection;
	int i=0,j=0;
	CFileStatus fileStatus;
	CString strTemp;
	if(CFile::GetStatus(strSettingFileName,fileStatus))
	{
		int iCount = m_BarParamStru.size();
		if(iCount>m_BarCount)iCount=m_BarCount;
		for( i=0; i<iCount; i++)
		{
			strSection.Format(_T("BarID%d"),i+1);

			m_IniBarParam.WriteDouble(strSection,_T("Left"),m_BarParamStru[i].Left);
			m_IniBarParam.WriteDouble(strSection,_T("Bottom"),m_BarParamStru[i].Bottom);
			m_IniBarParam.WriteDouble(strSection,_T("Width"),m_BarParamStru[i].Width);
			m_IniBarParam.WriteDouble(strSection,_T("Height"),m_BarParamStru[i].Height);

			m_IniBarParam.WriteDouble(strSection,_T("X1"),m_BarParamStru[i].X1);
			m_IniBarParam.WriteDouble(strSection,_T("X2"),m_BarParamStru[i].X2);
			m_IniBarParam.WriteDouble(strSection,_T("X3"),m_BarParamStru[i].X3);
			m_IniBarParam.WriteDouble(strSection,_T("X4"),m_BarParamStru[i].X4);
			m_IniBarParam.WriteDouble(strSection,_T("X5"),m_BarParamStru[i].X5);
			m_IniBarParam.WriteDouble(strSection,_T("X6"),m_BarParamStru[i].X6);
			m_IniBarParam.WriteDouble(strSection,_T("X7"),m_BarParamStru[i].X7);

			m_IniBarParam.WriteDouble(strSection,_T("Y1"),m_BarParamStru[i].Y1);
			m_IniBarParam.WriteDouble(strSection,_T("Y2"),m_BarParamStru[i].Y2);
			m_IniBarParam.WriteDouble(strSection,_T("Y3"),m_BarParamStru[i].Y3);
			m_IniBarParam.WriteDouble(strSection,_T("Y4"),m_BarParamStru[i].Y4);
			m_IniBarParam.WriteDouble(strSection,_T("Y5"),m_BarParamStru[i].Y5);
			m_IniBarParam.WriteDouble(strSection,_T("Y6"),m_BarParamStru[i].Y6);
			m_IniBarParam.WriteDouble(strSection,_T("Y7"),m_BarParamStru[i].Y7);

			m_IniBarParam.WriteDouble(strSection,_T("Z1"),m_BarParamStru[i].Z1);
			m_IniBarParam.WriteDouble(strSection,_T("Z2"),m_BarParamStru[i].Z2);
			m_IniBarParam.WriteDouble(strSection,_T("Z3"),m_BarParamStru[i].Z3);
			m_IniBarParam.WriteDouble(strSection,_T("Z4"),m_BarParamStru[i].Z4);
			m_IniBarParam.WriteDouble(strSection,_T("Z5"),m_BarParamStru[i].Z5);
			m_IniBarParam.WriteDouble(strSection,_T("Z6"),m_BarParamStru[i].Z6);
			m_IniBarParam.WriteDouble(strSection,_T("Z7"),m_BarParamStru[i].Z7);

		}
	}
}

void CTargetSingleTestApp::SetIniBarParam(int TaretID)
{
	CString CurrentPath;
	CString szCurPath(_T(""));
	GetModuleFileName(NULL,szCurPath.GetBuffer(MAX_PATH),MAX_PATH);	
	szCurPath.ReleaseBuffer();
	CurrentPath = szCurPath.Left(szCurPath.ReverseFind(_T('\\')) + 1);

	CString strSettingFileName = CurrentPath+_T("BarParamSet.ini");
	m_IniBarParam.SetPathName(strSettingFileName);

	CString strSection;
	int i=0;
	CFileStatus fileStatus;
	CString strTemp;
	if(CFile::GetStatus(strSettingFileName,fileStatus))
	{
		int iCount = m_BarParamStru.size();
		if((TaretID>iCount)||(TaretID<=0))return;
		i = TaretID-1;
		strSection.Format(_T("BarID%d"),i+1);

		m_IniBarParam.WriteDouble(strSection,_T("Left"),m_BarParamStru[i].Left);
		m_IniBarParam.WriteDouble(strSection,_T("Bottom"),m_BarParamStru[i].Bottom);
		m_IniBarParam.WriteDouble(strSection,_T("Width"),m_BarParamStru[i].Width);
		m_IniBarParam.WriteDouble(strSection,_T("Height"),m_BarParamStru[i].Height);

		m_IniBarParam.WriteDouble(strSection,_T("X1"),m_BarParamStru[i].X1);
		m_IniBarParam.WriteDouble(strSection,_T("X2"),m_BarParamStru[i].X2);
		m_IniBarParam.WriteDouble(strSection,_T("X3"),m_BarParamStru[i].X3);
		m_IniBarParam.WriteDouble(strSection,_T("X4"),m_BarParamStru[i].X4);
		m_IniBarParam.WriteDouble(strSection,_T("X5"),m_BarParamStru[i].X5);
		m_IniBarParam.WriteDouble(strSection,_T("X6"),m_BarParamStru[i].X6);
		m_IniBarParam.WriteDouble(strSection,_T("X7"),m_BarParamStru[i].X7);

		m_IniBarParam.WriteDouble(strSection,_T("Y1"),m_BarParamStru[i].Y1);
		m_IniBarParam.WriteDouble(strSection,_T("Y2"),m_BarParamStru[i].Y2);
		m_IniBarParam.WriteDouble(strSection,_T("Y3"),m_BarParamStru[i].Y3);
		m_IniBarParam.WriteDouble(strSection,_T("Y4"),m_BarParamStru[i].Y4);
		m_IniBarParam.WriteDouble(strSection,_T("Y5"),m_BarParamStru[i].Y5);
		m_IniBarParam.WriteDouble(strSection,_T("Y6"),m_BarParamStru[i].Y6);
		m_IniBarParam.WriteDouble(strSection,_T("Y7"),m_BarParamStru[i].Y7);

		m_IniBarParam.WriteDouble(strSection,_T("Z1"),m_BarParamStru[i].Z1);
		m_IniBarParam.WriteDouble(strSection,_T("Z2"),m_BarParamStru[i].Z2);
		m_IniBarParam.WriteDouble(strSection,_T("Z3"),m_BarParamStru[i].Z3);
		m_IniBarParam.WriteDouble(strSection,_T("Z4"),m_BarParamStru[i].Z4);
		m_IniBarParam.WriteDouble(strSection,_T("Z5"),m_BarParamStru[i].Z5);
		m_IniBarParam.WriteDouble(strSection,_T("Z6"),m_BarParamStru[i].Z6);
		m_IniBarParam.WriteDouble(strSection,_T("Z7"),m_BarParamStru[i].Z7);

	}
}



CString CTargetSingleTestApp::char_to_CStr( char* pchar )
{
	CString cstr(pchar);

	return cstr;
}

char* CTargetSingleTestApp::CStr_to_char( CString cstr )
{
	////CString cstr = _T("test")
	//	//������ʶ
	//	USES_CONVERSION;
	////����T2A��W2A��֧��ATL��MFC�е��ַ�
	//char * pFileName = T2A(cstr);  
	//return pFileName;
	return "";
}