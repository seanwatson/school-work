!************************************************************************************************************
! Sean Watson
! Assignment #3
! File: asn3.m
!
! Description:
!	This file conatains a program that takes a string as its input and
!	encodes it using the base 64 encoding scheme. The encoded string
!	is then output to stdout
!
! Register Legend:
!	%combined_r	The number of characters in the string to be encoded	%l0
!	%shift_r	Used to shift the current character into the proper	%l1
!			spot in the string
!	%string_r	The string that is to be encoded			%l2
!
!
!************************************************************************************************************

!************************************************************************************************************
! Constants
!************************************************************************************************************

define(EOL, 10)		! constant for the new line character
define(ONE_CHAR, 8)	! number of bits in one character
define(TWO_CHARS, 16)	! number of bits in two characters
define(STRING_END, -1)	! represents the end of the input string

!************************************************************************************************************
! Registers
!************************************************************************************************************

define(combined_r, l0)
define(shift_r, l1)
define(string_r, l2)

!************************************************************************************************************
! Main Program Begins
!************************************************************************************************************

	.global main

main: 	save	%sp, -96, %sp			! main program begins

	mov	0, %combined_r			! initialize the chars in the string to 0	
	mov	TWO_CHARS, %shift_r		! initialize the shift amount to two chars
	mov	0, %string_r			! clear the encoded string
	
loop:
	call	getchar				! get the next character from the input
	nop
	
	cmp	%o0, STRING_END			! if it is the end of the string break the loop
	be	done
	nop
	
	sll	%o0, %shift_r, %o0		! shift the character into the appropriate bits
	sub	%shift_r, ONE_CHAR, %shift_r	! decrement the shift amount by one char
	or	%o0, %string_r, %string_r	! combine the current char with the rest of the string
	inc	%combined_r			! increment the number of chars in the string
	
	cmp	%combined_r, 3			! add more characters to the string if there is less than 3
	bl	loop
	nop

	mov	%string_r, %o0			! move the string in to the 1st paramater
	mov	%combined_r, %o1		! move the number of chars in the string to the 2nd parameter
	call	get_b64_str			! get the encoded string for the current 3 chars
	nop

	call 	printf				! print the encoded string to stdout
	nop

	mov	%g0, %string_r			! reset the string to empty
	mov	%g0, %combined_r		! reset the number of chars in the string to 0
	mov	TWO_CHARS, %shift_r		! reset the shift amount to two chars

	ba	loop				! continue until the end of the string is reached
	nop

done:
	cmp	%combined_r, 0			! check if there are still chars to be encoded
	be	exit				! if no chars left finish up the program
	nop

	mov	%string_r, %o0			! move the remaining string to the 1st parameter
	mov	%combined_r, %o1		! move the number of chars in the string to the 2nd parameter
	call	get_b64_str			! get the encoded string for the remaining characters
	nop

	call	printf				! print the remaining encoded characters to stdout
	nop

exit:
	mov	EOL, %o0			! add a new line character to the end of the output
	call	putchar
	nop

	ret					! terminate the program
	restore
