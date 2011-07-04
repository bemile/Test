/*
 * MVCTPBuffer.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "MVCTPBuffer.h"

MVCTPBuffer::MVCTPBuffer(int size, MulticastComm* mcomm) {
	max_size = size;
	num_entry = 0;
	actual_size = 0;
	last_packet_id = 0;

	nil = (BufferEntry*) malloc(sizeof(BufferEntry));
	memset(nil, 0, sizeof(BufferEntry));
	nil->prev = nil;	// list tail
	nil->next = nil;	// list head

	comm = mcomm;
	StartReceiveThread();
}


MVCTPBuffer::~MVCTPBuffer() {
	pthread_mutex_destroy(&buf_mutex);
	pthread_mutex_destroy(&empty_mutex);
}

size_t MVCTPBuffer::GetDataSize() {
	return actual_size;
}


size_t MVCTPBuffer::GetData(void* buff, size_t len) {
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


int MVCTPBuffer::AddData(MVCTP_HEADER* header, char* data) {
	BufferEntry* entry = (BufferEntry*)malloc(sizeof(BufferEntry));
	entry->packet_id = header->packet_id;
	entry->data_len = header->data_len;
	entry->data = data;

	PushBack(entry);

	actual_size += header->data_len;
	last_packet_id = header->packet_id;
	num_entry++;

	return 1;
}


BufferEntry* MVCTPBuffer::Begin() {
	return nil->next;
}


BufferEntry* MVCTPBuffer::End() {
	return nil->prev;
}

bool MVCTPBuffer::IsEmpty() {
	return (Begin() == nil);
}


int MVCTPBuffer::PushBack(BufferEntry* entry) {
	return Insert(End(), entry);
}


int MVCTPBuffer::Insert(BufferEntry* pos, BufferEntry* entry) {
	entry->prev = pos->prev;
	entry->next = pos;
	pos->prev = pos->prev->next = entry;
	return 1;
}


int MVCTPBuffer::Delete(BufferEntry* entry) {
	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	DestroyEntry(entry);

	return 1;
}


int MVCTPBuffer::DeleteUntil(BufferEntry* entry) {
	if (entry == NULL)
		return 0;

	BufferEntry *tmp = NULL;
	for (tmp = Begin(); tmp != End(); tmp = tmp->next) {
		if (tmp == entry)
			break;

		Delete(tmp);
	}
	return 1;
}


BufferEntry* MVCTPBuffer::Find(u_int32_t pid) {
	BufferEntry *tmp = NULL;
	for (tmp = Begin(); tmp != End(); tmp = tmp->next) {
		if (tmp->packet_id == pid) {
			break;
		}
	}
	return tmp;
}


int MVCTPBuffer::ShrinkEntry(BufferEntry* entry, size_t new_size) {
	char* new_data = (char*)malloc(new_size);
	memcpy(new_data, entry->data + (entry->data_len - new_size), new_size);
	delete entry->data;
	entry->data = new_data;
	entry->data_len = new_size;

	return 1;
}

void MVCTPBuffer::DestroyEntry(BufferEntry* entry) {
	delete entry->data;
	delete entry;
}



void MVCTPBuffer::StartReceiveThread() {
	pthread_mutex_init(&buf_mutex, 0);
	pthread_mutex_init(&empty_mutex, 0);
	int val = pthread_create(&thread, NULL, &MVCTPBuffer::StartReceivingData, this);
}


void* MVCTPBuffer::StartReceivingData(void* ptr) {
	((MVCTPBuffer*)ptr)->Run();
	return NULL;
}


void MVCTPBuffer::Run() {
	MVCTP_HEADER header;
	bool is_empty = false;
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



