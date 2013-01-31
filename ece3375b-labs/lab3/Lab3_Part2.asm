PORTB	EQU $0001
PBDDR	EQU $0003
PUCR	EQU $000C		
		
	ORG $0400		;Main origin
	LDS #$4000		;Declare SP
	BCLR PUCR,%00000010	;Disable pull ups for port B
CLOSURE	LDAA #$FF
	STAA PBDDR		;Make port B outputs
	LDAA #$00
	STAA PORTB		;Load $00 to port B
	NOP
	NOP
	NOP
	NOP
	NOP			;Delay
	LDAA #$00
	STAA PBDDR		;Make port B inputs
	LDAA PORTB		;Load port B to ACCA
	CMPA #$F0		;Compare MSN
	BGE CLOSURE		;If MSN is 1111, keep scanning
	LDAA #!10		;If less than, wait 10ms
	PSHA
	JSR DELAY
	PULA
	LDAA PORTB		;Load port B to ACCA
	CMPA #$F0		;Compare MSN
	BGE CLOSURE		;If MSN is 1111 after debounce, keep scanning
		
	LDAA #$FE		;Scan for specific key, start with FE
NEXTCOL	LDAB #$FF			
	STAB PBDDR		;Make port B outputs
	STAA PORTB		;Load first column to port B
	NOP			;Delay
	NOP
	NOP
	NOP
	NOP
	LDAB #$00		;Make port B inputs
	STAB PBDDR
	LDAB PORTB		;Load port B into ACCB
	CMPB #$F0		;Compare MSN to 1111
	BLT COMP		;If not 1111, get complement
	ASLA			;Shift 0 left
	INCA			;Add 1 
	CMPA #%11101111		;Check if all columns scanned
	BNE NEXTCOL		;Try next column
	BRA CLOSURE		;Scan entire keypad again if none found
		
COMP	COMB			;Complement B
	PSHB			;Push B to stack
	JSR SEARCH		;Find the ASCII code
	PULB			;Pull the ASCII character
		
OPEN	LDAA #$FF
	STAA PBDDR		;Make port B outputs
	LDAA #$00
	STAA PORTB		;Load $00 to port B
	NOP
	NOP
	NOP
	NOP
	NOP			;Delay
	LDAA #$00
	STAA PBDDR		;Make port B inputs
	LDAA PORTB		;Load port B to ACCA
	CMPA #$F0		;Compare MSN
	BLT OPEN		;If switch still pressed, keep waiting
	LDAA #!10		;If none pressed, wait 10ms
	PSHA
	JSR DELAY
	PULA
	LDAA PORTB		;Load port B to ACCA
	CMPA #$F0	   	;Compare MSN
	BLT OPEN	   	;If switch pressed now, keep waiting
		
	END
		
		
	ORG $0500          	;Subroutine origin
DELAY   TSX                	;Copy SP to IX
DELAY1  LDY #!2000         	;Load loop counter (LC)
DELAY2  DEY                	;LC=LC-1
        BNE DELAY2        	;Repeat if not=0
        DEC $2,X        	;Decrement no. of ms delay
        BNE DELAY1         	;Repeat if not=0
        RTS                	;Return
		
        ORG $0600          	;Subroutine origin
SEARCH  TSX                
        LDAA $2,X          	;Get key byte from the stack
        LDY #$0700
COMPARE CMPA 2,Y+      	    	;Compare with every second bye in the table
			   	;Starting with base address
        BNE COMPARE        
        LDAA 1,-Y      	    	;Get ASCII CODE
        STAA $2,X      	    	;Overwrite key byte on stack
        RTS               	;Return to main

        ORG $0700          
KEYTAB  DB $82,'0'         
        DB $11,'1'
        DB $12,'2'
        DB $14,'3'
        DB $21,'4'
        DB $22,'5'
        DB $24,'6'
        DB $41,'7'
        DB $42,'8'
        DB $44,'9'
        DB $81,'A'
        DB $84,'B'
        DB $88,'C'
        DB $48,'D'
        DB $28,'E'
        DB $18,'F'
