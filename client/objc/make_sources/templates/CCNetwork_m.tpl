//
//  CCNetwork.m
//  testdrive
//
//  Created by Chen Rui on 4/27/15.
//  Copyright (c) 2015 Vanchu. All rights reserved.
//

#import "CCNetwork.h"
#import "CCDefinition.h"
#import <AsyncSocket.h>
#import <Reachability.h>

@interface CCNetwork()<AsyncSocketDelegate>
{
	AsyncSocket *_socket;
	NSMutableData *_data;
	Reachability *_reachability;
}
@end

@implementation CCNetwork

- (void)dealloc {
	[_reachability stopNotifier];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (instancetype)init {
	self = [super init];
	if (self) {
		_socket = [[AsyncSocket alloc] initWithDelegate:self];
		_data = [NSMutableData new];
		
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onReachabilityChanged:) name:kReachabilityChangedNotification object:nil];
		
		_reachability = [Reachability reachabilityWithHostName:CC_NETWORK_HOST];
		[_reachability startNotifier];
		
		[self onReachabilityChanged:nil];
	}
	return self;
}

- (void)sync:(NSData *)localHash {
	if (!_socket.isConnected) {
		return;
	}
	
	CC_PACKAGE_HEAD head;
	head.version = CC_VERSION;
	head.command = CC_CMD_SYNC;
	head.length = htonl(localHash.length);
	
	NSMutableData *data = [[NSMutableData alloc] initWithBytes:(const void *)&head length:sizeof(head)];
	[data appendData:localHash];
	
	[_socket writeData:data withTimeout:CC_NETWORK_WRITE_TIMEOUT tag:0];
}

#pragma mark - Delegate Functions with Socket
- (void)onSocket:(AsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port {
	[sock readDataWithTimeout:-1 tag:0];

	if ([self.delegate respondsToSelector:@selector(ccNetworkDidConnectToServer:)]) {
		[self.delegate ccNetworkDidConnectToServer:self];
	}
}

- (void)onSocket:(AsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag {
	if (data && data.length > 0) {
		[_data appendData:data];
	}
	[sock readDataWithTimeout:-1 tag:0];
	
	CC_PACKAGE_HEAD head;
	while (_data.length >= sizeof(head)) {
		memcpy(&head, _data.bytes, sizeof(head));
		head.length = ntohl(head.length);
		if (_data.length < 16) {
			break;
		}
		
		[_data replaceBytesInRange:NSMakeRange(0, sizeof(head)) withBytes:NULL length:0];
		
		NSData *hash = [_data subdataWithRange:NSMakeRange(0, 16)];
		NSData *content = [_data subdataWithRange:NSMakeRange(16, head.length - 16)];
		[_data replaceBytesInRange:NSMakeRange(0, head.length) withBytes:NULL length:0];

		if ([self.delegate respondsToSelector:@selector(ccNetwork:didUpdateConfig:withHash:)]) {
			[self.delegate ccNetwork:self didUpdateConfig:content withHash:hash];
		}
	}
}

- (void)onSocketDidDisconnect:(AsyncSocket *)sock {
	if (_data.length > 0) {
		[_data replaceBytesInRange:NSMakeRange(0, _data.length) withBytes:NULL length:0];
	}
	if ([self.delegate respondsToSelector:@selector(ccNetworkDidDisconnectFromServer:)]) {
		[self.delegate ccNetworkDidDisconnectFromServer:self];
	}
	
	if (_reachability.isReachable) {
		dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 5 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
			if ((_reachability.isReachable) && (!_socket.isConnected)) {
				[self onReachabilityChanged:nil];
			}
		});
	}
}

#pragma mark - onReachabilityChanged
- (void)onReachabilityChanged:(NSNotification *)notification {
	if (_reachability.isReachable) {
		if (_socket.isConnected) {
			return;
		}
		
		NSError *error = nil;
		if (![_socket connectToHost:CC_NETWORK_HOST onPort:CC_NETWORK_PORT withTimeout:CC_NETWORK_CONNECT_TIMEOUT error:&error]) {
			[_socket disconnect];
			return;
		}
	}
	else {
		if (!_socket.isConnected) {
			return;
		}
		[_socket disconnect];
	}
}
@end
