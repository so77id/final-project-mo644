#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

double *cholesky(double *A, int n){

	// Inicializando a matriz L com zeros
	double *L = (double *)calloc(n*n, sizeof(double));

	int i, j, k;
	double s;

	if(L == NULL)
		exit(EXIT_FAILURE);

	// Faz a decomposicao de cholesky pelas linhas (modo nao paralelizavel)
	// Nos algoritmos com openmp e pthreads foi necessario reescrever esse trecho de codigo
	// de modo a torná-lo paralelizavel
	for(i = 0; i < n; i++)
		for(j = 0; j < (i+1); j++) {
			s = 0.0;
			for(k = 0; k < j; k++)
				s += L[i * n + k] * L[j * n + k];
			if(i == j) L[i * n + j] = sqrt(A[i * n + i] - s);
			else L[i * n + j] = (1.0 / L[j * n + j] * (A[i * n + j] - s));
		}
		return L;
}

void show_matrix(double *A, int n) {
	int i, j;

	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++)
			printf("%.5f ", A[i * n + j]);
		printf("\n");
	}
}

int main() {
	int n, nt, i, j;
	double *m;
	struct timeval start, end;
	long unsigned int duracao;

	// Numero de threads
	//scanf("%d",&nt);
	nt=2; // mudar manualmente enquanto esta testando, depois colocamos como input junto no arquivo in

	// Dimensao da matriz m
	scanf("%d",&n);

	// A matriz sera alocada na forma de vetor
	// Alocando a memoria para o vetor m
	m = (double *)malloc(n*n*sizeof(double));

	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++)
			scanf("%lf", &m[i * n + j]);
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
