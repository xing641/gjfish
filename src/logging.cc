//
// Created by Tuotuo on 1/9/2022.
//

#include <ctime>
#include <cstring>
#include "logging.h"


static char logging_time_str[255];

char* logging_time() {
    time_t t;
    time(&t);
    struct tm* local_t = localtime(&t);
    strftime(logging_time_str, 255, "%Y-%m-%d %H:%M:%S", local_t);
    return logging_time_str;
}

char* logging_src_name(char* src_path) {
    char* src_name = strrchr(src_path, '/');
    if (src_name)
        src_name += 1;
    else
        src_name = src_path;
    return src_name;
}