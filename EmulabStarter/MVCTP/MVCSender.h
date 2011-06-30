/*
 * MVCSender.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef MVCSENDER_H_
#define MVCSENDER_H_

#include "mvctp.h"
#include "MulticastComm.h"
#include "MVCTPManager.h"

class MVCSender {
public:
	MVCSender();
	~MVCSender();

	int JoinGroup(const unsigned char* group_addr);
	int Send(char* data, size_t length);
	int JoinGroup(string addr);
	int IPSend(char* data, size_t length);


private:
	MVCTPManager mvctp_manager;
	MulticastComm comm;
};

#endif /* MVCSENDER_H_ */
