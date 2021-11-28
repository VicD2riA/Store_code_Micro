/*Base register address header file*/
#include "stm32l1xx.h"
/*Library related header files*/
#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_utils.h"

/*Timer peripherral driver included*/
#include "stm32l1xx_ll_tim.h"

/*Library LCD*/
#include "stm32l1xx_ll_lcd.h"
#include "stm32l152_glass_lcd.h"

#include <stdio.h>

void SystemClock_Config(void);
void TIMBase_Config(void);
void GPIO_Config(void);

uint16_t cnt = 0;
char disp_str[7];
int cnt2 = 30;

int main()
{
	SystemClock_Config();
	TIMBase_Config();
	GPIO_Config();
	LCD_GLASS_Init(); //LCD init
	LL_TIM_ClearFlag_UPDATE(TIM2);
	
	sprintf(disp_str, "%d" , cnt2);
	LCD_GLASS_DisplayString((uint8_t*)disp_str);	
	while(1)
	{			
		if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET && cnt2 != 0)
		{
			LL_TIM_ClearFlag_UPDATE(TIM2);
			cnt2--;
			LL_LCD_Clear();
			
			sprintf(disp_str, "%d" , cnt2);
			LCD_GLASS_DisplayString((uint8_t*)disp_str);							
		}	
		if(cnt2 == 0)
		{
			LL_TIM_ClearFlag_UPDATE(TIM2);
			LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_6);
			LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_7);
		}
				        
	}		
}

void GPIO_Config(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct; //Declare struct for GPIO config
	
	////////OUTPUT////////////
	LL_AHB1_GRP1_EnableClock (LL_AHB1_GRP1_PERIPH_GPIOB);
	
	GPIO_InitStruct.Mode  = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull  = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;	
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);//write configuration to GPIOB registor

	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void TIMBase_Config(void)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = 1000-1;
	timbase_initstructure.Prescaler = 32000-1;
	
	LL_TIM_Init(TIM2 , &timbase_initstructure);
	LL_TIM_EnableCounter(TIM2);
	
}
void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}