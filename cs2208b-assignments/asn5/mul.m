########################################################################
#
# File:		mul.m
# Author:	Sean Watson
# Last Updated:	April 8, 2012
#
# Implements multipulcation on the Western2208. Reads
# two integers from stdin and multiplies them using the
# algorithm from topic 4. It then outputs the result to
# stdout
#
# Register Legend:
#	%in_out_r	Input/output register	%r1
#	%mc_r		Multiplicand register	%r2
#	%pr_r		Product register	%r3
#	%mp_r		Multiplier register	%r4
#	%counter_r	Counter register	%r5
#	%tmp_r		For temporary storage	%r6
#
########################################################################

########################################################################
# Constants
########################################################################

define(REG_SIZE, 32)	# size of a Western2208 register
define(BIT, 1)		# size of a bit in bits
define(LSB, 1)		# mask for LSB
define(MSB, 31)		# shift amount to move LSB to MSB
define(NEW_LN, '\n')	# new line character

########################################################################
# Registers
########################################################################

define(in_out_r, r1)
define(mc_r, r2)
define(pr_r, r3)
define(mp_r, r4)
define(counter_r, r5)
define(tmp_r, r6)

########################################################################
# Static Variables
########################################################################

	.data

_prmt1:	.str "Operand 1: "	# prompt for 1st operand
_prmt2:	.str "Operand 2: "	# prompt for 2nd operand

#########################################################################
# Main Program
#########################################################################

	.text

_main:	mov	_prmt1, %in_out_r 		# prompt for 1st operand
	prs

	rdi
	mov	%in_out_r, %mc_r 		# store the multiplicand

	mov	_prmt2, %in_out_r		# prompt for 2nd operand
	prs

	rdi
	mov	%in_out_r, %mp_r		# store the multiplier

	mov	%r0, %pr_r			# clear the product
	mov	REG_SIZE, %counter_r		# initialize the counter

_loop:	sub	%counter_r, 1, %counter_r	# decrement the counter
	tst	%counter_r			# check if all bits processed
	jn	_done

	and	%mp_r, LSB, %tmp_r		# check if the LSB is a 1
	tst	%tmp_r				# if it is 0 dont add
	jz	_shift

	add	%mc_r, %pr_r, %pr_r		# add the mc to the pr

_shift:	srl	%mp_r, BIT, %mp_r		# shift the mp one bit

	and	%pr_r, LSB, %tmp_r		# check if the pr LSB is 1
	tst	%tmp_r				# if not, just shift it
	jz	_label

	mov	LSB, %tmp_r			# set the MSB of mp to 1
	sll	%tmp_r, MSB, %tmp_r
	or	%tmp_r, %mp_r, %mp_r

_label:	sra	%pr_r, BIT, %pr_r		# shift the pr one bit
	jmp	_loop				# repeat the loop

_done:	mov	%mp_r, %in_out_r		# print the result
	pri

	mov	NEW_LN, %in_out_r		# print a new line
	prc
