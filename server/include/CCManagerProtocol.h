/*
 * CCManagerProtocol.h
 *
 *  Created on: Jan 23, 2015
 *      Author: ray
 */

#ifndef CC_INCLUDE_MANAGER_PROTOCOL_H_
#define CC_INCLUDE_MANAGER_PROTOCOL_H_

#pragma pack(1)
typedef struct _CC_MANAGER_PACKAGE_HEAD {
	unsigned char command;
	unsigned int length;
} CC_MANAGER_PACKAGE_HEAD;
#pragma pack()

#define CC_MANAGER_CMD_UPDATE	0x01

#endif /* CC_INCLUDE_MANAGER_PROTOCOL_H_*/
