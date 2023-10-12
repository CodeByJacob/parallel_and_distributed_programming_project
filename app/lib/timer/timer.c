#include "timer.h"
#include<stdio.h>
#include<time.h>
#include<sys/time.h>
#include<sys/resource.h>


/*---------------------------------------------------------
 init_time - init timer
---------------------------------------------------------*/
TimerData init_time() {
    TimerData timerData;

    timerData.ct = clock();
    getrusage(RUSAGE_SELF, &timerData.rp);
    gettimeofday(&timerData.tp, &timerData.tzp);

    return timerData;
}

/*---------------------------------------------------------
 timeClockInSecond - return clock time in seconds since time init
---------------------------------------------------------*/
double timeClockInSecond(TimerData *timerData) {

    struct timeval tk;
    struct timezone tzp;
    double daytime;

    gettimeofday(&tk, &tzp);

    daytime = (tk.tv_usec - timerData->tp.tv_usec) / 1e6 + tk.tv_sec - timerData->tp.tv_sec;

    return (daytime);
}

/*---------------------------------------------------------
 timeCPUInSecond - return CPU time in seconds since time init
 ---------------------------------------------------------*/
double timeCPUInSecond(TimerData *timerData) {

    struct rusage rk;
    double cputime;

    getrusage(RUSAGE_SELF, &rk);

    cputime = (rk.ru_utime.tv_usec - timerData->rp.ru_utime.tv_usec) / 1e6;
    cputime += rk.ru_utime.tv_sec - timerData->rp.ru_utime.tv_sec;

    return (cputime);
}

/*---------------------------------------------------------
 printTime - print CPU and clock time in seconds since time init
 ---------------------------------------------------------*/
void printTime(TimerData *timerData) {

    clock_t time;
    struct rusage rk;
    struct timeval tk;
    double stdtime, cputime, daytime;

    time = clock();
    getrusage(RUSAGE_SELF, &rk);
    gettimeofday(&tk, &timerData->tzp);

    stdtime = (double) (time - timerData->ct) / (double) CLOCKS_PER_SEC;

    cputime = (rk.ru_utime.tv_usec - timerData->rp.ru_utime.tv_usec) / 1.0e6;
    cputime += rk.ru_utime.tv_sec - timerData->rp.ru_utime.tv_sec;

    daytime = (tk.tv_usec - timerData->tp.tv_usec) / 1.0e6 + tk.tv_sec - timerData->tp.tv_sec;

    printf("standard = %lf\n", stdtime);
    printf("CPU      = %lf\n", cputime);
    printf("clock    = %lf\n", daytime);

}
