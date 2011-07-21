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
		MVCTPSender mvctp_sender(send_buf_size);
		mvctp_sender.JoinGroup(group_id, mvctp_port);
		mvctp_sender.SetSendRate(5);

		SenderCommandClient command_client(&mvctp_sender);
		if (serv_addr.length() > 0) {
			ptr_monitor = new StatusMonitor(serv_addr, atoi(port.c_str()));
			ptr_monitor->SetCommandExecClient(&command_client);
			ptr_monitor->ConnectServer();
			ptr_monitor->StartClients();
		}

		this->Log(INFORMATIONAL, "Sender joined group.");

		while (true) {
			sleep(1);
		}
	}
	else {  //Is receiver
		CommandExecClient command_client;
		if (serv_addr.length() > 0) {
			ptr_monitor = new StatusMonitor(serv_addr, atoi(port.c_str()));
			ptr_monitor->SetCommandExecClient(&command_client);
			ptr_monitor->ConnectServer();
			ptr_monitor->StartClients();
		}

		MVCTPReceiver mvctp_receiver(recv_buf_size);
		mvctp_receiver.JoinGroup(group_id, mvctp_port);
		this->Log(INFORMATIONAL, "Receiver joined group.");


		char buff[BUFF_SIZE];
		sockaddr_in from;
		socklen_t socklen = sizeof(from);
		int bytes;
		while ( (bytes = mvctp_receiver.RawReceive(buff, BUFF_SIZE, 0, (SA *)&from, &socklen)) > 0) {
			//buff[bytes] = '\0';
			//string s = "I received a message: ";
			//s.append(buff);
			//this->Log(INFORMATIONAL, s);
		}
	}
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

