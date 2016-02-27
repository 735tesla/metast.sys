//
//  MSAPI.m
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import "MSAPI.h"

NSString const *MSBaseURL = @"http://nottesla.com/";

@implementation MSAPI

- (instancetype)init
{
    self = [super init];
    if (self) {
        
    }
    return self;
}

- (void)makeRequest:(NSString *)action withHandler:(void (^)(NSData *data, NSURLResponse *response, NSError *error))ourBlock {

    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/%@", MSBaseURL, action]];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    [request ]
    
    [[[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:ourBlock] resume];
}
@end
