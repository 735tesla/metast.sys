//
//  MSAPI.m
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import "MSAPI.h"

@implementation MSAPI

NSString const *MSBaseURL = @"http ://127.0.0.1:8080/";

- (instancetype)init
{
    self = [super init];
    if (self) {
        
    }
    return self;
}

- (void)makeRequest:(NSString *)action withHandler:(void (^)(NSData *data, NSURLResponse *response, NSError *error))ourBlock {
    
}
@end
