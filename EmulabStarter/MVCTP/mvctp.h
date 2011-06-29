/*
 * mvctp.h
 *
 *  Created on: Jun 28, 2011
 *      Author: jie
 */

#ifndef MVCTP_H_
#define MVCTP_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/utsname.h>
#include <net/if.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>
#include <sys/errno.h>
#include <unistd.h>

using namespace std;

typedef struct sockaddr SA;

const string multicast_addr = "224.1.2.3";
const int PORT_NUM = 11001;
const int BUFF_SIZE = 10000;

#endif /* MVCTP_H_ */
