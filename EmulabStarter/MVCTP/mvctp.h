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
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string.h>


using namespace std;

typedef struct sockaddr SA;
typedef struct ifreq	IFREQ;


typedef struct MVCTPHeader {
	u_int32_t		group_id;
	u_int16_t		src_port;
	u_int16_t		dest_port;
	u_int32_t		packet_id;
	u_int32_t		data_len;
} MVCTP_HEADER, *PTR_MVCTP_HEADER;


/*#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version:4;
    unsigned int ihl:4;
#else
# error "Please fix <bits/endian.h>"
#endif
*/



// Macros
#define MAX(a, b)  ((a) > (b) ? (a) : (b))


// Constant values
const string group_id = "224.1.2.3";
const unsigned char group_mac_addr[6] = {0x01, 0x00, 0x5e, 0x01, 0x02, 0x03};
const u_short mvctp_port = 123;
const int PORT_NUM = 11001;
const int BUFF_SIZE = 10000;
const int MVCTP_PACKET_LEN = ETH_FRAME_LEN - ETH_HLEN;
const int MVCTP_HLEN = sizeof(MVCTP_HEADER);
const int MVCTP_DATA_LEN = ETH_FRAME_LEN - ETH_HLEN - sizeof(MVCTP_HEADER);

// parameters for MVCTP over UDP
const int UDP_MVCTP_PACKET_LEN = 1200;
const int UDP_MVCTP_HLEN = sizeof(MVCTP_HEADER);
const int UDP_MVCTP_DATA_LEN = 1200 - sizeof(MVCTP_HEADER);



// Prototypes for global functions
void SysError(string s);


#endif /* MVCTP_H_ */
