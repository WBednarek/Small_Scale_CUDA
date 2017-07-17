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
#include <memory>

class ReadMatrixELL
{


	private:	
		// M - Rows, N - Columns , Nz - non zero values
		int M, N, nz;
		int numOfElementsInTheBiggestRow;
		std::string matrixName;
		std::unique_ptr<int[]>  JAOneDimensional;
		std::unique_ptr<double[]>  ASOneDimensional;
		std::vector<double> asVector;
		std::vector<int> jaVector;
		//int * JAOneDimensional;
		//double * ASOneDimensional;

		//std::vector<int> nonZeroValuesInTheRows;
		int nonZeroValuesInTheAllRows;

		void resizeMatrices(std::vector<std::tuple<int, int, double> > & rowsAndValues);

		void sortInputMatrixByTheRows(std::vector<std::tuple<int, int, double> > & rowsAndValues);


		
		void calculateELLValues(std::vector<std::tuple<int, int, double> > & rowsAndValues, std::vector<int> & nonZeroValuesInTheRows);


		//Calculates noon zeros in selected row
		void calclateNonZeroValuesInTheRows(std::vector<std::tuple<int, int, double> > & rowsAndValues, std::vector<int> & nonZeroValuesInTheRows);
	

		//Preparing one-dimensional matrix fling it with zeros
		void fillZerosOneDimensional();


		template<typename TYPE>
		void saveOneDimensionalELLMatrix(TYPE * matrix, std::string name, std::vector<int> & nonZeroValuesInTheRows);

		void displayPointerArray(int * arr);



	public:

		ReadMatrixELL(std::string matrixName);
		//ReadMatrixELL::ReadMatrixELL(const ReadMatrixELL& copy);
		~ReadMatrixELL();
		std::vector<double> getAS();
		std::vector<int> getJA();
		int getM();
		int getNZ();
		int getN();
		int getNumberOfElementsInTheBiggestRow();
		double getSelectedElementAS(int elemIndex) const;
		int getSelectedElementJA(int elemIndex) const;
		std::string getMatrixName();




};

