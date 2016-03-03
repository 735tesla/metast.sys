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

+ (void)makeRequest:(NSString *)action body:(id __nullable)body handler:(void (^)(id __nullable response, NSError * __nullable error))handler {
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@%@", MSBaseURL, action]];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
    request.HTTPMethod = @"POST";
    
    if (body)
        request.HTTPBody = [NSJSONSerialization dataWithJSONObject:body options:0 error:nil];
    
    [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData * __nullable data, NSURLResponse * __nullable response, NSError * __nullable error) {
        
        if (error)
            return handler(nil, error);
        
        NSError *JSONError;
        id JSONObject = [NSJSONSerialization JSONObjectWithData:data options:0 error:&JSONError];
        
        if (JSONError)
            return handler(nil, JSONError);
        
        handler(JSONObject, nil);
    }];
}
@end
