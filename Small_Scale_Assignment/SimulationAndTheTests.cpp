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
void SimulationAndTheTests<classType>::runCUDA(classType & mat , int numberOfThreads, int sizeOfBlock, int maximumBlocks, int numberOfSimulationRuns)
{

	std::ofstream results;
	ReadMatrixELL matrixELL1("cage4.mtx");
	OpenMP omp;


	double timeToComplete = 0;
	double avarageTime = 0;
	double avaragePerfomrance = 0;
	std::vector<double> X;
	std::vector<double> Y;

	int NZtimesNumberOfMatrixColumns = mat.getNZ(); // *matricesNumberOfColumns[k];
	X.resize(mat.getNZ()); // X.resize(NZtimesNumberOfMatrixColumns);
	Y.resize(mat.getNZ()); // Y.resize(NZtimesNumberOfMatrixColumns);
	std::fill(X.begin(), X.end(), 1.0);
	std::fill(Y.begin(), Y.end(), 0);

	
	std::string resultsFileName = (std::string) typeid(classType).name() + "CUDA" + mat.getMatrixName() + ".xls";
	results.open(resultsFileName);
	//In the .xls files next column separator is "\t" in Excel. 
	//When we want to use .csv extension files, depend on system separator could be "," or ";" .
	results << "CUDA" << std::endl;
	results << "Number of matrix columns\t Performance MFLOPS" << std::endl;
	for (int k = 0; k < matricesNumberOfColumns.size(); ++k)
	{
		for (int i = 0; i < numberOfSimulationRuns; ++i)
		{

			std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

			//Compute CUDA matrix by "tall" dense matrix multiplication product
			CUDASolver(mat, X, Y, sizeOfBlock, maximumBlocks, timeToComplete);

			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
			double complete = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
			avarageTime += complete;
			avaragePerfomrance += calcuatePerformance(NZtimesNumberOfMatrixColumns, complete);

		}
		avarageTime /= numberOfSimulationRuns;
		avaragePerfomrance /= numberOfSimulationRuns;
		//Save results to the file
		results << matricesNumberOfColumns[k];
		results << "\t" << avaragePerfomrance << "\n";

		avarageTime = 0;
		avaragePerfomrance = 0;
	}



/*


int NZtimesNumberOfMatrixColumns = mat.getNZ() * matricesNumberOfColumns[k];
X.resize(NZtimesNumberOfMatrixColumns);
Y.resize(NZtimesNumberOfMatrixColumns);
std::fill(X.begin(), X.end(), 1.0);
std::fill(Y.begin(), Y.end(), 0);

}

results << "\n\n\n\n";


*/
}



template<class classType>
void SimulationAndTheTests<classType>::runOpenMP(classType & mat, int numberOfThreads, int numberOfSimulationRuns)
{
	std::ofstream results;
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
	


	std::string resultsFileName = (std::string) typeid(classType).name() + "OpenMP" + mat.getMatrixName() + ".xls";
	

	results.open(resultsFileName); 
	results << "OpenMP" << std::endl;
	//In the .xls files next column separator is "\t" in Excel. 
	//When we want to use .csv extension files, depend on system separator could be "," or ";" .
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
			//Compute OpenMP matrix by "tall" dense matrix multiplication product
			omp.OpenMPSolver(mat, X, Y, numberOfThreads, timeToComplete, matricesNumberOfColumns[k]);


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
	results << "\n\n\n\n";
	
	


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




