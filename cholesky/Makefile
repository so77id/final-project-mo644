# Cholesky Makefile
PROGRAM="Cholesky"
GCC=gcc-6
FLAGS=-std=c99 -pedantic -Wall
PTHREAD_FLAG=-lpthread
OPENMP_FLAG=-fopenmp

FILENAME_SERIAL=cholesky_serial.c
EXECUTABLE_SERIAL=cholesky_serial.bin

FILENAME_OPENMP=cholesky_openmp.c
EXECUTABLE_OPENMP=cholesky_openmp.bin

FILENAME_PTHREAD=cholesky_pthreads.c
EXECUTABLE_PTHREAD=cholesky_pthreads.bin

build b: build-serial build-openmp build-pthreads
	@echo "[Building] Cholesky's"
build-serial bs: clean-serial
	@echo "[Building] Cholesky serial"
	@$(GCC) $(FLAGS) $(FILENAME_SERIAL) -o $(EXECUTABLE_SERIAL)
build-openmp bo: clean-openmp
	@echo "[Building] Cholesky openmp"
	@$(GCC) $(FLAGS) $(OPENMP_FLAG) $(FILENAME_OPENMP) -o $(EXECUTABLE_OPENMP)
build-pthreads bp: clean-pthreads
	@echo "[Building] Cholesky pthreads"
	@$(GCC) $(FLAGS) $(PTHREAD_FLAG) $(FILENAME_PTHREAD) -o $(EXECUTABLE_PTHREAD)
clean-serial cs:
	@echo "[Clean] Cholesky serial"
	@rm -f $(EXECUTABLE_SERIAL)
clean-openmp co:
	@echo "[Clean] Cholesky openmp"
	@rm -f $(EXECUTABLE_OPENMP)
clean-pthreads cp:
	@echo "[Clean] Cholesky pthreads"
	@rm -f $(EXECUTABLE_PTHREAD)
clean c: clean-serial clean-openmp clean-pthreads
	@echo "[clean] Cleaning files..."
help:
	@echo ""
	@echo "Makefile for $(PROGRAM)"
	@echo ""
	@echo "make []"
	@echo "   - build  : compile service"
	@echo "   - linux  : compile service to linux"
	@echo "   - clean  : remove all compiled code"
	@echo "   - docker : create docker container"
	@echo ""
.PHONY: serve build linux docker help%