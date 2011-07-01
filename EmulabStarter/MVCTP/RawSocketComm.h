/*
 * RawSocketComm.h
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */

#ifndef RAWSOCKETCOMM_H_
#define RAWSOCKETCOMM_H_

#include "mvctp.h"
#include <cstdio>

class RawSocketComm {
public:
	RawSocketComm(const char* if_name);
	int SendData(const unsigned char* dest_addr, void* buffer, size_t length);
	int ReceiveFrame(void* buffer);

private:
	int sockfd;
	int if_index;
	struct ifreq if_req;
	unsigned char mac_addr[6];
	struct sockaddr_ll dest_address; 	// target address
	char frame_buf[ETH_FRAME_LEN];
	unsigned char* etherhead;
	unsigned char* data;
	struct ethhdr* eh;

	string GetMacAddrString(const unsigned char* addr);
};

#endif /* RAWSOCKETCOMM_H_ */
