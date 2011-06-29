/*
 * MVCReceiver.cpp
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#include "MVCReceiver.h"

MVCReceiver::MVCReceiver() {
}

MVCReceiver::~MVCReceiver() {

}


int MVCReceiver::JoinGroup(string addr) {
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT_NUM);
	inet_pton(AF_INET, addr.c_str(), &sa.sin_addr);

	return comm.JoinGroup((SA *) &sa, sizeof(sa), (char*) NULL);
}


ssize_t MVCReceiver::ReceiveData(void* buff, size_t len, int flags, SA* from, socklen_t* from_len) {
	return comm.RecvData(buff, len, flags, from, from_len);
}

