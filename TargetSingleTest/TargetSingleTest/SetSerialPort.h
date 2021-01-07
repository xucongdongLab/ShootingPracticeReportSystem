#if !defined(AFX_SETSERIALPORT_H__4B93F676_1503_431E_87AF_EF1123833F2D__INCLUDED_)
#define AFX_SETSERIALPORT_H__4B93F676_1503_431E_87AF_EF1123833F2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetSerialPort.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetSerialPort dialog

class CSetSerialPort : public CDialog
{
// Construction
public:
	CSetSerialPort(CWnd* pParent = NULL);   // standard constructor
	void InitCOM1();
	int searchCom(CString *strPort);
	
	int int1;
	int int2;
	int int3;
	int int4;
	int int5;
	CString strPort[20];
	int portNum;
	


// Dialog Data
	//{{AFX_DATA(CSetSerialPort)
	enum { IDD = IDD_DLG_SETSERIALPORT };
	CComboBox	m_conCOM1;
	CString	m_str1;
	CString	m_str2;
	CString	m_str3;
	CString	m_str4;
	CString	m_str5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetSerialPort)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetSerialPort)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSerialOff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETSERIALPORT_H__4B93F676_1503_431E_87AF_EF1123833F2D__INCLUDED_)
