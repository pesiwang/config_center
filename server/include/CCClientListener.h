/*
 * CCClientListener.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef _INCLUDE_CC_CLIENT_LISTENER_H_
#define _INCLUDE_CC_CLIENT_LISTENER_H_

#include "handler/VasListenerHandler.h"
#include "CCClientService.h"

class CCClientListener : public VasListenerHandler {
public:
	CCClientListener();
	virtual ~CCClientListener();

	void onAccepted(int fd);
	void onClosed(int fd);

private:
	CCClientService _service;
};

#endif /* _INCLUDE_CC_CLIENT_LISTENER_H_*/
