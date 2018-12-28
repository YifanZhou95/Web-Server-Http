#ifndef CALCFRAMER_HPP
#define CALCFRAMER_HPP

#include <iostream>
#include <string.h>

#define BUFSIZE 1000000

class HttpFramer {
public:
	HttpFramer() {h=0; t=0;}

	void append(std::string chars);

	// Does this buffer contain at least one complete message?
	bool hasMessage() const;

	// Copy the first instruction in this buffer into the provided class
	std::string topMessage() const;

	// Pops off the first instruction from the buffer
	void popMessage();

	// prints the string to an ostream (useful for debugging)
	void printToStream(std::ostream& stream) const;

protected:
	// PUT ANY FIELDS/STATE HERE
	uint8_t buf[BUFSIZE];
	int h;
	int t;
};

#endif // CALCFRAMER_HPP
