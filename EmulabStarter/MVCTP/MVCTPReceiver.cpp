/*
 * MVCTPReceiver.cpp
 *
 *  Created on: Jul 21, 2011
 *      Author: jie
 */

#include "MVCTPReceiver.h"

MVCTPReceiver::MVCTPReceiver(int buf_size) {
	ptr_recv_buf_mgr = new ReceiveBufferMgr(buf_size, ptr_raw_sock_comm);
}

MVCTPReceiver::~MVCTPReceiver() {
	delete ptr_recv_buf_mgr;
}

const struct ReceiveBufferStats MVCTPReceiver::GetBufferStats() {
	return ptr_recv_buf_mgr->GetBufferStats();
}

void MVCTPReceiver::SetBufferSize(size_t buff_size) {
	ptr_recv_buf_mgr->SetBufferSize(buff_size);
}

size_t MVCTPReceiver::GetBufferSize() {
	return ptr_recv_buf_mgr->GetBufferSize();
}

int MVCTPReceiver::JoinGroup(string addr, ushort port) {
	MVCTPComm::JoinGroup(group_id, port);
	ptr_recv_buf_mgr->StartReceiveThread();
	return 1;
}


int MVCTPReceiver::RawSend(char* data, size_t length, bool send_out) {
	//return ptr_recv_buf_mgr->SendData(data, length, mac_group_addr, send_out);
	return -1;
}

ssize_t MVCTPReceiver::RawReceive(void* buff, size_t len, int flags, SA* from, socklen_t* from_len) {
	return ptr_recv_buf_mgr->GetData(buff, len);
}

ssize_t MVCTPReceiver::IPReceive(void* buff, size_t len, int flags, SA* from, socklen_t* from_len) {
	size_t bytes = ptr_recv_buf_mgr->GetData(buff, len);
	return bytes;
}
