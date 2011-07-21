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

class SenderCommandClient : public CommandExecClient {
public:
	SenderCommandClient(MVCTPSender* psender);
	SenderCommandClient(int sock, MVCTPSender* psender);

protected:
	virtual int HandleCommand(char* command);
	int HandleSendCommand(list<string>& slist);
	int MemoryTransfer(int size);

private:
	MVCTPSender* ptr_sender;
};


#endif /* SENDERCOMMANDCLIENT_H_ */
