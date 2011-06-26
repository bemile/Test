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

using namespace std;

typedef struct sockaddr SA;

enum MsgType {
	NODE_NAME = 1,
    IP_ADDRESS = 2,
    INFORMATIONAL = 3,
    WARNING = 4
};


class StatusMonitor {
public:
	StatusMonitor(string addr, int port);
	int ConnectServer();
	int SendMessage(int msg_type, string msg);

private:
	int sockfd;
	struct sockaddr_in servaddr;
	bool isConnected;
};


#endif /* STATUSMONITOR_H_ */
