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
	if (IsSender()) {
		MVCSender sender;
		sender.JoinGroup(group_mac_addr);

		SenderCommandClient command_client(&sender);
		string serv_addr = ptr_parser->GetValue("Monitor Server");
		string port = ptr_parser->GetValue("Monitor Server Port");
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
		string serv_addr = ptr_parser->GetValue("Monitor Server");
		string port = ptr_parser->GetValue("Monitor Server Port");
		if (serv_addr.length() > 0) {
			ptr_monitor = new StatusMonitor(serv_addr, atoi(port.c_str()));
			ptr_monitor->SetCommandExecClient(&command_client);
			ptr_monitor->ConnectServer();
			ptr_monitor->StartClients();
		}

		MVCReceiver receiver;
		receiver.JoinGroup(group_mac_addr);
		this->Log(INFORMATIONAL, "Receiver joined group.");


		char buff[BUFF_SIZE];
		//sockaddr_in from;
		//socklen_t socklen;
		int bytes;
		while ( (bytes = receiver.ReceiveData(buff, BUFF_SIZE)) > 0) {
			buff[bytes] = '\0';
			string s = "I received a message: ";
			s.append(buff);
			this->Log(INFORMATIONAL, s);
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
	if (nodename.find("node0") != string::npos) {
		return false;
	}
	else
		return true;
}

