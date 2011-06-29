/*
 * Logger.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <list>
#include <string>
#include "../CommUtil/StatusMonitor.h"

class Logger {
public:
	Logger();
	static int Log(int level, string msg);
	static void AddRemoteClient(StatusReportClient* pclient);

private:
	static list<StatusReportClient*> client_list;
};

#endif /* LOGGER_H_ */
