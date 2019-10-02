
/************************************************************************
* Simple example FreeRTOS project for STM32F103.
* Creates two periodic tasks with
*  different static priorities and other parameters.
* The tasks perform several thousand floating point divisions to burn off
*  CPU time.
* A logic analyzer can be used to trace run times of tasks.
* Only one task function is created. The two tasks share the same function
*  call. Their parameters are passed within a structure from main.
* ************************************************************************/


#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stdio.h> //extra
#include <string.h>
#include "stddef.h"
#include "FreeRTOS.h"
#include "task.h"
#include "list.h" 
#include "misc.h"
#include "semphr.h"
//#include "queue.h"
// This header is not to be included directly in this source. 
// RTOS reads it at compile time:
//#include "FreeRTOSConfig.h" 

//Some convenient macros:
#define PORT_RCC RCC_APB2Periph_GPIOC
#define GPIO_PORT GPIOC
#define GPIO_ON(pin) {GPIO_WriteBit(GPIO_PORT , pin , Bit_SET);}
#define GPIO_OFF(pin) {GPIO_WriteBit(GPIO_PORT , pin , Bit_RESET);}
//LED for semaphore:
#define EXLED GPIO_Pin_2
#define EXLED2 GPIO_Pin_10

//Board LEDs:
#define BLULED GPIO_Pin_8
#define GRNLED GPIO_Pin_9
//Create dummy output pins for float division.
#define DUMY1 GPIO_Pin_6
#define DUMY2 GPIO_Pin_5


TaskHandle_t T1_handle;
TaskHandle_t T2_handle;

//Turns on Blue:
#define T1_PERIOD 2800 //For example: 2000x1ms = 2s
#define T1_Priority 3 // Large number is higher priority.

//Turns on Green:
#define T2_PERIOD 3000
#define T2_Priority 2

//Set the workload for each task
// (expect about 100,000 operations per second):
#define T1_OPERATION_COUNT  30000 //100000
#define T2_OPERATION_COUNT  30000 //100000

SemaphoreHandle_t * Semaphore1;
SemaphoreHandle_t * Semaphore2;

//Function prototypes:
void IOinit(void);
static void Task1(void *pvParam); //Function of T1
static void Task2(void *pvParam); //Function of T2
void doManyOperations1(long int OPERATIONS_COUNT);



/////////////////////////////////////////////////////////////////////


int main(void)
{	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  //Needed for STM32F10x
  
  IOinit(); 
  /* Test LEDs:
  GPIO_ON(BLULED);
  GPIO_ON(GRNLED);
  GPIO_ON(EXLED2);
  GPIO_ON(DUMY1);
  GPIO_ON(DUMY2);
  */

  //Semaphore1 = xSemaphoreCreateMutex(); //Bu pri. inversion yapmiyormus.
  Semaphore1 = xSemaphoreCreateBinary(); //Bunu yapinca almiyorlar.
  Semaphore2 = xSemaphoreCreateBinary(); //Bunu yapinca almiyorlar.
  //Binary definition requires that we "give" it after creation.
  xSemaphoreGive(Semaphore1); 
  xSemaphoreGive(Semaphore2); 

  if( Semaphore1 == NULL ){ //If not successful, stop here and signal.
  GPIO_ON(BLULED);
  GPIO_ON(GRNLED);
  GPIO_ON(EXLED);
  while(1);
  }
  
  if( Semaphore2 == NULL ){ //If not successful, stop here and signal.
  GPIO_ON(BLULED);
  GPIO_ON(GRNLED);
  GPIO_ON(EXLED2);
  while(1);
  }
  
  // Create tasks:
  xTaskCreate(Task1,"Task1",64,NULL,T1_Priority,&T1_handle); 
  xTaskCreate(Task2,"Task2",64,NULL,T2_Priority,&T2_handle); 
   
  //Finally, start the scheduler.
  vTaskStartScheduler();
  
  return 0; //It should never reach here!
}


////////////////// Task 1 ///////////////////////////////////////////////////

static void Task1(void *Params) {
  TickType_t StartTime; // This value is automatically updated.

  StartTime = xTaskGetTickCount(); // The task starts at this time.
  
  // This is the main part of the task:
  while(1){ 
    GPIO_ON(BLULED);
    if(xSemaphoreTake(Semaphore1, portMAX_DELAY)) { //Block until available.
      GPIO_ON(EXLED);
      doManyOperations1(T1_OPERATION_COUNT/3);      //Do some work.
      
    if(xSemaphoreTake(Semaphore2, portMAX_DELAY)) { //Block until available.
      GPIO_ON(EXLED2);
	  doManyOperations1(T1_OPERATION_COUNT/3);      //Do some work.
      xSemaphoreGive(Semaphore2);
      GPIO_OFF(EXLED2);
    }  
    
      doManyOperations1(T1_OPERATION_COUNT/3);      //Do some work.   
      xSemaphoreGive(Semaphore1);
      GPIO_OFF(EXLED);
    }
    GPIO_OFF(BLULED);

    vTaskDelayUntil(&StartTime, T1_PERIOD); //Sleep until next period.
  }
}


////////////////// Task 2 ///////////////////////////////////////////////////

static void Task2(void *Params) {
  TickType_t StartTime; // This value is automatically updated.

  StartTime = xTaskGetTickCount(); // The task starts at this time.
  
  // This is the main part of the task:
  while(1){ 
    GPIO_ON(GRNLED);
    if(xSemaphoreTake(Semaphore2, portMAX_DELAY)) { //Block until available.
      GPIO_ON(EXLED2);
      doManyOperations1(T2_OPERATION_COUNT/3);      //Do some work.
      
    if(xSemaphoreTake(Semaphore1, portMAX_DELAY)) { //Block until available.
      GPIO_ON(EXLED);
	  doManyOperations1(T2_OPERATION_COUNT/3);      //Do some work.
      xSemaphoreGive(Semaphore1);
      GPIO_OFF(EXLED);
    }  
    
      doManyOperations1(T2_OPERATION_COUNT/3);      //Do some work.   
      xSemaphoreGive(Semaphore2);
      GPIO_OFF(EXLED2);
    }
    GPIO_OFF(GRNLED);

    vTaskDelayUntil(&StartTime, T2_PERIOD); //Sleep until next period.
  }
}
/////////////////////////////////////////////////////////////////////

// Burn off some processor cycles:
void doManyOperations1(long int OPERATION_COUNT){
  int i;
  float y;
  for (i=0; i<OPERATION_COUNT; ++i){ 
    y=435186.0/((float)i); // the constant is just some random value.
    //It is necessary to do something with the result so that
    // the optimizer does not compile the instructions out:
    if (y==0){ 
      GPIO_OFF(DUMY1);
    }
  }
}

/////////////////////////////////////////////////////////////////////

void IOinit(){
  GPIO_InitTypeDef GPIO_InitStructure;
  // Enable Peripheral Clocks
  RCC_APB2PeriphClockCmd(PORT_RCC , ENABLE); // (1)
  // Configure Pins
  GPIO_StructInit (& GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = EXLED|EXLED2|BLULED|GRNLED|DUMY1|DUMY2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIO_PORT , &GPIO_InitStructure);

  // Configure SysTick Timer
  if (SysTick_Config(SystemCoreClock / 1000))
    while (1);  
}

// See if these may be useful later on: (AO!)
/* vTaskSuspend(xHandle); */
/* vTaskResume(xHandle); */
/* vTaskDelete(xHandle); */
/* vTaskDelay(xDelay); */

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file , uint32_t line)
{
  /* Infinite loop */
  /* Use GDB to find out why we're here */ 
  while (1);
  
}
#endif 
