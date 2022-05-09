CC=/opt/cross-br-2022.02/bin/arm-linux-g++
LD:= $(CC)

CLS=test_drvTest


all:
	$(CC) -c -g -o $(CLS).o $(CLS).c
	$(LD) -o $(CLS).out $(CLS).o

clean:
	$(RM) $(CLS).o $(CLS).out
