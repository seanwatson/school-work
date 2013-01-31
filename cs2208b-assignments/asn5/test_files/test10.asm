################################################################################
#                                                                              #
# test10.asm                                                                   #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: April 1, 2012                                                  #
#                                                                              #
# Tests the 'jz' synthetic instruction.                                        #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test_files/test10.asm                                      #
# obelix[13]% ./cpu test10                                                     #
#                                                                              #
################################################################################

        # Start data segment
       	.data
_msg:	.str "The numbers from 5 down to 1 are:\n"

        # Start text segment
        .text
_main:  mov	_msg, %r1	# Print a message
	prs

	mov	5, %r2		# Start the countdown
	
_loop:	mov	%r2, %r1	# Print the next value
	pri

	mov	'\n', %r1	# Print a new line character
	prc

	sub	%r2, 1, %r2	# Decrement the count
	tst	%r2

	jn	_done
	jmp	_loop

_done:

