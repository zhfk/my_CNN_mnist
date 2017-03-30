#ifndef UTIL_H
#define UTIL_H
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include "Layer.h"
#include "active_loss_func.h"
#include "tool.h"
using namespace std;
class size;
//#define SIGMOID(x) (1/(1+exp(-x)))
/*********************************************************************************/
typedef ActiveFun::sigmoid<float> active;//激活函数
typedef LossFun::mse<float> loss;//损失函数
/*********************************************************************************/
#define _SHOW_CONV_TIME_	//显示卷积计算时间
//#define _debug_time		//花费时间
#define _USE_OPENCL_	//开启opencl宏定义标志
/**********************************************************************************/
#ifdef _USE_OPENCL_

	//声明opencl用到的基本变量,将所用的context,queue,program封装到结构体中
	typedef struct CL_cpp{
		cl_context* 		_context;
		cl_command_queue*  	_queue;
		cl_program* 		_program;
		bool initialization = false;
				//空参构造函数
		explicit CL_cpp() :_context(nullptr), _queue(nullptr), _program(nullptr), initialization(false){}
		//有参构造函数
		explicit CL_cpp(
				cl_context& context,
				cl_command_queue&   queue,
				cl_program& program
			) :_context(&context), _queue(&queue), _program(&program), initialization(true)
		{
		}
		//初始化
		bool Initialize(
				cl_context& 		context,
				cl_command_queue&   queue,
				cl_program& 		program
				)
		{
			if (this->initialization)return this->initialization;//若已经初始化则直接返回
			_context = &context;
			_queue = 	&queue;
			_program = &program;
			initialization = true;
			return this->initialization;
		}
	}CL_CPP;
	extern CL_CPP clcpp;//声明外部定义了变量
#endif

/*********************************************************************************/
// template <class T>
// int getArrayLen(T& array)
// {//使用模板定义一个函数getArrayLen,该函数将返回数组array的长度
// 	return (sizeof(array) / sizeof(array[0]));
// }
int ReverseInt (int i);
//void ReadMNIST(int NumberOfImages, int DataOfAnImage,vector<vector<float> > &arr);//save input data as vector
void ReadMNIST(int NumberOfImages, int DataOfAnImage,float ***arr, string name);//save input data as array
//void ReadMNIST_Label(int NumberOfImages, vector<vector<float> > &arr);
void ReadMNIST_Label(int NumberOfImages, float **arr, string name);
float get_time_s();
float get_time_us();
float ocl_get_exec_time_us(cl_event evt);
void randomMatrix(int x, int y, float** outmatrix);
void randperm(int n, int* out);
void convnValid(float **matrix,float **kernel,int m, int n, int km, int kn, float** outmatrix);// m n is the dimension of matrix and km kn is the dimension of kernel, outmatrix is result
void Sigmoid(float** matrix,float bias,int m, int n);// m n is the dimension of matrix
void ArrayPlus(float **x,float **y, int m, int n);//矩阵加法 结果放在y里；
void scaleMatrix(float** lastMap,size scaleSize, int m, int n, float** sampling);//sampling
void rot180(float** matrix, int m, int n, float** rotMatrix);
void convnFull(float** matrix, float** kernel, int m, int n, int km, int kn, float** outmatrix, float** extendMatrix);// convn full mode
void matrixDsigmoid(float** matrix, int m, int n, float** outmatrix);// calculate derivation of sigmoid function with matrix
void kronecker(float** matrix,size scale,int m, int n, float** outmatrix);
void matrixMultiply(float** matrix1, float** matrix2, int m, int n);//inner product of matrix 1 and matrix 2, result is matrix1
void sum(float**** errors, int j, int m, int n, int batchSize, float **outmatrix);
float sum(float** error,int m, int n);
void ArrayDivide(float** matrix, int batchSize, int m, int n);// result is matrix;
void ArrayMultiply(float** matrix, float val, int m, int n);// array multiply a float value, result in matrix
void setValue(float** maps, float** sum, int m, int n);
int findIndex(float*** p);
int findIndex(float* p);
#endif
