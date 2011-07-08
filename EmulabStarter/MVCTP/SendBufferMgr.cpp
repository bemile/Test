/*
 * SendBufferMgr.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "SendBufferMgr.h"

SendBufferMgr::SendBufferMgr(int size, MulticastComm* mcomm) {
	max_size = size;
	num_entry = 0;
	actual_size = 0;
	last_packet_id = 0;

	send_buf = new MVCTPBuffer(size);
	comm = mcomm;
	udp_comm = new UdpComm(BUFFER_UDP_PORT);

	void StartUdpThread();
}

SendBufferMgr::~SendBufferMgr() {
	pthread_mutex_destroy(&buf_mutex);
	delete udp_comm;
	delete send_buf;
}


int SendBufferMgr::SendData(char* data, size_t length) {
	int bytes_left = length;
	int bytes_sent = 0;
	char* pos = data;

	pthread_mutex_lock(&buf_mutex);
	while (bytes_left > 0) {
		int len;
		if (bytes_left > UDP_MVCTP_DATA_LEN)
			len = UDP_MVCTP_DATA_LEN;
		else
			len = bytes_left;

		char* ptr_data = (char*) malloc(len + MVCTP_HLEN);
		MVCTP_HEADER* header = (MVCTP_HEADER*)ptr_data;
		header->packet_id = ++last_packet_id;
		header->data_len = len;
		memcpy(ptr_data + MVCTP_HLEN, pos, len);

		BufferEntry* entry = (BufferEntry*) malloc(sizeof(BufferEntry));
		entry->packet_id = header->packet_id;
		entry->data_len = len + MVCTP_HLEN;
		entry->data = ptr_data;

		if (SendPacket(entry) <= 0) {
			SysError("SendBuffer error on sending packet");
		}

		pos += len;
		bytes_left -= len;
		bytes_sent += len;

		send_buf->PushBack(entry);
		actual_size += entry->data_len;
		num_entry++;
	}
	pthread_mutex_unlock(&buf_mutex);

	return bytes_sent;
}


int SendBufferMgr::SendPacket(BufferEntry* entry) {
	return comm->SendData(entry->data, entry->data_len, 0);
}


void SendBufferMgr::StartUdpThread() {
	pthread_mutex_init(&buf_mutex, 0);

	int res;
	if ( (res= pthread_create(&udp_thread, NULL, &SendBufferMgr::StartUdpNackReceive, this)) != 0) {
		SysError("SendBufferMgr:: pthread_create() error");
	}
}


void* SendBufferMgr::StartUdpNackReceive(void* ptr) {
	((SendBufferMgr*)ptr)->ReceiveNack();
	pthread_exit(NULL);
}


void SendBufferMgr::ReceiveNack() {
	char buf[UDP_PACKET_LEN];
	NackMsg*	nack_msg;
	int bytes;

	while (true) {
		if ( (bytes = udp_comm->RecvFrom(buf, UDP_PACKET_LEN, 0, (SA*)&sender_addr, &sender_socklen)) <= 0) {
			SysError("ReceiveBufferMgr::UdpReceive()::RecvData() error");
		}

		nack_msg = (NackMsg*)buf;
		Retransmit(nack_msg->packet_id);
	}
}


void SendBufferMgr::Retransmit(u_int32_t packet_id) {
	pthread_mutex_lock(&buf_mutex);
	for (BufferEntry* it = send_buf->End()->prev; it != send_buf->Begin()->prev; it = it->prev) {
		if (it->packet_id == packet_id)
			udp_comm->SendTo((void *)it->data, it->data_len, 0, (SA*)&sender_addr, sender_socklen);;
	}
	pthread_mutex_unlock(&buf_mutex);
}

