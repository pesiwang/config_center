//
//  CC.m
//  testdrive
//
//  Created by Chen Rui on 4/17/15.
//  Copyright (c) 2015 Vanchu. All rights reserved.
//

#import "CC.h"
#import "CCStorage.h"
#import "CCNetwork.h"

@interface CC()<CCNetworkDelegate>
{
	CCStorage *_storage;
	CCNetwork *_network;
	NSTimer *_timer;
}
@end

@implementation CC

+ (instancetype)sharedInstance {
	static CC *sharedInstance = nil;
	static dispatch_once_t onceToken = 0;
	dispatch_once(&onceToken, ^{
		sharedInstance = [CC new];
	});
	return sharedInstance;
}

- (instancetype)init {
	self = [super init];
	if (self) {
		_storage = [CCStorage new];
		_network = [CCNetwork new];
		_network.delegate = self;
	}
	return self;
}

- (CCNode *)root {
	return _storage.ccRoot;
}

#pragma mark - Timer
- (void)onTimer:(NSTimer *)timer {
	[_network sync:_storage.ccHash];
}

#pragma mark - CCNetworkDelegate
- (void)ccNetworkDidConnectToServer:(CCNetwork *)network {
	[network sync:_storage.ccHash];
	_timer = [NSTimer scheduledTimerWithTimeInterval:30.0f target:self selector:@selector(onTimer:) userInfo:nil repeats:YES];
}

- (void)ccNetwork:(CCNetwork *)network didUpdateConfig:(NSData *)config withHash:(NSData *)hash {
	[_storage save:config withHash:hash];
	[[NSNotificationCenter defaultCenter] postNotificationName:CC_EVENT_CONFIG_CHANGED object:self];
}

- (void)ccNetworkDidDisconnectFromServer:(CCNetwork *)network {
	if (_timer != nil) {
		[_timer invalidate];
		_timer = nil;
	}
}

@end
