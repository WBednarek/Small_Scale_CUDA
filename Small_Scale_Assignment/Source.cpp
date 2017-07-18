
//
//
//void OpenMPCSR(int numberOfThreads,  int M, double *AsCSR, 
//	int *IrpCSR, int *JaCSR, double *X, double *Y, int cnk )
//{ 
//	
//	double storeArray = 0;
//#pragma omp parallel numberOfThreads(numberOfThreads) private(index, j, storeArray) 
//{ 
//#pragma omp for schedule(dynamic, cnk) 
//
//	for ( int index = 0; index < M; ++index){ 
//		storeArray = 0; 
//		for ( int j = IrpCSR[index]; j <= IrpCSR[index + 1] - 1; ++j)
//		{ storeArray += AsCSR[j] * X[JaCSR[j]]; 
//		} 
//		Y[index] = storeArray; 
//}
//} 
//}
