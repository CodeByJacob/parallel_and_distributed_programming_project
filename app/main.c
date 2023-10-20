#include<stdio.h>

#include "lib/timer/timer.h"

void arithmetic_operation() {
    TimerData timerData = init_time("Test Category", "Test Name");
    int suma = 0;

    for (int i = 1; i <= 100; i++) {
        suma += i;
    }

    printf("Sum of numbers from 1 to 100: %d\n", suma);
    printTime(&timerData);
}

int main() {
    arithmetic_operation();
    return 0;
}

