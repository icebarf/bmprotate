#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long
get_time_nsec (void)
{
#ifndef ICE_DONT_BENCHMARK_CODE
    struct timespec time;
    if (timespec_get (&time, TIME_UTC) == 0)
        {
            fprintf (stdout, "Error: Filling timespec structure failed with "
                             "timespec_get()\n");
        }
    return time.tv_nsec;
#endif
}

long
delta (long t1, long t2)
{
#ifndef ICE_DONT_BENCHMARK_CODE
    return labs (t1 - t2);
#endif
}