#include "Layer.h"
#include "util.h"
Layer Layer:: buildInputLayer(size mapsize,string layer_name)
{
	Layer layer;
	layer.type = 'I';
	layer.outMapNum = 1;
	layer.mapSize = mapsize;
	layer.layerName = layer_name;
	return layer;
}

Layer Layer::buildConvLayer(int outMapNum, size kernelSize, string layer_name)
{
	Layer layer;
	layer.type = 'C';
	layer.outMapNum = outMapNum;
	layer.kernelSize = kernelSize;
	layer.layerName = layer_name;
	return layer;
}

Layer Layer::buildSampLayer(size scaleSize, string layer_name)
{
	Layer layer;
	layer.type = 'S';
	layer.scaleSize = scaleSize;
	layer.layerName = layer_name;
	return layer;
}

Layer Layer::buildOutputLayer(int classNum, string layer_name)
{
	Layer layer;
	layer.classNum = classNum;
	layer.type = 'O';
	layer.mapSize = size(1,1);
	layer.outMapNum = classNum;
	layer.layerName = layer_name;
	return layer;

}


void Layer::initKernel(int frontMapNum)
{
	kernel = new float***[frontMapNum];
	for (int i =0; i<frontMapNum;i++)
	{
		kernel[i]= new float**[outMapNum];
		for (int j=0; j<outMapNum; j++)
		{
			kernel[i][j] = new float*[kernelSize.x];
			for(int ii=0; ii<kernelSize.x; ii++)
			{
				kernel[i][j][ii] = new float[kernelSize.y];
			}
			//随机初始化卷积核
			randomMatrix(kernelSize.x,kernelSize.y,kernel[i][j]);
		}
	}
	
}

void Layer::initOutputKernel(int frontMapNum, size s)
{
	kernelSize = s;
	kernel = new float***[frontMapNum];
	for (int i =0; i<frontMapNum;i++)
	{
		kernel[i]=new float**[outMapNum];
		for (int j=0; j<outMapNum; j++)
		{
			kernel[i][j] = new float*[kernelSize.x];
			for(int ii=0; ii<kernelSize.x; ii++)
			{
				kernel[i][j][ii] = new float[kernelSize.y];
			}
			randomMatrix(kernelSize.x,kernelSize.y, kernel[i][j]);
		}
	}
	
}

void Layer::initErros(int batchSize)
{
	errors = new float***[batchSize];
	for (int i=0;i<batchSize;i++)
	{
		errors[i] = new float**[outMapNum];
		for (int m=0; m<outMapNum; m++)
		{
			errors[i][m] = new float*[mapSize.x];
			for (int n=0; n<mapSize.x;n++)
			{
				errors[i][m][n] = new float[mapSize.y];
			}
		}
	}
}

void Layer::setError(int num,int mapNo, int mapX, int mapY, float v)
{
	errors[num][mapNo][mapX][mapY] = v;
}

void Layer::setError(int numBatch, int mapNo, float** matrix, int m, int n)
{
	for(int i=0; i<m; i++)
	{
		for (int j=0; j<n; j++)
		{
			errors[numBatch][mapNo][i][j] = matrix[i][j];
		}
	}
}
