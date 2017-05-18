#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

double *cholesky(double *A, int n) {
	double *L = (double *)malloc(n*n*sizeof(double));
	if (L == NULL)
		exit(EXIT_FAILURE);
	
	for (int i = 0; i < n; i++)
		for (int j = 0; j < (i+1); j++) {
			double s = 0;
			for (int k = 0; k < j; k++)
				s += L[i * n + k] * L[j * n + k];
			L[i * n + j] = (i == j) ?
			sqrt(A[i * n + i] - s) :
				(1.0 / L[j * n + j] * (A[i * n + j] - s));
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
	double start, end;
	
	// Dimensao da matriz m
	scanf("%d",&n);
	
	// A matriz sera alocada na forma de vetor
	// Alocando a memoria para o vetor m
	m = (double *)malloc(n*n*sizeof(double));
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			scanf("%lf", &m[i * n + j]);
	}
	
	start = omp_get_wtime();
	double *c1 = cholesky(m, n);
	end = omp_get_wtime();
	
	show_matrix(c1, n);
	printf("\n");
	
	printf("%lf\n",end-start);
	
	free(c1);
	
	return 0;
}
