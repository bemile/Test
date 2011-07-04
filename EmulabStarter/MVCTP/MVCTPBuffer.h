/*
 * MVCTPBuffer.h
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#ifndef MVCTPBUFFER_H_
#define MVCTPBUFFER_H_

#include "mvctp.h"
#include "MulticastComm.h"
#include <pthread.h>

// buffer entry for a single packet
typedef struct BufferEntry {
	u_int32_t 	packet_id;
	size_t		data_len;
	char*		data;
	BufferEntry	*prev, *next;
} BUFFER_ENTRY, * PTR_BUFFER_ENTRY;


//	information for an MVCTP send/receive buffer
class MVCTPBuffer {
public:
	MVCTPBuffer(int size, MulticastComm* mcomm);
	~MVCTPBuffer();

	size_t 	GetDataSize();

	BufferEntry* Begin();
	BufferEntry* End();
	int PushBack(BufferEntry* entry);
	int Insert(BufferEntry* pos, BufferEntry* entry);
	int Delete(BufferEntry*	entry);
	int DeleteUntil(BufferEntry* entry);
	BufferEntry* Find(u_int32_t pid);
	bool 	IsEmpty();



protected:
	int 	num_entry;		// number of buffer entries in the buffer
	int 	max_size;		// Maximum data bytes for the buffer
	int 	actual_size;	// actual assigned data bytes in the buffer
	int		last_packet_id;	// ID number for the latest sent/received packet
	PTR_BUFFER_ENTRY	nil;		// pointer to the first and last entry

	MulticastComm* comm;

	int		AddData(MVCTP_HEADER* header, char* data);
	void DestroyEntry(BufferEntry* entry);
	int ShrinkEntry(BufferEntry* entry, size_t new_size);
};


#endif /* MVCTPBUFFER_H_ */
