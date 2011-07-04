/*
 * MVCTPManager.cpp
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */

#include "MVCTPManager.h"

MVCTPManager::MVCTPManager() {
	send_mvctp_header = (PTR_MVCTP_HEADER)send_packet_buf;
	send_data = send_packet_buf + sizeof(MVCTP_HEADER);

	eth_header = (struct ethhdr *)recv_frame_buf;
	recv_mvctp_header = (PTR_MVCTP_HEADER)(recv_frame_buf + ETH_HLEN);
	recv_data = (u_char*)recv_frame_buf + ETH_HLEN + sizeof(MVCTP_HEADER);

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



int MVCTPManager::JoinGroup(u_int32_t group_id, u_short port) {
	mvctp_group_id = group_id;
	SetMulticastMacAddress(mac_group_addr, group_id);
	send_mvctp_header->group_id  = group_id;
	send_mvctp_header->src_port = port;
	return 1;
}

int MVCTPManager::Send(void* buffer, size_t length) {
	int remained_len = length;
	u_char* pos = (u_char*)buffer;
	while (remained_len > MVCTP_DATA_LEN) {
		memcpy(send_data, pos, MVCTP_DATA_LEN);
		raw_sock_comm->SendFrame(mac_group_addr, send_packet_buf, MVCTP_PACKET_LEN);
		pos += MVCTP_DATA_LEN;
		remained_len -= MVCTP_DATA_LEN;
	}

	if (remained_len > 0) {
		memcpy(send_data, pos, remained_len);
		raw_sock_comm->SendFrame(mac_group_addr, send_packet_buf, MVCTP_HLEN + remained_len);
	}

	return length;
}


int MVCTPManager::Receive(void* buffer, size_t length) {
	size_t remained_len = length;
	size_t received_bytes = 0;
	char* ptr = (char*)buffer;

	int bytes;
	while (remained_len > 0) {
		if ( (bytes = raw_sock_comm->ReceiveFrame(recv_frame_buf)) < 0) {
			return received_bytes;
		}

		if (IsMyPacket()) {
			int data_len = bytes - ETH_HLEN - sizeof(MVCTP_HEADER);
			memcpy(ptr, recv_data, data_len);
			received_bytes += data_len;
			ptr += data_len;
			remained_len -= data_len;
			return received_bytes;
		}
	}

	return length;
}


bool MVCTPManager::IsMyPacket() {
	if (memcmp(eth_header->h_dest, mac_group_addr, 6) == 0 &&
			recv_mvctp_header->group_id == mvctp_group_id)
		return true;
	else
		return false;
}

void MVCTPManager::SetMulticastMacAddress(u_char* mac_addr, u_int ip_addr) {
	u_char* ptr = (u_char*)&ip_addr;
	mac_addr[0] = 0x01;
	mac_addr[1]	= 0x00;
	mac_addr[2] = 0x5e;
	mac_addr[3] = ptr[1] & 0x7f;
	mac_addr[4] = ptr[2];
	mac_addr[5] = ptr[3];
}
