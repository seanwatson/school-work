PORTB   EQU $0001                       
PBDDR   EQU $0003                       
		
		ORG $0400                       ;Main program origin
        LDS #$4000                      ;Declare SP
	LDAA #$FF
	STAA PBDDR						;Declare port B as outputs
CYCLE	BSET PORTB,%10000000			;Turn on port B bit 7		
        LDAA #!85                       ;Load ACCA with required delay in ms
	PSHA							;Push to stack
	JSR DELAY						;Jump to subroutine
	PULA							;Clean up stack
	BCLR PORTB,%10000000            ;Turn off bit 7 on port B
        LDAA #!85                       ;Load ACCA with delay
        PSHA                            ;Push ACCA on to the stack
        JSR DELAY                       ;Go to DELAY subroutine
        PULA                            ;Clean up stack
        BRA CYCLE						;Go to CYCLE

        ORG $0500                       ;Subroutine origin
DELAY   TSX                             ;Copy SP to IX
DELAY1  LDY #!2000                      ;Load loop counter (LC)
DELAY2  DEY                             ;LC=LC-1
        BNE DELAY2                      ;Repeat if not=0
        DEC $2,X                        ;Decrement no. of ms delay
        BNE DELAY1                      ;Repeat if not=0
        RTS                             ;Return
