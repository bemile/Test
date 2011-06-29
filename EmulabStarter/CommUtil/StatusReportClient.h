/*
 * StatusReportClient.h
 *
 *  Created on: Jun 26, 2011
 *      Author: jie
 */

#ifndef STATUSREPORTCLIENT_H_
#define STATUSREPORTCLIENT_H_


#include "CommUtil.h"

using namespace std;

class StatusReportClient {
public:
	StatusReportClient()	{sockfd = -1;}
	StatusReportClient(int sock);
	void SetSocket(int sock);
	int SendNodeInfo();
	int SendMessage(int msg_type, string msg);

private:
	int sockfd;

	int SendNodeName();
};

#endif /* STATUSREPORTCLIENT_H_ */
