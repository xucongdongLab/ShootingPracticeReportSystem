#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "quasi_newton.h"
//#include "stdafx.h"
//#include "qmath.h"
#define MAXITER 50
#define THRESHOLD 1e-10
#define eps 2.2204e-16//定义小正数

//using namespace std;
double d_comp(double Xk[], double Xb[])
{
	//////////////////////////////////////////////////////
	//计算传感器至弹道线的法向距离
	//Xk为解算坐标量
	//Xb为传感器坐标
	//弹着点解算坐标

	double	xp = Xk[0];
	double yp = Xk[1];
	//弹速解算量
    //double v = Xk[2];
	//%水平入射角解算量
	double	alpha = Xk[3];
	//垂直入射角解算量
	double beta = Xk[4];
	//计算入射角的正余弦值
	double sin_alpha = sin(alpha);
	double cos_alpha = cos(alpha);
	double sin_beta = sin(beta);
	double cos_beta = cos(beta);
	//声速
    //double c = 347.85;
	//传感器坐标
	double xi = Xb[0];
	double yi = Xb[1];
	double zi = Xb[2];
	//计算传感器至弹道线法向距离
	double dist = sqrt(
		((xp - xi) * cos_alpha - zi * sin_alpha)*((xp - xi) * cos_alpha - zi * sin_alpha)
		+ ((xp - xi)*sin_alpha*sin_beta - (yp - yi)*cos_beta + zi * cos_alpha*sin_beta)
		*((xp - xi)*sin_alpha*sin_beta - (yp - yi)*cos_beta + zi * cos_alpha*sin_beta)
	);
	return dist;

}

double l_comp(double Xj[], double Xb[], double Xk[])
{
	//////////////////////////////////////////////////////////////////////////////////////////
	//功能：计算基于当前解算坐标数据的测量传感器Mj至基准传感器Mb沿弹道线方向的距离差
	//Xb为基准传感器坐标，Xj为测量传感器坐标，Xk为当前解算量，主要使用估算的水平和垂直入射角
	//////////////////////////////////////////////////////////////////////////////////////////
    //double xp = Xk[0];
    //double yp = Xk[1];
	//弹速解算量
    //double v = Xk[2];
	//水平入射角解算量
	double alpha = Xk[3];
	//垂直入射角解算量
	double beta = Xk[4];
	//计算入射角的正余弦值
	double sin_alpha = sin(alpha);
	double cos_alpha = cos(alpha);
	double sin_beta = sin(beta);
	double cos_beta = cos(beta);
	//声速
    //double c = 347.85;
	//基准传感器Mi坐标
	double xi = Xb[0];
	double yi = Xb[1];
	double zi = Xb[2];
	//测量传感器Mj坐标
	double xj = Xj[0];
	double yj = Xj[1];
	double zj = Xj[2];
	//计算沿弹道线方向测量传感器至基准传感器的距离差
	double d_L = (zj - zi) * cos_alpha * cos_beta - (xj - xi) * sin_alpha * cos_beta - (yj - yi) * sin_beta;
	return d_L;
}

double  myfun(double Xk[], double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb)
{
	///////////////////////////////////////////////////////////////////
	//功能：解算未知量的目标函数
	//	Xk为未知量解算函数，x, y, z为传感器坐标，N_M为传感器数量，
	//	d_T_n为噪声化时差数据，Mb为基准传感器序数
	/////////////////////////////////////////////////////////////////
	int i;
	//double c = 347.85;//声速
	double X[3]; //传感器坐标向量：x y z 
	//  double eps = 2.220446049250313e-016;//小的正常数
	//计算各传感器至弹道线的法向距离
	//double * d = new double(N_M);//%各传感器至弹道线法向距离变量
	double d[7];
	for (i = 0; i < N_M; i++)
	{
		// 测量传感器坐标
		X[0] = xx[i];
		X[1] = yy[i];
		X[2] = zz[i];
		//解算各传感器至弹道线的法向距离
		d[i] = d_comp(Xk, X);

	}

	//计算各传感器沿弹道线方向至基准传感器的距离差
	//double* d_L = new double(N_M);
	double d_L[7];
	//基准传感器
	double Xb[3];
	Xb[0] = xx[Mb - 1];
	Xb[1] = yy[Mb - 1];
	Xb[2] = zz[Mb - 1];

	for (i = 0; i < N_M; i++)
	{
		// 测量传感器坐标
		X[0] = xx[i];
		X[1] = yy[i];
		X[2] = zz[i];
		//计算各测量传感器至基准传感器沿弹道线的距离差
		d_L[i] = l_comp(X, Xb, Xk);

	}
	//计算方程系数
	double v = Xk[2];
	
	//计算目标函数中的系数
    double K = 1000.0*v * c / sqrt(1000.0*v*1000.0*v - c * c);

	//计算解算目标函数值
	//double*  F = new double(N_M - 1);
	double F[6];

	int ind = 0;//目标函数序数
	double dd = d[Mb - 1];//至基准传感器的距离

	for (i = 0; i < N_M; i++)
	{
		if (i != (Mb - 1)) //若不是基准传感器，则计算目标函数差
		{
            F[ind] = d[i] - dd - K * (d_T_n[i] + d_L[i] / (1000.0*v+eps));
			ind++;
		}

	}
	//统计目标函数误差
	double error = 0;//返回值误差
	for (i = 0; i < (N_M - 1); i++)
		error += F[i] * F[i];
	   
	//返回误差值

	return sqrt(error);

}
//void comput_grad(int n,double *point,double *grad,double rPre[],double uav_hx,double uavGes[])
void comput_grad(int n, double *point, double *grad, double xx[], double yy[], double zz[], double c, int N_M, double d_T_n[], int Mb)
{
	/////////////////////////////////////////////////////////////////////////////////////
	//函数功能:DFP函数梯度计算函数
	//输入参数:n为维数(方程组)
	//        *point:迭代起点
	//        *grad:梯度
	//        Xk:解算量：靶面坐标(x,y) 弹速v 水平入射角α 垂直入射角β 
	//        x y z :为传感器 坐标向量
	//        N_M 为传感器数量
	//        d_T_n:为时差误差测量值
	//        Mb 为基准传感器序数
	//输出参数:无
	double h = 1E-3;//
	int i;
	double *temp;
	temp = new double[n];
	double grd;
	for (i = 1; i <= n; i++)
	{
		temp[i - 1] = point[i - 1];
	}
	for (i = 1; i <= n; i++)
	{
		temp[i - 1] += 0.5*h;
		//grd =getPlatGesFunction(temp,rPre,uav_hx,uavGes);
		grd = myfun(temp, xx, yy, zz, c, N_M, d_T_n, Mb);
		//grad[i-1]=4*pf(temp)/(3*h);//PF调用
		grad[i - 1] = 4 * grd / (3 * h);//PF调用
		temp[i - 1] -= h;
		//grad[i-1]-=4*pf(temp)/(3*h);//PF调用
		// grd =getPlatGesFunction(temp,rPre,uav_hx,uavGes);
		grd = myfun(temp, xx, yy, zz, c, N_M, d_T_n, Mb);
		grad[i - 1] -= 4 * grd / (3 * h);//PF调用
		temp[i - 1] += (3 * h / 2);
		//grad[i-1]-=(pf(temp)/(6*h));//PF调用
		// grd =getPlatGesFunction(temp,rPre,uav_hx,uavGes);
		grd = myfun(temp, xx, yy, zz, c, N_M, d_T_n, Mb);
		grad[i - 1] -= (grd / (6 * h));//PF调用
		temp[i - 1] -= (2 * h);
		//grad[i-1]+=(pf(temp)/(6*h));//PF调用
		// grd =getPlatGesFunction(temp,rPre,uav_hx,uavGes);
		grd = myfun(temp, xx, yy, zz, c, N_M, d_T_n, Mb);
		grad[i - 1] += (grd / (6 * h));//PF调用
		temp[i - 1] = point[i - 1];
	}
	delete[] temp;
}

//double line_search(double (*pf)(double *x),int n,double *start,double *direction)
// double line_search(int n,double *start,double *direction,double rPre[],double uav_hx,double uavGes[])
double line_search(int n, double *start, double *direction, double xx[], double yy[], double zz[], double c, int N_M, double d_T_n[], int Mb)
{
	/////////////////////////////////////////////////////
	//函数功能:DFP线性搜索函数
	//输入参数:n为维数(方程组)
	//start:搜索初值
	//direction:搜索方向
	//rPre:视轴线方向余
	//uav_hx:无人机航向角
	//uavGes:无人机姿态角
	//输出参数:最优步长
	int i;
	int   iterstep = 0;
	double step = 0.1;
	double a = 0, value_a, diver_a;
	double b, value_b, diver_b;
	double t, value_t, diver_t;
	double s, z, w;
	double *grad, *temp_point;
	grad = new double[n];
	temp_point = new double[n];
	//comput_grad(n,start,grad,rPre,uav_hx,uavGes);//计算梯度(调用PF)
	comput_grad(n, start, grad, xx, yy, zz, c, N_M, d_T_n, Mb);//计算梯度(调用PF)
	diver_a = 0;
	for (i = 1; i <= n; i++)
		diver_a = diver_a + grad[i - 1] * direction[i - 1];
	do
	{
		b = a + step;
		for (i = 1; i <= n; i++)
			temp_point[i - 1] = start[i - 1] + b * direction[i - 1];
		//comput_grad(pf,n,temp_point,grad);//计算梯度(调用PF)
		// comput_grad(n,temp_point,grad,rPre,uav_hx,uavGes);//计算梯度(调用PF)
		comput_grad(n, temp_point, grad, xx, yy, zz, c, N_M, d_T_n, Mb);//计算梯度(调用PF)
		diver_b = 0;
		for (i = 1; i <= n; i++)
			diver_b = diver_b + grad[i - 1] * direction[i - 1];
		if (fabs(diver_b) < THRESHOLD) //原来0.000001
		{
			delete[] grad;
			delete[] temp_point;
			return(b);
		}
		if (diver_b < -THRESHOLD)//原来0.0000001
		{
			a = b;
			diver_a = diver_b;
			step = 2 * step;
		}
	} while (diver_b <= THRESHOLD);//原来0.000001

	for (i = 1; i <= n; i++)
		temp_point[i - 1] = start[i - 1] + a * direction[i - 1];
	//value_a=(*pf)(temp_point);//PF调用
	//value_a = getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);
	value_a = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);

	for (i = 1; i <= n; i++)
		temp_point[i - 1] = start[i - 1] + b * direction[i - 1];
	//value_b=(*pf)(temp_point);//PF调用
	//value_b=getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);//PF调用
	value_b = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);

	do
	{
		iterstep++;
		s = 3 * (value_b - value_a) / (b - a);
		z = s - diver_a - diver_b;
		w = sqrt(fabs(z*z - diver_a * diver_b));
		t = a + (w - z - diver_a)*(b - a) / (diver_b - diver_a + 2 * w);
		//value_b=(*pf)(temp_point);//PF调用
		// value_b=getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);//PF调用
		value_b = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);//PF调用
		for (i = 1; i <= n; i++)
			temp_point[i - 1] = start[i - 1] + t * direction[i - 1];
		// value_t=(*pf)(temp_point);//PF调用
		// value_t=getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);//PF调用
		value_t = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);//PF调用
		//comput_grad(pf,n,temp_point,grad);//PF调用
		//  comput_grad(n,temp_point,grad,rPre,uav_hx,uavGes);//计算梯度(调用PF)
		comput_grad(n, temp_point, grad, xx, yy, zz, c, N_M, d_T_n, Mb);//计算梯度(调用PF)
		diver_t = 0;
		for (i = 1; i <= n; i++)
			diver_t = diver_t + grad[i - 1] * direction[i - 1];
		if (diver_t > 1E-5)
		{
			b = t;
			value_b = value_t;
			diver_b = diver_t;
		}
		else if (diver_t < -1E-5)
		{
			a = t;
			value_a = value_t;
			diver_a = diver_t;
		}
		else break;
	} while (((fabs(diver_t) >= THRESHOLD) && (fabs(b - a) > THRESHOLD)) || (iterstep <= MAXITER));//原来0.00001

	delete[] grad;
	delete[] temp_point;
	return(t);

}
//无约束变尺度法DFP函数声明
//
//参数：pf指向目标函数的指针，n变量个数，min_point接受初始点、存放结果
//返回：极小点处函数值
//
//double DFP(int n,double *min_point,double rPre[],double uav_hx,double uavGes[])
double DFP(int n, double *min_point, double xx[], double yy[], double zz[], double c, int N_M, double d_T_n[], int Mb)
{
	////////////////////////////////////////////////////////////
	//函数功能:DFP主程序
	//输入参数:n为维数(方程组)
	//min_point:迭代初始(最优值)
	//rPre:视轴线方向余弦
	//uav_hx:飞机方向余弦
	//uavGes:飞机姿态角
	//输出参数:极值时的方向余弦误差

	int i, j;
	int iterstep = 0;//迭代计数器
	int k = 0;
	double e = 1E-5;
	double g_norm;

	double *g0 = new double[n];                //梯度
	double *g1 = new double[n];
	double *dg = new double[n];

	double *p = new double[n];                //搜索方向 =-g
	double t;                                                //一维搜索步长

	double *x0 = new double[n];
	double *x1 = new double[n];
	double *dx = new double[n];

	double **H = new double*[n];
	for (i = 0; i < n; i++)                H[i] = new double[n];

	double **tempH = new double*[n];
	for (i = 0; i < n; i++)                tempH[i] = new double[n];

	double *gH = new double[n];
	double *Hg = new double[n];
	double num1;
	double num2;



	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
		{
			if (i == j)        H[i][j] = 1.0;        // H0=I
			else        H[i][j] = 0.0;
			tempH[i][j] = 0.0;
		}


	for (i = 0; i < n; i++)
		x0[i] = min_point[i];

	//comput_grad(pf,n,x0,g0);//计算梯度(调用PF)
	//comput_grad(n,x0,g0,rPre,uav_hx,uavGes);//计算梯度(调用PF)
	comput_grad(n, x0, g0, xx, yy, zz, c, N_M, d_T_n, Mb);//计算梯度(调用PF)
	g_norm = 0.0;
	for (i = 0; i < n; i++)        g_norm = g_norm + g0[i] * g0[i];
	g_norm = sqrt(g_norm);
	if (g_norm < e)
	{
		for (i = 0; i < n; i++)        min_point[i] = x0[i];//PF调用

		delete[] g0;
		delete[] g1;
		delete[] dg;
		delete[] p;
		delete[] x0;
		delete[] x1;
		delete[] dx;
		for (i = 0; i < n; i++)                delete[] H[i];
		delete[]H;
		for (i = 0; i < n; i++)                delete[] tempH[i];
		delete[]tempH;
		delete[] gH;
		delete[] Hg;
		//return getPlatGesFunction(min_point,rPre,uav_hx,uavGes);
		//return pf(min_point);//PF调用
		return myfun(min_point, xx, yy, zz,  c, N_M, d_T_n, Mb);
	}

	for (i = 0; i < n; i++)        p[i] = -g0[i];

	do
	{
		//  t=line_search(pf,n,x0,p);                                

		//t = line_search(n,x0,p,rPre,uav_hx,uavGes);//飞机姿态角)
		t = line_search(n, x0, p, xx, yy, zz,  c, N_M, d_T_n, Mb);
		for (i = 0; i < n; i++)        x1[i] = x0[i] + t * p[i];
		// comput_grad(pf,n,x1,g1);//计算梯度(调用PF)
		//comput_grad(n,x1,g1,rPre,uav_hx,uavGes);//计算梯度(调用PF)
		comput_grad(n, x1, g1, xx, yy, zz,  c, N_M, d_T_n, Mb);//计算梯度(调用PF)
		g_norm = 0.0;
		for (i = 0; i < n; i++)        g_norm = g_norm + g1[i] * g1[i];
		g_norm = sqrt(g_norm);
		//cout<<k<<"    "<<x0[0]<<"       "<<x0[1]<<"       "<<g_norm<<"\n";
		if (g_norm < e)
		{
			for (i = 0; i < n; i++)        min_point[i] = x1[i];

			delete[] g0;
			delete[] g1;
			delete[] dg;
			delete[] p;
			delete[] x0;
			delete[] x1;
			delete[] dx;
			for (i = 0; i < n; i++)                delete[] H[i];
			delete[]H;
			for (i = 0; i < n; i++)                delete[] tempH[i];
			delete[]tempH;
			delete[] gH;
			delete[] Hg;

			//return pf(min_point);//PF调用
			//	return getPlatGesFunction(min_point,rPre,uav_hx,uavGes); //20141219
	//		cout << "g_norm = " << g_norm << endl;
			return myfun(min_point, xx, yy, zz,  c, N_M, d_T_n, Mb);
		}
		for (i = 0; i < n; i++)
		{
			dx[i] = x1[i] - x0[i];
			dg[i] = g1[i] - g0[i];
		}
		//////////////////求Hk+1的矩阵运算

		//g*H,H*g
		for (i = 0; i < n; i++)
		{
			gH[i] = 0.0;
			Hg[i] = 0.0;
		}
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				gH[i] = gH[i] + dg[j] * H[j][i];
				//Hg[i]=Hg[i]+H[i][j]*dg[j];
				Hg[i] = gH[i];
			}
		}

		//num1,num2
		num1 = 0.0;
		num2 = 0.0;
		for (i = 0; i < n; i++)
		{
			num1 = num1 + dx[i] * dg[i];
			num2 = num2 + gH[i] * dg[i];
		}

		//tempH[i][j]
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
				tempH[i][j] = 0.0;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				tempH[i][j] = tempH[i][j] + H[i][j];
				tempH[i][j] = tempH[i][j] + dx[i] * dx[j] / num1;
				tempH[i][j] = tempH[i][j] - Hg[i] * gH[j] / num2;
			}
		}

		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				H[i][j] = tempH[i][j];
			}
		}
		/////////////////////////////

		//P 
		for (i = 0; i < n; i++)        p[i] = 0.0;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				p[i] = p[i] - H[i][j] * g1[j];
			}
		}

		for (i = 0; i < n; i++)
		{
			g0[i] = g1[i];
			x0[i] = x1[i];
		}
		k = k + 1;
		iterstep++;
	} while ((g_norm > e) && (iterstep < 50));//精度达到要求或迭代数最大时退出计算2015.6.23

	for (i = 0; i < n; i++)        min_point[i] = x1[i];

	delete[] g0;
	delete[] g1;
	delete[] dg;
	delete[] p;
	delete[] x0;
	delete[] x1;
	delete[] dx;
	for (i = 0; i < n; i++)                delete[] H[i];
	delete[]H;
	for (i = 0; i < n; i++)                delete[] tempH[i];
	delete[]tempH;
	delete[] gH;
	delete[] Hg;

	//return pf(min_point);//PF调用
	//return getPlatGesFunction(min_point,rPre,uav_hx,uavGes);
//	cout << "g_norm = " << g_norm << endl;
	return myfun(min_point, xx, yy, zz,  c, N_M, d_T_n, Mb);
}


/////////////
double fun(double *x)
{
       // return 100*( x[1]-x[0]*x[0] )*( x[1]-x[0]*x[0] ) + (1-x[0])*(1-x[0]);
	double y[3],r;
	y[0]=x[0]*x[0]+x[1]*x[1]+x[2]*x[2]-1.0;
    y[1]=2.0*x[0]*x[0]+x[1]*x[1]-4.0*x[2];
    y[2]=3.0*x[0]*x[0]-4.0*x[1]+x[2]*x[2];
	r = sqrt(y[0]*y[0]+y[1]*y[1]+y[2]*y[2]);
	return r;
}



//0.618法线搜索
//
//参数：指向目标函数的指针，变量个数，出发点，搜索方向
//返回：最优步长
//
//double line_search1(
//                                   double (*pf)(double *x),
//                                   int n,
//                                   double *start,
//                                   double *direction)
//{
//        int i;
//        int k;
//
//        double l,a,b,c,u,lamda,t,e;
//
//        double *xa=new double[n];
//        double *xb=new double[n];
//        double *xc=new double[n];
//        double *xl=new double[n];
//        double *xu=new double[n];
//
//        
//        //确定初始搜索区间
//        l=0.001;
//        a=0;
//
//        k=0;
//        do
//        {
//                k++;
//                c=a+l;
//                for(i=0;i<n;i++)
//                {
//                        xc[i]=start[i]+c*direction[i];
//                        xa[i]=start[i]+a*direction[i];
//                }
//                l=l/3;
//        }while( pf(xc) >= pf(xa) );                        // ???
//
//        k=0;
//        do
//        {
//                k++;
//                l=2*l;
//                b=c+l;
//                for(i=0;i<n;i++)
//                {
//                        xc[i]=start[i]+c*direction[i];
//                        xb[i]=start[i]+b*direction[i];
//                }
//                a=c;
//                c=b;
//        }while( pf(xb) <= pf(xc) );
//
//
//        a=0;
//        b=0.1;
//
//        //寻优
//        t=0.618;
//        e=0.0001;//2020。5。29 将e=0.000001 改为0.0001
//
//        lamda=b-t*(b-a);
//        u=a+t*(b-a);
//        
//        for(i=0;i<n;i++)
//        {
//                xl[i]=start[i]+lamda*direction[i];
//                xu[i]=start[i]+u*direction[i];
//        }
//
//        k=0;
//        do
//        {
//                k++;
//                if( pf(xl)<pf(xu) )
//                {
//                        b=u;
//                        u=lamda;
//                        lamda=b-t*(b-a);
//                }
//                else
//                {
//                        a=lamda;
//                        lamda=u;
//                        u=t*(b-a);
//                }
//        }while( b-a>=e );
//
//        lamda=(a+b)/2;
//
//        delete[] xa;
//        delete[] xb;
//        delete[] xc;
//        delete[] xl;
//        delete[] xu;
//        
//        return lamda ;
//
//}
