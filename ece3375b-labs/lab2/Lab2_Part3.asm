	ORG $0400          ;Main program origin
        LDS #$4000         ;Declare SP
        LDAA #!15          ;Load ACCA with ADC value N
        PSHA               ;Push ACCA on to the stack
        PSHB               ;Reserve a space on the stack
        JSR BCD            ;Go BCD subroutine
        JSR ASCII          ;Go ASCII subroutine
        PULA               ;Pull LSN from the stack into ACCA
        PULB               ;Pull MSN from the stack into ACCB

        ORG $0500          ;Subroutine origin
BCD     TSX	           ;Copy stack pointer to index register
	CLRA		   ;Clear ACCA
	CLRB		   ;Clear ACCB
        LDAA 3,x           ;Load ACCA with N from the stack
		CMPA #$09  ;Compare ACCA with $09
        BLS PACK	   ;Go to PACK if less than or equal to $09
CONT    INCB     	   ;Increment the Tens by 1
        SUBA #$0A	   ;Subtract 10 from the units
        CMPA #$09	   ;Compare ACCA with $09
	BHI CONT	   ;Go to CONT if greater than $09
PACK 	ASLB
	ASLB
	ASLB
	ASLB		   ;Shift tens by 4
	ABA		   ;Form packed BCD result
	STAA 2,x	   ;Store packed BCD on stack
	RTS		   ;Return to main

	ORG $0700          ;Subroutine origin
ASCII   TSX	           ;Copy SP to IX
        LDAA 2,x           ;Load ACCA with packed BCD number from the stack
	STAA 3,x	   ;Store the packed BCD on the stack
	BCLR 3,x,%00001111 ;Mask off the LSN
	LDAA 3,x
	ASRA
	ASRA
	ASRA
	ASRA		   ;Shift the MSN into the LSN's position
	LDAA 3,x	   ;Load the BCD's MSN into ACCA
        LDY #TABLE         ;Load table contents to IY
COMP1   CMPA 2,Y+          ;Compare with every second byte in the table starting with base address
        BNE COMP1          ;Branch if Z bit not=0
        LDAA 1,-Y          ;Get ASCII code representation
        STAA 3,x           ;Store ACCA on the stack
        LDY #TABLE         ;Load table contents to IY
	BCLR 2,x,%11110000 ;Mask of the MSN from the BCD
	LDAA 2,x	   ;Load the LSN into ACCA
COMP2   CMPA 2,Y+          ;Compare with every second byte in the table starting with base address
        BNE COMP2          ;Branch if Z bit not=0
        LDAA 1,-Y          ;Get ASCII code representation
        STAA 2,x           ;Store LSN on the stack
        RTS                ;Return to main program


        ORG $0800          ;Origin of look up table
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

        END
