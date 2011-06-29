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

extern ConfigParser* ptr_parser;

class MVCReceiver {
public:
	MVCReceiver();
	~MVCReceiver();
	int JoinGroup(string addr);
	int ReceiveData(void* buff, size_t len, int flags, SA* from, socklen_t* from_len);

private:
	MulticastComm comm;
};


#endif /* MVCRECEIVER_H_ */
