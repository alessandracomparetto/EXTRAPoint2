/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../utilities/utilities.h"

extern uint8_t partita;
extern uint8_t mode;
extern unsigned char direzione;
extern uint8_t vittoria;

/******************************************************************************
** TIMER 0 USATO PER POLLING DI TouchPanel e Joystick
******************************************************************************/

int count = 0;
int cancella = 0;

void TIMER0_IRQHandler (void)
{
	
	static int select = 0; 		//pressione di select
	static int joystick = 0;  //uso del joystick
	
	getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
	
	if (partita == 0){
		if(display.y >= 15 && display.y <= 225 && partita == 0){ //pressione sull'area del gioco
			partita = 1;
			GUI_Text(65, 150,"              ", White, White);
			GUI_Text(65, 162,"            ", White, White);
			GUI_Text(80, 47, (uint8_t *) "   Move   ", Purple, Yellow);

			GUI_Text(200, 0, (uint8_t *) "     ", Purple, Purple);
			
			posizionaRobot();
		}
	}
	
	if (partita == 1){
		//***********************************
		// RESTART and CLEAR BUTTONS
		//***********************************
		
		if ((display.y>=270 && display.y<=305)&& (display.x>= 15 && display.x<=105)){ //RESTART BUTTON
			partita = 0;
			vittoria = 0;
			GUI_Text(32, 281, (uint8_t *) "Restart", Magenta, White);
			GUI_Text(80, 47, (uint8_t *) "           ", Purple, Purple);
			coloraRettangolo(15, 69, 225, 251, White); //spazio per giocare
			GUI_Text(65, 150,"Touch to start", Purple, White);
			GUI_Text(65, 162,"  a new game", Purple, White);
			GUI_Text(32, 281, (uint8_t *) "Restart", Purple, White);
		}
		
		if (cancella == 0 && vittoria == 0){ //CLEAR BUTTON
			if ((display.y>=270 && display.y<=305) && (display.x>=135 && display.x<=225)){
				cancella = 1;
				GUI_Text(160, 281, (uint8_t *) "Clear", Magenta, Yellow);
				clear();
				mostraOstacoli();
				GUI_Text(160, 281, (uint8_t *) "Clear", Purple, Yellow);
				cancella = 0;
			}
		}
			
		//***********************************
		//JOYSTICK
	  // abilito il joystick a partita iniziata, ovvero dopo aver premuto il touch
		//***********************************

		if((LPC_GPIO1->FIOPIN & (1<<25)) == 0 && vittoria == 0){ /* Joytick Select pressed */
			select++;
			if(select == 1){
				if (mode == 0){
					GUI_Text(80, 47, (uint8_t *) " Explore ", Purple, Yellow);
					mode = 1;
					disegnaRobot();
				}else{
					GUI_Text(80, 47, (uint8_t *) "   Move  ", Purple, Yellow);
					mode = 0;
					disegnaRobot();
				}
			}
		}	else{
			select = 0;
		}
		
		if ((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && vittoria == 0){ //JOY_DOWN
			joystick ++;
			direzione = 1;
		}
		else if ((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && vittoria == 0){ //JOY_LEFT
			joystick ++;
			direzione = 0;
		}
		else if ((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && vittoria == 0){ //JOY_RIGHT
			joystick ++;
			direzione = 2;
		}
		else if ((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && vittoria == 0){ //JOY_UP
			joystick ++;
			direzione = 3;
		}
		else { //quando nessun pin è attivo riporto tutto allo stato iniziale
			joystick = 0;
			disable_timer(1);
			reset_timer(1);
		}
		
		if (joystick == 1 ){
			//controllo la modalità per compiere azioni diverse
			if(mode == 1) ruotaRobot();
			if(mode == 0) muovi();  
			mostraOstacoli();
			if(vittoria == 0 && mode == 0) enable_timer(1);
		}
	}
  
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** TIMER 1 USATO PER LA VELOCITA' DI MOVIMENTO
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	muovi();
	mostraOstacoli();
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
