#!/usr/make

SRC = ~/Library/Developer/Xcode/Templates/File\ Templates/WCDB
TEMPLATES = ~/Library/Developer/Xcode/Templates
FILE_TEMPLATES = ~/Library/Developer/Xcode/Templates/File\ Templates
ORIGIN = ./WCDB

all: $(SRC)
$(SRC): $(ORIGIN)
	if [ -f $(TEMPLATES) ] ; \
	then \
		rm $(TEMPLATES); \
	fi;
	rm -fR $(SRC)
	mkdir -p ~/Library/Developer/Xcode/Templates/File\ Templates
	cp -R $(ORIGIN) $(FILE_TEMPLATES)
force:
	rm -fR $(SRC)
	mkdir -p ~/Library/Developer/Xcode/Templates/File\ Templates
	cp -R $(ORIGIN) $(FILE_TEMPLATES)
clean:
