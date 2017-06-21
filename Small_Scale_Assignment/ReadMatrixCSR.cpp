#include "ReadMatrixCSR.h"




ReadMatrixCSR::ReadMatrixCSR(std::string matrixName)
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



	/************************/
	/* now write out matrix */
	/************************/
	//mm_write_banner(stdout, matcode);
	//mm_write_mtx_crd_size(stdout, M, N, nz);
	/*
		
	for (i = 0; i<nz; i++)
		fprintf(stdout, "%d %d %20.19g\n", I[i] + 1, J[i] + 1, matrixValue[i]);
	*/



	(*this).IRP = new int[M + 1];
	(*this).JA = new int[nz];
	(*this).AS = new double[nz];
	(*this).IRP[0] = 0;
	(*this).IRP[M] = nz;

	std::cout << "This matrix has " << N << " rows " << M << " columns and  " << nz <<" non zero values " << std::endl;

	rowsAndValues.resize(nz);
	for (int i = 0; i < nz; ++i)
	{
		rowsAndValues[i] = std::make_tuple(I[i], J[i], matrixValue[i]);
		
	}

	//Lambda sorting
	std::sort(rowsAndValues.begin(), rowsAndValues.end(), []( auto const &left, auto const &right) {
		if (std::get<0>(left) < std::get<0>(right))
			return true;
		else if ( (std::get<0>(left) == std::get<0>(right)) && (std::get<1>(left) < std::get<1>(right)) )
			return true;
		else
			return false;
						
	});

	calculateCSRValues();

}


void ReadMatrixCSR::calculateCSRValues()
{
	
	int IRPValue = 0;
	int IRPPos = 0;
	
	int columnNumber = J[0];

	for (int i = 0; i < nz; ++i)
	{
		//Create AS vector
		AS[i] = std::get<2>(rowsAndValues[i]);

		//Create JA vector
		JA[i] = std::get<1>(rowsAndValues[i]);

		//Create IRP vector
		if (std::get<0>(rowsAndValues[i]) == IRPPos)
		{
			IRPValue++;
		}
		else
		{
			IRPPos++;
			IRP[IRPPos] = IRPValue;	
			while (std::get<0>(rowsAndValues[i]) != IRPPos)
			{
				IRPPos++;
				IRP[IRPPos] = IRPValue;
			}
		IRPValue++;
		}
	}


}

void ReadMatrixCSR::displayPointerArray(int * arr)
{
	for (int i = 0; i < nz; i++) {

		std::cout << *(arr + i) << " ";
	}
	std::cout << std::endl;
}

std::vector<int> ReadMatrixCSR::getIRP()
{
	std::vector<int> irpVector(IRP, IRP + M + 1);
	return irpVector;
}

std::vector<double> ReadMatrixCSR::getAS()
{
	std::vector<double> asVector(AS, AS + nz);
	return  asVector;
}

std::vector<int> ReadMatrixCSR::getJA()
{
	std::vector<int> jaVector(JA, JA + nz);
	return jaVector;
}

int ReadMatrixCSR::getM()
{
	return M;
}

int ReadMatrixCSR::getNZ()
{
	return nz;
}

int ReadMatrixCSR::getN()
{
	return N;
}







ReadMatrixCSR::~ReadMatrixCSR()
{
	delete[] (JA);
	delete[] (IRP);
	delete[] (AS);
}
