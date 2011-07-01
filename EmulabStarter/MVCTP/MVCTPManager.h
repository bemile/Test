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
#include "NetInterfaceManager.h"

class MVCTPManager {
public:
	MVCTPManager();
	~MVCTPManager();
	int JoinGroup(u_int32_t group_id);
	int Send(void* buffer, size_t length);
	int Receive(void* buffer, size_t length);

private:
	NetInterfaceManager* if_manager;
	RawSocketComm* raw_sock_comm;
	u_int32_t mvctp_group_id;
	u_char mac_group_addr[6];

	// single MVCTP packet send buffer
	char send_packet_buf[ETH_DATA_LEN];
	MVCTP_HEADER* send_mvctp_header;
	char* send_data;

	// single MAC frame receive buffer
	char recv_frame_buf[ETH_FRAME_LEN];
	struct ethhdr* eth_header;
	MVCTP_HEADER* recv_mvctp_header;
	u_char* recv_data;




	bool IsMyPacket();
	void SetMulticastMacAddress(u_char* mac_addr, u_int ip_addr);

};
#endif /* MVCTPMANAGER_H_ */
