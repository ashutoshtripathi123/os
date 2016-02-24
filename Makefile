CC = powerpc-e600-linux-uclibc-cc
AR = powerpc-e600-linux-uclibc-ar
GCOV = powerpc-e600-linux-uclibc-gcov

CCFLAGS =	-O0 -g  -fmessage-length=0 -static -fprofile-arcs -ftest-coverage 

ARFLAGS = 	rcs

LDFLAGS =   -lrt -lpthread -lgcov

SOURCE =	newApproach.c

#LIBS =		libgetsynctime.a

TARGET =	newApproach

$(TARGET):	newApproach.c $(LIBS)
	$(CC) $(CCFLAGS) -o $(TARGET) newApproach.c  $(LDFLAGS)

.PHONY: clean
.PHONY: copy
.PHONY: all
.PHONY: coverage

all:	$(TARGET)

copy: newApproach
	cp newApproach /cygdrive/c/transfer/.

clean:
	rm -f $(TARGET) *.gcda *.gcno *.gcov

newApproach.o: newApproach.c	
	$(CC) $(CXXFLAGS) -c newApproach.c

coverage:
	chmod 600 newApproach.gcda
	$(GCOV) $(TARGET)
