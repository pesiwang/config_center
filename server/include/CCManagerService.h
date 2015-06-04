/*
 * CCManagerService.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef CC_INCLUDE_MANAGER_SERVICE_H_
#define CC_INCLUDE_MANAGER_SERVICE_H_

#include "handler/VasServerHandler.h"
#include "core/VasBuffer.h"
#include <string>

class CCManagerService : public VasServerHandler {
public:
	CCManagerService();
	virtual ~CCManagerService();

	void onDataArrived(int fd, VasBuffer *buffer);
	void onClosed(int fd);

	void _processCmd(int fd, unsigned char command, std::string& body);
};


#endif /* CC_INCLUDE_MANAGER_SERVICE_H_*/
