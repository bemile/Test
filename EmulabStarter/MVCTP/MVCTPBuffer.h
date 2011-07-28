/*
 * MVCTPBuffer.h
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#ifndef MVCTPBUFFER_H_
#define MVCTPBUFFER_H_

#include "mvctp.h"
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
	MVCTPBuffer(int buf_size);
	~MVCTPBuffer();

	size_t 	GetMaxBufferSize() {return max_buffer_size;};
	void 	SetMaxBufferSize(size_t buff_size) {max_buffer_size = buff_size;}
	size_t 	GetCurrentBufferSize() {return current_buffer_size;}
	size_t 	GetAvailableBufferSize() {return max_buffer_size - current_buffer_size;}
	int 	GetNumEntries() {return num_entry;}


	BufferEntry* 	Begin();
	BufferEntry* 	End();
	BufferEntry* 	Front();
	BufferEntry* 	Back();

	BufferEntry* 	Find(int32_t pid);
	int 			PushBack(BufferEntry* entry);
	int 			Insert(BufferEntry* pos, BufferEntry* entry);
	int 			Delete(BufferEntry*	entry);
	int 			DeleteUntil(BufferEntry* entry);
	bool 			IsEmpty();
	int 			ShrinkEntry(BufferEntry* entry, size_t new_size);
	int				AddEntry(MVCTP_HEADER* header, char* data);
	void			Clear();


protected:
	int 		num_entry;				// number of packet entries in the buffer
	size_t 		max_buffer_size;		// maximum data bytes assigned to the buffer
	size_t 		current_buffer_size;	// current occupied data bytes in the buffer

	PTR_BUFFER_ENTRY	nil;		// pointer to the first and last entry

	void DestroyEntry(BufferEntry* entry);
};


#endif /* MVCTPBUFFER_H_ */
