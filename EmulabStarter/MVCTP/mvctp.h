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
#include <sys/utsname.h>
#include <sys/errno.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
//#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>


using namespace std;

typedef struct sockaddr SA;

const string multicast_addr = "224.1.2.3";
const unsigned char group_mac_addr[6] = {0x01, 0x00, 0x5e, 0x01, 0x02, 0x03};
const int PORT_NUM = 11001;
const int BUFF_SIZE = 10000;

#endif /* MVCTP_H_ */
