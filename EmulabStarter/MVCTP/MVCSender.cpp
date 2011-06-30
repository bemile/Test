/*
 * MVCSender.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "MVCSender.h"

MVCSender::MVCSender() {
}

MVCSender::~MVCSender() {
}


int MVCSender::JoinGroup(string addr) {
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT_NUM);
	inet_pton(AF_INET, addr.c_str(), &sa.sin_addr);

	return comm.JoinGroup((SA *)&sa, sizeof(sa), (char*)NULL);
}


int MVCSender::JoinGroup(const unsigned char* group_addr) {
	return mvctp_manager.JoinGroup(group_addr);
}


int MVCSender::Send(char* data, size_t length) {
	return mvctp_manager.Send(data, length);
}

int MVCSender::IPSend(char* data, size_t length) {
	return comm.SendData(data, length, 0);
}
