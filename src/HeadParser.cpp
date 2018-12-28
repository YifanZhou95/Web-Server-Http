#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h> /* for recv() and send() */
#include <sys/sendfile.h> /* sendfile() */
#include <unistd.h>
#include "HeadParser.hpp"

Request HeadParser::parse(std::string insstr)
{
		Request ret;
		ret.isValid = 0;
		ret.connection = 1;  // keep connecting by default
		char *initial_line;
		char *line;
		int non_sp_position;
		initial_line = strtok((char *)insstr.c_str(), "\r\n");
		line = strtok(NULL, "\r\n");
		while(line){
			string str = line;
			if (str.find("Host:") == 0){
				string hostName_str = str.substr(5);
				if (hostName_str.length()==0)  return ret;
				non_sp_position = hostName_str.find_first_not_of(" ");
				hostName_str = hostName_str.substr(non_sp_position);
				char *hostName_ch = (char *)malloc(sizeof(char)*hostName_str.length());
				strcpy(hostName_ch, hostName_str.c_str());
				ret.host.hostname = (char *)malloc(sizeof(char)*hostName_str.length());
				strcpy(ret.host.hostname, hostName_str.c_str());
				cout << "parsed host name is: " << hostName_ch << endl;
			}
			else if (str.find("User-Agent:") == 0){
				string userAgent_str = str.substr(11);
				if (userAgent_str.length()==0)  return ret;
				non_sp_position = userAgent_str.find_first_not_of(" ");
				userAgent_str = userAgent_str.substr(non_sp_position);
				char *userAgent_ch = (char *)malloc(sizeof(char)*userAgent_str.length());
				strcpy(userAgent_ch, userAgent_str.c_str());
				ret.userAgent = (char *)malloc(sizeof(char)*userAgent_str.length());
				strcpy(ret.userAgent, userAgent_str.c_str());
				cout << "parsed User-Agent name is: " << userAgent_ch << endl;
			}
			else if (str.find("Connection:") == 0){
				string connection_str = str.substr(11);
				if (connection_str.length()==0)  return ret;
				non_sp_position = connection_str.find_first_not_of(" ");
				connection_str = connection_str.substr(non_sp_position);
				bool connection_status = connection_str=="close" ? 0 : 1;
				ret.connection = connection_str=="close" ? 0 : 1;
				cout << "parsed connection status is: " << connection_status << endl;
			}
			else {
				if (str.find(":") == string::npos)
					return ret;
			}

			line = strtok(NULL, "\r\n");
		}
		
		string url_str = "/";
		string content_type = "N";
		ret.host.contentType = (char *)malloc(sizeof(char)*1);
		strcpy(ret.host.contentType, content_type.c_str());
		ret.host.serverPath = (char *)malloc(sizeof(char)*1);
		strcpy(ret.host.serverPath, url_str.c_str());	
		
		/* To implement a sanity check for initial line. Begin from here... */
		string ini_line_str = initial_line;
		char *method = strtok(initial_line, " ");
		if (!method)	return ret;
		char *url = strtok(NULL, " ");
		if (!url)	return ret;
		char *version = strtok(NULL, " ");
		if (!version)	return ret;
		if (strcmp(method,"GET")!=0 || strlen(url)==0 || strcmp(version,"HTTP/1.1")!=0) return ret;
		ret.method = (char *)malloc(sizeof(char)*strlen(method));
		ret.host.serverPath = (char *)malloc(sizeof(char)*strlen(url));
		strcpy(ret.method, method);
		strcpy(ret.host.serverPath, url);
		cout << method << endl << url << endl << version << endl;
		/* Over */

		// check hostname and User-Agent
		if (!ret.host.hostname || !ret.userAgent)	return ret;

		url_str = url;
		// assign content_type
		if (url_str.find(".html") != string::npos)
			content_type = "text/html";
		if (url_str.find(".png") != string::npos)
			content_type = "image/png";
		if (url_str.find(".jpg") != string::npos)
			content_type = "image/jpeg";
		if (url_str == "/") {
			content_type = "text/html";
			free(ret.host.serverPath);
			ret.host.serverPath = (char *)malloc(sizeof(char)*strlen("/index.html"));
			strcpy(ret.host.serverPath, "/index.html");
		}
		ret.host.contentType = (char *)malloc(sizeof(char)*content_type.length());
		strcpy(ret.host.contentType, content_type.c_str());	
		ret.isValid = 1;
		return ret;
}


void geneResponse(int clntSocket, Request request, string doc_root){

	cout << "content type: " << request.host.contentType << endl;
	cout << "relative path: " << request.host.serverPath << endl;

    // Send 400 error if request is malformed
    if(!request.isValid) {
        sendError(clntSocket, 400);
        return;
    }
	
    // Send 404 error if file does not exist
    struct stat document;
    memset(&document, 0, sizeof(document));

	// append path
	string serverPath_str = request.host.serverPath;
	string filePath_str = doc_root + serverPath_str;

    // Get file descriptor of the file that will be sent
    char abspath[1024];
    realpath(filePath_str.c_str(), abspath);

    if (stat(abspath, &document) == -1) {
        sendError(clntSocket, 404);
        return;
    }

    // Send 403 error if file is not world readable
    if(!(document.st_mode & S_IROTH)) {
        sendError(clntSocket, 403);
        return;
    }

    if(access(abspath, F_OK ) == -1 ) {
        sendError(clntSocket, 404);
        return;
    }

    FILE * file = fopen(abspath, "r");

    // Send response header to the client
    sendHeader(clntSocket, 200, document.st_size, request.host.contentType);

    // Use sendFile() to send the file to the client
    off_t sent = 0;
    sendfile(clntSocket, fileno(file), &sent, document.st_size);

}


void sendHeader(int clntSocket, int errorCode, int contentLength, char * contentType) {

    char * codeString;

    switch (errorCode) {
        case 200:
            codeString = (char *)malloc(strlen("OK"));
            strcpy(codeString, "OK");
            break;
        case 400:
            codeString = (char *)malloc(strlen("Bad Request"));
            strcpy(codeString, "Bad Request");
            break;
        case 403:
            codeString = (char *)malloc(strlen("Forbidden"));
            strcpy(codeString, "Forbidden");
            break;
        case 404:
            codeString = (char *)malloc(strlen("Not Found"));
            strcpy(codeString, "Not Found");
            break;
    }

    // Send header
    int responseHeaderSize = snprintf(NULL, 0, "HTTP/1.1 %d %s\r\nServer: http-server-unknown\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", errorCode, codeString, contentType, contentLength) + 1;
    char * responseHeader = (char *)malloc(responseHeaderSize);
    snprintf(responseHeader, responseHeaderSize, "HTTP/1.1 %d %s\r\nServer: http-server-unknown\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", errorCode, codeString, contentType, contentLength);

    // Send the HTTP request to the server
    send(clntSocket, responseHeader, responseHeaderSize - 1, 0);

    free(codeString);
}


/**
 *
 */
void sendError(int clntSocket, int errorCode) {

    int bodySize = snprintf(NULL, 0, "<h1>%d Error</h1>\r\n", errorCode) + 1;
    char * responseBody = (char *)malloc(bodySize);
    snprintf(responseBody, bodySize, "<h1>%d Error</h1>\r\n", errorCode);

	char content_type[] = "text/html";
    // Sends the specified error code to the client
    sendHeader(clntSocket, errorCode, bodySize, content_type);

    // send body
    send(clntSocket, responseBody, bodySize, 0);

    //close(clntSocket); // Close client socket
}
