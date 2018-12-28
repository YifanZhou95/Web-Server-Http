#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */
#include <iostream>
#include <assert.h>
#include <sstream>
#include "httpd.h"
#include "HttpFramer.hpp"
#include "HeadParser.hpp"

#define RCVBUFSIZE 1024576

using namespace std;

void HandleTCPClient(int clntSocket, string doc_root)
{
	// PUT YOUR CODE HERE

	char buffer[RCVBUFSIZE];
	int numBytesRcvd;
	HttpFramer framer;
	HeadParser parser;
	string line_str;
	Request req;
	string result_str;
	string linefeed_str = "\r\n\0";

	bool pipeline_flag = 1; // By default, continue processing next http request

	if ((numBytesRcvd = recv(clntSocket, buffer, RCVBUFSIZE, 0)) < 0)
		DieWithSystemMessage("recv() failed");

	while(numBytesRcvd > 0){
		buffer[numBytesRcvd] = 0;
		string message_str = buffer;
		cout << message_str.length() << endl;
		framer.append(buffer);		
		while(pipeline_flag && framer.hasMessage()) {
			
			string line_str = framer.topMessage();
			framer.popMessage();
			cout << "topMessage" << endl << line_str << endl;
			
			req = parser.parse(line_str);
			pipeline_flag = req.connection;
			geneResponse(clntSocket, req, doc_root);
			cout << "hasMessage" << endl << framer.hasMessage() << endl;
			cout << "pipeline_flag" << endl << pipeline_flag << endl;
		}


		/* See if there is more data to receive */
		numBytesRcvd = recv(clntSocket, buffer, RCVBUFSIZE, 0);
		if (numBytesRcvd < 0)
			DieWithSystemMessage("recv() failed");
	}
	cout << "All Data Received...\n" << endl;


    close(clntSocket);    /* Close client socket */
}

