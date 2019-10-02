# FREE_RTOS_STM32_Implementation
FreeRTOS STM32 Examples

1- One periodic task on FreeRTOS is desired to run. "Makefile" complies the source code Lab6_RTOS.c while "Makefile.FreeRTOS" takes care of building the OS.

The task parameters:

Period (P1) = 1500 msec, Priority (p1) = 2, Execution Amount (e1) = 75,000

2- Two tasks T1 and T2 are desired to run. (See the source code Lab6_RTOS_2Tasks.c)

The task parameters:

T1: Period (P1) = 2000 msec, Priority (p1) = 3, Execution Amount (e1) = 100,000

T2: Period (P1) = 3500 msec, Priority (p2) = 2, Execution Amount (e2) = 100,000

3- It is desired to implement resources as semaphores in FreeRTOS. One semaphore and two tasks are implemented to show blocking. (See the source code Lab6_RTOS_Semaphore.c).

4- It is desired to demonstrate a deadlock. Two semaphores and two tasks are prepared. (See the source code Lab6_RTOS_Deadlock.c).

The task parameters:

T1: Period (P1) = 1500 msec, Priority (p1) = 3, Execution Amount (e1) = 30,000

T2: Period (P1) = 2500 msec, Priority (p2) = 2, Execution Amount (e2) = 30,000




