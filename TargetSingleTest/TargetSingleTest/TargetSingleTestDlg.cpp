
// TargetSingleTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TargetSingleTest.h"
#include "TargetSingleTestDlg.h"
#include "afxdialogex.h"
#include "BarParam.h"
#include "SetSerialPort.h"
#include <atlconv.h>

#include "quasi_newton.h"


extern CTargetSingleTestApp theApp;
BYTE g_BufferBar_Save[MAX_BUFFER];


CMutex FlagMutexSensorDetect;//对接收传感器测试线程m_SensorStatus的控制

#include "TargetDetectDlg.h"
CTargetDetectDlg *g_pTargetDetect;
void TargetDetect_Rev();
UINT RecDataForSensorDetectProc(LPVOID p);

#include "SensitivityDlg.h"
CSensitivityDlg *g_pSensitivity;
void Sensitivity_Rev();
UINT RecDataForSensitivityProc(LPVOID p);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//added by xucongdong 用于解算炸点坐标
#pragma comment(lib,"libmx.lib")
#pragma comment(lib,"libmat.lib")
#pragma comment(lib,"libeng.lib")
#pragma comment(lib,"libmex.lib")
#pragma comment(lib,"mclmcr.lib")
#pragma comment(lib,"mclmcrrt.lib")

#include "GetPos_Condor.h"
#pragma comment(lib,"GetPos_Condor.lib")

#include "LSM.h"
#pragma comment(lib,"LSM.lib")
#include "SevenSensor.h"
#pragma comment(lib,"SevenSensor.lib")


#ifndef BOMB_STRU
#define BOMB_STRU
typedef struct _BombStru
{
	int iBarID;
	int iBombID;
	int iX;
	int iY;
}BombStru;
#endif
#pragma comment(lib,"ExcelDll.lib")
_declspec(dllimport) void CreateExcel_Bomb(CString strFileName,CArray<BombStru,BombStru&> &BombStruArray);

CMutex FlagMutex;
bool computeFlag = false;
///报靶  added by xucongdong begin////////////
bool initCondors(const char *algorithmType)
{
	if(strcmp(algorithmType,"Seven_Condor") == 0)
	{
		mclmcrInitialize();
		if(!mclInitializeApplication(NULL,0)) return false;
		if(!SevenSensorInitialize()) return false;
	}
	else if(strcmp(algorithmType,"LSM_GetPos") == 0)
	{
		//初始化
		mclmcrInitialize();
		if(!mclInitializeApplication(NULL,0)) return false;
		if(!LSMInitialize()) return false;
	}
	else if(strcmp(algorithmType,"Four_Condor") == 0)
	{
		//初始化
		mclmcrInitialize();
		if(!mclInitializeApplication(NULL,0)) return false;
		if(!GetPos_CondorInitialize()) return false;
	}
	else if(strcmp(algorithmType,"Five_Condor") == 0)
	{
		//初始化
		mclmcrInitialize();
		if(!mclInitializeApplication(NULL,0)) return false;
		if(!GetPos_CondorInitialize()) return false;
	}
	else if(strcmp(algorithmType,"DLP_GetPos") == 0)
	{
		//初始化
	}
	else
	{
		return false;
	}
	computeFlag = true;
	return true;
}
void uninitCondors(const char *algorithmType)
{
	if(strcmp(algorithmType,"Seven_Condor") == 0)
	{
		SevenSensorTerminate();
		mclTerminateApplication();	
	}
	else if(strcmp(algorithmType,"LSM_GetPos") == 0)
	{
		LSMTerminate();
		mclTerminateApplication();	
	}
	else if(strcmp(algorithmType,"Four_Condor") == 0)
	{
		GetPos_CondorTerminate();
		mclTerminateApplication();	
	}
	else if(strcmp(algorithmType,"Five_Condor") == 0)
	{
		GetPos_CondorTerminate();
		mclTerminateApplication();	
	}
	else if(strcmp(algorithmType,"DLP_GetPos") == 0)
	{

	}
	else
	{

	}
	computeFlag = true;
}

bool GetTargetPosByFourT( const BarParamStru barParamStru, double temp, double T1, double T2, double T3, double T4, double T5,double &X,double &Y, double &V,double &Alpha,double &Beta)
{	
	//InitData为初始数据,targetWidth为靶面宽度（mm），targetHeight为靶面高度（mm），vspeed_min和vspeed_max为预估子弹的小最大值（m/s）
	//InitData为7*5的矩阵，第一列为时间数据（单位：us），第二至第四列代表传感器坐标x,y,z(单位：mm），每一行分别对应一个传感器,第五列从第一行至第六行分别是传感器数目、温度（摄氏度）、是否固定水平入射角、
	//是否固定俯仰角（0为否，1为是）、水平入射角值(弧度）、俯仰角值（弧度）
	mwArray InitData(7,5,mxDOUBLE_CLASS,mxREAL);
	mwArray TargetWidth(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray TargetHeight(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray Vspeed_min(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray Vspeed_max(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray A_min(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray A_max(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray B_min(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray B_max(1,1,mxDOUBLE_CLASS,mxREAL);

	//由于mwArray类型赋值时只能用一维数组赋值，所以需要先定义一维数组_data
	double _data[35] = {944,2324,1863,0,0,0,0, //时间数据
		barParamStru.X1,barParamStru.X2,barParamStru.X3,barParamStru.X4,barParamStru.X5,barParamStru.X6,barParamStru.X7,//传感器x坐标
		barParamStru.Y1,barParamStru.Y2,barParamStru.Y3,barParamStru.Y4,barParamStru.Y5,barParamStru.Y6,barParamStru.Y7,//传感器y坐标
		barParamStru.Z1,barParamStru.Z2,barParamStru.Z3,barParamStru.Z4,barParamStru.Z5,barParamStru.Z6,barParamStru.Z7,//传感器z坐标
		4,38,0,0,0,0,0};//分别为传感器数目、试验温度、水平入射角是否固定、俯仰角是否固定、水平入射角值、俯仰角值、保留数据（无效）
	//double data1[35] = {0};
//	double dWidth[] = {barParamStru.Width+2000};//计算的范围增加两边各1000
//	double dHeight[] = {barParamStru.Bottom+barParamStru.Height+1000};//计算的范围增加向上1000
	double dWidth[] = {10000};//计算的范围从中心点两边各5000
	double dHeight[] = {5000};//计算的范围从传感器向上5000
	double dVsmin[] = {500};
	double dVsmax[] = {1500};
	double dAmin[] = {-1};
	double dAmax[] = {1};
	double dBmin[] = {-1};
	double dBmax[] = {1};
		
	_data[0]=T1;
	_data[1]=T2;
	_data[2]=T3;
	_data[3]=T4;
//	_data[4]=T5;
	_data[29]=temp;//试验温度
//	CString str;
//	str.Format("T1=%f;T2=%f,T3=%f;T4=%f;T5=%f;temp=%f",_data[0],_data[1],_data[2],_data[3],_data[4],_data[29]);
//	AfxMessageBox(str);

	InitData.SetData(_data,7*5);//利用一维数组_data给二维数组InitData赋值
	TargetWidth.SetData(dWidth,1);
	TargetHeight.SetData(dHeight,1);
	Vspeed_min.SetData(dVsmin,1);
	Vspeed_max.SetData(dVsmax,1);
	A_min.SetData(dAmin,1);
	A_max.SetData(dAmax,1);
	B_min.SetData(dBmin,1);
	B_max.SetData(dBmax,1);
	mwArray outOpt;//输出值（x，y，子弹速度V，水平入射角A，俯仰角B）
	
	GetPos_Condor(1,outOpt,InitData,TargetWidth,TargetHeight,Vspeed_min,Vspeed_max,A_min,A_max,B_min,B_max);
	mwSize OutDims = outOpt.GetDimensions();

	X = outOpt.Get(OutDims,1);
	X=X*1000;
	Y = outOpt.Get(OutDims,2);
	Y=Y*1000;
	V = outOpt.Get(OutDims,3);
	Alpha = outOpt.Get(OutDims,4);
	Alpha = Alpha*180/3.1415926;
	Beta = outOpt.Get(OutDims,5);
	Beta = Beta*180/3.1415926;
	//子弹速度V，水平入射角A，俯仰角B
	//GetPos_Condor函数计算出来的XY单位为米；
	//将XY转化为mm
	if((X>4999)||(X<-4999)||(Y<1)||(Y>4999))return false;//不在计算的范围内
	return true;
}

bool GetTargetPosByFiveT( const BarParamStru barParamStru, double temp, double T1, double T2, double T3, double T4, double T5,double &X,double &Y ,double &V,double &Alpha,double &Beta)
{//double Width,double Height,double Bottom,
	//初始化
//	mclmcrInitialize();
//	if(!mclInitializeApplication(NULL,0)) return false;
//	if(!GetPos_CondorInitialize()) return false;
	
	//InitData为初始数据,targetWidth为靶面宽度（mm），targetHeight为靶面高度（mm），vspeed_min和vspeed_max为预估子弹的小最大值（m/s）
	//InitData为7*5的矩阵，第一列为时间数据（单位：us），第二至第四列代表传感器坐标x,y,z(单位：mm），每一行分别对应一个传感器,第五列从第一行至第六行分别是传感器数目、温度（摄氏度）、是否固定水平入射角、
	//是否固定俯仰角（0为否，1为是）、水平入射角值(弧度）、俯仰角值（弧度）
	mwArray InitData(7,5,mxDOUBLE_CLASS,mxREAL);
	mwArray TargetWidth(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray TargetHeight(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray Vspeed_min(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray Vspeed_max(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray A_min(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray A_max(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray B_min(1,1,mxDOUBLE_CLASS,mxREAL);
	mwArray B_max(1,1,mxDOUBLE_CLASS,mxREAL);

	//由于mwArray类型赋值时只能用一维数组赋值，所以需要先定义一维数组_data
	double _data[35] = {944,2324,1863,0,2132,0,0, //时间数据
		barParamStru.X1,barParamStru.X2,barParamStru.X3,barParamStru.X4,barParamStru.X5,barParamStru.X6,barParamStru.X7,//传感器x坐标
		barParamStru.Y1,barParamStru.Y2,barParamStru.Y3,barParamStru.Y4,barParamStru.Y5,barParamStru.Y6,barParamStru.Y7,//传感器y坐标
		barParamStru.Z1,barParamStru.Z2,barParamStru.Z3,barParamStru.Z4,barParamStru.Z5,barParamStru.Z6,barParamStru.Z7,//传感器z坐标
		5,38,0,0,0,0,0};//分别为传感器数目、试验温度、水平入射角是否固定、俯仰角是否固定、水平入射角值、俯仰角值、保留数据（无效）
	//double data1[35] = {0};
//	double dWidth[] = {barParamStru.Width+2000};//计算的范围增加两边各1000
//	double dHeight[] = {barParamStru.Bottom+barParamStru.Height+1000};//计算的范围增加向上1000
	double dWidth[] = {10000};//计算的范围从中心点两边各5000
	double dHeight[] = {5000};//计算的范围从传感器向上5000
	double dVsmin[] = {500};
	double dVsmax[] = {1500};
	double dAmin[] = {-1};
	double dAmax[] = {1};
	double dBmin[] = {-1};
	double dBmax[] = {1};

/*	
	//由于mwArray类型赋值时只能用一维数组赋值，所以需要先定义一维数组_data
	double _data[35] = {944,2324,1863,0,2132,0,0, //时间数据
		-1800,1800,-1800,0,1800,0,0,//传感器x坐标
		0,0,0,0,0,0,0,//传感器y坐标
		500,500,0,0,0,0,0,//传感器z坐标
		5,38,0,0,0,0,0};//分别为传感器数目、试验温度、水平入射角是否固定、俯仰角是否固定、水平入射角值、俯仰角值、保留数据（无效）
	//double data1[35] = {0};
	double dWidth[] = {Width};
	double dHeight[] = {Height+Bottom};
//	double dWidth[] = {3000};
//	double dHeight[] = {2000};
	double dVsmin[] = {500};
	double dVsmax[] = {1500};
	double dAmin[] = {-1};
	double dAmax[] = {1};
	double dBmin[] = {-1};
	double dBmax[] = {1};
*/
	
	_data[0]=T1;
	_data[1]=T2;
	_data[2]=T3;
	_data[3]=T4;
	_data[4]=T5;
	_data[29]=temp;//试验温度
//	CString str;
//	str.Format("T1=%f;T2=%f,T3=%f;T4=%f;T5=%f;temp=%f",_data[0],_data[1],_data[2],_data[3],_data[4],_data[29]);
//	AfxMessageBox(str);
	

	InitData.SetData(_data,7*5);//利用一维数组_data给二维数组InitData赋值
	TargetWidth.SetData(dWidth,1);
	TargetHeight.SetData(dHeight,1);
	Vspeed_min.SetData(dVsmin,1);
	Vspeed_max.SetData(dVsmax,1);
	A_min.SetData(dAmin,1);
	A_max.SetData(dAmax,1);
	B_min.SetData(dBmin,1);
	B_max.SetData(dBmax,1);
	mwArray outOpt;//输出值（x，y，子弹速度V，水平入射角A，俯仰角B）
	
	GetPos_Condor(1,outOpt,InitData,TargetWidth,TargetHeight,Vspeed_min,Vspeed_max,A_min,A_max,B_min,B_max);
	mwSize OutDims = outOpt.GetDimensions();

	X = outOpt.Get(OutDims,1);
	X=X*1000;
	Y = outOpt.Get(OutDims,2);
	Y=Y*1000;
	V = outOpt.Get(OutDims,3);
	Alpha = outOpt.Get(OutDims,4);
	Alpha = Alpha*180/3.1415926;
	Beta = outOpt.Get(OutDims,5);
	Beta = Beta*180/3.1415926;
	//子弹速度V，水平入射角A，俯仰角B
	//GetPos_Condor函数计算出来的XY单位为米；
	//将XY转化为mm

//	GetPos_CondorTerminate();
//	mclTerminateApplication();	
	if((X>4999)||(X<-4999)||(Y<1)||(Y>4999))return false;//不在计算的范围内
	return true;
}

bool GetTargetPosByDLP( const BarParamStru barParamStru, const double temp, const int iNum, const double T[],double &X,double &Y, double &V,double &Alpha,double &Beta)
{

	int n = 6;//方程数
	double  Xk[5];
    double pi = 3.14159265358979;

    double xx[7] = { -1.5,-0.75,0.75,1.5,1.5,0, -1.5 };//传感器位置
    double yy[7] = { 0.5,0,0,0.5,0,0,0 };
    double zz[7] = { 0,0,0,0,1,1,1 };
	double myT[7];

    Xk[0] = 0;//X初值
    Xk[1] = 1.7;//Y初值
    Xk[2] = 0.8;//弹速初值
    Xk[3] = 1.0 * pi / 180.0;//水平入射角初值
    Xk[4] = 1.0 * pi / 180.0;//垂直入射角初值
	xx[0] = barParamStru.X1/1000; yy[0] = barParamStru.Y1/1000; zz[0] = barParamStru.Z1/1000;
	xx[1] = barParamStru.X2/1000; yy[1] = barParamStru.Y2/1000; zz[1] = barParamStru.Z2/1000;
	xx[2] = barParamStru.X3/1000; yy[2] = barParamStru.Y3/1000; zz[2] = barParamStru.Z3/1000;
	xx[3] = barParamStru.X4/1000; yy[3] = barParamStru.Y4/1000; zz[3] = barParamStru.Z4/1000;
	xx[4] = barParamStru.X5/1000; yy[4] = barParamStru.Y5/1000; zz[4] = barParamStru.Z5/1000;
	xx[5] = barParamStru.X6/1000; yy[5] = barParamStru.Y6/1000; zz[5] = barParamStru.Z6/1000;
	xx[6] = barParamStru.X7/1000; yy[6] = barParamStru.Y7/1000; zz[6] = barParamStru.Z7/1000;
	for(int i = 0; i < 7; i++)
	{
		myT[i] = T[i]*0.000001;
	}
	//iNum传感器数量
	int nIndex = 0;//基准传感器序号
	for(nIndex = 0; nIndex < iNum; nIndex++)
	{
		if(T[nIndex] == 0) 
		{
			nIndex++;
			break;
		}
	}
	double c = 331.4 + temp * 0.61;//声速
    double  result = DFP(6, Xk, xx, yy, zz, c, iNum, myT, nIndex);
	if((result<0)||(result>10000))return false;
	X = Xk[0]*1000;
	Y = Xk[1]*1000;
	V = Xk[2];
	Alpha = Xk[3]*180/3.1415926;
	Beta = Xk[4]*180/3.1415926;
	if((X>4999)||(X<-4999)||(Y<1)||(Y>4999))return false;//不在计算的范围内
	else return true;

//double DFP(int n, double *min_point, double xx[], double yy[], double zz[], int N_M, double d_T_n[], int Mb)
}

//bool GetTargetPosByMultiT( const BarParamStru barParamStru, const char *algorithmType, const int iNum, const double temp, const double T[],double &X,double &Y, double &V,double &Alpha,double &Beta)
bool GetTargetPosByMultiT( const BarParamStru barParamStru, const char *algorithmType, const double temp, const double T[],double &X,double &Y, double &V,double &Alpha,double &Beta)
{
	if(strcmp(algorithmType,"DLP_GetPos") == 0)
	{
		int tNum = 7;//传感器数量
		if(!GetTargetPosByDLP(barParamStru, temp, tNum, T,X,Y, V,Alpha,Beta))
			return false;
	}
	else if(strcmp(algorithmType,"Seven_Condor") == 0)
	{
		//InitData为初始数据,targetWidth为靶面宽度（mm），targetHeight为靶面高度（mm），vspeed_min和vspeed_max为预估子弹的小最大值（m/s）
		//InitData为7*5的矩阵，第一列为时间数据（单位：us），第二至第四列代表传感器坐标x,y,z(单位：mm），每一行分别对应一个传感器,第五列从第一行至第六行分别是传感器数目、温度（摄氏度）、是否固定水平入射角、
		//是否固定俯仰角（0为否，1为是）、水平入射角值(弧度）、俯仰角值（弧度）
		mwArray InitData(7,5,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetWidth(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetHeight(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray G(1,1,mxDOUBLE_CLASS,mxREAL);//无效参数
		mwArray Vspeed_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray Vspeed_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_max(1,1,mxDOUBLE_CLASS,mxREAL);
		int tNum = 7;//传感器数量

		//由于mwArray类型赋值时只能用一维数组赋值，所以需要先定义一维数组_data
		double _data[35] = {T[0],T[1],T[2],T[3],T[4],T[5],T[6], //时间数据
			barParamStru.X1,barParamStru.X2,barParamStru.X3,barParamStru.X4,barParamStru.X5,barParamStru.X6,barParamStru.X7,//传感器x坐标
			barParamStru.Y1,barParamStru.Y2,barParamStru.Y3,barParamStru.Y4,barParamStru.Y5,barParamStru.Y6,barParamStru.Y7,//传感器y坐标
			barParamStru.Z1,barParamStru.Z2,barParamStru.Z3,barParamStru.Z4,barParamStru.Z5,barParamStru.Z6,barParamStru.Z7,//传感器z坐标
			(double)tNum,(double)temp,0,0,0,0,0};//分别为传感器数目、试验温度、水平入射角是否固定、俯仰角是否固定、水平入射角值、俯仰角值、保留数据（无效）

		double dWidth[] = {10000};//计算的范围从中心点两边各5000
		double dHeight[] = {5000};//计算的范围从传感器向上5000
		double dG[] = {0};
		//double dWidth[] = {barParamStru.Width+10000};//计算的范围从中心点两边各5000
		//double dHeight[] = {barParamStru.Height+barParamStru.Bottom+5000};//计算的范围从传感器向上5000
		//double dWidth[] = {barParamStru.Width+600};//计算的范围从中心点
		//double dHeight[] = {barParamStru.Height+barParamStru.Bottom+300};//计算的范围从传感器向上
		//double dG[] = {barParamStru.Bottom};
		//double dWidth[] = {3000};//计算的范围从中心点两边各5000
		//double dHeight[] = {2400};//计算的范围从传感器向上5000
		//double dG[] = {1000};
		double dVsmin[] = {500};
		double dVsmax[] = {1500};
		double dAmin[] = {-1};
		double dAmax[] = {1};
		double dBmin[] = {-1};
		double dBmax[] = {1};

		InitData.SetData(_data,7*5);//利用一维数组_data给二维数组InitData赋值
		TargetWidth.SetData(dWidth,1);
		TargetHeight.SetData(dHeight,1);
		G.SetData(dG,1);
		Vspeed_min.SetData(dVsmin,1);
		Vspeed_max.SetData(dVsmax,1);
		A_min.SetData(dAmin,1);
		A_max.SetData(dAmax,1);
		B_min.SetData(dBmin,1);
		B_max.SetData(dBmax,1);
		mwArray outOpt;//输出值（x，y，子弹速度V，水平入射角A，俯仰角B）
		mwSize OutDims;
		
		Seven_Condor(1,outOpt,InitData,TargetWidth,TargetHeight,G,Vspeed_min,Vspeed_max,A_min,A_max,B_min,B_max);
		OutDims = outOpt.GetDimensions();

		X = outOpt.Get(OutDims,1);
		X=X*1000;
		Y = outOpt.Get(OutDims,2);
		Y=Y*1000;
		V = outOpt.Get(OutDims,3);
		Alpha = outOpt.Get(OutDims,4);
		Alpha = Alpha*180/3.1415926;
		Beta = outOpt.Get(OutDims,5);
		Beta = Beta*180/3.1415926;
		//子弹速度V，水平入射角A，俯仰角B
		//GetPos_Condor函数计算出来的XY单位为米；
		//将XY转化为mm
		if((X>4999)||(X<-4999)||(Y<1)||(Y>4999))return false;//不在计算的范围内
	}
	else if(strcmp(algorithmType,"LSM_GetPos") == 0)
	{
		//InitData为初始数据,targetWidth为靶面宽度（mm），targetHeight为靶面高度（mm），vspeed_min和vspeed_max为预估子弹的小最大值（m/s）
		//InitData为7*5的矩阵，第一列为时间数据（单位：us），第二至第四列代表传感器坐标x,y,z(单位：mm），每一行分别对应一个传感器,第五列从第一行至第六行分别是传感器数目、温度（摄氏度）、是否固定水平入射角、
		//是否固定俯仰角（0为否，1为是）、水平入射角值(弧度）、俯仰角值（弧度）
		mwArray InitData(7,5,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetWidth(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetHeight(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray G(1,1,mxDOUBLE_CLASS,mxREAL);//无效参数
		mwArray Vspeed_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray Vspeed_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_max(1,1,mxDOUBLE_CLASS,mxREAL);
		int tNum = 7;//传感器数量

		//由于mwArray类型赋值时只能用一维数组赋值，所以需要先定义一维数组_data
		double _data[35] = {T[0],T[1],T[2],T[3],T[4],T[5],T[6], //时间数据
			barParamStru.X1,barParamStru.X2,barParamStru.X3,barParamStru.X4,barParamStru.X5,barParamStru.X6,barParamStru.X7,//传感器x坐标
			barParamStru.Y1,barParamStru.Y2,barParamStru.Y3,barParamStru.Y4,barParamStru.Y5,barParamStru.Y6,barParamStru.Y7,//传感器y坐标
			barParamStru.Z1,barParamStru.Z2,barParamStru.Z3,barParamStru.Z4,barParamStru.Z5,barParamStru.Z6,barParamStru.Z7,//传感器z坐标
			(double)tNum,(double)temp,0,0,0,0,0};//分别为传感器数目、试验温度、水平入射角是否固定、俯仰角是否固定、水平入射角值、俯仰角值、保留数据（无效）

		//double dWidth[] = {10000};//计算的范围从中心点两边各5000
		//double dHeight[] = {5000};//计算的范围从传感器向上5000
		//double dWidth[] = {barParamStru.Width+10000};//计算的范围从中心点两边各5000
		//double dHeight[] = {barParamStru.Height+barParamStru.Bottom+5000};//计算的范围从传感器向上5000
		double dWidth[] = {barParamStru.Width};//计算的范围从中心点
		double dHeight[] = {barParamStru.Height+barParamStru.Bottom};//计算的范围从传感器向上
		double dG[] = {barParamStru.Bottom};
		//double dWidth[] = {3000};//计算的范围从中心点两边各5000
		//double dHeight[] = {2400};//计算的范围从传感器向上5000
		//double dG[] = {1000};
		double dVsmin[] = {500};
		double dVsmax[] = {1500};
		double dAmin[] = {-1};
		double dAmax[] = {1};
		double dBmin[] = {-1};
		double dBmax[] = {1};

		InitData.SetData(_data,7*5);//利用一维数组_data给二维数组InitData赋值
		TargetWidth.SetData(dWidth,1);
		TargetHeight.SetData(dHeight,1);
		G.SetData(dG,1);
		Vspeed_min.SetData(dVsmin,1);
		Vspeed_max.SetData(dVsmax,1);
		A_min.SetData(dAmin,1);
		A_max.SetData(dAmax,1);
		B_min.SetData(dBmin,1);
		B_max.SetData(dBmax,1);
		mwArray outOpt;//输出值（x，y，子弹速度V，水平入射角A，俯仰角B）
		mwSize OutDims;

		LSM_GetPos(1,outOpt,InitData,TargetWidth,TargetHeight,G,Vspeed_min,Vspeed_max,A_min,A_max,B_min,B_max);
		OutDims = outOpt.GetDimensions();

		X = outOpt.Get(OutDims,1);
		X=X*1000;
		Y = outOpt.Get(OutDims,2);
		Y=Y*1000;
		V = outOpt.Get(OutDims,3);
		Alpha = outOpt.Get(OutDims,4);
		Alpha = Alpha*180/3.1415926;
		Beta = outOpt.Get(OutDims,5);
		Beta = Beta*180/3.1415926;
		//子弹速度V，水平入射角A，俯仰角B
		//将XY转化为mm
		if((X>4999)||(X<-4999)||(Y<1)||(Y>4999))return false;//不在计算的范围内
	}
	else if((strcmp(algorithmType,"Four_Condor") == 0)||(strcmp(algorithmType,"NewFour_Condor") == 0))
	{
		//InitData为初始数据,targetWidth为靶面宽度（mm），targetHeight为靶面高度（mm），vspeed_min和vspeed_max为预估子弹的小最大值（m/s）
		//InitData为7*5的矩阵，第一列为时间数据（单位：us），第二至第四列代表传感器坐标x,y,z(单位：mm），每一行分别对应一个传感器,第五列从第一行至第六行分别是传感器数目、温度（摄氏度）、是否固定水平入射角、
		//是否固定俯仰角（0为否，1为是）、水平入射角值(弧度）、俯仰角值（弧度）
		mwArray InitData(7,5,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetWidth(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetHeight(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray G(1,1,mxDOUBLE_CLASS,mxREAL);//无效参数
		mwArray Vspeed_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray Vspeed_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_max(1,1,mxDOUBLE_CLASS,mxREAL);
		int tNum = 4;//传感器数量

		//由于mwArray类型赋值时只能用一维数组赋值，所以需要先定义一维数组_data
		double _data[35] = {T[0],T[1],T[2],T[3],T[4],T[5],T[6], //时间数据
			barParamStru.X1,barParamStru.X2,barParamStru.X3,barParamStru.X4,barParamStru.X5,barParamStru.X6,barParamStru.X7,//传感器x坐标
			barParamStru.Y1,barParamStru.Y2,barParamStru.Y3,barParamStru.Y4,barParamStru.Y5,barParamStru.Y6,barParamStru.Y7,//传感器y坐标
			barParamStru.Z1,barParamStru.Z2,barParamStru.Z3,barParamStru.Z4,barParamStru.Z5,barParamStru.Z6,barParamStru.Z7,//传感器z坐标
			(double)tNum,(double)temp,0,0,0,0,0};//分别为传感器数目、试验温度、水平入射角是否固定、俯仰角是否固定、水平入射角值、俯仰角值、保留数据（无效）

		double dWidth[] = {10000};//计算的范围从中心点两边各5000
		double dHeight[] = {5000};//计算的范围从传感器向上5000
		//double dWidth[] = {barParamStru.Width+10000};//计算的范围从中心点两边各5000
		//double dHeight[] = {barParamStru.Height+barParamStru.Bottom+5000};//计算的范围从传感器向上5000
		//double dWidth[] = {barParamStru.Width+600};//计算的范围从中心点
		//double dHeight[] = {barParamStru.Height+barParamStru.Bottom+300};//计算的范围从传感器向上
		//double dG[] = {barParamStru.Bottom};
		//double dWidth[] = {3000};//计算的范围从中心点两边各5000
		//double dHeight[] = {2400};//计算的范围从传感器向上5000
		//double dG[] = {1000};
		double dVsmin[] = {500};
		double dVsmax[] = {1500};
		double dAmin[] = {-1};
		double dAmax[] = {1};
		double dBmin[] = {-1};
		double dBmax[] = {1};


		InitData.SetData(_data,7*5);//利用一维数组_data给二维数组InitData赋值
		TargetWidth.SetData(dWidth,1);
		TargetHeight.SetData(dHeight,1);
		//G.SetData(dG,1);
		Vspeed_min.SetData(dVsmin,1);
		Vspeed_max.SetData(dVsmax,1);
		A_min.SetData(dAmin,1);
		A_max.SetData(dAmax,1);
		B_min.SetData(dBmin,1);
		B_max.SetData(dBmax,1);
		mwArray outOpt;//输出值（x，y，子弹速度V，水平入射角A，俯仰角B）
		mwSize OutDims;

		GetPos_Condor(1,outOpt,InitData,TargetWidth,TargetHeight,Vspeed_min,Vspeed_max,A_min,A_max,B_min,B_max);
		OutDims = outOpt.GetDimensions();

		X = outOpt.Get(OutDims,1);
		X=X*1000;
		Y = outOpt.Get(OutDims,2);
		Y=Y*1000;
		V = outOpt.Get(OutDims,3);
		//没有其它的
		Alpha = 0;
		Beta = 0;
		//Alpha = outOpt.Get(OutDims,4);
		//Alpha = Alpha*180/3.1415926;
		//Beta = outOpt.Get(OutDims,5);
		//Beta = Beta*180/3.1415926;
		//子弹速度V，水平入射角A，俯仰角B
		//将XY转化为mm
		if((X>4999)||(X<-4999)||(Y<1)||(Y>4999))return false;//不在计算的范围内
	}
	else if((strcmp(algorithmType,"Five_Condor") == 0)||(strcmp(algorithmType,"NewFive_Condor") == 0))
	{
		//InitData为初始数据,targetWidth为靶面宽度（mm），targetHeight为靶面高度（mm），vspeed_min和vspeed_max为预估子弹的小最大值（m/s）
		//InitData为7*5的矩阵，第一列为时间数据（单位：us），第二至第四列代表传感器坐标x,y,z(单位：mm），每一行分别对应一个传感器,第五列从第一行至第六行分别是传感器数目、温度（摄氏度）、是否固定水平入射角、
		//是否固定俯仰角（0为否，1为是）、水平入射角值(弧度）、俯仰角值（弧度）
		mwArray InitData(7,5,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetWidth(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray TargetHeight(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray G(1,1,mxDOUBLE_CLASS,mxREAL);//无效参数
		mwArray Vspeed_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray Vspeed_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray A_max(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_min(1,1,mxDOUBLE_CLASS,mxREAL);
		mwArray B_max(1,1,mxDOUBLE_CLASS,mxREAL);
		int tNum = 5;//传感器数量

		//由于mwArray类型赋值时只能用一维数组赋值，所以需要先定义一维数组_data
		double _data[35] = {T[0],T[1],T[2],T[3],T[4],T[5],T[6], //时间数据
			barParamStru.X1,barParamStru.X2,barParamStru.X3,barParamStru.X4,barParamStru.X5,barParamStru.X6,barParamStru.X7,//传感器x坐标
			barParamStru.Y1,barParamStru.Y2,barParamStru.Y3,barParamStru.Y4,barParamStru.Y5,barParamStru.Y6,barParamStru.Y7,//传感器y坐标
			barParamStru.Z1,barParamStru.Z2,barParamStru.Z3,barParamStru.Z4,barParamStru.Z5,barParamStru.Z6,barParamStru.Z7,//传感器z坐标
			(double)tNum,(double)temp,0,0,0,0,0};//分别为传感器数目、试验温度、水平入射角是否固定、俯仰角是否固定、水平入射角值、俯仰角值、保留数据（无效）

		double dWidth[] = {10000};//计算的范围从中心点两边各5000
		double dHeight[] = {5000};//计算的范围从传感器向上5000
		//double dWidth[] = {barParamStru.Width+10000};//计算的范围从中心点两边各5000
		//double dHeight[] = {barParamStru.Height+barParamStru.Bottom+5000};//计算的范围从传感器向上5000
		//double dWidth[] = {barParamStru.Width+600};//计算的范围从中心点
		//double dHeight[] = {barParamStru.Height+barParamStru.Bottom+300};//计算的范围从传感器向上
		//double dG[] = {barParamStru.Bottom};
		//double dWidth[] = {3000};//计算的范围从中心点两边各5000
		//double dHeight[] = {2400};//计算的范围从传感器向上5000
		//double dG[] = {1000};
		double dVsmin[] = {500};
		double dVsmax[] = {1500};
		double dAmin[] = {-1};
		double dAmax[] = {1};
		double dBmin[] = {-1};
		double dBmax[] = {1};

		InitData.SetData(_data,7*5);//利用一维数组_data给二维数组InitData赋值
		TargetWidth.SetData(dWidth,1);
		TargetHeight.SetData(dHeight,1);
		//G.SetData(dG,1);
		Vspeed_min.SetData(dVsmin,1);
		Vspeed_max.SetData(dVsmax,1);
		A_min.SetData(dAmin,1);
		A_max.SetData(dAmax,1);
		B_min.SetData(dBmin,1);
		B_max.SetData(dBmax,1);
		mwArray outOpt;//输出值（x，y，子弹速度V，水平入射角A，俯仰角B）
		mwSize OutDims;

		GetPos_Condor(1,outOpt,InitData,TargetWidth,TargetHeight,Vspeed_min,Vspeed_max,A_min,A_max,B_min,B_max);
		OutDims = outOpt.GetDimensions();

		X = outOpt.Get(OutDims,1);
		X=X*1000;
		Y = outOpt.Get(OutDims,2);
		Y=Y*1000;
		V = outOpt.Get(OutDims,3);
		Alpha = 0;
		Beta = 0;
		//Alpha = outOpt.Get(OutDims,4);
		//Alpha = Alpha*180/3.1415926;
		//Beta = outOpt.Get(OutDims,5);
		//Beta = Beta*180/3.1415926;
		//子弹速度V，水平入射角A，俯仰角B
		//将XY转化为mm
		if((X>4999)||(X<-4999)||(Y<1)||(Y>4999))return false;//不在计算的范围内
	}
	else
	{
		return false;
	}
	return true;

}
bool NormalT(double T[],int iCount)
{
	if(iCount>7)return false;
	double minT = 0;
	minT = T[0];
	for(int i = 1; i < iCount; i++)
	{
		if(T[i]<minT) minT = T[i];
	}
	for(int i = 0; i<iCount; i++)
	{
		T[i] = T[i] - minT;
	}
	return true;
}
UINT AnalyzeZJProc(LPVOID p)
{
	CTargetSingleTestDlg *theDlg =  (CTargetSingleTestDlg *)p;
	theDlg->AnalyzeZJFlag = true;
	double temp,x,y,v,alpha,beta;
	while(1)
	{
		if(!theDlg->AnalyzeZJFlag)break;
		BULLET_DATA bd;
		if(theDlg->m_ZJBulletDataArray.GetCount()<=0)
		{
			Sleep(20);
			continue;
		}

		FlagMutex.Lock();

		bd = theDlg->m_ZJBulletDataArray.GetAt(0);
		theDlg->m_ZJBulletDataArray.RemoveAt(0);

		double T[7];
		T[0] = bd.Tick2[0];
		T[1] = bd.Tick2[1];
		T[2] = bd.Tick2[2];
		T[3] = bd.Tick2[3];
		T[4] = bd.Tick2[4];
		T[5] = bd.Tick2[5];
		T[6] = bd.Tick2[6];
		//T[0] = bd.Tick2[1];
		//T[1] = bd.Tick2[2];
		//T[2] = bd.Tick2[4];
		//T[3] = bd.Tick2[5];
		//T[4] = bd.Tick2[6];

		//T[5] = bd.Tick2[5];
		//T[6] = bd.Tick2[6];
		//NormalT(T,5);

    	temp = 0;
		x = 0;
		y = 0;
		v = 0;
		alpha = 0;
		beta = 0;


//added by xucongdong
//给靶宽增加2000mm来计算
		{
		//如果不在计算的范围内，则判断不是对该靶的射击
			//int iNum = 7;//传感器数量
			bool bFlag = false;
			temp = bd.temperature/10.0;
			theDlg->SaveAddTick("TickData.txt",T,temp);
			bFlag = GetTargetPosByMultiT(theApp.m_BarParamStru[bd.barid-1],theApp.strAlgType, temp, T ,x,y,v,alpha,beta);

			if(!bFlag)
			//if(!GetTargetPosByMultiT(theApp.m_BarParamStru[bd.barid-1],theApp.strAlgType, iNum, bd.temperature, T ,x,y))
			//if(!GetTargetPosByMultiT(theApp.m_BarParamStru[bd.barid-1], bd.temperature, iNum,T ,x,y))
			//if(!GetTargetPosByFiveT(theApp.m_BarParamStru[bd.barid-1], bd.temperature, TempValue1,TempValue2,TempValue3,TempValue4,TempValue5,x,y))
			{
				theDlg->InsertBomb(bd.barid,bd.bulletid,9999,9999,temp,v,alpha,beta);
				continue;
			}
		}

		if((x<(-1)*(theApp.m_BarParamStru[bd.barid-1].Width/2+300))||(x>(theApp.m_BarParamStru[bd.barid-1].Width/2+300))
			||(y>(theApp.m_BarParamStru[bd.barid-1].Height+theApp.m_BarParamStru[bd.barid-1].Bottom+300))||(y<(theApp.m_BarParamStru[bd.barid-1].Bottom-300)))
		{//判断是否在靶的区域内(靶的范围内向外扩300mm)
			//在列表中显示
			double dx,dy;
			dx = x;
			dy = y-theApp.m_BarParamStru[bd.barid-1].Bottom-theApp.m_BarParamStru[bd.barid-1].Height/2;//以靶标中心为原点
			theDlg->InsertBomb(bd.barid,bd.bulletid,dx,dy,temp,v,alpha,beta);
			//无效射击结果记录到数据库中
			bd.direct= 10;//无效
			bd.score = 0;
			bd.xOrg = x;
			bd.yOrg = y;
			continue;
		}
		else
		{//在靶的区域内
			//在列表中显示
			double dx,dy;
		//	theDlg->InsertBomb(bd.barid,bd.bulletid,x,y);//added by xucongdong
			dx = x;
			dy = y-theApp.m_BarParamStru[bd.barid-1].Bottom-theApp.m_BarParamStru[bd.barid-1].Height/2;//以靶标中心为原点
			theDlg->InsertBomb(bd.barid,bd.bulletid,dx,dy,temp,v,alpha,beta);//added by xucongdong
			//计算射击结果并标绘
			int direct;
			int score = (int)(theDlg->ProcessOrientZJ(bd.barid,bd.bulletid,dx,dy,direct,theApp.m_BarType));
			//射击结果记录到数据库中
			bd.direct= direct;
			bd.score = score;
			bd.xOrg = x;
			bd.yOrg = y;

			Sleep(10);
		}

		FlagMutex.Unlock();
	}

	return 1;
}


float CTargetSingleTestDlg::ProcessOrientZJ(int barID, int bulletID, double x, double y, int& direct, int type)
{
	double xInBa;
	double yInBa;

	double xRatio;
	double yRatio;
	bool bExist = false;
	
	if(theApp.m_BarType!=type)
	{
		AfxMessageBox(_T("靶型的配置信息有误,请重新配置！"));
		return -1;
	}
	y = y+theApp.m_BarParamStru[barID-1].Bottom+theApp.m_BarParamStru[barID-1].Height/2;//由相对于靶标中心点的坐标转换为以部署的传感器中心点为原点的坐标
	
	xInBa = (x-theApp.m_BarParamStru[barID-1].Left);
	yInBa = (y-theApp.m_BarParamStru[barID-1].Bottom);
	xRatio = xInBa/theApp.m_BarParamStru[barID-1].Width;
	yRatio = yInBa/theApp.m_BarParamStru[barID-1].Height;

	if(barID>theApp.m_BarCount||barID<=0)
	{
		CString strTemp;
		strTemp.Format(_T("靶位号为：%d，超出显示靶位数\n"),barID);
		AfxMessageBox(strTemp);
		return 0;
	}


	m_BaStatic[barID-1].SetShotPos(bulletID,xRatio,yRatio);
	m_BaStatic[barID-1].Invalidate();
	
	direct = m_BaStatic[barID-1].GetDirect(m_BaStatic[barID-1].GetShotCount()-1);

	float ___score = m_BaStatic[barID-1].GetCurRoq();

	return ___score;
	return 1.0;
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTargetSingleTestDlg 对话框




CTargetSingleTestDlg::CTargetSingleTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTargetSingleTestDlg::IDD, pParent)
	, m_iShellDiameter(0)
	, m_strAlgType(_T(""))
	, m_iVolt(0)
	, m_iAmplification(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTargetSingleTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TARGETSINGLETEST_LIST, m_BombList);
	DDX_Control(pDX, IDC_STATIC_TARGETSINGLETEST_PICTURE, m_Picture);
	DDX_Control(pDX, IDC_EDIT_TARGETSINGLETEST_REVTEXT, m_conRevText);
	DDX_Text(pDX, IDC_EDIT_TARGETSINGLETEST_REVTEXT, m_strRevText);
	DDX_Text(pDX, IDC_EDIT_TARGETSINGLETEST_SHELLDIAMETER, m_iShellDiameter);
	DDX_Control(pDX, IDC_COMBO_TARGETSINGLETEST_ALGORITHMTYPE, m_comAlgType);
	DDX_CBString(pDX, IDC_COMBO_TARGETSINGLETEST_ALGORITHMTYPE, m_strAlgType);
	DDX_Text(pDX, IDC_EDIT_TARGETSINGLETEST_VOLT, m_iVolt);
	DDX_Text(pDX, IDC_EDIT_TARGETSINGLETEST_AMPLIFICATION, m_iAmplification);
}

BEGIN_MESSAGE_MAP(CTargetSingleTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_BARPARAM, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestBarparam)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_OPENCOM, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestOpencom)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_CLOSECOM, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestClosecom)
	ON_MESSAGE(WM_COMM_RXCHAR, OnCommunication)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_START, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestStart)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_END, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestEnd)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_TEST, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestTest)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_RESET, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestReset)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_SHELLTYPE, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestShelltype)
	ON_BN_CLICKED(IDOK, &CTargetSingleTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_SAVE, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSave)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_BEGINALGORITHM, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestBeginalgorithm)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_ENDALGORITHM, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestEndalgorithm)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_RESENDDATA, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestResenddata)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_READVOLT, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestReadvolt)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_WRITEVOLT, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestWritevolt)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_SETAMPLIFICATION, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSetamplification)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_SAVESRCDATA, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSavesrcdata)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_SENSORDETECT, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSensordetect)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_STARTSEND, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestStartsend)
	ON_BN_CLICKED(IDC_BUN_TARGETSINGLETEST_ENDSEND, &CTargetSingleTestDlg::OnBnClickedBunTargetsingletestEndsend)
END_MESSAGE_MAP()


// CTargetSingleTestDlg 消息处理程序

BOOL CTargetSingleTestDlg::OnInitDialog()
{
    //USES_CONVERSION;  // 只需要调用一次，就可以在函数中进行多次转换

	//initCondors();
	CDialogEx::OnInitDialog();
	InitBombList();
	GetDlgItem(IDC_BUN_TARGETSINGLETEST_OPENCOM)->EnableWindow(true);
	GetDlgItem(IDC_BUN_TARGETSINGLETEST_CLOSECOM)->EnableWindow(false);

	GetDlgItem(IDC_BUN_TARGETSINGLETEST_ENDALGORITHM)->EnableWindow(false);



	portNum = 0;
	memset(m_Buffer_Bar,0,MAX_BUFFER);
	iCount_Bar = 0;
	memset(g_BufferBar_Save,0,MAX_BUFFER);
	m_ZJBulletDataArray.RemoveAll();
	InitShotBaStatic();
	m_strRevText = _T("");

	AnalyzeZJFlag = false;

	InitSensorStatus();
	InitSensitivity();
	RecTargetIDForSensitivity = 0;



	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTargetSingleTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTargetSingleTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTargetSingleTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestBarparam()
{
	CBarParam dlg;
	dlg.DoModal();
}

void CTargetSingleTestDlg::InitBombList(void)
{
	m_BombList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_ONECLICKACTIVATE);
//	m_BombList.SetExtendedStyle(m_List.GetExtendedStyle()| LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_BombList.InsertColumn(0,_T("靶位"),LVCFMT_CENTER,40,0);
	m_BombList.InsertColumn(1,_T("序号"),LVCFMT_CENTER,40,1);
	m_BombList.InsertColumn(2,_T("X"),LVCFMT_CENTER,50,2);
	m_BombList.InsertColumn(3,_T("Y"),LVCFMT_CENTER,50,3);
	m_BombList.InsertColumn(4,_T("SrcX"),LVCFMT_CENTER,50,4);
	m_BombList.InsertColumn(5,_T("SrcY"),LVCFMT_CENTER,50,5);
	m_BombList.InsertColumn(6,_T("温度"),LVCFMT_CENTER,80,6);
	m_BombList.InsertColumn(7,_T("速度"),LVCFMT_CENTER,80,7);
	m_BombList.InsertColumn(8,_T("水平入射角"),LVCFMT_CENTER,80,8);
	m_BombList.InsertColumn(9,_T("俯仰入射角"),LVCFMT_CENTER,80,9);
//	m_List.ShowWindow(SW_HIDE);
}

int CTargetSingleTestDlg::InsertBomb(int barID,int bombID,double X,double Y,double temp,double v,double alpha,double beta)
{
	int iCount = 0;
	CString strTemp;
	iCount = m_BombList.GetItemCount();
	strTemp.Format(_T("%d"),barID);
	m_BombList.InsertItem(iCount,strTemp);
	strTemp.Format(_T("%d"),bombID);
	m_BombList.SetItemText(iCount,1,strTemp);
	strTemp.Format(_T("%d"),(int)X);
	m_BombList.SetItemText(iCount,2,strTemp);
	strTemp.Format(_T("%d"),(int)Y);
	m_BombList.SetItemText(iCount,3,strTemp);

	strTemp.Format(_T("%d"),(int)X);
	m_BombList.SetItemText(iCount,4,strTemp);
	strTemp.Format(_T("%d"),(int)(Y+theApp.m_BarParamStru[barID-1].Bottom + theApp.m_BarParamStru[barID-1].Height/2));
	m_BombList.SetItemText(iCount,5,strTemp);

	strTemp.Format(_T("%0.2f"),temp);
	m_BombList.SetItemText(iCount,6,strTemp);
	strTemp.Format(_T("%0.2f"),v);
	m_BombList.SetItemText(iCount,7,strTemp);
	strTemp.Format(_T("%0.2f"),alpha);
	m_BombList.SetItemText(iCount,8,strTemp);
	strTemp.Format(_T("%0.2f"),beta);
	m_BombList.SetItemText(iCount,9,strTemp);

	return iCount+1;
}

void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestOpencom()
{
	//CSetSerialPort portDlg;
	//portDlg.DoModal();
	CSetSerialPort *dlg;
	dlg=NULL;
	dlg=new CSetSerialPort;
	//dlg->Create(IDD_DLG_SETSERIALPORT,NULL);
	if(dlg->DoModal() == IDOK)
	{
		CommInit(dlg->int1, dlg->int2, dlg->int3, dlg->int4, dlg->int5);
		GetDlgItem(IDC_BUN_TARGETSINGLETEST_OPENCOM)->EnableWindow(false);
		GetDlgItem(IDC_BUN_TARGETSINGLETEST_CLOSECOM)->EnableWindow(true);
		portNum = dlg->int1;
	}

}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestClosecom()
{
	AnalyzeZJFlag = false;
	theApp.m_MsComm.CloseListenTread();
	theApp.m_MsComm.ClosePort();
	Sleep(500);
	GetDlgItem(IDC_BUN_TARGETSINGLETEST_OPENCOM)->EnableWindow(true);
	GetDlgItem(IDC_BUN_TARGETSINGLETEST_CLOSECOM)->EnableWindow(false);
	portNum = 0;
}

void CTargetSingleTestDlg::CommInit(UINT portNo /*= 1*/,UINT baud /*= CBR_9600*/,char parity /*= 'N'*/,
						    UINT databits /*= 8*/, UINT stopsbits /*= 1*/)
{
	theApp.m_MsComm.CloseListenTread();
	theApp.m_MsComm.ClosePort();
	Sleep(500);
	{
		if(!theApp.m_MsComm.InitPort(this,portNo,baud))
			;//MessageBox(strTemp,theApp.m_strAppName);//.InitPort(this,6,38400,'N',8,1);
		if (!theApp.m_MsComm.OpenListenThread());

	}

}

char buff[MAX_BUFFER];
int strLength;
int igIndex = -1;
int igDataLength = -1;
LONG CTargetSingleTestDlg::OnCommunication(WPARAM ch, LPARAM port)
{
	CString strTemp_1;

	int tempport = (int )port;
	if(tempport == portNum)
	{
		m_Buffer_Bar[iCount_Bar]=(BYTE)ch;
		iCount_Bar++;
		if(igIndex == -1)
		{
			if((BYTE)ch == 0xcf)
			{
				igIndex = 0;
			}
		}
		else
		{
			igIndex++;
		}

		if(igIndex == 5)
		{
			igDataLength = (int)ch;
			if((igDataLength < 0)||(igDataLength > 100))
			{
				igIndex = -1;
				igDataLength = -1;
				iCount_Bar=0;
			}
		}
		else if(igIndex == 6+igDataLength)//&&(igDataLength > -1)&&(igDataLength < 100)
		{
			if((BYTE)ch == 0xfc)
			{
				for(int nIndex=0;nIndex<iCount_Bar+1;nIndex++)
				{
					if(m_Buffer_Bar[nIndex]==0xcf)
					{
						if((m_Buffer_Bar[nIndex+1]==0x00)&&(m_Buffer_Bar[nIndex+2]==0x01))
						{
							if((m_Buffer_Bar[nIndex+4]==0x03)&&(AnalyzeZJFlag)&&(computeFlag))//报靶
							{
								memcpy(g_BufferBar_Save,&m_Buffer_Bar[nIndex],iCount_Bar-nIndex);
								ZPAnalyzeData();
							}
							else if(m_Buffer_Bar[nIndex+4]==0x09)//上报传感器状态
							{
								if(RecDataSensorDetectFlag)
								{
									for(auto it = m_SensorStatus.begin(); it!=m_SensorStatus.end();it++)
									{
										if(it->barID == m_Buffer_Bar[nIndex+3]) 
										{
											if(it->sensorID[m_Buffer_Bar[nIndex+6]-1]==0)
											{
												it->sensorID[m_Buffer_Bar[nIndex+6]-1]=1;RecCount=1;
											}
											break;
										}
									}
								}
							}
							else if(m_Buffer_Bar[nIndex+4]==0x0A)//上报传感器灵敏度
							{
								if(RecDataSensitivityFlag)
								{
									m_Sensitivity[m_Buffer_Bar[nIndex+3]-1].Sensitivity=m_Buffer_Bar[nIndex+6];
									RecTargetIDForSensitivity = m_Buffer_Bar[nIndex+3];
								}
							}

							strLength = iCount_Bar;
							memcpy(buff,m_Buffer_Bar,strLength);//报文内容
							AddRevTextASCII(buff, strLength);
							AddRevTextASCII2File("TestData.txt",buff, strLength);
						}
						//iCount_Bar=0;
						break;
					}
				}
			}
			igIndex = -1;
			igDataLength = -1;
			iCount_Bar=0;
		}
		if(iCount_Bar>MAX_BUFFER-2)
		{
			iCount_Bar=0;
		}

	}
	return 0;
}
int m_ValueOld1,m_ValueOld2,m_ValueOld3,m_ValueOld4,m_ValueOld5,m_ValueOld6,m_ValueOld7,m_TOld;
int m_BulletID=0;
void CTargetSingleTestDlg::ZPAnalyzeData()
{
	//ZPAnalyzeDataSeven();
	if((strcmp(theApp.strAlgType,"Seven_Condor") == 0)||(strcmp(theApp.strAlgType,"LSM_GetPos") == 0)||(strcmp(theApp.strAlgType,"DLP_GetPos") == 0)||(strcmp(theApp.strAlgType,"NewFour_Condor") == 0)||(strcmp(theApp.strAlgType,"NewFive_Condor") == 0))
	{
		ZPAnalyzeDataSeven();
	}
	else if((strcmp(theApp.strAlgType,"Four_Condor") == 0)||(strcmp(theApp.strAlgType,"Five_Condor") == 0))
	{
		ZPAnalyzeDataFourFive();
	}

}
void CTargetSingleTestDlg::ZPAnalyzeDataFourFive(void)
{
	BULLET_DATA _ZJBulletData;

	int barId = g_BufferBar_Save[3];//靶号
	int T = g_BufferBar_Save[8];//温度
	int value1,value2,value3,value4,value5;
	int bCount = g_BufferBar_Save[7];//弹数
	for(int nIndex=0;nIndex<bCount;nIndex++)
	{
	//	m_BulletID=nIndex+1;//g_BufferBar_Save[9+10*nIndex+1];
		value1 = g_BufferBar_Save[9+10*nIndex+1]*256+g_BufferBar_Save[9+10*nIndex+2];
		value2 = g_BufferBar_Save[9+10*nIndex+3]*256+g_BufferBar_Save[9+10*nIndex+4];
		value3 = g_BufferBar_Save[9+10*nIndex+5]*256+g_BufferBar_Save[9+10*nIndex+6];
		value4 = g_BufferBar_Save[9+10*nIndex+7]*256+g_BufferBar_Save[9+10*nIndex+8];
		value5 = g_BufferBar_Save[9+10*nIndex+9]*256+g_BufferBar_Save[9+10*nIndex+10];

		if((m_ValueOld1 == value1)&&(m_ValueOld2==value2)&&(m_ValueOld3==value3)&&(m_ValueOld4==value4)&&(m_ValueOld5==value5)&&(m_TOld==T))
		{
			return;
		}
		m_ValueOld1 = value1;
		m_ValueOld2 = value2;
		m_ValueOld3 = value3;
		m_ValueOld4 = value4;
		m_ValueOld5 = value5;
		m_TOld = T;
		int Tick2[10];
		memset(Tick2,0,sizeof(int)*10);
		Tick2[0] = value1;
		Tick2[1] = value2;
		Tick2[2] = value3;
		Tick2[3] = value4;
		Tick2[4] = value5;
		if(barId<=0||barId>theApp.m_BarCount)
			return;
		_ZJBulletData.barid = barId;
		m_ZJBulletID[barId-1]++;
		_ZJBulletData.bulletid = m_ZJBulletID[barId-1];  
		memcpy(_ZJBulletData.Tick2,Tick2,sizeof(int)*10);
		_ZJBulletData.temperature = T;
		m_ZJBulletDataArray.Add(_ZJBulletData);
	}

}

short BytesToShort(UCHAR* arr)
{
	short data = 0;
	if (arr[0] & 0x8000) //负数：先-1，再按位取反
	{
		arr[0] = ~(arr[0] - 1);
		arr[1] = ~(arr[1] - 1);
		data = arr[0] * 256 + arr[1];
		data = -data;
	}
	else
		data = arr[0] * 256 + arr[1];

	return data;
}
int TwoByte2Int(UCHAR g1,UCHAR g2)
{
	//int signFlag = 1;
	int iResult = 0;
	if(g1&0x80)
	{
		//signFlag = -1;
		iResult = -1*((g1-0x80)*256+g2);
	}
	else
	{
		iResult = g1*256+g2;
	}
	return iResult;

}
void CTargetSingleTestDlg::ZPAnalyzeDataSeven(void)
{

	BULLET_DATA _ZJBulletData;

	int barId = g_BufferBar_Save[3];//靶号
	int bCount = g_BufferBar_Save[7];//弹数
	int T = TwoByte2Int(g_BufferBar_Save[8],g_BufferBar_Save[9]);//温度
	//g_BufferBar_Save[10]、g_BufferBar_Save[11]//弹序
	int value1,value2,value3,value4,value5,value6,value7;
	short sValue1,sValue2,sValue3,sValue4,sValue5,sValue6,sValue7;
	for(int nIndex=0;nIndex<bCount;nIndex++)
	{
		value1 = g_BufferBar_Save[11+14*nIndex+1]*256+g_BufferBar_Save[11+14*nIndex+2];
		value2 = g_BufferBar_Save[11+14*nIndex+3]*256+g_BufferBar_Save[11+14*nIndex+4];
		value3 = g_BufferBar_Save[11+14*nIndex+5]*256+g_BufferBar_Save[11+14*nIndex+6];
		value4 = g_BufferBar_Save[11+14*nIndex+7]*256+g_BufferBar_Save[11+14*nIndex+8];
		value5 = g_BufferBar_Save[11+14*nIndex+9]*256+g_BufferBar_Save[11+14*nIndex+10];
		value6 = g_BufferBar_Save[11+14*nIndex+11]*256+g_BufferBar_Save[11+14*nIndex+12];
		value7 = g_BufferBar_Save[11+14*nIndex+13]*256+g_BufferBar_Save[11+14*nIndex+14];

		if((m_ValueOld1 == value1)&&(m_ValueOld2==value2)&&(m_ValueOld3==value3)&&(m_ValueOld4==value4)&&(m_ValueOld5==value5)&&(m_ValueOld6==value6)&&(m_ValueOld7==value7)&&(m_TOld==T))
		{
			return;
		}
		m_ValueOld1 = value1;
		m_ValueOld2 = value2;
		m_ValueOld3 = value3;
		m_ValueOld4 = value4;
		m_ValueOld5 = value5;
		m_ValueOld6 = value6;
		m_ValueOld7 = value7;
		m_TOld = T;
		int Tick2[10];
		memset(Tick2,0,sizeof(int)*10);
		Tick2[0] = value1*0.25;
		Tick2[1] = value2*0.25;
		Tick2[2] = value3*0.25;
		Tick2[3] = value4*0.25;
		Tick2[4] = value5*0.25;
		Tick2[5] = value6*0.25;
		Tick2[6] = value7*0.25;
		if(barId<=0||barId>theApp.m_BarCount)
			return;
		_ZJBulletData.barid = barId;
		m_ZJBulletID[barId-1]++;
		_ZJBulletData.bulletid = m_ZJBulletID[barId-1];  
		memcpy(_ZJBulletData.Tick2,Tick2,sizeof(int)*10);
		_ZJBulletData.temperature = T;
		m_ZJBulletDataArray.Add(_ZJBulletData);
	}

}

void CTargetSingleTestDlg::InitShotBaStatic()
{
	UINT ID = 25000;

	for(int i=SHOTBA_NUM-1; i>=0; i--)
	{
		m_BaStatic[i].Create(NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,CRect(0,100,0,100),this,ID+i);
		m_BaStatic[i].ModifyStyle(0,SS_NOTIFY);
		m_BaStatic[i].SetParentInfo(this,ID_M_STATICCLICK+i);
		m_BaStatic[i].ShowWindow(SW_SHOW);
		m_BaStatic[i].SetBaType(theApp.m_BarType);
		m_BaStatic[i].SetUporDownType(1);
		CString strBarName;
		char strChar[40];
		strBarName.Format(_T("靶%d"),i+1);
//		strcpy(strChar,(LPSTR)(LPCTSTR)strBarName);
		m_BaStatic[i].SetShotBaName((LPSTR)(LPCTSTR)strBarName);
		//用于初始化其它内容
		CRect shotBaRect;
	    GetShotRect(shotBaRect);
		m_BaStatic[i].MoveWindow(&shotBaRect);
		m_BaStatic[i].ShowWindow(SW_HIDE);
		m_BaStatic[i].Invalidate(true);
	}
	if(theApp.m_BarCount>SHOTBA_NUM)
	{
		theApp.m_BarCount=SHOTBA_NUM;
	}
	m_BaStatic[theApp.m_BarID-1].ShowWindow(SW_SHOW);
	m_BaStatic[theApp.m_BarID-1].Invalidate(true);

}

void CTargetSingleTestDlg::GetShotRect(CRect &shotBaRect)
{
	CRect dlgRect;
	this->GetWindowRect(&dlgRect);
	GetDlgItem(IDC_STATIC_TARGETSINGLETEST_PICTURE)->GetWindowRect(&shotBaRect);
	shotBaRect.bottom = shotBaRect.bottom - dlgRect.top;
	shotBaRect.top = shotBaRect.top - dlgRect.top;
	shotBaRect.left = shotBaRect.left - dlgRect.left;
	shotBaRect.right = shotBaRect.right - dlgRect.left;
}

void CTargetSingleTestDlg::ClearBarAndList(void)
{
	for(int i=0; i<theApp.m_BarCount; i++)
	{
	//	m_BaScore[i].ClearData();
		m_BaStatic[i].ClearShotInfo();
	}
	//	_BulletID_Test=1;
//	m_strShotLotName="";
	m_BombList.DeleteAllItems();

	memset(m_ZJBulletID,0,sizeof(int)*STATIC_NUM);
	m_strRevText = "";
	UpdateData(false);

}

void CTargetSingleTestDlg::SaveData()
{

	CArray<BombStru,BombStru&> BombStruArray;
	BombStruArray.RemoveAll();
	BombStru tmp;
	char strTmp[20];
	int istrLength = 10;
	int iCount = m_BombList.GetItemCount();
	if(iCount>0)
	{
		CString strBuffer;
		char buffer[1000];
		memset(buffer,0,1000);
		char strFileName[1000];
		memset(strFileName,0,1000);
		CFileDialog dlg(true,_T("Text Files (*.txt)"),_T(""),OFN_CREATEPROMPT | OFN_PATHMUSTEXIST , _T("Text file(*.txt)|*.txt|All files (*.*)|*.*||"));
		if(dlg.DoModal() != IDOK)return;
		strcpy(strFileName,dlg.GetPathName());
		
		FILE *fp = fopen(strFileName,"w");
		strBuffer.Format("靶标编号	炸点序号	X坐标	Y坐标	SrcX坐标	SrcY坐标	温度	速度	水平入射角	信仰入射角\r\n");
		strcpy(buffer,strBuffer);
		fprintf(fp,buffer);
		int SrcX = 0;
		int SrcY = 0;
		char sTemp[20],sV[20],sAlpha[20],sBeta[20];
		memset(sTemp,0,20);
		memset(sV,0,20);
		memset(sAlpha,0,20);
		memset(sBeta,0,20);
		for(int nIndex = 0; nIndex < iCount; nIndex++)
		{
			m_BombList.GetItemText(nIndex,0,strTmp,istrLength);
			tmp.iBarID = atoi(strTmp);
			m_BombList.GetItemText(nIndex,1,strTmp,istrLength);
			tmp.iBombID = atoi(strTmp);
			m_BombList.GetItemText(nIndex,2,strTmp,istrLength);
			tmp.iX = atoi(strTmp);
			m_BombList.GetItemText(nIndex,3,strTmp,istrLength);
			tmp.iY = atoi(strTmp);
			BombStruArray.Add(tmp);

			m_BombList.GetItemText(nIndex,4,strTmp,istrLength);
			SrcX = atoi(strTmp);
			m_BombList.GetItemText(nIndex,5,strTmp,istrLength);
			SrcY = atoi(strTmp);

			m_BombList.GetItemText(nIndex,6,sTemp,istrLength);
			m_BombList.GetItemText(nIndex,7,sV,istrLength);
			m_BombList.GetItemText(nIndex,8,sAlpha,istrLength);
			m_BombList.GetItemText(nIndex,9,sBeta,istrLength);

			strBuffer.Format("      %d              %d           %d        %d           %d        %d        %s        %s        %s        %s\r\n",tmp.iBarID,tmp.iBombID,tmp.iX,tmp.iY,SrcX,SrcY,sTemp,sV,sAlpha,sBeta);
			strcpy(buffer,strBuffer);
			fprintf(fp,buffer);
		}
		fclose(fp);
	}
	//if(iCount>0)
	//{
	//	CreateExcel_Bomb("BombResult.xlt",BombStruArray);
	//}


//	FILE *fp = fopen("test.txt","a");
////	fprintf(fp,strTotalA.GetBuffer());
//	fclose(fp);

}


void CTargetSingleTestDlg::SaveAddTick(char *strFileName,double Tick[],double temp)
{
		CString strBuffer;
		char buffer[1000];
		memset(buffer,0,1000);
		FILE *fp = fopen(strFileName,"a");
	//	strBuffer.Format("Tick1		Tick2		Tick3		Tick4		Tick5		Tick6		Tick7\r\n");
		strBuffer.Format("%0.2f		%0.2f		%0.2f		%0.2f		%0.2f		%0.2f		%0.2f		%0.2f\r\n",temp,Tick[0],Tick[1],Tick[2],Tick[3],Tick[4],Tick[5],Tick[6]);
		strcpy(buffer,strBuffer);
		fprintf(fp,buffer);
		fclose(fp);
}


void CTargetSingleTestDlg::AddRevTextASCII2File(char *fileName,char *revText, long ilength)
{
	char str[600];
	memset(str,0,600);
	FILE *fp = fopen(fileName,"a");
//	FILE *fp = fopen("test.txt","a");
	int strLength;
	char * mtbuf;
	mtbuf=(char*)revText;
	strLength=ilength;
	char temp_str[5];
	memset(temp_str,0,5);
	byte temp_int;
	char temp_str1[10];
	CString strAscii="";
	for(int ii=0;ii<strLength;ii++)
	{
		temp_int=*(mtbuf+ii);
		memset(temp_str1,0,10);
		if(temp_int<0x10)
		{
			strAscii=strAscii+"0";
			strcpy(temp_str1,"0");
		}
		strAscii=strAscii+itoa(temp_int,temp_str,16)+" ";
//        strcat(temp_str1,temp_str);
//		strcat(temp_str1," ");
//		int revtext_length=m_strRevText.GetLength();
//		m_conRevText.SetSel(revtext_length-1,revtext_length,false);
//		m_conRevText.ReplaceSel(temp_str1,true);
	}
	strAscii=strAscii+"\r\n";
//	AddRevText("\r\n");
	strcpy(str,strAscii);
	
	fprintf(fp,str);
	fclose(fp);

}

void CTargetSingleTestDlg::AddRevTextASCII(char *revText, long ilength)
{
	//	char buff[2048];
	//	char sendstr[50];
	//	memset(buff,0,2048);
	//	memset(sendstr,0,50);
	
	int strLength;
	char * mtbuf;
	mtbuf=(char*)revText;
	strLength=ilength;
	char temp_str[5];
	memset(temp_str,0,5);
	byte temp_int;
	char temp_str1[10];
	for(int ii=0;ii<strLength;ii++)
	{
		//		temp_int=toascii(*(mtbuf+ii));
		temp_int=*(mtbuf+ii);
		memset(temp_str1,0,10);
		if(temp_int<0x10)
		{
			m_strRevText=m_strRevText+"0";
			strcpy(temp_str1,"0");
		}
		m_strRevText=m_strRevText+itoa(temp_int,temp_str,16)+" ";
        strcat(temp_str1,temp_str);
		strcat(temp_str1," ");
		int revtext_length=m_strRevText.GetLength();
		m_conRevText.SetSel(revtext_length-1,revtext_length,false);
		m_conRevText.ReplaceSel(temp_str1,true);
	}
	AddRevText("\r\n");
}
void CTargetSingleTestDlg::AddRevText(CString strText)
{
	long strLength,txtLength;
	CString str;
	txtLength=m_strRevText.GetLength();
	//	str.Format("\r\n%s",strText);
	str.Format("%s",strText);
	strLength=str.GetLength();
	m_strRevText=m_strRevText+str;
	m_conRevText.SetSel(txtLength,txtLength+strLength,false);
	m_conRevText.ReplaceSel(str,true);
	
}


bool CTargetSingleTestDlg::IsMovBar(int barID)
{
	if(m_BaStatic[barID-1].GetBaType()==1)
		return true;
	else
		return false;
}

void CTargetSingleTestDlg::SendDataZJ(int barid,int type, int dataSend)
{
	if((type==0x00)&&(dataSend==0x05))//测试
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x03;
		data[6] = dataSend;
		data[7] = dataSend;
		data[8] = dataSend;
		data[9] = 0xfc;
		theApp.m_MsComm.WriteData(data,10);
	}
	else if(type==0x05)//报靶器设备复位
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x00;
		data[6] = 0xfc;
		theApp.m_MsComm.WriteData(data,7);
	}
	else if((type==0x03)&&(dataSend==0x18))//传感器测试开始
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = dataSend;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(dataSend==0x19))//传感器测试结束
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = dataSend;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if(dataSend==0x14)
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = dataSend;
		data[7] = 0x00;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = dataSend;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}

}


void CTargetSingleTestDlg::SendDataZJ(int barid,int type, short actionCode1,short actionCode2)
{
	if((type==0x03)&&(actionCode1==0x18))//传感器测试开始
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(actionCode1==0x19))//传感器测试结束
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}	
	else if((type==0x03)&&(actionCode1==0x1A))//设置探头灵敏度(比较器电压)
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else if((type==0x03)&&(actionCode1==0x1B))//读取探头灵敏度(比较器电压)
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(actionCode1==0x1C))//设备复位
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(actionCode1==0x1D))//设置弹种
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else if((type==0x03)&&(actionCode1==0x1E))//设置放大倍数
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else if((type==0x03)&&(actionCode1==0x20))//重新上报数据
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}

	else if((type==0x01)&&(actionCode1==0x14))//灯光控制
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x01;
		data[2] = 0x08;//装甲起倒靶
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
}



void CTargetSingleTestDlg::SendDataMov(int barid,int type, int dataSend)
{
	if((type==0x00)&&(dataSend==0x05))//测试
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x03;
		data[6] = dataSend;
		data[7] = dataSend;
		data[8] = dataSend;
		data[9] = 0xfc;
		theApp.m_MsComm.WriteData(data,10);
	}
	else if(type==0x05)//报靶器设备复位
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x00;
		data[6] = 0xfc;
		theApp.m_MsComm.WriteData(data,7);
	}
	else if((type==0x03)&&(dataSend==0x18))//传感器测试开始
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = dataSend;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(dataSend==0x19))//传感器测试结束
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = dataSend;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if(dataSend==0x14)
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = dataSend;
		data[7] = 0x00;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = dataSend;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}

}


void CTargetSingleTestDlg::SendDataMov(int barid,int type, short actionCode1,short actionCode2)
{
	if((type==0x03)&&(actionCode1==0x18))//传感器测试开始
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(actionCode1==0x19))//传感器测试结束
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}	
	else if((type==0x03)&&(actionCode1==0x1A))//设置探头灵敏度(比较器电压)
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else if((type==0x03)&&(actionCode1==0x1B))//读取探头灵敏度(比较器电压)
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(actionCode1==0x1C))//设备复位
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}
	else if((type==0x03)&&(actionCode1==0x1D))//设置弹种
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else if((type==0x03)&&(actionCode1==0x1E))//设置放大倍数
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else if((type==0x03)&&(actionCode1==0x20))//重新上报数据
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x01;
		data[6] = actionCode1;
		data[7] = 0xfc;
		theApp.m_MsComm.WriteData(data,8);
	}

	else if((type==0x01)&&(actionCode1==0x14))//灯光控制
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
	else
	{
		BYTE data[20];
		data[0] = 0xcf;
		data[1] = 0x02;
		data[2] = 0x02;
		data[3] = barid;
		data[4] = type;
		data[5] = 0x02;
		data[6] = actionCode1;
		data[7] = actionCode2;
		data[8] = 0xfc;
		theApp.m_MsComm.WriteData(data,9);
	}
}



void CTargetSingleTestDlg::OnBnClickedOk()
{
	//uninitCondors();
	CDialogEx::OnOK();
}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSave()
{
	SaveData();
}



void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestBeginalgorithm()
{
	UpdateData(true);
	strcpy(theApp.strAlgType,m_strAlgType.GetString());
	if(initCondors(m_strAlgType.GetString()))
	{
		GetDlgItem(IDC_COMBO_TARGETSINGLETEST_ALGORITHMTYPE)->EnableWindow(false);
		GetDlgItem(IDC_BUN_TARGETSINGLETEST_BEGINALGORITHM)->EnableWindow(false);
		GetDlgItem(IDC_BUN_TARGETSINGLETEST_ENDALGORITHM)->EnableWindow(true);
	}
}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestEndalgorithm()
{
	uninitCondors(m_strAlgType.GetString());
	GetDlgItem(IDC_COMBO_TARGETSINGLETEST_ALGORITHMTYPE)->EnableWindow(true);
	GetDlgItem(IDC_BUN_TARGETSINGLETEST_BEGINALGORITHM)->EnableWindow(true);
	GetDlgItem(IDC_BUN_TARGETSINGLETEST_ENDALGORITHM)->EnableWindow(false);

}



void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestStart()
{
	if(AfxMessageBox("确定开始接收报靶吗?",MB_YESNO) == IDYES)
	{
		AnalyzeZJFlag = false;
		Sleep(500);
		m_ZJBulletDataArray.RemoveAll();
		m_Thread_Ananlyze = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AnalyzeZJProc,this,CREATE_SUSPENDED,&ThreadID);
		ResumeThread(m_Thread_Ananlyze);
		ClearBarAndList();
		//SendDataZJ(theApp.m_BarID,0x03, 0x16);
	}
}

void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestEnd()
{
	if(AfxMessageBox("确定停止接收报靶吗?",MB_YESNO) == IDYES)
	{
		//SendDataZJ(theApp.m_BarID,0x03, 0x17);
		AnalyzeZJFlag = false;
		Sleep(100);
	}
}

void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestTest()
{
	//if(AfxMessageBox("确定发送测试指令吗?",MB_YESNO) == IDYES)
	{
		SendDataZJ(theApp.m_BarID,0x00, 0x05);
	}
}



void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestShelltype()
{
	UpdateData(true);
	short sShellDia = 0;
	sShellDia = m_iShellDiameter;
	SendDataZJ(theApp.m_BarID,0x03, 0x1D, sShellDia);
}

void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestReadvolt()
{
	short sSolt = 0;
	SendDataZJ(theApp.m_BarID,0x03, 0x1B, sSolt);
	//m_iVolt = sSolt;
	UpdateData(false);

}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestWritevolt()
{
	////设置比较器电压
	//UpdateData(true);
	//short sSolt = 0;
	//sSolt = m_iVolt;
	//SendDataZJ(theApp.m_BarID,0x03, 0x1A, sSolt);

	//设置探头灵敏度
	g_pSensitivity=NULL;
	Sensitivity_Rev();

}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSetamplification()
{
	UpdateData(true);
	short sAmpli = 0;
	sAmpli = m_iAmplification;
	SendDataZJ(theApp.m_BarID,0x03, 0x1E, sAmpli);
}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestResenddata()
{
	if(AfxMessageBox("确定重新上报数据吗?",MB_YESNO) == IDYES)
	{
			//重新上报数据
		short sTmp = 0;
		SendDataZJ(theApp.m_BarID,0x03, 0x20, sTmp);
	}

}

void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestReset()
{
	if(AfxMessageBox("确定设备复位吗?",MB_YESNO) == IDYES)
	{
		short sTmp = 0;
		SendDataZJ(theApp.m_BarID,0x03, 0x1C, sTmp);
	}
}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSavesrcdata()
{
	UpdateData(true);
	char strFileName[800];
	memset(strFileName,0,800);
	CFileDialog dlg(true,_T("Text Files (*.txt)"),_T(""),OFN_CREATEPROMPT | OFN_PATHMUSTEXIST , _T("Text file(*.txt)|*.txt|All files (*.*)|*.*||"));
	if(dlg.DoModal() != IDOK)return;
	strcpy(strFileName,dlg.GetPathName());
	FILE *fp = fopen(strFileName,"w");
	//FILE *fp = fopen("TestSrcData.txt","a");
	fprintf(fp,m_strRevText.GetBuffer());
	fclose(fp);
}

void TargetDetect_Rev()
{
	if(g_pTargetDetect==NULL)
	{
	//	CTargetDetectDlg *g_pTargetDetect;
	
		g_pTargetDetect=new CTargetDetectDlg;
		g_pTargetDetect->Create(IDD_TARGETDETECTDLG,NULL);
		//g_hTargetDetect=dlg->GetSafeHwnd();
		g_pTargetDetect->CenterWindow();
		g_pTargetDetect->ShowWindow(1);
		g_pTargetDetect->RefreshListCtrl();
	}
	else
	{
//		CTargetDetectDlg *dlg;
//		g_pTargetDetect=(CTargetDetectDlg*)CWnd::FromHandle(g_hTargetDetect);
		g_pTargetDetect->RefreshListCtrl();
	}
}

void Sensitivity_Rev()
{
	int tmpTarID = 0;
	tmpTarID = ((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecTargetIDForSensitivity;
	if(g_pSensitivity==NULL)
	{
		g_pSensitivity=new CSensitivityDlg;
		g_pSensitivity->Create(IDD_SENSITIVITYDLG,NULL);
		g_pSensitivity->CenterWindow();
		g_pSensitivity->ShowWindow(1);	
		if(tmpTarID>0)
		{
			g_pSensitivity->SetStrVoltage(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_Sensitivity[tmpTarID-1].Sensitivity);
		}
	}
	else
	{
		if(tmpTarID>0)
		{
			g_pSensitivity->SetStrVoltage(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->m_Sensitivity[tmpTarID-1].Sensitivity);
		}
	
	}
}


void CTargetSingleTestDlg::InitSensorStatus(void)
{
	RecDataSensorDetectFlag=false;
	m_SensorStatus.clear();
	for(int i=0;i<theApp.m_BarCount;i++)
	{
		SensorStatusStru tmp;
		tmp.barID = i+1;
//		tmp.commStatus = 0;
		tmp.sensorID[0] = 0;
		tmp.sensorID[1] = 0;
		tmp.sensorID[2] = 0;
		tmp.sensorID[3] = 0;
		tmp.sensorID[4] = 0;
		tmp.sensorID[5] = 0;
		tmp.sensorID[6] = 0;
		m_SensorStatus.push_back(tmp);
	}
}
void CTargetSingleTestDlg::InitSensitivity(void)
{
	RecDataSensitivityFlag=false;
	m_Sensitivity.clear();
	for(int i=0;i<theApp.m_BarCount;i++)
	{
		SensitivityStru tmp;
		tmp.barID = i+1;
		tmp.Sensitivity = 0x0D;
		m_Sensitivity.push_back(tmp);
	}
}



UINT RecDataForSensorDetectProc(LPVOID p)
{
	int i=0;
	CTargetSingleTestDlg *theDlg =  (CTargetSingleTestDlg *)p;
	theDlg->RecDataSensorDetectFlag=true;
	while(theDlg->RecDataSensorDetectFlag)
	{
		if(((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecCount)
		{
			FlagMutexSensorDetect.Lock();
			TargetDetect_Rev();
			((CTargetSingleTestDlg*)(theApp.m_pMainWnd))->RecCount=0;
			FlagMutexSensorDetect.Unlock();
		}

		Sleep(100);//0.1秒检测一次
		if(!theDlg->RecDataSensorDetectFlag)break;
	}
 	return 0;
}

UINT RecDataForSensitivityProc(LPVOID p)
{
	int i=0;
	CTargetSingleTestDlg *theDlg =  (CTargetSingleTestDlg *)p;
	theDlg->RecDataSensitivityFlag=true;
	while(theDlg->RecDataSensitivityFlag)
	{
		if(theDlg->RecTargetIDForSensitivity>0)
		{
			Sensitivity_Rev();
			theDlg->RecDataSensitivityFlag = false;
			theDlg->RecTargetIDForSensitivity=0;
		}
		else
		{
			Sleep(100);//0.1秒检测一次
		}
	
	}
 	return 0;
}

void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestSensordetect()
{
	//报靶器检测
	g_pTargetDetect=NULL;
	TargetDetect_Rev();
}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestStartsend()
{
	if(AfxMessageBox("确定发送开始报靶指令吗?",MB_YESNO) == IDYES)
	{
		SendDataZJ(theApp.m_BarID,0x03, 0x16);
	}
}


void CTargetSingleTestDlg::OnBnClickedBunTargetsingletestEndsend()
{
	if(AfxMessageBox("确定发送停止报靶指令吗?",MB_YESNO) == IDYES)
	{
		SendDataZJ(theApp.m_BarID,0x03, 0x17);
	}

}
