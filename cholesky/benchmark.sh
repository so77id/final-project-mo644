BINARY_FOLDER=binaries
INPUT_FOLDER='../inputs'

EXECUTABLE_SERIAL=cholesky_serial.bin
EXECUTABLE_OPENMP=cholesky_openmp.bin
EXECUTABLE_PTHREAD=cholesky_pthreads.bin

ITERATIONS=5

INPUTS=("$INPUT_FOLDER/matrix_3x3.in" "$INPUT_FOLDER/matrix_5x5.in" "$INPUT_FOLDER/matrix_50x50.in" "$INPUT_FOLDER/matrix_100x100.in" "$INPUT_FOLDER/matrix_500x500.in" "$INPUT_FOLDER/matrix_1000x1000.in" "$INPUT_FOLDER/matrix_2000x2000.in" "$INPUT_FOLDER/matrix_3000x3000.in" "$INPUT_FOLDER/matrix_5000x5000.in")
N_THREADS=(1 2 4 8 16 24 32)

SERIAL_TIMES=()
OPENMP_TIMES=()
PTHREAD_TIMES=()

for INPUT in "${INPUTS[@]}"
do
    echo "INPUT FILE: ${INPUT}"
    SERIAL_TIME=0
    for (( i = 0; i < ${ITERATIONS}; i++ )); do
        TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_SERIAL}" < ${INPUT} | tail -n 1 | awk '{print $1}')
        SERIAL_TIME=$(bc -l <<< "$SERIAL_TIME + $TIME")
    done
    SERIAL_TIME=$(bc -l <<< ${SERIAL_TIME}/${ITERATIONS})
    SERIAL_TIMES+=($SERIAL_TIME)

    for N_THREAD in "${N_THREADS[@]}"
    do
        #OPENMP
        OPENMP_TIME=0
        for (( i = 0; i < ${ITERATIONS}; i++ )); do
            TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_OPENMP}" $N_THREAD < ${INPUT} | tail -n 1 | awk '{print $1}')
            OPENMP_TIME=$(bc -l <<< "$OPENMP_TIME + $TIME")
        done
        OPENMP_TIME=$(bc -l <<< ${OPENMP_TIME}/${ITERATIONS})

        OPENMP_TIMES+=($OPENMP_TIME)


        #PTHREAD
        PTHREAD_TIME=0
        for (( i = 0; i < ${ITERATIONS}; i++ )); do
            TIME=$("./${BINARY_FOLDER}/${EXECUTABLE_PTHREAD}" $N_THREAD < ${INPUT} | tail -n 1 | awk '{print $1}')
            PTHREAD_TIME=$(bc -l <<< "$PTHREAD_TIME + $TIME")
        done
        PTHREAD_TIME=$(bc -l <<< ${PTHREAD_TIME}/${ITERATIONS})

        PTHREAD_TIMES+=($PTHREAD_TIME)
    done

done


T=0
for (( J = 0; J < ${#INPUTS[@]}; J++ ));
do
    echo ${INPUTS[$J]}
    echo   "---------------------------------------------------"
    printf "| # THREADS |  OPENMP T  |  SERIAL T  |  SPEEDUP  |\n"
    echo   "---------------------------------------------------"
    I=$T
    for ((; ${I} < $(bc -l <<< ${#N_THREADS[@]}+${T}); I++ ));
    do
        N_THREAD=${N_THREADS[$(bc -l <<< ${I}-${T})]}
        OPENMP_TIME=${OPENMP_TIMES[${I}]}
        SERIAL_TIME=${SERIAL_TIMES[${J}]}
        SPEEDUP=$(bc -l <<< ${SERIAL_TIME}/${OPENMP_TIME})
        printf "| %9d | %10.3f | %10.3f | %4.7f |\n" ${N_THREAD} ${OPENMP_TIME} ${SERIAL_TIME} ${SPEEDUP}
    done
    echo   "---------------------------------------------------"
    echo "\n\n"
    T=$(bc -l <<< ${#N_THREADS[@]}+${T})
done



T=0
for (( J = 0; J < ${#INPUTS[@]}; J++ ));
do
    echo ${INPUTS[$J]}
    echo   "---------------------------------------------------"
    printf "| # THREADS |  PTHREAD T |  SERIAL T  |  SPEEDUP  |\n"
    echo   "---------------------------------------------------"
    I=$T
    for ((; ${I} < $(bc -l <<< ${#N_THREADS[@]}+${T}); I++ ));
    do
        N_THREAD=${N_THREADS[$(bc -l <<< ${I}-${T})]}
        PTHREAD_TIME=${PTHREAD_TIMES[${I}]}
        SERIAL_TIME=${SERIAL_TIMES[${J}]}
        SPEEDUP=$(bc -l <<< ${SERIAL_TIME}/${PTHREAD_TIME})
        printf "| %9d | %10.3f | %10.3f | %4.7f |\n" ${N_THREAD} ${PTHREAD_TIME} ${SERIAL_TIME} ${SPEEDUP}
    done

    echo   "---------------------------------------------------"
    echo "\n\n"
    T=$(bc -l <<< ${#N_THREADS[@]}+${T})
done