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

#pragma omp parellel for num_threads(threadsNumber)
unsigned int index = 0;
unsigned int upperBound = 0;
while (index < mat.getM())
{
upperBound = mat.getIRP().at(index + 1);

	for (int k = mat.getIRP().at(index); k < upperBound; ++k)
	{
		Y[index] += mat.getAS().at(k) * X[mat.getJA().at(k)];
	}
++index;
}
}


void OpenMP::OpenMPSolver(ReadMatrixELL & mat, std::vector<double>& X, std::vector<double>& Y, unsigned int threadsNumber, double & timeToComplete)
{	
std::fill(Y.begin(), Y.end(), 0);
std::chrono::high_resolution_clock::time_point start;
std::chrono::high_resolution_clock::time_point  end;
double fullTime = 0.0;
start = std::chrono::high_resolution_clock::now();
unsigned int numberOfRows = mat.getM();
unsigned int numOfElemsInTheBiggestRow = mat.getNumberOfElementsInTheBiggestRow();



	#pragma omp parallel for num_threads(threadsNumber)	
	unsigned int outputIndex = 0;
	unsigned int inputIndex = 0;
	while (outputIndex < numberOfRows)
	{
		for (int k = 0; k < numOfElemsInTheBiggestRow; ++k)
		{
			inputIndex = outputIndex * numOfElemsInTheBiggestRow + k;
			Y[outputIndex] += mat.getSelectedElementAS(inputIndex) * X[mat.getSelectedElementJA(inputIndex)];
		}
		++outputIndex;
	}
	
	end = std::chrono::high_resolution_clock::now();
	fullTime = timeToComplete = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
	std::cout << " TIME OF EELPACK OPENMP SOLUTION: " << fullTime << std::endl;
}




