APP_NAME = parallel_and_distributed_programming

CCOMP = gcc
MPICOMP = mpicc
LOADER = gcc

OPT = -g -DDEBUG

# LIBS
lib/timer/libtimer.a: timer.o
	ar rs lib/timer/libtimer.a lib/timer/timer.o

timer.o: lib/timer/timer.c lib/timer/timer.h
	$(CCOMP) -c lib/timer/timer.c -o lib/timer/timer.o

# APP
compile_sequential: main.c aes/aes_sequential.c lib/timer/libtimer.a
	$(CCOMP) -o $(APP_NAME) main.c aes/aes_sequential.c -L./lib/timer/ -I./lib/timer/ -ltimer

compile_openmp: main.c aes/aes_openmp.c lib/timer/libtimer.a
	$(CCOMP) -o $(APP_NAME) -fopenmp main.c aes/aes_openmp.c -L./lib/timer/ -I./lib/timer/ -ltimer

compile_mpi: main.c aes/aes_mpi.c lib/timer/libtimer.a
	$(MPICOMP) -o $(APP_NAME) main.c aes/aes_mpi.c -L./lib/timer/ -I./lib/timer/ -ltimer

compile_hybrid: main.c aes/aes_hybrid.c lib/timer/libtimer.a
	$(MPICOMP) -o $(APP_NAME) -fopenmp main.c aes/aes_hybrid.c -L./lib/timer/ -I./lib/timer/ -ltimer

run_and_make_plots:
	./$(APP_NAME) | python3 makePlot.py

clean:
	rm -f .o *.a ./lib/*/*.o ./lib/*/*.a $(APP_NAME)
