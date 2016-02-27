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
@property (nonatomic, readonly) BOOL bufferFull;
@property (nonatomic, readonly) BOOL executing;

- (instancetype)initWithCommand:(NSString *)command;
- (BOOL)startExecution;
- (BOOL)isFinished;
- (NSString*)getOutput;

@end
