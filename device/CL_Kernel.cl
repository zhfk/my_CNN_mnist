
__kernel void MatrixMuti(
							__global int* restrict inputA,
							__global int* restrict inputB,
							int size,
							__global int* restrict output
						)
{ 
	int row = get_global_id(0);
	int col = get_global_id(1);
	int sum=0;
	for(int i=0;i<size;++i)
	{
		sum += inputA[row*size+i]*inputB[i*size+col]; 
	}
	output[row*size+col]=sum;
}

__kernel void convolution(						
							__global float* restrict input,		
							__global float* restrict kernels,		
							int last_mapSize,
							int kernel_Size,
							__global float* restrict output,
							int this_mapSize
						)
{
	int gx = get_global_id(0);
	int gy = get_global_id(1);
	float sum=0.0f;
	for(int i=0;i<kernel_Size;i++)
		for(int j=0;j<kernel_Size;j++)
		{
			sum += input[(gy + i) * last_mapSize + (gx + j)] * kernels[i * kernel_Size + j];
		}
	output[gy*this_mapSize+gx]=sum;
}

__kernel void mean_pooling(						
							__global float* restrict input,				
							int last_mapSize,
							__global float* restrict output,
							int this_mapSize,
							int scale_size,
							int pool_size
						)
{
	int i = get_global_id(0);
	int j = get_global_id(1);
	float sum = 0.0f;
	for (int si = i * scale_size; si < (i + 1) * scale_size; si++) 
	{
		for (int sj = j * scale_size; sj < (j + 1) * scale_size; sj++) 
		{
			sum += input[sj*last_mapSize+si];
		}
	}
	output[j*this_mapSize+i] = sum / pool_size;
}
