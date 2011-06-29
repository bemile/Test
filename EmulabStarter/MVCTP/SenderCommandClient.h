/*
 * SenderCommandClient.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef SENDERCOMMANDCLIENT_H_
#define SENDERCOMMANDCLIENT_H_

#include "../CommUtil/CommandExecClient.h"
#include "MVCSender.h"

class SenderCommandClient : public CommandExecClient {
public:
	SenderCommandClient(MVCSender* psender) {ptr_sender = psender;};
	SenderCommandClient(int sock, MVCSender* psender) {sockfd = sock; ptr_sender = psender;};

protected:
	virtual int HandleCommand(char* command);

private:
	MVCSender* ptr_sender;
};


#endif /* SENDERCOMMANDCLIENT_H_ */
