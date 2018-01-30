VPATH=src:source
VERSION_NUMBER := -D'VERSION_NUMBER="$(shell date +%Y-%m-%d\ %H:%M:%S)"'

all: ebu_shift ebu_dump ebu_trim ebu_30to25 srt ebu_rename

ebu_30to25: ebu_30to25.o ebu.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/ebu_30to25 ebu_30to25.o ebu.o string_utils.o -lm

ebu_trim: ebu_trim.o ebu.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/ebu_trim ebu_trim.o ebu.o string_utils.o -lm

ebu_dump: ebu_dump.o ebu.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/ebu_dump ebu_dump.o ebu.o string_utils.o -lm

ebu_shift: ebu_shift.o ebu.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/ebu_shift ebu_shift.o ebu.o string_utils.o -lm

srt: srt.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/srt srt.o ebu.o string_utils.o -lm

ebu_rename: ebu_rename.o ebu.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/ebu_rename ebu_rename.o ebu.o string_utils.o -lm

%.o: %.c
	gcc $(VERSION_NUMBER) -std=c99 -c $^ -lm

clean:
	rm *.o
	rm bin/*
