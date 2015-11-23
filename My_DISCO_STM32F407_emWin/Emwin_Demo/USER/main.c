/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "SSD1963.h"
#include "GUI.h"
#include "My_DISCO_BSP.h"
#include "My_DISCO_BSP_Debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
static __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime); 

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	SSD1963_Init();

	SSD1963_Clear(Red);
	Delay(100);
	SSD1963_Clear(Blue);
	Delay(100);
	SSD1963_Clear(Red);
	SSD1963_Fill(0, 0, 50, 50, Blue);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	GUI_Init();
	GUI_SetColor(GUI_RED);
	GUI_Clear();
	
	BSP_KeyInit( BSP_KEY1, BSP_KEY1_GPIO_PORT );
	BSP_SPI_Init( );
	
	//Debug_ShowRegister( SPI2_BASE, (DebugPeripheralTypedef *)&DeBugSPI );
	//while(1);
	
	BSP_USART_Init( );
	
	//Debug_ShowRegister( GPIOA_BASE, (DebugPeripheralTypedef *)&DeBugGPIO );
	//Debug_ShowRegister( USART1_BASE, (DebugPeripheralTypedef *)&DeBugUART );
	//Debug_ShowRegister( RCC_BASE, (DebugPeripheralTypedef *)&DeBugRCC );
	//Debug_ShowSpecificRegister( RCC_BASE, (DebugPeripheralTypedef *)&DeBugRCC, "APB2ENR" );
	
	while(1)
	{
	  USART_SendData(USART1, 'b');
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {}
		
		Debug_ShowRegister( USART1_BASE, (DebugPeripheralTypedef *)&DeBugUART );	
	}
	
  while (1)
  {
		static uint32_t i = 0;
		
		SPI_I2S_SendData( BSP_SPIx, 'A' );
		Debug_ShowRegister( SPI2_BASE, (DebugPeripheralTypedef *)&DeBugSPI );
		GUI_DispDec(i++,5);
		
		Delay(50);
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0)
  { 
    TimingDelay--;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
