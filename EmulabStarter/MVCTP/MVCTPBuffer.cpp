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

}


MVCTPBuffer::~MVCTPBuffer() {
}

size_t MVCTPBuffer::GetDataSize() {
	return actual_size;
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



