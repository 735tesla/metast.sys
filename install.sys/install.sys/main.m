//
//  main.m
//  install.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSAPI.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        [[[MSAPI alloc] init] make]
    }
    return 0;
}
