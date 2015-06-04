/*
 * CCManagerListener.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef _INCLUDE_CC_MANAGER_LISTENER_H_
#define _INCLUDE_CC_MANAGER_LISTENER_H_

#include "handler/VasListenerHandler.h"
#include "CCManagerService.h"

class CCManagerListener : public VasListenerHandler {
public:
	CCManagerListener();
	virtual ~CCManagerListener();

	void onAccepted(int fd);
	void onClosed(int fd);

private:
	CCManagerService _service;
};

#endif /* _INCLUDE_CC_MANAGER_LISTENER_H_ */
