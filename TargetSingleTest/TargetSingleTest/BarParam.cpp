// BarParam.cpp : 实现文件
//

#include "stdafx.h"
#include "BarParam.h"
#include "afxdialogex.h"

#include "TargetSingleTest.h"

extern CTargetSingleTestApp theApp;
// CBarParam 对话框

IMPLEMENT_DYNAMIC(CBarParam, CDialogEx)

CBarParam::CBarParam(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBarParam::IDD, pParent)
	, m_Left(0),m_Bottom(0),m_Width(0),m_Height(0)
	,m_X1(0),m_X2(0),m_X3(0),m_X4(0),m_X5(0),m_X6(0),m_X7(0)
	,m_Y1(0),m_Y2(0),m_Y3(0),m_Y4(0),m_Y5(0),m_Y6(0),m_Y7(0)
	,m_Z1(0),m_Z2(0),m_Z3(0),m_Z4(0),m_Z5(0),m_Z6(0),m_Z7(0)
{

}

CBarParam::~CBarParam()
{
}

void CBarParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BARPARAM_LIST, m_List);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_LEFT, m_Left);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_BOTTOM, m_Bottom);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_WIDTH, m_Width);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_HEIGHT, m_Height);

	DDX_Text(pDX, IDC_EDIT_BARPARAM_X1, m_X1);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_X2, m_X2);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_X3, m_X3);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_X4, m_X4);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_X5, m_X5);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_X6, m_X6);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_X7, m_X7);

	DDX_Text(pDX, IDC_EDIT_BARPARAM_Y1, m_Y1);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Y2, m_Y2);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Y3, m_Y3);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Y4, m_Y4);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Y5, m_Y5);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Y6, m_Y6);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Y7, m_Y7);

	DDX_Text(pDX, IDC_EDIT_BARPARAM_Z1, m_Z1);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Z2, m_Z2);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Z3, m_Z3);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Z4, m_Z4);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Z5, m_Z5);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Z6, m_Z6);
	DDX_Text(pDX, IDC_EDIT_BARPARAM_Z7, m_Z7);

}


BEGIN_MESSAGE_MAP(CBarParam, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BARPARAM_LIST, &CBarParam::OnNMClickListBarparamList)
	ON_BN_CLICKED(IDC_BUN_BARPARAM_SET, &CBarParam::OnBnClickedBunBarparamSet)
	ON_BN_CLICKED(IDOK, &CBarParam::OnBnClickedOk)
END_MESSAGE_MAP()


// CBarParam 消息处理程序


BOOL CBarParam::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_MyImageList.Create(24,24,ILC_COLOR4, 0, 1);
	InitListCtrl();
	targetID = -1;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CBarParam::InitListCtrl()
{
	int iCount = 0;
	int i = 0;
	iCount = theApp.m_BarCount;
	m_MyImageList.DeleteImageList();
	
	//设置列表控件各项
	LVITEM item;
	CString title;
	item.mask=LVIF_TEXT|LVIF_IMAGE;
	item.iSubItem=0;        // 主项
	item.pszText=_T("");
	if(iCount>0)
	{
		char strTmp[40];
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
}

void CBarParam::OnNMClickListBarparamList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	UpdateData(true);
	CString strTargetID;
	POSITION iSelPos;
	iSelPos = m_List.GetFirstSelectedItemPosition(); 
	if(iSelPos != NULL)
	{
		strTargetID = m_List.GetItemText(UINT(iSelPos)-1,0);
		sscanf(strTargetID,_T("%d"),&targetID);
		m_Left = theApp.m_BarParamStru[targetID-1].Left;
		m_Bottom = theApp.m_BarParamStru[targetID-1].Bottom;
		m_Width = theApp.m_BarParamStru[targetID-1].Width;
		m_Height = theApp.m_BarParamStru[targetID-1].Height;

		m_X1 = theApp.m_BarParamStru[targetID-1].X1;
		m_X2 = theApp.m_BarParamStru[targetID-1].X2;
		m_X3 = theApp.m_BarParamStru[targetID-1].X3;
		m_X4 = theApp.m_BarParamStru[targetID-1].X4;
		m_X5 = theApp.m_BarParamStru[targetID-1].X5;
		m_X6 = theApp.m_BarParamStru[targetID-1].X6;
		m_X7 = theApp.m_BarParamStru[targetID-1].X7;

		m_Y1 = theApp.m_BarParamStru[targetID-1].Y1;
		m_Y2 = theApp.m_BarParamStru[targetID-1].Y2;
		m_Y3 = theApp.m_BarParamStru[targetID-1].Y3;
		m_Y4 = theApp.m_BarParamStru[targetID-1].Y4;
		m_Y5 = theApp.m_BarParamStru[targetID-1].Y5;
		m_Y6 = theApp.m_BarParamStru[targetID-1].Y6;
		m_Y7 = theApp.m_BarParamStru[targetID-1].Y7;

		m_Z1 = theApp.m_BarParamStru[targetID-1].Z1;
		m_Z2 = theApp.m_BarParamStru[targetID-1].Z2;
		m_Z3 = theApp.m_BarParamStru[targetID-1].Z3;
		m_Z4 = theApp.m_BarParamStru[targetID-1].Z4;
		m_Z5 = theApp.m_BarParamStru[targetID-1].Z5;
		m_Z6 = theApp.m_BarParamStru[targetID-1].Z6;
		m_Z7 = theApp.m_BarParamStru[targetID-1].Z7;

		UpdateData(false);	
	}
	else
	{
		targetID = -1;
	}
	*pResult = 0;
}


void CBarParam::OnBnClickedBunBarparamSet()
{
	if(targetID>0)
	{
		UpdateData(true);
		theApp.m_BarParamStru[targetID-1].Left = m_Left;
		theApp.m_BarParamStru[targetID-1].Bottom = m_Bottom;
		theApp.m_BarParamStru[targetID-1].Width = m_Width;
		theApp.m_BarParamStru[targetID-1].Height = m_Height;

		theApp.m_BarParamStru[targetID-1].X1 = m_X1;
		theApp.m_BarParamStru[targetID-1].X2 = m_X2;
		theApp.m_BarParamStru[targetID-1].X3 = m_X3;
		theApp.m_BarParamStru[targetID-1].X4 = m_X4;
		theApp.m_BarParamStru[targetID-1].X5 = m_X5;
		theApp.m_BarParamStru[targetID-1].X6 = m_X6;
		theApp.m_BarParamStru[targetID-1].X7 = m_X7;

		theApp.m_BarParamStru[targetID-1].Y1 = m_Y1;
		theApp.m_BarParamStru[targetID-1].Y2 = m_Y2;
		theApp.m_BarParamStru[targetID-1].Y3 = m_Y3;
		theApp.m_BarParamStru[targetID-1].Y4 = m_Y4;
		theApp.m_BarParamStru[targetID-1].Y5 = m_Y5;
		theApp.m_BarParamStru[targetID-1].Y6 = m_Y6;
		theApp.m_BarParamStru[targetID-1].Y7 = m_Y7;

		theApp.m_BarParamStru[targetID-1].Z1 = m_Z1;
		theApp.m_BarParamStru[targetID-1].Z2 = m_Z2;
		theApp.m_BarParamStru[targetID-1].Z3 = m_Z3;
		theApp.m_BarParamStru[targetID-1].Z4 = m_Z4;
		theApp.m_BarParamStru[targetID-1].Z5 = m_Z5;
		theApp.m_BarParamStru[targetID-1].Z6 = m_Z6;
		theApp.m_BarParamStru[targetID-1].Z7 = m_Z7;
	}
}


void CBarParam::OnBnClickedOk()
{
	int iTarID = 0;
	CString strTarID;
	int iCount = m_List.GetItemCount();
	for(int nIndex = 0; nIndex < iCount; nIndex++)
	{
		strTarID = m_List.GetItemText(nIndex,0);
		sscanf(strTarID,_T("%d"),&iTarID);
		theApp.SetIniBarParam(iTarID);
	}
	CDialogEx::OnOK();
}
