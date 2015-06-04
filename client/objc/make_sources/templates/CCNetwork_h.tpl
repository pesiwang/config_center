//
//  CCNetwork.h
//  testdrive
//
//  Created by Chen Rui on 4/27/15.
//  Copyright (c) 2015 Vanchu. All rights reserved.
//

#import <Foundation/Foundation.h>

#pragma pack(1)
typedef struct _CC_PACKAGE_HEAD {
	unsigned char version;
	unsigned char command;
	unsigned int length;
} CC_PACKAGE_HEAD;
#pragma pack()

#define CC_VERSION 0x01
#define CC_CMD_SYNC 0x01

@class CCNetwork;

@protocol CCNetworkDelegate <NSObject>
- (void)ccNetworkDidConnectToServer:(CCNetwork *)network;
- (void)ccNetworkDidDisconnectFromServer:(CCNetwork *)network;
- (void)ccNetwork:(CCNetwork *)network didUpdateConfig:(NSData *)config withHash:(NSData *)hash;
@end

@interface CCNetwork : NSObject

@property (weak, nonatomic) id<CCNetworkDelegate> delegate;

- (void)sync:(NSData *)localHash;

@end
