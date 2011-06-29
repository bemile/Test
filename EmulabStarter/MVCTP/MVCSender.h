/*
 * MVCSender.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef MVCSENDER_H_
#define MVCSENDER_H_

#include "mvctp.h"
#include "MulticastComm.h"

class MVCSender {
public:
	MVCSender();
	~MVCSender();
	int JoinGroup(string addr);
	int Send(char* data, size_t length);


private:
	MulticastComm comm;
};

#endif /* MVCSENDER_H_ */
