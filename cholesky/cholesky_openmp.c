#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double *cholesky(double *A, int n) {
	double *L = (double *)malloc(n*sizeof(double));
	if (L == NULL)
		exit(EXIT_FAILURE);
	
	for (int j = 0; j <n; j++) {            
		double s = 0;
		for (int k = 0; k < j; k++) {
			s += L[j * n + k] * L[j * n + k];
		}
		L[j * n + j] = sqrt(A[j * n + j] - s);
		#pragma omp parallel for
		for (int i = j+1; i <n; i++) {
			double s = 0;
			for (int k = 0; k < j; k++) {
				s += L[i * n + k] * L[j * n + k];
			}
			L[i * n + j] = (1.0 / L[j * n + j] * (A[i * n + j] - s));
		}
	}
	return L;
}

void show_matrix(double *A, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			printf("%2.5f ", A[i * n + j]);
		printf("\n");
	}
}

int main() {
	int n;
	double *m;
	
	// Dimensao da matriz
	scanf("%d",&n);
	
	// A matriz sera alocada na forma de vetor
	// Alocando a memoria para o vetor m
	m = (double *)malloc(n*sizeof(double));
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			scanf("%lf", &m[i * n + j]);
	}
	
	double *c1 = cholesky(m, n);
	show_matrix(c1, n);
	printf("\n");
	free(c1);
	
	return 0;
	
}