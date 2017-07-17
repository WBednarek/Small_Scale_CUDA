#include "OpenMP.h"
#define export OMP_NUM_THREADS = 4

OpenMP::OpenMP()
{
}

OpenMP::~OpenMP()
{
}



void OpenMP::OpenMPSolver(ReadMatrixCSR & mat, std::vector<double>& X, std::vector<double>& Y,  int threadsNumber, double & timeToComplete, unsigned int numberOfMatrixXColumn)
{
	int numberOfRows = mat.getN();


	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point  end;
	double fullTime = 0.0;
	start = std::chrono::high_resolution_clock::now();

	

	#pragma omp parallel for num_threads(threadsNumber)
	for (int i = 0; i < numberOfMatrixXColumn; ++i)
	{
		for (int index = 0; index < mat.getM(); ++index)
		{
			unsigned int upperBound = mat.getSelectedElementIRP(index + 1);

			for (int k = mat.getIRP().at(index); k < upperBound; ++k)
			{
				//Y[outputIndex + index * numberOfRows] += mat.getSelectedElementAS(k) * X[mat.getSelectedElementJA(k) + index * numberOfRows];
				Y[index + i * numberOfRows] += mat.getSelectedElementAS(k) * X[mat.getSelectedElementJA(k)+ i * numberOfRows];
			}
		}
	}

		

	end = std::chrono::high_resolution_clock::now();
	fullTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
	timeToComplete = fullTime;
	

}




void OpenMP::OpenMPSolver(ReadMatrixELL & mat, std::vector<double>& X, std::vector<double>& Y, int threadsNumber, double & timeToComplete, unsigned int numberOfMatrixXColumn)
{	

	/*
	
	if ((mat.getM() == 0) || (mat.getN() == 0) || (mat.getNumberOfElementsInTheBiggestRow() == 0) || (Y.size() != mat.getN()))
	{
		std::cout << "EXCEPTION ERROR; MATRIX SIZE MISMATCH" << std::endl;
		throw std::exception("EXEPTION ERROR; MATRIX SIZE MISMATCH");
	}
	
	*/
std::chrono::high_resolution_clock::time_point start;
std::chrono::high_resolution_clock::time_point  end;
double fullTime = 0.0;
start = std::chrono::high_resolution_clock::now();
int numberOfRows = mat.getM();
int numOfElemsInTheBiggestRow = mat.getNumberOfElementsInTheBiggestRow();
	



#pragma omp parallel for num_threads(threadsNumber)
for (int i = 0; i < numberOfMatrixXColumn; ++i)
{
	for (int outputIndex = 0; outputIndex < numberOfRows; ++outputIndex)
	{
		for (int k = 0; k < numOfElemsInTheBiggestRow; ++k)
		{
			long long inputIndex = outputIndex * numOfElemsInTheBiggestRow + k;
			Y[outputIndex + i * numberOfRows] += mat.getSelectedElementAS(inputIndex) * X[mat.getSelectedElementJA(inputIndex) + i * numberOfRows] ;
		}

	}	

}





end = std::chrono::high_resolution_clock::now();
fullTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000.0;
timeToComplete = fullTime;
//std::cout << " TIME OF EELPACK OPENMPfewwefwefwef SOLUTION: " << fullTime << std::endl;
//std::cout << " Threads number: " << omp_get_num_threads() << std::endl;
//std::cout << " Processors number: " << omp_get_num_procs() << std::endl;

}




