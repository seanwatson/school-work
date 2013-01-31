PORTA   EQU $0000                       
PADDR   EQU $0002
PORTM	EQU $0250
PMDDR	EQU $0252
PUCR	EQU $000C

	ORG $0400		;Main program origin
	LDS #$4000		;Declare stack pointer
	LDAA #!16		;16ms delay
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
	LDAA #!1
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
	
	
	END
		
		
	ORG $0500               ;Subroutine origin
DELAY   TSX                     ;Copy SP to IX
DELAY1  LDY #!2000              ;Load loop counter (LC)
DELAY2  DEY                     ;LC=LC-1
        BNE DELAY2              ;Repeat if not=0
        DEC $2,X                ;Decrement no. of ms delay
        BNE DELAY1              ;Repeat if not=0
        RTS      
