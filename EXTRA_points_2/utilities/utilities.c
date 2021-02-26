#ifndef UTILITIES
#define UTILITIES

#include "LPC17xx.h"
#include "utilities.h"
#include "../GLCD/GLCD.h" 
#include "../GLCD/AsciiLib.h"

//*********************************************************************************
// dichiarazione delle variabili
//*********************************************************************************
unsigned const char labirinto[13][15] = { {2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2},
																				 {0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																				 {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
																				 {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
																				 {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
																				 {1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0},
																				 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
																				 {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
																				 {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
																				 {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																				 {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																				 {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
																				 {2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2} };

uint8_t posizione[2]; 	 			 //posizione iniziale del robot
unsigned char direzione = 0;   //direzione iniziale del robot => 
															 //3 NORD - 2 EST - 1 SUD - 0 OVEST
uint8_t vittoria = 0;					 // flag che indica la fine del gioco
uint8_t partita = 0; 					 // il gioco non è ancora partito!
uint8_t mode = 0;							 // mode = 0 MOVE, MODE = 1 EXPLORE																				 
					
int Pink = RGB565CONVERT(239, 187, 255); //definizione del colore del percorso
																				 
//*************************************************
// (x0,y0) punto in alto a sinistra, (x1, y1) punto in basso a destra
//*************************************************
void coloraRettangolo(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1 , uint16_t color )
{
	short end_col = x1;
	short start_col = x0;
	short row = y0;
		
	while (row != y1){
		LCD_DrawLine(start_col, row, end_col, row, color);
		row ++;
	}
}

//*************************************************
// ritorna il valore in pixel di una certa riga o colonna
//*************************************************
int pixel(uint8_t coordinata, char d)
{
	if ( d == 'x'){ //riga
		return 69 + (14*coordinata);
	} else{ //colonna
			return 15 + (14*coordinata);
	}
}
//*************************************************
// disegna il triangolo
//*************************************************
void disegnaRobot()
	{
	uint16_t c; //colore del robot
	int i, end_col, end_row;
	int row = pixel(posizione[0], 'x');  // x e y sono le coordinate in pixel della casella 
	int col = pixel(posizione[1], 'y');
	
	if (mode == 0){
		c = Magenta;
	}else{
		c = Black;
	}
	
	if (direzione == 0){ //ovest
		row += 2;
		col += 12;
		end_col = col - 2;
		for(i=0; i<10; i++) {
			LCD_DrawLine(col, row, end_col, row, c);
			if ( i == 4){
				continue;
			}
			if (i < 5){
				end_col-=2;
			}else{
				end_col+=2;
			}
			row++;
		}
	}

	if (direzione == 1){ //sud
		row += 2;
		col += 2;
		end_row = row + 2;
		for(i=0; i<10; i++) {
			LCD_DrawLine(col, row, col, end_row, c);
			if ( i == 4){
				continue;
			}
			if (i < 5){
				end_row+=2;
			}else{
				end_row-=2;
			}
			col++;
		}
	}
	
	if (direzione == 2){ //est
		row += 2;
		col += 2;
		end_col = col + 2;
		
		for(i=0; i<10; i++) {
			LCD_DrawLine(col, row, end_col, row, c);
			if ( i == 4){
				continue;
			}
			if (i < 5){
				end_col+=2;
			}else{
				end_col-=2;
			}
			row++;
		}
	}
	
	if (direzione == 3){ //nord
		row += 12;
		col += 2;
		end_row = row - 2;
		for(i=0; i<10; i++) {
			LCD_DrawLine(col, row, col, end_row, c);
			if ( i == 4){
				continue;
			}
			if (i < 5){
				end_row-=2;
			}else{
				end_row+=2;
			}
			col++;
		}
	}
}
	
//*************************************************
// colora una sola casella
// le caselle sono 14 x 14 px 
//*************************************************
void coloraCasella(uint8_t x, uint8_t y, uint16_t c)
{
	int row = pixel(x, 'x');  
	int col = pixel(y, 'y');
	
	coloraRettangolo(col+1, row, col+12, row+13, c);
}
//*************************************************
// disegna il triangolo
//*************************************************
void soluzione (){
	uint16_t c;
	int i, j;
	coloraCasella(posizione[0],posizione[1], White); //cancello il robot
	for(i=0; i<13; i++){ //ciclo per le righe
		for (j = 0; j<15; j++){ //ciclo per le colonne
			if(labirinto[i][j] == 2) c = Green;
			else if(labirinto[i][j] == 1) c = Purple;
			else continue;
			coloraCasella(i, j, c);
		}
	}
	disegnaRobot();
}
//*************************************************
// inizializzazione delle condizioni iniziali, start del gioco
//*************************************************
void posizionaRobot()
{
	posizione[0] = 7;
	posizione[1] = 7;
	direzione = 2; 
	
	disegnaRobot();
	mode = 0;
	mostraOstacoli();
}

//*************************************************
// disegno la schermata iniziale
//*************************************************
void preparaGioco()
{
	GUI_Text(60, 30, (uint8_t *) "Blind Labyrinth", Yellow, Purple);
	coloraRettangolo(15, 69, 225, 251, White); //spazio per giocare
	coloraRettangolo(15, 270, 105, 305, White); // pulsante del restart
	GUI_Text(32, 281, (uint8_t *) "Restart", Purple, White);
	coloraRettangolo(135, 270, 225, 305, Yellow); // pulsante del clear
	GUI_Text(160, 281, (uint8_t *) "Clear", Purple, Yellow);
	
	GUI_Text(65, 150,"Touch to start", Purple, White);
	GUI_Text(65, 162,"  a new game", Purple, White);
}

//*************************************************
// aggiorno la direzione del robot
//*************************************************
void ruotaRobot()
{
	coloraCasella(posizione[0], posizione[1], White);
	disegnaRobot();
}

//*********************************************************************************
// prossimaCasella() controlla se la prossima casella è occupabile o meno
//*********************************************************************************
uint8_t prossimaCasella()
{
		int i;
		if (direzione == 3 || direzione == 0){  // se guarda a NORD o a OVEST
			i = -1;
		} else{
			i = 1;
		}
		
		if (direzione == 3 || direzione == 1){ //asse orizzontale
			if((posizione[0]+i < 13 && posizione[0]+i >= 0) && labirinto[posizione[0]+i][posizione [1]]!= 1) {
				return 0; //casella occupabile
			}
		}
		if (direzione == 2 || direzione == 0){ //asse verticale
			if((posizione [1]+i < 15 && posizione [1]+i >= 0) && labirinto[posizione[0]][posizione [1]+i]!= 1) {
				return 0;
			}
		}
		return 1;
}
//*************************************************
// muovo in avanti il robot
//*************************************************
void muovi()
{
	int i;
	uint8_t occupata = prossimaCasella();
	
	if (occupata == 0){ //mi dice se posso spostarmi o no
		if (direzione == 3 || direzione == 0){  // se guarda a NORD o a OVEST
			i = -1;
		} else{
			i = 1;
		}
		
		coloraCasella(posizione[0], posizione[1], Pink); 	//cancella la precedente posizione
		
		if (direzione == 3 || direzione == 1){
			posizione[0] = posizione[0]+i;
		}
		
		if (direzione == 2 || direzione == 0){ 
			posizione[1] = posizione[1]+i;
		}
	}
	if(occupata == 1){
		coloraCasella(posizione[0], posizione[1], White); 	//cancella la precedente posizione
	}
	disegnaRobot();		
	
	if(labirinto[posizione[0]][posizione[1]] == 2){ //condizione di vittoria
		vittoria = 1;
		GUI_Text(80, 47, (uint8_t *) " YOU WON! ", Purple, Yellow);
		soluzione();
		GUI_Text(32, 281, (uint8_t *) "Restart", Magenta, White);
	}
}
//*************************************************
// mostro gli ostacoli
//*************************************************
void mostraOstacoli()
{
	int distanza = 0;
  int i = 0;
  int row = posizione[0];
  int col = posizione[1];
    
  if (direzione == 3 || direzione == 0){  // se guarda a NORD o a OVEST
    i = -1;
  } else{
    i = 1;
  }
  
  if (direzione == 3 || direzione == 1){ //asse verticale
    while( (row+i < 13 && row+i >= 0) && labirinto[row+i][col]!= 1) {
      row = row + i;
      distanza ++;
    }
		row = row+i;
  }
  
  if (direzione == 2 || direzione == 0){ //asse orizzontale
    while( (col+i < 15 && col+i >= 0) && labirinto[row][col+i]!= 1) {
      col = col + i;
      distanza ++;
    }
		col = col+i;
  }
	
  if (distanza <= 5 && (row>=0 && row<13) && (col>=0 && col<15)){
		coloraCasella(row, col, Purple);
	}
}

//*************************************************
// cancell area di gioco tranne il robot
//*************************************************
void clear()
{
	uint8_t col1, col2, row1, row2;
	if(posizione[0] != 0){
		row1 = pixel(0, 'x');
		col1 = pixel(0, 'y');
		row2 = pixel( posizione[0], 'x');
		col2 = pixel( 15, 'y');
		coloraRettangolo(col1, row1, col2, row2, White);
	}
	if(posizione[1] != 0){
	row1 = pixel(posizione[0], 'x');
	col1 = pixel(0, 'y');
	row2 = pixel(posizione[0]+1, 'x');
	col2 = pixel(posizione[1], 'y');
		coloraRettangolo(col1, row1, col2, row2, White);
	}
	
	if(posizione[1] != 14){
		row1 = pixel(posizione[0], 'x');
		col1 = pixel(posizione[1]+1, 'y');
		row2 = pixel(posizione[0]+1, 'x');
		col2 = pixel(15, 'y');
		coloraRettangolo(col1, row1, col2, row2, White);
	}
	
	if(posizione[0] != 12){
		row1 = pixel(posizione[0]+1, 'x');
		col1 = pixel(0, 'y');
		row2 = pixel(13, 'x');
		col2 = pixel(15, 'y');
		coloraRettangolo(col1, row1, col2, row2, White);
	}
}
#endif
