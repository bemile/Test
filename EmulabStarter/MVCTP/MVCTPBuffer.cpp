/*
 * MVCTPBuffer.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "MVCTPBuffer.h"

MVCTPBuffer::MVCTPBuffer(int buf_size) {
	max_buffer_size = buf_size;
	current_buffer_size = 0;
	num_entry = 0;
	min_packet_id = 2100000000;
	max_packet_id = -1;

	AllocateFreePackets();
}


MVCTPBuffer::~MVCTPBuffer() {
	Clear();

	list<BufferEntry*>::iterator it;
	for (it = free_packet_list.begin(); it != free_packet_list.end(); it++) {
		DestroyEntry(*it);
	}
}


void MVCTPBuffer::AllocateFreePackets() {
	int numPackets = max_buffer_size / MVCTP_PACKET_LEN;
	if (numPackets > 10000) {
		numPackets = 10000;
	}

	char* ptr = (char*)malloc(numPackets * MVCTP_PACKET_LEN);
	if (ptr == NULL) {
		SysError("MVCTPBuffer::InitializeFreePackets()::malloc()");
	}

	for (int i = 0; i < numPackets; i++) {
		BufferEntry* entry = (BufferEntry*)malloc(sizeof(BufferEntry));
		entry->packet_buffer = ptr;
		entry->data = ptr;
		free_packet_list.push_back(entry);
		ptr += MVCTP_PACKET_LEN;
	}
}

void MVCTPBuffer::AddFreePacket(BufferEntry* entry) {
	if (entry != NULL)
		free_packet_list.push_back(entry);
}

BufferEntry* MVCTPBuffer::GetFreePacket() {
	if (free_packet_list.empty()) {
		AllocateFreePackets();
	}
	BufferEntry* ptr = free_packet_list.front();
	free_packet_list.pop_front();
	return ptr;
}


bool MVCTPBuffer::IsEmpty() {
	return (buffer_pool.size() == 0);
}


int MVCTPBuffer::Insert(BufferEntry* entry) {
	if (entry == NULL)
		return 0;

	current_buffer_size += entry->data_len;
	num_entry++;
	if (min_packet_id > entry->packet_id)
		min_packet_id = entry->packet_id;

	if (max_packet_id < entry->packet_id)
			max_packet_id = entry->packet_id;

	buffer_pool.insert(pair<int32_t, BufferEntry*>(entry->packet_id, entry));
	return 1;
}


int MVCTPBuffer::Delete(BufferEntry* entry) {
	if (entry == NULL)
		return 0;

	current_buffer_size -= entry->data_len;
	num_entry--;
	if (min_packet_id == entry->packet_id)
		min_packet_id++;
	else if (max_packet_id == entry->packet_id)
		max_packet_id--;

	buffer_pool.erase(entry->packet_id);
	AddFreePacket(entry);
	//DestroyEntry(entry);

	return 1;
}

// Safe-implementation of iterate-and-delete for the entry list
int MVCTPBuffer::DeleteUntil(int32_t start_id, int32_t end_id) {
	for (int32_t i = start_id; i < end_id; i++) {
		if (buffer_pool.find(i) != buffer_pool.end()) {
			Delete(buffer_pool.at(i));
			//buffer_pool.erase(i);
		}
	}
	return 1;
}

void MVCTPBuffer::Clear() {
	map<int32_t, BufferEntry*>::iterator it;
	for (it = buffer_pool.begin(); it != buffer_pool.end(); it++) {
		AddFreePacket(it->second);
		//DestroyEntry(it->second);
	}
	buffer_pool.clear();

	current_buffer_size = 0;
	num_entry = 0;
	min_packet_id = 2100000000;
	max_packet_id = -1;
}

BufferEntry* MVCTPBuffer::Find(int32_t pid) {
	BufferEntry *tmp = NULL;
	map<int32_t, BufferEntry*>::iterator it = buffer_pool.find(pid);
	if (it != buffer_pool.end()) {
		tmp = it->second;
	}
	return tmp;
}


int MVCTPBuffer::ShrinkEntry(BufferEntry* entry, size_t new_size) {
	if (entry->data_len < new_size)
		return -1;

	entry->data = entry->data + entry->data_len - new_size;
	entry->data_len = new_size;

//	current_buffer_size -= entry->data_len - new_size;
//	char* new_data = (char*) malloc(new_size);
//	memcpy(new_data, entry->data + (entry->data_len - new_size), new_size);
//	free(entry->data);
//	entry->data = new_data;
//	entry->data_len = new_size;

	return 1;
}

void MVCTPBuffer::DestroyEntry(BufferEntry* entry) {
	free(entry->packet_buffer);
	free(entry);
}


int MVCTPBuffer::AddEntry(MVCTP_HEADER* header, char* data) {
	BufferEntry* entry = GetFreePacket(); //(BufferEntry*)malloc(sizeof(BufferEntry));
	entry->packet_id = header->packet_id;
	entry->data_len = header->data_len;
	memcpy(entry->packet_buffer, data, header->data_len);
	//entry->data = data;

	buffer_pool.insert(pair<int32_t, BufferEntry*>(entry->packet_id, entry));

	//PushBack(entry);
	//cout << "Entry added. New front packet ID: " << Front()->packet_id << endl;
	return 1;
}



