/*
 * MVCTPManager.cpp
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */

#include "MVCTPManager.h"

MVCTPManager::MVCTPManager() {
	etherhead = frame_buf;
	data = frame_buf + ETH_HLEN;
	eh = (struct ethhdr *)etherhead;
}

int MVCTPManager::JoinGroup(const unsigned char* mac_addr) {
	memcpy(group_addr, mac_addr, 6);
	return 1;
}

int MVCTPManager::Send(void* buffer, size_t length) {
	raw_sock_comm.SendData(group_addr, buffer, length);
}

int MVCTPManager::Receive(void* buffer, size_t length) {
	size_t remained_len = length;
	size_t received_bytes = 0;
	char* ptr = (char*)buffer;

	int bytes;
	while (remained_len > 0) {
		if ( (bytes = raw_sock_comm.ReceiveFrame(frame_buf)) < 0) {
			return received_bytes;
		}

		if (MatchAddress()) {
			int data_len = bytes - ETH_HLEN;
			memcpy(ptr, data, data_len);
			received_bytes += data_len;
			ptr += data_len;
			remained_len -= data_len;
			return received_bytes;
		}
	}

	return length;
}


bool MVCTPManager::MatchAddress() {
	if (memcmp(eh->h_dest, group_addr, 6) == 0)
		return true;
	else
		return false;
}
