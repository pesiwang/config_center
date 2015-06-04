/*
 * CCClientListener.cpp
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#include "CCClientListener.h"
#include "CCOnline.h"
#include "core/VasHelper.h"
#include "core/VasEventbase.h"

CCClientListener::CCClientListener() {
}

CCClientListener::~CCClientListener() {
}

void CCClientListener::onAccepted(int fd) {
	VAS_LOGGER_DEBUG("client %d accepted", fd);
	VasEventbase::instance()->addServer(fd, &(this->_service), 60);
	CCOnline::sharedInstance()->add(fd);
}

void CCClientListener::onClosed(int fd) {
	VAS_LOGGER_DEBUG("listener %d closed", fd);
}
