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
} TimerData;

TimerData init_time();

double timeCPUInSecond(TimerData *timerData);

double timeClockInSecond(TimerData *timerData);

void printTime(TimerData *timerData);

#endif //PROJECT_TIMER_H