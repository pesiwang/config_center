//
//  CC.h
//  testdrive
//
//  Created by Chen Rui on 4/17/15.
//  Copyright (c) 2015 Vanchu. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CCItem.h"

#define CC_EVENT_CONFIG_CHANGED @"CC_EVENT_CONFIG_CHANGED"

@interface CC : NSObject

+ (instancetype)sharedInstance;

@property (strong, atomic, readonly) CCNode *root;

@end
