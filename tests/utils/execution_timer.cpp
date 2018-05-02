
//extern "C" {
#include "execution_timer.h"
//}

/*
clock_gettime:

Sufficiently recent versions of GNU libc and the Linux kernel support the following clocks:

CLOCK_REALTIME
System-wide realtime clock. Setting this clock requires appropriate privileges.
CLOCK_MONOTONIC
Clock that cannot be set and represents monotonic time since some unspecified starting point.
CLOCK_PROCESS_CPUTIME_ID
High-resolution per-process timer from the CPU.
CLOCK_THREAD_CPUTIME_ID
Thread-specific CPU-time clock.
*/

//#define USECHRONOTIMER

#include<time.h>
#include<stdio.h>

#ifdef USECHRONOTIMER
#include <chrono>
#include <iostream>
#endif

/*

#include <cstdio>
#include <ctime>
#include <chrono>

.... 

std::clock_t startcputime = std::clock();
do_some_fancy_stuff();
double cpu_duration = (std::clock() - startcputime) / (double)CLOCKS_PER_SEC;
std::cout << "Finished in " << cpu_duration << " seconds [CPU Clock] " << std::endl;


auto wcts = std::chrono::system_clock::now();
do_some_fancy_stuff();
std::chrono::duration<double> wctduration = (std::chrono::system_clock::now() - wcts);
std::cout << "Finished in " << wctduration.count() << " seconds [Wall Clock]" << std::endl;

*/

#ifdef USECHRONOTIMER
//static std::chrono::time_point<std::chrono::high_resolution_clock> wcstart, wcend;
static std::chrono::high_resolution_clock::time_point wcstart, wcend;
#else
static struct timespec start, end;
#endif


void __timer_prologue() {
  printf("Executing before main\n");

#ifdef USECHRONOTIMER
  //wcstart = std::chrono::system_clock::now();
  wcstart = std::chrono::high_resolution_clock::now();
#else
  //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  clock_gettime(CLOCK_REALTIME, &start);
#endif
}

void __timer_epilogue() {
#ifdef USECHRONOTIMER
  //wcend = std::chrono::system_clock::now();
  wcend = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> wctduration(wcend - wcstart);
  //std::cout << "Finished in " << wctduration.count() << " seconds [Wall Clock]" << std::endl;
  double runtime_sec = wctduration.count();
#else
  //clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
  clock_gettime(CLOCK_REALTIME, &end);
  double runtime_sec = ((end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0);
#endif
  FILE *fpexectime = fopen("/tmp/exec-time.txt","w");
  fprintf(fpexectime, "%f", runtime_sec);
  fclose(fpexectime);
  printf("Executing after main\n");
}

