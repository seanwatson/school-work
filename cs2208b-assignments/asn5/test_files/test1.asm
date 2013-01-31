################################################################################
#                                                                              #
# test1.asm                                                                    #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last updated: March 30, 2012                                                 #
#                                                                              #
# Prompts the user for two numbers, adds them, and prints the result.          #
# This file can be used to test that load_plugins() is working properly.       #
# If so, when you run your assembler in debug mode, you should see that all    #
# provided plugins are loaded and you should not get any "Unknown instruction" #
# errors.  Don't forget to set your LD_LIBRARY_PATH properly.                  #
#                                                                              #
# obelix[11]% make                                                             #
# obelix[12]% ./asm --debug test1.asm                                          #
#                                                                              #
################################################################################
_main:  mov '>', %r1      # Print a prompt 
	prc

	rdi		  # Read an integer into %r1
        mov %r1, %r2      # and store it

        mov '>', %r1      # Print a prompt 
	prc

	rdi		   # Read an integer into %r1
	add %r1, %r2, %r1  # Add the two
	pri                # Print the result

	mov '\n', %r1     # Print a new line character
	prc
