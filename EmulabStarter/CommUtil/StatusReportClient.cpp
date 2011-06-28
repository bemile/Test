/*
 * StatusReportClient.cpp
 *
 *  Created on: Jun 26, 2011
 *      Author: jie
 */

#include "StatusReportClient.h"

StatusReportClient::StatusReportClient(int sock) {
	sockfd = sock;
}


int StatusReportClient::SendMessage(int msg_type, string msg) {
	int res;
	if ( (res = write(sockfd, &msg_type, sizeof(msg_type))) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}

	int length = msg.length();
	if ( (res = write(sockfd, &length, sizeof(length))) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}

	if ( (res = write(sockfd, msg.c_str(), length)) < 0) {
		cout << "Error sending message. " << endl;
		return -1;
	}
	else
		return 1;
}

