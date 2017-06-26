BINARY_FOLDER=binaries
INPUT_FOLDER='./input'

EXECUTABLE_SERIAL=cholesky_serial.bin
EXECUTABLE_OPENMP=cholesky_openmp.bin
EXECUTABLE_OPENMP_THREADPOOL=cholesky_openmp_threadspool.bin
EXECUTABLE_PTHREAD=cholesky_pthreads.bin
EXECUTABLE_PTHREAD_PRODCON=cholesky_pthreads_prodcon.bin

ITERATIONS=1
N_THREADS=(16 24 32)

INPUTS=("$INPUT_FOLDER/matrix_5000x5000.in" "$INPUT_FOLDER/matrix_6000x6000.in" "$INPUT_FOLDER/matrix_7000x7000.in")


for INPUT in "${INPUTS[@]}"
do
    echo ""
    echo "${INPUT}"
    echo "--------------------------"

    SERIAL_TIME=0
    for (( i = 0; i < ${ITERATIONS}; i++ )); do
        TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_SERIAL}" < ${INPUT} | tail -n 1 | awk '{print $1}')
        SERIAL_TIME=$(bc -l <<< "$SERIAL_TIME + $TIME")
    done
    SERIAL_TIME=$(bc -l <<< ${SERIAL_TIME}/${ITERATIONS})
    SERIAL_TIMES+=($SERIAL_TIME)

    for N_THREAD in "${N_THREADS[@]}"
    do
        printf "Number of threads: %d\n" ${N_THREAD}
        #OPENMP
        OPENMP_TIME=0
        for (( i = 0; i < ${ITERATIONS}; i++ )); do
            TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_OPENMP}" $N_THREAD < ${INPUT} | tail -n 1 | awk '{print $1}')
            OPENMP_TIME=$(bc -l <<< "$OPENMP_TIME + $TIME")
        done
        OPENMP_TIME=$(bc -l <<< ${OPENMP_TIME}/${ITERATIONS})

        SPEEDUP=$(bc -l <<< ${SERIAL_TIME}/${OPENMP_TIME})
        printf "OpenMP SPEEDUP: %4.7f\n" ${SPEEDUP}

        #OPENMP THREADPOOL
        OPENMP_POOL_TIME=0
        for (( i = 0; i < ${ITERATIONS}; i++ )); do
            TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_OPENMP_THREADPOOL}" $N_THREAD < ${INPUT} | tail -n 1 | awk '{print $1}')
            OPENMP_POOL_TIME=$(bc -l <<< "$OPENMP_POOL_TIME + $TIME")
        done
        OPENMP_POOL_TIME=$(bc -l <<< ${OPENMP_POOL_TIME}/${ITERATIONS})

        SPEEDUP=$(bc -l <<< ${SERIAL_TIME}/${OPENMP_POOL_TIME})
        printf "OpenMP ThreadPool SPEEDUP: %4.7f\n" ${SPEEDUP}

        #PTHREAD
        PTHREAD_TIME=0
        for (( i = 0; i < ${ITERATIONS}; i++ )); do
            TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_PTHREAD}" $N_THREAD < ${INPUT} | tail -n 1 | awk '{print $1}')
            PTHREAD_TIME=$(bc -l <<< "$PTHREAD_TIME + $TIME")
        done
        PTHREAD_TIME=$(bc -l <<< ${PTHREAD_TIME}/${ITERATIONS})

        SPEEDUP=$(bc -l <<< ${SERIAL_TIME}/${PTHREAD_TIME})
        printf "Pthread SPEEDUP: %4.7f\n" ${SPEEDUP}


        #PTHREAD_PRODCON
        PTHREAD_TIME_PRODCON=0
        for (( i = 0; i < ${ITERATIONS}; i++ )); do
            TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_PTHREAD_PRODCON}" $N_THREAD < ${INPUT} | tail -n 1 | awk '{print $1}')
            PTHREAD_TIME_PRODCON=$(bc -l <<< "$PTHREAD_TIME_PRODCON + $TIME")
        done
        PTHREAD_TIME_PRODCON=$(bc -l <<< ${PTHREAD_TIME_PRODCON}/${ITERATIONS})

        SPEEDUP=$(bc -l <<< ${SERIAL_TIME}/${PTHREAD_TIME_PRODCON})
        printf "Pthread Producer consumer SPEEDUP: %4.7f\n\n" ${SPEEDUP}

    done
done