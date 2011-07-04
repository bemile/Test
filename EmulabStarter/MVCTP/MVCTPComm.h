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
#include "MVCTPBuffer.h"



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
	MVCTPManager 	mvctp_manager;
	MulticastComm* 	comm;
	MVCTPBuffer		*send_buf, *recv_buf;
	u_int32_t 		group_id;
};

#endif /* MVCTPCOMM_H_ */
