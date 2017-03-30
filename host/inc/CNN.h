#ifndef CNN_H
#define CNN_H
#include "iostream"
#include "util.h"
#include "vector"
#include "Layer.h"
using namespace std;
typedef vector<Layer > layers;
//Builder some layers that you want
class LayerBuilder
{
public: 
	layers mLayers;

	LayerBuilder(){};
	LayerBuilder(Layer layer){
		mLayers.push_back(layer);
	}
	void addLayer(Layer layer)
	{
		printf("add layer ==> %s \n", layer.getLayerName().c_str());
		mLayers.push_back(layer);
		
	}
};

class CNN
{
private:
	layers m_layers;
	int layerNum;
	int batchSize;
	float ALPHA;

public:
	CNN(LayerBuilder layerBuilder, int batchSize)
	{
		ALPHA = 1.0;
		m_layers =  layerBuilder.mLayers;
		layerNum = m_layers.size();
		this->batchSize = batchSize;
		setup(batchSize);//初始化权重和偏置
		//initPerator();
		printf("CNN: have build layerNum(s):%d ,batchSize:%d ,ALPHA: %.3f \n",this->layerNum,this->batchSize,this->ALPHA);
	};
	~CNN(){};
	void setBatchsize(int batchsize){
		this->batchSize = batchsize;
	}
	void train(float ***train_x,float **train_label, int numofimage);
	void test(float*** test_x,float** test_label, int numOfImage);
	
	void setup(int batchSize);// builder CNN with batchSize and initialize some parameters of each layer
	void forward(float*** x);
	void backPropagation(float*** x,float** y);
	//back-propagation
	void setOutLayerErrors(float*** x,float **y);
	void setHiddenLayerErrors();
	void setSampErrors(Layer &layer, Layer &nextLayer);
	void setConvErrors(Layer &layer, Layer &nextLayer);
	void updateKernels(Layer &layer, Layer &lastLayer);
	void updateBias(Layer &layer);
	void updateParas();
	//forward
	void setInLayerOutput(float*** x);
	void setConvOutput(Layer &layer, Layer &lastLayer);
	void setSampOutput(Layer &layer, Layer &lastLayer);
	void saveSomeThing(int fause,int number);
};


#endif
