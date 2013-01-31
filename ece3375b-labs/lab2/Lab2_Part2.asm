        ORG $0400          ;Main program origin
        LDS #$4000         ;Declare SP
        LDAA #$84          ;Load ACCA when key is depressed
        PSHA               ;Push ACCA on to the stack
        JSR SEARCH         
        PULA               ;Pull ACCA from the stack

        ORG $0500          ;Subroutine origin
SEARCH  TSX                
        LDAA $2,X          ;Get key byte from the stack
        LDY #$0700
COMPARE CMPA 2,Y+          ;Compare with every second bye in the table
						   ;Starting with base address
        BNE COMPARE        
        LDAA 1,-Y          ;Get ASCII CODE
        STAA $2,X          ;Overwrite key byte on stack
        RTS                ;Return to main

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

        END