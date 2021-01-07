#pragma once



typedef struct _a81
{
	int pos;
	CString strText;
	_a81 &operator=(const _a81 &p)
	{
		pos = p.pos;
		strText = p.strText;
		return *this;
	}
}POS_TEXT;

typedef CArray<POS_TEXT,POS_TEXT&> CPosTextArray;

class AFX_EXT_CLASS CD2dProgressBar : public CWnd
{
	DECLARE_DYNAMIC(CD2dProgressBar)
private:
	//ID2D1Factory* m_pD2d1Factory;
	//ID2D1HwndRenderTarget* m_pRenderTarget;
	//ID2D1SolidColorBrush* m_pSolidColorBrush;
	//ID2D1LinearGradientBrush* m_pLinearGradientBrush;
	//IDWriteFactory* m_pDWriteFactory;
	//ID2D1SolidColorBrush* m_pGreyBrush;
	//
	//BOOL CreateDeviceIndependentResource();
	//BOOL CreateDeviceDependentResource();
	//void DiscardDeviceDependentResource();
	//void DestoryResource();
	
	void Render(HDC hDC,CRect rcBounds);
	//void ResizeRenderTarget(int width,int height);
public:
	void SetValue(int progressValue);
	int GetValue(void);
	CRect m_rcBounds;
 private:
	int m_ProgressValue;
	int m_ProgressMaxValue;
	CPosTextArray m_PosTextArray;

public:
	CD2dProgressBar();
	virtual ~CD2dProgressBar();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	void SetMaxValue(int _maxValue);
	void SetPosText(int pos, CString strText);
	void ClearPosText(void);
};


