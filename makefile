VPATH=src:source

all: ebu_shift ebu_dump

ebu_dump: ebu_dump.o ebu.o
	mkdir -p bin
	gcc -o bin/ebu_dump ebu_dump.o ebu.o

ebu_shift: ebu_shift.o ebu.o
	mkdir -p bin
	gcc -o bin/ebu_shift ebu_shift.o ebu.o
%.o: %.c
	gcc -c $^

clean:
	rm *.o
	rm bin/*
