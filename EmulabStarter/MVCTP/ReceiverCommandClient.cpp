/*
 * ReceiverMonitor.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "ReceiverCommandClient.h"

ReceiverCommandClient::ReceiverCommandClient(MVCTPReceiver* preceiver) {
	ptr_receiver = preceiver;
}


ReceiverCommandClient::ReceiverCommandClient(int sock, MVCTPReceiver* preceiver) {
	sockfd = sock;
	ptr_receiver = preceiver;
}

int ReceiverCommandClient::HandleCommand(char* command) {
	string s = command;
	/*int length = s.length();
	int index = s.find(' ');
	string comm_name = s.substr(0, index);
	string args = s.substr(index + 1, length - index - 1);
	 */

	list<string> parts;
	Split(s, ' ', parts);
	if (parts.size() == 0)
		return 0;

	if (parts.front().compare("SetBufferSize") == 0) {
		if (parts.size() == 2) {
			ptr_receiver->SetBufferSize(atoi(parts.back().c_str()));
			SendMessage(COMMAND_RESPONSE, "Buffer size has been set.");
		}
	} else {
		CommandExecClient::HandleCommand(command);
		//ExecSysCommand(command);
	}

	return 1;
}
