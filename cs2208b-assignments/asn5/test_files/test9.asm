################################################################################
#                                                                              #
# test9.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: April 1, 2012                                                  #
#                                                                              #
# Tests the 'jmp' synthetic instruction.                                       #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test_files/test9.asm                                       #
# obelix[13]% ./cpu test9                                                      #
#                                                                              #
################################################################################

        # Start data segment
       	.data
_hello: .str "Hello\n"
_bye:  	.str "Goodbye\n"
_bad:	.str "You should not see this if jmp is working properly\n"

        # Start text segment
        .text
_main:  mov	_hello, %r1	# Print a message
	prs

	jmp	_done

	mov	_bad, %r1	# Print another message
	prs

_done:	mov	_bye, %r1	# Print yet another message
	prs

