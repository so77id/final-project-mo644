# Cholesky Makefile
BINARY_FOLDER=binaries
PROGRAM="Cholesky"
GCC=gcc
FLAGS=-std=c99 -pedantic  -Wall -lm -g -O2
PTHREAD_FLAG=-lpthread
OPENMP_FLAG=-fopenmp

FILENAME_SERIAL=cholesky_serial2.c
EXECUTABLE_SERIAL=cholesky_serial2.bin

FILENAME_OPENMP=cholesky_openmp.c
EXECUTABLE_OPENMP=cholesky_openmp.bin

FILENAME_PTHREAD=cholesky_pthreads.c
EXECUTABLE_PTHREAD=cholesky_pthreads.bin

FILENAME_PTHREAD_PRODCON=cholesky_pthreads_prodcon.c
EXECUTABLE_PTHREAD_PRODCON=cholesky_pthreads_prodcon.bin

build b: create-folder build-serial build-openmp build-pthreads build-pthreads_prodcon
	@echo "[Building] Cholesky's"
create-folder cf:
	@echo "[Creating] Folder"
	@mkdir -p $(BINARY_FOLDER)
build-serial bs: create-folder clean-serial
	@echo "[Building] Cholesky serial"
	@$(GCC) $(FILENAME_SERIAL) -o $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL) $(FLAGS)
build-openmp bo: create-folder clean-openmp
	@echo "[Building] Cholesky openmp"
	@$(GCC) $(FILENAME_OPENMP) -o $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP) $(FLAGS) $(OPENMP_FLAG)
build-pthreads bp: create-folder clean-pthreads
	@echo "[Building] Cholesky pthreads"
	@$(GCC) $(FILENAME_PTHREAD) -o $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD) $(FLAGS) $(PTHREAD_FLAG)
build-pthreads_prodcon bppc: create-folder clean-pthreads
	@echo "[Building] Cholesky pthreads producer consumer"
	@$(GCC) $(FILENAME_PTHREAD_PRODCON) -o $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD_PRODCON) $(FLAGS) $(PTHREAD_FLAG)
clean-serial cs:
	@echo "[Clean] Cholesky serial"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL)
clean-openmp co:
	@echo "[Clean] Cholesky openmp"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP)
clean-pthreads cp:
	@echo "[Clean] Cholesky pthreads"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD)
clean-pthreads_prodcon cppc:
	@echo "[Clean] Cholesky pthreads producer consumer"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD_PRODCON)
clean c: clean-serial clean-openmp clean-pthreads clean-pthreads_prodcon
	@echo "[clean] Cleaning files..."
	@rm -R $(BINARY_FOLDER)
	@mkdir -p $(BINARY_FOLDER)
help:
	@echo ""
	@echo "Makefile for $(PROGRAM)"
	@echo ""
	@echo "make []"
	@echo "   - build  : compile programs"
	@echo "   - clean  : remove all compiled code"
	@echo ""
.PHONY: serve build linux docker help%