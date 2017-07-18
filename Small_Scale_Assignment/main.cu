
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <string>
#include "ReadMatrixCSR.h"
#include "ReadMatrixELL.h"
#include "SimulationAndTheTests.h"
#include "SimulationAndTheTests.cpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <cuda.h>
#include <helper_cuda_drvapi.h>
#include <drvapi_error_string.h>
#include  "deviceQueryDrv.h"



/**
Zmienc nazwu

*/



void displayValues(std::vector<int> JA, std::vector<int> IRP, std::vector<double> AS )
{

	//Output file in to the project folder
	std::ofstream storeArray("output4.txt");
	if (storeArray.is_open())
	{
		storeArray << "JA has folllowing values: ";
		for (std::vector<int>::const_iterator i = JA.begin(); i != JA.end(); ++i)
		{
			storeArray << *i << ' ';
		}

		storeArray << std::endl;

		

		storeArray << "IRP has folllowing values: ";
		for (std::vector<int>::const_iterator i = IRP.begin(); i != IRP.end(); ++i)
		{
			storeArray << *i << ' ';
		}

		storeArray << std::endl;

		storeArray << "AS has folllowing values: ";
		for (std::vector<double>::const_iterator i = AS.begin(); i != AS.end(); ++i)
		{
			storeArray << *i << ' ';
		}

		storeArray << std::endl;

	}

}



void displayOneDimensionalELLValues(std::vector<int> JA, std::vector<double> AS)
{

	//Output file in to the project folder
	std::ofstream storeArray("ELLOneDimesionalNEW.txt");
	if (storeArray.is_open())
	{
		storeArray << "JA has folllowing values: ";
		for (std::vector<int>::const_iterator i = JA.begin(); i != JA.end(); ++i)
		{
			storeArray << *i << ' ';
		}

		storeArray << std::endl;


		storeArray << "AS has the following values: ";
		for (std::vector<double>::const_iterator i = AS.begin(); i != AS.end(); ++i)
		{
			storeArray << *i << ' ';
		}

		storeArray << std::endl;

	}

}

void readCudaParameters()
{
	CUdevice dev;
	int deviceCount = 0;

	// note your project will need to link with cuda.lib files on windows
	printf("CUDA Device Query (Driver API) statically linked version \n");

	CUresult error_id = cuInit(0);

	if (error_id != CUDA_SUCCESS)
	{
		printf("cuInit(0) returned %d\n-> %s\n", error_id, getCudaDrvErrorString(error_id));
		printf("Result = FAIL\n");
		exit(EXIT_FAILURE);
	}

	error_id = cuDeviceGetCount(&deviceCount);

	if (error_id != CUDA_SUCCESS)
	{
		printf("cuDeviceGetCount returned %d\n-> %s\n", (int)error_id, getCudaDrvErrorString(error_id));
		printf("Result = FAIL\n");
		exit(EXIT_FAILURE);
	}

	// This function call returns 0 if there are no CUDA capable devices.
	if (deviceCount == 0)
	{
		printf("There are no available device(s) that support CUDA\n");
	}
	else
	{
		printf("Detected %d CUDA Capable device(s)\n", deviceCount);
	}


	for (dev = 0; dev < deviceCount; ++dev)
	{
		int warpSize;
		getCudaAttribute<int>(&warpSize, CU_DEVICE_ATTRIBUTE_WARP_SIZE, dev);
		std::cout << "Warp size is: " << warpSize << std::endl;
	}

}





int main(int argc, char *argv[])
{

	std::vector<std::string> matriresList;
	std::cout << "USAGE: put all matrices into your main project folder \nor mass one matrix as program parameter\n Program gives .xls files in the output in program main directory folder\n" << std::endl;
	
	std::string currentMattix = argv[1];
	std::cout << "Argument 1 is: " << currentMattix << std::endl;

	readCudaParameters();

	std::vector<std::string> matricesNames = { 	
		"cage4.mtx",
		
		 };


	int simulationRuns = 10;
	unsigned int numberOfThreads = 4;
	unsigned int sizeOfBlock = 64;
	unsigned int maxNumberOfBlocks = 4096;

	std::cout << "The number of simulations repetitions: " << simulationRuns << std::endl << std::endl;;
	/**
	//Start Parallel computation
	*/
	for (auto it : matricesNames)
	{
		// Read input matrices
		ReadMatrixCSR matrixCSR(it);
		ReadMatrixELL matrixELL(it);
		SimulationAndTheTests<ReadMatrixCSR> simCSR;
		SimulationAndTheTests<ReadMatrixELL> simELLPack;
		//std::cout<<"ELL SIZE: "<<sizeof(matrixELL)<<std::endl;

		//OpenMP Run
		simCSR.runOpenMP(matrixCSR, numberOfThreads, simulationRuns);
		simELLPack.runOpenMP(matrixELL, numberOfThreads, simulationRuns);

		//CUDA Run
		simCSR.runCUDA(matrixCSR, numberOfThreads, sizeOfBlock, maxNumberOfBlocks, simulationRuns);
		simELLPack.runCUDA(matrixELL, numberOfThreads, sizeOfBlock, maxNumberOfBlocks, simulationRuns);

	}

	system("pause");
    return 0;
}

