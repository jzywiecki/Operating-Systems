#ifndef CW02_TIMERS_H
#define CW02_TIMERS_H
#include <sys/times.h>
#include <unistd.h>

long double time_difference(clock_t t1, clock_t t2);

void print_results(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms);

#endif //CW02_TIMERS_H
