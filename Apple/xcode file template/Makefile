#!/usr/make

SRC = ~/Library/Developer/Xcode/Templates/File\ Templates/WCDB
TEMPLATES = ~/Library/Developer/Xcode/Templates/File\ Templates
ORIGIN = ./WCDB

all: $(SRC)
$(SRC): $(ORIGIN)
	rm -fR $(SRC)
	cp -R $(ORIGIN) $(TEMPLATES)
force:
	rm -fR $(SRC)
	cp -R $(ORIGIN) $(TEMPLATES)
clean:
