/*	Author: Tinghui Song
 *  Partner(s) Name: 
 *	Lab Section: 24
 *	Assignment: Lab # 11 Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo link:
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "Timer.h"

unsigned short tempC = 0x02FF;
unsigned char count = 0;

enum States {Start, Wait, Inc, Dec, Reset} State;

void transmit_data(unsigned short data){
	int i;
	for(i = 0; i < 16; ++i){
		PORTC = 0x18;
		PORTC |= ((data >>i) & 0x01);
		PORTC |= 0x02;
	}
	PORTC |= 0x04;
	PORTC = 0x00;
}

void Tick(){
    unsigned char tempA = ~PINA;

    switch(State){ //transitions
		case Start:
			PORTB  = tempC;
		    	State = Wait;
		    	break;
		case Wait:
		    	if((tempA & 0x03) == 0x00){
		      	  State = Wait;
		    	}else if((tempA & 0x03) == 0x01){
		    	    State = Inc;
		    	    if(tempC < 0xFFFF){
		    	        tempC++;
				}
		   	 }
         		else if((tempA & 0x03) == 0x02){
		    	    State = Dec;
		   	     if(tempC > 0){
		    	        tempC--;
		    	    }

		  	 }
         		 else{
		   	     	State = Reset;
				tempC = 0;
		   	 }
		    	 break;
		case Dec:
		    	if((tempA & 0x03) == 0x00){
		     	   State = Wait;
		   	 }else if((tempA & 0x03) == 0x01){
		    	    State = Inc;
			    if(tempC < 0xFFFF){
		    	        tempC++;
		   	    }
		  	  }
         		else if((tempA & 0x03) == 0x02){
		  	         State = Dec;
				 if(count == 10){
					if(tempC > 0){
						    tempC--;
					}
				}
		 	  }
       			 else{
		  	      State = Reset;
		  	      tempC = 0;
		 	 }
		 	 break;
		case Inc:
			if((tempA & 0x03) == 0x00){
		   	     	State = Wait;
			  }
		 	  else if((tempA & 0x03) == 0x01){
		 	      	 State = Inc;
				 if(count == 10){
					if(tempC < 0xFFFF){
						tempC++;
					}
				}
		 	  }
        		else if((tempA & 0x03) == 0x02){
		   	    	 State = Dec;
				 if(tempC > 0){
			    	        tempC--;
		    	  	}
		  	 }
        		else{
		    	   State = Reset;
		    	   tempC = 0;
		   	}
			break;
        	case Reset:
		  	  if((tempA & 0x03) == 0x00){
		   	     State = Wait;
		   	 }
        		else if((tempA & 0x03) == 0x01){
		   	     State = Inc;
		   	     if(tempC < 9){
		   	         tempC++;
		   	     }
		   	 }
        		 else if((tempA & 0x03) == 0x02){
		   	    State = Dec;
		    	    if(tempC > 0)
		    	    {
		    	        tempC--;
		    	    }
		   	 }
       			  else{
		   	     State  = Reset;
		   	     tempC = 0;
		  	 }break;
       		 default:
		   	 State = Start;
		   	 break;
    }
	    switch(State){
		    break;
		case Wait:
		    break;
		case Inc:
		    break;
		case Dec:
		    break;
		case Reset:
		    break;
		default:
		    break;
	    }
}

int main(void) {

	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;


  TimerSet(50);
	TimerOn();
	State = Start;


	while (1) {
		while(!TimerFlag);
		TimerFlag = 0;
		Tick();
		transmit_data(tempC);
    	}
    	return 1;
}


