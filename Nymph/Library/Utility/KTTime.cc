/*
 * KTTime.cc
 *
 *  Created on: Oct 08, 2014
 *      Author: nsoblath
 *
 *      Copied from libthorax's thorax_time.c
 */

#include "KTTime.hh"

char eDateTimeFormat[] = "%FT%TZ";

#ifdef __MACH__
double eTimebase = 0.0;
uint64_t eTimeStart = 0;
#endif

int GetTimeMonotonic(struct timespec* time)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    if (! eTimeStart)
    {
        mach_timebase_info_data_t tb = { .numer = 0, .denom = 1 };
        mach_timebase_info(&tb);
        eTimebase = tb.numer;
        eTimebase /= tb.denom;
        eTimeStart = mach_absolute_time();
    }
    double diff = (mach_absolute_time() - eTimeStart) * eTimebase;
    time->tv_sec = diff * MACNANO;
    time->tv_nsec = diff - (time->tv_sec * MACGIGA);
    return 0;
#else
    return clock_gettime( CLOCK_MONOTONIC, time );
#endif
}

int GetTimeCurrent(struct timespec* time)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    if (! eTimeStart)
    {
        mach_timebase_info_data_t tb = { .numer = 0, .denom = 1 };
        mach_timebase_info(&tb);
        eTimebase = tb.numer;
        eTimebase /= tb.denom;
        eTimeStart = mach_absolute_time();
    }
    double diff = (mach_absolute_time() - eTimeStart) * eTimebase;
    time->tv_sec = diff * MACNANO;
    time->tv_nsec = diff - (time->tv_sec * MACGIGA);
    return 0;
#else
    return clock_gettime(CLOCK_PROCESS_CPUTIME_ID, time);
#endif

}

uint64_t TimeToNSec(struct timespec time)
{
    return (long long int)time.tv_sec * (long long int)NSEC_PER_SEC + (long long int)time.tv_nsec;
}

double TimeToSec(struct timespec time)
{
    return (double)time.tv_sec + (double)time.tv_nsec / (double)NSEC_PER_SEC;
}

void TimeDiff(struct timespec start, struct timespec end, struct timespec* diff)
{
    if ((end.tv_nsec - start.tv_nsec < 0))
    {
        diff->tv_sec = end.tv_sec - start.tv_sec - 1;
        diff->tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        diff->tv_sec = end.tv_sec - start.tv_sec;
        diff->tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return;
}

size_t GetTimeAbsoluteStr(char* ptr)
{
    time_t raw_time;
    struct tm* processed_time;

    time(&raw_time);
    processed_time = gmtime(&raw_time);
    return strftime(ptr, 512, eDateTimeFormat, processed_time);
}
