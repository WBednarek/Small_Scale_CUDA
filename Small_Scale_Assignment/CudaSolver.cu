#include "CudaSolver.h"


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

	//Based on local memory
	extern __shared__ double data[];

	unsigned int warpSize = 32;

	unsigned int threadIndex = threadIdx.x;
	unsigned int threadCompInx = blockIdx.x * blockDim.x+ threadIdx.x;
	//All modern GPU have warp sizeOfInt 32
	int warpIndex = threadCompInx / warpSize;

	int threadIndegOfWarp = threadCompInx & (warpSize - 1);
	//unsigned int gridSize = blockSize * gridDim.x * 2;

	data[0] = 0;
	int index = warpIndex;

	if (index < M)
	{

		data[threadIdx.x] = 0;
		for (unsigned j = IRP[index] + threadIdx.x; index < IRP[index + 1]; j+= warpSize)
		{
			data[threadIdx.x] += AS[j] * IN[JA[j]];
		
		}
		//__syncthreads();
		
		/*
		Experimental solution not finished

		//Block sizeOfInt up to 512
		if (blockSize >= 512)
		{
			if (threadIndex < 256)
			{
				data[threadIndex] += data[threadIndex + 256];
			}
			__syncthreads();
		}

		//Block sizeOfInt up to 256
		if (blockSize >= 256)
		{
			if (threadIndex < 128)
			{
				data[threadIndex] += data[threadIndex + 128];
			}
			__syncthreads();
		}
		//Block sizeOfInt up to 128
		if (blockSize >= 128)
		{
			if (threadIndex < 64)
			{
				data[threadIndex] += data[threadIndex + 64];
			}
			__syncthreads();
		}
		*/
		


		if (threadIndex < warpSize)
		{
			
			if (threadIndegOfWarp < 16) data[threadIndex] += data[threadIndex + 16];
			if (threadIndegOfWarp < 8) data[threadIndex] += data[threadIndex + 8];
			if (threadIndegOfWarp < 4) data[threadIndex] += data[threadIndex + 4];
			if (threadIndegOfWarp < 2) data[threadIndex] += data[threadIndex + 2];
			if (threadIndegOfWarp < 1) data[threadIndex] += data[threadIndex + 1];
		}

		//First thread stores output
		if (threadIndex == 0)
		{
			OUT[index] = data[threadIdx.x];
		}

	}

	
	
}


__global__ void ELLPackCudaMatrixVectorProduct(const int &M, const int & NZ, const int * JA, const double * AS, double * IN, double * OUT, int & maxBlocks)
{
	//Based on local memory
	extern __shared__ double data[];

	unsigned int block = blockIdx.x;
	unsigned int warpSize = 32;

	while (block < NZ)
	{
		unsigned int threadIndex = threadIdx.x;
		unsigned int threadCompIdx = blockIdx.x * NZ + threadIdx.x;
		//All modern GPU have warp sizeOfInt 32
		unsigned int warpIndex = threadCompIdx / warpSize;
		unsigned int threadIndegOfWarp = threadCompIdx & (warpSize - 1);
		//unsigned int gridSize = blockSize * gridDim.x * 2;

		unsigned int limit = blockDim.x / 2;
		data[threadIdx.x] = 0;

		while (threadIndex < NZ)
		{
			data[threadIdx.x] += AS[threadCompIdx] * IN[JA[threadCompIdx]];

			threadIndex += blockDim.x;
			threadCompIdx += blockDim.x;
		}
		__syncthreads();

		// data, OUT[block], limit
		while (limit > 0)
		{
			if (threadIdx.x < limit)
			{
				data[threadIdx.x] = data[threadIdx.x] + data[threadIdx.x + limit];
			}

			__syncthreads();

			limit = limit / 2;
		}

		if (threadIdx.x == 0)
		{
			OUT[block] = data[0];
		}


		block += maxBlocks;
		
	}
	


}





void CUDASolver(ReadMatrixCSR &mat, std::vector<double> &  X, std::vector<double>& Y, int blockSize, int maxBlocks, double & timeToComplete)
{

	//Size variables
	int M = mat.getM();
	int NZ = mat.getNZ();
	int N = mat.getN();

	//Host variables
	auto hostJA = mat.getJA();
	auto hostIRP = mat.getIRP();
	auto hostAS = mat.getAS();
	//Variables for host array
	int hostArrayM[5];
	hostArrayM[0] = M;
	Y.resize(NZ);
	std::fill(Y.begin(), Y.end(), 0);

	//device variables
	int * d_runParam = 0;
	int * d_JA = 0;
	int * d_IRP = 0;
	double * d_AS = 0;
	double * d_X = 0;
	double * d_Y = 0;


	cudaError cudaStatus;
	//One dimensional M number of blocks, one block one Row 
	dim3 numberOfBlocks(M, 1, 1);
	dim3 thredsInBlock(blockSize, 1, 1);

	
	int sizeOfInt = sizeof(int);
	int sizeDouble = sizeof(double);
	
	//Time measurements variables
	cudaEvent_t start;
	cudaEvent_t end;
	float totalTimeOfExtecution = 8;
	
	cudaEventCreate(&start);
	cudaEventCreate(&end);


	try
	{

		cudaStatus = cudaSetDevice(0);



		//Allocation of arrays to GPU , Host to GPU
		cudaStatus = cudaMalloc((void**)&d_runParam, sizeOfInt * 2);
		cudaStatus = cudaMalloc((void**)&d_JA, sizeOfInt * NZ);
		cudaStatus = cudaMalloc((void**)&d_IRP, sizeOfInt * (M + 1));
		cudaStatus = cudaMalloc((void**)&d_AS, sizeDouble* NZ);
		cudaStatus = cudaMalloc((void**)&d_Y, sizeDouble * N);
		cudaStatus = cudaMalloc((void**)&d_X, sizeDouble * N);



		//Copy data from host to device memory
		cudaStatus = cudaMemcpy(d_runParam, &hostArrayM[0], sizeOfInt * 2, cudaMemcpyHostToDevice);
		cudaStatus = cudaMemcpy(d_JA, &hostJA, sizeOfInt * NZ, cudaMemcpyHostToDevice);
		cudaStatus = cudaMemcpy(d_IRP, &hostIRP, sizeOfInt * (M + 1), cudaMemcpyHostToDevice);
		cudaStatus = cudaMemcpy(d_AS, &hostAS, sizeDouble* NZ, cudaMemcpyHostToDevice);
		cudaStatus = cudaMemcpy(d_X, &X[0], sizeDouble * N, cudaMemcpyHostToDevice);


		//Start time Measurement 
		cudaEventRecord(start);
		//Run kernel (const int &M, const int * JA, const int * IRP, const int * AS, double * OUT, double * IN)
		CSRCudaMatrixVectorProduct << <numberOfBlocks, thredsInBlock, blockSize * sizeDouble >> > (M, d_JA, d_IRP, d_AS, d_X, d_Y);

		cudaStatus = cudaGetLastError();
		if (cudaStatus != cudaSuccess)
		{
			throw std::exception("FAILED TO RUN KERNEL");
			std::cout << "KERNEL RUN FAILED" << std::endl;
		}

		cudaEventRecord(end);

		cudaStatus = cudaGetLastError();
		cudaStatus = cudaDeviceSynchronize();

		//copy resulst to host
		cudaStatus = cudaMemcpy(&Y[0], d_Y, N * sizeDouble, cudaMemcpyDeviceToHost);


		cudaEventSynchronize(end);
		cudaEventElapsedTime(&totalTimeOfExtecution, start, end);
	

	}


	catch (const std::exception & ex)
	{
		cudaFree(d_runParam);
		cudaFree(d_JA);
		cudaFree(d_IRP);
		cudaFree(d_AS);
		cudaFree(d_X);
		cudaFree(d_Y);
	}
	
}



	



void CUDASolver(ReadMatrixELL & mat, std::vector<double>& X, std::vector<double>& Y, int blockSize, int maxBlocks, double & timeToComplete)
{

	//Size variables
	int M = mat.getM();
	int NZ = mat.getNZ();
	int N = mat.getN();

	//Host varables
	auto hostJA = mat.getJA();
	auto hostAS = mat.getAS();
	//Varaibles for host arrary
	int hostArrayM[3];
	hostArrayM[0] = M;
	Y.resize(NZ);
	std::fill(Y.begin(), Y.end(), 0);

	//device variables
	int * d_runParam = 0;
	int * d_JA = 0;
	double * d_AS = 0;
	double * d_X = 0;
	double * d_Y = 0;


	cudaError cudaStatus;
	//One dimensional M number of blocks, one block one Row 
	dim3 numberOfBlocks(M, 1, 1);
	dim3 thredsInBlock(blockSize, 1, 1);


	int sizeOfInt = sizeof(int);
	int sizeDouble = sizeof(double);

	//Time measurements variables
	cudaEvent_t start;
	cudaEvent_t end;
	float totalTimeOfExtecution = 8.0;

	cudaEventCreate(&start);
	cudaEventCreate(&end);


	try
	{

		cudaStatus = cudaSetDevice(0);



		//Allocation of arrays to GPU , Host to GPU
		cudaStatus = cudaMalloc((void**)&d_runParam, sizeOfInt * 3);
		cudaStatus = cudaMalloc((void**)&d_JA, sizeOfInt * NZ);
		cudaStatus = cudaMalloc((void**)&d_AS, sizeDouble* NZ);
		cudaStatus = cudaMalloc((void**)&d_Y, sizeDouble * N);
		cudaStatus = cudaMalloc((void**)&d_X, sizeDouble * N);



		//Copy data from host to device memory
		cudaStatus = cudaMemcpy(d_runParam, &hostArrayM[0], sizeOfInt * 3, cudaMemcpyHostToDevice);
		cudaStatus = cudaMemcpy(d_JA, &hostJA, sizeOfInt * NZ, cudaMemcpyHostToDevice);
		cudaStatus = cudaMemcpy(d_AS, &hostAS, sizeDouble* NZ, cudaMemcpyHostToDevice);
		cudaStatus = cudaMemcpy(d_X, &X[0], sizeDouble * N, cudaMemcpyHostToDevice);


		//Start time Measurement 
		cudaEventRecord(start);
		//Run kernel (const int &M, const int * JA, const int * IRP, const int * AS, double * OUT, double * IN)
		ELLPackCudaMatrixVectorProduct<< <numberOfBlocks, thredsInBlock, blockSize * sizeDouble >> > (M, NZ, d_JA, d_AS, d_X, d_Y, maxBlocks);

		cudaEventRecord(end);

		cudaStatus = cudaGetLastError();
		cudaStatus = cudaDeviceSynchronize();

		//copy resulst to host
		cudaStatus = cudaMemcpy(&Y[0], d_Y, N * sizeDouble, cudaMemcpyDeviceToHost);


		//cudaEventSynchronize(end);
		//cudaEventElapsedTime(&totalTimeOfExtecution, start, end);


	}


	catch (const std::exception & ex)
	{
		cudaFree(d_runParam);
		cudaFree(d_JA);
		cudaFree(d_AS);
		cudaFree(d_X);
		cudaFree(d_Y);
	}

}

