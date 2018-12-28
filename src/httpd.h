#ifndef HTTPD_H
#define HTTPD_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <string>
//#include "CalcServer.h"

using namespace std;

struct ThreadArgs { // Structure of arguments to pass to client thread
	int clntSock;
	string root;
};

void start_httpd(unsigned short port, string doc_root);

void *thread_main(void *args);

//void Server(unsigned short echoServPort, string doc_root);

void HandleTCPClient(int clntSocket, string doc_root);   /* TCP client handling function */

int SetupTCPServerSocket(const char *service);

int AcceptTCPConnection(int servSock);  /* Accept TCP connection request */

//void DieWithError(const char *errorMessage);  /* Error handling function */

void DieWithUserMessage(const char *msg, const char *detail);	// Handle error with user msg

void DieWithSystemMessage(const char *msg);	// Handle error with sys msg

#endif // HTTPD_H
