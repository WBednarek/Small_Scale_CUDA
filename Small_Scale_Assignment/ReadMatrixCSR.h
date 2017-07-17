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


class ReadMatrixCSR
{

private:

	
	int M, N, nz;
	int i;
	
	std::string matrixName;
	int * JA;
	double * AS;
	int * IRP; // Pointer to IRP array

	//std::vector<std::pair<int, double> > rowsAndValues;
	std::vector<std::tuple<int, int, double> > rowsAndValues;

	void calculateCSRValues(int * J);

	void displayPointerArray(int * arr);

	

public:	

	ReadMatrixCSR(std::string matrixName);
	~ReadMatrixCSR();
	std::vector<int> getIRP();
	std::vector<double> getAS();
	std::vector<int> getJA();
	int getM();
	int getNZ();
	int getN();
	std::string getMatrixName();

};

