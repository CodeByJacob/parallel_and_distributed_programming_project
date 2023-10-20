#ifndef PROJECT_TIMER_H
#define PROJECT_TIMER_H


#include<time.h>
#include<sys/time.h>
#include<sys/resource.h>

typedef struct TimerData {
    clock_t ct;
    struct rusage rp;
    struct timeval tp;
    struct timezone tzp;
    char* testCategory;
    char* testName;
} TimerData;

TimerData init_time(char* testCategory, char* testName);

double getTimeCPUInSecond(TimerData *timerData);

double getTimeClockInSecond(TimerData *timerData);

double getStdTimeInSeconds(TimerData *timerData);

void printTime(TimerData *timerData);

#endif //PROJECT_TIMER_H