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
	return (2.0 * matrix.getNZ()*10.0) / (completionTime);
}


template<class classType>
void runSimulation<classType>::runCUDA(classType & mat, int numberOfThreads, int sizeOfBlock, int maximumBlocks, double & timeToComplete)
{

	std::vector<double> result(mat.getN());

	try
	{
		makeVector(mat.getNZ());
		auto start = std::chrono::high_resolution_clock::now();
		//Compute CUDA matrix-matrix, product
		CUDASolver(mat, X, result, sizeOfBlock, maximumBlocks, timeToComplete);

		auto end = std::chrono::high_resolution_clock::now();
		auto complete = (double) std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
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

	/*if (std::is_same<classType, ReadMatrixCSR>::value)
	{
		std::cout << "CUDA performance is: " << calcuatePerformance<ReadMatrixCSR>(mat, complete) << " GFLOPS" << std::endl;

	}
	
	if (std::is_same<classType, ReadMatrixELL>::value)
	{
		std::cout << "CUDA performance is: " << calcuatePerformance<ReadMatrixELL>(mat, complete) << " GFLOPS" << std::endl;
	}
	
	*/
	
}


template<class classType>
void runSimulation<classType>::runOpenMP(classType & mat, int numberOfThreads, double & timeToComplete)
{

	std::vector<double> result(mat.getN());
	OpenMP omp;
	try
	{
		makeVector(mat.getNZ());
		auto start = std::chrono::high_resolution_clock::now();
		//Compute OpenMP matrix-matrix, product
		//CUDASolver(mat, X, result, sizeOfBlock, maximumBlocks, timeToComplete);
		omp.OpenMPSolver(mat, X, result, numberOfThreads, timeToComplete);
		
		
	

		auto end = std::chrono::high_resolution_clock::now();
		auto complete = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		std::chrono::duration<double> diff = end - start;
		std::cout << "Current Matrix Type: " << typeid(classType).name() << std::endl;
		std::cout << "Time in milliseconds: " << diff.count() << " ms\n";
		std::cout << "OpenMP performance is: " << calcuatePerformance(mat, complete) << " GFLOPS" << std::endl;
		std::cout << std::endl;

	}
	catch (std::exception & e)
	{
		std::cout << "Standard exception: " << e.what() << std::endl;
	}

	/*if (std::is_same<classType, ReadMatrixCSR>::value)
	{
	std::cout << "CUDA performance is: " << calcuatePerformance<ReadMatrixCSR>(mat, complete) << " GFLOPS" << std::endl;

	}

	if (std::is_same<classType, ReadMatrixELL>::value)
	{
	std::cout << "CUDA performance is: " << calcuatePerformance<ReadMatrixELL>(mat, complete) << " GFLOPS" << std::endl;
	}

	*/

}

template<class classType>
void runSimulation<classType>::makeVector(int sizeOfVector)
{
	X.resize(sizeOfVector);
	unsigned seed;
	int randNumber;
	for (int i = 0; i < sizeOfVector; ++i)
	{
	    seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine dre(seed);
        std::uniform_real_distribution<double> gen(0, 5);
		randNumber = gen(dre);

		X[i] = randNumber;
	}
}




