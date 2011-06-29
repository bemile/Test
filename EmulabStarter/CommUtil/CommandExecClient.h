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
	CommandExecClient() {sockfd = -1;}
	CommandExecClient(int sock);
	~CommandExecClient();
	void Start();
	void Run();
	void Stop();
	bool IsConnected();
	void SetSocket(int sock);
	int SendMessage(int msg_type, string msg);

protected:
	int sockfd;
	pthread_t thread;
	pthread_mutex_t mutex;
	bool keep_alive;
	bool is_connected;

	static void* StartThread(void* ptr);
	virtual int HandleCommand(char* command);
	int ExecSysCommand(char* command);

private:

};

#endif /* COMMANDEXECCLIENT_H_ */
