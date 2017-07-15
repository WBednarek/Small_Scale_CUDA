#include "OpenMP.h"

OpenMP::OpenMP()
{
}

OpenMP::~OpenMP()
{
}

void OpenMP::OpenMPSolver(ReadMatrixCSR & mat, std::vector<double>& X, std::vector<double>& Y, unsigned int threadsNumber, double & timeToComplete)
{
	std::fill(Y.begin(), Y.end(), 0);
	unsigned int index = 0;
	unsigned int upperBound = 0;
	#pragma omp parallel for num_threads(threadsNumber)
	for(int index = 0; index < mat.getM(); ++index)
	{
	upperBound = mat.getIRP().at(index + 1);

		for (int k = mat.getIRP().at(index); k < upperBound; ++k)
		{
			Y[index] += mat.getAS().at(k) * X[mat.getJA().at(k)];
		}
	}

}


void OpenMP::OpenMPSolver(ReadMatrixELL & mat, std::vector<double>& X, std::vector<double>& Y, unsigned int threadsNumber, double & timeToComplete, unsigned int numberOfMatrixXColumn)
{	
std::fill(Y.begin(), Y.end(), 0);
std::chrono::high_resolution_clock::time_point start;
std::chrono::high_resolution_clock::time_point  end;
double fullTime = 0.0;
start = std::chrono::high_resolution_clock::now();


unsigned int numberOfRows = mat.getM();
unsigned int numOfElemsInTheBiggestRow = mat.getNumberOfElementsInTheBiggestRow();
	#pragma omp parallel for num_threads(threadsNumber)	
	for (int outputIndex = 0; outputIndex < numberOfRows; ++outputIndex)
	{
		for (int k = 0; k < numOfElemsInTheBiggestRow; ++k)
		{
			unsigned int inputIndex = outputIndex * numOfElemsInTheBiggestRow + k;
			Y[outputIndex] += mat.getSelectedElementAS(inputIndex) * X[mat.getSelectedElementJA(inputIndex)];
		}

}
/*
for (int i = 1; i <= numberOfMatrixXColumn; ++i)
{

inputIndex = 0;

}
*/
	
	
end = std::chrono::high_resolution_clock::now();
fullTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
std::cout << " TIME OF EELPACK OPENMPfewwefwefwef SOLUTION: " << fullTime << std::endl;
}




