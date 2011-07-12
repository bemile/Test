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
	string s = command;
	list<string> parts;
	Split(s, ' ', parts);
	if (parts.size() == 0)
		return 0;

	if (parts.front().compare("restart") == 0) {
		HandleRestartCommand();
	} else {
		ExecSysCommand(command);
	}

	return 1;
}


//
void CommandExecClient::HandleRestartCommand() {
	pid_t pid = 0;
	pid = fork();
	int status;
	if (pid < 0) {
		perror("process failed to fork" );
		return;
	}

	if (pid == 0) {	//child process
		wait(&status);

		chdir("/users/jieli/bin");
		execl( "/bin/sh", "sh", "/users/jieli/bin/run_starter.sh", (char *)0);
		//string command = "sudo killall emustarter\n/users/jieli/bin/run_starter.sh\n";
		//system(command.c_str());
		exit(0);
	}
	else {
		keep_alive = false;
		exit(0);
	}

	exit(0);
}



int CommandExecClient::ExecSysCommand(char* command) {
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



// Divide string s into sub strings separated by the character c
void CommandExecClient::Split(string s, char c, list<string>& slist) {
	const char* ptr = s.c_str();
	int start = 0;
	int cur_pos = 0;
	for (; *ptr != '\0'; ptr++) {
		if (*ptr == c) {
			if (cur_pos != start) {
				string subs = s.substr(start, cur_pos - start);
				slist.push_back(subs);
			}
			start = cur_pos + 1;
		}

		cur_pos++;
	}

	if (cur_pos != start) {
		string subs = s.substr(start, cur_pos - start);
		slist.push_back(subs);
	}
}





