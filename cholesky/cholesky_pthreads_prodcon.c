#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>


/**************************************
INIT BARRIER IMPLEMENTATION FOR MAC
***************************************/

//#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;

    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
//#endif // __APPLE__


/**************************************
END BARRIER IMPLEMENTATION FOR MAC
***************************************/

/**************************************
INIT QUEUE IMPLEMENTATION
***************************************/

struct node {
    unsigned int value;
    struct node *next;
};

struct queue {
    struct node *head;
    struct node *tail;
    int n_nodes;
};

// enqueue to Queue
void enqueue(struct queue *q, int value) {
    struct node *new = malloc(sizeof(struct node));
    if(new == NULL) return;
    new->value = value;
    new->next = NULL;

    if(q->tail == NULL){
        q->tail = q->head = new;
    } else {
        q->tail->next = new;
        q->tail = new;
    }
}

int empty(struct queue *q) {
    if(q->head == NULL) return 1;
    else return 0;
}


int dequeue(struct queue *q) {
    if(q->head == NULL) return -1;
    struct node *tmp = q->head;
    q->head = tmp->next;
    int value = tmp->value;
    free(tmp);
    if(q->head == NULL) q->tail = q->head;
    return value;
}

// Return new pointer to queue
struct queue* init_queue(){
    struct queue *q = malloc(sizeof(struct queue));
    q->head = q->tail = NULL;
    return q;
}


// Delete queue
void delete_queue(struct queue* q) {
    struct node *tmp;
    while(q->head != NULL) {
        tmp = q->head;
        q->head = tmp->next;
        free(tmp);
    }
    free(q);
}

void print_queue(struct queue *q) {
    printf("Printing: \n");
    for(struct node *iter = q->head; iter != NULL; iter = iter->next) {
        printf("%d ", iter->value);
    }
    printf("\n");
}

/**************************************
END QUEUE IMPLEMENTATION
***************************************/


struct worker_data
{
    struct queue *q;
    pthread_mutex_t *mutex;
    pthread_barrier_t *barrier;
    int n_threads;
    double *m_dst;
    double *m_src;
    int size;
    unsigned int j;
    double sum;
    int producer_finish;
    int producer_iter_finish;
    int in_barrier;
    int mode; // if is 0 == diag calculation 1 == rest calculation
};

void * worker(void * args) {
    struct worker_data *wd = args;
    unsigned int i, k;
    double s;

    while(1) {
        pthread_mutex_lock(wd->mutex);
        //printf("Antes del empty\n");
        //print_queue(wd->q);

        if(empty(wd->q)){
            if(wd->producer_finish) {
                printf("PROUDCED FINISH\n");
                pthread_mutex_unlock(wd->mutex);
                break;
            }
            else if(wd->producer_iter_finish){
                wd->in_barrier++;
                printf("sumado al barrier in_barrier: %d\n", wd->in_barrier);
                pthread_mutex_unlock(wd->mutex);
                pthread_barrier_wait(wd->barrier);
                continue;
            }
            else{
                printf("producer no finish in_barrier: %d, producer_finish: %d\n", wd->in_barrier, wd->producer_finish);
                pthread_mutex_unlock(wd->mutex);
                continue;
            }
        }
        i = dequeue(wd->q);
        pthread_mutex_unlock(wd->mutex);

        if(wd->mode == 0) {
            //pthread_mutex_lock(wd->mutex);
            wd->sum += wd->m_dst[wd->j * wd->size + i] * wd->m_dst[wd->j * wd->size + i];
            //pthread_mutex_unlock(wd->mutex);
        } else if(wd->mode == 1) {
            s = 0.0;
            for(k = 0; k < wd->j; k++) {
                s += wd->m_dst[i * wd->size + k] * wd->m_dst[wd->j * wd->size + k];
            }

            wd->m_dst[i * wd->size + wd->j] = (1.0 / wd->m_dst[wd->j * wd->size + wd->j] * (wd->m_src[i * wd->size + wd->j] - s));
        }

        //printf("Saque i: %d\n", i);
    }
}



double *cholesky(double *m_src, int size, int n_threads){
    // Check source
    if (m_src == NULL)
        exit(EXIT_FAILURE);

    double *m_dst = (double *)calloc(size*size,sizeof(double));

    unsigned int i, j, k, i_thread;
    pthread_t* diag_threads = malloc (n_threads*sizeof(pthread_t));

    // Creating and init mutex
    pthread_mutex_t diag_mutex;
    pthread_barrier_t diag_barrier;
    pthread_mutex_init (&diag_mutex, NULL);
    pthread_barrier_init(&diag_barrier, NULL, n_threads + 1);

    struct worker_data *wd = malloc(sizeof(struct worker_data));

    wd->q = init_queue();
    wd->mutex = &diag_mutex;
    wd->barrier = &diag_barrier;
    wd->n_threads = n_threads;
    wd->m_dst = m_dst;
    wd->m_src = m_src;
    wd->size = size;
    wd->producer_iter_finish = 0;
    wd->producer_finish = 0;
    wd->in_barrier = 0;
    wd->mode = 0;

    for(i_thread = 0; i_thread < n_threads; i_thread++){
        pthread_create(&diag_threads[i_thread], NULL, worker, (void*) wd);
    }

    for(j = 0; j < size; j++){
        printf("Iter: %d\n", j);
        wd->j = j;
        wd->sum = 0.0;
        wd->producer_iter_finish = 0;
        wd->mode = 0;


        printf("MODE 0\n");
        for(k = 0; k < j; k++) {
            printf("ENCOLANDO MODE 0 %d\n", k);
            pthread_mutex_lock(wd->mutex);
            enqueue(wd->q, k);
            pthread_mutex_unlock(wd->mutex);
        }
        wd->producer_iter_finish = 1;

        while(wd->in_barrier != n_threads) {
            pthread_yield(NULL);
        }

        printf("SALI WHILE MODE 0\n");
        //GET SUM
        wd->m_dst[wd->j * wd->size + wd->j] = sqrt(wd->m_src[wd->j * wd->size + wd->j] - wd->sum);

        wd->in_barrier = 0;
        wd->producer_iter_finish = 0;
        pthread_barrier_wait(wd->barrier);

        //printf("OUT BARRIER MODE 0\n");
        // MODE 1

        //printf("MODE 1\n");
        wd->mode = 1;


        for(i = wd->j+1; i < wd->size; i++) {
            //printf("ENCOLANDO MODE 1 %d\n", k);
            pthread_mutex_lock(wd->mutex);
            enqueue(wd->q, i);
            pthread_mutex_unlock(wd->mutex);
        }

        wd->producer_iter_finish = 1;
        while(1){
            printf("IN WHILE MODE 1\n");
            if(wd->in_barrier == n_threads) break;
            //pthread_yield();
        }

        wd->in_barrier = 0;
        wd->producer_iter_finish = 0;
        pthread_barrier_wait(wd->barrier);

        //printf("OUT BARRIER MODE 1\n");
    }
    //printf("FINISH\n");
    wd->producer_finish = 1;



    //printf("JOIN THREADS\n");
    for(i_thread = 0; i_thread < n_threads; i_thread++){
        pthread_join(diag_threads[i_thread], NULL);
        //printf("Termine threads\n");
    }

    pthread_mutex_destroy(&diag_mutex);
    pthread_barrier_destroy(&diag_barrier);

    delete_queue(wd->q);
    free(wd);
    free(diag_threads);
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

    n_threads = atoi(argv[1]); // mudar enquanto esta testando, depois colocamos como input junto no arquivo in

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


    show_matrix(m_dst, size);
    printf("\n");
    printf("%lu\n",duracao);
    free(m_src);
    free(m_dst);

    return 0;

}
