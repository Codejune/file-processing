ftlmgr : fdevicedriver.o ftlmgr.o
	gcc -o ftlmgr fdevicedriver.o ftlmgr.o

ftlmgr.o : ftlmgr.c flash.h
	gcc -c ftlmgr.c

fdevicedriver.o: fdevicedriver.c flash.h
	gcc -c fdevicedriver.c

clean:
	rm *.o
	rm ftlmgr
