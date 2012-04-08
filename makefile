VPATH=src:source

all: ebu_shift

ebu_shift: ebu_shift.o ebu.o
	mkdir -p bin
	gcc -o bin/ebu_shift ebu_shift.o ebu.o
%.o: %.c
	gcc -c $^

clean:
	rm *.o
	rm bin/*.exe
