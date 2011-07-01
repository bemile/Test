/*
 * RawSocketComm.cpp
 *
 *  Created on: Jun 29, 2011
 *      Author: jie
 */
#include "RawSocketComm.h"

RawSocketComm::RawSocketComm(const char* if_name) {
	if ( (sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
			SysError("Cannot create new socket.");
	}

	/*get the index of the network device*/
	memset(&if_req, 0, sizeof(if_req));
	strncpy(if_req.ifr_name, if_name, sizeof(if_req.ifr_name));
	if (ioctl(sockfd, SIOCGIFINDEX, &if_req)) {
		SysError("unable to get index");
	}
	if_index = if_req.ifr_ifindex;

	if (ioctl(sockfd, SIOCGIFHWADDR, &if_req) < 0) {
		SysError("Cannot get network interface info: ");
	}
	memcpy(mac_addr, if_req.ifr_addr.sa_data, ETH_ALEN);

	//int if_index = 2;
	//if (if_indextoname(if_index, if_req.ifr_name) == NULL) {
	//	SysError("Cannot find network interface.");
	//}

	dest_address.sll_family = AF_PACKET;
	dest_address.sll_protocol = htons(ETH_P_ALL);
	dest_address.sll_ifindex = if_index;
	dest_address.sll_hatype = 1;  	//ARPHRD_ETHER;
	dest_address.sll_pkttype = PACKET_OTHERHOST;
	dest_address.sll_halen = ETH_ALEN;

	etherhead = (unsigned char*)frame_buf;
	data = (unsigned char*)frame_buf + ETH_HLEN;
	eh = (struct ethhdr *)etherhead;
}


int RawSocketComm::SendData(const unsigned char* dest_addr, void* buffer, size_t length) {
	memcpy(dest_address.sll_addr, dest_addr, ETH_ALEN);
	dest_address.sll_addr[6] = 0x00;/*not used*/
	dest_address.sll_addr[7] = 0x00;/*not used*/

	/*set the frame header*/
	memcpy(frame_buf, dest_addr, ETH_ALEN);
	memcpy(frame_buf + ETH_ALEN, mac_addr, ETH_ALEN);
	eh->h_proto = htons(0x0000);

	int res;
	int remained_len = length;
	unsigned char* pos = (unsigned char*)buffer;
	while (remained_len > ETH_DATA_LEN) {
		memcpy(data, pos, ETH_DATA_LEN);
		if ( (res = sendto(sockfd, frame_buf, ETH_FRAME_LEN, 0,
				(SA*)&dest_address, sizeof(dest_address))) < 0) {
			SysError("sendto() error.");
		}
		pos += ETH_DATA_LEN;
		remained_len -= ETH_DATA_LEN;
	}

	if (remained_len > 0 ) {
		memcpy(data, pos, remained_len);
		if ( (res = sendto(sockfd, frame_buf, ETH_HLEN + remained_len, 0,
				(SA*)&dest_address, sizeof(dest_address))) < 0) {
			SysError("sendto() error.");
		}
	}

	return length;
}


int RawSocketComm::ReceiveFrame(void* buffer) {
	int bytes;
	if ( (bytes = recvfrom(sockfd, buffer, ETH_FRAME_LEN, 0, NULL, NULL)) < 0) {
		SysError("recvfrom() error.");
	}

	return bytes;
}


string RawSocketComm::GetMacAddrString(const unsigned char* addr) {
	char buff[50];
	sprintf(buff, "%#x:%#x:%#x:%#x:%#x:%#x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
	return buff;
}
