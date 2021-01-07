#ifndef QUASI_NEWTON_N
#define QUASI_NEWTON_N
#include "stdafx.h"
#include <math.h>

//void ff(double [],double [],int);
//void getVisionCosin(double uav_hx,double uavGes[],double eoLod[],double r[]);//计算转台视轴线方向余弦
//double getPlatGesFunction(double x[],double rPre[],double uav_hx,double uavGes[]);//计算光电转台姿态角函数
double  myfun(double Xk[], double x[], double y[], double z[],  double c, int N_M, double d_T_n[], int Mb);//激光报靶解算目标函数 
//void comput_grad(int n,double *point,double *grad,double rPre[],double uav_hx,double uavGes[]);//DFP计算梯度函数
void comput_grad(int n, double *point, double *grad, double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb);//DFP计算梯度函数
//double line_search1(double (*pf)(double *x),int n,double *start,double *direction); //DFP线性搜索函数1
//double line_search(int n,double *start,double *direction,double rPre[],double uav_hx,double uavGes[]);//DFP线性搜索函数2
double line_search(int n, double *start, double *direction, double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb);//DFP线性搜索函数2
//double DFP(int n, double *min_point,double rPre[],double uav_hx,double uavGes[]);//DFP主函数
double DFP(int n, double *min_point, double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb);//DFP主函数
double d_comp(double Xk[], double Xb[]);//计算弹道线法向距离
double l_comp(double Xj[], double Xb[], double Xk[]);//计算沿弹道线传感器距离

#endif