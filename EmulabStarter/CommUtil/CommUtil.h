/*
 * CommUtil.h
 *
 *  Created on: Jun 26, 2011
 *      Author: jie
 */

#ifndef COMMUTIL_H_
#define COMMUTIL_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <time.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

enum MsgType {
	NODE_NAME = 1,
    IP_ADDRESS = 2,
    INFORMATIONAL = 3,
    WARNING = 4,
    COMMAND = 5,
    COMMAND_RESPONSE = 6
};


#define BUFFER_SIZE 	10000


#endif /* COMMUTIL_H_ */