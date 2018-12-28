#ifndef CALCPARSER_HPP
#define CALCPARSER_HPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <stdlib.h>

using namespace std;


typedef struct Host_t {
    char * hostname;
    char * portNum;
    char * serverPath;
    char * contentType;
} Host;

typedef struct Resquest_t {
	Host host;
    char * method;
    char * userAgent;
	bool connection;
	bool isValid;
    //char * requestHeader;
    //char * requestBody;
} Request;


class HeadParser {
public:
	static Request parse(string insstr);
};

void geneResponse(int clntSocket, Request request, string doc_root);

void sendHeader(int clntSocket, int errorCode, int contentLength, char * contentType);

void sendError(int clntSocket, int errorCode);


#endif // CALCPARSER_HPP
