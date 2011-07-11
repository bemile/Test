/*
 * SenderCommandClient.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "SenderCommandClient.h"

SenderCommandClient::SenderCommandClient(MVCTPComm* psender) {
	ptr_sender = psender;
}


SenderCommandClient::SenderCommandClient(int sock, MVCTPComm* psender) {
	sockfd = sock;
	ptr_sender = psender;
}

int SenderCommandClient::HandleCommand(char* command) {
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

	if (parts.front().compare("send") == 0) {
		HandleSendCommand(parts);
	}
	else {
		CommandExecClient::HandleCommand(command);
		//ExecSysCommand(command);
	}

	return 1;
}




//
int SenderCommandClient::HandleSendCommand(list<string>& slist) {
	bool direct_transfer = true;
	bool file_transfer = false;
	bool memory_transfer = false;
	bool send_out_packets = true;

	string arg = "";
	list<string>::iterator it;
	for (it = slist.begin(); it != slist.end(); it++) {
		if ((*it)[0] == '-') {
			switch ((*it)[1]) {
			case 'm':
				memory_transfer = true;
				break;
			case 'f':
				file_transfer = true;
				break;
			case 'n':
				send_out_packets = false;
				break;
			default:
				break;
			}
		}
		else {
			arg.append(*it);
			arg.append(" ");
		}
	}

	//ptr_sender->IPSend(&command[index + 1], args.length(), true);
	if (direct_transfer) {
		ptr_sender->IPSend(arg.c_str(), arg.length(), send_out_packets);
	}

	// Send result status back to the monitor
	if (send_out_packets) {
		SendMessage(COMMAND_RESPONSE, "Data sent.");
	}
	else {
		SendMessage(COMMAND_RESPONSE, "Data recorded but not sent out (simulate packet loss).");
	}

	return 1;
}



