! ***********************************************************************************************************
! Sean Watson
! Assignment #3
! File: get_b64_char.m
!
! get_b64_char -
!	A function which takes a 6 bit index value and returns
!	the corresponding base 64 encoded character.
!
! Calling Sequence:
!	- The index value to be converted is passed in %o0
!
! Returns:
!	- The encoded character's ASCII value in %o0
!
! Register Legend:
!	%index_r	The 6 bit index value to be encoded	%i0
!	
!************************************************************************************************************

!************************************************************************************************************
! Constants
!************************************************************************************************************

define(LOW_SIX_BITS, 0x3F)	! low 6 bits are all ones, used for masking off the low 6 bits

define(CAPITAL_OFFSET, 65)	! offset to convert from an index to a capital letter
define(LOWER_OFFSET, 71)	! offset to convert from an index to a lowercase letter
define(NUMBER_OFFSET, 4)	! offset to convert from an index to a number
define(PLUS_OFFSET, 19)		! offset to convert from an index to a plus sign
define(BACKSLASH_OFFSET, 16)	! offset to convert from an index to backslash

define(CAPITAL_INDEX, 25)	! used to check if the index is a capital letter
define(LOWER_INDEX, 51)		! used to check if the index is a lowercase letter
define(NUMBER_INDEX, 61)	! used to check if the index is a number
define(PLUS_INDEX, 62)		! used to check if the index is a plus sign

!************************************************************************************************************
! Registers
!************************************************************************************************************

define(index_r, i0)

!************************************************************************************************************
! get_b64_char Function
!************************************************************************************************************

	.global get_b64_char

get_b64_char:
	save	%sp, -96, %sp				! function begins

	and	%index_r, LOW_SIX_BITS, %index_r	! mask the lowest 6 bits off of the index

	cmp	%index_r, CAPITAL_INDEX			! check if the index represents a capital letter
	ble	capital_letter				! branch if the index represents a capital letter
	nop

	cmp	%index_r, LOWER_INDEX			! check if the index represents a lowercase letter
	ble	lowercase_letter			! branch if the index represents a lowercase letter
	nop
	
	cmp	%index_r, NUMBER_INDEX			! check if the index represents a number
	ble	number					! branch if the index represents a number
	nop
	
	cmp	%index_r, PLUS_INDEX			! check if the index represents a plus sign
	be	plus_sign				! branch if the index represents a plus sign
	nop
	
	ba	backslash				! if this part of the code is reached, the index represents '/'
	nop

capital_letter:
	add	%index_r, CAPITAL_OFFSET, %index_r	! convert the index to the ASCII value it represents
	ret						! terminate the program and return the character
	restore

lowercase_letter:
	add	%index_r, LOWER_OFFSET, %index_r	! convert the index to the ASCII value it represents
	ret						! terminate the program and return the character
	restore
	
number:	sub	%index_r, NUMBER_OFFSET, %index_r	! convert the index to the ASCII value it represents
	ret						! terminate the program and return the character
	restore
	
plus_sign:
	sub	%index_r, PLUS_OFFSET, %index_r		! convert the index to a plus sign
	ret						! terminate the program and return the character
	restore

backslash:
	sub	%index_r, BACKSLASH_OFFSET, %index_r	! convert the index to a backslash
	ret						! terminate the program and return the character
	restore
