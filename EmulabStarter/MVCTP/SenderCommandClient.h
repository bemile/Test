/*
 * SenderCommandClient.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef SENDERCOMMANDCLIENT_H_
#define SENDERCOMMANDCLIENT_H_

#include "../CommUtil/CommandExecClient.h"
#include "MVCTPComm.h"

class SenderCommandClient : public CommandExecClient {
public:
	SenderCommandClient(MVCTPComm* psender);
	SenderCommandClient(int sock, MVCTPComm* psender);

	void Split(string s, char c, list<string>& slist);

protected:
	virtual int HandleCommand(char* command);
	int HandleSendCommand(list<string>& slist);

private:
	MVCTPComm* ptr_sender;
};


#endif /* SENDERCOMMANDCLIENT_H_ */
