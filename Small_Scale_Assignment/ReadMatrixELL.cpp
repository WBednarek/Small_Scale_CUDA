#include "ReadMatrixELL.h"




ReadMatrixELL::ReadMatrixELL(std::string matrixName)
{

	if ((f = fopen(matrixName.c_str(), "r")) == NULL)
	{
		std::cout << "There is no matrix to read" << std::endl;
		exit(1);
	}


	if (mm_read_banner(f, &matcode) != 0)
	{
		printf("Could not process Matrix Market banner.\n");
		exit(1);
	}


	/*  This is how one can screen matrix types if their application */
	/*  only supports a subset of the Matrix Market data types.      */

	if (mm_is_complex(matcode) && mm_is_matrix(matcode) &&
		mm_is_sparse(matcode))
	{
		printf("Sorry, this application does not support ");
		printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
		exit(1);
	}

	/* find out size of sparse matrix .... */

	if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) != 0)
		exit(1);


	/* reseve memory for matrices */

	I = (int *)malloc(nz * sizeof(int));
	J = (int *)malloc(nz * sizeof(int));
	matrixValue = (double *)malloc(nz * sizeof(double));


	/* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
	/*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
	/*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */



	for (i = 0; i<nz; i++)
	{
		fscanf(f, "%d %d %lg\n", &I[i], &J[i], &matrixValue[i]);
		I[i]--;  /* adjust from 1-based to 0-based */
		J[i]--;
	}

	if (f != stdin) fclose(f);



	resizeMatrices();
	//std::cout << "This matrix has " << M << " rows " << N << " columns and  " << nz << " non zero values " << std::endl;
	getMatrixDataFromFileToTuple();
	sortInputMatrixByTheRows();
	calclateNonZeroValuesInTheRows();
	//fillZeros<int>(nonZeroValuesInTheRows);
	fillZerosOneDimensional();
	calculateELLValues();

}


void ReadMatrixELL::resizeMatrices()
{

	
	//Resize JA and AS	two dimensional//
	(*this).JA = new int*[M];
	(*this).AS = new double*[M];
	for (int i = 0; i < N; ++i)
	{
		JA[i] = new int[N];
		AS[i] = new double[N];
	}
	
	rowsAndValues.resize(nz);
	//Resize one dimensional
	long long vectorSize = static_cast<long long>(M) * static_cast<long long>(nz);
	(*this).JAOneDimensional = new int[vectorSize];
	(*this).ASOneDimensional = new double[vectorSize];

}

void ReadMatrixELL::sortInputMatrixByTheRows()
{

	//Lambda sorting
	std::sort(rowsAndValues.begin(), rowsAndValues.end(), [](auto const &left, auto const &right) {
		if (std::get<0>(left) < std::get<0>(right))
			return true;
		else if ((std::get<0>(left) == std::get<0>(right)) && (std::get<1>(left) < std::get<1>(right)))
			return true;
		else
			return false;

	});
}

void ReadMatrixELL::getMatrixDataFromFileToTuple()
{
	for (int i = 0; i < nz; ++i)
	{
		rowsAndValues[i] = std::make_tuple(I[i], J[i], matrixValue[i]);

	}

}


/*

//Preparing matrix fling it with zeros
template<typename TYPE>
void ReadMatrixELL::fillZeros(std::vector<TYPE> vec)
{
numOfElementsInTheBiggestRow =  *std::max_element(vec.begin(), vec.end());
//int numOfElementsInTheBiggestRow = std::distance(vec.begin(), maxElementIterator);



for (int i = 0; i < M; ++i)
{
for (int j = 0; j <= numOfElementsInTheBiggestRow; ++j)
{
AS[i][j] = 0;
JA[i][j] = 0;
}
}



}

*/






void ReadMatrixELL::fillZerosOneDimensional()
{
	long long vectorSize = static_cast<long long>(M) * static_cast<long long>(nz);
	for (long long i = 0; i < vectorSize; ++i)
	{
		ASOneDimensional[i] = 0;
		JAOneDimensional[i] = 0;
	}
}


void ReadMatrixELL::calclateNonZeroValuesInTheRows()
{
	int nonZeroValuesInThisRow = 0;
	nonZeroValuesInTheRows.resize(N);
	int k = 0;
	int i = 0;

	while (i < nz - 1)
	{

		//Checking if number of row hasn't changed, This won't get last row
		if (std::get<0>(rowsAndValues[i]) == std::get<0>(rowsAndValues[i + 1]))
		{

			++nonZeroValuesInThisRow;
			++i;

		}
		else
		{
			++nonZeroValuesInThisRow; //
			nonZeroValuesInTheRows[k] = nonZeroValuesInThisRow;
			nonZeroValuesInThisRow = 0;
			++k;
			++i;


		}

	}

	//Add number of values to last row
	nonZeroValuesInTheRows[k] = nonZeroValuesInThisRow + 1;


	numOfElementsInTheBiggestRow = *std::max_element(nonZeroValuesInTheRows.begin(), nonZeroValuesInTheRows.end());

}


void ReadMatrixELL::calculateELLValues()
{
	int nzValueCounter = 0;
	long long idx = 0;
	for (int i = 0; i < nonZeroValuesInTheRows.size(); ++i)
	{
		for (int j = 0; j < nonZeroValuesInTheRows[i]; ++j)
		{

			/*
			//Create JA matrix, get<2> is matrix value
			AS[i][j] = std::get<2>(rowsAndValues[nzValueCounter]);
			//Create JA matrix, get<1> is row number, + 1 because values are displayed in the Matlab style
			JA[i][j] = std::get<1>(rowsAndValues[nzValueCounter]) + 1;
			
			*/
			

			//One dimensional case
			idx = i * static_cast<long long>(numOfElementsInTheBiggestRow) + j;
			ASOneDimensional[idx] = std::get<2>(rowsAndValues[nzValueCounter]);
			//Store results in C++ style, add one if you want to get Matlab one
			JAOneDimensional[idx] = std::get<1>(rowsAndValues[nzValueCounter]);

			nzValueCounter++;
		}

	}


	/**
		//Test displaying you can delete that
	std::cout << "JA" << std::endl;
	displayELLMatrix<int>(JA);

	std::cout << "AS" << std::endl;
	displayELLMatrix<double>(AS);

	saveOneDimensionalELLMatrix<int>(JAOneDimensional, "JA");
	saveOneDimensionalELLMatrix<double>(ASOneDimensional, "AS");


	**/

}

//displaying 2D matrix
template<typename TYPE>
void ReadMatrixELL::displayELLMatrix(TYPE ** matrix)
{
	//display AS or JA
	for (int i = 0; i < nonZeroValuesInTheRows.size(); ++i)
	{

		for (int j = 0; j < numOfElementsInTheBiggestRow; ++j)
		{
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;

	}
	std::cout << std::endl;
	std::cout << std::endl;

}


//displaying 1D matrix
template<typename TYPE>
void ReadMatrixELL::saveOneDimensionalELLMatrix(TYPE * matrix, std::string name)
{
	std::ofstream data(name + " ELLONEDIMENSIONAL.txt");
		data << name << std::endl;
		long long idx = 0;
	//save to file one-dimensional AS or JA
	for (int i = 0; i < nonZeroValuesInTheRows.size(); ++i)
	{
		for (int j = 0; j < numOfElementsInTheBiggestRow; ++j)
		{
			idx = i * static_cast<long long>(numOfElementsInTheBiggestRow) + j;
			data << matrix[idx]<< " ";
		}
		data << std::endl;

	}
	data << std::endl;
	data << std::endl;

}

void ReadMatrixELL::displayPointerArray(int * arr)
{
	for (int i = 0; i < nz; i++) {

		std::cout << *(arr + i) << " ";
	}
	std::cout << std::endl;
}


std::vector<double> ReadMatrixELL::getAS()
{
	std::vector<double> asVector(ASOneDimensional, ASOneDimensional + nonZeroValuesInTheRows.size() * numOfElementsInTheBiggestRow);
	return  asVector;
}

std::vector<int> ReadMatrixELL::getJA()
{
	std::vector<int> jaVector(JAOneDimensional, JAOneDimensional + nonZeroValuesInTheRows.size() * numOfElementsInTheBiggestRow);
	return jaVector;
}

int ReadMatrixELL::getM()
{
	checkM = M;
	return checkM;
}

int ReadMatrixELL::getNZ()
{
	return nz;
}

int ReadMatrixELL::getN()
{
	return N;
}

int ReadMatrixELL::getNumberOfElementsInTheBiggestRow()
{
	return numOfElementsInTheBiggestRow;
}

double ReadMatrixELL::getSelectedElementAS(int elemIndex) const
{
	return ASOneDimensional[elemIndex];
}

int ReadMatrixELL::getSelectedElementJA(int elemIndex) const
{
	return JAOneDimensional[elemIndex];
}




template<typename TYPE>
void ReadMatrixELL::freeMemory(TYPE ** matrix)
{
	for (int i = 0; i<M; i++)
		delete[] matrix[i];
	delete[] matrix;
}


ReadMatrixELL::~ReadMatrixELL()
{

	/*
	
	freeMemory<int>(JA);
	freeMemory<double>(AS);
	*/
	
	delete[] ASOneDimensional;
	delete[] JAOneDimensional;
	
}
