################################################################################
#                                                                              #
# test6.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: April 1, 2012                                                  #
#                                                                              #
# Tests the 'sub' synthetic instruction.                                       #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test_files/test6.asm                                       #
# obelix[13]% ./cpu test6                                                      #
#                                                                              #
################################################################################

        # Start data segment
        .data
_msg:   .str "Operand 1: "
_msg2:  .str "Operand 2: "
_res:	.str "Result: "
_res2:  .str "FYI, 5 - 2 = "

        # Start text segment
        .text
_main:  mov	_msg, %r1	# Prompt the user for operand 1
	prs

	rdi			# Read an integer
	mov	%r1, %r2	# and store it

	mov	_msg2, %r1	# Prompt the user for operand 2
	prs
	
	rdi			# Read another integer
	mov	%r1, %r3	# and store it

	mov	_res, %r1	# Print the first result message
	prs

	sub	%r2, %r3, %r1	# Subtract the two operands
	pri			# and print the result

	mov	'\n', %r1	# Print a new line character
	prc

	mov	_res2, %r1	# Print the second result message
	prs

	mov	5, %r2		# Subtract 5 - 2
	sub	%r2, 2, %r1	
	pri			# and print the result

	mov	'\n', %r1	# Print a new line character
	prc
