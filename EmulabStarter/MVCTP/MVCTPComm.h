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
#include "SendBufferMgr.h"
#include "ReceiveBufferMgr.h"


class MVCTPComm {
public:
	MVCTPComm(int send_buf_size, int recv_buf_size);
	~MVCTPComm();

	int JoinGroup(string addr, ushort port);
	int RawSend(char* data, size_t length);
	int RawReceive(void* buff, size_t len);
	int IPSend(char* data, size_t length);
	int IPReceive(void* buff, size_t len, int flags, SA* from, socklen_t* from_len);


private:
	string		sender_ip;

	MVCTPManager 		mvctp_manager;
	MulticastComm* 		comm;

	SendBufferMgr		*send_buf;
	ReceiveBufferMgr 	*recv_buf;
	u_int32_t 			group_id;
};

#endif /* MVCTPCOMM_H_ */
