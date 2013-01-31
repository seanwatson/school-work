####################################################################
#
# File:		sort.m
# Author:	Sean Watson
# Last Updated:	April 8, 2012
#
# Reads 10 integers from stdin, sorts them using a
# bubble sort algorithm, then displays the integers
# from least to greatest
#
# Register Definitions:
#	%tmp1_r		For temporary storage of calculations	%r1
#	%tmp2_r		For temporary storage of calculations	%r2
#	%tmp3_r		For temporary storage of calculations	%r3
#	%counter_r	Counter for loops			%r4
#	%end_r		The last element to check for swapping	%r5
#	%new_end_r	The last element to check next loop	%r6
#	%arr_ptr_r	A pointer to the array			%r7
#
####################################################################

####################################################################
# CONSTANTS
####################################################################

define(SIZE, 10)	# size of the array
define(LAST, 9)		# last element of the array
define(WORD, 4)		# size of a word in bytes
define(X_FOUR, 2)	# shift amount to multiply by four
define(NEW_LN, '\n')	# new line character

####################################################################
# REGISTERS
####################################################################

define(tmp1_r, r1)
define(tmp2_r, r2)
define(tmp3_r, r3)
define(counter_r, r4)
define(end_r, r5)
define(new_end_r, r6)
define(arr_ptr_r, r7)

####################################################################
# STATIC VARIABLES
####################################################################

	.data
	
_arr:	.word 0				# arrary for the data elements
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0

_prmt:	.str  "Enter the next value: "	# prompt for input

####################################################################
# MAIN PROGRAM
####################################################################

	.text

_main:	mov	SIZE, %counter_r		# initialize the counter
	mov	_arr, %arr_ptr_r		# initialize the array pointer
	mov	LAST, %end_r			# initialize the search end

_input:	sub	%counter_r, 1, %counter_r	# decrement the counter
	tst	%counter_r			# jump once 10 ints read
	jn	_sort

	mov	_prmt, %tmp1_r			# prompt for a new integer
	prs

	rdi					# read the integer
	stw	%tmp1_r, [%arr_ptr_r]		# store it in the array
	add	%arr_ptr_r, WORD, %arr_ptr_r	# move pointer to next location
	
	jmp	_input				# read another integer

_sort:	mov	_arr, %arr_ptr_r		# reset the array pointer
	
_loop:	mov	%r0, %counter_r			# reset the counter
	mov	%r0, %new_end_r			# initialize the new search end

_for:	add	%counter_r, 1, %counter_r	# increment the counter	
	sub	%end_r, %counter_r, %tmp1_r	# check if its the end of the array
	tst	%tmp1_r				# break the loop if its the end
	jn	_next

	sll	%counter_r, X_FOUR, %tmp3_r	# times the counter by four to get offset	
	ldw	[%arr_ptr_r + %tmp3_r], %tmp2_r # load the first integer to compare

	sub	%tmp3_r, WORD, %tmp3_r		# decrement the index offset
	ldw	[%arr_ptr_r + %tmp3_r], %tmp1_r	# load the second integer to compare

	sub	%tmp1_r, %tmp2_r, %tmp3_r	# compare the two values
	tst	%tmp3_r				# if they are in the right order jump
	jn	_for

	sll	%counter_r, X_FOUR, %tmp3_r	# times the counter by four to get offset
	stw	%tmp1_r, [%arr_ptr_r + %tmp3_r]	# swap the first value

	sub	%tmp3_r, 4, %tmp3_r		# decrement the index offset
	stw	%tmp2_r, [%arr_ptr_r + %tmp3_r]	# swap the second value

	mov	%counter_r, %new_end_r		# save the current location as the new end
	sub	%new_end_r, 1, %new_end_r	# search up to the location before the last swap
	jmp	_for				# repeat the loop

_next:	tst	%new_end_r			# check if the array is fully sorted
	jz	_done

	mov	%new_end_r, %end_r		# do another iteration up to the new end
	jmp 	_loop

_done:	mov	SIZE, %counter_r		# initalize the counter

_print:	sub	%counter_r, 1, %counter_r	# decrement the counter
	tst	%counter_r			# check if all elements have been printed
	jn	_quit

	ldw	[%arr_ptr_r], %tmp1_r		# load the next int to print
	pri					# print the integer

	mov	NEW_LN, %tmp1_r			# print a new line
	prc

	add	%arr_ptr_r, WORD, %arr_ptr_r	# increment the array pointer
	jmp	_print				# print the next integer
_quit:						# terminate the program
