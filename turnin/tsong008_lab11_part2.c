/*	Author: Tinghui Song
 *  Partner(s) Name: 
 *	Lab Section: 24
 *	Assignment: Lab # 11 Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo link: https://drive.google.com/file/d/16yroIV7wFaZyorf1Fm_2kNCTtsFGnL8r/view?usp=sharing
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "Timer.h"
#endif

unsigned char tempA;
unsigned char Ouput;
unsigned char cycle;
unsigned char power;

void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
         PORTC = 0x08;
         PORTC |= ((data >> i) & 0x01);
         PORTC |= 0x02;
    }
    PORTC |= 0x04;
    PORTC = 0x00;
}

enum L1_States{L1_Start, L1_1} L1_State;

void Light1_Tick(){
	switch(L1_State){
		case L1_Start:
			Ouput = 0xAA;
			L1_State = L1_1;
			break;
		case L1_1:
			Ouput = ~Ouput;
			L1_State = L1_1;
			break;
	}
}

enum L2_States{L2_Start, L2_1, L2_2} L2_State;

void Light2_Tick(){
	switch(L2_State){
		case L2_Start:
			Ouput = 0x80;
			L2_State = L2_1;
			break;
		case L2_1:
			if (Ouput > 0x01) {
				Ouput = Ouput >> 1;
				L2_State = L2_1;
			}
			else {
				L2_State = L2_2;
			}
			break;
		case L2_2:
			if (Ouput < 0x080) {
				Ouput = Ouput << 1;
				L2_State = L2_2;
			}
			else {
				L2_State = L2_1;
			}
			break;
	}
}

enum L3_States{L3_Start, L3_1} L3_State;

void Light3_Tick(){
	switch(L3_State){
		case L3_Start:
			Ouput = 0xCC;
			L3_State = L3_1;
			break;
		case L3_1:
			Ouput = ~Ouput;
			L3_State = L3_1;
			break;
	}
}

enum Button_States{Button_Start, Button_Wait} Button_State;

void Button_Tick(){
	switch(Button_State){
		case Button_Start:
			power = 0x00;
			cycle = 0x01;
			Button_State = Button_Wait;
			break;
		case Button_Wait:
			if (power == 0x00 && tempA == 0x03){
				power = 0x01;
				Button_State = Button_Wait;
			} 
			else if (power == 0x01 && tempA == 0x03) {
				power = 0x00;
				Ouput = 0x00;
				cycle = 0x01;
				Button_State = Button_Wait;
			} 
			else if (power == 0x01 && tempA == 0x01 && cycle < 0x03) {
				cycle++;
				Button_State = Button_Wait;
			} 
			else if (power == 0x01 && tempA == 0x02 && cycle == 0x01){
				Button_State = Button_Wait;
			} 
			else if (power == 0x01 && tempA == 0x02 && cycle > 0x01) {
				cycle--;
				Button_State = Button_Wait;
			} 
			else if (power == 0x01 && tempA == 0x01 && cycle == 0x03){
				Button_State = Button_Wait;
			} 
			else {
				Button_State = Button_Wait;
			}
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	/* Insert your solution below */
	TimerSet(200);
	Button_State = Button_Start;
	TimerOn();

    while (1) {
	tempA = ~PINA & 0x03;
	Button_Tick();
	if (cycle == 0x01 && power) {
		L2_State = L1_Start;
		L3_State = L3_Start;
		Light1_Tick();
	} 
	else if (cycle == 0x02 && power) {
		L1_State = L1_Start;
		L3_State = L3_Start;
		Light2_Tick();
	} 
	else if (cycle == 0x03 && power) {
		L1_State = L1_Start;
		L2_State = L1_Start;
		Light3_Tick();
	}
	transmit_data(Ouput);
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}

