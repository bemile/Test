/*
 * MVCTPManager.h
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */

#ifndef MVCTPMANAGER_H_
#define MVCTPMANAGER_H_

#include "mvctp.h"
#include "RawSocketComm.h"

class MVCTPManager {
public:
	MVCTPManager();
	int JoinGroup(const unsigned char* mac_addr);
	int Send(void* buffer, size_t length);
	int Receive(void* buffer, size_t length);

private:
	RawSocketComm raw_sock_comm;
	unsigned char group_addr[6];
	char frame_buf[ETH_FRAME_LEN];
	char* etherhead;
	char* data;
	struct ethhdr* eh;

	bool MatchAddress();

};
#endif /* MVCTPMANAGER_H_ */
