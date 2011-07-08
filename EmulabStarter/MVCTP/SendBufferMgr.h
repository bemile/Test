/*
 * SendBufferMgr.h
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#ifndef SENDBUFFERMGR_H_
#define SENDBUFFERMGR_H_

#include "mvctp.h"
#include "MVCTPBuffer.h"
#include "UdpComm.h"
#include <pthread.h>

class SendBufferMgr {
public:
	SendBufferMgr(int size, MulticastComm* mcomm);
	~SendBufferMgr();

	int SendData(char* data, size_t length);
	int SendPacket(BufferEntry* entry);
	void StartUdpThread();

private:
	MVCTPBuffer* send_buf;
	MulticastComm* comm;
	UdpComm*		udp_comm;
	sockaddr_in		sender_addr;
	socklen_t 		sender_socklen;

	int 	num_entry;		// number of buffer entries in the buffer
	int 	max_size;		// Maximum data bytes for the buffer
	int 	actual_size;	// actual assigned data bytes in the buffer
	int		last_packet_id;	// ID number for the latest sent/received packet


	pthread_t udp_thread;
	pthread_mutex_t buf_mutex;
	static void* StartUdpNackReceive(void* ptr);
	void ReceiveNack();
	void Retransmit(u_int32_t packet_id);
};

#endif /* SENDBUFFERMGR_H_ */
