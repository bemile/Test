/*
 * SenderCommandClient.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "SenderCommandClient.h"

int SenderCommandClient::HandleCommand(char* command) {
	string s = command;
	int length = s.length();
	int index = s.find(' ');
	string comm_name = s.substr(0, index);
	string args = s.substr(index + 1, length - index - 1);

	if (comm_name.compare("send") == 0) {
		ptr_sender->Send(&command[index + 1], args.length());
		SendMessage(COMMAND_RESPONSE, "Data sent.");
	}
	else {
		ExecSysCommand(command);
	}

	return 1;
}
