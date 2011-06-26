/*
 * StatusMonitor.cpp
 *
 *  Created on: Jun 25, 2011
 *      Author: jie
 */

#include "StatusMonitor.h"

StatusMonitor::StatusMonitor(string addr, int port) {
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family 	 = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(addr.c_str());
	servaddr.sin_port 		 = htons(port);

	isConnected = false;
}


int StatusMonitor::ConnectServer() {
	if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		cout << "socket() error" << endl;
		return -1;
	}

	int res;
	if ((res = connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
		cout << "connect() error" << endl;
		return -1;
	}
	else {
		isConnected = true;
		SendNodeName();
		return 1;
	}
}


int StatusMonitor::SendNodeName() {
	struct utsname host_name;
	uname(&host_name);
	SendMessage(NODE_NAME, host_name.nodename);

}

int StatusMonitor::StartClients() {
	if (!isConnected)
		return -1;

	CommandExecClient comm_exec_client(sockfd);
	comm_exec_client.Start();

}


int StatusMonitor::SendMessage(int msg_type, string msg) {
	if (!isConnected)
		return -1;

	int res;
	if ( (res = write(sockfd, &msg_type, sizeof(msg_type))) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}

	int length = msg.length();
	if ( (res = write(sockfd, &length, sizeof(length))) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}

	if ( (res = write(sockfd, msg.c_str(), length)) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}
	else
		return 1;
}
