#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


double *cholesky(double *A, int n, int nt) {
	double *L = (double *)calloc(n*n,sizeof(double));
	int j, k, i;
	double s;
	
	if (L == NULL)
		exit(EXIT_FAILURE);
	
	// Faz a decomposicao de cholesky pelas colunas
	for(j = 0; j < n; j++) {
		s = 0.0;
		
		// eh necessario obter a diagonal antes de paralelizar os outros elementos
		# pragma omp parallel for num_threads(nt) default(none) shared(L, n, j) private(k) reduction(+: s)
		for(k = 0; k < j; k++) {
			s += L[j * n + k] * L[j * n + k];
		}
		
		L[j * n + j] = sqrt(A[j * n + j] - s);
		
		// obtendo os outros elementos da coluna (exceto a diagonal)
		#pragma omp parallel for num_threads(nt) private(i, k, s) shared(j, n, L, A)
		for(i = j+1; i <n; i++) {
			s = 0.0;
			for(k = 0; k < j; k++) {
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
	int n, nt;
	double *m;
	double start, end;
	
	// Numero de threads
	scanf("%d",&nt);
	
	// Dimensao da matriz
	scanf("%d",&n);
	
	// A matriz sera alocada na forma de vetor
	// Alocando a memoria para o vetor m
	m = (double *)calloc(n*n,sizeof(double));
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			scanf("%lf", &m[i * n + j]);
	}
	
	
	start = omp_get_wtime();
	double *c1 = cholesky(m, n, nt);
	end = omp_get_wtime();
	printf("%lf\n",end-start);
	
	show_matrix(c1, n);
	printf("\n");
	free(c1);
	
	return 0;
	
}
