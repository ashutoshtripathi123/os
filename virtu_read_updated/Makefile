CC = powerpc-e600-linux-uclibc-cc
AR = powerpc-e600-linux-uclibc-ar
GCOV = powerpc-e600-linux-uclibc-gcov

CCFLAGS =	-O0 -g  -fmessage-length=0 -static -fprofile-arcs -ftest-coverage 

ARFLAGS = 	rcs

LDFLAGS =   -lrt -lpthread -lgcov

SOURCE =	virtu_read.c

#LIBS =		libgetsynctime.a

TARGET =	virtu_read

INC_DIR = .

$(TARGET):	virtu_read.c  $(LIBS)
	$(CC) $(CCFLAGS) -o $(TARGET) virtu_read.c inputParser.c calcUnits.c controlManager.c findFault.c $(LDFLAGS) -I$(INC_DIR)

.PHONY: clean
.PHONY: copy
.PHONY: all
.PHONY: coverage

all:	$(TARGET)

copy: virtu_read
	cp virtu_read /cygdrive/c/transfer/.

clean:
	rm -f $(TARGET) *.gcda *.gcno *.gcov

virtu_read.o: virtu_read.c	
	$(CC) $(CXXFLAGS) -c virtu_read.c

coverage:
	chmod 600 virtu_read.gcda
	$(GCOV) $(TARGET)
