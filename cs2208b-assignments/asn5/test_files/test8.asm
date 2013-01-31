################################################################################
#                                                                              #
# test8.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: April 1, 2012                                                  #
#                                                                              #
# Tests the 'and' synthetic instruction.                                       #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test_files/test8.asm                                       #
# obelix[13]% ./cpu test8                                                      #
#                                                                              #
################################################################################

        # Start data segment
        .data
_msg:   .str "0x4 AND 0xC = "
_msg2:  .str "0x0 AND 0x4 = "

        # Start text segment
        .text
_main:  mov	_msg, %r1	# Print a message
	prs

	mov	4, %r1		# Store the two test values
	mov	0xC, %r2

	and	%r1, %r2, %r1	# AND them
	pri			# and print the result

	mov	'\n', %r1	# Print a new line character
	prc

	mov	_msg2, %r1	# Print a message
	prs

	and	%r0, 4, %r1	# OR the second set of test values
	pri			# and print the result

	mov	'\n', %r1	# Print a new line character
	prc
