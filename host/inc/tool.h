#ifndef __TOOL_H
#define __TOOL_H

#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
//work dims define
#define MatrixDim				2
#define BufferDim				1
#define DefaultDim  			1
#define Image2dDim				2
#define Image3dDim				3
//debugInfo type
#define INFO 					1
#define ERROR 					2
#define WARNNING 				3
#define FILE_NOT_EXIST 			4
#define LOAD_FILE_FAILED		5
#define OPEN_FILE_FAILED		6
#define NULL_POINTER			7
#define STRING_BUFFER_LEN 		1024

//debug
//cout<<"run to here-->"<<__FILE__<<":"<<__LINE__<<endl;
void _debugInfo(const char *file,int line,cl_int msgtype,const char *msg,...);
//MSGTYPE can be one of the <debugInfo type> macro define
#define debug_msg(MSGTYPE, ... )  _debugInfo(__FILE__,__LINE__,MSGTYPE,__VA_ARGS__)
// High-resolution timer.
extern float getCurrentTimestamp();
/** convert the kernel file into a string */
extern int convertToString(const char *filename, std::string& s);
/**Getting platforms and choose an available one.*/
extern int getPlatform(cl_platform_id &platform);
/**Step 2:Query the platform and choose the first GPU device if has one.*/
extern cl_device_id *getCl_device_id(cl_platform_id &platform,cl_uint& numDevices);
//for dispaly Platform Infomation alone
extern int dispalyPlatformInfo();
//get xxx.aocx binary file full name string
extern std::string getBoardBinaryFile(const char *prefix, cl_device_id device);
//create Program From Binary file named (xxx.aocx)
extern cl_program createProgramFromBinary(cl_context context, const char *binary_file_name, const cl_device_id *devices, unsigned num_devices);
//show kernel build log
extern void ShowBuildLog(cl_program program, cl_device_id* devices);
//show help message
#endif

