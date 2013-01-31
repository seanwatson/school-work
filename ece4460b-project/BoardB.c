/*
 * ECE 4460 Project
 *
 * File:	BoardB.c
 * Author:	Sean Watson
 *
 * Implements a PID controller that controls the position of
 * a ball in a cylinder
 *
 * This board's responsibilities:
 *	-	read the desired position from the serial port
 *	-	read the actual position and calculate the error
 *	-	generate a PWM signal based on the error
 *	-	allow adjustment of K values using push buttons
 *	-	transmit the actual position, error, and K values to the other board via serial
 *
 * Connections:
 *	-	serial lines to other board
 *	-	push buttons from demo board to digital ins
 *	-	ball levitation aparatus to analog in and outs
 *
 */

/* Definitions */
#define POTCHAN 	0	// channel 0 of ADC (ADC0)
#define MOTCHAN 	1	// channel 1 of digital output (OUT1)

#define IRCHAN 		1		//position sensor input channel
#define IRMIN		0.07
#define IR25		0.246
#define IR50		0.52
#define IR75		1.052
#define IRMAX		2.51

#define KP_SW		3		//digital input channel for kp button
#define KI_SW		4		//digital input channel for ki button
#define KD_SW		5		//digital input channel for kd button
#define KSTEP		10		//size of step when changing k values
#define SAMPLETIME 25

#define HOVER 0x300

/* Variable declarations */

char TMRB_MSB;	// this 8-bit value will be written to TBM2R (bits 8 and 9 of TMRB2 match register)
char TMRB_LSB;	// this 8-bit value will be written to TBL2R (bits 0 to 7 of TMRB2 match register)
char PWM_flag;	// The backround process will continusly write this value to the motor.

float potVol, irVol, difference;	//desired position, actual position, error variables
float kp, ki, kd;			//PID constant variables
float int_sum;				//integral sum
float last_err;				//last error signal value
float pid;					//error signal
float packet[5];			//packet for serial transmission
float newDesired;			//new desired pot vol
int num;				//PWM signal variable

/* function prototype */
void Tmr_B_isr();			//pwm interrupts
void EXT_isr();				//external interrupt prototype
void scaleIRVol();		//prototype for scaling ir values

main(){

	/* Initializations */
	brdInit();

	potVol = 0;
	kp = 0.3;
	ki = 0;
	kd = 0;
	difference = 0;
	last_err = 0;
	int_sum = 0;
	PWM_flag =0;
	newDesired = 0;
	num = 0;

	anaInCalib(IRCHAN,0,10,0xFFF,-10);
	anaInEEWr(IRCHAN);

	/* Initialize push buttons */
	WrPortI(PEDDR, &PEDDRShadow,0x00); // set port E as all inputs
	WrPortI(I0CR,&I0CRShadow,0x13); // enable external INT0 on PE4, falling edge
	SetVectExtern2000(1,&EXT_isr); // set EXT_isr() to be the ISR for the external interrupt, having priority 1

	
	TMRB_MSB = 0x00;// Initialize TMRB2 match register to coincide with 50% duty cycle
	TMRB_LSB = 0x00;// Initialize TMRB2 match register to coincide with 50% duty cycle


	/* Setup Timer A */
	WrPortI(TAT1R, &TAT1RShadow, 0xFF);	// set TMRA1 to count down from 255
	WrPortI(TACR, &TACRShadow, 0x00);	// Disable TMRA interrupts (TMRA used only to clock TMRB)
	WrPortI(TACSR, &TACSRShadow, 0x01); 	// Enable main clock (PCLK/2) for TMRA1

	/* set up timer B interrupt for generating PWM */
	SetVectIntern(0x0b, Tmr_B_isr); 	// set up timer B interrupt vector
	WrPortI(TBCR, &TBCRShadow, 0x06); 	// clock timer B with TMRA1 to priority 2 interrupt
	WrPortI(TBM1R, NULL, 0x00); 		// set up match register 1 to 0x00
	WrPortI(TBL1R, NULL, 0x00);		// set up match register 1 to 0x00
	WrPortI(TBM2R, NULL, TMRB_MSB); 	// set up match register 2 to to give an initial 50% duty cycle
	WrPortI(TBL2R, NULL, TMRB_LSB);	// set up match register 2 to to give an initial 50% duty cycle
	WrPortI(TBCSR, &TBCSRShadow, 0x07);	// enable Timer B to interrupt on B1 and B2 match

	/* Initialize serial port */	
	serBopen(19200); 			// Open serial port B and set baud rate to 19200
	serBwrFlush(); 				// Flush transmit data buffer
	serBrdFlush();				// Flush receive data buffer
	serMode(0);				// Define serial communication hard-link as 3-wire RS-232 (no flow control lines)

  	/* Backround process */
	while(1){

		costate{
			digOut(MOTCHAN, PWM_flag);
			}

		//Get new desired position
		costate{
			// read analog voltage
			wfd newDesired = cof_serBgetc();	// Wait for a char to be received, then get it
			newDesired = (float) newDesired;
			if(newDesired >=0 ){
				potVol = newDesired / 255;
			}
		}

		//Calculate error and adjust
		costate{
			waitfor(DelayMs(SAMPLETIME));
		//Get actual position
			irVol=anaInVolts(IRCHAN);
			scaleIRVol();
			
			//Calculate error
			last_err = difference;
			difference = (potVol - irVol);
		
			int_sum += ((difference + last_err)/2);
			pid = ((kp*difference) + (SAMPLETIME/1000)*ki*(int_sum) + kd*((difference - last_err)/(SAMPLETIME/1000)));

			num = (int)(HOVER + pid * 0x3FF);
			if (num > 0x3FF){
				num = 0x3FF;
			}
			else if (num < 0){
				num = 0;
			}

			TMRB_MSB = (char) (num >> 2);
			TMRB_LSB = (char) num;
		}
		
		//Send actual, error, and k values
		costate{
		
			waitfor(DelayMs(250));
			
			//Pack the data
			packet[0] = irVol;
			packet[1] = pid;
			packet[2] = kp;
			packet[3] = ki;
			packet[4] = kd;
			
			//Transmit the packet
			wfd cof_serBwrite(packet,5*sizeof(float));
		}
	
	}
}

/* Timer B interrupt service routine */
nodebug root interrupt void Tmr_B_isr(){

	char TMRB_status;

	TMRB_status = RdPortI(TBCSR);	// Read-out and clear interrupt status flags

	/* If interrupt triggered by TMRB1 match, a new cycle has begun; set PWM output flag high */
	if(TMRB_status & 0x02){
		PWM_flag = 1;		// Raise output flag to 1
		WrPortI(TBM1R, NULL, 0x00);	// set up match register(s) to interrupt at the begining of the next cycle
		WrPortI(TBL1R, NULL, 0);// set up match register(s) to interrupt at the begining of the next cycle
	}

	/* If interrupt triggered by TMRB match, the output will be low for the remainder of the cycle; set PWM flag low */
	if(TMRB_status & 0x04){
		PWM_flag = 0;			// Drop output flag to 0
		WrPortI(TBM2R, NULL, TMRB_MSB);	// set up match register(s) to corespond with duty cycle
		WrPortI(TBL2R, NULL, TMRB_LSB);	// set up match register(s) to corespond with duty cycle
	}
}

/* Adjusts K values */
nodebug root interrupt void EXT_isr(){

	char kpIn;
	char kiIn;
	char kdIn;

	kpIn = digIn(KP_SW);
	kiIn = digIn(KI_SW);
	kdIn = digIn(KD_SW);

	if (kpIn && kiIn && kdIn){
		kp = 0;
		ki = 0;
		kd = 0;
	}
	else if (kpIn && kiIn){
		kd -= KSTEP;
	}
	else if (kpIn && kdIn){
		ki -= KSTEP;
	}
	else if (kiIn && kdIn){
		kp -= KSTEP;
	}
	else if (kpIn){
		kp += KSTEP;
	}
	else if (kiIn){
		ki += KSTEP;
	}
	else if (kdIn){
		kd += KSTEP;
	}
}

/* Uses linear approximation to scale the IR reading to a percentage */
void scaleIRVol(){
	if (irVol < IR25){
		irVol = (IR25 - irVol)/(IR25 - IRMIN);
		if (irVol > 1){
			irVol = 1;
		}
		else if (irVol < 0){
			irVol = 0;
		}
		irVol = 1 - irVol;
		irVol *= 0.25;
	}
	else if (irVol < IR50){
		irVol = (IR50 - irVol)/(IR50 - IR25);
		if (irVol > 1){
			irVol = 1;
		}
		else if (irVol < 0){
			irVol = 0;
		}
		irVol = 1 - irVol;
		irVol *= 0.25;
		irVol += 0.25;
	}
	else if (irVol < IR75){
		irVol = (IR75 - irVol)/(IR75 - IR50);
		if (irVol > 1){
			irVol = 1;
		}
		else if (irVol < 0){
			irVol = 0;
		}
		irVol = 1 - irVol;
		irVol *= 0.25;
		irVol += 0.50;
	}
	else{
		irVol = (IRMAX - irVol)/(IRMAX - IR75);
		if (irVol > 1){
			irVol = 1;
		}
		else if (irVol < 0){
			irVol = 0;
		}
		irVol = 1 - irVol;
		irVol *= 0.25;
		irVol += 0.75;
	}
}

