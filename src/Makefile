CC=g++
CFLAGS=-ggdb -std=c++11 -Wall -pedantic

H_FILES=httpd.h
FILES=main.cpp httpd.cpp DieWithMessage.c HandleTCPClient.c HttpFramer.cpp HeadParser.cpp -lpthread

all: httpd

httpd: $(FILES) $(H_FILES)
	$(CC) $(CFLAGS) -o httpd $(FILES)

clean:
	rm -rf httpd
