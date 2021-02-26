/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "timer/timer.h"
#include "joystick/joystick.h"
#include "utilities/utilities.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	joystick_init();											/* Joystick Initialization            */
  LCD_Initialization();
	TP_Init();
	TouchPanel_Calibrate();

	LCD_Clear(Purple);
	preparaGioco();												//si disegna la schermata iniziale
	
	init_timer(0, 0x4E2 ); 						    /* 50us * 25MHz = 1.25*10^3 = 0x4E2 => polling */
	enable_timer(0);								
	init_timer(1, 0x017D7840);						/* 1s * 25MHz = 0x17D 7840     => velocità di movimento*/
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
