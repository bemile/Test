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

	char msg[512];
	if (parts.front().compare("SetRecvBuffSize") == 0) {
		if (parts.size() == 2) {
			size_t size = atoi(parts.back().c_str());
			ptr_receiver->SetBufferSize(size);
			sprintf(msg, "Receive buffer size has been set to %d bytes.", size);
			SendMessage(COMMAND_RESPONSE, msg);
		}
	}
	else if (parts.front().compare("SetSocketBuffSize") == 0) {
		if (parts.size() == 2) {
			size_t size = atoi(parts.back().c_str());
			ptr_receiver->SetSocketBufferSize(size);
			sprintf(msg, "Socket buffer size has been set to %d bytes.", size);
			SendMessage(COMMAND_RESPONSE, msg);
		}
	}
	else if (parts.front().compare("ResetBuffer") == 0) {
		ptr_receiver->ResetBuffer();
		SendMessage(COMMAND_RESPONSE, "Receive buffer has been reset.");
	}
	else {
		CommandExecClient::HandleCommand(command);
		//ExecSysCommand(command);
	}

	return 1;
}
