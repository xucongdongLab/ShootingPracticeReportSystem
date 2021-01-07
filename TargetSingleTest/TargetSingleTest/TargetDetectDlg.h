#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTargetDetectDlg 对话框

class CTargetDetectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTargetDetectDlg)

public:
	CTargetDetectDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTargetDetectDlg();

	void InitListCtrl();
	void RefreshListCtrl();
	CImageList m_MyImageList;
	int targetID;

// 对话框数据
	enum { IDD = IDD_TARGETDETECTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List;
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListTargetdetectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBunTargetdetectStart();
	afx_msg void OnBnClickedBunTargetdetectEnd();
//	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBunTargetdetectReset();
	afx_msg void OnBnClickedBunTargetdetectCancel();
	void SetSensorDetailText(int targetID);
	CEdit m_conTargetDetail;
	CString m_strTargetDetail;
	afx_msg void OnClose();
};
