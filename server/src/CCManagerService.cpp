/*
 * CCManagerService.cpp
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#include "CCManagerProtocol.h"
#include "CCClientProtocol.h"
#include "CCManagerService.h"
#include "CCConfig.h"
#include "CCOnline.h"
#include "core/VasEventbase.h"
#include "core/VasHelper.h"
#include <arpa/inet.h>
#include <openssl/md5.h>

using namespace std;

CCManagerService::CCManagerService() {

}

CCManagerService::~CCManagerService() {
}

void CCManagerService::onDataArrived(int fd, VasBuffer *buffer) {
	CC_MANAGER_PACKAGE_HEAD pkgHead;
	if (buffer->size() < sizeof(pkgHead)) {
		return;
	}

	buffer->peek((char *)&pkgHead, sizeof(pkgHead));
	pkgHead.length = ntohl(pkgHead.length);

	if (buffer->size() < sizeof(pkgHead) + pkgHead.length) {
		return;
	}
	buffer->shrink(sizeof(pkgHead));

	string body;
	if (pkgHead.length > 0) {
		body.resize(pkgHead.length);
		buffer->read((char *)body.data(), pkgHead.length);
	}

	this->_processCmd(fd, pkgHead.command, body);
}

void CCManagerService::onClosed(int fd) {
	VAS_LOGGER_DEBUG("manager %d closed", fd);
}

void CCManagerService::_processCmd(int fd, unsigned char command, string& body) {
	if (command == CC_MANAGER_CMD_UPDATE) {
		if (body.size() > MD5_DIGEST_LENGTH) {
			string hash = body.substr(0, MD5_DIGEST_LENGTH);
			if(CCConfig::sharedInstance()->getHash() != hash) {
				string content = body.substr(MD5_DIGEST_LENGTH);
				CCConfig::sharedInstance()->update(hash, content);

				CC_CLIENT_PACKAGE_HEAD pkgHead;
				pkgHead.version = CC_CLIENT_VERSION;
				pkgHead.command = CC_CLIENT_CMD_SYNC;
				pkgHead.length = MD5_DIGEST_LENGTH + CCConfig::sharedInstance()->getContent().size();
				pkgHead.length = htonl(pkgHead.length);
	
				for(set<int>::const_iterator iter = CCOnline::sharedInstance()->getClients().begin(); iter != CCOnline::sharedInstance()->getClients().end(); ++iter) {
					VAS_LOGGER_DEBUG("pushing to client %d", *iter);
					VasBuffer buffer;
					buffer.write((const char*)&pkgHead, sizeof(pkgHead));
					buffer.write((const char*)hash.data(), MD5_DIGEST_LENGTH);
					buffer.write((const char*)content.data(), content.size());
					VasEventbase::instance()->writeSocket(*iter, &buffer);
				}
			}
		}
	}
}
