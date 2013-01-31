################################################################################
#                                                                              #
# test0.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: March 29, 2012                                                 #
#                                                                              #
# This is a test program that does nothing.  You can build the provided        #
# assignment code and then run this program on it to see the default behaviour #
# of the provided code:                                                        #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm test0.asm                                                  #
#                                                                              #
################################################################################

        # Start data segment
        .data
_msg:   .str "Hello world\n"
_val:   .word 0xAABBCCDD

        # Start text segment
        .text
_main:  nor %r0, %r0, %r1
        add %r1, %r0, %r1
        tst %r2

