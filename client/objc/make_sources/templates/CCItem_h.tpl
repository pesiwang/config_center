#import <Foundation/Foundation.h>

#import "CCDefinition.h"

@protocol CCString <NSObject>
@end
@interface CCString : NSObject
- (instancetype)initWithValue:(NSString *)value;
@property (strong, nonatomic, readonly) NSString *value;
@end

@protocol CCInteger <NSObject>
@end
@interface CCInteger : NSObject
- (instancetype)initWithValue:(NSInteger)value;
@property (assign, nonatomic, readonly) NSInteger value;
@end

@protocol CCBool <NSObject>
@end
@interface CCBool : NSObject
- (instancetype)initWithValue:(BOOL)value;
@property (assign, nonatomic, readonly) BOOL value;
@end

@protocol CCObject <NSObject>
@required
- (instancetype)initWithDictionary:(NSDictionary *)dictionary;
@end

#pragma mark - Generated class
<%foreach $nodesMap as $name => $object%>

@class <%$name%>;
@protocol <%$name%> <NSObject>
@end
<%/foreach%>
<%foreach $nodesMap as $name => $object%>

@interface <%$name%> : NSObject<CCObject> 
<%foreach $object->value as $subname => $subobject%>
@property (strong, nonatomic, readonly) <%$subobject|cc_item_2_objc_classname%> *<%$subname%>;
<%/foreach%>
@end
<%/foreach%>
