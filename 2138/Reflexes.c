/******************************************************************************
 *
 * Copyright:
 *    (C) 2011 Zbyszko Natkañski
 *
 * File:
 *    Reflexes.c
 *
 * Description:
 *    Implementation of Reflexes application.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <ea_init.h>
#include "lcd.h"
#include "key.h"
#include "Arrow.h"
#include "hw.h"
#include "bt.h"
#include "uart.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


/*****************************************************************************
 * Local variables
 ****************************************************************************/
static tU32 systemTimeMs;
static tU32 a;
static char buf[20];


/*****************************************************************************
 * External variables
 ****************************************************************************/


/*****************************************************************************
 *
 * Description:
 *    Check if current place for figure is valid
 *
 ****************************************************************************/


static void timer(void){
	T1TCR=0x02;
	T1MCR=0x0;
	T1PR=CORE_FREQ/PBSD/1000-1;
	T1TCR=0x01;
}

static char* read(void){
	systemTimeMs=T1TC;
	sprintf(buf, "%d", systemTimeMs);
	return buf;
}

static tU8 rand(void){
	a=T0TC%4;
	return a;
}

static char* getrand(void){
	sprintf(buf, "%d", rand());
	return buf;
}

#define LEFT_MOVE 1
#define RIGHT_MOVE 0
#define UP_MOVE 2
#define DOWN_MOVE 3

void initApp(void){

	//   lcdColor(0,0);
	//   lcdClrscr();
	// 
	//   lcdRect(14, 0, 102, 128, 0x6d);
	//   lcdRect(15, 17, 100, 110, 0);
	// 
	//   lcdGotoxy(48,1);
	//   lcdColor(0x6d,0);
	//   lcdPuts("Reflexer");
	// 
	// for(;;){
	// 
	// 	tU8 pressKey;
	// 	pressKey = checkKey();
	//     if (pressKey != KEY_NOTHING)
	//     {
	// 	if(pressKey == KEY_LEFT) getLeftArrow();
	// 	else if(pressKey == KEY_UP) getUpArrow();
	// 	else if(pressKey == KEY_DOWN) getDownArrow();
	// 	else if(pressKey == KEY_RIGHT) getRightArrow();
	//     }
	// }
	tU32 gameTime=0;
	tU8 i,j;

	tU8 pressKey;


	for (i=0 ; i<6 ; i++){
		lcdClrscr();
		timer();
		tU8 random=rand();
		tU8 move;
		drawArrow(random);

		reading_loop:
		for ( ; ;){
			setLED(LED_RED,FALSE);
			setLED(LED_GREEN,FALSE);

			pressKey = checkKey();
			if(pressKey == KEY_LEFT) {
				move=LEFT_MOVE;
				break;
			}
			else if(pressKey == KEY_UP) {
				move=UP_MOVE;
				break;
			}
			else if(pressKey == KEY_DOWN) {
				move=DOWN_MOVE;
				break;
			}
			else if(pressKey == KEY_RIGHT) {
				move=RIGHT_MOVE;
				break;
			}
		}
		if(move==random){
			setLED(LED_GREEN,TRUE);
			gameTime=gameTime+read();
			getrand();
		}else{
			setBuzzer(TRUE);
			setLED(LED_RED,TRUE);

			osSleep(20);
			setBuzzer(FALSE);


			goto reading_loop;
		}
	}

	setLED(LED_GREEN,FALSE);

	lcdClrscr();
	lcdGotoxy(48,1);
	lcdColor(0x00,0xe0);
	lcdPuts(read());
	osSleep(1000);

}

void authorsFooter(void){

}

void instructionsInfo(void){

}
