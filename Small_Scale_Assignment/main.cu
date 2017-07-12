
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <string>
#include "ReadMatrixCSR.h"
#include "runSimulation.h"
#include "runSimulation.cpp"
#include "ReadMatrixELL.h"


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


cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size);

__global__ void addKernel(int *c, const int *a, const int *b)
{
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}


void displayValues(std::vector<int> JA, std::vector<int> IRP, std::vector<double> matrixValue )
{

	//Output file in to the project folder
	std::ofstream data("output4.txt");
	if (data.is_open())
	{
		data << "JA has folllowing values: ";
		for (std::vector<int>::const_iterator i = JA.begin(); i != JA.end(); ++i)
		{
			data << *i << ' ';
		}

		data << std::endl;

		

		data << "IRP has folllowing values: ";
		for (std::vector<int>::const_iterator i = IRP.begin(); i != IRP.end(); ++i)
		{
			data << *i << ' ';
		}

		data << std::endl;

		data << "AS has folllowing values: ";
		for (std::vector<double>::const_iterator i = matrixValue.begin(); i != matrixValue.end(); ++i)
		{
			data << *i << ' ';
		}

		data << std::endl;

	}

}



void displayOneDimensionalELLValues(std::vector<int> JA, std::vector<double> AS)
{

	//Output file in to the project folder
	std::ofstream data("ELLOneDimesionalNEW.txt");
	if (data.is_open())
	{
		data << "JA has folllowing values: ";
		for (std::vector<int>::const_iterator i = JA.begin(); i != JA.end(); ++i)
		{
			data << *i << ' ';
		}

		data << std::endl;


		data << "AS has folllowing values: ";
		for (std::vector<double>::const_iterator i = AS.begin(); i != AS.end(); ++i)
		{
			data << *i << ' ';
		}

		data << std::endl;

	}

}





int main(int argc, char *argv[])
{
    const int arraySize = 5;
    const int a[arraySize] = { 1, 2, 3, 4, 5 };
    const int b[arraySize] = { 10, 20, 30, 40, 50 };
    int c[arraySize] = { 0 };
	std::vector<std::string> matriresList;
	CUdevice dev;
	int major = 0, minor = 0;
	int deviceCount = 0;
	char deviceName[256];
	unsigned int numberOfThreads = 4;
	unsigned int sizeOfBlock = 64;
	unsigned int maxNumberOfBlocks = 4096;

	std::cout << "Runnig CUDA simulation" << std::endl;

	std::cout << "List of matrices running on:" << std::endl;

	std::string currentMattix = argv[1];
	std::cout << "Argument 1 is: " << currentMattix << std::endl;


	
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



    // Add vectors in parallel.
    cudaError_t cudaStatus = addWithCuda(c, a, b, arraySize);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addWithCuda failed!");
        return 1;
    }
   

    // cudaDeviceReset must be called before exiting in order for profiling and
    // tracing tools such as Nsight and Visual Profiler to show complete traces.
    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }


	// Read input matrices
	ReadMatrixCSR matrixCSR(currentMattix);
	ReadMatrixELL matrixELL(currentMattix);
	runSimulation<ReadMatrixCSR> simCSR;
	runSimulation<ReadMatrixELL> simELLPack;

	double timeToComplete = 0;

	
	


	//std::vector<int> JA1 = matrixCSR.getJA();
	//displayValues(matrixCSR.getJA(), matrixCSR.getIRP(), matrixELL.getAS());

	displayOneDimensionalELLValues(matrixELL.getJA(), matrixELL.getAS());

	/**
	//Start Parallel computation
	*/

	//CUDA Run
	//simELLPack.runCUDA(matrixELL, numberOfThreads, sizeOfBlock, maxNumberOfBlocks, timeToComplete);
	//simCSR.runCUDA(matrixCSR, numberOfThreads, sizeOfBlock, maxNumberOfBlocks, timeToComplete);
	//sim.template runCUDA<ReadMatrixCSR>(matrixCSR, numberOfThreads, sizeOfBlock, maxNumberOfBlocks, timeToComplete);

	//OpenMP Run
	simCSR.runOpenMP(matrixCSR, numberOfThreads, timeToComplete);
	simELLPack.runOpenMP(matrixELL, numberOfThreads,  timeToComplete);
	system("pause");
    return 0;
}









// Helper function for using CUDA to add vectors in parallel.
cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size)
{
    int *dev_a = 0;
    int *dev_b = 0;
    int *dev_c = 0;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }


    // Copy input vectors from host memory to GPU buffers.
    cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

    // Launch a kernel on the GPU with one thread for each element.
    addKernel<<<1, size>>>(dev_c, dev_a, dev_b);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }
    
    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
    cudaFree(dev_c);
    cudaFree(dev_a);
    cudaFree(dev_b);
    
	
    return cudaStatus;
}














/*


CUdevice dev;
int major = 0, minor = 0;
int deviceCount = 0;
char deviceName[256];



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
error_id = cuDeviceComputeCapability(&major, &minor, dev);

if (error_id != CUDA_SUCCESS)
{
printf("cuDeviceComputeCapability returned %d\n-> %s\n", (int)error_id, getCudaDrvErrorString(error_id));
printf("Result = FAIL\n");
exit(EXIT_FAILURE);
}

error_id = cuDeviceGetName(deviceName, 256, dev);

if (error_id != CUDA_SUCCESS)
{
printf("cuDeviceGetName returned %d\n-> %s\n", (int)error_id, getCudaDrvErrorString(error_id));
printf("Result = FAIL\n");
exit(EXIT_FAILURE);
}

printf("\nDevice %d: \"%s\"\n", dev, deviceName);

int driverVersion = 0;
cuDriverGetVersion(&driverVersion);
printf("  CUDA Driver Version:                           %d.%d\n", driverVersion / 1000, (driverVersion % 100) / 10);
printf("  CUDA Capability Major/Minor version number:    %d.%d\n", major, minor);

size_t totalGlobalMem;
error_id = cuDeviceTotalMem(&totalGlobalMem, dev);

if (error_id != CUDA_SUCCESS)
{
printf("cuDeviceTotalMem returned %d\n-> %s\n", (int)error_id, getCudaDrvErrorString(error_id));
printf("Result = FAIL\n");
exit(EXIT_FAILURE);
}

char msg[256];
SPRINTF(msg, "  Total amount of global memory:                 %.0f MBytes (%llu bytes)\n",
(float)totalGlobalMem / 1048576.0f, (unsigned long long) totalGlobalMem);
printf("%s", msg);

int multiProcessorCount;
getCudaAttribute<int>(&multiProcessorCount, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, dev);

printf("  (%2d) Multiprocessors, (%3d) CUDA Cores/MP:     %d CUDA Cores\n",
multiProcessorCount, _ConvertSMVer2CoresDRV(major, minor),
_ConvertSMVer2CoresDRV(major, minor) * multiProcessorCount);

int clockRate;
getCudaAttribute<int>(&clockRate, CU_DEVICE_ATTRIBUTE_CLOCK_RATE, dev);
printf("  GPU Max Clock rate:                            %.0f MHz (%0.2f GHz)\n", clockRate * 1e-3f, clockRate * 1e-6f);
int memoryClock;
getCudaAttribute<int>(&memoryClock, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, dev);
printf("  Memory Clock rate:                             %.0f Mhz\n", memoryClock * 1e-3f);
int memBusWidth;
getCudaAttribute<int>(&memBusWidth, CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, dev);
printf("  Memory Bus Width:                              %d-bit\n", memBusWidth);
int L2CacheSize;
getCudaAttribute<int>(&L2CacheSize, CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, dev);

if (L2CacheSize)
{
printf("  L2 Cache Size:                                 %d bytes\n", L2CacheSize);
}

int maxTex1D, maxTex2D[2], maxTex3D[3];
getCudaAttribute<int>(&maxTex1D, CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_WIDTH, dev);
getCudaAttribute<int>(&maxTex2D[0], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_WIDTH, dev);
getCudaAttribute<int>(&maxTex2D[1], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_HEIGHT, dev);
getCudaAttribute<int>(&maxTex3D[0], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_WIDTH, dev);
getCudaAttribute<int>(&maxTex3D[1], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_HEIGHT, dev);
getCudaAttribute<int>(&maxTex3D[2], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE3D_DEPTH, dev);
printf("  Max Texture Dimension Sizes                    1D=(%d) 2D=(%d, %d) 3D=(%d, %d, %d)\n",
maxTex1D, maxTex2D[0], maxTex2D[1], maxTex3D[0], maxTex3D[1], maxTex3D[2]);

int  maxTex1DLayered[2];
getCudaAttribute<int>(&maxTex1DLayered[0], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_WIDTH, dev);
getCudaAttribute<int>(&maxTex1DLayered[1], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE1D_LAYERED_LAYERS, dev);
printf("  Maximum Layered 1D Texture Size, (num) layers  1D=(%d), %d layers\n",
maxTex1DLayered[0], maxTex1DLayered[1]);

int  maxTex2DLayered[3];
getCudaAttribute<int>(&maxTex2DLayered[0], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_WIDTH, dev);
getCudaAttribute<int>(&maxTex2DLayered[1], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_HEIGHT, dev);
getCudaAttribute<int>(&maxTex2DLayered[2], CU_DEVICE_ATTRIBUTE_MAXIMUM_TEXTURE2D_LAYERED_LAYERS, dev);
printf("  Maximum Layered 2D Texture Size, (num) layers  2D=(%d, %d), %d layers\n",
maxTex2DLayered[0], maxTex2DLayered[1], maxTex2DLayered[2]);

int totalConstantMemory;
getCudaAttribute<int>(&totalConstantMemory, CU_DEVICE_ATTRIBUTE_TOTAL_CONSTANT_MEMORY, dev);
printf("  Total amount of constant memory:               %u bytes\n", totalConstantMemory);
int sharedMemPerBlock;
getCudaAttribute<int>(&sharedMemPerBlock, CU_DEVICE_ATTRIBUTE_MAX_SHARED_MEMORY_PER_BLOCK, dev);
printf("  Total amount of shared memory per block:       %u bytes\n", sharedMemPerBlock);
int regsPerBlock;
getCudaAttribute<int>(&regsPerBlock, CU_DEVICE_ATTRIBUTE_MAX_REGISTERS_PER_BLOCK, dev);
printf("  Total number of registers available per block: %d\n", regsPerBlock);
int warpSize;
getCudaAttribute<int>(&warpSize, CU_DEVICE_ATTRIBUTE_WARP_SIZE, dev);
printf("  Warp size:                                     %d\n", warpSize);
int maxThreadsPerMultiProcessor;
getCudaAttribute<int>(&maxThreadsPerMultiProcessor, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR, dev);
printf("  Maximum number of threads per multiprocessor:  %d\n", maxThreadsPerMultiProcessor);
int maxThreadsPerBlock;
getCudaAttribute<int>(&maxThreadsPerBlock, CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_BLOCK, dev);
printf("  Maximum number of threads per block:           %d\n", maxThreadsPerBlock);

int blockDim[3];
getCudaAttribute<int>(&blockDim[0], CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_X, dev);
getCudaAttribute<int>(&blockDim[1], CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Y, dev);
getCudaAttribute<int>(&blockDim[2], CU_DEVICE_ATTRIBUTE_MAX_BLOCK_DIM_Z, dev);
printf("  Max dimension size of a thread block (x,y,z): (%d, %d, %d)\n", blockDim[0], blockDim[1], blockDim[2]);
int gridDim[3];
getCudaAttribute<int>(&gridDim[0], CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_X, dev);
getCudaAttribute<int>(&gridDim[1], CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Y, dev);
getCudaAttribute<int>(&gridDim[2], CU_DEVICE_ATTRIBUTE_MAX_GRID_DIM_Z, dev);
printf("  Max dimension size of a grid size (x,y,z):    (%d, %d, %d)\n", gridDim[0], gridDim[1], gridDim[2]);

int textureAlign;
getCudaAttribute<int>(&textureAlign, CU_DEVICE_ATTRIBUTE_TEXTURE_ALIGNMENT, dev);
printf("  Texture alignment:                             %u bytes\n", textureAlign);

int memPitch;
getCudaAttribute<int>(&memPitch, CU_DEVICE_ATTRIBUTE_MAX_PITCH, dev);
printf("  Maximum memory pitch:                          %u bytes\n", memPitch);

int gpuOverlap;
getCudaAttribute<int>(&gpuOverlap, CU_DEVICE_ATTRIBUTE_GPU_OVERLAP, dev);

int asyncEngineCount;
getCudaAttribute<int>(&asyncEngineCount, CU_DEVICE_ATTRIBUTE_ASYNC_ENGINE_COUNT, dev);
printf("  Concurrent copy and kernel execution:          %s with %d copy engine(s)\n", (gpuOverlap ? "Yes" : "No"), asyncEngineCount);

int kernelExecTimeoutEnabled;
getCudaAttribute<int>(&kernelExecTimeoutEnabled, CU_DEVICE_ATTRIBUTE_KERNEL_EXEC_TIMEOUT, dev);
printf("  Run time limit on kernels:                     %s\n", kernelExecTimeoutEnabled ? "Yes" : "No");
int integrated;
getCudaAttribute<int>(&integrated, CU_DEVICE_ATTRIBUTE_INTEGRATED, dev);
printf("  Integrated GPU sharing Host Memory:            %s\n", integrated ? "Yes" : "No");
int canMapHostMemory;
getCudaAttribute<int>(&canMapHostMemory, CU_DEVICE_ATTRIBUTE_CAN_MAP_HOST_MEMORY, dev);
printf("  Support host page-locked memory mapping:       %s\n", canMapHostMemory ? "Yes" : "No");

int concurrentKernels;
getCudaAttribute<int>(&concurrentKernels, CU_DEVICE_ATTRIBUTE_CONCURRENT_KERNELS, dev);
printf("  Concurrent kernel execution:                   %s\n", concurrentKernels ? "Yes" : "No");

int surfaceAlignment;
getCudaAttribute<int>(&surfaceAlignment, CU_DEVICE_ATTRIBUTE_SURFACE_ALIGNMENT, dev);
printf("  Alignment requirement for Surfaces:            %s\n", surfaceAlignment ? "Yes" : "No");

int eccEnabled;
getCudaAttribute<int>(&eccEnabled, CU_DEVICE_ATTRIBUTE_ECC_ENABLED, dev);
printf("  Device has ECC support:                        %s\n", eccEnabled ? "Enabled" : "Disabled");

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
int tccDriver;
getCudaAttribute<int>(&tccDriver, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, dev);
printf("  CUDA Device Driver Mode (TCC or WDDM):         %s\n", tccDriver ? "TCC (Tesla Compute Cluster Driver)" : "WDDM (Windows Display Driver Model)");
#endif

int unifiedAddressing;
getCudaAttribute<int>(&unifiedAddressing, CU_DEVICE_ATTRIBUTE_UNIFIED_ADDRESSING, dev);
printf("  Device supports Unified Addressing (UVA):      %s\n", unifiedAddressing ? "Yes" : "No");

int pciDomainID, pciBusID, pciDeviceID;
getCudaAttribute<int>(&pciDomainID, CU_DEVICE_ATTRIBUTE_PCI_DOMAIN_ID, dev);
getCudaAttribute<int>(&pciBusID, CU_DEVICE_ATTRIBUTE_PCI_BUS_ID, dev);
getCudaAttribute<int>(&pciDeviceID, CU_DEVICE_ATTRIBUTE_PCI_DEVICE_ID, dev);
printf("  Device PCI Domain ID / Bus ID / location ID:   %d / %d / %d\n", pciDomainID, pciBusID, pciDeviceID);

const char *sComputeMode[] =
{
"Default (multiple host threads can use ::cudaSetDevice() with device simultaneously)",
"Exclusive (only one host thread in one process is able to use ::cudaSetDevice() with this device)",
"Prohibited (no host thread can use ::cudaSetDevice() with this device)",
"Exclusive Process (many threads in one process is able to use ::cudaSetDevice() with this device)",
"Unknown",
NULL
};

int computeMode;
getCudaAttribute<int>(&computeMode, CU_DEVICE_ATTRIBUTE_COMPUTE_MODE, dev);
printf("  Compute Mode:\n");
printf("     < %s >\n", sComputeMode[computeMode]);
}


// If there are 2 or more GPUs, query to determine whether RDMA is supported
if (deviceCount >= 2)
{
int gpuid[64]; // we want to find the first two GPUs that can support P2P
int gpu_p2p_count = 0;
int tccDriver = 0;

for (int i = 0; i < deviceCount; i++)
{
checkCudaErrors(cuDeviceComputeCapability(&major, &minor, i));
getCudaAttribute<int>(&tccDriver, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, i);

// Only boards based on Fermi or later can support P2P
if ((major >= 2)
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
// on Windows (64-bit), the Tesla Compute Cluster driver for windows must be enabled to support this
&& tccDriver
#endif
)
{
// This is an array of P2P capable GPUs
gpuid[gpu_p2p_count++] = i;
}
}

// Show all the combinations of support P2P GPUs
int can_access_peer;
char deviceName0[256], deviceName1[256];

if (gpu_p2p_count >= 2)
{
for (int i = 0; i < gpu_p2p_count; i++)
{
for (int j = 0; j < gpu_p2p_count; j++)
{
if (gpuid[i] == gpuid[j])
{
continue;
}
checkCudaErrors(cuDeviceCanAccessPeer(&can_access_peer, gpuid[i], gpuid[j]));
checkCudaErrors(cuDeviceGetName(deviceName0, 256, gpuid[i]));
checkCudaErrors(cuDeviceGetName(deviceName1, 256, gpuid[j]));
printf("> Peer-to-Peer (P2P) access from %s (GPU%d) -> %s (GPU%d) : %s\n", deviceName0, gpuid[i],
deviceName1, gpuid[j],
can_access_peer ? "Yes" : "No");
}
}
}
}

printf("Result = PASS\n");
*/