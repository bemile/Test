/*
 * Tester.h
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */

#ifndef TESTER_H_
#define TESTER_H_

#include "mvctp.h"
#include "MVCTPComm.h"
#include "SenderCommandClient.h"
#include "../CommUtil/StatusMonitor.h"
#include "../starter/ConfigParser.h"

extern ConfigParser* ptr_parser;

class Tester {
public:
	Tester();
	~Tester();
	void StartTest();
	void Log(int level, string msg);

private:
	StatusMonitor* ptr_monitor;

	bool IsSender();

};

#endif /* TESTER_H_ */
