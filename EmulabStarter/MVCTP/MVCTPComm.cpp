/*
 * MVCTPComm.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "MVCTPComm.h"

MVCTPComm::MVCTPComm() {
}

MVCTPComm::~MVCTPComm() {
}


int MVCTPComm::JoinGroup(string addr) {
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT_NUM);
	inet_pton(AF_INET, addr.c_str(), &sa.sin_addr);

	group_id = sa.sin_addr.s_addr;
	mvctp_manager.JoinGroup(group_id);

	return comm.JoinGroup((SA *)&sa, sizeof(sa), (char*)NULL);
}


int MVCTPComm::Send(char* data, size_t length) {
	return mvctp_manager.Send(data, length);
}

ssize_t MVCTPComm::Receive(void* buff, size_t len) {
	return mvctp_manager.Receive(buff, len);
}


int MVCTPComm::IPSend(char* data, size_t length) {
	return comm.SendData(data, length, 0);
}

ssize_t MVCTPComm::IPReceive(void* buff, size_t len, int flags, SA* from, socklen_t* from_len) {
	return comm.RecvData(buff, len, flags, from, from_len);
}

