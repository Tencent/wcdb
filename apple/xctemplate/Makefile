#!/usr/make

DESTINATION = ~/Library/Developer/Xcode/Templates/File\ Templates/WCDB
TEMPLATES = ~/Library/Developer/Xcode/Templates
FILE_TEMPLATES = ~/Library/Developer/Xcode/Templates/File\ Templates
SOURCE = ./WCDB/* ./WCDB/**/* ./WCDB/**/**/*
SOURCE_DIR = ./WCDB

all: $(DESTINATION)
$(DESTINATION): $(SOURCE)
	if [ -f $(TEMPLATES) ] ; \
	then \
		rm $(TEMPLATES); \
	fi;
	rm -fR $(DESTINATION)
	mkdir -p $(FILE_TEMPLATES)
	cp -R $(SOURCE_DIR) $(FILE_TEMPLATES)
clean:
