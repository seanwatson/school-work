/*
 * ECE 4460 Project
 * 
 * File:	BoardA.c
 * Author:	Sean Watson
 *
 * Implements a PID controller that controls the position of
 * a ball in a cylinder
 *
 * This board's responsibilities:
 *	-	read the desired position from the potentiometer
 *	-	transmit the desired position via serial
 *	- 	read the actual position, error, and K values from serial
 *	-	read a display mode from the keyboard
 *	-	display the values using the selected display mode
 *
 * Connections:
 *	-	serial lines to other board
 *	-	potentiometer from demo board to analog in
 *	-	development machine
 */

#define STEP_INPUT	0.5		//step input (percentage of height)
#define SAMPLETIME	250	//input sampling time
#define POTCHAN 	0		//potentiometer input channel
#define POTMAX 		9.286937	//maximum pot voltage
#define POTMIN		0.012211	//minimum pot voltage

//Screen positions for various values to be printed
#define ERRORX 		0
#define ERRORY 		10
#define ACTUALX 	0
#define ACTUALY 	0
#define DESIREDX 	0
#define DESIREDY	5
#define KPX 		0
#define KPY 		15
#define KIX 		0
#define KIY 		20
#define KDX 		0
#define KDY 		25
#define SCREEN_REFRESH 1000	//screen refresh rate

void EXT_isr();				//external interrupt prototype
float scale(float,float,float);		//prototype for scaling values between 0 and 1
void DispStr(int,int,char*);		//prototype for printing a string to a location
void displayBasic();			//display all current values
void displayGraphic();			//display a graphic showing position and values
void displayDetailed();			//display current and past values

float potVol;	//desired position
float irVol, pid;		//actual position and error values
float kp, ki, kd;		//PID constant variables
float packet[5];		//packet for recieving values
int num;				//PID signal variable
char displayIn;			//display type read from keyboard
char inMode;			//system input mode (step or pot)
char display[256];		//string for printing outputs

main(){
	/* Initialize board and variables */
	brdInit();				// Required for BL2000 series boards
	kp = 0;
	ki = 0;
	kd = 0;
	irVol = 0;
	pid = 0;
	displayIn = '1';
	
	/* Calibrate ADC to give avtual input voltage */
	anaInCalib(POTCHAN,0,10,0xFFF,-10);
	anaInEEWr(POTCHAN);
	
	/* Initialize serial port */	
	serBopen(19200); 			// Open serial port B and set baud rate to 19200
	serBwrFlush(); 				// Flush transmit data buffer
	serBrdFlush();				// Flush receive data buffer
	serMode(0);				// Define serial communication hard-link as 3-wire RS-232 (no flow control lines)

	/* Let the user pick an input mode */
	printf("Select input mode 1) Step, 2) Potentiometer(default): ");
	inMode = getchar();

	while(1){
		//Get new desired position costate
		costate{
			waitfor(DelayMs(SAMPLETIME));

			//Give step input or potentiometer input
			if(inMode == '1'){
				potVol = STEP_INPUT;	//Set input to the desired step
			}
			else{
				//Get desired position
				potVol=anaInVolts(POTCHAN);
				//printf("%f\n",potVol);
				potVol = scale(potVol, POTMAX, POTMIN);
			}

			//Pack the input
			num = (int)(potVol * 255);
			
			//Transmit new speed
			wfd cof_serBputc((char) num);
		}
		
		//Display costate
		costate{
			waitfor(DelayMs(SCREEN_REFRESH));
			
			if(displayIn == '3')
				displayDetailed();
			else if (displayIn == '2')
				displayGraphic();
			else
				displayBasic();
		}
		
		//Display type costate
		costate{
			waitfor(kbhit());
			displayIn = getchar();
		}
		
		//Get actual, error, and k values
		costate{
			
			//Get values
			wfd cof_serBread(packet,5*sizeof(float),20);
			
			//Extract the packet
			irVol = packet[0];
			pid = packet[1];
			kp = packet[2];
			ki = packet[3];
			kd = packet[4];
		}
	}
}
/* Displays the current value of all variables */
void displayBasic(){
	printf("\x1bt");
	//display actual position
	display[0] = '\0';
	sprintf(display, "Actual Position: %f", irVol);
	DispStr(ACTUALX,ACTUALY,display);
	
	//display desired position
	display[0] = '\0';
	sprintf(display, "Desired Position: %f", potVol);
	DispStr(DESIREDX,DESIREDY,display);
	
	//display error signal
	display[0] = '\0';
	sprintf(display, "Error Signal: %f", pid);
	DispStr(ERRORX,ERRORY,display);

	//display k values
	display[0] = '\0';
	sprintf(display, "Kp: %f", kp);
	DispStr(KPX,KPY,display);

	display[0] = '\0';
	sprintf(display, "Ki: %f", ki);
	DispStr(KIX,KIY,display);
	
	display[0] = '\0';
	sprintf(display, "Kd: %f", kd);
	DispStr(KDX,KDY,display);
}
/* Displays the balls position as a graphic */
void displayGraphic(){
	int i;
	printf("\x1bt");
	//Display sides of the container
	
	for (i = 1; i<40;i++){
		DispStr(20, 3 + i, "|                 |");
	}

	//Display top and bottom of container
	DispStr(21, 3, "-----------------");
	DispStr(21, 43, "-----------------");
	
	//Display ball pointer
	display[0] = '\0';
	sprintf(display,  "<--Ball %f Percent", irVol * 100);			
	DispStr(40, 43 - (40 * irVol), display);
}
/* Displays values in a table for easy plotting */
void displayDetailed(){
	printf("T: %u ", MS_TIMER);
	printf("Act: %.3f ", irVol);
	printf("Des: %.3f ", potVol);
	printf("Err: %.3f ", potVol - irVol);
	printf("PID: %.3f ", pid);
	printf("Kp: %.1f ", kp);
	printf("Ki: %.1f ", ki);
	printf("Kd: %.1f\n", kd);

}
/* Scales a value to a percentage */
float scale(float n, float max, float min){
	float pos;
	pos = (n - min)/(max - min);
	if (pos > 1){
		pos = 1;
	}
	else if(pos < 0){
		pos = 0;
	}
	return pos;
}
/* set the STDIO cursor location and display a string  */
void DispStr(int x, int y, char *s)
{
   x += 0x20;
   y += 0x20;
   printf ("\x1B=%c%c%s", x, y, s);
}
