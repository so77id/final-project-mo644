#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

// Comentarios iniciais
// o programa ainda nao funciona, nao sei o motivo


// variaveis globais
// int e, n, nt;
//double *L, *A;
//double sum;
//pthread_mutex_t lock;

struct worker_data {
	int e;
	int n_threads;
	double *m_dst;
	double *m_src;
	double sum;
	unsigned int i_thread;
	int size;
};

void *diag_worker_parallel(void* arg){

	struct worker_data *dd = arg;

	int k;
	double s;

	unsigned int my_rank = dd->i_thread; // Pega o numero da thread
	int local_m = dd->e/dd->n_threads;
	int my_first_k = my_rank*local_m;
	int my_last_k = (my_rank+1)*local_m-1;
	int aux;

	s = 0.0;

	// Caso o valor de j (ou e) nao seja divisivel pelo numero de threads
	// alocamos os valores que ainda faltam de j na ultima thread

	if(my_rank==dd->n_threads-1){
		aux = (my_last_k - my_first_k + 1)*dd->n_threads;
		if(aux != dd->e) my_last_k = my_last_k + dd->e - aux;
	}

	for(k = my_first_k; k <= my_last_k; k++){
		dd->sum += dd->m_dst[dd->e * dd->size + k] * dd->m_dst[dd->e * dd->size + k];
	}

	// calculamos o valor de s localmente em cada thread e depois incorporamos na variavel global sum
	// o mutex serve para nao ter conflito na hora de atualizar

	return NULL;
}

void *rest_worker_parallel(void* arg){
	int i, k;
	double s;

	struct worker_data *dd = arg;

	unsigned int my_rank = dd->i_thread; // Pega o numero da thread
	int local_m = (dd->size - dd->e - 1)/dd->n_threads;
	int my_first_i = my_rank*local_m;

	my_first_i = my_first_i+dd->e+1;

	int my_last_i = my_first_i + (local_m-1);
	int aux;

	if(my_rank==dd->n_threads-1){
		aux = dd->size - 1 - my_last_i;
		if(aux != 0) my_last_i = my_last_i + aux;
	}


	for(i = my_first_i; i <=my_last_i; i++){
		s = 0.0;
		for(k = 0; k < dd->e; k++){
			s += dd->m_dst[i * dd->size + k] * dd->m_dst[dd->e * dd->size + k];
		}
		dd->m_dst[i * dd->size + dd->e] = (1.0 / dd->m_dst[dd->e * dd->size + dd->e] * (dd->m_src[i * dd->size + dd->e] - s));
	}

	return NULL;
}

void *rest_worker_serial(int j, int n, double *L, double *A, int nt){
	int i, k;
	double s;

	for(i = j+1; i <n; i++){
		s = 0.0;
		for(k = 0; k < j; k++){
			s += L[i * n + k] * L[j * n + k];
		}
		L[i * n + j] = (1.0 / L[j * n + j] * (A[i * n + j] - s));
	}

	return NULL;
}

double *cholesky(double *m_src, int size, int n_threads){
	double *m_dst = (double *)calloc(size*size,sizeof(double));
	int j, k, i;
	double s;
	double sum;
	unsigned int i_thread;
	pthread_t* thread_handles;

	// Struct to transfer data to threads
	thread_handles = malloc (n_threads*sizeof(pthread_t));


	if (m_src == NULL)
		exit(EXIT_FAILURE);

	// Faz a decomposicao de cholesky pelas colunas
	for(j = 0; j < size; j++){

		// eh necessario obter a diagonal antes de paralelizar os outros elementos
		sum = 0;
		//e = j;

		// nao faz sentido paralelizar quando o valor de j eh menor do que o numero de threads
		//if(j >= n_threads){

		struct worker_data *threads_data=malloc(n_threads*sizeof(struct worker_data));

		for(i_thread = 0; i_thread < n_threads; i_thread++){
			threads_data[i_thread].m_dst = m_dst;
			threads_data[i_thread].m_src = m_src;
			threads_data[i_thread].n_threads = n_threads;
			threads_data[i_thread].e = j;
			threads_data[i_thread].size = size;
			threads_data[i_thread].sum = sum;
			threads_data[i_thread].i_thread = i_thread;

			pthread_create(&thread_handles[i_thread], NULL, diag_worker_parallel, (void*) &threads_data[i_thread]);
		}

		for(i_thread = 0; i_thread < n_threads; i_thread++){
			pthread_join(thread_handles[i_thread], NULL);
			sum += threads_data[i_thread].sum;
		}

		//}
		/*else{
			for(k = 0; k < j; k++) {
				sum += m_dst[j * size + k] * m_dst[j * size + k];
			}
		}*/

		m_dst[j * size + j] = sqrt(m_src[j * size + j] - sum);


		//diag(j, n, Lm_dstA, n_threads);

		// obtendo os outros elementos da coluna (exceto a diagonal)
		//rest_worker_parallel(j, size, m_dst, m_src, n_threads);

		//if(size - j - 1 >= n_threads){
			//struct worker_data *threads_data=malloc(n_threads*sizeof(struct worker_data));

		for(i_thread = 0; i_thread < n_threads; i_thread++){
			threads_data[i_thread].m_dst = m_dst;
			threads_data[i_thread].m_src = m_src;
			threads_data[i_thread].n_threads = n_threads;
			threads_data[i_thread].e = j;
			threads_data[i_thread].size = size;
			threads_data[i_thread].i_thread = i_thread;

			pthread_create(&thread_handles[i_thread], NULL, rest_worker_parallel, (void*) &threads_data[i_thread]);
		}

		for(i_thread = 0; i_thread < n_threads; i_thread++){
			pthread_join(thread_handles[i_thread], NULL);
		}

		/*}
		else{
			rest_worker_serial(j, size, m_dst, m_src, n_threads);
		}*/

	}

	return m_dst;
}

void show_matrix(double *A, int n){
	int i, j;

	for(i = 0; i < n; i++){
		for(j = 0; j < n; j++)
			printf("%2.5f ", A[i * n + j]);
		printf("\n");
	}
}

int main(int argc, char const *argv[]) {
	if (argc < 2) {
		printf("To execute this program need send nthreads for argv\n");
		printf("./name n_threads\n");
		return(-1);
	}

	int size, n_threads, i, j;
	double *m_src;
	double *m_dst;
	long unsigned int duracao;
	struct timeval start, end;

	// Numero de threads
	//scanf("%d",&nt);
	// mudar manualmente enquanto esta testando, depois colocamos como input junto no arquivo in

	n_threads = int(argv[1]); // mudar enquanto esta testando, depois colocamos como input junto no arquivo in

	// Dimensao da matriz
	scanf("%d",&size);


	// A matriz sera alocada na forma de vetor
	// Alocando a memoria para o vetor m
	m_src = (double *)calloc(size*size,sizeof(double));

	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++)
			scanf("%lf", &m_src[i * size + j]);
	}

	gettimeofday(&start, NULL);
	m_dst = cholesky(m_src, size, n_threads);
	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));

	printf("%lu\n",duracao);

	show_matrix(m_dst, size);
	printf("\n");
	free(m_src);
	free(m_dst);

	return 0;

}
