/*	Author: Tinghui Song
 *  Partner(s) Name: 
 *	Lab Section: 24
 *	Assignment: Lab # 11 Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo link:
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "Timer.h"
#endif

unsigned char tempA;

unsigned char output_1;
unsigned char output_2;

const unsigned char R1 = 0x01;
const unsigned char R2 = 0x02;


unsigned char nextState_1;
unsigned char nextState_2;

unsigned char power_1;
unsigned char power_2;

void transmit_data(unsigned char data, unsigned char reg) {
    int i;
    if (reg == R1){
    	for (i = 0; i < 8 ; ++i) {
         	PORTC = 0x08;
         	PORTC |= ((data >> i) & 0x01);
         	PORTC |= 0x02;
    	}
    	PORTC |= 0x04;
    	PORTC = 0x00;
    } else if (reg == R2) {
	for (i = 0; i < 8 ; ++i) {
         	PORTC = 0x20;
         	PORTC |= ((data >> i) & 0x01);
        	PORTC |= 0x02;
    	}
    	PORTC |= 0x10;
    	PORTC = 0x00;
    }
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

enum L4_States{L4_Start, L4_1} L4_State;

void Light4_LED(){
        switch(L4_State){
                case L4_Start:
                        output_2 = 0xAA;
                        L4_State = L4_1;
                        break;
                case L4_1:
                        output_2 = ~output_2;
                        L4_State = L4_1;
                        break;
        }
}

enum L5_States{L5_Start, L5_1} L5_State;

void Light5_Tick(){
	switch(L5_State){
		case L5_Start:
			output_1 = 0xFF;
			L5_State = L5_1;
			break;

		case L5_1:
			output_1 = ~output_1;
			L5_State = L5_1;
			break;
	}
}

enum Light6_States{L6_Start, L6_1} Light6_State;

void Light6_Tick(){
        switch(Light6_State){

		case L6_Start:
			output_2 = 0xCC;
			Light6_State = L6_1;
			break;

		case L6_1:
			output_2 = ~output_2;
			Light6_State = L6_1;
			break;

        }
}


enum Button_States{Button_Start, Button_Wait} Button_State;

void Button_Tick(){
	switch(Button_State){
		case Button_Start:
			power_1 = 0x00;
			power_2 = 0x00;
			nextState_1 = 0x01;
			nextState_2 = 0x01;
			Button_State = Button_Wait;
			break;
		case Button_Wait:
			if (power_1 == 0x00 && tempA == 0x03){
				power_1 = 0x01;
				Button_State = Button_Wait;
			} 
			else if (power_1 == 0x01 && tempA == 0x03) {
				power_1 = 0x00;
				output_1 = 0x00;
				nextState_1 = 0x01;
				Button_State = Button_Wait;
			} 
			else if (power_1 == 0x01 && tempA == 0x01 && nextState_1 < 0x03) {
				nextState_1++;
				Button_State = Button_Wait;
			} 
			else if (power_1 == 0x01 && tempA == 0x02 && nextState_1 > 0x01) {
				nextState_1--;
				Button_State = Button_Wait;
			} 
			else if (power_1 == 0x01 && tempA == 0x02 && nextState_1 == 0x01){
				Button_State = Button_Wait;
			} 
			else if (power_1 == 0x01 && tempA == 0x01 && nextState_1 == 0x03){
				Button_State = Button_Wait;
			} 
			else {
				Button_State = Button_Wait;
			}

			if (power_2 == 0x00 && tempA == 0x30) {
				power_2 = 0x01;
				Button_State = Button_Wait;
			} 
			else if (power_2 == 0x01 && tempA == 0x30) {
				power_2 = 0x00;
				output_2 = 0x00;
				nextState_2 = 0x01;
				Button_State = Button_Wait;
			} 
			else if (power_2 == 0x01 && tempA == 0x10 && nextState_2 < 0x03) {
				nextState_2++;
				Button_State = Button_Wait;
			} 
			else if (power_2 == 0x01 && tempA == 0x20 && nextState_2 > 0x01) {
				nextState_2--;
				Button_State = Button_Wait;
			} 
			else if (power_2 == 0x01 && tempA == 0x20 && nextState_2 == 0x01) {
				Button_State = Button_Wait;
			} 
			else if (power_2 == 0x01 && tempA == 0x10 && nextState_2 == 0x03) {
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
	DDRA = 0x00; 	PORTA = 0xFF;
	DDRC = 0xFF; 	PORTC = 0x00;
	
    /* Insert your solution below */
	TimerSet(300);
	Button_State = Button_Start;
	TimerOn();

    while (1) {
	tempA = ~PINA & 0x33;
	Button_Tick();
	if (nextState_1 == 0x01 && power_1) {
		L2_State = L2_Start;
		L5_State = L5_Start;
		Light1_Tick();
	} 
	else if (nextState_1 == 0x02 && power_1) {
		L1_State = L1_Start;
		L5_State = L5_Start;
		Light2_Tick();
	} 
	else if (nextState_1 == 0x03 && power_1) {
		L2_State = L2_Start;
		L1_State = L1_Start;
		Light5_Tick();
	}
	if (nextState_2 == 0x01 && power_2) {
		L4_State = L4_Start;
		Light6_State = L6_Start;
		Light3_LED();
	} 
	else if (nextState_2 == 0x02 && power_2) {
		L3_State = L3_Start;
		Light6_State = L6_Start;
		Light4_LED();
	} 
	else if (nextState_2 == 0x03 && power_2) {
		L4_State = L4_Start;
		L3_State = L3_Start;
		Light6_Tick();
	}
	transmit_data(output_1, R1);
	transmit_data(output_2, R2);
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}

