#include <tool.h>

using namespace std;


//根据参数，判断设备类别。是CPU、GPU、ACCELERATOR或其他设备
static const char* GetDeviceType(cl_device_type it)
{
	if (it == CL_DEVICE_TYPE_CPU)
		return "CPU";
	else if (it == CL_DEVICE_TYPE_GPU)
		return "GPU";
	else if (it == CL_DEVICE_TYPE_ACCELERATOR)
		return "ACCELERATOR";
	else
		return "DEFAULT";

}
// High-resolution timer.
/*
float getCurrentTimestamp() {       // Linux
  timespec a;
  clock_gettime(CLOCK_MONOTONIC, &a);
  return (float(a.tv_nsec) * 1.0e-9) + float(a.tv_sec);
}
*/

void ShowBuildLog(cl_program program, cl_device_id* devices) {
	// Shows the log
	char* build_log;
	size_t log_size;
	// First call to know the proper size
	clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, 0, NULL,
			&log_size);
	build_log = new char[log_size + 1];
	// Second call to get the log
	clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, log_size,
			build_log, NULL);
	build_log[log_size] = '\0';
	printf("%-15s ===> %s \n","build_log",build_log);
	delete[] build_log;
}

// Print the error associciated with an error code
static void printError(cl_int error)
{
	// Print error message
	switch(error)
	{
		case -1:
			printf("CL_DEVICE_NOT_FOUND ");
			break;
		case -2:
			printf("CL_DEVICE_NOT_AVAILABLE ");
			break;
		case -3:
			printf("CL_COMPILER_NOT_AVAILABLE ");
			break;
		case -4:
			printf("CL_MEM_OBJECT_ALLOCATION_FAILURE ");
			break;
		case -5:
			printf("CL_OUT_OF_RESOURCES ");
			break;
		case -6:
			printf("CL_OUT_OF_HOST_MEMORY ");
			break;
		case -7:
			printf("CL_PROFILING_INFO_NOT_AVAILABLE ");
			break;
		case -8:
			printf("CL_MEM_COPY_OVERLAP ");
			break;
		case -9:
			printf("CL_IMAGE_FORMAT_MISMATCH ");
			break;
		case -10:
			printf("CL_IMAGE_FORMAT_NOT_SUPPORTED ");
			break;
		case -11:
			printf("CL_BUILD_PROGRAM_FAILURE ");
			break;
		case -12:
			printf("CL_MAP_FAILURE ");
			break;
		case -13:
			printf("CL_MISALIGNED_SUB_BUFFER_OFFSET ");
			break;
		case -14:
			printf("CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST ");
			break;
		case -30:
			printf("CL_INVALID_VALUE ");
			break;
		case -31:
			printf("CL_INVALID_DEVICE_TYPE ");
			break;
		case -32:
			printf("CL_INVALID_PLATFORM ");
			break;
		case -33:
			printf("CL_INVALID_DEVICE ");
			break;
		case -34:
			printf("CL_INVALID_CONTEXT ");
			break;
		case -35:
			printf("CL_INVALID_QUEUE_PROPERTIES ");
			break;
		case -36:
			printf("CL_INVALID_COMMAND_QUEUE ");
			break;
		case -37:
			printf("CL_INVALID_HOST_PTR ");
			break;
		case -38:
			printf("CL_INVALID_MEM_OBJECT ");
			break;
		case -39:
			printf("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR ");
			break;
		case -40:
			printf("CL_INVALID_IMAGE_SIZE ");
			break;
		case -41:
			printf("CL_INVALID_SAMPLER ");
			break;
		case -42:
			printf("CL_INVALID_BINARY ");
			break;
		case -43:
			printf("CL_INVALID_BUILD_OPTIONS ");
			break;
		case -44:
			printf("CL_INVALID_PROGRAM ");
			break;
		case -45:
			printf("CL_INVALID_PROGRAM_EXECUTABLE ");
			break;
		case -46:
			printf("CL_INVALID_KERNEL_NAME ");
			break;
		case -47:
			printf("CL_INVALID_KERNEL_DEFINITION ");
			break;
		case -48:
			printf("CL_INVALID_KERNEL ");
			break;
		case -49:
			printf("CL_INVALID_ARG_INDEX ");
			break;
		case -50:
			printf("CL_INVALID_ARG_VALUE ");
			break;
		case -51:
			printf("CL_INVALID_ARG_SIZE ");
			break;
		case -52:
			printf("CL_INVALID_KERNEL_ARGS ");
			break;
		case -53:
			printf("CL_INVALID_WORK_DIMENSION ");
			break;
		case -54:
			printf("CL_INVALID_WORK_GROUP_SIZE ");
			break;
		case -55:
			printf("CL_INVALID_WORK_ITEM_SIZE ");
			break;
		case -56:
			printf("CL_INVALID_GLOBAL_OFFSET ");
			break;
		case -57:
			printf("CL_INVALID_EVENT_WAIT_LIST ");
			break;
		case -58:
			printf("CL_INVALID_EVENT ");
			break;
		case -59:
			printf("CL_INVALID_OPERATION ");
			break;
		case -60:
			printf("CL_INVALID_GL_OBJECT ");
			break;
		case -61:
			printf("CL_INVALID_BUFFER_SIZE ");
			break;
		case -62:
			printf("CL_INVALID_MIP_LEVEL ");
			break;
		case -63:
			printf("CL_INVALID_GLOBAL_WORK_SIZE ");
			break;
		default:
			printf("UNRECOGNIZED ERROR CODE (%d) ", error);
	}
}


void _debugInfo(const char *file,int line,cl_int msgtype,const char *msg,...)
{

	switch(msgtype)
	{
	case INFO:
		printf("%-15s ===> %s:%d-->","DEBUG_INFO",file,line);
		break;
	case WARNNING:
		printf("%-15s ===> %s:%d-->","DEBUG_WARN",file,line);
		break;
	default:
		printf("%-15s ===> %s:%d-->","DEBUG_ERROR",file,line);
		printError(msgtype);
	}
    // Print custom message.
    va_list vl;
    va_start(vl, msg);
    vprintf(msg, vl);
    printf("\n");
    va_end(vl);
}

static bool fileExists(const char *file_name)
{         // Linux
  return access(file_name, R_OK) != -1;
}

string getBoardBinaryFile(const char *prefix, cl_device_id device)
{
  // First check if <prefix>.aocx exists. Use it if it does.
  std::string file_name = std::string(prefix) + ".aocx";
  if(fileExists(file_name.c_str()))
  {
    return file_name;
  }
  else
  {
	  cout<<"no such file named "<<file_name<<" ! "<<endl;
	  return NULL;
  }
}
// Loads a file in binary form.
static unsigned char *loadBinaryFile(const char *file_name, size_t *size)
{
	// Open the File
	FILE* fp;
	fp = fopen(file_name, "rb");
	if(fp == 0)
	{
		return NULL;
	}
	// Get the size of the file
	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);
	// Allocate space for the binary
	unsigned char *binary = new unsigned char[*size];
	// Go back to the file start
	rewind(fp);
	// Read the file into the binary
	if(fread((void*)binary, *size, 1, fp) == 0)
	{
		delete[] binary;
		fclose(fp);
		return NULL;
	}
  return binary;
}
// Create a program for all devices associated with the context.
cl_program createProgramFromBinary(
		cl_context context,
		const char *binary_file_name,
		const cl_device_id *devices,
		unsigned num_devices)
{
  // Early exit for potentially the most common way to fail: AOCX does not exist.
  if(!fileExists(binary_file_name))
  {
    //printf("AOCX file '%s' does not exist.\n", binary_file_name);
    debug_msg(FILE_NOT_EXIST, "AOCX file '%s' does not exist.\n",binary_file_name);
    exit(-1);
  }
  // Load the binary.
  size_t binary_size;
  unsigned char  *binary=loadBinaryFile(binary_file_name, &binary_size);
  if(binary == NULL)
  {
    //cout<<"Failed to load binary file"<<endl;
    debug_msg(LOAD_FILE_FAILED, "Failed to load binary file!");
    return NULL;
  }
  size_t binary_lengths[num_devices];
  unsigned char *binaries[num_devices];
  for(unsigned i = 0; i < num_devices; ++i)
  {
    binary_lengths[i] = binary_size;
    binaries[i] = binary;
  }
  cl_int status;
  cl_int binary_status[num_devices];
  cl_program program = clCreateProgramWithBinary(
												  context,
												  num_devices,
												  devices,
												  binary_lengths,
												  (const unsigned char **) binaries,
												  binary_status, &status);
  if(status != CL_SUCCESS)
  {
	  debug_msg(status, "Failed to create program with binary");
	  //cout<< "Failed to create program with binary"<<endl;
	  return NULL;
  }
  for(unsigned i = 0; i < num_devices; ++i)
  {
    if(binary_status[i]!=CL_SUCCESS)
    {
    	debug_msg(binary_status[i], " Failed to load binary for device ");
    	//cout<<" Failed to load binary for device "<<i<<endl;
    }
   }
  return program;
}
/** convert the kernel file into a string */
int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if (f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size + 1];
		if (!str)
		{
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	debug_msg(OPEN_FILE_FAILED, " Failed to load binary for device ");
	//cout << "Error: failed to open file\n:" << filename << endl;
	return -1;
}

/**Getting platforms and choose an available one.*/
int getPlatform(cl_platform_id &platform)
{
	char char_buffer[STRING_BUFFER_LEN];
	platform = NULL;//the chosen platform

	cl_uint numPlatforms=0;//the NO. of platforms
	cl_int  status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		debug_msg(status, "Error: Getting platforms!");
		//cout << "Error: Getting platforms!" << endl;
		return -1;
	}
	cout <<endl<< "found " << numPlatforms <<"Platform(s)";
	/**For clarity, choose the first available platform. */
	if (numPlatforms > 0)
	{
		cl_platform_id* platforms =(cl_platform_id*)malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		if (status != CL_SUCCESS)
		{
			debug_msg(status,"clGetPlatformIDs error");
			//cout << "clGetPlatformIDs error" << endl;
			return -1;
		}
		cout << endl<<"Query platform Info:";
		cout << endl<<"=============================" << endl;
		for(size_t i=0;i<numPlatforms;i++)
		{
			printf("platformId[%d]\n",i);
			//获取可用计算平台的名称
			clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
			printf("%-40s = %s\n","CL_PLATFORM_NAME",char_buffer);
			//获取可用计算平台的版本号,即OpenCL的版本号
			clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, STRING_BUFFER_LEN, char_buffer, NULL);
			printf("%-40s = %s\n","CL_PLATFORM_VENDOR",char_buffer);

			clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, STRING_BUFFER_LEN, char_buffer, NULL);
			printf("%-40s = %s\n\n","CL_PLATFORM_VERSION",char_buffer);

		}
		platform=*platforms;
		free(platforms);
		return 0;
	}
	else
	{
		cout <<endl<< "found " << numPlatforms <<"Platform(s)";
		return -1;
	}

}

/**Step 2:Query the platform and choose the first GPU device if has one.*/
cl_device_id *getCl_device_id(cl_platform_id &platform,cl_uint& numDevices)
{
	cl_device_type 	int_type;
	cl_ulong 	 	long_entries;
	cl_uint      	int_entries;
	size_t 		 	sizet_entries;
	size_t 			itemsize[3];
	char 			char_buffer[STRING_BUFFER_LEN];
	cl_device_id 	*devices = NULL;
	cl_bool 		imageSupport = CL_FALSE;
	cl_int    		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	cout<<"found "<<numDevices <<" device(s)";
	if (numDevices > 0) //devices available.
	{
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
		if (devices == NULL)
		{
			//cout << "malloc error" << endl;
			debug_msg(NULL_POINTER,"malloc error");
			exit(-1);
		}
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
		if (status != CL_SUCCESS)
		{
			debug_msg(status, "clGetDeviceIDs error");
			//cout << "clGetDeviceIDs error" << endl;
			return 0;
		}	
		else{
			cout <<endl<< "Query devices Info:";
			cout << endl<<"=============================" << endl;
			 for(size_t  i = 0; i < numDevices; ++i)
			  {
			    //printf("deviceId[%d],device name[ %s ] \n",i, getDeviceName(device[i]).c_str());
				printf("deviceId[%d]\n",i);
				//获取设备名称
				clGetDeviceInfo(devices[i], CL_DEVICE_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
				printf("%-40s = %s\n","CL_DEVICE_NAME",char_buffer);
				//获取设备类别
				clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(cl_device_type), &int_type, NULL);
				printf("%-40s = %s\n","CL_DEVICE_TYPE",GetDeviceType(int_type));

				//获取设备版本号
				clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, STRING_BUFFER_LEN, char_buffer, NULL);
				printf("%-40s = %s\n","CL_DRIVER_VERSION",char_buffer);

				//获取扩展支持
				clGetDeviceInfo(devices[i], CL_DEVICE_EXTENSIONS, STRING_BUFFER_LEN, char_buffer, NULL);
				printf("%-40s = %s\n","CL_DEVICE_EXTENSIONS",char_buffer);

				//获取profile
				clGetDeviceInfo(devices[i], CL_DEVICE_PROFILE, STRING_BUFFER_LEN, char_buffer, NULL);
				printf("%-40s = %s\n","CL_DEVICE_PROFILE",char_buffer);

				//获取设备频率
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &int_entries, NULL);
				printf("%-40s = %d(MHz)\n","CL_DEVICE_MAX_CLOCK_FREQUENCY(MHz)",int_entries);

				//获取可分配内存大小
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &long_entries, NULL);
				printf("%-40s = %d(MB)\n","CL_DEVICE_MAX_MEM_ALLOC_SIZE(MB)",long_entries/1024/1024);

				//获取设备全局内存大小
				clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &long_entries, NULL);
				printf("%-40s = %d(MB)\n","CL_DEVICE_GLOBAL_MEM_SIZE(MB)",long_entries/1024/1024);
				//获取设备CACHE内存大小
				clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &long_entries, NULL);
				printf("%-40s = %d(KB)\n","CL_DEVICE_GLOBAL_MEM_CACHE_SIZE(KB)",long_entries/1024);

				//获取local内存大小
				clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &long_entries, NULL);
				printf("%-40s = %d(KB)\n","CL_DEVICE_LOCAL_MEM_SIZE(KB)",long_entries/1024);

				//获取constant内存大小
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), &long_entries, NULL);
				printf("%-40s = %d(MB)\n","CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE(MB)",long_entries/1024/1024);

				//数据并行执行模块用来声明global和localwork-item IDS的最大维度。该参数最小为3
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &int_entries, NULL);
				printf("%-40s = %d\n","CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS",int_entries);

				//获取最大工作组数
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &sizet_entries, NULL);
				printf("%-40s = %d\n","CL_DEVICE_MAX_WORK_GROUP_SIZE",sizet_entries);

				//获取每个维度work-item size   数组类型
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES,3* sizeof(itemsize),itemsize, NULL);
				printf("%-40s = <%d,%d,%d>\n","CL_DEVICE_MAX_WORK_ITEM_SIZES",itemsize[0],itemsize[1],itemsize[2]);

				//获取最大计算核心数
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &int_entries, NULL);
				printf("%-40s = %d\n","CL_DEVICE_MAX_COMPUTE_UNITS",int_entries);

				//判断是否支持图像
				clGetDeviceInfo(devices[i], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL);
				printf("%-40s = %s\n","CL_DEVICE_IMAGE_SUPPORT",imageSupport==CL_TRUE?"yes":"no");
				//最大采样器数目
				clGetDeviceInfo(devices[i], CL_DEVICE_MAX_SAMPLERS, sizeof(cl_uint), &int_entries, NULL);
				printf("%-40s = %d\n","CL_DEVICE_MAX_SAMPLERS",int_entries);
				printf("=============================\n");
			  }
		}
		
	}
	
	return devices;
}


int dispalyPlatformInfo()
{
	char dname[512];
	cl_device_id devices[20];
	cl_platform_id* platform_id = NULL;
	cl_uint num_devices;
	cl_device_type int_type;
	cl_ulong long_entries;
	cl_uint num_platform;
	cl_int err;

	/*查询系统上可用的计算平台，可以理解为初始化
	*第一次获取可用的平台数量，第二次获取一个可用的平台。
	*/
	err = clGetPlatformIDs(0, NULL, &num_platform);

	if (err != CL_SUCCESS)
	{
		debug_msg(err, "clGetPlatformIDs error");
		//cout << "clGetPlatformIDs error" << endl;
		return 0;
	}

	cout << "PlatForm num:" << num_platform << endl;

	unsigned int st = 0;

	platform_id = new cl_platform_id[num_platform];

	err = clGetPlatformIDs(num_platform, platform_id, NULL);

	if (err != CL_SUCCESS)
	{
		printError(err);
		debug_msg(err,"clGetPlatformIDs error");
		//cout << "clGetPlatformIDs error" << endl;
		return 0;
	}


	for (unsigned int st = 0; st< num_platform; st++)
	{
		cout << "--------Platform：" << st + 1 <<"--------------"<< endl;

		//获取可用计算平台的名称
		clGetPlatformInfo(platform_id[st], CL_PLATFORM_NAME, 512, dname, NULL);
		cout << "CL_PLATFORM_NAME:" << dname << endl;

		//获取可用计算平台的版本号,即OpenCL的版本号
		clGetPlatformInfo(platform_id[st], CL_PLATFORM_VENDOR, 512, dname, NULL);
		cout << "CL_PLATFORM_VERSION:" << dname << endl;

		//获取可用计算平台的设备数目
		clGetDeviceIDs(platform_id[st], CL_DEVICE_TYPE_ALL, 20, devices, &num_devices);
		cout << "Device num:" << num_devices << endl;

		//循环两次，检测两个设备的属性
		for (unsigned int n = 0; n<num_devices; n++)
		{
			cout << endl << "Device " << n + 1 << endl;
			//获取设备名称
			clGetDeviceInfo(devices[n], CL_DEVICE_NAME, 512, dname, NULL);
			cout << "Device :" << dname << endl;

			//获取设备类别
			clGetDeviceInfo(devices[n], CL_DEVICE_TYPE, sizeof(cl_device_type), &int_type, NULL);
			cout << "Device Type:" << GetDeviceType(int_type) << endl;

			//获取设备版本号
			clGetDeviceInfo(devices[n], CL_DRIVER_VERSION, 512, dname, NULL);
			cout << "Device version:" << dname << endl;

			//获取扩展支持
			clGetDeviceInfo(devices[n], CL_DEVICE_EXTENSIONS, 512, dname, NULL);
			cout << "Device extensions:" << dname << endl;

			//获取设备全局内存大小
			clGetDeviceInfo(devices[n], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &long_entries, NULL);
			cout << "Device global mem(MB):" << long_entries / 1024 / 1024 << endl;

			//获取设备CACHE内存大小
			clGetDeviceInfo(devices[n], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_ulong), &long_entries, NULL);
			cout << "Device global mem cache(KB):" << long_entries / 1024 << endl;

			//获取本地内存大小
			clGetDeviceInfo(devices[n], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &long_entries, NULL);
			cout << "Device Locale mem(KB) :" << long_entries / 1024 << endl;

			//获取设备频率
			clGetDeviceInfo(devices[n], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_ulong), &long_entries, NULL);
			cout << "Device Max clock(MHz) :" << long_entries << endl;

			//获取最大工作组数
			clGetDeviceInfo(devices[n], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_ulong), &long_entries, NULL);
			cout << "Device Max Group size :" << long_entries << endl;

			//获取最大计算核心数
			clGetDeviceInfo(devices[n], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_ulong), &long_entries, NULL);
			cout << "Device Max parallel cores:" << long_entries << endl;

		}
		cout <<"-------------end--------------" <<endl<< endl;
	}
	return 0;
}
