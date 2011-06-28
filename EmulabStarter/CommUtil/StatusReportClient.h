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
	StatusReportClient(int sock);
	int SendMessage(int msg_type, string msg);

private:
	int sockfd;
};

#endif /* STATUSREPORTCLIENT_H_ */
