#include <iostream>
#include <errno.h>
#include <limits.h>
#include <sstream>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include "httpd.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */

using namespace std;

void start_httpd(unsigned short port, string doc_root)
{
	cerr << "Starting server (port: " << port <<
		", doc_root: " << doc_root << ")" << endl;

	stringstream ss;
	ss << port;
	const char *service = ss.str().c_str();

	// Create socket for incoming connections
	int servSock = SetupTCPServerSocket(service);
	if (servSock < 0)
		DieWithUserMessage("SetupTCPServerSocket() failed", service);

	cout << service << endl;
	for (;;) /* Run forever */
	{
        // Create separate memory for client argument
        struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(
                sizeof(struct ThreadArgs));
        if (threadArgs == NULL)
            DieWithSystemMessage("malloc() failed");

		/* Wait for a client to connect */
		cout << "waiting for connecting..." << endl;
		int clntSock = AcceptTCPConnection(servSock);

        threadArgs->clntSock = clntSock;
		threadArgs->root = doc_root;

        // Create client thread
		cout << "creating thread..." << endl;
        pthread_t threadID;
        int returnValue = pthread_create(&threadID, NULL, thread_main, threadArgs);

        if (returnValue != 0)
            DieWithUserMessage("pthread_create() failed", strerror(returnValue));

        printf("with thread %ld\n", (long int) threadID);

	}
	/* NOT REACHED */
}

/* The thread that gets spawned whenever a client connects. */
void *thread_main(void *args)
{
    // Guarantees that thread resources are deallocated upon return
    pthread_detach(pthread_self());

    // Extract socket file descriptor from argument
    int clntSock = ((struct ThreadArgs *) args)->clntSock;
	string doc_root = ((struct ThreadArgs *) args)->root;
    free(args); // Deallocate memory for argument

    HandleTCPClient(clntSock, doc_root);

    return (NULL);
}


int SetupTCPServerSocket(const char *service) {
	// Construct the server address structure
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = PF_INET;
	addrCriteria.ai_flags = AI_PASSIVE;
	addrCriteria.ai_socktype = SOCK_STREAM;
	addrCriteria.ai_protocol = IPPROTO_TCP;

	struct addrinfo *servAddr; // List of server addresses
	int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
	if (rtnVal != 0)
		DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

	int servSock = -1;
	for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
		// Create a TCP socket
		servSock = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
		if (servSock < 0)
			continue;		// Socket creation failed; try next address
		// Bind to the local address and set socket to list
		if ((bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) == 0) && (listen(servSock, MAXPENDING) == 0)) {
			// Print local address of socket
			struct sockaddr_storage localAddr;
			socklen_t addrSize = sizeof(localAddr);
			if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0)
				DieWithSystemMessage("getsockname() failed");
			fputs("Binding to ", stdout);

			// PrintSocketAddress not implemented ...
			// PrintSocketAddress((struct sockaddr *) &localAddr, stdout);

			fputc('\n', stdout);
			break;		// Bind and list successful
		}
		close(servSock);
		servSock = -1;		// Close and try again
	}
	cout << "Binding successfully!" << endl;
	// Free address list allocated by getaddrinfo()
	freeaddrinfo(servAddr);
	return servSock;
}


int AcceptTCPConnection(int servSock) {
	struct sockaddr_in clntAddr; // Client address
	// Set length of client address structure (in-out parameter)
	socklen_t clntAddrLen = sizeof(clntAddr);
	// Wait for a client to connect
	int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
	if (clntSock < 0)
		DieWithSystemMessage("accept() failed");

	/* clntSock is connected to a client! */
	printf("Connected to client %s\n", inet_ntoa(clntAddr.sin_addr));
	fputc('\n', stdout);
	return clntSock;
}

