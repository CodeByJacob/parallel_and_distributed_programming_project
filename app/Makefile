APP_NAME = parallel_and_distributed_programming
APP_NAME_SEQUENTIAL = $(APP_NAME)_sequential
APP_NAME_OPENMP = $(APP_NAME)_openmp
APP_NAME_MPI = $(APP_NAME)_mpi
APP_NAME_HYBRID = $(APP_NAME)_hybrid

CCOMP = gcc
MPICOMP = mpicc
LOADER = gcc

OPT = -g -DDEBUG

# LIBS
lib/timer/libtimer.a: timer.o
	ar rs lib/timer/libtimer.a lib/timer/timer.o

timer.o: lib/timer/timer.c lib/timer/timer.h
	$(CCOMP) -c lib/timer/timer.c -o lib/timer/timer.o

# APP COMPILE
compile_sequential: main.c aes/aes_sequential.c lib/timer/libtimer.a
	$(CCOMP) -o $(APP_NAME_SEQUENTIAL) main.c aes/aes_sequential.c -L./lib/timer/ -I./lib/timer/ -ltimer

compile_openmp: main.c aes/aes_openmp.c lib/timer/libtimer.a
	$(CCOMP) -o $(APP_NAME_OPENMP) -fopenmp main.c aes/aes_openmp.c -L./lib/timer/ -I./lib/timer/ -ltimer

compile_mpi: main.c aes/aes_mpi.c lib/timer/libtimer.a
	$(MPICOMP) -o $(APP_NAME_MPI) main.c aes/aes_mpi.c -L./lib/timer/ -I./lib/timer/ -ltimer

compile_hybrid: main.c aes/aes_hybrid.c lib/timer/libtimer.a
	$(MPICOMP) -o $(APP_NAME_HYBRID) -fopenmp main.c aes/aes_hybrid.c -L./lib/timer/ -I./lib/timer/ -ltimer

# APP RUN
run_sequential: compile_sequential
	./$(APP_NAME_SEQUENTIAL) Sequential_Test

run_openmp: compile_openmp
	./$(APP_NAME_OPENMP) OpenMP_Test

run_mpi: compile_mpi
	./$(APP_NAME_MPI) MPI_Test

run_hybrid: compile_hybrid
	./$(APP_NAME_HYBRID) Hybrid_Test

run_all: run_sequential run_openmp run_mpi run_hybrid
	./$(APP_NAME_SEQUENTIAL) Sequential_Test && \
    	 ./$(APP_NAME_OPENMP) OpenMP_Test && \
    	 ./$(APP_NAME_MPI) MPI_Test && \
    	 ./$(APP_NAME_HYBRID) Hybrid_Test

run_and_make_plots: run_sequential run_openmp run_mpi run_hybrid
	$(MAKE) run_all | python3 makePlot.py

# CLEAN
clean:
	rm -f .o *.a ./lib/*/*.o ./lib/*/*.a $(APP_NAME)
