
// TargetSingleTestDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "ShotBaStatic.h"
#include "D2dProgressBar.h"

#define ID_M_STATICCLICK 20000
#define SHOTBA_NUM 10
#pragma comment(lib,"ShotBaDll.lib")

#define STATIC_NUM 100
#define MAX_BUFFER 1024
typedef struct _a31
{
	int barid;
	int bulletid;
	int temperature;
	int Tick2[10];
	int score;
	int direct;
	double xOrg;
	double yOrg;

	_a31()
	{
		barid = 1;
		bulletid = 1;
		temperature = 0;
		memset(Tick2,0,sizeof(int)*10);
		score = 0;
		direct = 0;
		xOrg = 0;
		yOrg = 0;
	}

}BULLET_DATA;

typedef CArray<BULLET_DATA,BULLET_DATA&> CBulletDataArray;


typedef struct _SensorStatusStru
{
	int barID;
//	int commStatus;
	int sensorID[7];
}SensorStatusStru;
typedef struct _SensitivityStru
{
	int barID;
	short Sensitivity;
	//0x0D--电压的十倍：即： 1.3V
	//0x12--电压的十倍：即： 1.8V
	//0x17--电压的十倍：即： 2.3V
}SensitivityStru;

// CTargetSingleTestDlg 对话框
class CTargetSingleTestDlg : public CDialogEx
{
// 构造
public:
	CTargetSingleTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TARGETSINGLETEST_DIALOG };


	int portNum;
	BYTE m_Buffer_Bar[MAX_BUFFER];
	int iCount_Bar;
	int m_ZJBulletID[STATIC_NUM];
	CBulletDataArray m_ZJBulletDataArray;
	HANDLE m_Thread_Ananlyze;
	DWORD ThreadID;
	bool AnalyzeZJFlag;
	CShotBaStatic m_BaStatic[SHOTBA_NUM];

	CEdit	m_conRevText;
	CString	m_strRevText;

	HANDLE theRecDataThreadForSensorDetect;//用于靶标器检测
	bool RecDataSensorDetectFlag;//用于靶标器检测
	int RecCount;//收到的检测指令数
	vector<SensorStatusStru> m_SensorStatus;//靶标器检测
	void InitSensorStatus(void);

	HANDLE theRecDataThreadForSensitivity;//用于靶标器传感器灵敏度设置
	bool RecDataSensitivityFlag;//用于靶标器传感器灵敏度设置
	vector<SensitivityStru> m_Sensitivity;//用于靶标器传感器灵敏度设置
	int RecTargetIDForSensitivity;//当前接收到的靶标编号
	void InitSensitivity(void);
	bool IsMovBar(int barID);//added by xucongdong





protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LONG OnCommunication(WPARAM ch, LPARAM port);
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_BombList;
	CStatic m_Picture;
	void InitBombList(void);
	int InsertBomb(int barID,int bombID,double X,double Y,double temp,double v,double alpha,double beta);
	void CommInit(UINT portNo = 1, UINT baud =9600,char parity= 'N', UINT databits = 8, UINT stopsbits = 1);
	void ZPAnalyzeData(void);
	void ZPAnalyzeDataFourFive(void);
	void ZPAnalyzeDataSeven(void);
	void InitShotBaStatic();
	void GetShotRect(CRect &shotBaRect);
	void ClearBarAndList();
	float ProcessOrientZJ(int barID, int bulletID, double x, double y, int& direct, int type);

	void AddRevText(CString strText);
	void AddRevTextASCII(char *revText, long ilength);
	void AddRevTextASCII2File(char *fileName,char *revText, long ilength);
	void SaveData();
	void SaveAddTick(char *strFileName,double Tick[],double temp);

	void SendDataZJ(int barid,int type, int dataSend);
	void SendDataZJ(int barid,int type, short actionCode1,short actionCode2);
	void SendDataMov(int barid,int type, int dataSend);
	void SendDataMov(int barid,int type, short actionCode1,short actionCode2);

	afx_msg void OnBnClickedBunTargetsingletestBarparam();
	afx_msg void OnBnClickedBunTargetsingletestOpencom();
	afx_msg void OnBnClickedBunTargetsingletestClosecom();
	afx_msg void OnBnClickedBunTargetsingletestStart();
	afx_msg void OnBnClickedBunTargetsingletestEnd();
	afx_msg void OnBnClickedBunTargetsingletestTest();
	afx_msg void OnBnClickedBunTargetsingletestReset();
	int m_iShellDiameter;
	afx_msg void OnBnClickedBunTargetsingletestShelltype();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBunTargetsingletestSave();
	CComboBox m_comAlgType;
	CString m_strAlgType;
	afx_msg void OnBnClickedBunTargetsingletestBeginalgorithm();
	afx_msg void OnBnClickedBunTargetsingletestEndalgorithm();
	int m_iVolt;
	int m_iAmplification;
	afx_msg void OnBnClickedBunTargetsingletestResenddata();
	afx_msg void OnBnClickedBunTargetsingletestReadvolt();
	afx_msg void OnBnClickedBunTargetsingletestWritevolt();
	afx_msg void OnBnClickedBunTargetsingletestSetamplification();
	afx_msg void OnBnClickedBunTargetsingletestSavesrcdata();
	afx_msg void OnBnClickedBunTargetsingletestSensordetect();
	afx_msg void OnBnClickedBunTargetsingletestStartsend();
	afx_msg void OnBnClickedBunTargetsingletestEndsend();
};
