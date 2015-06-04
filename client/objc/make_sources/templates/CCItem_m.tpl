#import "CCItem.h"

@interface CCString()
{
	NSString *_value;
}
@end
@implementation CCString
- (instancetype)init {
	self = [super init];
	if (self) {
		_value = @"";
	}
	return self;
}

- (instancetype)initWithValue:(NSString *)value {
	if (self = [self init]) {
		_value = value;
	}
	return self;
}
@end

@interface CCInteger()
{
	NSInteger _value;
}
@end
@implementation CCInteger
- (instancetype)init{
	self = [super init];
	if (self) {
		_value = 0;
	}
	return self;
}
- (instancetype)initWithValue:(NSInteger)value {
	if (self = [self init]) {
		_value = value;
	}
	return self;
}
@end

@interface CCBool()
{
	BOOL _value;
}
@end
@implementation CCBool
- (instancetype)init {
	self = [super init];
	if (self) {
		_value = NO;
	}
	return self;
}
- (instancetype)initWithValue:(BOOL)value {
	if (self = [self init]) {
		_value = value;
	}
	return self;
}
@end

NSArray *parse_array(NSArray *items, Class itemClass) {
	NSMutableArray *parsedItems = [[NSMutableArray alloc] initWithCapacity:items.count];
	if (itemClass == CCString.class) {
		for (id rawItem in items) {
			if ([rawItem isKindOfClass:NSString.class]) {
				[parsedItems addObject:[[CCString alloc] initWithValue:rawItem]];
			}
		}
	}
	else if (itemClass == CCInteger.class) {
		for (id rawItem in items) {
			if ([rawItem isKindOfClass:NSNumber.class]) {
				[parsedItems addObject:[[CCInteger alloc] initWithValue:[rawItem integerValue]]];
			}
		}
	}
	else if (itemClass == CCBool.class) {
		for (id rawItem in items) {
			if ([rawItem isKindOfClass:NSNumber.class]) {
				[parsedItems addObject:[[CCInteger alloc] initWithValue:[rawItem boolValue]]];
			}
		}
	}
	else {
		for (id rawItem in items) {
			if ([rawItem isKindOfClass:NSDictionary.class]) {
				[parsedItems addObject:[[itemClass alloc] initWithDictionary:rawItem]];
			}
		}
	}
	return parsedItems;
}

#pragma mark - Generated class
<%foreach $nodesMap as $name => $object%>

@implementation <%$name%>
- (instancetype)init {
	if (self = [super init]) {
<%foreach $object->value as $subname => $subobject%>
		_<%$subname%> = <%if $subobject|is_a:'CCArray'%>(<%$subobject|cc_item_2_objc_classname%> *)@[]<%else%>[<%$subobject|cc_item_2_objc_classname%> new]<%/if%>;
<%/foreach%>
	}
	return self;
}
- (instancetype)initWithDictionary:(NSDictionary *)dictionary {
	if (self = [self init]) {
		id item = nil;
<%foreach $object->value as $subname => $subobject%>
		item = [dictionary objectForKey:@"<%$subname%>"];
<%if $subobject|cc_item_2_objc_classname=="CCString"%>
		if ((item != nil) && [item isKindOfClass:NSString.class]) {
			_<%$subname%> = [[CCString alloc] initWithValue:item];
		}
<%else if $subobject|cc_item_2_objc_classname=="CCInteger"%>
		if ((item != nil) && [item isKindOfClass:NSNumber.class]) {
			_<%$subname%> = [[CCInteger alloc] initWithValue:[item integerValue]];
		}
<%else if $subobject|cc_item_2_objc_classname=="CCBool"%>
		if ((item != nil) && [item isKindOfClass:NSNumber.class]) {
			_<%$subname%> = [[CCBool alloc] initWithValue:[item boolValue]];
		}
<%else if $subobject|is_a:'CCArray'%>
		if ((item != nil) && [item isKindOfClass:NSArray.class]) {
			_<%$subname%> = (NSArray<<%$subobject->value|cc_item_2_objc_classname%>> *)(parse_array(item, <%$subobject->value|cc_item_2_objc_classname%>.class));
		}
<%else%>
		if ((item != nil) && [item isKindOfClass:NSDictionary.class]) {
			_<%$subname%> = [[<%$subobject|cc_item_2_objc_classname%> alloc] initWithDictionary:item];
		}
<%/if%>
<%/foreach%>
	}
	return self;
}
@end
<%/foreach%>
