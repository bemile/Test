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
	int ReportStatus(int msg_type, string msg);
	int SendMessage(int msg_type, string msg);

private:
	int sockfd;
	struct sockaddr_in servaddr;
	bool isConnected;
	CommandExecClient comm_exec_client;
	StatusReportClient stat_rep_client;
	int SendNodeName();

};


#endif /* STATUSMONITOR_H_ */
