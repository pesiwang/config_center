/*
 * CCClientProtocol.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef CC_INCLUDE_CLIENT_PROTOCOL_H_
#define CC_INCLUDE_CLIENT_PROTOCOL_H_

#pragma pack(1)
typedef struct _CC_CLIENT_PACKAGE_HEAD {
	unsigned char version;
	unsigned char command;
	unsigned int length;
} CC_CLIENT_PACKAGE_HEAD;
#pragma pack()

#define CC_CLIENT_VERSION	0x01
#define CC_CLIENT_CMD_SYNC	0x01

#endif /* CC_INCLUDE_CLIENT_PROTOCOL_H_*/
