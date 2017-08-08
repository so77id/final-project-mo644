# Final project MO644

The objective of this project is the parallelization of the Cholesky's algorithm using the OpenMP and Pthread tools.

## Students
RA: 147512
Name: Nathália Menini Cardoso dos Santos
Email: nathmenini@gmail.com

RA: 192744
Name: Miguel Antonio Rodriguez Santander
Email: m.rodriguezs1990@gmail.com

## Makefile

A `Makefile` has been created with the following commands:
* `make`: This command creates a folder called `binaries` in which compiles the three codes created for the benchmark.
* `make clean`: This commnad delete all the content of the `binaries` folder.
*  `make run`: This command runs the benchmark with the input files in the `input` folder, which will be run with different number of threads (16, 24 and 32).


## Compilation
Comand:
``` bash
make
```

The files will be compiled are:
* `cholesky_serial.c`: original version of Cholesky's descomposition.
*  `cholesky_serial_for_parallel.c`: serial version, which is parallelizable (slower than the original).
*  `cholesky_openmp.c`: parallelization using the `OpenMP` tools.
*  `cholesky_pthreads.c`: parallelization using `Pthreads` tool, in which the number of calculations is divided for each thread.
*  `cholesky_pthreads_prodcon.c`: implementation of producer-consumer architecture with `Pthreads`.

## Execution
The inputs of the codes will be by standard input, which will consist of two lines, the first will be the square matrix size `n`, and the second will be `n*n` numbers representing the matrix.

The number of threads will be give by argument input.

Example:

The execution of serial code:
``` bash
./binaries/cholesky_serial.bin < ./input/matrix_5000x5000.in
```

The execution of parallel code:
``` bash
./binaries/cholesky_pthreads.bin 32 < ./input/matrix_5000x5000.in
```

## Cleaning
Command:
``` bash
make clean
```
This command delete all compiled codes.


## Benchmark
Comand:
``` bash
make run
```

This commnad execute the file `run.sh`, which is configured for execute the different implementations with each input files and with different number of threads. For the benchmark the number of threads will be `16`, `24` and `32`.

The `speedup` is calculated as: `parallel time / serial time`

#### output.dat
```txt
./input/matrix_5000x5000.in
--------------------------
Number of threads: 16
OpenMP SPEEDUP: 2.0587971
Pthread SPEEDUP: 1.8232368
Pthread Producer consumer SPEEDUP: 0.0179181

Number of threads: 24
OpenMP SPEEDUP: 1.6803953
Pthread SPEEDUP: 1.4147363
Pthread Producer consumer SPEEDUP: 0.0114220

Number of threads: 32
OpenMP SPEEDUP: 1.4583506
Pthread SPEEDUP: 1.0219771
Pthread Producer consumer SPEEDUP: 0.0114220


./input/matrix_6000x6000.in
--------------------------
Number of threads: 16
OpenMP SPEEDUP: 2.0587971
Pthread SPEEDUP: 1.8232368
Pthread Producer consumer SPEEDUP: 0.0179181

Number of threads: 24
OpenMP SPEEDUP: 1.6803953
Pthread SPEEDUP: 1.4147363
Pthread Producer consumer SPEEDUP: 0.0114220

Number of threads: 32
OpenMP SPEEDUP: 1.4583506
Pthread SPEEDUP: 1.0219771
Pthread Producer consumer SPEEDUP: 0.0114220


./input/matrix_7000x7000.in
--------------------------
Number of threads: 16
OpenMP SPEEDUP: 2.0587971
Pthread SPEEDUP: 1.8232368
Pthread Producer consumer SPEEDUP: 0.0179181

Number of threads: 24
OpenMP SPEEDUP: 1.6803953
Pthread SPEEDUP: 1.4147363
Pthread Producer consumer SPEEDUP: 0.0114220

Number of threads: 32
OpenMP SPEEDUP: 1.4583506
Pthread SPEEDUP: 1.0219771
Pthread Producer consumer SPEEDUP: 0.0114220
```