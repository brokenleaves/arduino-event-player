#ifndef SimpleSerialProtocol_h
#define SimpleSerialProtocol_h

typedef struct {
	char name[9];
	unsigned char count;
	long parameter[8];
} SimpleSerialCommand;

class SimpleSerialProtocol {
	public:
		SimpleSerialProtocol();
		void serialize(unsigned char *buf, SimpleSerialCommand *cmd);
		boolean process(unsigned char data);
		SimpleSerialCommand *getReceivedCommand();
		boolean isError();
		void reset();
	private:
		boolean error;
		boolean ignoreWhitespace;
		unsigned char state;
		unsigned char inputIndex;
		char input[17];
		SimpleSerialCommand cmd;
};

#endif

