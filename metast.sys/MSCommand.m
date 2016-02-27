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

@implementation MSCommand

FILE *descriptor = NULL;
char *buffer = NULL;

- (instancetype)initWithCommand:(NSString *)command {
    self = [super init];
    if (self) {
        _command = command;
        _bufferFull = NO;
        _executing = NO;
        if ((buffer = malloc(BUFFER_SIZE)) == NULL) {
            return nil;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
    return self;
}

- (BOOL)startExecution {
    _executing = YES;
    if (descriptor == NULL) {
        if ((descriptor = popen([_command UTF8String], "r+")) != NULL) {
            return YES;
        }
    }
    return NO;
}

- (BOOL)isFinished {
    const char *restrict result = fgets(buffer, BUFFER_SIZE, descriptor);
    _bufferFull = YES;
    return result == NULL;
}


- (NSString*)getOutput {
    NSString *output = nil;
    if (_bufferFull) {
        output = [[NSString alloc] initWithUTF8String:buffer];
        _bufferFull = NO;
    } else {
        memset(buffer, 0, BUFFER_SIZE);
        if (fgets(buffer, BUFFER_SIZE, descriptor) == NULL) {
            return nil;
        }
        output = [[NSString alloc] initWithUTF8String:buffer];
        memset(buffer, 0, BUFFER_SIZE);
    }
    return output;
}

- (void)dealloc {
    pclose(descriptor);
    free(buffer);
    buffer = NULL;
}

@end
