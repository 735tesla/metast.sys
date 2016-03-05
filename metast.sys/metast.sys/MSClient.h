//
//  MSClient.h
//  metast.sys
//
//  Created by James Pickering on 3/4/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MSClient : NSObject

@property (strong, nonatomic) NSString *UUID;

+ (instancetype)sharedClient;

- (void)runCommandCycle;

@end
