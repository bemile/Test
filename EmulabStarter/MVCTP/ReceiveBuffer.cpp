/*
 * ReceiveBuffer.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "ReceiveBuffer.h"

ReceiveBuffer::ReceiveBuffer(int size, MulticastComm* mcomm): MVCTPBuffer(size, mcomm) {
	StartReceiveThread();
}

ReceiveBuffer::~ReceiveBuffer() {
	pthread_mutex_destroy(&buf_mutex);
	pthread_mutex_destroy(&empty_mutex);
}

size_t ReceiveBuffer::GetData(void* buff, size_t len) {
	char* pos = (char*)buff;
	size_t bytes_copied = 0;
	size_t bytes_remained = len;
	u_int32_t last_pid = 0;

	//wait until there are some data in the buffer
	while (IsEmpty()) {
		usleep(10000);
	}

	BufferEntry *tmp = NULL;
	int sleep_turns = 0;
	while (true) {
		pthread_mutex_lock(&buf_mutex);
		for (tmp = Begin(); tmp != End(); tmp = tmp->next) {
			last_pid = tmp->packet_id;
			if (bytes_remained <= 0)
				break;
			else if (bytes_remained < tmp->data_len) {
				memcpy(pos, tmp->data, bytes_remained);
				pos += bytes_remained;
				bytes_copied += bytes_remained;
				bytes_remained = 0;
				ShrinkEntry(tmp, tmp->data_len - bytes_remained);
				break;
			}
			else {
				memcpy(pos, tmp->data, tmp->data_len);
				pos += tmp->data_len;
				bytes_copied += tmp->data_len;
				bytes_remained -= tmp->data_len;
			}
		}

		DeleteUntil(tmp);
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



void ReceiveBuffer::StartReceiveThread() {
	pthread_mutex_init(&buf_mutex, 0);
	pthread_mutex_init(&empty_mutex, 0);
	int val = pthread_create(&thread, NULL, &ReceiveBuffer::StartReceivingData, this);
}


void* ReceiveBuffer::StartReceivingData(void* ptr) {
	((ReceiveBuffer*)ptr)->Run();
	return NULL;
}


void ReceiveBuffer::Run() {
	MVCTP_HEADER header;
	while (true) {
		if (comm->RecvData(&header, sizeof(MVCTP_HEADER), 0, NULL, NULL) <= 0) {
			SysError("MVCTPBuffer error on receiving data");
		}

		char* data = (char*)malloc(header.data_len);
		if (comm->RecvData(data, header.data_len, 0, NULL, NULL) <= 0) {
			SysError("MVCTPBuffer error on receiving data");
		}

		pthread_mutex_lock(&buf_mutex);
		AddData(&header, data);
		pthread_mutex_unlock(&buf_mutex);
	}
}


