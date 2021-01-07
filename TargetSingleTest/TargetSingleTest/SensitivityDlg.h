#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSensitivityDlg 对话框

class CSensitivityDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSensitivityDlg)

public:
	CSensitivityDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSensitivityDlg();

	void InitListCtrl();
//	void RefreshListCtrl();
	CImageList m_MyImageList;
	int targetID;

// 对话框数据
	enum { IDD = IDD_SENSITIVITYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cobVoltage;
	CString m_strVoltage;
	virtual BOOL OnInitDialog();
	CListCtrl m_List;
	afx_msg void OnNMClickListTargetdetectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBunSensitivitySet();
	afx_msg void OnBnClickedBunSensitivityGet();
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	void InitCobVoltage(void);
	void SetStrVoltage(short iVoltage);
	CEdit m_conStrVoltage;
};
