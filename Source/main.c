/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


uint64_t u64Ticks=0; // Counts OS ticks (default = 1000Hz).
uint64_t u64IdleTicks=0; // Value of u64IdleTicksCnt is copied once per sec.
uint64_t u64IdleTicksCnt=0; // Counts when the OS has no task to execute.
uint16_t u16PWM1=0;

#define mainTIME_TASK_PRIORITY ( tskIDLE_PRIORITY  )
#define mainTIME2_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )

xTaskHandle hTimeTask;
xTaskHandle hTimeTask2;


GPIO_InitTypeDef  GPIO_InitStructure;

void Delay(__IO uint32_t nCount);


void myTask1(void *arg){
    portTickType xLastWakeTime = xTaskGetTickCount();

    while(1){
        GPIO_ToggleBits(GPIOD,GPIO_Pin_13);
        vTaskDelayUntil( &xLastWakeTime, ( 1000 / portTICK_RATE_MS ) );
    }
}

void myTask2(void *arg){

   portTickType xLastWakeTime = xTaskGetTickCount();

    while(1){
        GPIO_ToggleBits(GPIOD,GPIO_Pin_15);
        vTaskDelayUntil( &xLastWakeTime, ( 500 / portTICK_RATE_MS ) );
    }
}

void GPIOInit(){
	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
}

int main(void)
{
		GPIOInit();
		GPIO_SetBits(GPIOD, GPIO_Pin_12);
    
    xTaskCreate( myTask1, (signed char *) "Task1", configMINIMAL_STACK_SIZE,
    NULL, mainTIME_TASK_PRIORITY, &hTimeTask );

    xTaskCreate( myTask2, (signed char *) "Task2", configMINIMAL_STACK_SIZE,
    NULL, mainTIME2_TASK_PRIORITY, &hTimeTask2 );

	
    vTaskStartScheduler();

    // Will only get here if there was insufficient memory to create
    // the idle task.
    while(1);
}
/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

// This FreeRTOS callback function gets called once per tick (default = 1000Hz).
// ----------------------------------------------------------------------------
void vApplicationTickHook( void ) {
    ++u64Ticks;
}

// This FreeRTOS call-back function gets when no other task is ready to execute.
// On a completely unloaded system this is getting called at over 2.5MHz!
// ----------------------------------------------------------------------------
void vApplicationIdleHook( void ) {
    ++u64IdleTicksCnt;
}

// A required FreeRTOS function.
// ----------------------------------------------------------------------------
void vApplicationMallocFailedHook( void ) {
    configASSERT( 0 ); // Latch on any failure / error.
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
