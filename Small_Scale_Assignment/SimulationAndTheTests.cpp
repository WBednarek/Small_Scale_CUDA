#include "SimulationAndTheTests.h"


template<class classType>
SimulationAndTheTests<classType>::SimulationAndTheTests()
{
	matricesNumberOfColumns = { 2,3,4,8 };
}

template<class classType>
SimulationAndTheTests<classType>::~SimulationAndTheTests()
{
}



template<class classType>
auto SimulationAndTheTests<classType>::calcuatePerformance(int NZ, double completionTime)
{
	return 2.0 * (float) NZ / completionTime / (float)1000.0;
}


template<class classType>
void SimulationAndTheTests<classType>::runCUDA(classType & mat, int numberOfThreads, int sizeOfBlock, int maximumBlocksdouble, double & timeToComplete)
{
	//results.imbue(std::locale(std::cout.getloc(), new DecimalSeparator<char>(',')));
	//results.imbue(std::locale(std::cout.getloc(), new DecimalSeparator<char>(',')));
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
		std::cout << "CUDA performance is: " << calcuatePerformance(mat, complete) << " MFLOPS" << std::endl;

		
		
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
		std::cout << "CUDA performance is: " << calcuatePerformance<ReadMatrixCSR>(mat, complete) << " MFLOPS" << std::endl;

	}
	
	
	
	*/
	
}




template<class classType>
void SimulationAndTheTests<classType>::runOpenMP(classType & mat, int numberOfThreads, int numberOfSimulationRuns)
{

	ReadMatrixELL matrixELL1("cage4.mtx");
	OpenMP omp;


	double timeToComplete = 0;
	std::vector<double> X;
	std::vector<double> Y;
//	makeVector_X(X, mat.getN(), numberOfMatrixXColumns);
	//makeVector_X(mat.getN(), numberOfMatrixXColumns);
	//std::vector<double> Y(mat.getN() *  numberOfMatrixXColumns);
	double avarageTime = 0;
	double avaragePerfomrance = 0;
	


	std::string resultsFileName = (std::string) typeid(classType).name() + ".xls";
	results.open(resultsFileName); 
	results << "Number of matrix columns\t Performance MFLOPS" << std::endl;

	for (int k = 0; k < matricesNumberOfColumns.size(); ++k )
	{
		int NZtimesNumberOfMatrixColumns = mat.getNZ() * matricesNumberOfColumns[k];
		X.resize(NZtimesNumberOfMatrixColumns);
		Y.resize(NZtimesNumberOfMatrixColumns);
		std::fill(X.begin(), X.end(), 1.0);
		std::fill(Y.begin(), Y.end(), 0);

		for (int i = 0; i < numberOfSimulationRuns; ++i)
		{

			std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
			//Compute OpenMP matrix-matrix, product
			omp.OpenMPSolver(matrixELL1, X, Y, numberOfThreads, timeToComplete, matricesNumberOfColumns[k]);


			std::chrono::high_resolution_clock::time_point  end = std::chrono::high_resolution_clock::now();
			double complete = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
			//std::chrono::duration<double> diff = end - start;
			//std::cout << "Current Matrix Type: " << typeid(classType).name() << std::endl;
			avarageTime += timeToComplete;
			//std::cout << "timeToComplete in milliseconds: " << timeToComplete << " ms\n";
			avaragePerfomrance += calcuatePerformance(NZtimesNumberOfMatrixColumns, timeToComplete);
			//std::cout << "calcuatePerformance " << calcuatePerformance(NZtimesNumberOfMatrixColumns, timeToComplete) << " MFLOPS\n";

			/*
			try
			{



			}
			catch (std::exception & e)
			{
				std::cout << "Standard exception: " << e.what() << std::endl;
			}
			*/
			
			
			

		}
		//results << "Number of columns " << matricesNumberOfColumns[k] << "\n";
		avarageTime /= numberOfSimulationRuns;
		avaragePerfomrance /= numberOfSimulationRuns;
		results << matricesNumberOfColumns[k];
		results << "\t" << avaragePerfomrance << "\n";
		
		avarageTime = 0;
		avaragePerfomrance = 0;
	}
	
	//std::cout << "Average time in milliseconds: " << avarageTime << " ms\n";
	//std::cout << "OpenMP average performance is: " << avaragePerfomrance << " MFLOPS" << std::endl;
	//std::cout << "OpenMP average direct performance is: " << calcuatePerformance(NZtimesNumberOfMatrixColumns, avarageTime) << " MFLOPS" << std::endl;
	
	
	


}

template<class classType>
void SimulationAndTheTests<classType>::makeVector_X(std::vector<double>& X, int sizeOfMaxtixRow, int numberOfMatrixXColumns)
{
	//X.resize(sizeOfMaxtixRow * numberOfMatrixXColumns);
	unsigned seed;
	int randNumber;
	
	//for (int i = 0; i < sizeOfMaxtixRow; ++i)
	for (int i = 0; i < sizeOfMaxtixRow * numberOfMatrixXColumns; ++i)
	{
		seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine dre(seed);
		std::uniform_real_distribution<double> gen(0, 5);
		randNumber = gen(dre);
		X[i] = 1.0;
	}

	


}




