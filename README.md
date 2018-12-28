# Building a Web Server


## Overview

In this project, you are going to build a simple webserver that implements a subset of the HTTP/1.1 protocol specification (as defined in this document–you do not need to read any other HTTP specifications or write-ups).

### Client/server protocol
This client-server protocol that is layered on top of the reliable stream-oriented transport protocol TCP. Clients issue request messages to the server, and servers reply with response messages. In its most basic form, a single HTTP-level request-reply exchange happens over a single, dedicated TCP connection. The client first connects to the server, sends the HTTP request message, the server replies with an HTTP response, and then the server closes the connection.

Repeatedly setting up and tearing down TCP connections reduces overall network throughput and efficiency, and so HTTP has a mechanism whereby a client can reuse a TCP connection to a given server. The idea is that the client opens a TCP connection to the server, issues an HTTP request, gets an HTTP reply, and then issues another HTTP request on the already open outbound part of the connection. The server replies with the response, and this can continue through multiple request-reply interactions. The client signals the last request by setting a “Connection: close” header. The server indicates that it will not handle additional requests by setting the “Connection: close” header in the response. Note that the client can issue more than one HTTP request without necessarily waiting for full HTTP replies to be returned.

### HTTP messages
HTTP request and response messages are plain-text, consisting of a header section and an optional body section. The header section is separated from the body section with a blank line. The header consists of an initial line (which is different between requests and responses), followed by zero or more key-value pairs. Every line is terminated by a CRLF (carriage-return followed by a line feed).

## To build

make
