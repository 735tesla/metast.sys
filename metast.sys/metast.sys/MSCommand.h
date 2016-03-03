//
//  MSCommand.h
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MSCommand : NSObject

@property (nonatomic, strong, readonly) NSString *command;
@property (nonatomic, strong, readonly) NSString *output;
@property (nonatomic, readonly) BOOL running;
@property (nonatomic, readonly) BOOL finished;

- (instancetype)initWithCommand:(NSString *)command;
- (BOOL)run;

@end
