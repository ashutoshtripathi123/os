CC = powerpc-e600-linux-uclibc-cc
AR = powerpc-e600-linux-uclibc-ar
GCOV = powerpc-e600-linux-uclibc-gcov

CCFLAGS =	-O0 -g  -fmessage-length=0 -static -fprofile-arcs -ftest-coverage 

ARFLAGS = 	rcs

LDFLAGS =   -lrt -lpthread -lgcov

SOURCE =	code.c

#LIBS =		libgetsynctime.a

TARGET =	code

$(TARGET):	code.c $(LIBS)
	$(CC) $(CCFLAGS) -o $(TARGET) code.c  $(LDFLAGS)

.PHONY: clean
.PHONY: copy
.PHONY: all
.PHONY: coverage

all:	$(TARGET)

copy: code
	cp code /cygdrive/c/transfer/.

clean:
	rm -f $(TARGET) *.gcda *.gcno *.gcov

code.o: code.c	
	$(CC) $(CXXFLAGS) -c code.c

coverage:
	chmod 600 code.gcda
	$(GCOV) $(TARGET)
