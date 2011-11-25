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

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/


/*****************************************************************************
 * Local variables
 ****************************************************************************/


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
	
	tU8 i,j;
	for (i=0 ; i<6 ; i++){
		timer();
		tU8 random=rand();
		drawArrow(random);
		osSleep(10);
		read();
	}
}

void authorsFooter(void){

}

void instructionsInfo(void){

}
