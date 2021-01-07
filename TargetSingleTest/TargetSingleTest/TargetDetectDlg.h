#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CTargetDetectDlg �Ի���

class CTargetDetectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTargetDetectDlg)

public:
	CTargetDetectDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTargetDetectDlg();

	void InitListCtrl();
	void RefreshListCtrl();
	CImageList m_MyImageList;
	int targetID;

// �Ի�������
	enum { IDD = IDD_TARGETDETECTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
