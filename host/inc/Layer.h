#ifndef LAYER_H
#define LAYER_H
#include "iostream"
#include "vector"
#include "util.h"
using namespace std;

class size
{
public:
	int x;
	int y;

	size(){};
	~size(){};
	size(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	size divide(size scalesize)
	{
		int x = this->x/scalesize.x;
		int y = this->y/scalesize.y;
		if(x*scalesize.x!= this->x||y*scalesize.y!=this->y)
		{
			std::cout<<this<<"can not divide"<<std::endl;
		}
		return size(x,y);
	}

	size substract(size s,int append)
	{
		int x = this->x-s.x+append;
		int y = this->y-s.y+append;
		return size(x,y);
	}

};

class Layer
{
private:
	int outMapNum;
	char type;
	string layerName;
	size mapSize;
	size scaleSize;
	size kernelSize;
	int classNum;

public:
	Layer(){};
	~Layer(){
// 		delete []kernel;
// 		delete []outputmaps;
// 		delete []errors;
// 		delete []bias;
	};
	float**** kernel;
	float**** outputmaps;
	float**** errors;
	float* bias;

	Layer buildInputLayer(size mapSize, string layer_name);
	Layer buildConvLayer(int outMapNum, size kernelize, string layer_name);
	Layer buildSampLayer(size scaleSize, string layer_name);
	Layer buildOutputLayer(int classNum, string layer_name);

	void initKernel(int frontMapNum);
	void initOutputKernel(int frontMapNum,size s);
	void initErros(int batchSize);
	void initOutputmaps(int batchSize){
		outputmaps = new float***[batchSize];
		for (int i=0;i<batchSize;i++)
		{
			outputmaps[i] = new float**[outMapNum];
			for (int j=0; j<outMapNum; j++)
			{
				outputmaps[i][j]=new float*[getMapSize().x];
				for(int ii=0; ii<getMapSize().x; ii++)
				{
					outputmaps[i][j][ii] = new float[getMapSize().y];
				}
			}
		}
	}
	void initBias(int frontmapNum){
		bias =  new float[outMapNum];
		for (int i=0; i<outMapNum; i++)
		{
			//bias[i]=0.0;
			bias[i]=(float)(rand()%1000)*0.001-0.5;//(-0.5,0.5)
		}
	}

	void setError(int numBatch,int mapNo, int mapX, int mapY, float v);
	float** getError(int numBatch, int mapNo){
		return errors[numBatch][mapNo];
	}
	void setError(int numBatch, int mapNo, float** matrix, int m, int n);

	float** getKernel(int numBatch, int mapNo){
		return kernel[numBatch][mapNo];
	}

	int getOutMapNum(){
		return outMapNum;
	}
	char getType(){
		return type;
	}
	string getLayerName()
	{
		return layerName;
	}
	size getMapSize(){
		return mapSize;
	}
	void setMapSize(size mapSize){
		this->mapSize = mapSize;
	}
	void setOutMapNum(int outMapNum){
		this->outMapNum = outMapNum;
	}
	size getKernelSize(){
		return kernelSize;
	}
	size getScaleSize(){
		return scaleSize;
	}

};


#endif
