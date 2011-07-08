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
	int32_t 	packet_id;
	size_t		data_len;
	char*		data;
	BufferEntry	*prev, *next;
} BUFFER_ENTRY, * PTR_BUFFER_ENTRY;


//	information for an MVCTP send/receive buffer
class MVCTPBuffer {
public:
	MVCTPBuffer(int size);
	~MVCTPBuffer();

	BufferEntry* Begin();
	BufferEntry* End();
	int PushBack(BufferEntry* entry);
	int Insert(BufferEntry* pos, BufferEntry* entry);
	int Delete(BufferEntry*	entry);
	int DeleteUntil(BufferEntry* entry);
	BufferEntry* Find(int32_t pid);
	bool 	IsEmpty();
	int ShrinkEntry(BufferEntry* entry, size_t new_size);
	int		AddEntry(MVCTP_HEADER* header, char* data);


protected:
	PTR_BUFFER_ENTRY	nil;		// pointer to the first and last entry

	void DestroyEntry(BufferEntry* entry);
};


#endif /* MVCTPBUFFER_H_ */
