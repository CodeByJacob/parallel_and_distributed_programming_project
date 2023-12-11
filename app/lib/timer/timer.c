#include <stdio.h>
#include <sys/time.h>
#include "timer.h"

TimerData init_time(char* testCategory, char* testName, int testSize) {
    TimerData timerData;
    struct timeval startTime;
    gettimeofday(&startTime, NULL);
    timerData.clockTime = (double)startTime.tv_sec + (double)startTime.tv_usec / 1e6;
    timerData.testCategory = testCategory;
    timerData.testName = testName;
    timerData.testSize = testSize;
    return timerData;
}

double calculateTimeClockInSecond(TimerData *timerData) {
    struct timeval endTime;
    gettimeofday(&endTime, NULL);
    double endTimeInSeconds = (double)endTime.tv_sec + (double)endTime.tv_usec / 1e6;
    return endTimeInSeconds - timerData->clockTime;
}

void printTime(TimerData *timerData) {
    double elapsed = calculateTimeClockInSecond(timerData);
    printf("TestCategory:%s|TestName:%s|Size:%d|ClockTime:%lf\n", timerData->testCategory, timerData->testName, timerData->testSize, elapsed);
}
