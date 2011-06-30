/*
 * MVCReceiver.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef MVCRECEIVER_H_
#define MVCRECEIVER_H_

#include "mvctp.h"
#include "../starter/ConfigParser.h"
#include "MulticastComm.h"
#include "MVCTPManager.h"

extern ConfigParser* ptr_parser;

class MVCReceiver {
public:
	MVCReceiver();
	~MVCReceiver();
	int JoinGroup(string addr);
	int IPReceiveData(void* buff, size_t len, int flags, SA* from, socklen_t* from_len);
	int JoinGroup(const unsigned char* mac_addr);
	int ReceiveData(void* buff, size_t len);

private:
	MulticastComm comm;
	MVCTPManager mvctp_manager;
};


#endif /* MVCRECEIVER_H_ */
