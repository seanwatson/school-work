! ***********************************************************************************************************
! Sean Watson
! Assignment #3
! File: get_b64_str.m
!
! get_b64_str -
!	Takes up to 3 characters in a string as input and returns the equivalent
!	base 64 encoded string.
!
! Calling Sequence:
!	- The characters to be encoded are passed in %o0
!	- The number of characters in the string are passed in %o1
!
! Returns:
!	- A pointer to the equivalent base 64 encoded string in %o0
!
! Register Legend:
!	%input_r	Up to 3 characters stored in bits 23-0		%i0
!	%input_size_r	The number of characters in input_r		%i1
!	%char1_r	The character stored in bits 23-18		%l0
!	%char2_r	The character stored in bits 17-12		%l1
!	%char3_r	The character stored in bits 11-6		%l2
!	%str_ptr_r	A pointer to the start of the output string	%l3
!	%padding_r	Stores the padding character			%l4
!
!************************************************************************************************************

!************************************************************************************************************
! Registers
!************************************************************************************************************

define(input_r, i0)
define(input_size_r, i1)
define(char1_r, l0)
define(char2_r, l1)
define(char3_r, l2)
define(str_ptr_r, l3)
define(padding_r, l4)

!************************************************************************************************************
! get_b64_str Function
!************************************************************************************************************

		.data					! variable declarations

out_str:	.byte 0, 0, 0, 0, 0			! declare a 5 byte static variable to hold the output string
	
		.text					! static declarations
		.global get_b64_str

get_b64_str:	save	%sp, -96, %sp			! function begins
		
		set	out_str, %str_ptr_r		! store a pointer to the start of the output string

		mov	'=', %padding_r			! initialize the padding character register
		stb	%padding_r, [%str_ptr_r + 2]	! pad byte 2 of the output incase it isnt used
		stb	%padding_r, [%str_ptr_r + 3]	! pad byte 3 of the output incase it isnt used

		srl	%input_r, 18, %char1_r		! move the 1st character into the low 6 bits
		srl	%input_r, 12, %char2_r		! move the 2nd character into the low 6 bits
		srl	%input_r, 6, %char3_r		! move the 3rd character into the low 6 bits
		
		mov	%char1_r, %o0			! encode the first character
		call	get_b64_char
		nop
		stb	%o0, [%str_ptr_r]		! store the encoded character in byte 0 of the output

		mov	%char2_r, %o0			! encode the second character
		call	get_b64_char
		nop
		stb	%o0, [%str_ptr_r + 1]		! store the encoded character in byte 1 of the output

		cmp	%input_size_r, 2		! check if there are more characters
		bl	done				! if no more characters, branch to the end
		nop

		mov	%char3_r, %o0			! encode the 3rd character
		call	get_b64_char
		nop
		stb	%o0, [%str_ptr_r + 2]		! store the encoded character in byte 2 of the output

		cmp	%input_size_r, 3		! check if there are more characters
		bl	done				! if no more characters, branch to the end
		nop
		
		mov	%input_r, %o0			! encode the 4th character
		call	get_b64_char
		nop
		stb	%o0, [%str_ptr_r + 3]		! store the encoded character in byte 3 of the output
done:
		set	out_str, %input_r		! return a pointer to the encoded string
		ret					! terminate the function and return the pointer
		restore
