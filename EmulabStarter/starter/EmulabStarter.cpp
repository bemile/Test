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
#include "../CommUtil/StatusMonitor.h"
#include "ConfigParser.h"

using namespace std;

int main(int argc, char** argv) {

	if (argc != 2) {
		cout << "usage: a.out config_file_name" << endl;
		return -1;
	}

	ConfigParser parser(argv[1]);
	/*map<string, string> params = parser.GetParamSet();
	map<string, string>::iterator iter;
	int size = params.size();
	for (iter = params.begin(); iter != params.end(); iter++) {
		cout << (*iter).first << "\t\t" << (*iter).second << endl;
	}*/

	string serv_addr = parser.GetValue("Monitor Server");
	string port = parser.GetValue("Monitor Server Port");
	if (serv_addr.length() > 0) {
		StatusMonitor monitor(serv_addr, atoi(port.c_str()));
		monitor.ConnectServer();

		// Send node name
		struct utsname host_name;
		uname(&host_name);
		monitor.SendMessage(NODE_NAME, host_name.nodename);

		// Send normal message
		monitor.SendMessage(INFORMATIONAL, "Hello server.");
	}

	return 0;
}
