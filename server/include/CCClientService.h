/*
 * CCClientService.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef CC_INCLUDE_CLIENT_SERVICE_H_
#define CC_INCLUDE_CLIENT_SERVICE_H_

#include "handler/VasServerHandler.h"
#include "core/VasBuffer.h"
#include <string>

class CCClientService : public VasServerHandler {
public:
	CCClientService();
	virtual ~CCClientService();

	void onDataArrived(int fd, VasBuffer *buffer);
	void onClosed(int fd);

	void _processCmd(int fd, unsigned char command, std::string& body);
};


#endif /* CC_INCLUDE_CLIENT_SERVICE_H_*/
