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

	nil = (BufferEntry*) malloc(sizeof(BufferEntry));
	memset(nil, 0, sizeof(BufferEntry));
	nil->prev = nil;	// list back
	nil->next = nil;	// list front
}


MVCTPBuffer::~MVCTPBuffer() {
	DeleteUntil(End());
}


BufferEntry* MVCTPBuffer::Begin() {
	return nil->next;
}


BufferEntry* MVCTPBuffer::End() {
	return nil;
}

BufferEntry* MVCTPBuffer::Front() {
	return nil->next;
}

BufferEntry* MVCTPBuffer::Back() {
	return nil->prev;
}

bool MVCTPBuffer::IsEmpty() {
	return (nil->next == nil);
}


int MVCTPBuffer::PushBack(BufferEntry* entry) {
	return Insert(Back(), entry);
}


int MVCTPBuffer::Insert(BufferEntry* pos, BufferEntry* entry) {
	current_buffer_size += entry->data_len;
	num_entry++;

	entry->prev = pos;
	entry->next = pos->next;
	pos->next = pos->next->prev = entry;

	return 1;
}


int MVCTPBuffer::Delete(BufferEntry* entry) {
	current_buffer_size -= entry->data_len;
	num_entry--;

	entry->prev->next = entry->next;
	entry->next->prev = entry->prev;
	DestroyEntry(entry);

	return 1;
}

// Safe-implementation of iterate-and-delete for the entry list
int MVCTPBuffer::DeleteUntil(BufferEntry* entry) {
	if (entry == NULL)
		return 0;

	BufferEntry *it = NULL;
	for (it = Begin(); it != End();) {
		if (it == entry)
			break;

		BufferEntry* tmp = it;
		it = it->next;
		Delete(tmp);
	}
	return 1;
}

void MVCTPBuffer::Clear() {
	DeleteUntil(End());
}

BufferEntry* MVCTPBuffer::Find(int32_t pid) {
	BufferEntry *tmp = NULL;
	for (tmp = Begin(); tmp != End(); tmp = tmp->next) {
		if (tmp->packet_id == pid) {
			break;
		}
	}
	return tmp;
}


int MVCTPBuffer::ShrinkEntry(BufferEntry* entry, size_t new_size) {
	if (entry->data_len < new_size)
		return -1;

	current_buffer_size -= entry->data_len - new_size;

	char* new_data = (char*)malloc(new_size);
	memcpy(new_data, entry->data + (entry->data_len - new_size), new_size);
	free(entry->data);
	entry->data = new_data;
	entry->data_len = new_size;

	return 1;
}

void MVCTPBuffer::DestroyEntry(BufferEntry* entry) {
	free(entry->data);
	free(entry);
}


int MVCTPBuffer::AddEntry(MVCTP_HEADER* header, char* data) {
	BufferEntry* entry = (BufferEntry*)malloc(sizeof(BufferEntry));
	entry->packet_id = header->packet_id;
	entry->data_len = header->data_len;
	entry->data = data;

	int32_t packet_id = header->packet_id;
	BufferEntry* it = Back();
	for (; it != Front()->prev; it = it->prev) {
		if (it->packet_id == packet_id)
			return 0;
		else if (it->packet_id < packet_id) {
			break; //Insert(it->next, entry);
		}
	}

	Insert(it, entry);
	//PushBack(entry);
	//cout << "Entry added. New front packet ID: " << Front()->packet_id << endl;
	return 1;
}



