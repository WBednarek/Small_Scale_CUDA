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
	sortInputMatrixByRows();
	calculateELLValues();

}


void ReadMatrixELL::resizeMatrices()
{
	//Resize JA and AS	//
	(*this).JA = new int*[M];
	(*this).AS = new double*[M];
	for (int i = 0; i < N; ++i)
	{
		JA[i] = new int[N];
		AS[i] = new double[N];
	}

	rowsAndValues.resize(nz);

}

void ReadMatrixELL::sortInputMatrixByRows()
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


void ReadMatrixELL::calculateELLValues()
{
	
	calclateNonZeroValuesInRows();
	fillZeros<int>(nonZeroValuesInRows);

	int nzValueCounter = 0;
	for (int i = 0; i < nonZeroValuesInRows.size(); ++i)
	{
		for (int j = 0; j < nonZeroValuesInRows[i]; ++j)
		{
			//Create JA matrix, get<2> is matrix value
			AS[i][j] = std::get<2>(rowsAndValues[nzValueCounter]);
			//Create JA matrix, get<1> is row number, + 1 because values are displayed in the Matlab style
			JA[i][j] = std::get<1>(rowsAndValues[nzValueCounter]) + 1;

			nzValueCounter++;
		}

	}	

	std::cout << "JA" << std::endl;
	displayELLMatrix<int>(JA);

	std::cout << "AS" << std::endl;
	displayELLMatrix<double>(AS);
		

}


//displaying 2D matrix
template<typename TYPE>
void ReadMatrixELL::displayELLMatrix(TYPE ** matrix)
{

	//display AS
	for (int i = 0; i < nonZeroValuesInRows.size(); ++i)
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


void ReadMatrixELL::calclateNonZeroValuesInRows()
{
	int nonZeroValuesInThisRow = 0;
	nonZeroValuesInRows.resize(N);

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
			nonZeroValuesInRows[k] = nonZeroValuesInThisRow;
			nonZeroValuesInThisRow = 0;
			++k;
			++i;


		}

	}

	//Add number of values to last row
	nonZeroValuesInRows[k] = nonZeroValuesInThisRow + 1;

}



void ReadMatrixELL::displayPointerArray(int * arr)
{
	for (int i = 0; i < nz; i++) {

		std::cout << *(arr + i) << " ";
	}
	std::cout << std::endl;
}

//std::vector<int> ReadMatrixELL::getIRP()
//{
//	std::vector<int> irpVector(IRP, IRP + M + 1);
//	return irpVector;
//}
//
std::vector<double> ReadMatrixELL::getAS()
{
	std::vector<double> asVector(*AS, *AS + nz);
	return  asVector;
}

//std::vector<int> ReadMatrixELL::getJA()
//{
//	std::vector<int> jaVector(JA, JA + nz);
//	return jaVector;
//}

int ReadMatrixELL::getM()
{
	return M;
}

int ReadMatrixELL::getNZ()
{
	return nz;
}

int ReadMatrixELL::getN()
{
	return N;
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
	freeMemory<int>(JA);
	freeMemory<double>(AS);
	
}
