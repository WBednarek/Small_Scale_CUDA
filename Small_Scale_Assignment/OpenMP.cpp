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


#pragma omp parellel for num_threads(threadsNumber)
	unsigned int outputIndex = 0;
	unsigned int inputIndex = 0;
	while (outputIndex < mat.getM())
	{
		for (int k = 0; k < mat.getNumberOfElementsInTheBiggestRow(); ++k)
		{
			inputIndex = outputIndex * mat.getNumberOfElementsInTheBiggestRow() + k;
			Y[outputIndex] += mat.getAS().at(inputIndex) * X[mat.getJA().at(inputIndex)];
		}
		++outputIndex;
	}


}




