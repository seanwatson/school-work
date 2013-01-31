!*********************************************************************************************************
! Sean Watson
! Assignment #4
! File: asn4.m
!
! Description:
!	This file contains a program that takes a miniclass file as an argument,
!	reads bytecodes from it into memory and executes the bytecodes in a JVM.
!
! Usage:	asn4	[.miniclass file]
!
! Return Values:
!	0 - Normal execution, no runtime errors
!	1 - Wrong number of arguments
!	2 - Error reading .miniclass file
!	3 - Division by 0 during bytecode execution
!
! Register Legend:
!	%argc_r		The number of arguments passed in the program call	%i0
!	%argv_r		The argument vector, contains a pointer to the filepath	%i1
!
!*********************************************************************************************************

include(macro_defs.m)			! needed for some macros used

!*********************************************************************************************************
! Constants
!*********************************************************************************************************

define(BYTE,1)                          ! size of a byte in bytes
define(SIZE,100)                        ! size of bytecode array (assumed)
define(ONE_ARG, 2)			! argc value if one argument is passed
define(WORD, 4)				! size of a word in bytes
define(ARG_ERR, 1)			! return value for an argument error
define(READ_ERR, 2)			! return value for a reading error
define(DIV_ERR, 3)			! return value for a divide by 0

!*********************************************************************************************************
! Registers
!*********************************************************************************************************

define(argc_r, i0)
define(argv_r, i1)

!*********************************************************************************************************
! Main Program Begins
!*********************************************************************************************************

	.data									! variable declarations

arg_msg:	.asciz "ERROR: Wrong number of parameters. Terminating\n"	! argument error message
read_msg:	.asciz "ERROR: Problem reading .miniclass file. Terminating\n"	! file read error message

	.text					! static declarations
	
	local_var
	var(byte_stream_s,BYTE,SIZE*BYTE)       ! array for storing bytecode stream

        begin_main				! main program begins

	cmp	%argc_r, ONE_ARG		! check the number of arguments passed
	bne	arg_error			! show an error message if wrong number of args
	nop
	
        ld	[%argv_r + WORD], %o0		! pass a pointer to the .miniclass file name
        call    get_codes               	! read bytecodes from file into array
        add     %fp,byte_stream_s,%o1 		! [DS] pass address of bytecode array

	tst	%o0				! check if there was an error reading the file
	bneg	read_error			! if so display an error
	nop
	
        mov     %o0,%o1                 	! pass number of bytes in bytecode stream
        call    execute_codes          	 	! execute instructions in bytecode
        add     %fp,byte_stream_s,%o0   	! [DS] pass address of bytecode array

	tst	%o0				! check if there was a division error during execution
	bneg	div_error			! if so return an error value
	nop

	ba	quit
	clr	%argc_r				! [DS] return 0 to indicate normal execution

arg_error:
	set	arg_msg, %o0			! print the error message
	call	printf
	nop

	ba	quit
	mov	ARG_ERR, %argc_r		! [DS] return 1 to indicate an argument error

read_error:
	set	read_msg, %o0			! print the error message
	call	printf
	nop

	ba	quit
	mov	READ_ERR, %argc_r		! [DS] return 2 to indicate a read error

div_error:
	mov	DIV_ERR, %argc_r		! return 3 to indicate a division by 0
	
quit:   ret                            		! exit the program and hope it worked aiiiight
        restore
