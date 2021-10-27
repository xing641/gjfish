#include "utils.hpp"

int ExecuteCMD(const char *cmd, string& result)
{
    int iRet = -1;
    char buf_ps[1024];
    char ps[1024] = {0};
    FILE *ptr;

    strcpy(ps, cmd);

    if((ptr = popen(ps, "r")) != NULL)
    {
        while(fgets(buf_ps, sizeof(buf_ps), ptr) != NULL)
        {
           istringstream ss(buf_ps);
           ss >> result;
        }
        pclose(ptr);
        ptr = NULL;
        iRet = 0;  // 处理成功
    }
    else
    {
        printf("popen %s error\n", ps);
        iRet = -1; // 处理失败
    }

    return iRet;
}

static inline bool KC__is_prime_number(size_t n) {
    if (n < 2)
        return false;
    for (size_t i = 2; i * i <= n; i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

size_t KC__max_prime_number(size_t limit) {
    size_t n = limit;
    while (!KC__is_prime_number(n) && (n > 0))
        n--;
    if (n == 0) {
        exit(EXIT_FAILURE);
    }
    return n;
}