#include "iostream"
#include "util.h"
#include "vector"
#include "CNN.h"
#include "time.h"
#include "stdlib.h"
using namespace std;
#define TrainImageNum	60000
#define TestImageNum	10000
#define EPOCH 			10
const string data_dir="./";
int main()
{
	printf("program start -----> \n");

#ifdef _USE_OPENCL_
	debug_msg(INFO, "Using opencl ....");
	cl_uint numOfDevice;
	cl_int    status;
	/**Step 1: Getting platforms and choose an available one(first).*/
	cl_platform_id platform;
	getPlatform(platform);
	/**Step 2:Query the platform and choose the first GPU device if has one.*/
	cl_device_id *devices = getCl_device_id(platform,numOfDevice);

	/**Step 3: Create context.*/
	cl_context context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);

	/**Step 4: Creating command queue associate with the context.*/
	cl_command_queue commandQueue = clCreateCommandQueue(context, *devices, 0, NULL);

	//此处添加读取二进制 kernel文件
	  // Create the program for all device. Use the first device as the
	  // representative device (assuming all device are of the same type).

	const char *cl_kernel_file="CL_Kernel";
	string binary_file = getBoardBinaryFile(cl_kernel_file, devices[0]);
	printf("%-15s ===> %s \n","Using AOCX",binary_file.c_str());
	cl_program program = createProgramFromBinary(context, binary_file.c_str(), devices, numOfDevice);


	 // debug("Kernel execute scale calculate Matrix Multiple [%d x %d]",SIZE,SIZE);
	  // Build the program that was just created.
	status = clBuildProgram(program, 0, devices, "", NULL, NULL);
	// Shows the log
	ShowBuildLog(program, devices);

	//CL_CPP clcpp(CL_context, commandQueue, CL_program);
	clcpp.Initialize(context, commandQueue, program);

#endif

	// initialize input data
 	float*** train_x, ***test_x;
	float **train_label, **test_label;
	int batchsize = 10;
	int length = 28;
	//训练数据60000张图片
	train_x = new float**[TrainImageNum];
	//测试数据10000张图片
	test_x = new float**[TestImageNum];
	//训练数据label
	train_label = new float *[TrainImageNum];
	//测试label
	test_label = new float *[TestImageNum];
	//初始化数据存储空间
	for (int i = 0; i<TrainImageNum; i++)
	{
		train_x[i]=new float*[28];
		train_label[i]=new float[10];
		if (i<TestImageNum)
		{
			test_x[i]=new float*[28];
			test_label[i] = new float [10];
		}
		for (int j=0; j<28; j++)
		{
			train_x[i][j]=new float[28];
			if (i<TestImageNum)
			{
				test_x[i][j]=new float[28];
			}
		}
	}
	//读取数据
	ReadMNIST(TrainImageNum, length, train_x, data_dir+"data/train-images.idx3-ubyte");
	ReadMNIST_Label(TrainImageNum, train_label, data_dir+"data/train-labels.idx1-ubyte");
	ReadMNIST(TestImageNum, length, test_x, data_dir+"data/t10k-images.idx3-ubyte");
	ReadMNIST_Label(TestImageNum, test_label, data_dir+"data/t10k-labels.idx1-ubyte");

    // constructor CNN
	LayerBuilder builder;
    Layer layer;
    builder.addLayer(layer.buildInputLayer(size(28,28),"input_0"));
    builder.addLayer(layer.buildConvLayer(6, size(5, 5),"Conv_1"));
    builder.addLayer(layer.buildSampLayer( size(2, 2),"Sample_2"));
    builder.addLayer(layer.buildConvLayer(12, size(5, 5),"Conv_3"));
    builder.addLayer(layer.buildSampLayer( size(2, 2),"Sample_4"));
    //builder.addLayer(layer.buildConvLayer(20, size(4, 4)));
	builder.addLayer(layer.buildOutputLayer(10, "output_5"));

	CNN cnn = CNN(builder, batchsize);// biuder batchsize

    for (int i=0; i<EPOCH; i++)
    {
    	printf("EPOCH : %d/%d\n",i+1,EPOCH);
		float t1 = get_time_s();
		cnn.train(train_x, train_label, TrainImageNum);
		float t2 = get_time_s();
		cout << "train cost time : "<<(int)(t2-t1)/60<<" minute(s) "<<(int)(t2-t1)%60<<" s"<<endl;
		cnn.test(test_x, test_label, TestImageNum);
		float t3 = get_time_s();
		cout << "test cost time : "<<(int)(t3-t2)/60<<" minute(s) "<<(int)(t3-t2)%60<<" s"<<endl;
		cout << "totally cost time : "<<(int)(t3-t1)/60<<" minute(s) "<<(int)(t3-t1)%60<<" s"<<endl;
    }
	//删除训练数据
	for (int i = 0; i<TrainImageNum; i++)
	{
		delete []train_label[i];
		for (int j=0; j<28; j++)
		{
			delete []train_x[i][j];
		}
		delete []train_x[i];
	}
	//删除测试数据
	for (int i = 0; i<TestImageNum; i++)
	{
		delete []test_label[i];
		for (int j=0; j<28; j++)
		{
			delete []test_x[i][j];
		}
		delete []test_x[i];
	}
#ifdef _USE_OPENCL_
	status = clReleaseProgram(program);    //Release the program object.
	status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
	status = clReleaseContext(context);//Release context.
#endif
	delete []train_label;
	delete []train_x;
	delete []test_x;
	delete []test_label;
	printf("program over -----> \n");
	return 0;
}

