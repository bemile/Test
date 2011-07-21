/*
 * ReceiveBufferMgr.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "ReceiveBufferMgr.h"

ReceiveBufferMgr::ReceiveBufferMgr(int size, InetComm* mcomm) {
	max_size = size;
	num_entry = 0;
	actual_size = 0;
	last_recv_packet_id = 0;
	last_del_packet_id = 0;

	recv_buf = new MVCTPBuffer(size);
	comm = mcomm;
	udp_comm = new UdpComm(BUFFER_UDP_RECV_PORT);

	//TODO: remove this after the problem of getting sender address info is solved
	hostent * record = gethostbyname("node0.ldm-test.MVC.emulab.net");
	sender_udp_addr.sin_port = htons(BUFFER_UDP_SEND_PORT);
	sender_udp_addr.sin_family = AF_INET;

	in_addr * address = (in_addr * )record->h_addr;
	memcpy(&sender_udp_addr.sin_addr, address, sizeof(address));
	cout << "Sender address: " << inet_ntoa(*address) << endl;
	//inet_pton(AF_INET, record->h_addr_list, &sender_udp_addr.sin_addr);
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

	//wait until there are some data in the buffer
	while (recv_buf->IsEmpty() || recv_buf->Front()->packet_id != last_del_packet_id  + 1) {
		usleep(10000);
	}

	int sleep_turns = 0;
	while (true) {
		pthread_mutex_lock(&buf_mutex);
		int32_t last_pid = recv_buf->Front()->packet_id - 1;
		BufferEntry *tmp;
		for (tmp = recv_buf->Begin(); tmp != recv_buf->End(); tmp = tmp->next) {
			if (tmp->packet_id != (last_pid + 1))
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
			} else {
				memcpy(pos, tmp->data, tmp->data_len);
				pos += tmp->data_len;
				bytes_copied += tmp->data_len;
				bytes_remained -= tmp->data_len;
			}
		}

		if (tmp != recv_buf->Begin()) {
			last_del_packet_id = tmp->prev->packet_id;
			recv_buf->DeleteUntil(tmp);
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
	char buf[ETH_DATA_LEN];
	header = (MVCTP_HEADER*)buf;
	bool is_first_packet = true;

	while (true) {
		if ( (bytes = comm->RecvData(buf, ETH_DATA_LEN, 0, (SA*)&sender_multicast_addr, &sender_socklen)) <= 0) {
			SysError("MVCTPBuffer error on receiving data");
		}
		//cout << "I received one packet. Packet length: " << bytes << endl;

		char* data = (char*)malloc(header->data_len);
		memcpy(data, buf + MVCTP_HLEN, header->data_len);

		// Initialize the packet id information on receiving the first packet
		if (is_first_packet) {
			char ip[20];
			sockaddr_in * ptr_sock = (sockaddr_in *)&sender_multicast_addr;
			inet_ntop(AF_INET, (void*)&(ptr_sock->sin_addr), ip, 20);
			ip[15] = 0;
			cout << "Sender IP address: " << ip << endl;
			cout << "Sender Port: " << ntohs(ptr_sock->sin_port) << endl;

			last_recv_packet_id = header->packet_id - 1;
			last_del_packet_id = header->packet_id - 1;
			is_first_packet = false;
		}

		// Record missing packets if there is a gap in the packet_id
		if (header->packet_id - last_recv_packet_id > 1) {
			cout << "Packet loss detected. Received Packet ID: " << header->packet_id
					<< "  Supposed ID:" << last_recv_packet_id + 1 << endl;

			pthread_mutex_lock(&nack_list_mutex);
			clock_t time = clock() - 0.1 * CLOCKS_PER_SEC;
			for (int32_t i = last_recv_packet_id + 1; i != header->packet_id; i++) {
				NackMsgInfo info;
				info.packet_id = i;
				info.time_stamp = time;
				info.num_retries = 0;
				missing_packet_list.push_back(info);
			}
			pthread_mutex_unlock(&nack_list_mutex);
			cout << "Missing packets added to the retransmit list." << endl;
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
			if ( (cur_time - it->time_stamp) > INIT_RTT / 1000 * CLOCKS_PER_SEC ) {
				SendNackMsg(it->packet_id);
				it->num_retries++;
				it->time_stamp = cur_time;
				cout << "One NACK message sent." << endl;
			}
		}
		pthread_mutex_unlock(&nack_list_mutex);

		usleep(10000);
	}
}


// Send a retransmission request to the sender
int ReceiveBufferMgr::SendNackMsg(int32_t packet_id) {
	NackMsg msg;
	msg.packet_id = packet_id;
	return udp_comm->SendTo((void *)&msg, sizeof(msg), 0,
			(SA*)&sender_udp_addr, sizeof(sender_udp_addr));
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

		cout << "One retransmission packet received. Packet ID: " << header->packet_id << endl;
		// Discard duplicated packet that has already been used and deleted from the buffer
		if (header->packet_id <= last_del_packet_id)
			continue;

		char* data = (char*)malloc(header->data_len);
		memcpy(data, buf + MVCTP_HLEN, header->data_len);
		pthread_mutex_lock(&buf_mutex);
		recv_buf->AddEntry(header, data);
		actual_size += header->data_len;
		num_entry++;
		pthread_mutex_unlock(&buf_mutex);

		cout << "Retransmission packet added to the buffer." << endl;
	}
}


void ReceiveBufferMgr::DeleteNackFromList(int32_t packet_id) {
	pthread_mutex_lock(&nack_list_mutex);
	list<NackMsgInfo>::iterator it;
	for (it = missing_packet_list.begin(); it != missing_packet_list.end(); it++) {
		if (it->packet_id == packet_id) {
			missing_packet_list.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(&nack_list_mutex);
}



