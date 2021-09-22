#include<iostream>
#include "test.hpp"
using namespace std;


// #include <argp.h>
// #include <string.h>
// #include <time.h>
// #include <stdio.h>

// #include "param.hpp"
// #include "logging.h"
// #include "assert.h"

// const char* argp_program_version = "Gjfish 0.0.1";

// static inline void KC__print_usage(const char* program_name) {
//     printf("Usage: %s <CMD> [OPTION...] ARGS...\n"
//            "  <CMD> is one of: count\n"
//            "\n"
//            "  -?, --help                 Give this help list\n"
//            "  -V, --version              Print program version\n",
//            program_name);
// }

// int main(int argc, char** argv) {
//     // KC__ASSERT(argp_program_version != NULL);

//     if (argc < 1) {
//         return 0;
//     }
//     const char* program_name = argv[0];

//     // KC__LOG_FILE = stderr;

//     argc--;
//     argv++;

//     if (argc == 0) {
//         KC__print_usage(program_name);
//         return 0;
//     }

//     if (strcmp(argv[0], "count") == 0) {
//         KC__Param param;
//         KC__param_init(&param, argc, argv);

//         time_t start_time = time(NULL);

//         // KC__MemAllocator *ma = KC__mem_allocator_create(param.mem_limit);

//         // test1(ma);

//         time_t end_time = time(NULL);

//         // KC__param_destroy(&param);

//     }  else if ((strcmp(argv[0], "-?") == 0) || (strcmp(argv[0], "--help") == 0)) {
//         KC__print_usage(program_name);

//     } else if ((strcmp(argv[0], "-V") == 0) || (strcmp(argv[0], "--version") == 0)) {
//         printf("%s\n", argp_program_version);

//     } else {
//         KC__print_usage(program_name);
//     }

//     return 0;
// }

int main()
{
    return 0;
}