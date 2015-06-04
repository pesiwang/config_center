//
//  CCStorage.m
//  testdrive
//
//  Created by Chen Rui on 4/27/15.
//  Copyright (c) 2015 Vanchu. All rights reserved.
//

#import "CCStorage.h"
#import <zlib.h>

@implementation CCStorage

- (instancetype)init {
	if (self = [super init]) {
		NSFileManager *fileManager = [NSFileManager defaultManager];
		if(![fileManager fileExistsAtPath:[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:CC_STORAGE_FILE]]) {
			NSString *databasePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:CC_STORAGE_FILE];
			[fileManager copyItemAtPath:databasePath toPath:[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:CC_STORAGE_FILE] error:nil];
		}

		[self _loadLocal];
	}
	return self;
}

- (void)save:(NSData *)config withHash:(NSData *)hash {
	CCNode *ccRoot = [self uncompress:config];
	if (ccRoot == nil) {
		NSLog(@"bad content from network");
		return;
	}

	_ccRoot = ccRoot;
	_ccHash = hash;

	sqlite3 *db;
	if (SQLITE_OK != sqlite3_open([[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:CC_STORAGE_FILE] UTF8String], &db)) {
		[NSException raise:@"bad data" format:@"cannot open database"];
	}

	sqlite3_exec(db, "DELETE FROM config", nil, nil, nil);
	sqlite3_stmt *stmt = NULL;
	const char* sql = "REPLACE INTO config(`hash`, `content`) VALUES(?,?)";
	sqlite3_prepare_v2(db, sql, (int)strlen(sql), &stmt, NULL);
	sqlite3_bind_blob(stmt, 1, hash.bytes, (int)(hash.length), NULL);
	sqlite3_bind_blob(stmt, 2, config.bytes, (int)(config.length), NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

- (void)_loadLocal {
	sqlite3 *db;
	if (SQLITE_OK != sqlite3_open([[[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject] stringByAppendingPathComponent:CC_STORAGE_FILE] UTF8String], &db)) {
		[NSException raise:@"bad data" format:@"cannot open database"];
	}

	sqlite3_stmt *stmt = NULL;
	const char* sql = "SELECT `hash`, `content` FROM config";
	sqlite3_prepare_v2(db, sql, (int)strlen(sql), &stmt, NULL);
	if(SQLITE_ROW != sqlite3_step(stmt)){
		[NSException raise:@"bad data" format:@"failed to execute select from local storage"];
	}

	_ccHash = [NSData dataWithBytes:sqlite3_column_blob(stmt, 0) length:sqlite3_column_bytes(stmt, 0)];
	NSData *content = [NSData dataWithBytes:sqlite3_column_blob(stmt, 1) length:sqlite3_column_bytes(stmt, 1)];
	sqlite3_finalize(stmt);
	sqlite3_close(db);


	_ccRoot = [self uncompress:content];
	if (_ccRoot == nil) {
		NSLog(@"bad content from database");
		_ccRoot = [CCNode new];
	}
}

- (CCNode *)uncompress:(NSData *)content {
	NSMutableData *decompressed = [NSMutableData new];
	z_stream stream;
	stream.next_in = (Bytef *)(content.bytes);
	stream.avail_in = (uInt)(content.length);
	stream.total_out = 0;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;

	if (inflateInit2(&stream, (15 + 16)) != Z_OK) return nil;

	Bytef outbuf[0x4000];
	BOOL done = NO;
	while (!done) {
		stream.next_out = outbuf;
		stream.avail_out = 0x4000;
		int status = inflate (&stream, Z_SYNC_FLUSH);
		if (status == Z_STREAM_END) {
			done = YES;
		}
		else if (status != Z_OK) {
			inflateEnd(&stream);
			return nil;
		}
		[decompressed appendBytes:(const void *)outbuf length:0x4000 - stream.avail_out];
	}
	if (inflateEnd (&stream) != Z_OK) return nil;

	NSError *error = nil;
	NSDictionary *dictData = [NSJSONSerialization JSONObjectWithData:decompressed options:0 error:&error];
	if (error || !dictData) {
		return nil;
	}

	return [[CCNode alloc] initWithDictionary:dictData];
}

@end

