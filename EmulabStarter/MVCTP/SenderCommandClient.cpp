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

	if (parts.front().compare("Send") == 0) {
		parts.erase(parts.begin());
		HandleSendCommand(parts);
	}
	else if (parts.front().compare("SetRate") == 0) {
		if (parts.size() == 2) {
			ptr_sender->SetSendRate(atoi(parts.back().c_str()));
			SendMessage(COMMAND_RESPONSE, "Data sending rate has been set.");
		}
	}
	else if (parts.front().compare("SetBufferSize") == 0) {
		if (parts.size() == 2) {
			ptr_sender->SetBufferSize(atoi(parts.back().c_str()));
			SendMessage(COMMAND_RESPONSE, "Buffer size has been set.");
		}
	} else {
		CommandExecClient::HandleCommand(command);
		//ExecSysCommand(command);
	}

	return 1;
}




//
int SenderCommandClient::HandleSendCommand(list<string>& slist) {
	bool memory_transfer = false;
	bool file_transfer = false;
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
		TransferMemoryData(mem_transfer_size);
	}
	else if (file_transfer) {

	}
	else {
		TransferString(arg, send_out_packets);
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
int SenderCommandClient::TransferMemoryData(int size) {
	TransferMessage msg;
	msg.msg_type = MEMORY_TRANSFER;
	msg.data_len = size;
	ptr_sender->RawSend((char*)&msg, sizeof(msg), true);

	char buffer[MVCTP_DATA_LEN];
	memset(buffer, 1, MVCTP_DATA_LEN);

	timeval last_time, cur_time;
	long size_count = 0;
	float time_diff;
	gettimeofday(&last_time, NULL);

	int remained_size = size;
	int period = size / MVCTP_DATA_LEN / 10;
	int send_count = 0;
	while (remained_size > 0) {
		int packet_size = remained_size > MVCTP_DATA_LEN ? MVCTP_DATA_LEN
				: remained_size;
		ptr_sender->RawSend(buffer, packet_size, true);
		remained_size -= packet_size;

		// periodically calculate transfer speed
		size_count += (packet_size + MVCTP_HLEN + ETH_HLEN);
		send_count++;
		if (send_count % period == 0) {
			gettimeofday(&cur_time, NULL);
			time_diff = (cur_time.tv_sec - last_time.tv_sec)
					+ (cur_time.tv_usec - last_time.tv_usec) / 1000000.0;

			last_time = cur_time;
			float rate = size_count / time_diff / 1024.0 / 1024.0 * 8;
			size_count = 0;
			char buf[100];
			sprintf(buf, "Data sending rate: %3.2f Mbps", rate);
			SendMessage(COMMAND_RESPONSE, buf);
		}
	}

	return 1;
}


int SenderCommandClient::TransferString(string str, bool send_out_packets) {
	TransferMessage msg;
	msg.msg_type = STRING_TRANSFER;
	msg.data_len = str.length();
	ptr_sender->RawSend((char*)&msg, sizeof(msg), send_out_packets);
	ptr_sender->RawSend(str.c_str(), msg.data_len, send_out_packets);

	return 1;
}


