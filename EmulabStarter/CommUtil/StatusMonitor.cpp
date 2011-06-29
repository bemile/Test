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

StatusMonitor::~StatusMonitor() {

}


int StatusMonitor::ConnectServer() {
	if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		cout << "socket() error" << endl;
		return -1;
	}

	int res;
	while ((res = connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
		cout << "connect() error" << endl;
		sleep(30);
		//return -1;
	}

	isConnected = true;
	return 1;
}



int StatusMonitor::StartClients() {
	if (!isConnected)
		return -1;

	stat_rep_client.SetSocket(sockfd);
	stat_rep_client.SendNodeInfo();
	stat_rep_client.SendMessage(INFORMATIONAL, "Status report client started.");

	ptr_comm_exec_client->SetSocket(sockfd);
	ptr_comm_exec_client->SendMessage(INFORMATIONAL, "Command execution client started.");
	ptr_comm_exec_client->Start();

	/*while (comm_exec_client.IsConnected()) {
		sleep(1);
	}

	close(sockfd);
	comm_exec_client.Stop();

	ConnectServer();
	StartClients();
	*/
	return 1;
}


void StatusMonitor::SetCommandExecClient(CommandExecClient* ptr_client) {
	ptr_comm_exec_client = ptr_client;
}

CommandExecClient* StatusMonitor::GetCommandExecClient() {
	return ptr_comm_exec_client;
}

StatusReportClient* StatusMonitor::GetStatusReportClient() {
	return &stat_rep_client;
}
