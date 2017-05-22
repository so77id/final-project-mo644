#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

// Comentarios iniciais
// o programa ainda nao funciona, nao sei o motivo


// variaveis globais
int e, n, nt;
double *L, *A;
double sum;
pthread_mutex_t lock;

void *diag(void* rank){
	int k;
	double s;
	
	long my_rank = (long) rank; // Pega o numero da thread
	int local_m = e/nt;
	int my_first_k = my_rank*local_m;
	int my_last_k = (my_rank+1)*local_m-1;
	int aux;
	
	s = 0.0;
	
	// Caso o valor de j (ou e) nao seja divisivel pelo numero de threads
	// alocamos os valores que ainda faltam de j na ultima thread
	if(my_rank==nt-1){
		aux = (my_last_k - my_first_k + 1)*nt;
		if(aux != e) my_last_k = my_last_k + e - aux;
	}
	
	for(k = my_first_k; k <= my_last_k; k++){
		s += L[e * n + k] * L[e * n + k];
	}
	
	// calculamos o valor de s localmente em cada thread e depois incorporamos na variavel global sum
	// o mutex serve para nao ter conflito na hora de atualizar
	pthread_mutex_lock(&lock);
	sum = sum + s;
	pthread_mutex_unlock(&lock);
	
	return NULL;
}

void *rest(int j, int n, double *L, double *A, int nt){
	int i, k;
	double s;
	
	// nao mexi nessa parte, ainda tem que parelelizar o for anterior
	//#pragma omp parallel for num_threads(nt) private(i, k, s) shared(j, n, L, A)
	for(i = j+1; i <n; i++){
		s = 0.0;
		for(k = 0; k < j; k++){
			s += L[i * n + k] * L[j * n + k];
		}
		L[i * n + j] = (1.0 / L[j * n + j] * (A[i * n + j] - s));
	}
	
	return NULL;
}

double *cholesky(double *A, int n, int nt){
	double *L = (double *)calloc(n*n,sizeof(double));
	int j, k, i;
	double s;
	long thread;
	pthread_t* thread_handles;
	
	thread_handles = malloc (nt*sizeof(pthread_t));
	
	
	if (L == NULL)
		exit(EXIT_FAILURE);
	
	// Faz a decomposicao de cholesky pelas colunas
	for(j = 0; j < n; j++){
		
		// eh necessario obter a diagonal antes de paralelizar os outros elementos
		sum = 0;
		e = j;
		
		// nao faz sentido paralelizar quando o valor de j eh menor do que o numero de threads
		if(j >= nt){
			for(thread = 0; thread < nt; thread++){
				pthread_create(&thread_handles[thread], NULL, diag, (void*) thread);
			}
			
			for(thread = 0; thread < nt; thread++){
				pthread_join(thread_handles[thread], NULL);
			}
		}
		else{
			for(k = 0; k < j; k++) {
				sum += L[j * n + k] * L[j * n + k];
			}
		}
		
		L[j * n + j] = sqrt(A[j * n + j] - sum);
		
		// destroir o mutex criado
		pthread_mutex_destroy(&lock);
		
		//diag(j, n, L, A, nt);
		
		// obtendo os outros elementos da coluna (exceto a diagonal)
		rest(j, n, L, A, nt);
	}
	return L;
}

void show_matrix(double *A, int n){
	int i, j;
	
	for(i = 0; i < n; i++){
		for(j = 0; j < n; j++)
			printf("%2.5f ", A[i * n + j]);
		printf("\n");
	}
}

int main() {
	int n, nt, i, j;
	double *m;
	long unsigned int duracao;
	struct timeval start, end;
	
	// Numero de threads
	scanf("%d",&nt);
	
	// Dimensao da matriz
	scanf("%d",&n);
	
	// A matriz sera alocada na forma de vetor
	// Alocando a memoria para o vetor m
	m = (double *)calloc(n*n,sizeof(double));
	
	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++)
			scanf("%lf", &m[i * n + j]);
	}
	
	gettimeofday(&start, NULL);
	double *c1 = cholesky(m, n, nt);
	gettimeofday(&end, NULL);
	
	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
	
	printf("%lu\n",duracao);
	
	show_matrix(c1, n);
	printf("\n");
	free(c1);
	
	return 0;
	
}
