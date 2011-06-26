//============================================================================
// Name        : EmulabStarter.cpp
// Author      : Jie Li
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdio>
#include "../CommUtil/StatusMonitor.h"
#include "ConfigParser.h"

using namespace std;

int main() {
	/*ConfigParser parser("config");
	map<string, string> params = parser.GetParamSet();
	map<string, string>::iterator iter;
	int size = params.size();
	for (iter = params.begin(); iter != params.end(); iter++) {
		cout << (*iter).first << "\t\t" << (*iter).second << endl;
	}*/

	StatusMonitor monitor("192.168.1.101", 10000);
	monitor.ConnectServer();

	struct utsname host_name;
	uname(&host_name);
	char buf[100];

	sprintf(buf, "Domain Name: %s", host_name.domainname);
	monitor.SendMessage(buf);

	sprintf(buf, "Machine Name: %s", host_name.machine);
	monitor.SendMessage(buf);

	sprintf(buf, "Node Name: %s", host_name.nodename);
	monitor.SendMessage(buf);

	sprintf(buf, "System Name: %s", host_name.sysname);
	monitor.SendMessage(buf);

	sprintf(buf, "Release: %s", host_name.release);
	monitor.SendMessage(buf);

	sprintf(buf, "Version: %s", host_name.version);
	monitor.SendMessage(buf);

	return 0;
}
