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
		int M, N, nz;
		int i, *I, *J;
		double *matrixValue;
		std::string matrixName;
		int ** JA;
		double ** AS;
		int * IRP; // Pointer to IRP array
		int maxRow, maxColumn;
		int numOfElementsInTheBiggestRow;
		std::vector<int> nonZeroValuesInRows;
		std::vector<std::tuple<int, int, double> > rowsAndValues;

		void resizeMatrices();

		void sortInputMatrixByRows();

		void getMatrixDataFromFileToTuple();

		
		void calculateELLValues();

		template<typename TYPE>
		void freeMemory(TYPE ** matrix);

		//Calculates noon zeros in selected row
		void calclateNonZeroValuesInRows();

		//Preparing matrix fling it with zeros
		//Preparing matrix fling it with zeros
		template<typename TYPE>
		void fillZeros(std::vector<TYPE> vec);

		//displaying 2D matrix
		template<typename TYPE>
		void displayELLMatrix(TYPE ** matrix);

		void displayPointerArray(int * arr);



	public:

		ReadMatrixELL(std::string matrixName);
		~ReadMatrixELL();
		std::vector<int> getIRP();
		std::vector<double> getAS();
		std::vector<int> getJA();
		int getM();
		int getNZ();
		int getN();




};
