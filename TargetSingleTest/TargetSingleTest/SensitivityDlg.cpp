// SensitivityDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TargetSingleTest.h"
#include "SensitivityDlg.h"
#include "afxdialogex.h"

#include "TargetSingleTestDlg.h"
extern UINT RecDataForSensitivityProc(LPVOID p);
extern CSensitivityDlg *g_pSensitivity;
// CSensitivityDlg 对话框

IMPLEMENT_DYNAMIC(CSensitivityDlg, CDialogEx)

CSensitivityDlg::CSensitivityDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSensitivityDlg::IDD, pParent)
	, m_strVoltage(_T(""))
{

}

CSensitivityDlg::~CSensitivityDlg()
{
}

void CSensitivityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SENSITIVITY_VOLTAGE, m_cobVoltage);
	DDX_Text(pDX, IDC_EDIT_SENSITIVITY_STRVOLTAGE, m_strVoltage);
	DDX_Control(pDX, IDC_LIST_TARGETDETECT_LIST, m_List);
	DDX_Control(pDX, IDC_EDIT_SENSITIVITY_STRVOLTAGE, m_conStrVoltage);
}


BEGIN_MESSAGE_MAP(CSensitivityDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TARGETDETECT_LIST, &CSensitivityDlg::OnNMClickListTargetdetectList)
	ON_BN_CLICKED(IDC_BUN_SENSITIVITY_SET, &CSensitivityDlg::OnBnClickedBunSensitivitySet)
	ON_BN_CLICKED(IDC_BUN_SENSITIVITY_GET, &CSensitivityDlg::OnBnClickedBunSensitivityGet)
	ON_BN_CLICKED(IDOK, &CSensitivityDlg::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSensitivityDlg 消息处理程序


BOOL CSensitivityDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_MyImageList.Create(24,24,ILC_COLOR4, 0, 1);
	InitListCtrl();
	InitCobVoltage();
	targetID = -1;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSensitivityDlg::InitListCtrl()
{
	int iCount,i;
	m_MyImageList.DeleteImageList();
	
	//设置列表控件各项
	LVITEM item;
	CString title;
	item.mask=LVIF_TEXT|LVIF_IMAGE;
	item.iSubItem=0;        // 主项
	item.pszText=_T("");

	char strTmp[40];
	iCount = theApp.m_BarCount;//((CBaServerDlg*)(theApp.m_pMainWnd))->
	CBitmap bmp;
	m_MyImageList.Create(24,24,ILC_COLOR4, 0, 1);
	bmp.LoadBitmap(IDB_SMALLREDCIRCLE);
	for(i=0; i<iCount; i++)
	{
		m_MyImageList.Add(&bmp,RGB(0,0,0));
	}
	bmp.DeleteObject();
	//处理列表控件
	m_List.DeleteAllItems();
	m_List.SetImageList(&m_MyImageList, LVSIL_SMALL);
	for(i=0; i<iCount; i++)
	{
		title = itoa(i+1,strTmp,10);
		item.iItem=i;           // 准备添加项
		item.iImage=i;// 设置关联图像列表中指定图像的索引值
		m_List.InsertItem(&item);// 添加第1项
		m_List.SetItemText(i,0,title);
	}
}

void CSensitivityDlg::OnNMClickListTargetdetectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	UpdateData(true);
	CString strTargetID;
	POSITION iSelPos;
	iSelPos = m_List.GetFirstSelectedItemPosition(); 
	if(iSelPos != NULL)
	{
		strTargetID = m_List.GetItemText(UINT(iSelPos)-1,0);
		targetID = atoi(strTargetID);
	    //swscanf(strTargetID,_T("%d"),&targetID);
		SetStrVoltage(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_Sensitivity[targetID-1].Sensitivity);
	}
	else
	{
		targetID = -1;
	}
	*pResult = 0;
}


void CSensitivityDlg::OnBnClickedBunSensitivitySet()
{
	if(targetID==-1)return;
	short actionCode1,actionCode2;
	actionCode1 = 0x1A;
	int nIndex = -1;
	nIndex = m_cobVoltage.GetCurSel();
	if(nIndex == 0) 
	{
		actionCode2 = 0x0D;
	}
	else if(nIndex == 1)
	{
		actionCode2 = 0x12;
	}
	else if(nIndex == 2)
	{
		actionCode2 = 0x17;
	}
	else
	{
		return;
	}

	if(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->IsMovBar(targetID))
	{
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataMov(targetID,0x03,actionCode1,actionCode2);
	}
	else
	{
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataZJ(targetID,0x03,actionCode1,actionCode2);
	}
}


void CSensitivityDlg::OnBnClickedBunSensitivityGet()
{
	if(targetID==-1)return;
	short actionCode1,actionCode2;
	actionCode1 = 0x1B;
	actionCode2 = 0x00;

	DWORD ThreadForSensitivityID;
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->theRecDataThreadForSensitivity = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)RecDataForSensitivityProc,theApp.m_pMainWnd,CREATE_SUSPENDED,&ThreadForSensitivityID);
	ResumeThread(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->theRecDataThreadForSensitivity);

	if(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->IsMovBar(targetID))
	{
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataMov(targetID,0x03,actionCode1,actionCode2);
	}
	else
	{
		((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->SendDataZJ(targetID,0x03,actionCode1,actionCode2);
	}
}


void CSensitivityDlg::OnBnClickedOk()
{
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecDataSensitivityFlag=false;
	g_pSensitivity=NULL;
	CDialogEx::OnOK();
}


void CSensitivityDlg::OnClose()
{
	((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecDataSensitivityFlag=false;
	g_pSensitivity=NULL;
	CDialogEx::OnClose();
}


void CSensitivityDlg::InitCobVoltage(void)
{
	m_cobVoltage.ResetContent();
	m_cobVoltage.AddString(_T("1.3V"));
	m_cobVoltage.AddString(_T("1.8V"));
	m_cobVoltage.AddString(_T("2.3V"));
}


void CSensitivityDlg::SetStrVoltage(short iVoltage)
{
	CString strMsg = _T("");
	if(iVoltage == 0x0D)
	{
		strMsg = _T("1.3V");
	}
	else if(iVoltage == 0x12)
	{
		strMsg = _T("1.8V");
	}
	else if(iVoltage == 0x17)
	{
		strMsg = _T("2.3V");
	}
	m_conStrVoltage.SetSel(0,-1,false);
	m_conStrVoltage.ReplaceSel(strMsg,true);
}
