/*
 * CCManagerListener.cpp
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#include "CCManagerListener.h"
#include "core/VasHelper.h"
#include "core/VasEventbase.h"

CCManagerListener::CCManagerListener() {
}

CCManagerListener::~CCManagerListener() {
}

void CCManagerListener::onAccepted(int fd) {
	VAS_LOGGER_DEBUG("manager %d accepted", fd);
	VasEventbase::instance()->addServer(fd, &(this->_service), 10);
}

void CCManagerListener::onClosed(int fd) {
	VAS_LOGGER_DEBUG("listener %d closed", fd);
}
