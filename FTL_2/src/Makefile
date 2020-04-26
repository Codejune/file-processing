# 제공되는 Makefile은 정확하게 동작하지 않을 수 있으므로
# 환경에 맞게 수정해서 사용하기 바랍니다.


############## SunOS ########################
CC       = gcc -g
CCOPTS   = -c #-Wall
LINK     = gcc -g

DEPEND  = makedepend

LINKOPTS = -lm -lnsl
#############################################

SOURCES = ftlmgr.c fdevicedriver.c main.c

OBJECTS = ftlmgr.o fdevicedriver.o main.o

.SUFFIXES : .C .c .o

.c.o:
	$(CC) $(CCOPTS) $<

all: sectormapftl

sectormapftl: $(OBJECTS)
	$(LINK) -o sectormapftl $(OBJECTS) $(LINKOPTS)

clean:
	rm *.o sectormapftl core

depend : $(SOURCES)
	$(DEPEND) $(DEFINES) -I/usr/local/lib/g++-include $(SOURCES)

# DO NOT DELETE
