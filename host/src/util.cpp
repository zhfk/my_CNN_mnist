//比较耗时的计算在此文件中，要把其中比较耗时的计算用核函数实现
#include "util.h"
#include <unistd.h>
using namespace std;

#ifdef _USE_OPENCL_
	CL_CPP clcpp; ////定义opencl用到的基本变量,将所用的context,queue,program封装到结构体中
#endif

void check_file_exist(string name)
{
	//检查文件是否存在
	int status = access(name.c_str(), 0);
	if (!status) {
		printf("Load ==> %s !\n", name.c_str());
	}
	else {
		printf("%s:%d:%s not found\n",__FILE__,__LINE__, name.c_str());
		exit(-1);
	}
}


int ReverseInt (int i)
{
	unsigned char ch1, ch2, ch3, ch4;
	ch1=i&255;
	ch2=(i>>8)&255;
	ch3=(i>>16)&255;
	ch4=(i>>24)&255;
	return((int)ch1<<24)+((int)ch2<<16)+((int)ch3<<8)+ch4;
}
// void ReadMNIST(int NumberOfImages, int DataOfAnImage,vector<vector<float> > &arr)
// {
// 	arr.resize(NumberOfImages,vector<float>(DataOfAnImage));
// 	ifstream file ("G:\\t10k-images.idx3-ubyte",ios::binary);
// 	if (file.is_open())
// 	{
// 		int magic_number=0;
// 		int number_of_images=0;
// 		int n_rows=0;
// 		int n_cols=0;
// 		file.read((char*)&magic_number,sizeof(magic_number));
// 		magic_number= ReverseInt(magic_number);
// 		file.read((char*)&number_of_images,sizeof(number_of_images));
// 		number_of_images= ReverseInt(number_of_images);
// 		file.read((char*)&n_rows,sizeof(n_rows));
// 		n_rows= ReverseInt(n_rows);
// 		file.read((char*)&n_cols,sizeof(n_cols));
// 		n_cols= ReverseInt(n_cols);
// 		for(int i=0;i<number_of_images;++i)
// 		{
// 			for(int r=0;r<n_rows;++r)
// 			{
// 				for(int c=0;c<n_cols;++c)
// 				{
// 					unsigned char temp=0;
// 					file.read((char*)&temp,sizeof(temp));
// 					arr[i][(n_rows*r)+c]= (float)temp/255.0;
// 				}
// 			}
// 		}
// 	}
// }

void ReadMNIST(int NumberOfImages, int DataOfAnImage,float ***arr, string name)
{
	//arr = new float**[NumberOfImages];
	check_file_exist(name);
	ifstream file (name.c_str(),ios::binary);
	if (file.is_open())
	{
		int magic_number=0;
		int number_of_images=0;
		int n_rows=0;
		int n_cols=0;
		file.read((char*)&magic_number,sizeof(magic_number));
		magic_number= ReverseInt(magic_number);
		file.read((char*)&number_of_images,sizeof(number_of_images));
		number_of_images= ReverseInt(number_of_images);
		file.read((char*)&n_rows,sizeof(n_rows));
		n_rows= ReverseInt(n_rows);
		file.read((char*)&n_cols,sizeof(n_cols));
		n_cols= ReverseInt(n_cols);
		for(int i=0;i<NumberOfImages;++i)
		{
			//arr[i] = new float*[DataOfAnImage];
			for(int r=0;r<n_rows;++r)
			{
				//arr[i][r] = new float[DataOfAnImage];
				for(int c=0;c<n_cols;++c)
				{
					unsigned char temp=0;
					file.read((char*)&temp,sizeof(temp));
					arr[i][r][c]= (float)temp/255.0;
				}
			}
		}
	}
}

// void ReadMNIST_Label(int NumberOfImages,vector<vector<float> > &arr)
// {
// 	arr.resize(NumberOfImages,vector<float>(10));
// 	ifstream file ("G:\\t10k-labels.idx1-ubyte",ios::binary);
// 	if (file.is_open())
// 	{
// 		int magic_number=0;
// 		int number_of_images=0;
// 		//int n_rows=0;
// 		//int n_cols=0;
// 		file.read((char*)&magic_number,sizeof(magic_number));
// 		magic_number= ReverseInt(magic_number);
// 		file.read((char*)&number_of_images,sizeof(number_of_images));
// 		number_of_images= ReverseInt(number_of_images);
// 		//file.read((char*)&n_rows,sizeof(n_rows));
// 		//n_rows= ReverseInt(n_rows);
// 		//file.read((char*)&n_cols,sizeof(n_cols));
// 		//n_cols= ReverseInt(n_cols);
// 		for(int i=0;i<NumberOfImages;++i)
// 		{
// 			unsigned char temp=0;
// 			file.read((char*)&temp,sizeof(temp));
// 			int label = (int) temp;		
// 			for (int j =0; j<10; j++)
// 			{
// 				if (j == label)
// 					arr[i][j] = 1.0;
// 				else
// 					arr[i][j] = 0.0;
// 			}
// 		}
// 	}
// }

void ReadMNIST_Label(int NumberOfImages,float **arr, string name)
{
	/*arr = new float *[NumberOfImages];*/
	check_file_exist(name);
	ifstream file (name.c_str(),ios::binary);
	if (file.is_open())
	{
		int magic_number=0;
		int number_of_images=0;
		
		file.read((char*)&magic_number,sizeof(magic_number));
		magic_number= ReverseInt(magic_number);
		file.read((char*)&number_of_images,sizeof(number_of_images));
		number_of_images= ReverseInt(number_of_images);
		
		for(int i=0;i<NumberOfImages;++i)
		{
			//arr[i] = new float[10];
			unsigned char temp=0;
			file.read((char*)&temp,sizeof(temp));
			int label = (int) temp;		
			for (int j =0; j<10; j++)
			{
				if (j == label)
					arr[i][j] = 1.0;
				else
					arr[i][j] = 0.0;
			}
		}
	}
}

float get_time_s()
{
	// High-resolution timer.
	 // Linux
	  timespec a;
	  clock_gettime(CLOCK_MONOTONIC, &a);
	  return ((float(a.tv_nsec) * 1.0e-9) + float(a.tv_sec));
}
float get_time_us()
{
	// High-resolution timer.
	 // Linux
	  timespec a;
	  clock_gettime(CLOCK_MONOTONIC, &a);
	  return ((float(a.tv_nsec) * 1.0e-3) + float(a.tv_sec)*1e6);
}
float ocl_get_exec_time_us(cl_event evt)
{
//  cl_ulong kernelEventQueued;
//  cl_ulong kernelEventSubmit;
  cl_ulong kernelEventStart;
  cl_ulong kernelEventEnd;
//  clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_QUEUED, sizeof(cl_ulong), &kernelEventQueued, NULL);
//  clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_SUBMIT, sizeof(cl_ulong), &kernelEventSubmit, NULL);
  clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &kernelEventStart, NULL);
  clGetEventProfilingInfo(evt, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &kernelEventEnd, NULL);
  cl_ulong exectime_us = kernelEventEnd-kernelEventStart;
  return (float)exectime_us*1.0e-3;
}
void randomMatrix(int x, int y, float** matrix)
{
	for(int i=0; i<x; i++)
	{
		//matrix[i] = new float[y];
		for(int j=0; j<y; j++)
		{
			matrix[i][j]=(float)(rand()%1000)*0.001-0.5;
		}
	}
}

void randperm(int n, int *perm)
{
	int i, j, t;
	for (i = 0; i < n; i++)
	{
		perm[i] = i;
	}			
	for(i=0; i<n; i++) 
	{
		j = rand()%(n-i)+i;
		t = perm[j];
		perm[j] = perm[i];
		perm[i] = t;
	}
}

void convnValid(float **matrix,float **kernel,int m, int n, int km, int kn, float** outmatrix)
{
	int kns = n-kn+1;
	int kms = m-km+1;
#ifdef _SHOW_CONV_TIME_
	float t1 = get_time_us();
#endif

#ifdef _USE_OPENCL_

	size_t input_size = m*n;
	size_t kenel_size = km*kn;
	size_t output_szie = kns*kms;

	float *input = new float[input_size];
	float *cal_kernel = new float[kenel_size];
	float *output = new float[output_szie];

	cl_event events[3];

		for (size_t i = 0; i < m; i++)
			for (size_t j = 0; j < n; j++)
				input[i*n+j]=matrix[i][j];

		for (size_t i = 0; i < km; i++)
			for (size_t j = 0; j < kn; j++)
				cal_kernel[i*kn + j] = kernel[i][j];

	cl_mem inputBuffer = clCreateBuffer(*clcpp._context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR , input_size*sizeof(float), (void*)input, NULL);
	cl_mem kernelBuffer = clCreateBuffer(*clcpp._context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR , kenel_size*sizeof(float), (void*)cal_kernel, NULL);
	cl_mem outputBuffer = clCreateBuffer(*clcpp._context, CL_MEM_WRITE_ONLY, output_szie*sizeof(float), NULL, NULL);

	//create Kernel
	const char* KernelFunction="convolution";	//set kernel Argument
	cl_kernel CL_kernel = clCreateKernel(*clcpp._program, KernelFunction, NULL);
	clSetKernelArg(CL_kernel, 0, sizeof(cl_mem),  (void *)&inputBuffer);
	clSetKernelArg(CL_kernel, 1, sizeof(cl_mem),  (void *)&kernelBuffer);
	clSetKernelArg(CL_kernel, 2, sizeof(cl_int), (void *)&m);
	clSetKernelArg(CL_kernel, 3, sizeof(cl_int), (void *)&km);
	clSetKernelArg(CL_kernel, 4, sizeof(cl_mem),  (void *)&outputBuffer);
	clSetKernelArg(CL_kernel, 5, sizeof(cl_int), (void *)&kms);

	size_t globalThreads[]={(size_t)m,(size_t)n};
	//size_t localThreads[]={};
	//printf("%-15s ===> <%d,%d> \n","globalworksize",globalThreads[0],globalThreads[1]);
	//printf("%-15s ===> <%d,%d> \n","localworksize",localThreads[0],localThreads[1]);
	int status = clEnqueueNDRangeKernel(*clcpp._queue, CL_kernel, MatrixDim, NULL, globalThreads, nullptr, 0, NULL, &events[0]);
	status = clWaitForEvents(1,&events[0]);
	if (status != CL_SUCCESS)
		{
			//cout <<"Error: Waiting for kernel run to finish.(clWaitForEvents)"<<endl;
			debug_msg(status, "Waiting for kernel run to finish.(clWaitForEvents)");
			return ;
		}
#ifdef _SHOW_CONV_TIME_
	float kernel_time = ocl_get_exec_time_us(events[0]);
	cout << "kernel_time : "<<kernel_time<<" us ,";
#endif
	status = clReleaseEvent(events[0]);//等待执行完成
	clEnqueueReadBuffer(*clcpp._queue, outputBuffer, CL_TRUE, 0,output_szie * sizeof(float), output, 0, NULL,  &events[1]);
	status = clWaitForEvents(1, &events[1]);

	if (status != CL_SUCCESS)
	{
		//cout <<"Error: Waiting for read buffer call to finish. (clWaitForEvents)"<<endl;
		debug_msg(status, "Waiting for clEnqueueMapBuffer call to finish. (clWaitForEvents)");
		return ;
	}
#ifdef 	_SHOW_CONV_TIME_
	float read_time = ocl_get_exec_time_us(events[1]);
	cout << "read_time : "<<read_time<<" us "<<endl;
	getchar();
#endif
	status = clReleaseEvent(events[1]);
	for (size_t i = 0; i < kms; i++)
		for (size_t j = 0; j < kns; j++)
		{
			outmatrix[i][j] = output[i*kns + j];
			//printf("outmatrix[%d][%d]=%f\n", i, j, outmatrix[i][j]);
		}

	status = clReleaseKernel(CL_kernel);//*Release kernel.
	status = clReleaseMemObject(inputBuffer);//Release mem object.
	status = clReleaseMemObject(kernelBuffer);//Release mem object.
	status = clReleaseMemObject(outputBuffer);
	delete[] input;
	delete[] cal_kernel;
	delete[] output;

#else
	//measure cpu convolution time (eg. 2048us)

	for (int i=0; i<kms; i++)
	{
		/*outmatrix[i] = new float[kns];*/
		for(int j=0; j<kns; j++)
		{
			float sum=0.0;
			for(int ki=0; ki<km; ki++)
			{
				for (int kj=0; kj<kn; kj++)
				{
					sum+=matrix[i+ki][j+kj]*kernel[ki][kj];
				}
			}
			outmatrix[i][j] = sum;
		}
	}

#endif
#ifdef _SHOW_CONV_TIME_
	float t2 = get_time_us();
	cout << "conv cost time : "<<(t2-t1)<<" us "<<endl;
#endif
}

void Sigmoid(float **matrix,float bias,int m, int n)
{
// 	int m = getArrayLen(matrix);
// 	int n = getArrayLen(matrix[0]);
	
	for (int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
		{
			matrix[i][j] = active::f(matrix[i][j]+bias);
		}
	}
}

void ArrayPlus(float **x,float **y, int m, int n)
{
// 	int m = getArrayLen(x);
// 	int n = getArrayLen(x[0]);
	for (int i=0; i<m; i++)
	{
		for(int j=0; j<n; j++)
		{
			y[i][j]=x[i][j]+y[i][j];
		}
	}
}

void scaleMatrix(float** matrix, size scale, int m, int n, float** outMatrix)
{
// 	int m = getArrayLen(matrix);
// 	int n = getArrayLen(matrix[0]);
	int sm =m/scale.x;
	int sn = n/scale.y;
	if (sm*scale.x!=m || sn*scale.y!=n)
	{
		cout<<"scale can not divide by matrix";
	}
	int s = scale.x*scale.y;
#ifdef _USE_OPENCL_

		size_t input_size = m*n;
		size_t output_szie = sm*sn;

		float *input = new float[input_size];
		float *output = new float[output_szie];

		for (size_t i = 0; i < m; i++)
			for (size_t j = 0; j < n; j++)
				input[i*n+j]=matrix[i][j];

		cl_mem inputBuffer = clCreateBuffer(*clcpp._context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR , input_size*sizeof(float), (void*)input, NULL);
		cl_mem outputBuffer = clCreateBuffer(*clcpp._context, CL_MEM_WRITE_ONLY, output_szie*sizeof(float), NULL, NULL);

		//create Kernel
		const char* KernelFunction="mean_pooling";
		cl_kernel CL_kernel = clCreateKernel(*clcpp._program, KernelFunction, NULL);
		//set kernel Argument
		clSetKernelArg(CL_kernel, 0, sizeof(cl_mem),  (void *)&inputBuffer);
		clSetKernelArg(CL_kernel, 1, sizeof(cl_int), (void *)&m);
		clSetKernelArg(CL_kernel, 2, sizeof(cl_mem),  (void *)&outputBuffer);
		clSetKernelArg(CL_kernel, 3, sizeof(cl_int), (void *)&sm);
		clSetKernelArg(CL_kernel, 4, sizeof(cl_int), (void *)&scale.x);
		clSetKernelArg(CL_kernel, 5, sizeof(cl_int), (void *)&s);
		cl_event events[2];
		size_t globalThreads[]={(size_t)m,(size_t)n};
		//size_t localThreads[]={};
		//printf("%-15s ===> <%d,%d> \n","globalworksize",globalThreads[0],globalThreads[1]);
		//printf("%-15s ===> <%d,%d> \n","localworksize",localThreads[0],localThreads[1]);
		int status = clEnqueueNDRangeKernel(*clcpp._queue, CL_kernel, MatrixDim, NULL, globalThreads, nullptr, 0, NULL, &events[0]);
		status = clWaitForEvents(1,&events[0]);
		if (status != CL_SUCCESS)
			{
				//cout <<"Error: Waiting for kernel run to finish.(clWaitForEvents)"<<endl;
				debug_msg(status, "Waiting for kernel run to finish.(clWaitForEvents)");
				return ;
			}
#ifdef _SHOW_CONV_TIME_
	float scale_time = ocl_get_exec_time_us(events[0]);
	cout << "scale_time : "<<scale_time<<" us ,";
#endif
		status = clReleaseEvent(events[0]);//等待执行完成
		clEnqueueReadBuffer(*clcpp._queue, outputBuffer, CL_TRUE, 0,output_szie * sizeof(float), output, 0, NULL,  &events[1]);
		status = clWaitForEvents(1, &events[1]);
		if (status != CL_SUCCESS)
		{
			//cout <<"Error: Waiting for read buffer call to finish. (clWaitForEvents)"<<endl;
			debug_msg(status, "Waiting for clEnqueueMapBuffer call to finish. (clWaitForEvents)");
			return ;
		}
#ifdef 	_SHOW_CONV_TIME_
	float read_time = ocl_get_exec_time_us(events[1]);
	cout << "read_time : "<<read_time<<" us "<<endl;
#endif
		status = clReleaseEvent(events[1]);
		for (size_t i = 0; i < sm; i++)
			for (size_t j = 0; j < sn; j++)
			{
				outMatrix[i][j] = output[i*sn + j];
				//printf("outmatrix[%d][%d]=%f\n", i, j, outmatrix[i][j]);
			}
		status = clReleaseKernel(CL_kernel);//*Release kernel.
		status = clReleaseMemObject(inputBuffer);//Release mem object.
		status = clReleaseMemObject(outputBuffer);
		delete[] input;
		delete[] output;
#else
#ifdef _SHOW_CONV_TIME_
	float t1 = get_time_us();
#endif
	for (int i = 0; i < sm; i++) {
		for (int j = 0; j < sn; j++) {
			float sum = 0.0;
			for (int si = i * scale.x; si < (i + 1) * scale.x; si++) {
				for (int sj = j * scale.y; sj < (j + 1) * scale.y; sj++) {
					sum += matrix[si][sj];
				}
			}
			outMatrix[i][j] = sum / s;
		}
	}
#ifdef _SHOW_CONV_TIME_
	float t2 = get_time_us();
	cout << "scale cost time : "<<(t2-t1)<<" us "<<endl;
#endif
#endif
}

void rot180(float** matrix, int m, int n, float** M)
{
	// 按列对称交换
	for (int i=0; i<m; i++)
	{
		for (int j=0; j<n; j++)
		{
			M[i][j] = matrix[i][n-1-j];
		}
	}
	// 按行对称交换
	for(int j=0; j<n; j++)
	{
		for(int i=0; i<m/2; i++)
		{
			float tmp = M[i][j];
			M[i][j] = M[m-1-i][j];
			M[m-1-i][j] = tmp;
		}
	}
}

void convnFull(float** matrix, float** kernel, int m, int n, int km, int kn, float** outmatrix, float **extendMatrix)
{
// 	float **extendMatrix; //为什么这种方式内存无限被吃？
	// 扩展 并初始化 全0矩阵
// 	extendMatrix = new float*[m + 2 * (km - 1)];
// 	for (int k=0; k<m+2*(km-1); k++)
// 	{
// 		extendMatrix[k] = new float[n+2*(kn-1)];
// 		for (int a=0; a<n+2*(kn-1); a++)
// 		{
// 			extendMatrix[k][a] = 0.0;
// 		}
// 	}
	//对应部分赋值
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++)
			extendMatrix[i + km - 1][j + kn - 1] = matrix[i][j];
	}

	convnValid(extendMatrix, kernel, m + 2 * (km - 1), n+2*(kn-1), km, kn, outmatrix);
//	delete []extendMatrix;
}

void matrixDsigmoid(float** matrix, int m, int n, float** M)
{
	for (int i=0;i<m;i++)
	{
		for (int j=0; j<n;j++)
		{
			//M[i][j] = matrix[i][j]*(1-matrix[i][j]);
			M[i][j] = active::df(matrix[i][j]);
		}
	}
}

void kronecker(float** matrix,size scale,int m, int n, float** OutMatrix)
{
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			for (int ki = i * scale.x; ki < (i + 1) * scale.x; ki++) {
				for (int kj = j * scale.y; kj < (j + 1) * scale.y; kj++) {
					OutMatrix[ki][kj] = matrix[i][j];
				}
			}
		}
	}
}

void matrixMultiply(float** matrix1, float** matrix2, int m, int n)
{
	for (int i=0; i<m; i++)
	{
		for (int j=0; j<n; j++)
		{
			matrix1[i][j] = matrix1[i][j]*matrix2[i][j];
		}
	}
}

void sum(float**** errors, int j, int m, int n, int batchSize, float **M)
{
	for (int mi = 0; mi < m; mi++) {
		for (int nj = 0; nj < n; nj++) {
			float sum = 0;
			for (int i = 0; i < batchSize; i++){
				sum += errors[i][j][mi][nj];
			}
			M[mi][nj] = sum;
		}
	}
}

float sum(float** error, int m, int n)
{
	float sum = 0.0;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			sum += error[i][j];
		}
	}
	return sum;
}

void ArrayDivide(float** M, int batchSize, int m, int n)
{
	for (int i=0; i<m; i++)
	{
		for (int j=0; j<n; j++)
		{
			M[i][j] = M[i][j]/batchSize;
		}
	}
}

void ArrayMultiply(float** matrix, float val, int m, int n)
{

	for (int i=0; i<m; i++)
	{
		for (int j=0; j<n; j++)
		{
			matrix[i][j] = matrix[i][j]*val;
		}
	}
}

int findIndex(float*** p)
{
	int index = 0;
	float v;
	float Max = p[0][0][0];
	for(int i=1; i<10; i++)
	{
		v = p[i][0][0];
		if(p[i][0][0] > Max)
		{
			Max = p[i][0][0];
			index = i;
		}
	}
	return index;
}

int findIndex(float* p)
{
	int index = 0;
	float Max = p[0];
	for(int i=1; i<10; i++)
	{
		float v = p[i];
		if(p[i] > Max)
		{
			Max = p[i];
			index = i;
		}
	}
	return index;
}

void setValue(float** maps, float** sum, int m, int n)
{
	for (int i = 0; i<m; i++)
	{
		for (int j=0; j<n; j++)
		{
			maps[i][j] = sum[i][j];
		}
	}
}
