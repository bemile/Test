/*
 * CommUtil.h
 *
 *  Created on: Jun 26, 2011
 *      Author: jie
 */

#ifndef COMMUTIL_H_
#define COMMUTIL_H_

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
