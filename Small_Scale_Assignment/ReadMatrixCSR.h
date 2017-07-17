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


class ReadMatrixCSR
{

private:

	
	int M, N, nz;
	int i;
	
	std::string matrixName;
	//int * JA;
	//double * AS;
	//int * IRP; // Pointer to IRP array

	std::unique_ptr<int[]>  JA;
	std::unique_ptr<double[]>  AS;
	std::unique_ptr<int[]>  IRP;

	std::vector<double> asVector;
	std::vector<int> jaVector;
	std::vector<int> irpVector;

	//std::vector<std::pair<int, double> > rowsAndValues;


	void calculateCSRValues(std::vector<std::tuple<int, int, double> > rowsAndValues, int * J);

	void displayPointerArray(int * arr);

	

public:	

	ReadMatrixCSR(std::string matrixName);
	ReadMatrixCSR(const ReadMatrixCSR& copy);
	~ReadMatrixCSR();
	std::vector<int> getIRP();
	std::vector<double> getAS();
	std::vector<int> getJA();
	double getSelectedElementAS(int elemIndex) const;
	int getSelectedElementJA(int elemIndex) const;
	int getSelectedElementIRP(int elemIndex) const;
	int getM();
	int getNZ();
	int getN();
	std::string getMatrixName();

};

