#include "runSimulation.h"



runSimulation::runSimulation()
{
}



runSimulation::~runSimulation()
{
}
auto runSimulation::calcuatePerformance(ReadMatrixCSR & matrix, double completionTime)
{
	return (2.0 * matrix.getNZ()*10) / (completionTime );
}

void runSimulation::runCSRCUDA(ReadMatrixCSR & mat, int numberOfThreads, int sizeOfBlock, int maximumBlocks, double & timeToComplete)
{
	makeVector(mat.getNZ());
	auto start = std::chrono::high_resolution_clock::now();
	CSRCUDASolver(mat, X, sizeOfBlock, maximumBlocks, timeToComplete);
	auto end = std::chrono::high_resolution_clock::now();
	auto complete = (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() ;
	std::chrono::duration<double> diff = end - start;
	std::cout << "Time in miliseconds"  << diff.count() * 100 << " ms\n";
	
	std::cout << "CUDA performance is " << calcuatePerformance(mat, complete) << " GFLOPS" << std::endl;

	
}



void runSimulation::makeVector(int sizeOfVector)
{
	X.resize(sizeOfVector);
	unsigned seed;
	int randNumber;
	for (int i = 0; i < sizeOfVector; ++i)
	{
	    seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine dre(seed);
        std::uniform_int_distribution<int> gen(0, 5);
		randNumber = gen(dre);

		X[i] = randNumber;
	}
}

