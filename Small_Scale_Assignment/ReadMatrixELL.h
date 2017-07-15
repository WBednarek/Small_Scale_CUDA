#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "mmio.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <tuple>

class ReadMatrixELL
{


	private:
		int ret_code;
		MM_typecode matcode;
		FILE *f;
		// M - Rows, N - Columns , Nz - non zero values
		int M, N, nz;
		int checkM; // something happens
		int i, *I, *J;
		double *matrixValue;
		std::string matrixName;
		int *JAOneDimensional;
		double *ASOneDimensional;
	
		int **JA;
		double **AS;
		//int *IRP; // Pointer to IRP array
		int maxRow, maxColumn;
		int numOfElementsInTheBiggestRow;
		std::vector<int> nonZeroValuesInTheRows;
		std::vector<std::tuple<int, int, double> > rowsAndValues;

		void resizeMatrices();

		void sortInputMatrixByTheRows();

		void getMatrixDataFromFileToTuple();

		
		void calculateELLValues();

		template<typename TYPE>
		void freeMemory(TYPE ** matrix);

		//Calculates noon zeros in selected row
		void calclateNonZeroValuesInTheRows();

	
		//Preparing matrix fling it with zeros
		//template<typename TYPE>
		//void fillZeros(std::vector<TYPE> vec);

		//Preparing one-dimensional matrix fling it with zeros
		void fillZerosOneDimensional();

		//displaying 2D matrix
		template<typename TYPE>
		void displayELLMatrix(TYPE ** matrix);

		template<typename TYPE>
		void saveOneDimensionalELLMatrix(TYPE * matrix, std::string name);

		void displayPointerArray(int * arr);



	public:

		ReadMatrixELL(std::string matrixName);
		~ReadMatrixELL();
		std::vector<double> getAS();
		std::vector<int> getJA();
		int getM();
		int getNZ();
		int getN();
		int getNumberOfElementsInTheBiggestRow();
		double getSelectedElementAS(int elemIndex) const;
		int getSelectedElementJA(int elemIndex) const;




};

