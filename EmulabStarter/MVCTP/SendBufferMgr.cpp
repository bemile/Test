/*
 * SendBufferMgr.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "SendBufferMgr.h"

SendBufferMgr::SendBufferMgr(int size, InetComm* mcomm) {
	last_packet_id = 0;

	send_buf = new MVCTPBuffer(size);
	comm = mcomm;
	udp_comm = new UdpComm(BUFFER_UDP_SEND_PORT);

	StartUdpThread();
}

SendBufferMgr::~SendBufferMgr() {
	pthread_mutex_destroy(&buf_mutex);
	delete udp_comm;
	delete send_buf;
}


void SendBufferMgr::SetBufferSize(size_t buff_size) {
	send_buf->SetMaxBufferSize(buff_size);
}

// Send out data through the multicast socket
// send_out: whether or not actually send out the packet (for testing reliability)
//              should be removed in real implementation
int SendBufferMgr::SendData(const char* data, size_t length, void* dst_addr, bool send_out) {
	int bytes_left = length;
	int bytes_sent = 0;
	const char* pos = data;

	pthread_mutex_lock(&buf_mutex);
	while (bytes_left > 0) {
		int len = bytes_left > MVCTP_DATA_LEN ? MVCTP_DATA_LEN : bytes_left;

		char* ptr_data = (char*) malloc(len + MVCTP_HLEN);
		MVCTP_HEADER* header = (MVCTP_HEADER*)ptr_data;
		header->packet_id = ++last_packet_id;
		header->data_len = len;
		memcpy(ptr_data + MVCTP_HLEN, pos, len);

		BufferEntry* entry = (BufferEntry*) malloc(sizeof(BufferEntry));
		entry->packet_id = header->packet_id;
		entry->data_len = len + MVCTP_HLEN;
		entry->data = ptr_data;

		SendPacket(entry, dst_addr, send_out);

		pos += len;
		bytes_left -= len;
		bytes_sent += len;
	}
	pthread_mutex_unlock(&buf_mutex);

	return bytes_sent;
}


void SendBufferMgr::SendPacket(BufferEntry* entry, void* dst_addr, bool send_out) {
	size_t avail_buf_size = send_buf->GetAvailableBufferSize();
	if (avail_buf_size < entry->data_len) {
		MakeRoomForNewPacket(entry->data_len - avail_buf_size);
	}
	send_buf->PushBack(entry);

	if (send_out) {
		if (comm->SendData(entry->data, entry->data_len, 0, dst_addr) < 0) {
			SysError("SendBufferMgr::SendPacket()::SendData() error");
		}
		//cout << "Successfully sent packet. Packet length: " << entry->data_len << endl;
	}
}


void SendBufferMgr::MakeRoomForNewPacket(size_t room_size) {
	size_t size = 0;
	BufferEntry* it;
	for (it = send_buf->Begin(); it != send_buf->End(); it = it->next) {
		if (size > room_size)
			break;

		size += it->data_len;
	}

	send_buf->DeleteUntil(it);
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
	NackMsg*	nack_msg = (NackMsg*)buf;
	int bytes;

	while (true) {
		if ( (bytes = udp_comm->RecvFrom(buf, UDP_PACKET_LEN, 0, (SA*)&sender_addr, &sender_socklen)) <= 0) {
			SysError("ReceiveBufferMgr::UdpReceive()::RecvData() error");
		}

		if (nack_msg->proto == MVCTP_PROTO_TYPE) {
			//cout << "One retransmission request received. Packet Number: " << nack_msg->num_missing_packets << endl;
			Retransmit(nack_msg);
		}
	}
}


void SendBufferMgr::Retransmit(NackMsg* ptr_msg) {
	pthread_mutex_lock(&buf_mutex);
	for (int i = 0; i < ptr_msg->num_missing_packets; i++) {
		int32_t packet_id = ptr_msg->packet_ids[i];
		for (BufferEntry* it = send_buf->Back(); it != send_buf->Begin()->prev; it = it->prev) {
			if (it->packet_id == packet_id) {
				udp_comm->SendTo((void *)it->data, it->data_len, 0,
							(SA*)&sender_addr, sender_socklen);
				cout << "One packet retransmitted. Packet ID: " << packet_id << endl;
			}
		}
	}
	pthread_mutex_unlock(&buf_mutex);
}

