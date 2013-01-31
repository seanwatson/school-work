CTL2	EQU $0122
CTL4	EQU $0124
CTL5	EQU $0125
ADLB	EQU $0133
PORTA   EQU $0000                       
PADDR   EQU $0002
PORTB	EQU $0001
PBDDR	EQU $0003
PORTM	EQU $0250
PMDDR	EQU $0252
PUCR	EQU $000C
LED1    EQU %01000000      ;Equates LED1 to PB6
LED2    EQU %10000000      ;Equates LED2 to PB7
LED3    EQU %00000001      ;Equates LED3 to PA0
LED4    EQU %00000010      ;Equates LED4 to PA1
LED5    EQU %00000100      ;Equates LED5 to PA2
LED6    EQU %00001000      ;Equates LED6 to PA3
LED7    EQU %00010000      ;Equates LED7 to PA4
LED8    EQU %00100000      ;Equates LED8 to PA5
LED9    EQU %01000000      ;Equates LED9 to PA6
LED10   EQU %10000000      ;Equates LED10 to PA7

	HC12
	ORG $0400		;Main origin
	LDS #$4000		;Initialize stack pointer
	JSR LCDI		;Initialize LCD display
	JSR	ACDI		;Initialize A to D converter
	BCLR PUCR,%00000010	;Disable pull ups for port B
BEGIN	PSHA			;Reserve byte on stack
	LDAA ADLB		;Get N from A/D converter
	PSHA			;Push N to stack
	JSR SCALE		;Scale N to get degrees celcius
	PULA			;Pull off unnecessary high byte
	PULA			;Pull off temperature
	PSHA               	;Push temperature on to the stack
        PSHB               	;Reserve a space on the stack
		PSHB		;Reserve another space
        JSR BCD            	;Go BCD subroutine
        JSR ASCII       	;Go ASCII subroutine
	JSR WRITE		;Write the temperature
	PULA			;Balance the stack with temperature and one reserved byte
	JSR TEST		;Find which LEDs to turn on
	PULA			;Pull the LEDs for port B
	STAA PORTB		;Turn on the port B LEDs
	PULA			;Pull the LEDs for port A
	STAA PORTA		;Turn on the LEDs for port A
	LDAA #!250
	PSHA
	JSR DELAY
	PULA			;Delay half a second and repeat
	BRA BEGIN		;Repeat
			
	ORG $0500
ACDI	LDAA #%11000000		
	STAA CTL2		;Turn on the converter
	NOP
	NOP			;Delay as it turns on
	LDAA #%11100101
	STAA CTL4		;Set up the CTL4 register
	LDAA #%10100001
	STAA CTL5		;Set up the CTL5 register
	RTS	
	
	ORG $0600
LCDI	LDAA #!16		;16ms delay
	PSHA
	JSR DELAY
	PULA
	LDAA #$FF
	STAA PMDDR		;Declare port M as outputs
	BCLR PORTM,%00011100	;Clear E,R/W,RS
	STAA PADDR		;Declare port A as outputs
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;Transfer function set command
	LDAA #!5
	PSHA
	JSR DELAY
	PULA			;Delay 5ms
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;Transfer function set command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA			;Delay 1ms
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;Transfer function set command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA			;Delay 1ms
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;One line display and font selection
	LDAA #!1
	PSHA
	JSR DELAY
	PULA			;Delay 1ms
	LDAA #$08
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;Display off command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA			;Delay 1ms
	LDAA #$01
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;Clear display
	LDAA #!1
	PSHA
	JSR DELAY
	PULA			;Delay 1ms
	LDAA #$06
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;Entry mode set
	LDAA #!1
	PSHA
	JSR DELAY
	PULA			;Delay 1ms
	LDAA #$0E
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000	;Display on command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA			;Delay 1ms
	RTS
	
	ORG $0700          ;Subroutine origin
DELAY   TSX                ;Copy SP to IX
DELAY1  LDY #!2000         ;Load loop counter (LC)
DELAY2  DEY                ;LC=LC-1
        BNE DELAY2         ;Repeat if not=0
        DEC $2,X           ;Decrement no. of ms delay
        BNE DELAY1         ;Repeat if not=0
        RTS      
		
	ORG $0800
SCALE	TSY		   ;Copy SP to index register
	LDAA $2,Y	   ;Load N into ACCA
	LDAB #!50	   ;Load 50 into ACCB
	MUL		   ;ACCA x ACCB -> ACCD
	LDX #!255	   ;Load X with 255
	IDIV		   ;D/X -> D
	STX 2,Y	           ;Store D in 2,Y and 3,Y
	RTS
		
        ORG $0900          ;Subroutine origin
BCD     TSX	           ;Copy stack pointer to index register
	CLRA		   ;Clear ACCA
	CLRB		   ;Clear ACCB
        LDAA 4,X           ;Load ACCA with N from the stack
	CMPA #$09          ;Compare ACCA with $09
        BLS PACK           ;Go to PACK if less than or equal to $09
CONT    INCB               ;Increment the Tens by 1
        SUBA #$0A          ;Subtract 10 from the units
        CMPA #$09	   ;Compare ACCA with $09
	BHI CONT	   ;Go to CONT if greater than $09
PACK 	ASLB
	ASLB
	ASLB
	ASLB		   ;Shift tens by 4
	ABA		   ;Form packed BCD result
	STAA 3,X	   ;Store packed BCD on stack
	RTS		   ;Return to main
	
	ORG $1000 	   ;Subroutine origin
ASCII   TSX		   ;Copy SP to IX
        LDAA 3,X           ;Load ACCA with packed BCD number from the stack
	STAA 2,X	   ;Store the packed BCD on the stack
	BCLR 3,X,%00001111 ;Mask off the LSN
	LDAA 3,X	   ;Store MSN into ACCA
	ASRA
	ASRA
	ASRA
	ASRA		   ;Shift the MSN into the LSN's position
        LDY #TABLE         ;Load table contents to IY
COMP1   CMPA 2,Y+          ;Compare with every second byte in the table starting with base address
        BNE COMP1          ;Branch if Z bit not=0
        LDAA 1,-Y          ;Get ASCII code representation
        STAA 3,X           ;Store MSN on the stack
        LDY #TABLE         ;Load table contents to IY
	BCLR 2,X,%11110000 ;Mask of the MSN from the BCD
	LDAA 2,X	   ;Load the LSN into ACCA
COMP2   CMPA 2,Y+          ;Compare with every second byte in the table starting with base address
        BNE COMP2          ;Branch if Z bit not=0
        LDAA 1,-Y          ;Get ASCII code representation
        STAA 2,X           ;Store LSN on the stack
        RTS                ;Return to main program
		
	ORG $1100          ;Origin of look up table
TABLE   DB $01,'1'         ;ASCII look up table for the depressed key
        DB $02,'2'
        DB $03,'3'
        DB $04,'4'
        DB $05,'5'
        DB $06,'6'
        DB $07,'7'
        DB $08,'8'
        DB $09,'9'
	DB $00,'0'
		
	ORG $1200
WRITE	TSX
	LDY #$02
	LDAA #$FF
        STAA PMDDR             	;set Port M to output
        BCLR PORTM,%00011100   	;clear E, R/W and RS lines
        STAA PADDR             	;set Port A to output
        LDAA #$01
        STAA PORTA             	;Clear Display command
        BSET PORTM,%00010000   	;set E
        NOP
	NOP
	NOP
        BCLR PORTM,%00010000   	;clear E
	LDAA #!1
	PSHA
	JSR DELAY
	PULA
NEXT	JSR BUSY
	LDAA #$FF
	STAA PADDR		;Make port A outputs
	LDAA 3,X		;Load the current number
	BSET PORTM,%00000100	;Set the RS bit
	BCLR PORTM,%00001000	;Clear the RW bit
	STAA PORTA		;Put the number on port A
	BSET PORTM,%00010000	;Set enable high
	NOP			;Delay
	NOP
	NOP
	NOP
	NOP
	BCLR PORTM,%00010000	;Set enable low
	DEX			;Decrement NUMBER register
	DEY			;Decrement NUMBER counter
	BNE NEXT		;If more letters, branch to NEXT
	RTS
	
	ORG $1300
BUSY	LDAA #$00		;Make port A inputs
	STAA PADDR
	BCLR PORTM,%00000100	;Clear RS bit
	BSET PORTM,%00001000	;Set RW bit
	BSET PORTM,%00010000	;Set enable high
	NOP			;Delay
	NOP
	NOP
	NOP
	NOP
	LDAA PORTA		;Load port A to ACCA
	BCLR PORTM,%00010000	;Set enable low
	CMPA #%10000000		;See if BF is set
	BEQ BUSY		;If set, start busy loop again
	RTS			;If not busy, return
		
	ORG $1400          ;Subroutine origin
TEST    TSX	           ;Copy stack pointer to IX
        LDAA $3,X	   ;Load ACCA with N from the stack
        BCLR $2,X,%11111111;Clearing bits LED1-2
        BCLR $3,X,%11111111;Clearing bits LED3-10
TEST1   CMPA #!0	   ;ACCA - 0
        BHS BD1            ;Branch if result >= 0
TEST2   CMPA #!5	   ;ACCA - 5
        BHS BD2	           ;Branch if result >= 0
TEST3   CMPA #!10          ;ACCA - 10
        BHS BD3            ;Branch if result >= 0
TEST4   CMPA #!15          ;ACCA - 15
        BHS BD4            ;Branch if result >= 0
TEST5   CMPA #!20          ;ACCA - 20
        BHS BD5            ;Branch if result >= 0
TEST6   CMPA #!25          ;ACCA - 25
        BHS BD6            ;Branch if result >= 0
TEST7   CMPA #!30          ;ACCA - 30
        BHS BD7            ;Branch if result >= 0
TEST8   CMPA #!35          ;ACCA - 35
        BHS BD8            ;Branch if result >= 0
TEST9   CMPA #!40          ;ACCA - 40
        BHS BD9            ;Branch if result >= 0
TEST10  CMPA #!45	   ;ACCA - 45
        BHS BD10           ;Branch if result >=0
FIN     RTS		   ;Return to main program

BD1     BSET 2,X,LED1      ;Turn on LED1
        BRA TEST2	   ;Check next level
BD2     BSET 2,X,LED2      ;Turn on LED2
        BRA TEST3	   ;Check next level
BD3     BSET 3,X,LED3      ;Turn on LED3
        BRA TEST4          ;Check next level
BD4     BSET 3,X,LED4      ;Turn on LED4
        BRA TEST5          ;Check next level
BD5     BSET 3,X,LED5      ;Turn on LED5
        BRA TEST6          ;Check next level
BD6     BSET 3,X,LED6      ;Turn on LED6
        BRA TEST7          ;Check next level
BD7     BSET 3,X,LED7      ;Turn on LED7
        BRA TEST8          ;Check next level
BD8     BSET 3,X,LED8      ;Turn on LED8
        BRA TEST9          ;Check next level
BD9     BSET 3,X,LED9      ;turn on LED9
        BRA TEST10         ;Check next level
BD10    BSET 3,X,LED10     ;Turn on LED10
        BRA FIN	           ;Return to main
