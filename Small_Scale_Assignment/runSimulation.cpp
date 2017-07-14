#include "runSimulation.h"


template<class classType>
runSimulation<classType>::runSimulation()
{
}


template<class classType>
runSimulation<classType>::~runSimulation()
{
}


template<class classType>
auto runSimulation<classType>::calcuatePerformance(classType & matrix, double completionTime)
{
	return 2.0 * matrix.getNZ() / completionTime / 1000000.0;
}


template<class classType>
void runSimulation<classType>::runCUDA(classType & mat, int numberOfThreads, int sizeOfBlock, int maximumBlocks, double & timeToComplete)
{

	std::vector<double> result(mat.getN());
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point  end;
	double complete;
	try
	{
		makeVector_X(mat.getNZ(), 2);
		start = std::chrono::high_resolution_clock::now();
		//Compute CUDA matrix-matrix, product
		CUDASolver(mat, X, result, sizeOfBlock, maximumBlocks, timeToComplete);

		end = std::chrono::high_resolution_clock::now();
		complete = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		std::chrono::duration<double> diff = end - start;
		std::cout << "Current Matrix Type: " << typeid(classType).name() << std::endl;
		std::cout << "Time in milliseconds: " << diff.count() << " ms\n";
		std::cout << "CUDA performance is: " << calcuatePerformance(mat, complete) << " GFLOPS" << std::endl;
		
		std::cout << std::endl;
	}
	catch (std::exception & e)
	{
		std::cout << "Standard exception: " << e.what() << std::endl;
	}

	/*
	
	//Test code, seems to doesn't work
	
	if (std::is_same<classType, ReadMatrixCSR>::value)
	{
		std::cout << "CUDA performance is: " << calcuatePerformance<ReadMatrixCSR>(mat, complete) << " GFLOPS" << std::endl;

	}
	
	
	
	*/
	
}


template<class classType>
void runSimulation<classType>::runOpenMP(classType & mat, int numberOfThreads, double & timeToComplete)
{

	
	OpenMP omp;

	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point  end;
	double complete;

	double avarageTime;
	makeVector_X(mat.getN(), 2);
	std::vector<double> Y(mat.getN());


	for (int i = 0; i < 1; ++i)
	{

		try
		{
			
			start = std::chrono::high_resolution_clock::now();
			//Compute OpenMP matrix-matrix, product
			//CUDASolver(mat, X, Y, sizeOfBlock, maximumBlocks, timeToComplete);
			omp.OpenMPSolver(mat, X, Y, numberOfThreads, timeToComplete);

			end = std::chrono::high_resolution_clock::now();
			complete = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
			//std::chrono::duration<double> diff = end - start;
			//std::cout << "Current Matrix Type: " << typeid(classType).name() << std::endl;
			std::cout << "Time in milliseconds: " << timeToComplete << " ms\n";
			std::cout << "OpenMP performance is: " << calcuatePerformance(mat, complete) << " GFLOPS" << std::endl;
			std::cout << std::endl;

		}
		catch (std::exception & e)
		{
			std::cout << "Standard exception: " << e.what() << std::endl;
		}
	}
	


}

template<class classType>
void runSimulation<classType>::makeVector_X(int sizeOfMaxtixRow, int numberOfMatrixXColumns)
{
	X.resize(sizeOfMaxtixRow * numberOfMatrixXColumns);
	unsigned seed;
	int randNumber;
	for (int i = 0; i < sizeOfMaxtixRow * numberOfMatrixXColumns; ++i)
	{
		seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine dre(seed);
		std::uniform_real_distribution<double> gen(0, 5);
		randNumber = gen(dre);
		X[i] = randNumber;
	}

	


}




