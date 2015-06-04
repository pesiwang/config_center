/*
 * main.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: ray
 */

#include "core/VasEventbase.h"
#include "core/VasHelper.h"
#include "CCClientListener.h"
#include "CCManagerListener.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		VAS_LOGGER_DEBUG("usage: %s <config.json>", argv[0]);
		return -1;
	}

	VasHelper::Config::load(argv[1]);

	int clientListenerFd = VasHelper::Socket::listen(VasHelper::Config::valueAsString("client_host").c_str(), VasHelper::Config::valueAsInteger("client_port"));
	int managerListenerFd = VasHelper::Socket::listen(VasHelper::Config::valueAsString("manager_host").c_str(), VasHelper::Config::valueAsInteger("manager_port"));
	CCClientListener clientListener;
	CCManagerListener managerListener;

	VasEventbase::instance()->addListener(managerListenerFd, &managerListener);
	VasEventbase::instance()->addListener(clientListenerFd, &clientListener);
	VasEventbase::instance()->dispatch();
	return 0;
}
