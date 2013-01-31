################################################################################
#                                                                              #
# test5.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: April 1, 2012                                                  #
#                                                                              #
# Tests the 'neg' synthetic instruction.                                       #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test_files/test5.asm                                       #
# obelix[13]% ./cpu test5                                                      #
#                                                                              #
################################################################################

        # Start data segment
        .data
_msg:   .str "Enter the value to negate: "
_msg2:  .str "Enter another value to negate: "

        # Start text segment
        .text
_main:  mov	_msg, %r1	# Print a prompt
	prs

	rdi			# Read an integer
	mov	%r1, %r2	# and store it

	mov	_msg2, %r1	# Print a prompt
	prs
	
	rdi			# Read another integer
	mov	%r1, %r3	# and store it

	neg	%r2, %r1	# Negate the first integer
	pri			# and print it

	mov	'\n', %r1	# Print a new line character
	prc

	neg	%r3		# Negate the second integer
	mov	%r3, %r1	# and print it
	pri
	
	mov	'\n', %r1	# Print a new line character
	prc
