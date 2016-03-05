//
//  MSCommand.h
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MSCommand : NSObject

/**
 *  Command id
 */
@property (nonatomic, strong) NSNumber *identifier;

/**
 *  The command to be run
 */
@property (nonatomic, strong, readonly) NSString *command;

/**
 *  Output of the command. Updated every time variable is accessed.
 */
@property (nonatomic, strong, readonly) NSString *output;

/**
 *  Checks if the command is running.
 */
@property (nonatomic, readonly) BOOL running;

/**
 *  Checks if the command finished running. 
 *  Note: commands can only be run once. All other attempts to run will fail
 */
@property (nonatomic, readonly) BOOL finished;

/**
 *  Creates an MSCommand instance
 *
 *  @param command command to be run
 *
 *  @return MSCommand instance
 */
- (instancetype)initWithCommand:(NSString *)command;

/**
 *  Runs the command
 *  Note: commands can fail to run if they a) have already been run or b) if popen fails to open a FILE
 *
 *  @return success
 */
- (BOOL)run;

@end
