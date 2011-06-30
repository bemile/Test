/*
 * RawSocketComm.h
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */

#ifndef RAWSOCKETCOMM_H_
#define RAWSOCKETCOMM_H_

#include "mvctp.h"

class RawSocketComm {
public:
	RawSocketComm();
	int SendData(unsigned char* dest_addr, void* buffer, size_t length);
	int ReceiveFrame(void* buffer);

private:
	int sockfd;
	struct ifreq if_req;
	char mac_addr[6];
	struct sockaddr_ll dest_address; 	// target address
	unsigned char frame_buf[ETH_FRAME_LEN];
	unsigned char* etherhead;
	unsigned char* data;
	struct ethhdr* eh;

	void SysError(string s);
};

#endif /* RAWSOCKETCOMM_H_ */
