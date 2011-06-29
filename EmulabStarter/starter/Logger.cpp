/*
 * Logger.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "Logger.h"

list<StatusReportClient*> Logger::client_list;

Logger::Logger()	{

}

int Logger::Log(int level, string msg) {
	list<StatusReportClient*>::iterator it;
	for (it = client_list.begin(); it != client_list.end(); it++) {
		(*it)->SendMessage(level, msg);
	}
	return 1;
}

void Logger::AddRemoteClient(StatusReportClient* pclient) {
	client_list.push_back(pclient);
}
