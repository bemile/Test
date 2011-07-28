/*
 * Tester.cpp
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */

#include "Tester.h"

Tester::Tester() {
}

Tester::~Tester() {
	delete ptr_monitor;
}

void Tester::StartTest() {
	string serv_addr = ConfigInfo::GetInstance()->GetValue("Monitor_Server");
	string port = ConfigInfo::GetInstance()->GetValue("Monitor_Server_Port");

	int send_buf_size = atoi(ConfigInfo::GetInstance()->GetValue("Send_Buffer_Size").c_str());
	int recv_buf_size = atoi(ConfigInfo::GetInstance()->GetValue("Recv_Buffer_Size").c_str());

	if (IsSender()) {
		ptr_mvctp_sender = new MVCTPSender(send_buf_size);
		ptr_mvctp_sender->JoinGroup(group_id, mvctp_port);
		ptr_mvctp_sender->SetSendRate(50);

		SenderCommandClient command_client(ptr_mvctp_sender);
		if (serv_addr.length() > 0) {
			ptr_monitor = new StatusMonitor(serv_addr, atoi(port.c_str()));
			ptr_monitor->SetCommandExecClient(&command_client);
			ptr_monitor->ConnectServer();
			ptr_monitor->StartClients();
		}

		this->Log(INFORMATIONAL, "I'm the sender. Just joined the multicast group.");

		while (true) {
			sleep(1);
		}
	}
	else {  //Is receiver
		// Set the receiver process to real-time scheduling mode
		//struct sched_param sp;
		//sp.__sched_priority = sched_get_priority_max(SCHED_RR);
		//sched_setscheduler(0, SCHED_RR, &sp);

		ptr_mvctp_receiver = new MVCTPReceiver(recv_buf_size);
		ptr_mvctp_receiver->JoinGroup(group_id, mvctp_port);

		ReceiverCommandClient command_client(ptr_mvctp_receiver);
		if (serv_addr.length() > 0) {
			ptr_monitor = new StatusMonitor(serv_addr, atoi(port.c_str()));
			ptr_monitor->SetCommandExecClient(&command_client);
			ptr_monitor->ConnectServer();
			ptr_monitor->StartClients();
		}

		this->Log(INFORMATIONAL, "I'm a receiver. Just joined the multicast group.");

		TransferMessage msg;
		sockaddr_in from;
		socklen_t socklen = sizeof(from);
		int bytes;
		while (true) {
			if ( (bytes = ptr_mvctp_receiver->RawReceive(&msg, sizeof(msg), 0, (SA *)&from, &socklen)) < 0) {
				SysError("Tester::StartTester::RawReceive() error");
			}

			switch (msg.msg_type) {
			case STRING_TRANSFER:
				HandleStringTransfer(msg);
				break;
			case MEMORY_TRANSFER:
				HandleMemoryTransfer(msg, MVCTP_DATA_LEN);
				break;
			case FILE_TRANSFER:
				HandleFileTransfer(msg, 4096);
				break;
			default:
				break;
			}
		}
	}
}


void Tester::HandleStringTransfer(TransferMessage& msg) {
	char buff[msg.data_len + 1];
	sockaddr_in from;
	socklen_t socklen = sizeof(from);

	int bytes;
	if ((bytes = ptr_mvctp_receiver->RawReceive(buff, msg.data_len, 0, (SA *) &from,
			&socklen)) < 0) {
		SysError("Tester::HandleStringTransfer::RawReceive() error");
	}

	buff[bytes] = '\0';
	string s = "I received a message: ";
	s.append(buff);
	this->Log(INFORMATIONAL, s);
}


void Tester::HandleMemoryTransfer(TransferMessage& msg, size_t buff_size) {
	size_t remained_size = msg.data_len;
	char buff[buff_size];
	sockaddr_in from;
	socklen_t socklen = sizeof(from);

	timeval start_time, end_time;
	char s[256];
	sprintf(s, "Start memory transfer... Total size to transfer: %d", msg.data_len);
	this->Log(INFORMATIONAL, s);
	gettimeofday(&start_time, NULL);
	int bytes = 0;
	while (remained_size > 0) {
		int recv_size = remained_size > buff_size ? buff_size : remained_size;
		if ((bytes = ptr_mvctp_receiver->RawReceive(buff, recv_size, 0,
				(SA *) &from, &socklen)) < 0) {
			SysError("Tester::HandleMemoryTransfer::RawReceive() error");
		}

		remained_size -= bytes;
	}
	gettimeofday(&end_time, NULL);
	float trans_time = end_time.tv_sec - start_time.tv_sec +
			(end_time.tv_usec - start_time.tv_usec) / 1000000.0 + 0.001;

	sprintf(s, "Memory transfer finished. Total transfer time: %.2f", trans_time);
	this->Log(INFORMATIONAL, s);

	struct ReceiveBufferStats stats = ptr_mvctp_receiver->GetBufferStats();
	float retrans_rate = stats.num_retransmitted_packets * 1.0 / stats.num_received_packets;
	float throughput = msg.data_len * 8 / 1024.0 / 1024.0 / trans_time;

	// Format:Transfer Time (Sec), Throughput (Mbps), #Packets, #Retransmitted Packets, #Retransmission Rate
	sprintf(s, "%.2f,%.2f,%d,%d,%f\n", trans_time, throughput, stats.num_received_packets,
					stats.num_retransmitted_packets, retrans_rate);
	this->Log(INFORMATIONAL, s);

	ptr_mvctp_receiver->ResetBuffer();

//	char file_name[30];
//	string host_name = ExecSysCommand("hostname -f");
//	int index = host_name.find('-');
//	host_name = host_name.substr(0, index);
//	sprintf(file_name, "/users/jieli/results/mem_transfer_size%d_buff%d_%s.csv",
//			msg.data_len, ptr_mvctp_receiver->GetBufferSize(), host_name.c_str());
//
//	ofstream fout(file_name);
//	if (!fout.fail()) {
//		struct ReceiveBufferStats stats = ptr_mvctp_receiver->GetBufferStats();
//		float retrans_rate = stats.num_retransmitted_packets * 1.0 / stats.num_received_packets;
//		fout << "Transfer Time (Sec), #Packets, #Retransmitted Packets, #Retransmission Rate" << endl;
//		sprintf(s, "%.2f,%d,%d,%f\n", trans_time, stats.num_received_packets,
//				stats.num_retransmitted_packets, retrans_rate);
//		fout << s;
//		fout.close();
//	}
}


void Tester::HandleFileTransfer(TransferMessage& msg, size_t buff_size) {

}


void Tester::Log(int level, string msg) {
	ptr_monitor->GetStatusReportClient()->SendMessage(level, msg);
}

bool Tester::IsSender() {
	struct utsname host_name;
	uname(&host_name);
	string nodename = host_name.nodename;
	if (nodename.find("node0") != string::npos ||
			nodename.find("ubuntu") != string::npos) {
		return true;
	}
	else
		return false;
}

string Tester::ExecSysCommand(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char output[4096];
    string result = "";
    while(!feof(pipe)) {
    	int bytes = fread(output, 1, 4095, pipe);
    	output[bytes] = '\0';
    	result += output;
    }
    pclose(pipe);
    return result;
}

