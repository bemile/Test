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
	keep_alive = true;
	is_connected = true;
	int val = pthread_create(&thread, NULL, &CommandExecClient::StartThread, this);
}

void CommandExecClient::Stop() {
	keep_alive = false;
	is_connected = false;
}

bool CommandExecClient::IsConnected() {
	return is_connected;
}

void CommandExecClient::SetSocket(int sock) {
	sockfd = sock;
}


void* CommandExecClient::StartThread(void* ptr) {
	((CommandExecClient*)ptr)->Run();
	return NULL;
}


void CommandExecClient::Run() {
	while (keep_alive) {
		int res;
		int msg_type;

		pthread_mutex_lock(&mutex);
		if ((res = read(sockfd, &msg_type, sizeof(msg_type))) < 0) {
			cout << "Error reading message. " << endl;
			is_connected = false;
			return;
			//exit(0);
		}

		int msg_length;
		if ((res = read(sockfd, &msg_length, sizeof(msg_length))) < 0) {
			cout << "Error sending message. " << endl;
			is_connected = false;
			return;
		}

		char buffer[BUFFER_SIZE];
		if ((res = read(sockfd, buffer, msg_length)) < 0) {
			cout << "Error sending message. " << endl;
			is_connected = false;
			return;
		} else {
			buffer[res] = '\0';
		}

		switch (msg_type) {
		case COMMAND:
			HandleCommand(buffer);
			break;
		default:
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
}


int CommandExecClient::HandleCommand(char* command) {
	return ExecSysCommand(command);
}


int CommandExecClient::ExecSysCommand(char* command) {
	cout << "I received a command: " << command << endl;
		FILE* pFile = popen(command, "r");
		if (pFile != NULL) {
			char output[BUFFER_SIZE];
			int bytes = fread(output, 1, BUFFER_SIZE, pFile);
			output[bytes] = '\0';
			pclose(pFile);
			SendMessage(COMMAND_RESPONSE, output);
		} else {
			cout << "Cannot get output from execution." << endl;
		}
		return 1;
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





