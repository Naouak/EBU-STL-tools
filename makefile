VPATH=src:source

all: ebu_shift ebu_dump ebu_trim ebu_30to25

ebu_30to25: ebu_30to25.o ebu.o
	mkdir -p bin
	gcc -o bin/ebu_30to25 ebu_30to25.o ebu.o

ebu_trim: ebu_trim.o ebu.o
	mkdir -p bin
	gcc -o bin/ebu_trim ebu_trim.o ebu.o

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
