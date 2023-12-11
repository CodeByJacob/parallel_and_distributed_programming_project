#ifndef PROJECT_TIMER_H
#define PROJECT_TIMER_H

#include<time.h>
#include<sys/time.h>
#include<sys/resource.h>

typedef struct TimerData{
    double clockTime;
    char *testCategory;
    char *testName;
} TimerData;

TimerData init_time(char* testCategory, char* testName);

double calculateTimeClockInSecond(TimerData *timerData);

void printTime(TimerData *timerData);

#endif //PROJECT_TIMER_H
