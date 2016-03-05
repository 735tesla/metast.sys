//
//  MSCommand.m
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import "MSCommand.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

@interface MSCommand ()

@property (nonatomic) BOOL bufferFull;
@property (nonatomic) BOOL running;

@end

@implementation MSCommand

FILE *_descriptor = NULL;
char *_buffer = NULL;
BOOL _finished = NO;

- (instancetype)initWithCommand:(NSString *)command {
    self = [super init];
    if (self) {
        _command = command;
        _bufferFull = NO;
        _running = NO;
        
        if ((_buffer = malloc(BUFFER_SIZE)))
            return nil;
        
        memset(_buffer, 0, BUFFER_SIZE);
    }
    return self;
}

- (BOOL)run {
    BOOL __block success = NO;
    static dispatch_once_t token = 0;
    dispatch_once(&token, ^{
        if (_descriptor == NULL) {
            if ((_descriptor = popen(_command.UTF8String, "r+")) != NULL) {
                self.running = YES;
                success = YES;
                return;
            }
        }
        self.running = NO;
    });
    return success;
}

- (NSString *)output {
    NSString *output = nil;
    if (self.bufferFull) {
        output = [[NSString alloc] initWithUTF8String:_buffer];
        self.bufferFull = NO;
    } else {
        memset(_buffer, 0, BUFFER_SIZE);
        if (fgets(_buffer, BUFFER_SIZE, _descriptor) == NULL) {
            return nil;
        }
        output = [[NSString alloc] initWithUTF8String:_buffer];
        memset(_buffer, 0, BUFFER_SIZE);
    }
    
    return output;
}

- (BOOL)finished {
    if (_finished)
        return YES;
    
    const char *restrict result = fgets(_buffer, BUFFER_SIZE, _descriptor);
    if (result != NULL)
        return NO;
    
    self.bufferFull = YES;
    self.running = NO;
    _finished = YES;
    return YES;
}

- (void)dealloc {
    pclose(_descriptor);
    free(_buffer);
    _buffer = NULL;
}

@end
