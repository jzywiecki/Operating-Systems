#include "timers.h"
#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>

long double time_difference(clock_t t1, clock_t t2){
    return (long double)(t2 - t1) / sysconf(_SC_CLK_TCK);
}

void print_results(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    printf("real time: %Lf\n", time_difference(clock_start, clock_end));
    printf("user time: %Lf\n", time_difference(start_tms.tms_utime, end_tms.tms_utime));
    printf("sys time: %Lf\n\n", time_difference(start_tms.tms_stime, end_tms.tms_stime));
}
