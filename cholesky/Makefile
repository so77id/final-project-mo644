# Cholesky Makefile
BINARY_FOLDER=binaries
PROGRAM="Cholesky"
GCC=gcc
FLAGS=-std=c99 -pedantic -Wall -lm
PTHREAD_FLAG=-lpthread
OPENMP_FLAG=-fopenmp

FILENAME_SERIAL=cholesky_serial.c
EXECUTABLE_SERIAL=cholesky_serial.bin

FILENAME_OPENMP=cholesky_openmp.c
EXECUTABLE_OPENMP=cholesky_openmp.bin

FILENAME_PTHREAD=cholesky_pthreads.c
EXECUTABLE_PTHREAD=cholesky_pthreads.bin

build b: create-folder build-serial build-openmp build-pthreads
	@echo "[Building] Cholesky's"
create-folder cf:
	@echo "[Creating] Folder"
	@mkdir -p $(BINARY_FOLDER)
build-serial bs: create-folder clean-serial
	@echo "[Building] Cholesky serial"
	@$(GCC) $(FLAGS) $(FILENAME_SERIAL) -o $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL)
build-openmp bo: create-folder clean-openmp
	@echo "[Building] Cholesky openmp"
	@$(GCC) $(FLAGS) $(OPENMP_FLAG) $(FILENAME_OPENMP) -o $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP)
build-pthreads bp: create-folder clean-pthreads
	@echo "[Building] Cholesky pthreads"
	@$(GCC) $(FLAGS) $(PTHREAD_FLAG) $(FILENAME_PTHREAD) -o $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD)
clean-serial cs:
	@echo "[Clean] Cholesky serial"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_SERIAL)
clean-openmp co:
	@echo "[Clean] Cholesky openmp"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_OPENMP)
clean-pthreads cp:
	@echo "[Clean] Cholesky pthreads"
	@rm -f $(BINARY_FOLDER)/$(EXECUTABLE_PTHREAD)
clean c: clean-serial clean-openmp clean-pthreads
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