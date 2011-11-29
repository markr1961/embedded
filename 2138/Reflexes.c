/******************************************************************************
 *
 * Copyright:
 *    (C) 2011
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
#include "uart.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/
#define LEFT_MOVE 1
#define RIGHT_MOVE 0
#define UP_MOVE 2
#define DOWN_MOVE 3

/*****************************************************************************
 * Local variables
 ****************************************************************************/
static tU32 systemTimeMs;
static tU32 a;
static char buf[20];
static char result[10];
static tU32 gameTime=0;


/*****************************************************************************
 * External variables
 ****************************************************************************/


/*****************************************************************************
 *
 * Description:
 *    Method resets and initialized Timer, the value is prescaled for obtaing
 *    the time in miliseconds.
 *
 ****************************************************************************/
static void timer(void){
	T1TCR=0x02;
	T1MCR=0x0;
	T1PR=CORE_FREQ/PBSD/1000-1;
	T1TCR=0x01;
}

/*****************************************************************************
 *
 * Description:
 *    Method resets and initialized Timer, the value is prescaled for obtaing
 *    the time in miliseconds.
 *
 ****************************************************************************/
static tU32 getTime(void){
	systemTimeMs=T1TC;
	return systemTimeMs;
}

/*****************************************************************************
 *
 * Description:
 *    Very "complex" methods used to randomize the number of arrow to send
 *    on display.
 *
 ****************************************************************************/
static tU32 rand(void){
	a=T0TC%4;
	return a;
}

/*****************************************************************************
 *
 * Description:
 *    Method which converts the time from tU32 value into char, use in order
 *    to display the user result on screen.
 *
 ****************************************************************************/
static char* showResults(){
	sprintf(result,"%d",gameTime/5);
	return result;

}
/*****************************************************************************
 *
 * Description:
 *    Main and the most important method for handling the reflex application.
 *
 ****************************************************************************/
void initApp(void){

	/*
	 * Commented code below presents implemented functionalities up to midterm
	 */
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

	tU8 i;
	gameTime=0;
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
			gameTime=gameTime+getTime();
			//lcdPuts(showResults());
			//getrand();
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
	lcdGotoxy(20,1);
	lcdColor(0x00,0xe0);
	lcdPuts("Your Score:");
	lcdGotoxy(48,20);
	lcdPuts(showResults());
	lcdGotoxy(80,20);
	lcdPuts("ms");

	osSleep(500);

}
/*****************************************************************************
 *
 * Description:
 *    Method showing the author footer
 *
 ****************************************************************************/
void authorsFooter(void){
	lcdClrscr();

	tU8 pressKey;

	for(;;){
		pressKey = checkKey();
		lcdGotoxy(2,1);
		lcdColor(0x00,0xe0);
		lcdPuts("Maciej Wilkowski");

		lcdGotoxy(2,20);
		lcdPuts("Szymon Rachanski");

		lcdGotoxy(2,40);
		lcdPuts("Tomasz Debski");

		lcdGotoxy(2,60);
		lcdPuts("Zbyszko Natkanski");

		if(pressKey == KEY_CENTER) break;
	}
}

/*****************************************************************************
 *
 * Description:
 *    Method displays application instruction for user
 *
 ****************************************************************************/
void instructionsInfo(void){
	lcdClrscr();
	tU8 pressKey;

	for(;;){
		pressKey = checkKey();

		lcdGotoxy(5,5);
		lcdColor(0x00,0xe0);
		lcdPuts("Instructions:");
		lcdGotoxy(5, 25);
		lcdPuts("Press proper");
		lcdGotoxy(5, 45);
		lcdPuts("joy direction");
		lcdGotoxy(5, 65);
		lcdPuts("when you see");
		lcdGotoxy(5, 85);
		lcdPuts("arrow on screen.");




		if(pressKey == KEY_CENTER) break;
	}
}

/*****************************************************************************
 *
 * Description:
 *    Method tries to send the message via uart. (unfinished functionality)
 *
 ****************************************************************************/
void highScores(void){
	lcdClrscr();

	tU8 pressKey;

	for(;;){
		pressKey = checkKey();

		lcdPuts("Sending...");
		//Here, should be sent a data with highest score...
		//uart1SendChars("Hello World!\r\n", 12);
		uart1SendChar(showResults());

		if(pressKey == KEY_CENTER) break;
	}
}
