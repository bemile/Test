/*
 * StatusMonitor.h
 *
 *  Created on: Jun 25, 2011
 *      Author: jie
 */

#ifndef STATUSMONITOR_H_
#define STATUSMONITOR_H_

#include "CommUtil.h"
#include "CommandExecClient.h"
#include "StatusReportClient.h"

using namespace std;

typedef struct sockaddr SA;

//class CommandExecClient;

class StatusMonitor {
public:
	StatusMonitor(string addr, int port);
	~StatusMonitor();

	int ConnectServer();
	int StartClients();
	void SetCommandExecClient(CommandExecClient* ptr_client);
	CommandExecClient* GetCommandExecClient();
	StatusReportClient* GetStatusReportClient();


private:
	int sockfd;
	struct sockaddr_in servaddr;
	bool isConnected;
	StatusReportClient stat_rep_client;
	CommandExecClient* ptr_comm_exec_client;
};


#endif /* STATUSMONITOR_H_ */
