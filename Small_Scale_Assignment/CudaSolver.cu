#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "ReadMatrix.h"


/**
*
* @param M Numner of rows
* @param JA Array of column indieces
* @param IRP Array of pointers to row start
* @param AS Array of cooefficients
* @param OUT Solution vector
* @param blockAmount
*/


__global__ void CSRCudaMatrixVectorProduct(const int &M, const int * JA, const int * IRP, const double * AS, double * IN, double * OUT)
{

	extern __shared__ double data[];

	
	unsigned int threadIndex = threadIdx.x;
	unsigned int t_idx = blockIdx.x * blockDim.x+ threadIndex;
	//All modern GPU have warp size 32
	int w_idx = t_idx / 32;

	int tw_idx = t_idx & (32 - 1);
	//unsigned int gridSize = blockSize * gridDim.x * 2;

	data[0] = 0;
	int i = w_idx;

	if (i < M)
	{

		data[threadIndex] = 0;
		for (unsigned j = IRP[i] + threadIdx.x; i < IRP[i + 1]; j+=32)
		{
			data[threadIndex] += AS[j] * IN[JA[j]];
		
		}
		__syncthreads();
		/*
		//Block size up to 512
		if (blockSize >= 512)
		{
			if (threadIndex < 256)
			{
				data[threadIndex] += data[threadIndex + 256];
			}
			__syncthreads();
		}

		//Block size up to 256
		if (blockSize >= 256)
		{
			if (threadIndex < 128)
			{
				data[threadIndex] += data[threadIndex + 128];
			}
			__syncthreads();
		}
		//Block size up to 128
		if (blockSize >= 128)
		{
			if (threadIndex < 64)
			{
				data[threadIndex] += data[threadIndex + 64];
			}
			__syncthreads();
		}
		*/
		


		if (threadIndex < 32)
		{
			
			if (tw_idx < 16) data[threadIndex] += data[threadIndex + 16];
			if (tw_idx < 8) data[threadIndex] += data[threadIndex + 8];
			if (tw_idx < 4) data[threadIndex] += data[threadIndex + 4];
			if (tw_idx < 2) data[threadIndex] += data[threadIndex + 2];
			if (tw_idx < 1) data[threadIndex] += data[threadIndex + 1];
		}

		//First thread stores output
		if (threadIndex == 0)
		{
			OUT[i] = data[threadIdx.x];
		}

	}

	
	
}



void CRSCUDASolver(ReadMatrixCSR &mat,  std::vector<double> & X, std::vector<double> & Y, float exeTime, int blockSize, int maxBlocks )
{

	//Host varables
	auto hostJA = mat.getJA();
	auto hostIRP = mat.getIRP();
	auto hostAS = mat.getAS();

	//device variables
	int * d_runParam = 0;
	int * d_JA;
	int * d_IRP;
	double * d_AS;
	double * d_X;
	double * d_Y;

	//Size variables
	int M = mat.getM();
	int NZ = mat.getNZ();
	int N = mat.getN();
	cudaError cudaStatus;
	//One dimensional M number of blocks, one block one Row 
	dim3 numberOfBlocks(M, 1, 1);
	dim3 thredsInBlock(blockSize, 1, 1);
	
	int size = sizeof(int);
	int sizeDouble = sizeof(double);
	cudaStatus = cudaSetDevice(0);

	//Varaibles for host arrary
	int hostArrayM[5];
	hostArrayM[0] = M;

	//Allocation of arrays to GPU , Host to GPU
	cudaStatus = cudaMalloc((void**)&d_runParam, size*2);
	cudaStatus = cudaMalloc((void**)&d_JA, size * NZ);
	cudaStatus = cudaMalloc((void**)&d_IRP, size * (M+1));
	cudaStatus = cudaMalloc((void**)&d_AS, sizeDouble* NZ);
	cudaStatus = cudaMalloc((void**)&d_Y, sizeDouble * N);
	cudaStatus = cudaMalloc((void**)&d_X, sizeDouble * N);



	//Copy data from host to device memory
	cudaStatus = cudaMemcpy(d_runParam, &hostArrayM[0], size * 2, cudaMemcpyHostToDevice);
	cudaStatus = cudaMemcpy(d_JA, &hostJA, size * NZ, cudaMemcpyHostToDevice);
	cudaStatus = cudaMemcpy(d_IRP, &hostIRP, size * (M + 1), cudaMemcpyHostToDevice);
	cudaStatus = cudaMemcpy(d_AS, &hostAS, sizeDouble* NZ, cudaMemcpyHostToDevice);
	cudaStatus = cudaMemcpy(d_X, &X[0], sizeDouble * N, cudaMemcpyHostToDevice);

	//Run kernel (const int &M, const int * JA, const int * IRP, const int * AS, double * OUT, double * IN)
	CSRCudaMatrixVectorProduct << <numberOfBlocks, thredsInBlock, blockSize * sizeDouble >> > (M, d_JA, d_IRP, d_AS, d_X, d_Y);


	//copy resulst to host
	cudaStatus = cudaMemcpy(&Y[0], d_Y, N * sizeDouble, cudaMemcpyDeviceToHost);
}
	


