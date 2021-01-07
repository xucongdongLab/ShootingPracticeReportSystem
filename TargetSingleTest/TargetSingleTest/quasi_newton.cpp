#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "quasi_newton.h"
//#include "stdafx.h"
//#include "qmath.h"
#define MAXITER 50
#define THRESHOLD 1e-10
#define eps 2.2204e-16//����С����

//using namespace std;
double d_comp(double Xk[], double Xb[])
{
	//////////////////////////////////////////////////////
	//���㴫�����������ߵķ������
	//XkΪ����������
	//XbΪ����������
	//���ŵ��������

	double	xp = Xk[0];
	double yp = Xk[1];
	//���ٽ�����
    //double v = Xk[2];
	//%ˮƽ����ǽ�����
	double	alpha = Xk[3];
	//��ֱ����ǽ�����
	double beta = Xk[4];
	//��������ǵ�������ֵ
	double sin_alpha = sin(alpha);
	double cos_alpha = cos(alpha);
	double sin_beta = sin(beta);
	double cos_beta = cos(beta);
	//����
    //double c = 347.85;
	//����������
	double xi = Xb[0];
	double yi = Xb[1];
	double zi = Xb[2];
	//���㴫�����������߷������
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
	//���ܣ�������ڵ�ǰ�����������ݵĲ���������Mj����׼������Mb�ص����߷���ľ����
	//XbΪ��׼���������꣬XjΪ�������������꣬XkΪ��ǰ����������Ҫʹ�ù����ˮƽ�ʹ�ֱ�����
	//////////////////////////////////////////////////////////////////////////////////////////
    //double xp = Xk[0];
    //double yp = Xk[1];
	//���ٽ�����
    //double v = Xk[2];
	//ˮƽ����ǽ�����
	double alpha = Xk[3];
	//��ֱ����ǽ�����
	double beta = Xk[4];
	//��������ǵ�������ֵ
	double sin_alpha = sin(alpha);
	double cos_alpha = cos(alpha);
	double sin_beta = sin(beta);
	double cos_beta = cos(beta);
	//����
    //double c = 347.85;
	//��׼������Mi����
	double xi = Xb[0];
	double yi = Xb[1];
	double zi = Xb[2];
	//����������Mj����
	double xj = Xj[0];
	double yj = Xj[1];
	double zj = Xj[2];
	//�����ص����߷����������������׼�������ľ����
	double d_L = (zj - zi) * cos_alpha * cos_beta - (xj - xi) * sin_alpha * cos_beta - (yj - yi) * sin_beta;
	return d_L;
}

double  myfun(double Xk[], double xx[], double yy[], double zz[],  double c, int N_M, double d_T_n[], int Mb)
{
	///////////////////////////////////////////////////////////////////
	//���ܣ�����δ֪����Ŀ�꺯��
	//	XkΪδ֪�����㺯����x, y, zΪ���������꣬N_MΪ������������
	//	d_T_nΪ������ʱ�����ݣ�MbΪ��׼����������
	/////////////////////////////////////////////////////////////////
	int i;
	//double c = 347.85;//����
	double X[3]; //����������������x y z 
	//  double eps = 2.220446049250313e-016;//С��������
	//������������������ߵķ������
	//double * d = new double(N_M);//%���������������߷���������
	double d[7];
	for (i = 0; i < N_M; i++)
	{
		// ��������������
		X[0] = xx[i];
		X[1] = yy[i];
		X[2] = zz[i];
		//������������������ߵķ������
		d[i] = d_comp(Xk, X);

	}

	//������������ص����߷�������׼�������ľ����
	//double* d_L = new double(N_M);
	double d_L[7];
	//��׼������
	double Xb[3];
	Xb[0] = xx[Mb - 1];
	Xb[1] = yy[Mb - 1];
	Xb[2] = zz[Mb - 1];

	for (i = 0; i < N_M; i++)
	{
		// ��������������
		X[0] = xx[i];
		X[1] = yy[i];
		X[2] = zz[i];
		//�������������������׼�������ص����ߵľ����
		d_L[i] = l_comp(X, Xb, Xk);

	}
	//���㷽��ϵ��
	double v = Xk[2];
	
	//����Ŀ�꺯���е�ϵ��
    double K = 1000.0*v * c / sqrt(1000.0*v*1000.0*v - c * c);

	//�������Ŀ�꺯��ֵ
	//double*  F = new double(N_M - 1);
	double F[6];

	int ind = 0;//Ŀ�꺯������
	double dd = d[Mb - 1];//����׼�������ľ���

	for (i = 0; i < N_M; i++)
	{
		if (i != (Mb - 1)) //�����ǻ�׼�������������Ŀ�꺯����
		{
            F[ind] = d[i] - dd - K * (d_T_n[i] + d_L[i] / (1000.0*v+eps));
			ind++;
		}

	}
	//ͳ��Ŀ�꺯�����
	double error = 0;//����ֵ���
	for (i = 0; i < (N_M - 1); i++)
		error += F[i] * F[i];
	   
	//�������ֵ

	return sqrt(error);

}
//void comput_grad(int n,double *point,double *grad,double rPre[],double uav_hx,double uavGes[])
void comput_grad(int n, double *point, double *grad, double xx[], double yy[], double zz[], double c, int N_M, double d_T_n[], int Mb)
{
	/////////////////////////////////////////////////////////////////////////////////////
	//��������:DFP�����ݶȼ��㺯��
	//�������:nΪά��(������)
	//        *point:�������
	//        *grad:�ݶ�
	//        Xk:����������������(x,y) ����v ˮƽ����Ǧ� ��ֱ����Ǧ� 
	//        x y z :Ϊ������ ��������
	//        N_M Ϊ����������
	//        d_T_n:Ϊʱ��������ֵ
	//        Mb Ϊ��׼����������
	//�������:��
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
		//grad[i-1]=4*pf(temp)/(3*h);//PF����
		grad[i - 1] = 4 * grd / (3 * h);//PF����
		temp[i - 1] -= h;
		//grad[i-1]-=4*pf(temp)/(3*h);//PF����
		// grd =getPlatGesFunction(temp,rPre,uav_hx,uavGes);
		grd = myfun(temp, xx, yy, zz, c, N_M, d_T_n, Mb);
		grad[i - 1] -= 4 * grd / (3 * h);//PF����
		temp[i - 1] += (3 * h / 2);
		//grad[i-1]-=(pf(temp)/(6*h));//PF����
		// grd =getPlatGesFunction(temp,rPre,uav_hx,uavGes);
		grd = myfun(temp, xx, yy, zz, c, N_M, d_T_n, Mb);
		grad[i - 1] -= (grd / (6 * h));//PF����
		temp[i - 1] -= (2 * h);
		//grad[i-1]+=(pf(temp)/(6*h));//PF����
		// grd =getPlatGesFunction(temp,rPre,uav_hx,uavGes);
		grd = myfun(temp, xx, yy, zz, c, N_M, d_T_n, Mb);
		grad[i - 1] += (grd / (6 * h));//PF����
		temp[i - 1] = point[i - 1];
	}
	delete[] temp;
}

//double line_search(double (*pf)(double *x),int n,double *start,double *direction)
// double line_search(int n,double *start,double *direction,double rPre[],double uav_hx,double uavGes[])
double line_search(int n, double *start, double *direction, double xx[], double yy[], double zz[], double c, int N_M, double d_T_n[], int Mb)
{
	/////////////////////////////////////////////////////
	//��������:DFP������������
	//�������:nΪά��(������)
	//start:������ֵ
	//direction:��������
	//rPre:�����߷�����
	//uav_hx:���˻������
	//uavGes:���˻���̬��
	//�������:���Ų���
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
	//comput_grad(n,start,grad,rPre,uav_hx,uavGes);//�����ݶ�(����PF)
	comput_grad(n, start, grad, xx, yy, zz, c, N_M, d_T_n, Mb);//�����ݶ�(����PF)
	diver_a = 0;
	for (i = 1; i <= n; i++)
		diver_a = diver_a + grad[i - 1] * direction[i - 1];
	do
	{
		b = a + step;
		for (i = 1; i <= n; i++)
			temp_point[i - 1] = start[i - 1] + b * direction[i - 1];
		//comput_grad(pf,n,temp_point,grad);//�����ݶ�(����PF)
		// comput_grad(n,temp_point,grad,rPre,uav_hx,uavGes);//�����ݶ�(����PF)
		comput_grad(n, temp_point, grad, xx, yy, zz, c, N_M, d_T_n, Mb);//�����ݶ�(����PF)
		diver_b = 0;
		for (i = 1; i <= n; i++)
			diver_b = diver_b + grad[i - 1] * direction[i - 1];
		if (fabs(diver_b) < THRESHOLD) //ԭ��0.000001
		{
			delete[] grad;
			delete[] temp_point;
			return(b);
		}
		if (diver_b < -THRESHOLD)//ԭ��0.0000001
		{
			a = b;
			diver_a = diver_b;
			step = 2 * step;
		}
	} while (diver_b <= THRESHOLD);//ԭ��0.000001

	for (i = 1; i <= n; i++)
		temp_point[i - 1] = start[i - 1] + a * direction[i - 1];
	//value_a=(*pf)(temp_point);//PF����
	//value_a = getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);
	value_a = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);

	for (i = 1; i <= n; i++)
		temp_point[i - 1] = start[i - 1] + b * direction[i - 1];
	//value_b=(*pf)(temp_point);//PF����
	//value_b=getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);//PF����
	value_b = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);

	do
	{
		iterstep++;
		s = 3 * (value_b - value_a) / (b - a);
		z = s - diver_a - diver_b;
		w = sqrt(fabs(z*z - diver_a * diver_b));
		t = a + (w - z - diver_a)*(b - a) / (diver_b - diver_a + 2 * w);
		//value_b=(*pf)(temp_point);//PF����
		// value_b=getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);//PF����
		value_b = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);//PF����
		for (i = 1; i <= n; i++)
			temp_point[i - 1] = start[i - 1] + t * direction[i - 1];
		// value_t=(*pf)(temp_point);//PF����
		// value_t=getPlatGesFunction(temp_point,rPre,uav_hx,uavGes);//PF����
		value_t = myfun(temp_point, xx, yy, zz, c, N_M, d_T_n, Mb);//PF����
		//comput_grad(pf,n,temp_point,grad);//PF����
		//  comput_grad(n,temp_point,grad,rPre,uav_hx,uavGes);//�����ݶ�(����PF)
		comput_grad(n, temp_point, grad, xx, yy, zz, c, N_M, d_T_n, Mb);//�����ݶ�(����PF)
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
	} while (((fabs(diver_t) >= THRESHOLD) && (fabs(b - a) > THRESHOLD)) || (iterstep <= MAXITER));//ԭ��0.00001

	delete[] grad;
	delete[] temp_point;
	return(t);

}
//��Լ����߶ȷ�DFP��������
//
//������pfָ��Ŀ�꺯����ָ�룬n����������min_point���ܳ�ʼ�㡢��Ž��
//���أ���С�㴦����ֵ
//
//double DFP(int n,double *min_point,double rPre[],double uav_hx,double uavGes[])
double DFP(int n, double *min_point, double xx[], double yy[], double zz[], double c, int N_M, double d_T_n[], int Mb)
{
	////////////////////////////////////////////////////////////
	//��������:DFP������
	//�������:nΪά��(������)
	//min_point:������ʼ(����ֵ)
	//rPre:�����߷�������
	//uav_hx:�ɻ���������
	//uavGes:�ɻ���̬��
	//�������:��ֵʱ�ķ����������

	int i, j;
	int iterstep = 0;//����������
	int k = 0;
	double e = 1E-5;
	double g_norm;

	double *g0 = new double[n];                //�ݶ�
	double *g1 = new double[n];
	double *dg = new double[n];

	double *p = new double[n];                //�������� =-g
	double t;                                                //һά��������

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

	//comput_grad(pf,n,x0,g0);//�����ݶ�(����PF)
	//comput_grad(n,x0,g0,rPre,uav_hx,uavGes);//�����ݶ�(����PF)
	comput_grad(n, x0, g0, xx, yy, zz, c, N_M, d_T_n, Mb);//�����ݶ�(����PF)
	g_norm = 0.0;
	for (i = 0; i < n; i++)        g_norm = g_norm + g0[i] * g0[i];
	g_norm = sqrt(g_norm);
	if (g_norm < e)
	{
		for (i = 0; i < n; i++)        min_point[i] = x0[i];//PF����

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
		//return pf(min_point);//PF����
		return myfun(min_point, xx, yy, zz,  c, N_M, d_T_n, Mb);
	}

	for (i = 0; i < n; i++)        p[i] = -g0[i];

	do
	{
		//  t=line_search(pf,n,x0,p);                                

		//t = line_search(n,x0,p,rPre,uav_hx,uavGes);//�ɻ���̬��)
		t = line_search(n, x0, p, xx, yy, zz,  c, N_M, d_T_n, Mb);
		for (i = 0; i < n; i++)        x1[i] = x0[i] + t * p[i];
		// comput_grad(pf,n,x1,g1);//�����ݶ�(����PF)
		//comput_grad(n,x1,g1,rPre,uav_hx,uavGes);//�����ݶ�(����PF)
		comput_grad(n, x1, g1, xx, yy, zz,  c, N_M, d_T_n, Mb);//�����ݶ�(����PF)
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

			//return pf(min_point);//PF����
			//	return getPlatGesFunction(min_point,rPre,uav_hx,uavGes); //20141219
	//		cout << "g_norm = " << g_norm << endl;
			return myfun(min_point, xx, yy, zz,  c, N_M, d_T_n, Mb);
		}
		for (i = 0; i < n; i++)
		{
			dx[i] = x1[i] - x0[i];
			dg[i] = g1[i] - g0[i];
		}
		//////////////////��Hk+1�ľ�������

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
	} while ((g_norm > e) && (iterstep < 50));//���ȴﵽҪ�����������ʱ�˳�����2015.6.23

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

	//return pf(min_point);//PF����
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



//0.618��������
//
//������ָ��Ŀ�꺯����ָ�룬���������������㣬��������
//���أ����Ų���
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
//        //ȷ����ʼ��������
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
//        //Ѱ��
//        t=0.618;
//        e=0.0001;//2020��5��29 ��e=0.000001 ��Ϊ0.0001
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
