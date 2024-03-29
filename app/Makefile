APP_NAME = parallel_and_distributed_programming
APP_NAME_SEQUENTIAL = $(APP_NAME)_sequential
APP_NAME_OPENMP = $(APP_NAME)_openmp
APP_NAME_MPI = $(APP_NAME)_mpi
APP_NAME_HYBRID = $(APP_NAME)_hybrid

PYTHON_ENV = ./venv/bin

prepare_python:
	@(python3 -m venv venv && chmod +x $(PYTHON_ENV)/activate && $(PYTHON_ENV)/activate && $(PYTHON_ENV)/pip3 install -r requirements.txt) > /dev/null 2>&1

CCOMP = gcc
MPICOMP = mpicc
LOADER = gcc

OPT = -g -DDEBUG

# LIBS
LIBS = -L./lib/timer/ -I./lib/timer/ -ltimer -L./lib/test_helper/ -I./lib/test_helper/ -ltest_helper
LIBS_MPI = -L./lib/timer/ -I./lib/timer/ -ltimer_mpi -L./lib/test_helper/ -I./lib/test_helper/ -ltest_helper

lib/timer/libtimer.a: timer.o
	@ar rs lib/timer/libtimer.a lib/timer/timer.o

timer.o: lib/timer/timer.c lib/timer/timer.h
	@$(CCOMP) -c lib/timer/timer.c -o lib/timer/timer.o

lib/timer/libtimer_mpi.a: timer_mpi.o
	@ar rs lib/timer/libtimer_mpi.a lib/timer/timer_mpi.o

timer_mpi.o: lib/timer/timer_mpi.c lib/timer/timer.h
	@$(MPICOMP) -c lib/timer/timer_mpi.c -o lib/timer/timer_mpi.o

lib/test_helper/libtest_helper.a: test_helper.o
	@ar rs lib/test_helper/libtest_helper.a lib/test_helper/test_helper.o

test_helper.o: lib/test_helper/test_helper.c lib/test_helper/test_helper.h
	@$(CCOMP) -c lib/test_helper/test_helper.c -o lib/test_helper/test_helper.o

# APP COMPILE
compile_sequential: main.c aes/aes_sequential.c lib/timer/libtimer.a lib/test_helper/libtest_helper.a
	@$(CCOMP) -o $(APP_NAME_SEQUENTIAL) main.c aes/aes_common.c aes/aes_sequential.c $(LIBS)

compile_openmp: main.c aes/aes_openmp.c lib/timer/libtimer.a lib/test_helper/libtest_helper.a
	@$(CCOMP) -o $(APP_NAME_OPENMP) -fopenmp main.c aes/aes_common.c aes/aes_openmp.c $(LIBS)

compile_mpi: main.c aes/aes_mpi.c lib/timer/libtimer_mpi.a lib/test_helper/libtest_helper.a
	@$(MPICOMP) -o $(APP_NAME_MPI) main.c aes/aes_common.c aes/aes_mpi.c $(LIBS_MPI)

compile_hybrid: main.c aes/aes_hybrid.c lib/timer/libtimer_mpi.a lib/test_helper/libtest_helper.a
	@$(MPICOMP) -o $(APP_NAME_HYBRID) -fopenmp main.c aes/aes_common.c aes/aes_hybrid.c $(LIBS_MPI)

compile_all: compile_sequential compile_openmp compile_mpi compile_hybrid

# APP RUN
run_sequential: compile_sequential
	@./$(APP_NAME_SEQUENTIAL) Sequential_Test

run_sequential_valgrind: compile_sequential
	@valgrind --leak-check=full ./$(APP_NAME_SEQUENTIAL) Sequential_Test

run_openmp: compile_openmp
	@./$(APP_NAME_OPENMP) OpenMP_Test

run_mpi: compile_mpi
	@mpirun ./$(APP_NAME_MPI) MPI_Test

run_mpi_valgrind: compile_mpi
	@mpirun valgrind --leak-check=full ./$(APP_NAME_MPI) MPI_Test

run_hybrid: compile_hybrid
	@mpirun ./$(APP_NAME_HYBRID) Hybrid_Test

run_all: compile_all
	./$(APP_NAME_SEQUENTIAL) Sequential_Test 2>/dev/null; \
    ./$(APP_NAME_OPENMP) OpenMP_Test 2>/dev/null; \
    mpirun ./$(APP_NAME_MPI) MPI_Test 2>/dev/null || true; \
    mpirun -np 2 ./$(APP_NAME_HYBRID) Hybrid_Test 2>/dev/null || true;


run_all_for_all_processes: compile_all
	./$(APP_NAME_SEQUENTIAL) Sequential_Test 2>/dev/null; \
    ./$(APP_NAME_OPENMP) OpenMP_Test 2>/dev/null; \
    (for i in 2 3 4; do \
    	mpirun -np $$i ./$(APP_NAME_MPI) MPI_Test-$$i 2>/dev/null || true; \
    done;) \
    mpirun -np 2 ./$(APP_NAME_HYBRID) Hybrid_Test 2>/dev/null || true;


run_and_make_plots: prepare_python
	(for i in 1 2 3; do \
   		$(MAKE) run_all 2>/dev/null; \
   		sleep 1; \
   	done;) | $(PYTHON_ENV)/python3 makePlot.py && ls results/


run_hybrid_for_all_processes_and_threads: compile_sequential compile_hybrid
	for i in 2; do \
   		for j in 2 3; do \
   			export OMP_NUM_THREADS=$$j; \
   			mpirun -np $$i ./$(APP_NAME_HYBRID) Hybrid_Test-$$i-$$j 2>/dev/null || true; \
   			sleep 1; \
   		done; \
   	done;

run_openmp_for_all_threads: compile_sequential compile_openmp
	for j in 2 3 4; do \
		export OMP_NUM_THREADS=$$j; \
		./$(APP_NAME_OPENMP) OpenMP_Test-$$j || true; \
		sleep 1; \
	done; \


run_mpi_for_all_processes: compile_sequential compile_mpi
	for i in 2 3 4; do \
		mpirun -np $$i ./$(APP_NAME_HYBRID) MPI_Test-$$i 2>/dev/null || true; \
		sleep 1; \
   	done;

run_all_speedup_and_efficiency_and_make_plots: compile_sequential compile_openmp compile_mpi compile_hybrid
	(for i in 1 2 3; do \
		   		./$(APP_NAME_SEQUENTIAL) Sequential_Test 2>/dev/null; \
		   		$(MAKE) run_openmp_for_all_threads 2>/dev/null; \
                sleep 1; \
                $(MAKE) run_mpi_for_all_processes 2>/dev/null; \
                sleep 1; \
                $(MAKE) run_hybrid_for_all_processes_and_threads 2>/dev/null; \
		   		sleep 1; \
		   	done;) | $(PYTHON_ENV)/python3 makeSpeedUpAndEfficiencyPlots.py && ls results/

# CLEAN
clean:
	@rm -f .o *.a ./lib/*/*.o ./lib/*/*.a $(APP_NAME)*

clean_results:
	@rm -fr results
