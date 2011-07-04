/*
 * SendBuffer.cpp
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#include "SendBuffer.h"

SendBuffer::~SendBuffer() {

}


int SendBuffer::SendData(char* data, size_t length) {
	int bytes_left = length;
	int bytes_sent = 0;
	char* pos = data;

	while (bytes_left > 0) {
		int len;
		if (bytes_left > UDP_MVCTP_DATA_LEN)
			len = UDP_MVCTP_DATA_LEN;
		else
			len = bytes_left;

		BufferEntry* entry = (BufferEntry*) malloc(sizeof(BufferEntry));
		entry->packet_id = ++last_packet_id;
		entry->data_len = len + MVCTP_HLEN;

		char* ptr_data = (char*) malloc(len + MVCTP_HLEN);
		MVCTP_HEADER* header = (MVCTP_HEADER*)ptr_data;
		header->packet_id = entry->packet_id;
		header->data_len = len;
		memcpy(ptr_data + MVCTP_HLEN, pos, len);
		entry->data = ptr_data;

		PushBack(entry);
		actual_size += entry->data_len;
		num_entry++;

		pos += len;
		bytes_left -= len;
		bytes_sent += len;

		if (SendPacket(entry) <= 0) {
			SysError("SendBuffer error on sending packet");
		}
	}

	return bytes_sent;
}


int SendBuffer::SendPacket(BufferEntry* entry) {
	return comm->SendData(entry->data, entry->data_len, 0);
}
