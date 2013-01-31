################################################################################
#                                                                              #
# test4.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: April 1, 2012                                                  #
#                                                                              #
# Tests the 'not' synthetic instruction.                                       #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test4.asm                                                  #
# obelix[13]% ./cpu test4                                                      #
#                                                                              #
################################################################################

        # Start data segment
        .data
_msg:   .str "The 'not' of 0 is: "

        # Start text segment
        .text
_main:  mov	_msg, %r0	# Print the string
	prs

	not	%r0, %r1	# Complement %r0 and store result in %r1 (to get 0xFFFFFFFF)
	not	%r1		# Complement %r1 again (to get 0)
	not	%r1		# Complement %r1 again (to get 0xFFFFFFFF)

	pri			# Print the result (-1)

	mov	'\n', %r1	# Print a new line character
	prc
