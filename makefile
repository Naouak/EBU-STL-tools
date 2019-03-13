NAME=EBU-STL-Tools
VPATH=src:source
VERSION=1.0.0
VERSION_NUMBER := -D'VERSION_NUMBER="$(VERSION)"'
PACKAGE=$(NAME)-$(VERSION)

all: ebu_shift ebu_dump ebu_trim ebu_30to25 ebu_from_srt ebu_rename

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

ebu_from_srt: ebu_from_srt.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/ebu_from_srt ebu_from_srt.o ebu.o string_utils.o -lm

ebu_rename: ebu_rename.o ebu.o string_utils.o
	mkdir -p bin
	gcc $(VERSION_NUMBER) -std=c99 -o bin/ebu_rename ebu_rename.o ebu.o string_utils.o -lm

%.o: %.c
	gcc $(VERSION_NUMBER) -std=c99 -c $^ -lm

install: all
	mkdir -p $(DESTDIR)/usr/bin
	cp bin/* $(DESTDIR)/usr/bin

uninstall:
	rm -f $(DESTDIR)/usr/bin/ebu_30to25
	rm -f $(DESTDIR)/usr/bin/ebu_trim
	rm -f $(DESTDIR)/usr/bin/ebu_dump
	rm -f $(DESTDIR)/usr/bin/ebu_shift
	rm -f $(DESTDIR)/usr/bin/ebu_from_srt
	rm -f $(DESTDIR)/usr/bin/ebu_rename

dist: clean
	mkdir $(PACKAGE)
	cp -rpf source $(PACKAGE)/
	cp README.markdown $(PACKAGE)/
	cp LICENSE $(PACKAGE)/
	cp makefile $(PACKAGE)/
	tar -pcJf $(PACKAGE).tar.xz $(PACKAGE)/
	rm -rf $(PACKAGE)

clean:
	rm -f *.o
	rm -f bin/*
