################################################################################
#                                                                              #
# Makefile                                                                     #
#                                                                              #
# Author: Jeff Shantz <x@y, x = jshantz4, y = csd.uwo.ca>                      #
# Last Updated: March 29, 2012                                                 #
#                                                                              #
# Builds the plugins for the 2208 Western CPU Assembler.                       #
# Use the Makefile in the parent directory to build your plugins.              #
#                                                                              #
# Note that if you're working on a Mac, you'll need to see the uncomment the   #
# Mac settings where directed below.                                           #
#                                                                              #
################################################################################

MACPLUGINS= lib_ins_rdc.dylib \
            lib_ins_rdi.dylib \
            lib_ins_prc.dylib \
            lib_ins_pri.dylib \
            lib_ins_prs.dylib \
            lib_ins_mov.dylib \
#           lib_ins_not.dylib \
#           lib_ins_neg.dylib \
#           lib_ins_sub.dylib \
#           lib_ins_or.dylib \
#           lib_ins_and.dylib \
#           lib_ins_jmp.dylib \
#           lib_ins_jz.dylib
#           lib_ins_jn.dylib

UNIXPLUGINS=lib_ins_rdc.so \
            lib_ins_rdi.so \
            lib_ins_prc.so \
            lib_ins_pri.so \
            lib_ins_prs.so \
            lib_ins_mov.so \
            lib_ins_not.so \
            lib_ins_neg.so \
 	    lib_ins_sub.so \
            lib_ins_or.so \
            lib_ins_and.so \
            lib_ins_jmp.so \
            lib_ins_jz.so \
 	    lib_ins_jn.so

COPTS=-Wall -I ../include -fPIC

# If you're working on obelix, use the following:
PLUGINS=$(UNIXPLUGINS)
LDALTEXEC=/usr/sfw/bin/gld

# Otherwise, if you're working on a Mac, uncomment the following:
#PLUGINS=$(MACPLUGINS)
#LDALTEXEC=

plugins: assembler_plugin.o $(PLUGINS)

%.so: %.o assembler_plugin.o
	LD_ALTEXEC=$(LDALTEXEC) gcc -o $@ -shared -exported_symbols_list,exportlist.txt -Wl,-soname,$@ -Wl,-undefined,dynamic_lookup $^

%.dylib: %.o assembler_plugin.o
	gcc -o $@ -dynamiclib $^

%.o: %.c
	 gcc $(COPTS) -c $^

clean:
	rm -f $(PLUGINS) *.o
