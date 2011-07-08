/*
 * ReceiveBufferMgr.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "ReceiveBufferMgr.h"

ReceiveBufferMgr::ReceiveBufferMgr(int size, MulticastComm* mcomm) {
	recv_buf = new MVCTPBuffer(size);
	comm = mcomm;
	udp_comm = new UdpComm(BUFFER_UDP_PORT);

	max_size = size;
	num_entry = 0;
	actual_size = 0;
	last_recv_packet_id = 0;
	last_del_packet_id = 0;

}


ReceiveBufferMgr::~ReceiveBufferMgr() {
	pthread_mutex_destroy(&buf_mutex);
	pthread_mutex_destroy(&nack_list_mutex);

	delete udp_comm;
	delete recv_buf;
}


size_t ReceiveBufferMgr::GetData(void* buff, size_t len) {
	char* pos = (char*)buff;
	size_t bytes_copied = 0;
	size_t bytes_remained = len;
	u_int32_t last_pid = recv_buf->Begin()->packet_id;

	//wait until there are some data in the buffer
	while (recv_buf->IsEmpty()) {
		usleep(10000);
	}

	BufferEntry *tmp = NULL;
	int sleep_turns = 0;
	while (true) {
		pthread_mutex_lock(&buf_mutex);
		if (recv_buf->Begin()->packet_id == last_del_packet_id  + 1) {
			for (tmp = recv_buf->Begin(); tmp != recv_buf->End(); tmp = tmp->next) {
				if (tmp->packet_id != (last_pid + 1) )
					break;

				last_pid = tmp->packet_id;
				if (bytes_remained <= 0)
					break;
				else if (bytes_remained < tmp->data_len) {
					memcpy(pos, tmp->data, bytes_remained);
					pos += bytes_remained;
					bytes_copied += bytes_remained;
					bytes_remained = 0;
					recv_buf->ShrinkEntry(tmp, tmp->data_len - bytes_remained);
					break;
				}
				else {
					memcpy(pos, tmp->data, tmp->data_len);
					pos += tmp->data_len;
					bytes_copied += tmp->data_len;
					bytes_remained -= tmp->data_len;
				}
			}

			if (tmp != NULL) {
				last_del_packet_id = tmp->packet_id - 1;
				recv_buf->DeleteUntil(tmp);
			}
		}
		pthread_mutex_unlock(&buf_mutex);

		if (bytes_remained <= 0 || sleep_turns >= 5) {
			return bytes_copied;
		}
		else {
			usleep(10000);
			sleep_turns++;
		}
	}

	return bytes_copied;
}



void ReceiveBufferMgr::StartReceiveThread() {
	pthread_mutex_init(&buf_mutex, 0);
	pthread_mutex_init(&nack_list_mutex, 0);
	int res;
	if ( (res= pthread_create(&recv_thread, NULL, &ReceiveBufferMgr::StartReceivingData, this)) != 0) {
		SysError("ReceiveBufferMgr:: pthread_create() error");
	}

	if ( (res= pthread_create(&nack_thread, NULL, &ReceiveBufferMgr::StartNackThread, this)) != 0) {
		SysError("ReceiveBufferMgr:: pthread_create() error");
	}

	if ( (res= pthread_create(&udp_thread, NULL, &ReceiveBufferMgr::StartUdpReceiveThread, this)) != 0) {
		SysError("ReceiveBufferMgr:: pthread_create() error");
	}
}

// Helper function to start the multicast data receiving thread
void* ReceiveBufferMgr::StartReceivingData(void* ptr) {
	((ReceiveBufferMgr*)ptr)->Run();
	pthread_exit(NULL);
}


void ReceiveBufferMgr::Run() {
	MVCTP_HEADER* header;
	int bytes;
	char buf[1500];
	header = (MVCTP_HEADER*)buf;
	bool is_first_packet = true;

	while (true) {
		if ( (bytes = comm->RecvData(buf, 1500, 0, (SA*)&sender_addr, &sender_socklen)) <= 0) {
			SysError("MVCTPBuffer error on receiving data");
		}

		char* data = (char*)malloc(header->data_len);
		memcpy(data, buf + MVCTP_HLEN, header->data_len);

		// Initialize the packet id information on receiving the first packet
		if (is_first_packet) {
			last_recv_packet_id = header->packet_id;
			last_del_packet_id = header->packet_id - 1;
			is_first_packet = false;
		}

		// Record missing packets if there is a gap in the packet_id
		if (header->packet_id - last_recv_packet_id > 1) {
			pthread_mutex_lock(&nack_list_mutex);
			clock_t time = clock() - 0.1 * CLOCKS_PER_SEC;
			for (u_int32_t i = last_recv_packet_id + 1; i != header->packet_id; i++) {
				NackMsgInfo info;
				info.packet_id = i;
				info.time_stamp = time;
				info.num_retries = 0;
				missing_packet_list.push_back(info);
			}
			pthread_mutex_unlock(&nack_list_mutex);
		}

		// Add the received packet to the buffer
		pthread_mutex_lock(&buf_mutex);
		recv_buf->AddEntry(header, data);
		actual_size += header->data_len;
		last_recv_packet_id = header->packet_id;
		num_entry++;
		pthread_mutex_unlock(&buf_mutex);
	}
}


void* ReceiveBufferMgr::StartNackThread(void* ptr) {
	((ReceiveBufferMgr*)ptr)->NackRun();
	pthread_exit(NULL);
}

// Keep checking the missing list for retransmission request
void ReceiveBufferMgr::NackRun() {
	list<NackMsgInfo>::iterator it;
	while (true) {
		clock_t cur_time = clock();
		pthread_mutex_lock(&nack_list_mutex);
		for (it = missing_packet_list.begin(); it != missing_packet_list.end(); it++) {
			if ( (cur_time - it->time_stamp) > INIT_RTT * CLOCKS_PER_SEC / 1000) {
				SendNackMsg(it->packet_id);
				it->num_retries++;
				it->time_stamp = cur_time;
			}
		}
		pthread_mutex_unlock(&nack_list_mutex);

		usleep(10000);
	}
}

// Helper function to start the unicast UDP thread
void* ReceiveBufferMgr::StartUdpReceiveThread(void* ptr) {
	((ReceiveBufferMgr*)ptr)->UdpReceive();
	pthread_exit(NULL);
}


// Receive retransmitted packets from the sender through UDP
void ReceiveBufferMgr::UdpReceive() {
	char buf[UDP_PACKET_LEN];
	MVCTP_HEADER* header = (MVCTP_HEADER*) buf;
	int bytes;

	while (true) {
		if ( (bytes = udp_comm->RecvFrom(buf, UDP_PACKET_LEN, 0, NULL, NULL)) <= 0) {
			SysError("ReceiveBufferMgr::UdpReceive()::RecvData() error");
		}

		char* data = (char*)malloc(header->data_len);
		memcpy(data, buf + MVCTP_HLEN, header->data_len);
		pthread_mutex_lock(&buf_mutex);
		recv_buf->AddEntry(header, data);
		actual_size += header->data_len;
		num_entry++;
		pthread_mutex_unlock(&buf_mutex);
	}
}

// Send a retransmission request to the sender
int ReceiveBufferMgr::SendNackMsg(u_int32_t packet_id) {
	NackMsg msg;
	msg.packet_id = packet_id;
	return udp_comm->SendTo((void *)&msg, sizeof(msg), 0, (SA*)&sender_addr, sender_socklen);
}

