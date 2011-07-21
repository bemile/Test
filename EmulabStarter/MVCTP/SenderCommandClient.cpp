/*
 * SenderCommandClient.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "SenderCommandClient.h"

SenderCommandClient::SenderCommandClient(MVCTPSender* psender) {
	ptr_sender = psender;
}


SenderCommandClient::SenderCommandClient(int sock, MVCTPSender* psender) {
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
		parts.erase(parts.begin());
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
	bool file_transfer = false;
	bool memory_transfer = false;
	bool send_out_packets = true;

	int 	mem_transfer_size = 0;

	string arg = "";
	list<string>::iterator it;
	for (it = slist.begin(); it != slist.end(); it++) {
		if ((*it)[0] == '-') {
			switch ((*it)[1]) {
			case 'm':
				it++;
				memory_transfer = true;
				mem_transfer_size = atoi((*it).c_str());	// in Megabytes
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
	if (memory_transfer) {
		MemoryTransfer(mem_transfer_size);
	}
	else if (file_transfer) {

	}
	else {
		ptr_sender->RawSend(arg.c_str(), arg.length(), send_out_packets);
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

// Transfer memory-to-memory data to all receivers
// size: the size of data to transfer (in megabytes)
int SenderCommandClient::MemoryTransfer(int size) {
	char buffer[MVCTP_DATA_LEN];
	memset(buffer, 1, MVCTP_DATA_LEN);

	timeval last_time, cur_time;
	long size_count = 0;
	long time_diff;
	gettimeofday(&last_time, NULL);
	long num_packets = (int) (size / MVCTP_DATA_LEN);
	for (long i = 0; i < num_packets; i++) {
		ptr_sender->RawSend(buffer, MVCTP_DATA_LEN, true);
		size_count += ETH_FRAME_LEN;
		gettimeofday(&cur_time, NULL);
		time_diff = (cur_time.tv_sec - last_time.tv_sec) * 1000000
					+ (cur_time.tv_usec - last_time.tv_usec);
		if (time_diff > 1000000) {
			last_time = cur_time;
			float rate = size_count / 1024.0 / 1024.0 * 8;
			size_count = 0;
			char buf[100];
			sprintf(buf, "Data sending rate: %3.2f Mbps", rate);
			SendMessage(COMMAND_RESPONSE, buf);
		}
	}

	return 1;
}


