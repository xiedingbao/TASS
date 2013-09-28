CXX = g++
CFLAGS =-g -Wall -O3 -I include -I. -I /usr/local/include/libxml2
LINK_FLAGS = 
LINK_EXTRA_FLAGS=-lpthread  -fopenmp -lrt
EXTERNAL_LIB=/usr/lib/libz3.so /usr/local/lib/libxml2.so

all: tass
	@echo 'compiled successfully'

tass: lex.yy.o parser.tab.o System.o process_xml.o tass.o Constraint.o BMSC.o HMSC.o TimeAnalyze.o PathCheck.o 
	$(CXX) $(CFLAGS) -o $@ $^  $(LINK_FLAGS) $(EXTERNAL_LIB) $(LINK_EXTRA_FLAGS)

%.o: %.cpp
	$(CXX)  -c $(CFLAGS)  -o $@ $< 

%.o: %.c
	$(CXX)  -c $(CFLAGS) -o $@ $<

parser.tab.c: parser.y
	bison -d  -v parser.y

lex.yy.c: parser.l parser.tab.c
	flex parser.l

clean: 
	rm -f tass *.o *~ include/*~  parser.tab.c parser.tab.h parser.output lex.yy.c 
