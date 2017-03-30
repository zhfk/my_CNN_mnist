#include "CNN.h"
#include "vector"
#include "iostream"
using namespace std;
//#define DSIGMOID(S) (S*(1-S)) // derivative of the sigmoid as a function of the sigmoid's output
void CNN::train(float ***train_x,float** train_label,int NumOfImage)
{
	cout<<"Start train ..."<<endl;
	int epochsNum =NumOfImage/batchSize;//  (60000/10=6000)
	if(NumOfImage%batchSize != 0)
	{
		cout<<"Please reset batchSize!"<<endl;
	}
	float ***Train;
	float** TrainLabel;
	int* randPerm ;
	Train = new float **[batchSize];
	TrainLabel = new float *[batchSize];
	randPerm = new int[NumOfImage];
	randperm(NumOfImage, randPerm);
	printf("NumOfImage:%d ,epochsNum:%d ,batchSize:%d\n", NumOfImage, epochsNum, batchSize);
	for ( int i=0; i<epochsNum;i++)//6000
	{
		printf("\n-----------------------------process rate : %d/%d --------------------------------\n",i,epochsNum);
		float t1 = get_time_s();
		for ( int j=0; j<batchSize; j++)
		{
			if (i == 0)
			{
				Train[j] = new float *[28];
			}
			for (int m=0; m<28; m++)
			{
				if (i ==0)
				{
					Train[j][m] = new float [28];
				}				
				for (int n=0; n<28; n++)
				{
					Train[j][m][n] = train_x[randPerm[i*batchSize+j]][m][n];
				}
			}
			if (i == 0)
			{
				TrainLabel[j] = new float [10];
			}
			for (int l=0;l<10;l++)
			{
				TrainLabel[j][l] = train_label[randPerm[i*batchSize+j]][l];
			}
		}
		forward(Train);
		backPropagation(Train,TrainLabel);
		updateParas();
		float t2 = get_time_s();
		cout << "epochs["<<i<<"] cost time : "<<(int)(t2-t1)/60<<" minute(s) "<<(int)(t2-t1)%60<<" second(s) = "<<(int)((t2-t1)*1000)<<" ms"<<endl;
	}	
	//cout<<"|"<<endl;
    cout<< "Finish train !"<<endl;
	delete []randPerm;
	for (int i=0; i<batchSize; i++)
	{
		delete []TrainLabel[i];
		for (int j=0; j<28; j++)
		{
			delete []Train[i][j];
		}
		delete []Train[i];
	}
	delete []Train;
	delete []TrainLabel;
}

void CNN::setup(int batchSize)
{
	layers::iterator iter = m_layers.begin();
	//Layer inputLayer = this->m_layers[0];
	(*iter).initOutputmaps(batchSize);
	iter++;
	//为所有层的权重和偏置初始化
	for ( ;iter < m_layers.end();iter++)
	{
		debug_msg(INFO, "setup parameters ---> %10s ",(iter)->getLayerName().c_str());
		int frontMapNum = (*(iter-1)).getOutMapNum();
		
		switch ((*iter).getType())
		{
		case 'I':
			break;
		case 'C':
			// set map size
			(*iter).setMapSize((*(iter-1)).getMapSize().substract((*iter).getKernelSize(),1));
			// initial convolution kernel, quantities: frontMapNum*outMapNum
			(*iter).initKernel(frontMapNum);//随机初始化卷积核
			//each map has one bias, so frontMapNum is not necessary
			(*iter).initBias(frontMapNum);//初始化偏置为0
 			(*iter).initErros(batchSize);
 			// each layer should initialize output map
 			(*iter).initOutputmaps(batchSize);
			break;
		case 'S':
			(*iter).setOutMapNum((frontMapNum));
			(*iter).setMapSize((*(iter-1)).getMapSize().divide((*iter).getScaleSize()));
			(*iter).initErros(batchSize);
			(*iter).initOutputmaps(batchSize);
			break;
		case 'O':
			(*iter).initOutputKernel(frontMapNum, (*(iter-1)).getMapSize());
			(*iter).initBias(frontMapNum);
			(*iter).initErros(batchSize);
			(*iter).initOutputmaps(batchSize);
			break;
		default:
			break;
		}
	}
}

/**
float*** x   Train
float** y	  TrainLabel
*/
void CNN::backPropagation(float*** x,float** y)
{
	setOutLayerErrors(x,y);
	setHiddenLayerErrors();
}

void CNN::forward(float ***x)
{
	setInLayerOutput(x);
	layers::iterator iter = m_layers.begin();
	iter++;
	//从卷积层开始计算
	for(; iter<m_layers.end(); iter++)
	{

		debug_msg(INFO, "\tFP\t%10s ---> %10s ",(iter-1)->getLayerName().c_str(),iter->getLayerName().c_str());

#ifdef _debug_time
		float t1 = get_time_s();
#endif

		switch((*iter).getType())
		{
		case 'C':
			setConvOutput((*iter), (*(iter-1)));
			break;
		case 'S':
			setSampOutput((*iter), (*(iter-1)));
			break;
		case 'O':
			setConvOutput((*iter), (*(iter-1)));
			break;
		default:
			printf("Unknow Layer !\n");
			break;
		}

#ifdef _debug_time
		float t2 = get_time_s();
		cout << "cost time : "<<(int)((t2-t1)*1000)<<" ms "<<endl;
#endif
	}
}

void CNN::setInLayerOutput(float*** x)
{
	layers:: iterator iter = m_layers.begin();
	//Layer inputLayer = m_layers[0];
	size mapSize = (*iter).getMapSize();
 	if(28 != mapSize.x)
	{
 		cout<<"数据记录的大小与定义的map大小不一致!" << endl;	
	}
	for(int i=0;i<batchSize;i++)
	{
		setValue((*iter).outputmaps[i][0],x[i],28,28);//28 is the size of input image witch we can change
		//(*iter).outputmaps[i][0] = x[i];
	}
}
// & for change the value in m_Layers
void CNN::setConvOutput(Layer& layer, Layer& lastLayer)
{	
	int mapNum = layer.getOutMapNum();//输出的map数
	int lastMapNum = lastLayer.getOutMapNum();//上一层的map数
	int last_x=lastLayer.getMapSize().x;//上一层map的高度
	int last_y=lastLayer.getMapSize().y;//上一层map的宽度
	int kernel_x=layer.getKernelSize().x;//获取卷积核的高度
	int kernel_y=layer.getKernelSize().y;//获取卷积核的宽度
	int x = layer.getMapSize().x;//本层map的高度
	int y = layer.getMapSize().y;//本层map的宽度
	float** sum, **sumNow;
	sum = new float*[x];
	sumNow = new float*[x];
	for (int nn=0; nn<x;nn++)
	{
		sum[nn] = new float[y];
		sumNow[nn] = new float[y];
	}
	for (size_t i = 0; i < x; i++)
	{
		for (size_t j = 0; j < y; j++)
			sum[i][j] = 0.0f;
	}
	for(int numBatch=0; numBatch<batchSize;numBatch++)
	{	
		for (int i =0; i<mapNum; i++)//输出map数
		{	
			for (int j=0; j<lastMapNum; j++)//输入map数
			{
				float **lastMap;
				lastMap= lastLayer.outputmaps[numBatch][j];
				convnValid(lastMap, layer.kernel[j][i], last_x, last_y, kernel_x, kernel_y, sumNow);//本次卷积值放入sumNow内
				ArrayPlus(sumNow,sum,x,y);// sumNow 和sum求和 结果放在sum内
			}
			Sigmoid(sum,layer.bias[i],x, y);
			setValue(layer.outputmaps[numBatch][i],sum, x, y);// 函数调用时b不用到y，只传一维地址太危险，避免直接指针操作，传值稳妥

		}
	}
	for (int i=0; i<x; i++)
	{
		delete []sum[i];
		delete []sumNow[i];
	}
	delete []sum;
	delete []sumNow;
}

void CNN::setSampOutput(Layer &layer, Layer &lastLayer)
{
	int lastMapNum = lastLayer.getOutMapNum();//上层map数
	int last_x=lastLayer.getMapSize().x;//上层map高度
	int last_y=lastLayer.getMapSize().y;//上层map宽度
	int x = layer.getMapSize().x;//本层map高度
	int y = layer.getMapSize().y;//本层map宽度
	float **sampMatrix, **lastMap;
	size scaleSize;
	sampMatrix = new float *[layer.getMapSize().x];
	for (int j=0; j<layer.getMapSize().x;j++)
	{
		sampMatrix[j] = new float[layer.getMapSize().y];
	}
	for(int num=0; num<batchSize;num++)
	{
		for (int i=0; i<lastMapNum; i++)
		{
			lastMap = lastLayer.outputmaps[num][i];
			scaleSize = layer.getScaleSize();
			scaleMatrix(lastMap,scaleSize,last_x,last_y,sampMatrix);
			//sampMatrix = scaleMatrix(lastMap,scaleSize,lastLayer.getMapSize().x,lastLayer.getMapSize().y);
			setValue(layer.outputmaps[num][i],sampMatrix,x,y);
			//layer.outputmaps[num][i]=sampMatrix;
		}
	}
	for (int i=0; i<x; i++)
	{
		delete []sampMatrix[i];
	}
	delete []sampMatrix;
}

/**
float*** x   Train
float** y	  TrainLabel
*/
void CNN::setOutLayerErrors(float*** x,float** y)
{
	
	layers :: iterator iter = m_layers.end();
	iter--;//从最后一层开始
	int mapNum = (*iter).getOutMapNum();
	float meanError=0.0, maxError = 0.0;
	for(int numBatch=0; numBatch<batchSize;numBatch++)
	{		
		for (int m=0; m<mapNum; m++)
		{
			float outmaps = (*iter).outputmaps[numBatch][m][0][0];
			float target = y[numBatch][m];
			(*iter).setError(numBatch,m,0,0,active::df(outmaps)*LossFun::Loss_gradients<loss,float>(target,outmaps));//误差计算
 		}
 	}
}

void CNN::setHiddenLayerErrors()
{
	layers :: iterator iter= m_layers.end();
	iter = iter - 2;
	for (; iter > m_layers.begin(); iter--)
	{
		debug_msg(INFO, "\tBP\t%10s <--- %10s ",iter->getLayerName().c_str(),(iter+1)->getLayerName().c_str());

#ifdef _debug_time
		float t1 = get_time_s();
#endif
		switch((*(iter+1)).getType())
		{
		case 'C':
			setSampErrors((*iter), (*(iter+1)));
			break;
		case 'S':
			setConvErrors((*iter), (*(iter+1)));
			break;
		case 'O':
			setSampErrors((*iter), (*(iter+1)));//O层就是conv层 只不过kernelsize可能为1
			break;
		default:
			break;
		}
#ifdef _debug_time
		float t2 = get_time_s();
		cout << "cost time : "<<(int)((t2-t1)*1000)<<" ms "<<endl;
#endif
	}

}
//下一层为卷积层
void CNN::setSampErrors(Layer &layer, Layer &nextLayer)
{
	int mapNum = layer.getOutMapNum();
	int nextMapNum = nextLayer.getOutMapNum();
	int next_x=nextLayer.getMapSize().x;
	int next_y=nextLayer.getMapSize().y;
	int kernel_x=nextLayer.getKernelSize().x;
	int kernel_y=nextLayer.getKernelSize().y;
	int x = layer.getMapSize().x;
	int y = layer.getMapSize().y;
	float **nextError;
	float **kernel;
	float **sum, **rotMatrix, **sumNow;
	//initialize
	sum = new float *[x];
	for (int k=0; k<x; k++)
	{
		sum[k] = new float[y];
	}
	rotMatrix = new float *[kernel_x];
	for (int kk=0; kk<kernel_x; kk++)
	{
		rotMatrix[kk] = new float[kernel_y];
	}
	sumNow =  new float *[x];
	for (int k=0; k<x; k++)
	{
		sumNow[k] = new float[y];
	}
	float **extendMatrix;
	// 扩展 并初始化 全0矩阵
	int m = next_x, n = next_y, km = kernel_x, kn = kernel_y;
	extendMatrix = new float*[m + 2 * (km - 1)];
	for (int k=0; k<m+2*(km-1); k++)
	{
		extendMatrix[k] = new float[n+2*(kn-1)];
		for (int a=0; a<n+2*(kn-1); a++)
		{
			extendMatrix[k][a] = 0.0;
		}
	}
	//calculate
	for (int numBatch=0; numBatch<batchSize; numBatch++)
	{
		for (int i=0; i<mapNum; i++)
		{
			
			for (int j=0; j<nextMapNum; j++)
			{
				
				nextError = nextLayer.getError(numBatch,j);
				kernel = nextLayer.getKernel(i,j);
				if (j==0)
				{
 					rot180(kernel, kernel_x, kernel_y, rotMatrix);
 					convnFull(nextError, rotMatrix, next_x, next_y, kernel_x, kernel_y, sum, extendMatrix);
					//sum = convnFull(nextError, rot180(kernel, nextLayer.getKernelSize().x, nextLayer.getKernelSize().y), nextLayer.getMapSize().x, nextLayer.getKernelSize().y,nextLayer.getKernelSize().x, nextLayer.getKernelSize().y);
				}
				else
				{
					rot180(kernel, kernel_x, kernel_y, rotMatrix);
					convnFull(nextError, rotMatrix, next_x, next_y, kernel_x, kernel_y, sumNow, extendMatrix);
					ArrayPlus(sumNow, sum, x, y);
					//sum = ArrayPlus(convnFull(nextError, rot180(kernel, nextLayer.getKernelSize().x, nextLayer.getKernelSize().y), nextLayer.getMapSize().x, nextLayer.getKernelSize().y,nextLayer.getKernelSize().x, nextLayer.getKernelSize().y), sum, layer.getMapSize().x, layer.getMapSize().y);
				}

			}
//			cout<<"get one outputmap : batch["<<numBatch<<"],outputmap["<<i<<"]"<<endl;
//			getchar();
			layer.setError(numBatch, i, sum, x, y);
		}
	}
	for (int i=0; i<x; i++)
	{
		delete []sum[i];
		delete []sumNow[i];
	}
	for (int i=0; i<kernel_x; i++)
	{
		delete []rotMatrix[i];
	}
	for (int i=0; i<m + 2 * (km - 1); i++)
	{
		delete []extendMatrix[i];
	}
	delete []rotMatrix;
	delete []sumNow;
	delete []sum;
	delete []extendMatrix;
}

void CNN::setConvErrors(Layer &layer, Layer &nextLayer)
{   // 本层为卷积层，下一层为采样层，即两层的map个数相同，且一个map只与令一层的一个map连接，
	// 因此只需将下一层的残差kronecker扩展再用点积即可
	int mapNum = layer.getOutMapNum();
	int x = layer.getMapSize().x;
	int y = layer.getMapSize().y;
	int nx = nextLayer.getMapSize().x;
	int ny = nextLayer.getMapSize().y;
	float **nextError;
	float **map;
	float **outMatrix, **kroneckerMatrix;
	size scale;
	outMatrix = new float *[x];
	kroneckerMatrix = new float *[x];
	for (int i=0; i<x; i++)
	{
		outMatrix[i] = new float [y];
		kroneckerMatrix[i] = new float[y];
	}
	for (int numBatch=0; numBatch<batchSize; numBatch++)
	{
		for (int m=0; m<mapNum; m++)
		{
			scale = nextLayer.getScaleSize();
			nextError = nextLayer.getError(numBatch,m);
			map = layer.outputmaps[numBatch][m];
			//矩阵相乘，但对第二个矩阵的每个元素value进行1-value操作
			matrixDsigmoid(map, x, y, outMatrix);
			kronecker(nextError, scale, nx, ny, kroneckerMatrix);
			matrixMultiply(outMatrix, kroneckerMatrix, x, y);
			//outMatrix = matrixMultiply(outMatrix, kronecker(nextError, scale, nx, ny), x, y);
			layer.setError(numBatch, m, outMatrix, x, y);
		}
	}
	for (int i=0; i<x; i++)
	{
		delete []outMatrix[i];
		delete []kroneckerMatrix[i];
	}
	delete []outMatrix;
	delete []kroneckerMatrix;
}

void CNN::updateParas()
{
	layers :: iterator iter = m_layers.begin();
	iter++;
	for (; iter<m_layers.end(); iter++)
	{
		debug_msg(INFO, "\tUP\t%10s <--- %10s ",iter->getLayerName().c_str(),(iter-1)->getLayerName().c_str());
#ifdef _debug_time
		float t1 = get_time_s();
#endif
		switch((*iter).getType())
		{
		case 'C':
			updateKernels(*iter,*(iter-1));
			updateBias(*iter);
			break;
		case 'O':
			updateKernels(*iter,*(iter-1));
			updateBias(*iter);
			break;
		default:
			break;
		}
#ifdef _debug_time
		float t2 = get_time_s();
		cout << "cost time : "<<(int)((t2-t1)*1000)<<" ms "<<endl;
#endif
	}
}
	

void CNN::updateBias(Layer &layer)
{
	float ****errors = layer.errors;
	int mapNum = layer.getOutMapNum();
	int x = layer.getMapSize().x;
	int y = layer.getMapSize().y;
	float **error;
	error = new float *[x];
	for (int i=0; i<x; i++)
	{
		error[i] = new float[y];
	}
	for (int j=0; j<mapNum; j++)
	{
		sum(errors,j, x, y, batchSize, error);
		float deltaBias = sum(error, layer.getMapSize().x, layer.getMapSize().y)/batchSize;
		float bias = layer.bias[j] + ALPHA * deltaBias;
		layer.bias[j] = bias;
	}
	for (int i=0; i<x; i++)
	{
		delete []error[i];
	}
	delete []error;
}

void CNN::updateKernels(Layer &layer, Layer &lastLayer)
{
	int mapNum = layer.getOutMapNum();
	int lastMapNum = lastLayer.getOutMapNum();
	int kernel_x = layer.getKernelSize().x;
	int kernel_y = layer.getKernelSize().y;
	int last_x = lastLayer.getMapSize().x;
	int last_y = lastLayer.getMapSize().y;
	int x = layer.getMapSize().x;
	int y = layer.getMapSize().y;
	float** deltakernel, **deltaNow;
	deltakernel = new float*[kernel_x];
	deltaNow = new float*[kernel_x];
	for (int ii=0; ii<kernel_x; ii++)
	{
		deltakernel[ii]=new float[kernel_y];
		deltaNow[ii] =new float[kernel_y];
	}
	for (int j=0; j<mapNum; j++)
	{
		for (int i=0; i<lastMapNum; i++)
		{
			for (int r=0; r<batchSize; r++)
			{
				float** error = layer.errors[r][j];
				if(r==0){
					convnValid(lastLayer.outputmaps[r][i],error,last_x, last_y, x, y, deltakernel);
					//deltakernel = convnValid(lastLayer.outputmaps[r][i],error, lastLayer.getMapSize().x, lastLayer.getMapSize().y, layer.getMapSize().x, layer.getMapSize().y);
				}
				else{
					convnValid(lastLayer.outputmaps[r][i],error,last_x, last_y, x, y, deltaNow);
					ArrayPlus( deltaNow, deltakernel,kernel_x, kernel_y);
					//deltakernel = ArrayPlus(convnValid(lastLayer.outputmaps[r][i],error, lastLayer.getMapSize().x, lastLayer.getMapSize().y, layer.getMapSize().x, layer.getMapSize().y), deltakernel,layer.getKernelSize().x, layer.getKernelSize().y);
				}
			}
			ArrayDivide(deltakernel, batchSize, layer.getKernelSize().x, layer.getKernelSize().y);
			ArrayMultiply(deltakernel, ALPHA,layer.getKernelSize().x, layer.getKernelSize().y);
			ArrayPlus(deltakernel , layer.kernel[i][j], layer.getKernelSize().x, layer.getKernelSize().y);
		}
	}
	for (int i=0; i<kernel_x; i++)
	{
		delete []deltakernel[i];
		delete []deltaNow[i];
	}
	delete []deltakernel;
	delete []deltaNow;
}

void CNN::test(float*** test_x,float** test_label, int number)
{
	cout<<"Start test ..."<<endl;
	int fause=0, predict,real;
	int Num = number/batchSize;
	float ***Test;
	//setBatchsize(1);
	Test = new float **[batchSize];
	printf("NumOfImage:%d ,epochsNum:%d ,batchSize:%d\n", number, Num, batchSize);
/*	cout<<"|";
	for(int i=0;i<10;i++)cout<<"-";
	cout<<"|"<<endl;
	cout<<"|";
	cout.flush();*/
	for (int i=0; i<Num; i++)
	{
		printf("\n-----------------------------batch process rate : %d/%d --------------------------------\n",i,Num);
		float t1 = get_time_s();
		for ( int j=0; j<batchSize; j++)
		{
			if (i == 0)
			{
				Test[j] = new float *[28];
			}
			
			for (int m=0; m<28; m++)
			{
				if (i==0)
				{
					Test[j][m] = new float [28];
				}			
				for (int n=0; n<28; n++)
				{
					Test[j][m][n] = test_x[i*batchSize+j][m][n];
				}
			}
		}
		forward(Test);
		layers ::iterator iter = m_layers.end();
		iter--;
		for (int ii=0; ii<batchSize; ii++)
		{
			predict = findIndex((*iter).outputmaps[ii]);
			real = findIndex(test_label[i*batchSize+ii]);
			if(predict != real)
			{
				fause++;
			}
		}
		/*if (i%100==0)
		{
			cout<<"*";
			cout.flush();
		}*/
		float t2 = get_time_s();
		cout << "batch["<<i<<"] cost time : "<<(int)(t2-t1)/60<<" minute(s) "<<(int)(t2-t1)%60<<" second(s) = "<<(int)((t2-t1)*1000)<<" ms"<<endl;
	}
	//cout<<"|"<<endl;
	cout<<"Finish test"<<endl;
	cout<<"Error predict number: "<< fause<<endl;
	cout<<"Rate of error :"<< 1.0*fause/number<<endl;
	cout << "accuracy : " << (number - fause) << "/" << number << endl;
	saveSomeThing(fause,number);
	for (int i=0; i<batchSize; i++)
	{
		for (int j=0; j<28; j++)
		{
			delete []Test[i][j];
		}
		delete []Test[i];
	}
	delete []Test;
}

void CNN::saveSomeThing(int fause,int number)
{
	char file_name[]="./accuracy_record.txt";
	ofstream file(file_name);
	if (!file)
	{
		printf("create or open %s error !");
		return;
	}
	file<<"Error predict number: "<< fause<<endl;
	file << "accuracy : " << (number - fause) << "/" << number << endl<<endl;
	file.close();
}
