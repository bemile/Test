/*
 * SenderCommandClient.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef SENDERCOMMANDCLIENT_H_
#define SENDERCOMMANDCLIENT_H_

#include "../CommUtil/CommandExecClient.h"
#include "MVCTPSender.h"
#include <sys/time.h>


enum MsgTag {
	MSG_START = 1010101010,
	MSG_END = 1111111111
};

enum TransferMsgType {
	STRING_TRANSFER,
	MEMORY_TRANSFER,
	FILE_TRANSFER
};

struct TransferMessage {
	 TransferMsgType	msg_type;
	 int 		data_len;
	 char       file_name[30];
};


class SenderCommandClient : public CommandExecClient {
public:
	SenderCommandClient(MVCTPSender* psender);
	SenderCommandClient(int sock, MVCTPSender* psender);

protected:
	virtual int HandleCommand(char* command);
	int HandleSendCommand(list<string>& slist);

	int TransferString(string str, bool send_out_packets);
	int TransferMemoryData(int size);

private:
	MVCTPSender* ptr_sender;
};


#endif /* SENDERCOMMANDCLIENT_H_ */
