#include "CudaSolver.h"


/**
*
* @param M Number of rows
* @param JA Array of column JA
* @param IRP Array of pointers to selected start
* @param AS Array of coefficients
* @param OUT Solution vector
* @param blockAmount
*/


__global__ void CSRCudaMatrixVectorProduct(const int &M, const int * JA, const int * IRP, const double * AS, double * IN, double * OUT)
{

	//Based on local memory
	extern __shared__ double storeArray[];

	unsigned int warpSize = 32; //All modern GPU have warp sizeOfInt 32
	unsigned int threadID = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int warpID = threadID / warpSize;
	unsigned int threadIndOfWarp = threadID & (warpSize - 1);

	unsigned int threadIndex = threadIdx.x;
	
	unsigned int selected = warpID;
		

	//int warpIndex = threadID / warpSize;

	//int threadIndegOfWarp = threadID & (warpSize - 1);
	//unsigned int gridSize = blockSize * gridDim.x * 2;

	//storeArray[0] = 0;
	//int index = warpIndex;

	if (selected < M)
	{
		unsigned int rowBegining = IRP[selected];
		unsigned int endOfRow = IRP[selected + 1];

		storeArray[threadIdx.x] = 0;
		for (unsigned j = rowBegining + threadIndOfWarp; j < endOfRow; j+= warpSize)
		{
			storeArray[threadIdx.x] += AS[j] * IN[JA[j]];
		
		}


		if (threadIndOfWarp < 16) storeArray[threadIdx.x] += storeArray[threadIdx.x + 16];
		if (threadIndOfWarp < 8) storeArray[threadIdx.x] += storeArray[threadIdx.x + 8];
		if (threadIndOfWarp < 4) storeArray[threadIdx.x] += storeArray[threadIdx.x + 4];
		if (threadIndOfWarp < 2) storeArray[threadIdx.x] += storeArray[threadIdx.x + 2];
		if (threadIndOfWarp < 1) storeArray[threadIdx.x] += storeArray[threadIdx.x + 1];


		if (threadIndOfWarp == 0)
		{
			OUT[selected] = storeArray[threadIdx.x];
		}

	
		//First thread stores output
		

		//__syncthreads();
		
		/*
		Experimental solution not finished

		//Block sizeOfInt up to 512
		if (blockSize >= 512)
		{
			if (threadIndex < 256)
			{
				storeArray[threadIndex] += storeArray[threadIndex + 256];
			}
			__syncthreads();
		}

		//Block sizeOfInt up to 256
		if (blockSize >= 256)
		{
			if (threadIndex < 128)
			{
				storeArray[threadIndex] += storeArray[threadIndex + 128];
			}
			__syncthreads();
		}
		//Block sizeOfInt up to 128
		if (blockSize >= 128)
		{
			if (threadIndex < 64)
			{
				storeArray[threadIndex] += storeArray[threadIndex + 64];
			}
			__syncthreads();
		}
		*/
		


		

	}

	
	
}


__global__ void ELLPackCudaMatrixVectorProduct(const int &M, const int & NZ, const int * JA, const double * AS, double * IN, double * OUT, int & maxBlocks)
{
	//Based on local memory
	extern __shared__ double storeArray[];

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
		storeArray[threadIdx.x] = 0;

		while (threadIndex < NZ)
		{
			storeArray[threadIdx.x] += AS[threadCompIdx] * IN[JA[threadCompIdx]];

			threadIndex += blockDim.x;
			threadCompIdx += blockDim.x;
		}
		__syncthreads();

		// storeArray, OUT[block], limit
		while (limit > 0)
		{
			if (threadIdx.x < limit)
			{
				storeArray[threadIdx.x] = storeArray[threadIdx.x] + storeArray[threadIdx.x + limit];
			}

			__syncthreads();

			limit = limit / 2;
		}

		if (threadIdx.x == 0)
		{
			OUT[block] = storeArray[0];
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



		//Copy storeArray from host to device memory
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



		//Copy storeArray from host to device memory
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

