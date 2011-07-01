/*
 * mvctp.cpp
 *
 *  Created on: Jun 30, 2011
 *      Author: jie
 */
#include "mvctp.h"

void SysError(string s) {
	perror(s.c_str());
	exit(-1);
}
