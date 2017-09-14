#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

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

	unsigned int my_rank = dd->i_thread; // Pega o numero da thread
	int local_m = dd->e/dd->n_threads;
	int my_first_k = my_rank*local_m;
	int my_last_k = (my_rank+1)*local_m-1;
	int aux;

	if(my_rank==dd->n_threads-1){
		aux = (my_last_k - my_first_k + 1)*dd->n_threads;
		if(aux != dd->e) my_last_k = my_last_k + dd->e - aux;
	}

	for(k = my_first_k; k <= my_last_k; k++){
		dd->sum += dd->m_dst[dd->e * dd->size + k] * dd->m_dst[dd->e * dd->size + k];
	}

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

double *cholesky(double *m_src, int size, int n_threads){
	double *m_dst = (double *)calloc(size*size,sizeof(double));
	int j;
	double sum;
	unsigned int i_thread;
	pthread_t* thread_handles = malloc (n_threads*sizeof(pthread_t));
	struct worker_data *threads_data=malloc(n_threads*sizeof(struct worker_data));

	if (m_src == NULL)
		exit(EXIT_FAILURE);

	for(i_thread = 0; i_thread < n_threads; i_thread++){
			threads_data[i_thread].m_dst = m_dst;
			threads_data[i_thread].m_src = m_src;
			threads_data[i_thread].n_threads = n_threads;
			threads_data[i_thread].size = size;
			threads_data[i_thread].i_thread = i_thread;
	}


	for(j = 0; j < size; j++){

		sum = 0;

		//diag_worker_parallel
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

		m_dst[j * size + j] = sqrt(m_src[j * size + j] - sum);

		//rest
		for(i_thread = 0; i_thread < n_threads; i_thread++){

			pthread_create(&thread_handles[i_thread], NULL, rest_worker_parallel, (void*) &threads_data[i_thread]);
		}

		for(i_thread = 0; i_thread < n_threads; i_thread++){
			pthread_join(thread_handles[i_thread], NULL);
		}

	}

	free(threads_data);
	free(thread_handles);
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

	n_threads = atoi(argv[1]);

	// Dimensao da matriz
	scanf("%d",&size);


	// A matriz sera alocada na forma de vetor
	m_src = (double *)calloc(size*size,sizeof(double));

	for(i = 0; i < size; i++) {
		for(j = 0; j < size; j++)
			scanf("%lf", &m_src[i * size + j]);
	}

	gettimeofday(&start, NULL);
	m_dst = cholesky(m_src, size, n_threads);
	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));


	show_matrix(m_dst, size);
	printf("\n");
	printf("%lu\n",duracao);
	free(m_src);
	free(m_dst);

	return 0;

}
