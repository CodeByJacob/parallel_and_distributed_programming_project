#include<stdio.h>

#include "lib/timer/timer.h"

void arithmetic_operation(int loopSize, char *testName) {
    TimerData timerData = init_time("Arythmetic test", testName);
    int suma = 0;

    for (int i = 1; i <= loopSize; i++) {
        suma += i * i * i;
    }

    printTime(&timerData);
}

void anotherArythmeticOperation(int loopSize, char *testName) {
  TimerData timerData = init_time("Another Arythmetic test", testName);
  int suma = 0;

  for (int i = 1; i <= loopSize; i++) {
    suma += i * i * 3 * 2 * 5;
  }

  printTime(&timerData);
}

int main() {
    arithmetic_operation(100, "LoopSize=100");
    arithmetic_operation(1000, "LoopSize=1000");
    arithmetic_operation(10000, "LoopSize=10000");

    anotherArythmeticOperation(100, "LoopSize=100");
    anotherArythmeticOperation(1000, "LoopSize=1000");
    anotherArythmeticOperation(10000, "LoopSize=10000");

    return 0;
}

