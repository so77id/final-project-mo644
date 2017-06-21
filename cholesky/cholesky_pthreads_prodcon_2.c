#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>


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
    if(q->head) return 1;
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
    unsigned int i_thread;
    struct shared_data *sd;
    unsigned int j;
    double sum;
};

struct shared_data
{
    struct queue *q;
    int producer_finish;
    pthread_mutex_t *mutex;
    int n_threads;
    double *m_dst;
    double *m_src;
    int size;
};

void *diag_worker_parallel(void *args) {
    struct worker_data *wd = args;
    struct queue *q = wd->sd->q;
    unsigned int k;

    if(wd->i_thread == 0){
        // Producer
        for(k = 0; k < wd->j; k++) {
            // Adquire Mutex Queue
            //printf("producer antes de lock");
            pthread_mutex_lock(wd->sd->mutex);
            // Enqueue index

            //printf("producer encolando");
            enqueue(q, k);
            // Relese Mutex Queue

            //printf("producer deslokeando");
            pthread_mutex_unlock(wd->sd->mutex);

            //printf("producer despues de lock");
        }
        //printf("producer despues finish\n");
        wd->sd->producer_finish = 1;
    }

    // Consumer
    while(1) {
        // if queue is empty and producer finish his work, left loop
        //printf("consumer %i, primer if\n", wd->i_thread);
        pthread_mutex_lock(wd->sd->mutex);
        if(!empty(q)){
            if(wd->sd->producer_finish){
                pthread_mutex_unlock(wd->sd->mutex);
                break;
            }
            else{
                pthread_mutex_unlock(wd->sd->mutex);
                continue;
            }
        }
        pthread_mutex_unlock(wd->sd->mutex);

        // Adquire Mutex Queue
        //printf("consumer %i, antes del lock\n", wd->i_thread);
        pthread_mutex_lock(wd->sd->mutex);
        // dequeue index

        //printf("consumer %i, decolando\n", wd->i_thread);
        k = dequeue(q);
        // Relese Mutex Queue

        //printf("consumer %i, deslokeando\n", wd->i_thread);
        pthread_mutex_unlock(wd->sd->mutex);

        //printf("consumer %i, antes del sum\n", wd->i_thread);

        wd->sum += wd->sd->m_dst[wd->j * wd->sd->size + k] * wd->sd->m_dst[wd->j * wd->sd->size + k];

        //printf("consumer %i, despues del sum\n", wd->i_thread);

    }
}

void *rest_worker_parallel(void* args){
    struct worker_data *wd = args;
    struct queue *q = wd->sd->q;
    unsigned int i;

    if(wd->i_thread == 0){
     // Producer
        for(i = wd->j+1; i < wd->sd->size; i++) {
            // Adquire Mutex Queue
            printf("producer antes de lock\n");
            pthread_mutex_lock(wd->sd->mutex);
            // Enqueue index

            printf("producer encolando\n");
            enqueue(q, i);
            // Relese Mutex Queue

            printf("producer deslokeando\n");
            pthread_mutex_unlock(wd->sd->mutex);

            printf("producer despues de lock\n");
        }
        printf("producer despues finish\n");
        wd->sd->producer_finish = 1;
    }

    // Consumer
    double s;
    unsigned int k;
    while(1) {
        // if queue is empty and producer finish his work, left loop
        printf("consumer %i, primer if\n", wd->i_thread);

        pthread_mutex_lock(wd->sd->mutex);
        if(!empty(q)){
            if(wd->sd->producer_finish){
                pthread_mutex_unlock(wd->sd->mutex);
                break;
            }
            else{
                pthread_mutex_unlock(wd->sd->mutex);
                continue;
            }
        }
        pthread_mutex_unlock(wd->sd->mutex);

        // if queue is empty and producer not finish his work, continue to next iteration

        // Adquire Mutex Queue
        printf("consumer %i, antes del lock\n", wd->i_thread);
        pthread_mutex_lock(wd->sd->mutex);
        // dequeue index

        printf("consumer %i, decolando\n", wd->i_thread);
        i = dequeue(q);
        // Relese Mutex Queue

        printf("consumer %i, deslokeando\n", wd->i_thread);
        pthread_mutex_unlock(wd->sd->mutex);

        printf("consumer %i, antes del sum\n", wd->i_thread);
        s = 0.0;
        for(k = 0; k < wd->j; k++) {
            s += wd->sd->m_dst[i * wd->sd->size + k] * wd->sd->m_dst[wd->j * wd->sd->size + k];
        }

        printf("consumer %i, antes del asignacion \n", wd->i_thread);
        printf("%f\n", wd->sd->m_dst[i * wd->sd->size + wd->j]);
        pthread_mutex_lock(wd->sd->mutex);
        wd->sd->m_dst[i * wd->sd->size + wd->j] = (1.0 / wd->sd->m_dst[wd->j * wd->sd->size + wd->j] * (wd->sd->m_src[i * wd->sd->size + wd->j] - s));
        pthread_mutex_unlock(wd->sd->mutex);
        printf("consumer %i, despues del asignacion \n", wd->i_thread);
        printf("%f\n", wd->sd->m_dst[i * wd->sd->size + wd->j]);
        //printf("consumer %i, despues del sum\n", wd->i_thread);

    }
}


double *cholesky(double *m_src, int size, int n_threads){
    // Check source
    if (m_src == NULL)
        exit(EXIT_FAILURE);

    //n_threads+=1;
    double *m_dst = (double *)calloc(size*size,sizeof(double));

    int j;
    double sum;
    unsigned int i_thread;
    pthread_t* thread_handles = malloc (n_threads*sizeof(pthread_t));

    //Create worker structs
    struct worker_data *threads_data=malloc(n_threads*sizeof(struct worker_data));

    // Create shared data for threads
    struct shared_data *sd = malloc(sizeof(struct shared_data));

    // Creating and init mutex
    pthread_mutex_t mutex;
    pthread_mutex_init (&mutex, NULL);

    sd->q = init_queue();
    sd->producer_finish = 0;
    sd->mutex = &mutex;
    sd->n_threads = n_threads;
    sd->m_dst = m_dst;
    sd->m_src = m_src;
    sd->size = size;

    // Write data to workers
    for(i_thread = 0; i_thread < n_threads; i_thread++){
            threads_data[i_thread].i_thread = i_thread;
            threads_data[i_thread].sd = sd;
    }


    for(j = 0; j < size; j++){

        printf("Nueva iteracion\n");

        sum = 0.0;
        sd->producer_finish = 0;

        //diag_worker_parallel
        for(i_thread = 0; i_thread < n_threads; i_thread++){
            threads_data[i_thread].j = j;
            threads_data[i_thread].sum = sum;
            // Call workers
            //printf("llamando threads\n");
            pthread_create(&thread_handles[i_thread], NULL, diag_worker_parallel, (void*) &threads_data[i_thread]);
        }

        for(i_thread = 0; i_thread < n_threads; i_thread++){
            // Join workers
            //printf("join threadss\n");
            pthread_join(thread_handles[i_thread], NULL);
            sum += threads_data[i_thread].sum;
        }


        //printf("Calculando matrix\n");
        m_dst[j * size + j] = sqrt(m_src[j * size + j] - sum);

        //rest
        sd->producer_finish = 0;
        for(i_thread = 0; i_thread < n_threads; i_thread++){

            printf("llamando threads\n");
            pthread_create(&thread_handles[i_thread], NULL, rest_worker_parallel, (void*) &threads_data[i_thread]);
        }

        for(i_thread = 0; i_thread < n_threads; i_thread++){

            printf("join threadss\n");
            pthread_join(thread_handles[i_thread], NULL);
        }

        printf("----------------------\n");
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
