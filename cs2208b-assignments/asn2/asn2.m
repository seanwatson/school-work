! ***********************************************************************************************************
! Sean Watson
! Assignment #2
! File: asn2.m
!
! Description:
!	This file contains a program which prompts the user for an
!	amount of time in hh:mm format and an hourly wage in $d.cc
!	format. The program then calculates and displays the amount
!	of money earned for the amount of time worked.
!
! Register Legend:
!
!	%mins_worked_r	The amount of time worked in minutes		%l0
!	%cents_hr_r	The amount of money earned per hour in cents 	%l1
!	%money_r	The amount of money earned before formatting	%l2
!	%dollars_r	The amount of whole dollars earned		%l3
!	%conv_factor	For converting cents/hr X mins to dollars	%l4
!
! Executable Instructions: 74
!
!************************************************************************************************************

!************************************************************************************************************
! Constants
!************************************************************************************************************

define(EOL, 10)				! constant for the end of line character
define(TIME_PROMPT, '>')		! constant used to prompt the user for the amount of time worked
define(DOLLAR_PROMPT, '$')		! constant used to prompt the user for their hourly wage
define(DECIMAL_POINT, '.')		! constant used for displaying the decimal point in the output

define(MINS_PER_HR, 60)			! represents the number of minutes in one hour, used for conversion
define(CENTS_PER_DOLLAR, 100)		! represents the number of cents in one dollar, used for conversion
define(MINS_HR_X_TEN, 600)		! the number of minutes per hour times 10, used for formatting

!************************************************************************************************************
! Registers
!************************************************************************************************************

define(mins_worked_r, l0)
define(cents_hr_r, l1)
define(money_r, l2)
define(dollars_r, l3)
define(conv_factor, l4)

!************************************************************************************************************
! Main Program Begins
!************************************************************************************************************

	.global main

main: 	save	%sp, -96, %sp				! main program begins

	mov	600, %o0				! initialize a register with cents/dollar * mins/hr
	call	.mul					! 	which is 100 * 60 = 6000
	mov	10, %o1					! 	this will be needed in formatting the output
	mov	%o0, %conv_factor			! store the value so it can be used later

	call	writeChar				! prompt the user to enter the time worked
	mov	TIME_PROMPT, %o0			! [DELAY SLOT]

	call	readInt					! read the amount of hours worked
	nop

	call	.mul					! convert the hours worked into minutes
	mov	MINS_PER_HR, %o1			! [DELAY SLOT]
	mov	%o0,%mins_worked_r			! store the amount of minutes worked so far

	call	readChar				! remove the colon in hh:mm from the keyboard buffer
	nop

	call	readInt					! read the amount of minutes worked
	nop

	add	%mins_worked_r, %o0, %mins_worked_r	! add the amount of minutes worked to the total
							! %mins_worked_r now stores the total amount of time
							!	worked in minutes
	
	call 	writeChar				! prompt the user to enter their hourly wage
	mov	DOLLAR_PROMPT, %o0			! [DELAY SLOT]

	call	readInt					! read the amount of whole dollars earned per hr
	nop
	
	call	.mul					! convert the number of whole dollars to cents
	mov	CENTS_PER_DOLLAR, %o1			! [DELAY SLOT]
	mov	%o0, %cents_hr_r			! store the amount of cents per hr so far

	call	readChar				! remove the decimal point in $d.cc from the buffer
	nop

	call	readInt					! read the amount of cents earned per hr
	nop
	
	add	%cents_hr_r, %o0, %o0			! add the amount of cents to the total earned per hr
							! %cents_hr_r now holds the total amount of cents
							!	earned per hr
							
							! multiply the amount of cents earned per hr times
	call	.mul					! the amount of minutes worked to get the total wage
	mov	%mins_worked_r, %o1			! [DELAY SLOT]

	mov	%o0, %money_r				! store the total wage in cents x mins / hr for later use

	call	.div					! extract the whole dollar amount from the raw wage
	mov	%conv_factor, %o1			! [DELAY SLOT]
	mov	%o0, %dollars_r

	call	writeChar				! display a $ for the total wage
	mov	DOLLAR_PROMPT, %o0			! [DELAY SLOT]

	mov	%dollars_r, %o0				! display the amount of whole dollars earned
	call	writeInt
	nop

	call	writeChar				! display the decimal point
	mov	DECIMAL_POINT, %o0			! [DELAY SLOT]

	mov	%money_r, %o0				! find the remainder of the wage earned
	call	.rem
	mov	%conv_factor, %o1			! [DELAY SLOT]

	call	.div					! convert the value into 10s of cents
	mov	MINS_HR_X_TEN, %o1			! [DELAY SLOT]

	call	writeInt				! display the 10s of cents earned
	nop

	mov	%money_r, %o0				! find the remainder of the wage earned
	call	.rem
	mov	MINS_HR_X_TEN, %o1			! [DELAY SLOT]

	call	.div					! convert the value into cents
	mov	MINS_PER_HR, %o1			! [DELAY SLOT]

	call	writeInt				! display the cents earned
	nop
	
	call	writeChar				! newline at the end of program
	mov	EOL, %o0				! [DELAY SLOT]

	ret						! terminate the program
	restore
