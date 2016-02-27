//
//  main.m
//  users.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <OpenDirectory/OpenDirectory.h>
#include <err.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        ODSession *s = [ODSession defaultSession];
        ODNode *root = [ODNode nodeWithSession:s name:@"/Local/Default" error:nil];
        ODQuery *usersQuery = [ODQuery queryWithNode:root
                                      forRecordTypes:kODRecordTypeUsers
                                           attribute:kODAttributeTypeAllAttributes
                                           matchType:0
                                         queryValues:nil
                                     returnAttributes:nil
                                      maximumResults:0 error:nil];
        NSArray *users = [usersQuery resultsAllowingPartial:NO error:nil];
        for (ODRecord *user in users) {
            NSString *userName = [user recordName];
            struct passwd *userPwd = getpwnam([userName UTF8String]);
            if (userPwd->pw_uid < 500 || userPwd->pw_uid >= 4294967294)
                continue;
            NSString *fullName = [user valuesForAttribute:kODAttributeTypeFullName error:nil][0];
            printf("%s : %s\n", [userName UTF8String], [fullName UTF8String]);
        }
    }
    return 0;
}
