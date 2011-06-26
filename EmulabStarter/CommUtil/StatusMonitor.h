/*
 * StatusMonitor.h
 *
 *  Created on: Jun 25, 2011
 *      Author: jie
 */

#ifndef STATUSMONITOR_H_
#define STATUSMONITOR_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <time.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>
#include "CommUtil.h"
#include "CommandExecClient.h"

using namespace std;

typedef struct sockaddr SA;

//class CommandExecClient;

class StatusMonitor {
public:
	StatusMonitor(string addr, int port);
	int ConnectServer();
	int StartClients();
	int SendMessage(int msg_type, string msg);

private:
	int sockfd;
	struct sockaddr_in servaddr;
	bool isConnected;

	int SendNodeName();

};


#endif /* STATUSMONITOR_H_ */
