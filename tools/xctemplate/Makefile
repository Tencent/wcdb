#!/usr/make

DESTINATION = ~/Library/Developer/Xcode/Templates/File\ Templates/WCDB
TEMPLATES = ~/Library/Developer/Xcode/Templates
FILE_TEMPLATES = ~/Library/Developer/Xcode/Templates/File\ Templates
SOURCE_DIR = `pwd`/WCDB

all:
	if [ -f $(TEMPLATES) ] ; \
	then \
		rm $(TEMPLATES); \
	fi;
	rm -fR $(DESTINATION)
	mkdir -p $(FILE_TEMPLATES)
	ln -s $(SOURCE_DIR) $(DESTINATION)
clean:
