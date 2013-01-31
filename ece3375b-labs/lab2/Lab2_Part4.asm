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

        ORG $0400          ;Main program origin
        LDS #$4000         ;Declare SP
        LDAA #!160         ;Load ACCA with ADC value N
        PSHA               ;Push ACCA on to the stack
        PSHA               ;Reserve a space on the stack
        JSR TEST           ;Go TEST subroutine
        PULA               ;Pull to ACCA byte for port B from the stack
        PULB               ;Pull to ACCB byte for port A from the stack

        ORG $0500          ;Subroutine origin
TEST    TSX	               ;Copy stack pointer to IX
        LDAA $2,x	       ;Load ACCA with N from the stack
        BCLR $2,x,%11111111;Clearing bits LED1-2
        BCLR $3,x,%11111111;Clearing bits LED3-10
TEST1   CMPA #!125	       ;ACCA - 125
        BHS BD1            ;Branch if result >= 0
TEST2   CMPA #!130	       ;ACCA - 130
        BHS BD2	           ;Branch if result >= 0
TEST3   CMPA #!135         ;ACCA - 135
        BHS BD3            ;Branch if result >= 0
TEST4   CMPA #!140         ;ACCA - 140
        BHS BD4            ;Branch if result >= 0
TEST5   CMPA #!145         ;ACCA - 145
        BHS BD5            ;Branch if result >= 0
TEST6   CMPA #!150         ;ACCA - 150
        BHS BD6            ;Branch if result >= 0
TEST7   CMPA #!155         ;ACCA - 155
        BHS BD7            ;Branch if result >= 0
TEST8   CMPA #!160         ;ACCA - 160
        BHS BD8            ;Branch if result >= 0
TEST9   CMPA #!165         ;ACCA - 165
        BHS BD9            ;Branch if result >= 0
TEST10  CMPA #!170	       ;ACCA - 170
        BHS BD10           ;Branch if result >=0
FIN     RTS			       ;Return to main program

BD1     BSET 2,x,LED1      ;Turn on LED1
        BRA TEST2	       ;Check next level
BD2     BSET 2,x,LED2      ;Turn on LED2
        BRA TEST3	       ;Check next level
BD3     BSET 3,x,LED3      ;Turn on LED3
        BRA TEST4          ;Check next level
BD4     BSET 3,x,LED4      ;Turn on LED4
        BRA TEST5          ;Check next level
BD5     BSET 3,x,LED5      ;Turn on LED5
        BRA TEST6          ;Check next level
BD6     BSET 3,x,LED6      ;Turn on LED6
        BRA TEST7          ;Check next level
BD7     BSET 3,x,LED7      ;Turn on LED7
        BRA TEST8          ;Check next level
BD8     BSET 3,x,LED8      ;Turn on LED8
        BRA TEST9          ;Check next level
BD9     BSET 3,x,LED9      ;turn on LED9
        BRA TEST10         ;Check next level
BD10    BSET 3,x,LED10     ;Turn on LED10
        BRA FIN	           ;Return to main

		END