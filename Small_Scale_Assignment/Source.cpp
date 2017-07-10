
//
//
//void OpenMPCSR(int numberOfThreads,  int M, double *AsCSR, 
//	int *IrpCSR, int *JaCSR, double *X, double *Y, int cnk )
//{ 
//	
//	double data = 0;
//#pragma omp parallel numberOfThreads(numberOfThreads) private(index, j, data) 
//{ 
//#pragma omp for schedule(dynamic, cnk) 
//
//	for ( int index = 0; index < M; ++index){ 
//		data = 0; 
//		for ( int j = IrpCSR[index]; j <= IrpCSR[index + 1] - 1; ++j)
//		{ data += AsCSR[j] * X[JaCSR[j]]; 
//		} 
//		Y[index] = data; 
//}
//} 
//}
