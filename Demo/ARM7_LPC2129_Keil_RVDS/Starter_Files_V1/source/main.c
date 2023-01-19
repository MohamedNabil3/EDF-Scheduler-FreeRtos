/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/*        Tasks Handlers        */
//TaskHandle_t Task1Handler=NULL;
//TaskHandle_t Task2Handler=NULL;
TaskHandle_t Button_1_Monitor_Handler = NULL;
TaskHandle_t Button_2_Monitor_Handler = NULL;
TaskHandle_t Periodic_Transmitter_Handler = NULL;
TaskHandle_t Uart_Receiver_Handler = NULL;
TaskHandle_t Load_1_Simulation_Handler = NULL;
TaskHandle_t Load_2_Simulation_Handler = NULL;



typedef struct{
		
	unsigned char* Message;
	unsigned char  length;
}Message;

//volatile unsigned char *Messages[3]={ NULL , NULL , NULL }; 
Message strings[3] = { {NULL,0} , {NULL,0} , {NULL,0} };
volatile unsigned char Write_Index=0 , Read_Index=0;


void vApplicationIdleHook( void )
{
		//GPIO_write(PORT_0,PIN4,PIN_IS_HIGH);
		//GPIO_write(PORT_0,PIN1,PIN_IS_LOW);

}
void vApplicationTickHook( void ){

		GPIO_write(PORT_0,PIN9,PIN_IS_HIGH);
		GPIO_write(PORT_0,PIN9,PIN_IS_LOW);
		
}



void Button_1_Monitor( void * pvParameters )
{
	pinState_t Pin1_Last_state = PIN_IS_LOW;
	pinState_t Pin1_Current_state = PIN_IS_LOW;
	
	int xLastWakeTime = xTaskGetTickCount(); 

	for( ;; )
		{
			/* Task code goes here. */
			Pin1_Current_state = GPIO_read(PORT_0,PIN1);
			if ( Pin1_Current_state != Pin1_Last_state ) {
				
				if(strings[Write_Index].Message == NULL ){	//if the array of strings isn't full write new message
					
						if(Pin1_Current_state == PIN_IS_LOW){	//Falling Edge
							
								strings[Write_Index].Message = (unsigned char*)"Button1 Falling Edge" ;
								strings[Write_Index].length = 20 ;
						}
					else {	//Rising Edge
						
							strings[Write_Index].Message = (unsigned char*)"Button1 Rising Edge" ;
							strings[Write_Index].length = 19;					
					}
						Write_Index = ( Write_Index + 1) % 3 ; //return the the index to 0 if the array is full
				}

				Pin1_Last_state=Pin1_Current_state;
			}
			GPIO_write(PORT_0 , PIN5 , PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTime , 50);
			GPIO_write(PORT_0 , PIN5 , PIN_IS_HIGH);
	}
}

void Button_2_Monitor( void * pvParameters )
{
	pinState_t Pin2_Last_state = PIN_IS_LOW;
	pinState_t Pin2_Current_state = PIN_IS_LOW;
	
	int xLastWakeTime = xTaskGetTickCount(); 
	
	
	for( ;; )
		{
			/* Task code goes here. */
			Pin2_Current_state = GPIO_read(PORT_0,PIN2);
			if ( Pin2_Current_state != Pin2_Last_state ) {
				
				if(strings[Write_Index].Message == NULL ){	//if the array of strings isn't full write new message
					
						if(Pin2_Current_state == PIN_IS_LOW){	//Falling Edge
							
								strings[Write_Index].Message = (unsigned char*)"Button2 Falling Edge" ;
								strings[Write_Index].length = 20 ;
						}
					else {	//Rising Edge
						
							strings[Write_Index].Message = (unsigned char*)"Button2 Rising Edge" ;
							strings[Write_Index].length = 19 ;					
					}
					Write_Index = ( Write_Index + 1) % 3 ; //return the the index to 0 if the array is full
				}
				
				Pin2_Last_state=Pin2_Current_state;
			}
			GPIO_write(PORT_0 , PIN6 , PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTime , 50);
			GPIO_write(PORT_0 , PIN6 , PIN_IS_HIGH);
	}
}

void Periodic_Transmitter( void * pvParameters )
{
	int xLastWakeTime = xTaskGetTickCount(); 
	for( ;; )
		{
			
			/* Task code goes here. */
			if(strings[Write_Index].Message == NULL ){	//if the array of strings isn't full write new message				
						strings[Write_Index].Message = (unsigned char*)"Periodic String" ;
						strings[Write_Index].length = 15 ;
				}
				Write_Index = ( Write_Index + 1) % 3 ; //return the the index to 0 if the array is full
				GPIO_write(PORT_0 , PIN7 , PIN_IS_LOW);
				vTaskDelayUntil( &xLastWakeTime , 100);
				GPIO_write(PORT_0 , PIN7 , PIN_IS_HIGH);
			}
}
void Uart_Receiver( void * pvParameters )
{
	int xLastWakeTime = xTaskGetTickCount(); 
	for( ;; )
		{
			/* Task code goes here. */
				if(strings[Read_Index].Message != NULL){
					vSerialPutString( (signed char*)strings[Read_Index].Message , strings[Read_Index].length );
					strings[Read_Index].Message = NULL;
					strings[Read_Index].length = 0;
					Read_Index = ( Read_Index + 1 ) % 3;
				}
				GPIO_write(PORT_0 , PIN8 , PIN_IS_LOW);
				vTaskDelayUntil( &xLastWakeTime , 20);
				GPIO_write(PORT_0 , PIN8 , PIN_IS_HIGH);
	}
}
void Load_1_Simulation( void * pvParameters )
{
	int i = 0;

int xLastWakeTime = xTaskGetTickCount(); 
	for( ;; )
	{
			/* Task code goes here. */
			
			for(i=0;i<33000; i++)
			{
				i = i;
			}
			GPIO_write(PORT_0,PIN3,PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTime , 10);
			GPIO_write(PORT_0 , PIN3 , PIN_IS_HIGH);
	}
}
void Load_2_Simulation( void * pvParameters )
{
		int i = 0;
		int xLastWakeTime = xTaskGetTickCount();		 
		for( ;; )
		{
			/* Task code goes here. */
			for(i=0;i<48000;i++)
			{
				i = i;
			}
			GPIO_write(PORT_0 , PIN4 , PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTime , 100);
			GPIO_write(PORT_0 , PIN4 , PIN_IS_HIGH);
			}
}




/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

		
    /* Create Tasks here 
	
	 // xTaskCreate(    
	//									Task1,
   //                 "Task1",           Text name for the task. 
    //                100,     				 Stack size in words, not bytes. 
     //               ( void * ) 0,    Parameter passed into the task. 
     //               1,									 Priority at which the task is created. 
    //                &Task1Handler );       Used to pass out the created task's handle. 
	  //xTaskCreate(    
		//								Task2,
     //               "Task2",          Text name for the task. 
      //              100,     				 Stack size in words, not bytes. 
      //              ( void * ) 0,    Parameter passed into the task. 
       //             1,									 Priority at which the task is created. 
      //              &Task2Handler );       Used to pass out the created task's handle. */
	
	
 	  		xTaskPeriodicCreate(    
										Button_1_Monitor,
                    "Button_1_Monitor",          /* Text name for the task. */
                    100,     				 /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,									/* Priority at which the task is created. */
                    &Button_1_Monitor_Handler,
										50 );      /* Used to pass out the created task's handle. */
										
	  xTaskPeriodicCreate(    
										Button_2_Monitor,
                    "Button_2_Monitor",          /* Text name for the task. */
                    100,     				 /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,									/* Priority at which the task is created. */
                    &Button_2_Monitor_Handler,
										50);      /* Used to pass out the created task's handle. */
										
	  xTaskPeriodicCreate(    
										Periodic_Transmitter,
                    "Periodic_Transmitter",          /* Text name for the task. */
                    100,     				 /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,									/* Priority at which the task is created. */
                    &Periodic_Transmitter_Handler,
										100 );      /* Used to pass out the created task's handle. */
	  xTaskPeriodicCreate(    
										Uart_Receiver,
                    "Uart_Receiver",          /* Text name for the task. */
                    100,     				 /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,									/* Priority at which the task is created. */
                    &Uart_Receiver_Handler,
										20 );      /* Used to pass out the created task's handle. */

	  xTaskPeriodicCreate(    
										Load_1_Simulation,
                    "Load_1_Simulation",          /* Text name for the task. */
                    100,     				 /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,									/* Priority at which the task is created. */
                    &Load_2_Simulation_Handler,
										10 );      /* Used to pass out the created task's handle. */		
	  xTaskPeriodicCreate(    
										Load_2_Simulation,
                    "Load_2_Simulation",          /* Text name for the task. */
                    100,     				 /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    1,									/* Priority at which the task is created. */
                    &Load_2_Simulation_Handler,
										100 );      /* Used to pass out the created task's handle. */										
	
	

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


