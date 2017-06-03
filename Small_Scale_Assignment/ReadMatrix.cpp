#include "ReadMatrix.h"




ReadMatrix::ReadMatrix(std::string matrixName)
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

	mm_write_banner(stdout, matcode);
	mm_write_mtx_crd_size(stdout, M, N, nz);
	for (i = 0; i<nz; i++)
		fprintf(stdout, "%d %d %20.19g\n", I[i] + 1, J[i] + 1, matrixValue[i]);


	std::cout << "This is N " << N << " M " << M << " and NZ " << nz << std::endl;

	rowsAndValues.resize(nz);
	for (int i = 0; i < nz; ++i)
	{
		rowsAndValues[i] = std::make_pair(I[i], matrixValue[i]);
	}

	std::sort(rowsAndValues.begin(), rowsAndValues.end(), [](const std::pair<int, double> &left, const std::pair<int, double> &right) {
		return left.first < right.first;
	});

	calculateCSRValues();

}


void ReadMatrix::calculateCSRValues()
{
	(*this).IRP = new int [M +1];
	(*this).JA = new int[N + 1];
	(*this).AS = new double[nz];
	(*this).IRP[0] = 0;
	(*this).IRP[M] = nz;
	int k = 0;
	int j = 0;
	
	int columnNumber = J[0];

	for (int i = 0; i < nz; ++i)
	{
		//Create AS vector
		AS[i] = rowsAndValues[i].second;

		if (rowsAndValues[i].first == j)
		{
			k++;
		}
		else
		{
			j++;
			IRP[j] = k;	
			while (rowsAndValues[i].first != j)
			{
				j++;
				IRP[j] = k;
			}
		k++;
		}
	}

	std::cout << "IRP: ";
	displayPointerArray(IRP);
	std::cout << "AS: ";

	std::cout << "JA: ";
	displayPointerArray(J);
	
	int sa = 12;
	double wd = 0.32;
	wd += sa;

}

void ReadMatrix::displayPointerArray(int * arr)
{
	for (int i = 0; i < nz; i++) {

		std::cout << *(arr + i) << " ";
	}
	std::cout << std::endl;
}

std::vector<int> ReadMatrix::getIRP()
{
	std::vector<int> irpVector(IRP, IRP + M + 1);
	return irpVector;
}

std::vector<double> ReadMatrix::getAS()
{
	std::vector<double> asVector(AS, AS + nz);
	return  asVector;
}

std::vector<int> ReadMatrix::getJA()
{
	std::vector<int> jaVector(J, J + nz);
	return jaVector;
}






ReadMatrix::~ReadMatrix()
{

}
