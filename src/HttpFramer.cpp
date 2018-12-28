#include <assert.h>
#include "HttpFramer.hpp"

using namespace std;

void HttpFramer::append(string chars)
{
	// PUT YOUR CODE HERE
	memcpy(buf+t, chars.c_str(), chars.length());
	t += chars.length();
	// cout << "head: " << h << "\ttail: " << t << endl;
}

bool HttpFramer::hasMessage() const
{
	// PUT YOUR CODE HERE
	// cout << "head: " << h << "\ttail: " << t << endl;
	for(int i = h; i < t-3; i++){
		if (buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n') {
			if (t-h >= 4) return true; // one complete message at least; two char in the end
		}
	}
	// if (t-h > 2) {return true;} // two char in the end
	return false;
}

string HttpFramer::topMessage() const
{
	// PUT YOUR CODE HERE
	for(int i = h; i < t-3; i++){
		if (buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n') {
			// cout << "head: " << h << "\ttop: " << i << endl;
			char* message = (char*)malloc(sizeof(char)*(i-h+1));
			memcpy(message, buf+h, i-h);
			message[i-h] = 0;
			string res = message;
			//cout << sizeof(message) << endl;
			//cout << res.length() << endl;
			return res;
		}
	}
	return "";
}

void HttpFramer::popMessage()
{
	// PUT YOUR CODE HERE
	for(int i = h; i < t-3; i++){
		if (buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n') {
			// cout << "head: " << h << "\tpop: " << i << endl;
			h = i + 4;
			break;
		}
	}
}

void HttpFramer::printToStream(ostream& stream) const
{
	// (OPTIONAL) PUT YOUR CODE HERE--useful for debugging
	char* bufEff = (char*)malloc(sizeof(char)*(t-h+1));
	memcpy(bufEff, &buf, t-h);
	bufEff[t-h] = 0;
	cout << bufEff << endl;
}
