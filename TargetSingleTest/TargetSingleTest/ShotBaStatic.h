#pragma once

#ifndef __SHOTBA_STATIC_H__
#define __SHOTBA_STATIC_H__



#include "gdiplus.h"
using namespace Gdiplus;
typedef struct _shotPos_struct{
	int id;
	int KO;
	int Score;

	float x,y;
	bool bInShot;
	float xRatio,yRatio;
}SHOTPOS_STRUCT;

typedef struct _DispTypeStruct
{
	int type;
	CString strTypeName;
}DISPTYPE_STRUCT;


extern DISPTYPE_STRUCT disType_struct[];
extern int dispType_struct_length;

typedef CArray<SHOTPOS_STRUCT,SHOTPOS_STRUCT&> CShotPosArray;

#define GP_NUM3 8
#define GP_NUM4 6
#define GP_NUM 5
#define GRAPH_NUM6 2

class AFX_EXT_CLASS CShotBaStatic : public CStatic
{
	DECLARE_DYNAMIC(CShotBaStatic)
	int m_BaType;	

	int m_GroupId;

	int BaWidth;
	int BaHeight;

	int BlankWidth;
	int BlankHeight;

	float m_CenterX;
	float m_CenterY;
	float m_rad_i;
	float m_rad_Out;
	float _CenterX,_CenterY;
	int m_UPorDownType;//0 is up,1 is down
	int m_DispType;           //this parameter is the type of disp
	
	int m_DispContentType;
	int m_ScoreDispType;
	int m_BatteryLevel;
	CString m_ShotBaName;
	int m_BulletType;
	CRect rectReal;//真正绘制区域

	int m_TotalRoq;
	int m_CurRoq;
	int m_CurKO;
	int m_DirScore[12];
	int m_CurDir;
	bool m_bChangeColor;

	int m_Connected;

	Color m_BaOffColor;
	Color m_BaDownColor;
	Color m_BaColor;

	float m_Scale;
	CRect m_rcBounds;

	CWnd *m_pParent;
	UINT m_ClickCmd;

	CString m_strPersonName;

	bool SaveFlag;
	CString strBmpFileName;
	CShotPosArray m_ShotPosArray;


	double m_DrawBaLeft;
	double m_DrawBaBottom;
	double m_DrawBaWidth;
	double m_DrawBaHeight;

	Region *m_ScoreRegion[50];

	GraphicsPath *g_Gp6[GRAPH_NUM6];
	GraphicsPath *g_Gp7[GRAPH_NUM6];
	GraphicsPath *g_Gp8[GRAPH_NUM6];
public:
	CShotBaStatic();
	virtual ~CShotBaStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	short GetScore(short index);
	int m_ShotID;
public:
	bool InitD2D();
	void SetUporDownType(int type);
	void SetBatteryLevel(int level);
	void SetScoreDispType(int type);
	void SetDispType(int type);
	void SetDispContentType(int type);
	void SetBulletType(int _type);
public:
	afx_msg void OnPaint();
	void SetShotBaName(char *strName);
//	void SetShotBaName(CString strName);
	CString GetBaName(void);
	//void SetShotPos(int shotID, float x, float y);
	int GetCurRoq(void);
	int GetTotalRoq(void);
	void SetCurRoq(int score);
	void SetCurRoq(int score,int ko);
	void SetCurRoq(int score,int ko,double x,double y);
	void SetCurDirScore(int *Score);
	void SetCurDir(int _CurDir);
	//int GetTotalRoq(void);
	void SetConnectSta(int _Connected);
	int GetConnectSta(void);
	void SetScale(double m_Scale);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void SetParentInfo(CWnd* pParent, UINT nCmd);
	void SetPersonName(CString _strPersonName);
	bool PaintContent(HDC hDC, CRect rcBounds,bool printFlag=false);
	void WriteToBitmap(CString strFileName);
	void PaintContent2(HDC hDC, CRect rcBounds,CString strFileName);
	void ClearShotInfo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	void DrawBa(Graphics& _Graph,int type);
public:
	void DrawBattery(Graphics &_Graph,int level);
	void SetBaColor(Color _Color);
	void SetGroupId(int _GroupId);



	void DrawScoreList(Graphics &_Graph);
	void DrawScoreElec(Graphics &_Graph);
	void DrawScoreExtra(Graphics &_Graph);


	void SetBaType(int type);
	int GetBaType(void);
	void SetPointByScoreAndDirect(int _Score, int _direct, int baType, double & x, double & y);

public:
	void SetShotPos(int shotID, double x, double y);
	void ChangeShotPos(int shotID, double x, double y);//added 20201107
	void DeleteShot(int shotID);//added 20201107

	int GetDirect(short index);
	int GetShotCount(void);
	static int GetDispTypeCount();
	static DISPTYPE_STRUCT * GetDispTypeContent();
};


#endif