#include "WProgram.h"
#include "SimpleSerialProtocol.h"

#define PARSER_STATE_NAME 0
#define PARSER_STATE_PARAMETER 1

SimpleSerialProtocol::SimpleSerialProtocol() {
	reset();
}

void SimpleSerialProtocol::reset() {
	error = false;
	ignoreWhitespace = true;
	state = PARSER_STATE_NAME;
	inputIndex = 0;

	cmd.count = 0;
	for (char i = 0; i < 9; i++) {
		cmd.name[i] = 0;
	}
	for (char i = 0; i < 8; i++) {
		cmd.parameter[i] = 0;
	}
}

boolean SimpleSerialProtocol::isError() {
	return error;
}

SimpleSerialCommand *SimpleSerialProtocol::getReceivedCommand() {
	return &cmd;
}

void SimpleSerialProtocol::serialize(unsigned char *buf, SimpleSerialCommand *cmd) {
	// TODO
}

boolean SimpleSerialProtocol::process(unsigned char data) {
	if (error) {
		return false;
	}

	boolean fieldEnd = (data == ',');
	boolean commandEnd = ((data == 10) || (data == 13) || (data == ';'));

	boolean whitespace = ((data == 32) || (data == 9));
	if (ignoreWhitespace) {
		if (whitespace) {
			return false;
		}
		ignoreWhitespace = false;
	}

	if (whitespace) {
		// TODO: check if whitespace disallowed
		ignoreWhitespace = true;
	}
	if (fieldEnd || commandEnd) {
		if (state >= PARSER_STATE_PARAMETER) {
			if (cmd.count >= 8) {
				error = true;
				return false;
			}
			input[inputIndex] = 0;
			cmd.parameter[cmd.count] = atol(input);
			cmd.count++;
			// TODO: check conversion result
		}
		
		inputIndex = 0;
		state++;
		ignoreWhitespace = true;
		return commandEnd;
	}

	switch (state) {
		case PARSER_STATE_NAME:
			cmd.name[inputIndex] = data;
			inputIndex++;
			if (inputIndex > 8) {
				error = true;
				return false;
			}
		break;
		default:
			input[inputIndex] = data;
			inputIndex++;
			if (inputIndex > 16) {
				error = true;
				return false;
			}
	}

	return false;
}

