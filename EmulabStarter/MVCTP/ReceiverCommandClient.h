/*
 * ReceiverCommandClient.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef RECEIVERCOMMANDCLIENT_H_
#define RECEIVERCOMMANDCLIENT_H_

#include "../CommUtil/CommandExecClient.h"
#include "MVCTPReceiver.h"
#include <sys/time.h>

class ReceiverCommandClient : public CommandExecClient {
public:
	ReceiverCommandClient(MVCTPReceiver* preceiver);
	ReceiverCommandClient(int sock, MVCTPReceiver* psender);

protected:
	virtual int HandleCommand(char* command);

private:
	MVCTPReceiver* ptr_receiver;
};

#endif /* RECEIVERCOMMANDCLIENT_H_ */
