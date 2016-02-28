//
//  main.m
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <unistd.h>
#import "MSAPI.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSLog(@"%@", [[NSUUID UUID] UUIDString]);
        pid_t pid = fork();
        if (pid > 0) {
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            exit(EXIT_FAILURE);
        }
        
        while (YES) {
            // Get command from API, execute, send reponse
        }
    }
    return 0;
}
