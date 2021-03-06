# Cholesky Makefile
BINARY_FOLDER=binaries
PROGRAM="Cholesky"
GCC=gcc-6
FLAGS=-std=c99 -pedantic -Wall -lm -g -O2
PTHREAD_FLAG=-lpthread
OPENMP_FLAG=-fopenmp
OUTPUT_FILE=output.dat

FILENAME_SERIAL=cholesky_serial.c
EXECUTABLE_SERIAL=cholesky_serial.bin

FILENAME_SERIAL_FOR_PARALLEL=cholesky_serial_for_parallel.c
EXECUTABLE_SERIAL_FOR_PARALLEL=cholesky_serial_for_parallel.bin

FILENAME_OPENMP=cholesky_openmp.c
EXECUTABLE_OPENMP=cholesky_openmp.bin

FILENAME_OPENMP_THREADPOOL=cholesky_openmp_threadspool.c
EXECUTABLE_OPENMP_THREADPOOL=cholesky_openmp_threadspool.bin

FILENAME_PTHREAD=cholesky_pthreads.c
EXECUTABLE_PTHREAD=cholesky_pthreads.bin

FILENAME_PTHREAD_PRODCON=cholesky_pthreads_prodcon.c
EXECUTABLE_PTHREAD_PRODCON=cholesky_pthreads_prodcon.bin

build b: create-folder build-serial build-serial-for-parallel build-openmp build-pthreads build-pthreads_prodcon build-openmp-threadpool
	@echo "[Building] Cholesky's"
create-folder cf:
	@echo "[Creating] Folder"
	@mkdir -p $(BINARY_FOLDER)

build-serial bs: create-folder clean-serial
	@echo "[Building] Cholesky serial"
	@$(GCC) $(FILENAME_SERIAL) -o $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL) $(FLAGS)

build-serial-for-parallel bsfp: create-folder clean-serial-for-parallel
	@echo "[Building] Cholesky serial for parallel"
	@$(GCC) $(FILENAME_SERIAL_FOR_PARALLEL) -o $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL_FOR_PARALLEL) $(FLAGS)

build-openmp bo: create-folder clean-openmp
	@echo "[Building] Cholesky openmp"
	@$(GCC) $(FILENAME_OPENMP) -o $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP) $(FLAGS) $(OPENMP_FLAG)

build-openmp-threadpool botp: create-folder clean-openmp-threadpool
	@echo "[Building] Cholesky openmp threadpool"
	@$(GCC) $(FILENAME_OPENMP) -o $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP_THREADPOOL) $(FLAGS) $(OPENMP_FLAG)

build-pthreads bp: create-folder clean-pthreads
	@echo "[Building] Cholesky pthreads"
	@$(GCC) $(FILENAME_PTHREAD) -o $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD) $(FLAGS) $(PTHREAD_FLAG)

build-pthreads_prodcon bppc: create-folder clean-pthreads-prodcon
	@echo "[Building] Cholesky pthreads producer consumer"
	@$(GCC) $(FILENAME_PTHREAD_PRODCON) -o $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD_PRODCON) $(FLAGS) $(PTHREAD_FLAG)

clean-serial cs:
	@echo "[Clean] Cholesky serial"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL)

clean-serial-for-parallel csfp:
	@echo "[Clean] Cholesky serial for parallel"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL_FOR_PARALLEL)

clean-openmp co:
	@echo "[Clean] Cholesky openmp"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP)

clean-openmp-threadpool cotp:
	@echo "[Clean] Cholesky openmp threadpool"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP_THREADPOOL)

clean-pthreads cp:
	@echo "[Clean] Cholesky pthreads"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD)

clean-pthreads-prodcon cppc:
	@echo "[Clean] Cholesky pthreads producer consumer"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD_PRODCON)

clean c: clean-serial clean-serial-for-parallel clean-openmp clean-openmp-threadpool clean-pthreads clean-pthreads-prodcon
	@echo "[clean] Cleaning files..."
	@rm -R $(BINARY_FOLDER)
	@mkdir -p $(BINARY_FOLDER)

run r:
	@echo "[run] Running benchmark"
	/bin/bash benchmark.sh > $(OUTPUT_FILE)
help:
	@echo ""
	@echo "Makefile for $(PROGRAM)"
	@echo ""
	@echo "make []"
	@echo "   - build  : compile programs"
	@echo "   - clean  : remove all compiled code"
	@echo ""
.PHONY: %