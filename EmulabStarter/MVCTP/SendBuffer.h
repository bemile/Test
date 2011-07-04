/*
 * SendBuffer.h
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#ifndef SENDBUFFER_H_
#define SENDBUFFER_H_

#include "mvctp.h"
#include "MVCTPBuffer.h"

class SendBuffer : public MVCTPBuffer{
public:
	SendBuffer(int size, MulticastComm* mcomm) : MVCTPBuffer(size, mcomm) {}
	~SendBuffer();

	int SendData(char* data, size_t length);
	int SendPacket(BufferEntry* entry);

private:

};

#endif /* SENDBUFFER_H_ */
