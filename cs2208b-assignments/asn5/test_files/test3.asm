################################################################################
#                                                                              #
# test3.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: March 30, 2012                                                 #
#                                                                              #
# Tests that the do_align function is working properly.                        #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm --debug test3.asm                                          #
#                                                                              #
# The assembler will generate the file 'test3', which is the executable file.  #
# You can check this file with 'od'.                                           #
#                                                                              #
################################################################################

        # Start data segment
        .data
_val1:  .byte 0xAA
	.align 2
_val2:  .half 0xCAFE
_val3:  .byte 0xBB
_val4:  .byte 0xCC
        .align 4
_val5:  .word 0xDEADBEEF
        .align 4
_msg:   .str "Hello\n"

        # Start text segment
        .text
_main:  add %r0, 1, %r1
        add %r1, 1, %r1
	add %r1, 1, %r1
_last:  add %r1, 1, %r1

