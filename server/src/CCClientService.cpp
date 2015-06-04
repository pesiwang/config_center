/*
 * CCClientService.cpp
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#include "CCClientProtocol.h"
#include "CCClientService.h"
#include "CCConfig.h"
#include "CCOnline.h"
#include "core/VasEventbase.h"
#include "core/VasHelper.h"
#include <arpa/inet.h>
#include <openssl/md5.h>

using namespace std;

CCClientService::CCClientService() {

}

CCClientService::~CCClientService() {
}

void CCClientService::onDataArrived(int fd, VasBuffer *buffer) {
	CC_CLIENT_PACKAGE_HEAD pkgHead;
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

void CCClientService::onClosed(int fd) {
	VAS_LOGGER_DEBUG("client %d closed", fd);
	CCOnline::sharedInstance()->remove(fd);
}

void CCClientService::_processCmd(int fd, unsigned char command, string& body) {
	if (command == CC_CLIENT_CMD_SYNC) {
		if (body.size() == MD5_DIGEST_LENGTH) {
			if ((CCConfig::sharedInstance()->getHash().size() == MD5_DIGEST_LENGTH) 
					&& (memcmp(CCConfig::sharedInstance()->getHash().data(), body.data(), MD5_DIGEST_LENGTH) != 0)) {
				CC_CLIENT_PACKAGE_HEAD pkgHead;
				pkgHead.version = CC_CLIENT_VERSION;
				pkgHead.command = CC_CLIENT_CMD_SYNC;
				pkgHead.length = MD5_DIGEST_LENGTH + CCConfig::sharedInstance()->getContent().size();
				pkgHead.length = htonl(pkgHead.length);

				VasBuffer buffer;
				buffer.write((const char*)&pkgHead, sizeof(pkgHead));
				buffer.write((const char*)CCConfig::sharedInstance()->getHash().data(), MD5_DIGEST_LENGTH);
				buffer.write((const char*)CCConfig::sharedInstance()->getContent().data(), CCConfig::sharedInstance()->getContent().size());
				VasEventbase::instance()->writeSocket(fd, &buffer);
			}
		}
	}
}
