//
//  MSClient.m
//  metast.sys
//
//  Created by James Pickering on 3/4/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import "MSClient.h"
#import "MSAPI.h"

@implementation MSClient

+ (instancetype)sharedClient {
    static MSClient *client = nil;
    static dispatch_once_t token;
    dispatch_once(&token, ^{
        client = [[self alloc] init];
    });
    return client;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _UUID = [[NSUUID UUID] UUIDString];
    }
    return self;
}

- (void)runCommandCycle {
    dispatch_group_t cycleGroup = dispatch_group_create();
    dispatch_group_enter(cycleGroup);
    [MSAPI nextCommand:^(MSCommand * _Nonnull command) {
        [command run];
        while (!command.finished) {
            [MSAPI updateCommandOutput:command handler:nil];
        }
        dispatch_group_enter(cycleGroup);
        [MSAPI didFinishRunningCommand:command handler:^(id  _Nullable response, NSError * _Nullable error) {
            dispatch_group_leave(cycleGroup);
        }];
        dispatch_group_leave(cycleGroup);
    }];
    dispatch_group_wait(cycleGroup, DISPATCH_TIME_FOREVER);
}

@end
