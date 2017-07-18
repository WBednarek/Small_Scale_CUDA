#include "ReadMatrixCSR.h"




ReadMatrixCSR::ReadMatrixCSR(std::string matrixName)
{
	(*this).matrixName = matrixName;
	MM_typecode matcode;
	FILE *f;
	int  *I, *J;
	int ret_code;
	double *matrixValue;


	std::vector<std::tuple<int, int, double> > rowsAndValues;

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
	/*  only supports a subset of the Matrix Market storeArray types.      */

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

	(*this).JA = std::make_unique<int[]>(nz);
	(*this).IRP = std::make_unique<int[]>(M + 1);
	(*this).AS = std::make_unique<double[]>(nz);
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

	calculateCSRValues(rowsAndValues, J);

	for (int i = 0; i < nz; ++i)
	{
		jaVector.push_back(JA.get()[i]);
	}

	for (int i = 0; i < nz; ++i)
	{
		asVector.push_back(AS.get()[i]);
	}

	for (int i = 0; i < M + 1; ++i)
	{
		irpVector.push_back(IRP.get()[i]);
	}

}

ReadMatrixCSR::ReadMatrixCSR(const ReadMatrixCSR & copy)
{

	M = copy.M;
	N = copy.N;
	nz = copy.nz;
	matrixName = copy.matrixName;


	(*this).JA = std::make_unique<int[]>(nz);
	(*this).AS= std::make_unique<double[]>(nz);
	(*this).IRP = std::make_unique<int[]>(M + 1);
	irpVector = copy.irpVector;
	asVector = copy.asVector;
	jaVector = copy.jaVector;

	for (int i = 0; i < nz; ++i)
	{
		JA.get()[i] = copy.JA.get()[i];
	}

	for (int i = 0; i < nz; ++i)
	{
		AS.get()[i] = copy.AS.get()[i];
	}

	for (int i = 0; i < M + 1; ++i)
	{
		IRP.get()[i] = copy.IRP.get()[i];
	}

}


void ReadMatrixCSR::calculateCSRValues(std::vector<std::tuple<int, int, double> > rowsAndValues,int * J)
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
	return irpVector;
}

std::vector<double> ReadMatrixCSR::getAS()
{

	return  asVector;
}

std::vector<int> ReadMatrixCSR::getJA()
{

	return jaVector;
}

double ReadMatrixCSR::getSelectedElementAS(int elemIndex) const
{
	return AS[elemIndex];
}

int ReadMatrixCSR::getSelectedElementJA(int elemIndex) const
{
	return JA[elemIndex];
}

int ReadMatrixCSR::getSelectedElementIRP(int elemIndex) const
{
	return IRP[elemIndex];
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

std::string ReadMatrixCSR::getMatrixName()
{
	return matrixName;
}







ReadMatrixCSR::~ReadMatrixCSR()
{
	
}
