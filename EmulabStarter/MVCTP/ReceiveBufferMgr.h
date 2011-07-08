/*
 * ReceiveBufferMgr.h
 *
 *  Created on: Jul 3, 2011
 *      Author: jie
 */

#ifndef RECEIVEBUFFERMGR_H_
#define RECEIVEBUFFERMGR_H_

#include "mvctp.h"
#include "MVCTPBuffer.h"
#include "UdpComm.h"
#include <pthread.h>


class ReceiveBufferMgr {
public:
	ReceiveBufferMgr(int size, MulticastComm* mcomm);
	~ReceiveBufferMgr();

	size_t 	GetData(void* buff, size_t len);
	void StartReceiveThread();

private:
	MVCTPBuffer* 	recv_buf;
	MulticastComm* 	comm;
	UdpComm*		udp_comm;
	sockaddr_in		sender_addr;
	socklen_t 		sender_socklen;

	int 	num_entry;		// number of buffer entries in the buffer
	int 	max_size;		// Maximum data bytes for the buffer
	int 	actual_size;	// actual assigned data bytes in the buffer
	u_int32_t			last_recv_packet_id;	// ID number for the latest sent/received packet
	u_int32_t			last_del_packet_id;
	list<NackMsgInfo> 	missing_packet_list;

	int SendNackMsg(u_int32_t packet_id);

	pthread_t recv_thread, nack_thread, udp_thread;
	pthread_mutex_t buf_mutex, nack_list_mutex;
	static void* StartReceivingData(void* ptr);
	void Run();
	static void* StartNackThread(void* ptr);
	void NackRun();
	static void* StartUdpReceiveThread(void* ptr);
	void UdpReceive();
};

#endif /* RECEIVEBUFFERMGR_H_ */
