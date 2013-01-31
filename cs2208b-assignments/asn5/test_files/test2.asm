################################################################################
#                                                                              #
# test2.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: March 30, 2012                                                 #
#                                                                              #
# Tests that the handle_directive function is working properly.                #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm --debug test2.asm                                          #
#                                                                              #
# The assembler will generate the file 'test2', which is the executable file.  #
# You can check this file with 'od'.                                           #
#                                                                              #
################################################################################

        # Start data segment
        .data
_val1:  .byte 0xAA
_val2:  .byte 0xBB
_val3:  .half 0xCAFE
_val4:  .word 0xDEADBEEF
_msg:   .str "Hello\n"

        # Start text segment
        .text
_main:  add %r0, 1, %r1
        add %r1, 1, %r1
	add %r1, 1, %r1
_last:  add %r1, 1, %r1

