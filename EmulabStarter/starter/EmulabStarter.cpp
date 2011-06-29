//============================================================================
// Name        : EmulabStarter.cpp
// Author      : Jie Li
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../CommUtil/StatusMonitor.h"
#include "ConfigParser.h"
#include "Logger.h"
#include "../MVCTP/Tester.h"

using namespace std;

ConfigParser* ptr_parser;
StatusMonitor* ptr_monitor;

void StartStatusMonitor();
void Clean();


int main(int argc, char** argv) {
	if (argc != 2) {
		cout << "usage: a.out config_file_name" << endl;
		return -1;
	}

	// Parse configuration file
	ptr_parser = new ConfigParser();
	ptr_parser->Parse(argv[1]);

	// Start status monitor
	//StartStatusMonitor();

	// Configure logger
	//Logger::AddRemoteClient(ptr_monitor->GetStatusReportClient());


	Tester tester;
	tester.StartTest();

	Clean();
	return 0;
}


// Start status monitor and connect to remote server (if configured)
void StartStatusMonitor() {
	pid_t pid;
	if ((pid = fork()) < 0)
		return;
	else if (pid == 0) {
		// run status monitor as a daemon in the child process
		setsid();
		chdir("/");
		umask(0);

		string serv_addr = ptr_parser->GetValue("Monitor Server");
		string port = ptr_parser->GetValue("Monitor Server Port");
		if (serv_addr.length() > 0) {
			ptr_monitor = new StatusMonitor(serv_addr, atoi(port.c_str()));
			ptr_monitor->ConnectServer();
			ptr_monitor->StartClients();
		}
	}
}


void Clean() {
	delete ptr_parser;
	delete ptr_monitor;
}
