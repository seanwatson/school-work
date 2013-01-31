PORTA	EQU $0000
PADDR	EQU $0002
PORTM	EQU $0250
PMDDR	EQU $0252

	ORG $0400	;Main origin
	LDS #$4000	;Declare stack pointer
	LDX #$0700	;Load x index register to record input
	LDY #$000A	;Load y for a loop counter
	LDAA #$FF
	STAA PMDDR	;Make port M outputs
LOAD	LDAA #$01
	STAA 0,X	;Store the key pressed in memory
	INX
	DEY
	BNE LOAD	;Repeat 10 times
	
	LDY #$0600	;Load the y index register with my student number
	CMX 0,Y		;Compare the first digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare second digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare third digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare fourth digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare fifth digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare sixth digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare seventh digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare eigth digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare ninth digits
	BNE ERR		;Branch if not equal
	INX		;Increment registers
	INY
	CMX 0,Y		;Compare tenth digits
	BNE ERR		;Branch if not equal
	BRA CORRECT 	;If all correct, branch to correct
		
ERR	LDX #$0550	;Load x index register with ERROR table
	LDY #$0005	;Load y as letter loop counter
ERR2	JSR BUSY	;Wait until LCD is not busy
	LDAA #$FF
	STAA PADDR		;Make port A outputs
	LDAA 0,X		;Load the current letter
	BSET PORTM,%00000100	;Set the RS bit
	BCLR PORTM,%00001000	;Clear the RW bit
	STAA PORTA		;Put the letter on port A
	BSET PORTM,%00010000	;Set enable high
	NOP			;Delay
	NOP
	NOP
	NOP
	NOP
	BCLR PORTM,%00010000	;Set enable low
	INX			;Increment letter register
	DEY			;Decrement letter counter
	BNE ERR2		;If more letters, branch to ERR2
	END
		
CORRECT	LDX #$0500		;Load surname table
	LDY #$0006		;Load letter counter
CORR2	JSR BUSY		;Check if busy
	LDAA #$FF		;Make port A outputs
	STAA PADDR
	LDAA 0,X		;Load current letter
	BSET PORTM,%00000100	;Set RS bit
	BCLR PORTM,%00001000	;Clear RW bit
	STAA PORTA		;Put letter on port A
	BSET PORTM,%00010000	;Set enable high
	NOP			;Delay
	NOP
	NOP
	NOP
	NOP
	BCLR PORTM,%00010000	;Set enable low
	INX			;Increment letter
	DEY			;Decrement letter counter
	BNE CORR2		;If not done, write next letter
	END

	ORG $0900
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
		
	ORG $0500
SURNAME	DB 'WATSON'

	ORG $0550
ERRR	DB 'ERROR'

	ORG $0600
NUMBER	DB $00,$02,$05,$00,$04,$07,$01,$04,$08,$05

		
