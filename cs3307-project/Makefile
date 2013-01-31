INCDIRS=-I/gaul/special/cs3307/cs3307/Boost/boost_1_45_0
LFLAGS=-L/gaul/special/cs3307/cs3307/Boost/boost_1_45_0/bin.v2/libs/program_options/build/gcc-4.1.2/release/link-static/threading-multi/ -lboost_program_options
CFLAGS=-Wall -Wshadow -Wredundant-decls $(INCDIRS)

BIN=asn1

# As you complete each class, uncomment the class below to ensure it is compiled
# and linked with your main program
CLASSES_IMPLEMENTED=\
	Document.o \
	WordIterator.o \
	UppercaseFilter.o \
	ROT13Filter.o \
	PirateFilter.o

asn1: main.cpp $(CLASSES_IMPLEMENTED)
	g++ $(CFLAGS) -o $(BIN) $^ $(LFLAGS) 

Document.o: Document.cpp Document.h DocumentFilter.h WordIterator.h
	g++ $(CFLAGS) -c $<

WordIterator.o: WordIterator.cpp WordIterator.h
	g++ $(CFLAGS) -c $<

UppercaseFilter.o: UppercaseFilter.cpp UppercaseFilter.h DocumentFilter.h
	g++ $(CFLAGS) -c $<

ROT13Filter.o: ROT13Filter.cpp ROT13Filter.h DocumentFilter.h
	g++ $(CFLAGS) -c $<

PirateFilter.o: PirateFilter.cpp PirateFilter.h DocumentFilter.h
	g++ $(CFLAGS) -c $<

clean: 
	rm -f *.o

realclean: clean
	rm -f $(BIN)
