APP_NAME = parallel_and_distributed_programming

CCOMP = gcc
LOADER = gcc

OPT = -g -DDEBUG

# LIBS
lib/timer/libtimer.a: timer.o
	ar rs lib/timer/libtimer.a lib/timer/timer.o

timer.o: lib/timer/timer.c lib/timer/timer.h
	$(CCOMP) -c lib/timer/timer.c -o lib/timer/timer.o

# APP
compile: main.c lib/timer/libtimer.a
	$(CCOMP) -o $(APP_NAME) main.c -L./lib/timer/ -I./lib/timer/ -ltimer

run_and_make_plots: compile
	./$(APP_NAME) | python3 makePlot.py

clean:
	rm -f .o *.a ./lib/*/*.o ./lib/*/*.a $(APP_NAME)
