!***************************************************************************************************
! Sean Watson
! Assignment #4
! File: execute_codes.m
!
! execute_codes:
!	Implements a simple Java Virtual Machine. This program reads
!	bytecodes from an input bytecode array and executes them.
!
! Calling Sequence:
!	- A pointer to the bytecode array is passed in %o0
!	- The number of bytes in the bytecode array
!
! Returns:
!	0 - Successful termination
!	-1 - Division by 0 error
!
! Register Legend:
!	%bytecode_r	A pointer to the bytecode array			%i0
!	%op1_r		Used for data manipulation in bytecode exe	%l0
!	%op2_r		Used for data manipulation in bytecode exe	%l1
!	%op3_r		Used for data manipulation in bytecode exe	%l2
!	%locals_r	Points to the start of the local variable array	%l4
!	%pcounter_r	Program counter, points to current bytecode	%l5
!	%instr_r	Instruction register, holds current bytecode	%l6
!	%stkptr_r	Points to the top of the stack			%l7
!
!***************************************************************************************************

include(macro_defs.m)	! needed for some of the macros used

!***************************************************************************************************
! Register Definitions
!***************************************************************************************************

define(stkptr_r,l7)   
define(pcounter_r,l5) 
define(instr_r,l6)    
define(op1_r,l0)      
define(op2_r,l1)      
define(op3_r,l2)      
define(locals_r,l4)   
define(bytecode_r,i0)  

!****************************************************************************************************
! Constants
!****************************************************************************************************

define(MAXSIZE,20)      ! max. number of stack elements
define(LEN,4)           ! size of each stack element
define(EOL,10)          ! newline character
define(MAXLOCAL, 10)	! max number of local variables
define(TWO_BYTE, 2)	! to skip one byte when moving the program counter
define(THREE_BYTE, 3)	! to skip two bytes when moving the program counter
define(BYTE, 1)		! size of a byte in bytes
define(INDEX_ADJ, 2)	! used to convert an index to an array offset
define(LOW_FIVE, 0x1F)	! sometimes follows a high five, also used to mask 5 low bits out
define(DIV_ERR, -1)	! returned if there was a divide or remainder by 0
define(BYTE_BITS, 8)	! number of bits in a byte

!****************************************************************************************************
! Constants for bytecode operations
!****************************************************************************************************

define(ICONST_0,3)	! push 0 onto the operand stack
define(BIPUSH,16)       ! push (param 1) onto the operand stack
define(ILOAD,21)	! copy a value from the local variable array to the stack 
define(ISTORE,54)	! pop the top of the stack and put it in the local variable array
define(POP,87)          ! pop the top element from the operand stack
define(DUP,89)		! push a copy of the top of the stack onto the stack
define(IADD,96)         ! (2nd from top) + (top), result onto stack
define(ISUB,100)        ! (2nd from top) - (top), result onto stack
define(IMUL,104)        ! (2nd from top) * (top), result onto stack
define(IDIV,108)        ! (2nd from top) / (top), result onto stack
define(IREM,112)	! (2nd from top) % (top), result onto stack
define(ISHL,120)	! logical shift left top of stack by second from top
define(ISHR,122)	! arithmetic shift right top of stack by second from top
define(IUSHR,124)	! logical shift right top of stack by second from top
define(IAND,126)	! (2nd from top) & (top), result onto stack
define(IOR,128)		! (2nd from top) | (top), result onto stack
define(IINC,132)	! increment a local variable by a value
define(IFEQ,153)	! branch to offset if top of stack = 0
define(IFGE,156)	! branch to offset if top of the stack >= 0
define(IF_ICMPGE,162)	! branch to offset if (2nd from top) >= (top)
define(GOTO,167)	! branch to offset
define(RETURN,177)      ! return (exit from the program)
define(IREAD,186)	! read an int from keyboard, push to stack
define(IPRINT,187)      ! print top element on operand stack

!***************************************************************************************************
! execute_codes program begins
!***************************************************************************************************

	.data							! variable declarations

div_by_zero:    .asciz "ERROR: Divide by zero. Terminating\n"   ! division error message
        
        .text						! static declarations

        local_var					! initalize local variables
        var(stack_s,LEN,LEN*MAXSIZE)            	! offset for the operand stack
	var(local_s,LEN,LEN*MAXLOCAL)			! offset for the local variable array

	begin_fn(execute_codes)				! begin execute_codes function

							! set up the operand stack
init:   add     %fp, stack_s, %stkptr_r  	        ! point to start of stack memory
        add     %stkptr_r, LEN*MAXSIZE, %stkptr_r 	! move pointer to 'bottom' of stack

	add	%fp, local_s, %locals_r			! set up the local variable array

	mov	%bytecode_r, %pcounter_r		! point the program counter at the first bytecode

							! search for the current bytecode
top:	ldub	[%pcounter_r], %instr_r			! read the current bytecode

        cmp     %instr_r, RETURN                     	! the 'return' command? (177)
        be      success
        nop

        cmp     %instr_r, IPRINT                     	! the 'iprint' command? (187)
        be      iprint
        nop
        
        cmp     %instr_r, BIPUSH                     	! the 'bipush' command? (16)
        be      bipush
        nop

        cmp     %instr_r, POP                        	! the 'pop' command? (87)
        be      pop
        nop

        cmp     %instr_r, IADD                       	! the 'iadd' command? (96)
        be      iadd
        nop

        cmp     %instr_r, ISUB                       	! the 'isub' command? (100)
        be      isub
        nop

        cmp     %instr_r, IMUL                       	! the 'imul' command? (104)
        be      imul
        nop

        cmp     %instr_r, IDIV                       	! the 'idiv' command? (108)
        be      idiv
        nop

        cmp     %instr_r, ICONST_0                     	! the 'iconst_0' command? (3)
        be      iconst_0
        nop

        cmp     %instr_r, ILOAD                     	! the 'iload' command? (21)
        be      iload
        nop
        
        cmp     %instr_r, ISTORE                     	! the 'istore' command? (54)
        be      istore
        nop

        cmp     %instr_r, DUP                        	! the 'dup' command? (89)
        be      dup
        nop

        cmp     %instr_r, IREM                       	! the 'irem' command? (112)
        be      irem
        nop

        cmp     %instr_r, ISHR                       	! the 'ishr' command? (122)
        be      ishr
        nop

        cmp     %instr_r, IINC                       	! the 'iinc' command? (132)
        be      iinc
        nop

        cmp     %instr_r, IFEQ                       	! the 'ifeq' command? (153)
        be      ifeq
        nop

        cmp     %instr_r, GOTO                     	! the 'goto' command? (167)
        be      goto
        nop

	cmp	%instr_r, IREAD				! the 'iread' command? (186)
	be	iread
	nop

	cmp	%instr_r, IAND				! the 'iand' command? (126)
	be	iand
	nop

	cmp	%instr_r, IOR				! the 'ior' command? (128)
	be	ior
	nop

	cmp	%instr_r, IUSHR				! the 'iushr' command? (124)
	be	iushr
	nop

	cmp	%instr_r, ISHL				! the 'ishl' command? (120)
	be	ishl
	nop

	cmp	%instr_r, IFGE				! the 'ifge' command? (156)
	be	ifge
	nop

	cmp	%instr_r, IF_ICMPGE			! the 'if_icmpge' command? (162)
	be	if_icmpge
	nop

!***************************************************************************************************
! Execute bytecodes
!***************************************************************************************************

							! push a value onto the stack
bipush: ldsb	[%pcounter_r + BYTE], %op1_r		! read the extra parameter
        
        dec     LEN, %stkptr_r                   	! push the parameter onto the stack
        st      %op1_r, [%stkptr_r]

        ba      top                             	! back to read next command
        inc	TWO_BYTE, %pcounter_r			! [DS] move program counter to next bytecode

							! pop the top element off the operand stack
pop:    inc     LEN, %stkptr_r                   	! move stack ptr and go back
        ba      top                             
        inc	%pcounter_r				! [DS] move program counter to next bytecode
        
							! add the top two stack elements, push the result
iadd:   ld      [%stkptr_r], %op2_r               	! pop the first operand
        inc     LEN, %stkptr_r				
        ld      [%stkptr_r], %op1_r               	! get second operand
                                                
        add     %op1_r, %op2_r, %op1_r             	! do the addition
        st      %op1_r, [%stkptr_r]               	! put the result back on the stack

        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move the program counter to next bytecode

							! subtract the top of the stack from the second from the top
							! put the result back on the stack
isub:   ld      [%stkptr_r], %op2_r               	! pop the first operand
        inc     LEN, %stkptr_r
        ld      [%stkptr_r], %op1_r               	! load second operand
                                                
        sub     %op1_r, %op2_r, %op1_r             	! do the subtraction
        st      %op1_r, [%stkptr_r]               	! put the result back on the stack

        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move the program counter to next bytecode

							! multiply the top two stack elements, push result back
imul:   ld      [%stkptr_r], %o1                 	! pop the first operand
        inc     LEN, %stkptr_r
        ld      [%stkptr_r], %o0                 	! load second operand
                                                
        call    .mul                            	! do the multiplication
        nop
        st      %o0, [%stkptr_r]                 	! put the result back on the stack

        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move the program counter to next bytecode

							! (2nd from the top) / (top), result back on the stack
idiv:   ld      [%stkptr_r], %o1                 	! pop first operand
        inc     LEN, %stkptr_r

        tst     %o1                             	! make sure it's not divide by zero
        be      div_error
        nop

        ld      [%stkptr_r], %o0                 	! load second operand
                                                
        call    .div                            	! do the division
        nop
        st      %o0, [%stkptr_r]                 	! put the result back on the stack

        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move program counter to next bytecode

							! print the number at the top of the operand stack
iprint: call    writeInt                        	! print value at top of stack
        ld      [%stkptr_r], %o0                 	! [DELAY SLOT FILLED]

        call    writeChar                       	! print blank line
        mov     EOL, %o0                        	! [DELAY SLOT FILLED]

        ba      top                             	! get next command
        inc	%pcounter_r

iconst_0:						! pushes 0 onto the top of the stack
	dec	LEN, %stkptr_r				! push 0 onto the stack
	st	%g0, [%stkptr_r]

        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move program counter to next bytecode
	
							! pushes a value from local variable onto stack
iload:	ldsb	[%pcounter_r + BYTE], %op1_r		! load the index of the local variable

	sll	%op1_r, INDEX_ADJ, %op1_r		! convert the index value to an array offset			
	ld	[%op1_r + %locals_r], %op2_r		! load the local variable

	dec	LEN, %stkptr_r				! push the local variable
	st	%op2_r, [%stkptr_r]

        ba      top                             	! back to the start
        inc	TWO_BYTE, %pcounter_r			! [DS] move program counter to next bytecode
	
							! stores a value from the stack to a local variable
istore:	ldsb	[%pcounter_r + BYTE], %op1_r		! load the index to store the value to

	ld	[%stkptr_r], %op2_r			! pop value from the stack
	inc	LEN, %stkptr_r

	sll	%op1_r, INDEX_ADJ, %op1_r		! convert the index to an array offset
	st	%op2_r, [%op1_r + %locals_r]		! store the value to the array

        ba      top                             	! back to the start
        inc	TWO_BYTE, %pcounter_r			! [DS] move program counter to next bytecode
	
							! pushes a copy of the top of the stack
dup:	ld	[%stkptr_r], %op1_r			! load the value of the top of the stack
	
	dec	LEN, %stkptr_r				! push the value onto the stack
	st	%op1_r, [%stkptr_r]

        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move the program counter to next bytecode
	
							! (2nd from top) mod (top), push result
irem:	ld	[%stkptr_r], %o1			! pop the second argument off
	inc	LEN, %stkptr_r

	tst	%o1					! check for divide by 0
	be	div_error
	nop

	ld	[%stkptr_r], %o0			! pop the first argument off

	call	.rem					! do the remainder
	nop
	st	%o0, [%stkptr_r]			! push the result
	
        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move program counter to next bytecode
	
							! (top) arithmetic shift right by (2nd from top), push result
ishr:	ld	[%stkptr_r], %op1_r			! pop the number to be shifted
	inc	LEN, %stkptr_r
	ld	[%stkptr_r], %op2_r			! load the amount to be shifted

	and	%op2_r, LOW_FIVE, %op2_r		! mask off the low 5 bits of the shift amount
	sra	%op1_r, %op2_r, %op1_r			! do the shift

	st	%op1_r, [%stkptr_r]			! push the result

        ba      top                             	! back to the start
        inc	%pcounter_r				! [DS] move program counter to next bytecode
	
							! increment a local variable by a value
iinc:	ldsb	[%pcounter_r + BYTE], %op1_r		! load the index of the local variable
	ldsb	[%pcounter_r + TWO_BYTE], %op2_r	! load the amount to be incremented

	sll	%op1_r, INDEX_ADJ, %op1_r		! convert the index to an array offset
	ld	[%op1_r + %locals_r], %op3_r		! load the local variable

	add	%op2_r, %op3_r, %op3_r			! do the increment

	st	%op3_r, [%op1_r + %locals_r]		! store the local variable back

        ba      top                             	! back to the start
        inc	THREE_BYTE, %pcounter_r			! [DS] move program counter to next bytecode

							! branches by an offset if top of the stack is 0
ifeq:	ld	[%stkptr_r], %op1_r			! pop the top of the stack
	inc	LEN, %stkptr_r
	
	tst	%op1_r					! if it isnt 0, back to start
	bne	top
	inc	THREE_BYTE, %pcounter_r			! [DS] move program counter to next bytecode
	
	dec	THREE_BYTE, %pcounter_r			! move the program counter back if branch not taken
	ldsb	[%pcounter_r + BYTE], %op1_r		! load the high 8 bits of offset
	ldub	[%pcounter_r + TWO_BYTE], %op2_r	! load the low 8 bits of offset

	sll	%op1_r, BYTE_BITS, %op1_r		! shift the high 8 bits into bits 8-15
	or	%op1_r, %op2_r, %op1_r			! combine the high and low bits
	
	ba	top					! back to start
	add	%op1_r, %pcounter_r, %pcounter_r	! [DS] add the offset to the program counter
	
							! branch to offset
goto:	ldsb	[%pcounter_r + BYTE], %op1_r		! load the high 8 bits of offset
	ldub	[%pcounter_r + TWO_BYTE], %op2_r	! load the low 8 bits of offset

	sll	%op1_r, BYTE_BITS, %op1_r		! shift the high 8 bits into bits 8-15
	or	%op1_r, %op2_r, %op1_r			! combine the high and low bits
	
	ba	top					! back to the start
	add	%op1_r, %pcounter_r, %pcounter_r	! [DS] add the offset to the program counter

							! reads an int from the keyboard, pushes to stack
iread:	call	readInt					! read the int
	nop

	dec	LEN, %stkptr_r				! push the int to the stack
	st	%o0, [%stkptr_r]

	ba	top					! back to the start
	inc	%pcounter_r				! [DS] move program counter to next bytecode

							! (2nd from top) & (top), result onto stack
iand:	ld	[%stkptr_r], %op1_r			! pop the first integer off
	inc	LEN, %stkptr_r
	ld	[%stkptr_r], %op2_r			! pop the second integer off

	and	%op1_r, %op2_r, %op1_r			! and the two integers
	st	%op1_r, [%stkptr_r]			! push the result to the stack

	ba	top					! back to the start
	inc	%pcounter_r				! [DS] move program counter to next bytecode
	
							! (2nd from the top) | (top), result onto stack
ior:	ld	[%stkptr_r], %op1_r			! pop the first integer off
	inc	LEN, %stkptr_r
	ld	[%stkptr_r], %op2_r			! pop the second integer off

	or	%op1_r, %op2_r, %op1_r			! or the two integers
	st	%op1_r, [%stkptr_r]			! push the result to the stack
	
	ba	top					! back to the start
	inc	%pcounter_r				! [DS] move program counter to next bytecode

							! logical shift right (top) by (2nd from top)
iushr:	ld	[%stkptr_r], %op1_r			! pop the number to be shifted
	inc	LEN, %stkptr_r
	ld	[%stkptr_r], %op2_r			! load the amount to be shifted

	and	%op2_r, LOW_FIVE, %op2_r		! mask off the low 5 bits of the shift amount
	srl	%op1_r, %op2_r, %op1_r			! do the shift

	st	%op1_r, [%stkptr_r]			! push the result

        ba      top                             	! back to the start
	inc	%pcounter_r				! [DS] move program counter to next bytecode

							! logical shift left (top) by (2nd from top)
ishl:	ld	[%stkptr_r], %op1_r			! pop the number to be shifted
	inc	LEN, %stkptr_r
	ld	[%stkptr_r], %op2_r			! load the amount to be shifted

	and	%op2_r, LOW_FIVE, %op2_r		! mask off the low 5 bits of the shift amount
	sll	%op1_r, %op2_r, %op1_r			! do the shift

	st	%op1_r, [%stkptr_r]			! push the result

        ba      top                             	! back to the start
	inc	%pcounter_r				! [DS] move program counter to next bytecode
	
							! branches by an offset if top of the stack is >= 0
ifge:	ld	[%stkptr_r], %op1_r			! pop the top of the stack
	inc	LEN, %stkptr_r
	
	tst	%op1_r					! if it is less than 0, back to start
	bneg	top
	inc	THREE_BYTE, %pcounter_r			! [DS] move program counter to next bytecode
	
	dec	THREE_BYTE, %pcounter_r			! move the program counter back if branch not taken
	ldsb	[%pcounter_r + BYTE], %op1_r		! load the high 8 bits of offset
	ldub	[%pcounter_r + TWO_BYTE], %op2_r	! load the low 8 bits of offset

	sll	%op1_r, BYTE_BITS, %op1_r		! shift the high 8 bits into bits 8-15
	or	%op1_r, %op2_r, %op1_r			! combine the high and low bits
	
	ba	top					! back to start
	add	%op1_r, %pcounter_r, %pcounter_r	! [DS] add the offset to the program counter

if_icmpge:						! branches if (2nd from top) >= (top)
	ld	[%stkptr_r], %op1_r			! pop the first int from top of the stack	
	inc	LEN, %stkptr_r
	ld	[%stkptr_r], %op2_r			! pop the 2nd integer
	inc	LEN, %stkptr_r
	
	cmp	%op1_r, %op2_r				! compare the two values
	bl	top					! if (2nd from top) < (top), back to start
	inc	THREE_BYTE, %pcounter_r			! [DS] move program counter to next bytecode
	
	dec	THREE_BYTE, %pcounter_r			! move the program counter back if branch not taken
	ldsb	[%pcounter_r + BYTE], %op1_r		! load the high 8 bits of offset
	ldub	[%pcounter_r + TWO_BYTE], %op2_r	! load the low 8 bits of offset

	sll	%op1_r, BYTE_BITS, %op1_r		! shift the high 8 bits into bits 8-15
	or	%op1_r, %op2_r, %op1_r			! combine the high and low bits
	
	ba	top					! back to start
	add	%op1_r, %pcounter_r, %pcounter_r	! [DS] add the offset to the program counter

!***************************************************************************************************
! Program end
!***************************************************************************************************

success:
	ba	quit					! return 0 for successful execution
	clr	%bytecode_r				! [DS]
	
							! Error: division by zero 
div_error:
        set     div_by_zero, %o0
        call    printf                          	! print an error msg and exit
        nop

	mov	DIV_ERR, %bytecode_r			! return error value

quit:   ret						! exit the function
        restore
