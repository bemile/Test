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

	if_manager = new NetInterfaceManager();
	string if_name;
	for (PTR_IFI_INFO ptr_ifi = if_manager->GetIfiHead(); ptr_ifi != NULL;
			ptr_ifi = ptr_ifi->ifi_next) {
		if_name = ptr_ifi->ifi_name;
		if (if_name.find("eth") != string::npos) {
			break;
		}
	}
	cout << "interface name: " << if_name << endl;
	raw_sock_comm = new RawSocketComm(if_name.c_str());
}

MVCTPManager::~MVCTPManager() {
	delete if_manager;
	if_manager = NULL;
	delete raw_sock_comm;
	raw_sock_comm = NULL;
}



int MVCTPManager::JoinGroup(const unsigned char* mac_addr) {
	memcpy(group_addr, mac_addr, 6);
	return 1;
}

int MVCTPManager::Send(void* buffer, size_t length) {
	raw_sock_comm->SendData(group_addr, buffer, length);
}

int MVCTPManager::Receive(void* buffer, size_t length) {
	size_t remained_len = length;
	size_t received_bytes = 0;
	char* ptr = (char*)buffer;

	int bytes;
	while (remained_len > 0) {
		if ( (bytes = raw_sock_comm->ReceiveFrame(frame_buf)) < 0) {
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
