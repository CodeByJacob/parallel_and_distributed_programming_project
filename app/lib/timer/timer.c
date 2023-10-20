#include "timer.h"
#include<stdio.h>
#include<time.h>
#include<sys/time.h>
#include<sys/resource.h>


/*---------------------------------------------------------
 init_time - init timer
---------------------------------------------------------*/
TimerData init_time(char* testCategory, char* testName) {
    TimerData timerData;

    timerData.ct = clock();
    getrusage(RUSAGE_SELF, &timerData.rp);
    gettimeofday(&timerData.tp, &timerData.tzp);

    timerData.testCategory = testCategory;
    timerData.testName = testName;

    return timerData;
}

/*---------------------------------------------------------
 getTimeClockInSecond - return clock time in seconds since time init
---------------------------------------------------------*/
double getTimeClockInSecond(TimerData *timerData) {

    struct timeval tk;
    struct timezone tzp;
    double daytime;

    gettimeofday(&tk, &tzp);

    daytime = (tk.tv_usec - timerData->tp.tv_usec) / 1e6 + tk.tv_sec - timerData->tp.tv_sec;

    return (daytime);
}

/*---------------------------------------------------------
 getTimeCPUInSecond - return CPU time in seconds since time init
 ---------------------------------------------------------*/
double getTimeCPUInSecond(TimerData *timerData) {

    struct rusage rk;
    double cputime;

    getrusage(RUSAGE_SELF, &rk);

    cputime = (rk.ru_utime.tv_usec - timerData->rp.ru_utime.tv_usec) / 1e6;
    cputime += rk.ru_utime.tv_sec - timerData->rp.ru_utime.tv_sec;

    return (cputime);
}

/*---------------------------------------------------------
 getStdTimeInSeconds - return CPU time in seconds since time init
 ---------------------------------------------------------*/
double getStdTimeInSeconds(TimerData *timerData) {
    clock_t time = clock();

    return (double) (time - timerData->ct) / (double) CLOCKS_PER_SEC;
}

/*---------------------------------------------------------
 printTime - print CPU and clock time in seconds since time init
 ---------------------------------------------------------*/
void printTime(TimerData *timerData) {
    double stdtime, cputime, daytime;

    stdtime = getStdTimeInSeconds(timerData);
    cputime = getTimeCPUInSecond(timerData);
    daytime = getTimeClockInSecond(timerData);

    printf("standard = %lf\n", stdtime);
    printf("CPU      = %lf\n", cputime);
    printf("clock    = %lf\n", daytime);

}
