#include "CLF_Time.h"
#include <time.h>

uint32_t CLF_Time::nowEpoch()
{
    time_t now;
    time(&now);
    return (uint32_t)now;
}