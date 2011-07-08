/*
 * MVCTPComm.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "MVCTPComm.h"

MVCTPComm::MVCTPComm(int send_buf_size, int recv_buf_size) {
	comm = new MulticastComm();

	send_buf = new SendBufferMgr(send_buf_size, comm);
	recv_buf = new ReceiveBufferMgr(recv_buf_size, comm);
}


MVCTPComm::~MVCTPComm() {
	delete send_buf;
	send_buf = NULL;
	delete recv_buf;
	recv_buf = NULL;
	delete comm;
	comm = NULL;
}



int MVCTPComm::JoinGroup(string addr, ushort port) {
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT_NUM);
	inet_pton(AF_INET, addr.c_str(), &sa.sin_addr);

	group_id = sa.sin_addr.s_addr;
	mvctp_manager.JoinGroup(group_id, port);

	comm->JoinGroup((SA *)&sa, sizeof(sa), (char*)NULL);
	recv_buf->StartReceiveThread();
	return 1;
}


int MVCTPComm::RawSend(char* data, size_t length) {
	return mvctp_manager.Send(data, length);
}

ssize_t MVCTPComm::RawReceive(void* buff, size_t len) {
	return mvctp_manager.Receive(buff, len);
}


int MVCTPComm::IPSend(char* data, size_t length) {
	return send_buf->SendData(data, length);
	//return comm->SendData(data, length, 0);
}

ssize_t MVCTPComm::IPReceive(void* buff, size_t len, int flags, SA* from, socklen_t* from_len) {
	size_t bytes = recv_buf->GetData(buff, len);
	return bytes;
}

