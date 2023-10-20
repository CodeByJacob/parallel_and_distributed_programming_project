#include "timer.h"
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <time.h>

/*---------------------------------------------------------
 init_time - init timer
---------------------------------------------------------*/
TimerData init_time(char *testCategory, char *testName) {
  TimerData timerData;

  timerData.ct = clock();
  getrusage(RUSAGE_SELF, &timerData.rp);
  gettimeofday(&timerData.tp, &timerData.tzp);

  timerData.testCategory = testCategory;
  timerData.testName = testName;

  return timerData;
}

/*---------------------------------------------------------
 calculateTimeClockInSecond - return clock time in seconds since time init
---------------------------------------------------------*/
double calculateTimeClockInSecond(TimerData *timerData) {

  struct timeval tk;
  struct timezone tzp;
  double daytime;

  gettimeofday(&tk, &tzp);

  daytime = (tk.tv_usec - timerData->tp.tv_usec) / 1e6 + tk.tv_sec -
            timerData->tp.tv_sec;

  return (daytime);
}

/*---------------------------------------------------------
 calculateTimeCPUInSecond - return CPU time in seconds since time init
 ---------------------------------------------------------*/
double calculateTimeCPUInSecond(TimerData *timerData) {

  struct rusage rk;
  double cputime;

  getrusage(RUSAGE_SELF, &rk);

  cputime = (rk.ru_utime.tv_usec - timerData->rp.ru_utime.tv_usec) / 1e6;
  cputime += rk.ru_utime.tv_sec - timerData->rp.ru_utime.tv_sec;

  return (cputime);
}

/*---------------------------------------------------------
 calculateStdTimeInSeconds - return CPU time in seconds since time init
 ---------------------------------------------------------*/
double calculateStdTimeInSeconds(TimerData *timerData) {
  clock_t time = clock();

  return (double)(time - timerData->ct) / (double)CLOCKS_PER_SEC;
}

/*---------------------------------------------------------
 printTime - print CPU and clock time in seconds since time init
 ---------------------------------------------------------*/
void printTime(TimerData *timerData) {
  double stdTime, cpuTime, clockTime;

  stdTime = calculateStdTimeInSeconds(timerData);
  cpuTime = calculateTimeCPUInSecond(timerData);
  clockTime = calculateTimeClockInSecond(timerData);

  printf("TestCategory:%s|TestName:%s|TimeStandard:%lf|TimeCPU:%lf|TimeClock:%"
         "lf\n",
         timerData->testCategory, timerData->testName, stdTime, cpuTime,
         clockTime);
}
