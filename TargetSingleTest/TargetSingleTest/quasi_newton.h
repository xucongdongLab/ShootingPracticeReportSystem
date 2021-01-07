#ifndef QUASI_NEWTON_N
#define QUASI_NEWTON_N
#include "stdafx.h"
#include <math.h>

//void ff(double [],double [],int);
//void getVisionCosin(double uav_hx,double uavGes[],double eoLod[],double r[]);//����ת̨�����߷�������
//double getPlatGesFunction(double x[],double rPre[],double uav_hx,double uavGes[]);//������ת̨��̬�Ǻ���
double  myfun(double Xk[], double x[], double y[], double z[],  double c, int N_M, double d_T_n[], int Mb);//���ⱨ�н���Ŀ�꺯�� 
//void comput_grad(int n,double *point,double *grad,double rPre[],double uav_hx,double uavGes[]);//DFP�����ݶȺ���
void comput_grad(int n, double *point, double *grad, double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb);//DFP�����ݶȺ���
//double line_search1(double (*pf)(double *x),int n,double *start,double *direction); //DFP������������1
//double line_search(int n,double *start,double *direction,double rPre[],double uav_hx,double uavGes[]);//DFP������������2
double line_search(int n, double *start, double *direction, double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb);//DFP������������2
//double DFP(int n, double *min_point,double rPre[],double uav_hx,double uavGes[]);//DFP������
double DFP(int n, double *min_point, double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb);//DFP������
double d_comp(double Xk[], double Xb[]);//���㵯���߷������
double l_comp(double Xj[], double Xb[], double Xk[]);//�����ص����ߴ���������

#endif