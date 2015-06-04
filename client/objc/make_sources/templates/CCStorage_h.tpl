//
//  CCStorage.h
//  testdrive
//
//  Created by Chen Rui on 4/27/15.
//  Copyright (c) 2015 Vanchu. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "CCItem.h"
#import <sqlite3.h>

@interface CCStorage : NSObject
@property (strong, nonatomic, readonly) NSData *ccHash;
@property (strong, nonatomic, readonly) CCNode *ccRoot;

- (void)save:(NSData *)config withHash:(NSData *)hash;
@end
