/*
 * MVCTPComm.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef MVCTPCOMM_H_
#define MVCTPCOMM_H_

#include "mvctp.h"
#include "MulticastComm.h"
#include "MVCTPManager.h"

class MVCTPComm {
public:
	MVCTPComm();
	~MVCTPComm();

	int JoinGroup(string addr);
	int Send(char* data, size_t length);
	int Receive(void* buff, size_t len);
	int IPSend(char* data, size_t length);
	int IPReceive(void* buff, size_t len, int flags, SA* from, socklen_t* from_len);


private:
	MVCTPManager mvctp_manager;
	MulticastComm comm;
	u_int32_t group_id;
};

#endif /* MVCTPCOMM_H_ */
