/*
 * KTTime.hh
 *
 *  Created on: Oct 08, 2014
 *      Author: nsoblath
 *
 *      Copied from libthorax's thorax_time.h
 */

#ifndef KTTIME_HH_
#define KTTIME_HH_

#include <inttypes.h>
#include <sys/time.h>
#include <time.h>

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC 1000000000
#endif

#ifndef SEC_PER_NSEC
#define SEC_PER_NSEC 1.e-9
#endif

#ifdef __MACH__
#include <mach/mach_time.h>

#ifndef MACNANO
#define MACNANO (+1.0E-9)
#define MACGIGA UINT64_C(1000000000)
#endif // MACNANO

extern double eTimebase;
extern uint64_t eTimestart;
#endif // __MACH__


extern char eDateTimeFormat[];


int GetTimeMonotonic(struct timespec* time);

int GetTimeCurrent(struct timespec* time);

uint64_t TimeToNSec(struct timespec time);

double TimeToSec(struct timespec time);

void TimeDiff(struct timespec start, struct timespec end, struct timespec* diff);

size_t GetTimeAbsoluteStr(char* ptr);

#endif // KTTIME_HH_
