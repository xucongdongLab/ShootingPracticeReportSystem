// TargetDetectDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TargetSingleTest.h"
#include "TargetDetectDlg.h"
#include "afxdialogex.h"

#include "TargetSingleTestDlg.h"
extern UINT RecDataForSensorDetectProc(LPVOID p);
extern CTargetDetectDlg *g_pTargetDetect;
//#include <string>
//using namespace std;
// CTargetDetectDlg �Ի���

IMPLEMENT_DYNAMIC(CTargetDetectDlg, CDialogEx)

CTargetDetectDlg::CTargetDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTargetDetectDlg::IDD, pParent)
	, m_strTargetDetail(_T(""))
{

}

CTargetDetectDlg::~CTargetDetectDlg()
{
}

void CTargetDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TARGETDETECT_LIST, m_List);
	DDX_Control(pDX, IDC_EDIT_BUN_TARGETDETECT_DETAIL, m_conTargetDetail);
	DDX_Text(pDX, IDC_EDIT_BUN_TARGETDETECT_DETAIL, m_strTargetDetail);
}


BEGIN_MESSAGE_MAP(CTargetDetectDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TARGETDETECT_LIST, &CTargetDetectDlg::OnClickListTargetdetectList)
	ON_BN_CLICKED(IDC_BUN_TARGETDETECT_START, &CTargetDetectDlg::OnBnClickedBunTargetdetectStart)
	ON_BN_CLICKED(IDC_BUN_TARGETDETECT_END, &CTargetDetectDlg::OnBnClickedBunTargetdetectEnd)
	ON_BN_CLICKED(IDC_BUN_TARGETDETECT_RESET, &CTargetDetectDlg::OnBnClickedBunTargetdetectReset)
	ON_BN_CLICKED(IDC_BUN_TARGETDETECT_CANCEL, &CTargetDetectDlg::OnBnClickedBunTargetdetectCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTargetDetectDlg ��Ϣ�������


BOOL CTargetDetectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_MyImageList.Create(24,24,ILC_COLOR4, 0, 1);
	InitListCtrl();
	targetID = -1;
	GetDlgItem(IDC_BUN_TARGETDETECT_END)->EnableWindow(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTargetDetectDlg::InitListCtrl()
{
	int iCount,i;
	//����ͼ���б�
//		m_MyImageList.Create(24,24,ILC_COLOR4, 0, iCount);
	//	iCount=m_MyImageList.GetImageCount();
//	for(i=0;i<iCount;i++)
//	{
//		m_MyImageList.Remove(0);
//	}
	m_MyImageList.DeleteImageList();
	
	//�����б�ؼ�����
	LVITEM item;
	CString title;
	item.mask=LVIF_TEXT|LVIF_IMAGE;
	item.iSubItem=0;        // ����
	item.pszText=_T("");

	char strTmp[40];
	iCount = theApp.m_BarCount;//((CBaServerDlg*)(theApp.m_pMainWnd))->
	CBitmap bmp;
//		bmp.LoadBitmap(IDB_SMALLREDCIRCLE);
	m_MyImageList.Create(24,24,ILC_COLOR4, 0, 1);
	bmp.LoadBitmap(IDB_SMALLREDCIRCLE);
	for(i=0; i<iCount; i++)
	{
		m_MyImageList.Add(&bmp,RGB(0,0,0));
	}
	bmp.DeleteObject();
	//�����б�ؼ�
	m_List.DeleteAllItems();
	m_List.SetImageList(&m_MyImageList, LVSIL_SMALL);
//		m_List.SetImageList(&m_MyImageList, LVSIL_NORMAL);
	for(i=0; i<iCount; i++)
	{
		title = itoa(i+1,strTmp,10);
		item.iItem=i;           // ׼�������
		item.iImage=i;// ���ù���ͼ���б���ָ��ͼ�������ֵ
		m_List.InsertItem(&item);// ��ӵ�1��
		m_List.SetItemText(i,0,title);
	}
}

void CTargetDetectDlg::RefreshListCtrl()
{
	int iCount;
	m_MyImageList.DeleteImageList();
	
	//�����б�ؼ�����
	LVITEM item;
	CString title;
	item.mask=LVIF_TEXT|LVIF_IMAGE;
	item.iSubItem=0;        // ����
	item.pszText=_T("");

	char strTmp[40];
	iCount = ((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_SensorStatus.size();
	m_MyImageList.Create(24,24,ILC_COLOR4, 0, 1);
	CBitmap RedBmp;
	RedBmp.LoadBitmap(IDB_SMALLREDCIRCLE);
	CBitmap GreenBmp;
	GreenBmp.LoadBitmap(IDB_SMALLGREENCIRCLE);
	CBitmap YellowBmp;
	YellowBmp.LoadBitmap(IDB_SMALLYELLOWCIRCLE);
	for(auto it = ((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_SensorStatus.begin();it!=((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_SensorStatus.end();it++)
	{
		if((it->sensorID[0]==1)&&(it->sensorID[1]==1)&&(it->sensorID[2]==1)&&(it->sensorID[3]==1)&&(it->sensorID[4]==1)&&(it->sensorID[5]==1)&&(it->sensorID[6]==1))//&&(it->commStatus==1)
		{
			m_MyImageList.Add(&GreenBmp,RGB(0,0,0));
		}
		else if((it->sensorID[0]==0)&&(it->sensorID[1]==0)&&(it->sensorID[2]==0)&&(it->sensorID[3]==0)&&(it->sensorID[4]==0)&&(it->sensorID[5]==0)&&(it->sensorID[6]==0))//&&(it->commStatus==0)
		{
			m_MyImageList.Add(&RedBmp,RGB(0,0,0));
		}
		else
		{
			m_MyImageList.Add(&YellowBmp,RGB(0,0,0));
		}
	}
	RedBmp.DeleteObject();
	GreenBmp.DeleteObject();
	YellowBmp.DeleteObject();
	//�����б�ؼ�
	m_List.DeleteAllItems();
	m_List.SetImageList(&m_MyImageList, LVSIL_SMALL);
	
	for(auto it = ((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_SensorStatus.begin();it!=((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_SensorStatus.end();it++)
	{
		title = itoa(it->barID,strTmp,10);
		item.iItem=it->barID-1;           // ׼�������
		item.iImage=it->barID-1;// ���ù���ͼ���б���ָ��ͼ�������ֵ
		m_List.InsertItem(&item);// ��ӵ�1��
		m_List.SetItemText(it->barID-1,0,title);
	}
	SetSensorDetailText(targetID);
}


void CTargetDetectDlg::OnClickListTargetdetectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(true);
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	CString strTargetID;
	POSITION iSelPos;
	iSelPos = m_List.GetFirstSelectedItemPosition(); 
	if(iSelPos != NULL)
	{
		strTargetID = m_List.GetItemText(UINT(iSelPos)-1,0);
		targetID = atoi(strTargetID);
		//swscanf(strTargetID,_T("%d"),&targetID);
	}
	else
	{
		targetID = -1;
	}
	SetSensorDetailText(targetID);
//	UpdateData(false);	
	*pResult = 0;
}

void CTargetDetectDlg::OnBnClickedBunTargetdetectStart()
{
	if(targetID==-1)return;

	DWORD ThreadForSensorDetectID;
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->theRecDataThreadForSensorDetect = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RecDataForSensorDetectProc,theApp.m_pMainWnd,CREATE_SUSPENDED,&ThreadForSensorDetectID);
	ResumeThread(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->theRecDataThreadForSensorDetect);

	if(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->IsMovBar(targetID))
	{
	//	((CBaServerDlg*)(theApp.m_pMainWnd))->SendDataMov(targetID,0x00,0x05);
	//	Sleep(200);
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataMov(targetID,0x03,0x18);
	}
	else
	{
	//	((CBaServerDlg*)(theApp.m_pMainWnd))->SendDataZJ(targetID,0x00,0x05);
	//	Sleep(200);
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataZJ(targetID,0x03,0x18);
	}
	GetDlgItem(IDC_BUN_TARGETDETECT_END)->EnableWindow(true);
	GetDlgItem(IDC_BUN_TARGETDETECT_START)->EnableWindow(false);
	GetDlgItem(IDC_BUN_TARGETDETECT_RESET)->EnableWindow(false);
	GetDlgItem(IDC_BUN_TARGETDETECT_CANCEL)->EnableWindow(false);
}


void CTargetDetectDlg::OnBnClickedBunTargetdetectEnd()
{
	if(targetID==-1)return;

	if(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->IsMovBar(targetID))
	{
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataMov(targetID,0x03,0x19);
	}
	else
	{
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataZJ(targetID,0x03,0x19);
	}
	GetDlgItem(IDC_BUN_TARGETDETECT_END)->EnableWindow(false);
	GetDlgItem(IDC_BUN_TARGETDETECT_START)->EnableWindow(true);
	GetDlgItem(IDC_BUN_TARGETDETECT_RESET)->EnableWindow(true);
	GetDlgItem(IDC_BUN_TARGETDETECT_CANCEL)->EnableWindow(true);

	//ֹͣ�߳�
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecDataSensorDetectFlag=false;
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecCount=0;

}

void CTargetDetectDlg::OnBnClickedBunTargetdetectReset()
{
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecDataSensorDetectFlag=false;
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecCount=0;
	Sleep(400);
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->InitSensorStatus();

	targetID=-1;
	RefreshListCtrl();
}


void CTargetDetectDlg::SetSensorDetailText(int targetID)
{
	CString strMsg,strTmp,strTmp1,strTmp2,strTmp3,strTmp4,strTmp5,strTmp6,strTmp7;
	strMsg="";
	int iCount=0;
	for(auto it = ((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_SensorStatus.begin();it!=((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_SensorStatus.end();it++)
	{
		if(it->barID==targetID)
		{
			if(it->sensorID[0]==1) {strTmp1.Format(_T("  1�Ŵ���������"));iCount++;}
			else strTmp1.Format(_T("  1�Ŵ������쳣"));
			if(it->sensorID[1]==1) {strTmp2.Format(_T("  2�Ŵ���������"));iCount++;}
			else strTmp2.Format(_T("  2�Ŵ������쳣"));
			if(it->sensorID[2]==1) {strTmp3.Format(_T("  3�Ŵ���������"));iCount++;}
			else strTmp3.Format(_T("  3�Ŵ������쳣"));
			if(it->sensorID[3]==1) {strTmp4.Format(_T("  4�Ŵ���������"));iCount++;}
			else strTmp4.Format(_T("  4�Ŵ������쳣"));
			if(it->sensorID[4]==1) {strTmp5.Format(_T("  5�Ŵ���������"));iCount++;}
			else strTmp5.Format(_T("  5�Ŵ������쳣"));
			if(it->sensorID[5]==1) {strTmp6.Format(_T("  6�Ŵ���������"));iCount++;}
			else strTmp6.Format(_T("  6�Ŵ������쳣"));
			if(it->sensorID[6]==1) {strTmp7.Format(_T("  7�Ŵ���������"));iCount++;}
			else strTmp7.Format(_T("  7�Ŵ������쳣"));
			if((iCount==0))//&&(it->commStatus==0)
			{
				strTmp.Format(_T("  %d�Ű�ͨ���쳣"),targetID);
			}
			else
			{
				strTmp.Format(_T("  %d�Ű�ͨ������"),targetID);
			}
			strMsg.Format(_T("\r\n%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s\r\n\r\n%s"),strTmp,strTmp1,strTmp2,strTmp3,strTmp4,strTmp5,strTmp6,strTmp7);
		}
	}

	m_conTargetDetail.SetSel(0,-1,false);
	m_conTargetDetail.ReplaceSel(strMsg,true);

}

void CTargetDetectDlg::OnBnClickedBunTargetdetectCancel()
{
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecDataSensorDetectFlag=false;
	g_pTargetDetect=NULL;
	CDialogEx::OnCancel();
}

void CTargetDetectDlg::OnClose()
{
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecDataSensorDetectFlag=false;
	g_pTargetDetect=NULL;
	CDialogEx::OnClose();
}
