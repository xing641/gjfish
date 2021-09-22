#ifndef UTILS_HPP
#define UTILS_HPP
#include<iostream>
#include<cstring>
#include<sstream>
using namespace std;

int ExecuteCMD(const char *cmd, string& result);

static inline bool KC__is_prime_number(size_t n);

size_t KC__max_prime_number(size_t limit);

#endif