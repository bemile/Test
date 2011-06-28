/*
 * CommandExecClient.h
 *
 *  Created on: Jun 26, 2011
 *      Author: jie
 */

#ifndef COMMANDEXECCLIENT_H_
#define COMMANDEXECCLIENT_H_

#include <pthread.h>
#include "CommUtil.h"


using namespace std;

class CommandExecClient {
public:
	CommandExecClient(int sock);
	~CommandExecClient();
	void Start();
	void Run();
	void Stop();
	int SendMessage(int msg_type, string msg);

private:
	int sockfd;
	pthread_t thread;
	pthread_mutex_t mutex;
	bool keepAlive;

	static void* StartThread(void* ptr);
	int HandleCommand();
};

#endif /* COMMANDEXECCLIENT_H_ */
