/*
 * CommandExecClient.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: jie
 */

#include "CommandExecClient.h"


CommandExecClient::CommandExecClient(int sock) {
	sockfd = sock;
}

CommandExecClient::~CommandExecClient() {
	pthread_mutex_destroy(&mutex);
}


void CommandExecClient::Start() {
	pthread_mutex_init(&mutex, 0);
	keepAlive = true;
	int val = pthread_create(&thread, NULL, &CommandExecClient::StartThread, this);
}

void CommandExecClient::Stop() {
	keepAlive = false;
}


void* CommandExecClient::StartThread(void* ptr) {
	((CommandExecClient*)ptr)->Run();
	return NULL;
}


void CommandExecClient::Run() {
	while (keepAlive) {
		int res;
		int msg_type;

		pthread_mutex_lock(&mutex);
		if ((res = read(sockfd, &msg_type, sizeof(msg_type))) < 0) {
			cout << "Error sending message. " << endl;
		}

		switch (msg_type) {
		case COMMAND:
			HandleCommand();
			break;
		default:
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
}


int CommandExecClient::HandleCommand() {
	int res;
	int msg_length;
	if ((res = read(sockfd, &msg_length, sizeof(msg_length))) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}

	char buffer[BUFFER_SIZE];
	if ((res = read(sockfd, buffer, msg_length)) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	} else {
		buffer[res] = '\0';
		cout << "I received a command: " << buffer << endl;
		FILE* pFile = popen(buffer, "r");
		if (pFile != NULL) {
			char output[BUFFER_SIZE];
			int bytes = fread(output, 1, BUFFER_SIZE, pFile);
			output[bytes] = '\0';
			pclose(pFile);
			SendMessage(COMMAND_RESPONSE, output);
		}
		else {
			cout << "Cannot get output from execution." << endl;
		}
		return 1;
	}
}


int CommandExecClient::SendMessage(int msg_type, string msg) {
	int res;
	if ( (res = write(sockfd, &msg_type, sizeof(msg_type))) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}

	int length = msg.length();
	if ( (res = write(sockfd, &length, sizeof(length))) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}

	if ( (res = write(sockfd, msg.c_str(), length)) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}
	else
		return 1;
}





