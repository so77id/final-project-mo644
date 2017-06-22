#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>


double *cholesky(double *A, int n) {
	double *L = (double *)calloc(n*n,sizeof(double));
	int j, k, i;
	double s;

	if (L == NULL)
		exit(EXIT_FAILURE);

	// Faz a decomposicao de cholesky pelas colunas
	for(j = 0; j < n; j++) {
		s = 0.0;

		// eh necessario obter a diagonal antes de paralelizar os outros elementos
		for(k = 0; k < j; k++) {
			s += L[j * n + k] * L[j * n + k];
		}

		L[j * n + j] = sqrt(A[j * n + j] - s);
		// obtendo os outros elementos da coluna (exceto a diagonal)
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
	int n;
	double *m;
	struct timeval start, end;
	long unsigned int duracao;

	// Dimensao da matriz
	scanf("%d",&n);

	// A matriz sera alocada na forma de vetor
	// Alocando a memoria para o vetor m
	m = (double *)calloc(n*n,sizeof(double));

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++){
			scanf("%lf", &m[i * n + j]);
		}
	}


	gettimeofday(&start, NULL);
	double *c1 = cholesky(m, n);
	gettimeofday(&end, NULL);
	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));


	show_matrix(c1, n);
	printf("\n");
	printf("%lu\n",duracao);
	free(c1);

	return 0;

}