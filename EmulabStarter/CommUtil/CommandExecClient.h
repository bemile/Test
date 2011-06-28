/*
 * CommandExecClient.h
 *
 *  Created on: Jun 26, 2011
 *      Author: jie
 */

#ifndef COMMANDEXECCLIENT_H_
#define COMMANDEXECCLIENT_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <time.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>
#include "CommUtil.h"

using namespace std;

class CommandExecClient {
public:
	CommandExecClient(int sock);
	void Start();
	int SendMessage(int msg_type, string msg);

private:
	//StatusMonitor* pmonitor;
	int sockfd;

	int HandleCommand();

};

#endif /* COMMANDEXECCLIENT_H_ */
