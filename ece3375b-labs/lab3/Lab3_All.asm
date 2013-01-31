PORTA   EQU $0000                       
PADDR   EQU $0002
PORTB	EQU $0001
PBDDR	EQU $0003
PORTM	EQU $0250
PMDDR	EQU $0252
PUCR	EQU $000C
INPUT	EQU $0900
COUNT	EQU $1400

	ORG $0400				;Main program origin
	LDS #$4000				;Declare stack pointer
	JSR INIT				;Initialize display
	LDAA #$00
	STAA COUNT				;Set counter to 0				
	LDAA #$FF
	STAA PMDDR				;Make port M outputs
	BCLR PUCR,%00000010			;Disable pull ups for port B

CLOSURE	LDAA #$FF
	STAA PBDDR				;Make port B outputs
	LDAA #$00
	STAA PORTB				;Load $00 to port B
	NOP
	NOP
	NOP
	NOP
	NOP					;Delay
	LDAA #$00
	STAA PBDDR				;Make port B inputs
	LDAA PORTB				;Load port B to ACCA
	CMPA #$F0				;Compare MSN
	BGE CLOSURE				;If MSN is 1111, keep scanning
	LDAA #!10				;If less than, wait 10ms
	PSHA
	JSR DELAY
	PULA
	LDAA PORTB				;Load port B to ACCA
	CMPA #$F0				;Compare MSN
	BGE CLOSURE				;If MSN is 1111 after debounce, keep scanning		
	
	LDAA #$FE				;Scan for specific key, start with FE
NEXTCOL	LDAB #$FF			
	STAB PBDDR				;Make port B outputs
	STAA PORTB				;Load first column to port B
	NOP					;Delay
	NOP
	NOP
	NOP
	NOP
	LDAB #$00				;Make port B inputs
	STAB PBDDR
	LDAB PORTB				;Load port B into ACCB
	CMPB #$F0				;Compare MSN to 1111
	BLT COMP				;If not 1111, get complement
	ASLA					;Shift 0 left
	INCA					;Add 1 
	CMPA #%11101111				;Check if all columns scanned
	BNE NEXTCOL				;Try next column
	BRA CLOSURE				;Scan entire keypad again if none found	

COMP	COMB					;Complement B
	PSHB					;Push B to stack
	JSR SEARCH				;Find the ASCII code
	PULB					;Pull the ASCII character	

OPEN	LDAA #$FF
	STAA PBDDR				;Make port B outputs
	LDAA #$00
	STAA PORTB				;Load $00 to port B
	NOP
	NOP
	NOP
	NOP
	NOP					;Delay
	LDAA #$00
	STAA PBDDR				;Make port B inputs
	LDAA PORTB				;Load port B to ACCA
	CMPA #$F0				;Compare MSN
	BLT OPEN				;If switch still pressed, keep waiting
	LDAA #!10				;If none pressed, wait 10ms
	PSHA
	JSR DELAY
	PULA
	LDAA PORTB				;Load port B to ACCA
	CMPA #$F0				;Compare MSN
	BLT OPEN				;If switch pressed now, keep waiting
		
LOAD	LDX #$0900				;Load start of number
	LDAA #$00
	ABA					;Transfer ASCII number to ACCA
	LDAB COUNT				;Load b with counter
	ABX					;Offset X by count
	STAA 0,X				;Store the key pressed in memory
	INB
	STAB COUNT				;Increment and store count
	CMPB #$000A	
	BNE CLOSURE				;Repeat 10 times

CHECK	LDY #$1300				;Load the y index register with my student number
	LDX #$0900
	CMX 0,Y					;Compare the first digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare second digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare third digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare fourth digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare fifth digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare sixth digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare seventh digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare eigth digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare ninth digits
	BNE ERR					;Branch if not equal
	INX					;Increment registers
	INY
	CMX 0,Y					;Compare tenth digits
	BNE ERR					;Branch if not equal
	BRA CORRECT 				;If all correct, branch to correct		
	
ERR	LDX #$1200				;Load x index register with ERROR table
	LDY #$0005				;Load y as letter loop counter
ERR2	JSR BUSY				;Wait until LCD is not busy
	LDAA #$FF
	STAA PADDR				;Make port A outputs
	LDAA 0,X				;Load the current letter
	BSET PORTM,%00000100			;Set the RS bit
	BCLR PORTM,%00001000			;Clear the RW bit
	STAA PORTA				;Put the letter on port A
	BSET PORTM,%00010000			;Set enable high
	NOP					;Delay
	NOP
	NOP
	NOP
	NOP
	BCLR PORTM,%00010000			;Set enable low
	INX					;Increment letter register
	DEY					;Decrement letter counter
	BNE ERR2				;If more letters, branch to ERR2
	END
	
CORRECT	LDX #$1100				;Load surname table
	LDY #$0006				;Load letter counter
CORR2	JSR BUSY				;Check if busy
	LDAA #$FF				;Make port A outputs
	STAA PADDR
	LDAA 0,X				;Load current letter
	BSET PORTM,%00000100			;Set RS bit
	BCLR PORTM,%00001000			;Clear RW bit
	STAA PORTA				;Put letter on port A
	BSET PORTM,%00010000			;Set enable high
	NOP					;Delay
	NOP
	NOP
	NOP
	NOP
	BCLR PORTM,%00010000			;Set enable low
	INX					;Increment letter
	DEY					;Decrement letter counter
	BNE CORR2				;If not done, write next letter
	END	

	ORG $0500
INIT	LDAA #!16				;16ms delay
	PSHA
	JSR DELAY
	PULA
	LDAA #$FF
	STAA PMDDR				;Declare port M as outputs
	BCLR PORTM,%00011100			;Clear E,R/W,RS
	STAA PADDR				;Declare port A as outputs
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;Transfer function set command
	LDAA #!5
	PSHA
	JSR DELAY
	PULA					;Delay 5ms
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;Transfer function set command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA					;Delay 1ms
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;Transfer function set command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA					;Delay 1ms
	LDAA #$30
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;One line display and font selection
	LDAA #!1
	PSHA
	JSR DELAY
	PULA					;Delay 1ms
	LDAA #$08
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;Display off command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA					;Delay 1ms
	LDAA #$01
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;Clear display
	LDAA #!1
	PSHA
	JSR DELAY
	PULA					;Delay 1ms
	LDAA #$06
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;Entry mode set
	LDAA #!1
	PSHA
	JSR DELAY
	PULA					;Delay 1ms
	LDAA #$0E
	STAA PORTA
	BSET PORTM,%00010000
	BCLR PORTM,%00010000			;Display on command
	LDAA #!1
	PSHA
	JSR DELAY
	PULA					;Delay 1ms
	RTS
	
	ORG $0600               ;Subroutine origin
DELAY   TSX                     ;Copy SP to IX
DELAY1  LDY #!2000              ;Load loop counter (LC)
DELAY2  DEY                     ;LC=LC-1
        BNE DELAY2              ;Repeat if not=0
        DEC $2,X                ;Decrement no. of ms delay
        BNE DELAY1              ;Repeat if not=0
        RTS      
		
        ORG $0700         	;Subroutine origin
SEARCH  TSX                
        LDAA $2,X        	;Get key byte from the stack
        LDY #$0800
COMPARE CMPA 2,Y+  		;Compare with every second byte in the table
				;Starting with base address
        BNE COMPARE       
        LDAA 1,-Y         	;Get ASCII CODE
        STAA $2,X       	;Overwrite key byte on stack
        RTS            		;Return to main

        ORG $0800          
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
		
		ORG $1000
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
	BGE BUSY		;If set, start busy loop again
	RTS			;If not busy, return
	
	ORG $1100
SURNAME	DB 'WATSON'

	ORG $1200
ERRR	DB 'ERROR'

	ORG $1300
NUMBER	DB $00,$02,$05,$00,$04,$07,$01,$04,$08,$05
