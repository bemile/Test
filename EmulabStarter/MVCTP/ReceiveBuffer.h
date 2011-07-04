/*
 * ReceiveBuffer.h
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#ifndef RECEIVEBUFFER_H_
#define RECEIVEBUFFER_H_

#include "mvctp.h"
#include "MVCTPBuffer.h"
#include <pthread.h>

class ReceiveBuffer: public MVCTPBuffer {
public:
	ReceiveBuffer(int size, MulticastComm* mcomm);
	~ReceiveBuffer();

	void StartReceiveThread();
	size_t 	GetData(void* buff, size_t len);

private:
	pthread_t thread;
	pthread_mutex_t buf_mutex, empty_mutex;
	static void* StartReceivingData(void* ptr);
	void Run();
};

#endif /* RECEIVEBUFFER_H_ */
