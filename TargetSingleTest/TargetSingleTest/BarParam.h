#pragma once
#include "stdafx.h"
// CBarParam �Ի���

#include "resource.h"		// ������

class CBarParam : public CDialogEx
{
	DECLARE_DYNAMIC(CBarParam)

public:
	CBarParam(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBarParam();

	void InitListCtrl();
	CImageList m_MyImageList;
	int targetID;

// �Ի�������
	enum { IDD = IDD_DLG_BARPARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_List;
	double m_Left;
	double m_Bottom;
	double m_Width;
	double m_Height;

	double m_X1;
	double m_X2;
	double m_X3;
	double m_X4;
	double m_X5;
	double m_X6;
	double m_X7;

	double m_Y1;
	double m_Y2;
	double m_Y3;
	double m_Y4;
	double m_Y5;
	double m_Y6;
	double m_Y7;

	double m_Z1;
	double m_Z2;
	double m_Z3;
	double m_Z4;
	double m_Z5;
	double m_Z6;
	double m_Z7;
	afx_msg void OnNMClickListBarparamList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBunBarparamSet();
	afx_msg void OnBnClickedOk();
};
