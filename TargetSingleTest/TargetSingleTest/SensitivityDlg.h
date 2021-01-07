#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CSensitivityDlg �Ի���

class CSensitivityDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSensitivityDlg)

public:
	CSensitivityDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSensitivityDlg();

	void InitListCtrl();
//	void RefreshListCtrl();
	CImageList m_MyImageList;
	int targetID;

// �Ի�������
	enum { IDD = IDD_SENSITIVITYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
