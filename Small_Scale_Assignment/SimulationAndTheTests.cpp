#include "SimulationAndTheTests.h"


template<class classType>
SimulationAndTheTests<classType>::SimulationAndTheTests()
{
}

template<class classType>
SimulationAndTheTests<classType>::~SimulationAndTheTests()
{
}



template<class classType>
auto SimulationAndTheTests<classType>::calcuatePerformance(classType & matrix, double completionTime)
{
	return 2.0 * matrix.getNZ() / completionTime / 1000000.0;
}


template<class classType>
void SimulationAndTheTests<classType>::runCUDA(classType & mat, int numberOfThreads, int sizeOfBlock, int maximumBlocksdouble, double & timeToComplete)
{
	std::vector<double> X(mat.getN());
	std::vector<double> result(mat.getN());
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point  end;
	double complete;
	try
	{
		makeVector_X(X, mat.getNZ(), 2);
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
void SimulationAndTheTests<classType>::runOpenMP(classType & mat, int numberOfThreads, int numberOfMatrixXColumns, int numberOfSimulationRuns)
{

	ReadMatrixELL matrixELL1("cage4.mtx");
	OpenMP omp;

	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point  end;
	double complete;
	double timeToComplete = 0;
	std::vector<double> X(mat.getN());
	std::vector<double> Y(mat.getN());
	makeVector_X(X, mat.getN(), numberOfMatrixXColumns);
	//makeVector_X(mat.getN(), numberOfMatrixXColumns);
	//std::vector<double> Y(mat.getN() *  numberOfMatrixXColumns);
	double avarageTime = 0;

	for (int i = 0; i < numberOfSimulationRuns; ++i)
	{

		start = std::chrono::high_resolution_clock::now();
		//Compute OpenMP matrix-matrix, product
		omp.OpenMPSolver(matrixELL1, X, Y, numberOfThreads, timeToComplete, numberOfMatrixXColumns);


		end = std::chrono::high_resolution_clock::now();
		complete = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
		//std::chrono::duration<double> diff = end - start;
		//std::cout << "Current Matrix Type: " << typeid(classType).name() << std::endl;
		avarageTime += timeToComplete;

		try
		{
			
			

		}
		catch (std::exception & e)
		{
			std::cout << "Standard exception: " << e.what() << std::endl;
		}
	}
	avarageTime /= numberOfSimulationRuns;
	std::cout << "Average time in milliseconds: " << avarageTime << " ms\n";
	std::cout << "OpenMP performance is: " << calcuatePerformance(mat, avarageTime) << " GFLOPS" << std::endl;
	std::cout << std::endl;
	


}

template<class classType>
void SimulationAndTheTests<classType>::makeVector_X(std::vector<double>& X, int sizeOfMaxtixRow, int numberOfMatrixXColumns)
{
	//X.resize(sizeOfMaxtixRow * numberOfMatrixXColumns);
	unsigned seed;
	int randNumber;
	//for (int i = 0; i < sizeOfMaxtixRow * numberOfMatrixXColumns; ++i)
	for (int i = 0; i < sizeOfMaxtixRow; ++i)
	{
		seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine dre(seed);
		std::uniform_real_distribution<double> gen(0, 5);
		randNumber = gen(dre);
		X[i] = 1.0;
	}

	


}




