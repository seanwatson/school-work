################################################################################
#                                                                              #
# test7.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: April 1, 2012                                                  #
#                                                                              #
# Tests the 'or' synthetic instruction.                                        #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test_files/test7.asm                                       #
# obelix[13]% ./cpu test7                                                      #
#                                                                              #
################################################################################

        # Start data segment
        .data
_msg:   .str "0x4 OR 0x8 = "
_msg2:  .str "0x0 OR 0x4 = "

        # Start text segment
        .text
_main:  mov	_msg, %r1	# Print a message
	prs

	mov	4, %r1		# Store the two test values
	mov	8, %r2

	or	%r1, %r2, %r1	# OR them
	pri			# and print the result

	mov	'\n', %r1	# Print a new line character
	prc

	mov	_msg2, %r1	# Print a message
	prs

	or	%r0, 4, %r1	# OR the second set of test values
	pri			# and print the result

	mov	'\n', %r1	# Print a new line character
	prc
