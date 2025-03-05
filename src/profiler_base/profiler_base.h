#ifndef __PROFILER_BASE_H_
#define __PROFILER_BASE_H_

#ifdef EMERALDS_ENABLE_PROFILER
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>

  #define _EMERALDS_SELECT_PROFILE(_1, _2, NAME, ...) NAME

  #define _EMERALDS_PROFILE1(...) _EMERALDS_PROFILE2(__func__, __VA_ARGS__)
  #define _EMERALDS_PROFILE2(name, ...) \
    do {                                \
      profiler_start_profiling(name);   \
      __VA_ARGS__;                      \
      profiler_stop_profiling(name);    \
    } while(0)

  #define PROFILE(...)                                    \
    _EMERALDS_SELECT_PROFILE(                             \
      __VA_ARGS__, _EMERALDS_PROFILE2, _EMERALDS_PROFILE1 \
    )                                                     \
    (__VA_ARGS__)

/**
 * @brief Data for each profile
 * @param function_name - The name of the function
 * @param call_count - The number of times the function was called
 * @param total_time - The total time the function took to execute
 * @param start_time - The time the function started executing
 */
typedef struct EmeraldsProfileData {
  const char *function_name;
  size_t call_count;
  double total_time;
  double start_time;
} EmeraldsProfileData;

/**
 * @brief Profiler object
 * @param profile_data - An array of EmeraldsProfileData
 * @param profile_index - The index of the last profile
 */
typedef struct EmeraldsProfiler {
  EmeraldsProfileData profile_data[65536];
  size_t profile_index;
} EmeraldsProfiler;

/** @brief A global object used internally for profile data */
static EmeraldsProfiler emeralds_profiler;

  #if defined(_WIN32)
    #include <Windows.h>
  #elif defined(__unix__) || defined(__linux__)
    #if defined(__clang__)
      #define CSPEC_CLOCKID 0
    #endif

    #include <sys/time.h>

    #if !defined(CSPEC_CLOCKID)
      #if defined(CLOCK_MONOTONIC)
        #define CSPEC_CLOCKID CLOCK_MONOTONIC
      #else
        #define CSPEC_CLOCKID 1
      #endif
    #endif
  #elif defined(__MACH__) && defined(__APPLE__)
    #include <mach/mach.h>
    #include <mach/mach_time.h>
    #include <time.h>
  #else
    #include <time.h>
  #endif

/**
 * @desc: A cross platform timer function for profiling
 * @return The time in nanoseconds
 */
static size_t _profiler_get_time(void) {
  static size_t is_init = 0;

  #if defined(_WIN32)
  static LARGE_INTEGER win_frequency;
  if(0 == is_init) {
    QueryPerformanceFrequency(&win_frequency);
    is_init = 1;
  }
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return (1e9 * now.QuadPart) / win_frequency.QuadPart;
  #elif defined(__APPLE__)
  static mach_timebase_info_data_t info;
  size_t now;
  if(0 == is_init) {
    mach_timebase_info(&info);
    is_init = 1;
  }
  now = mach_absolute_time();
  now *= info.numer;
  now /= info.denom;
  return now;
  #elif defined(__linux)
  struct timeval tv;
  if(0 == is_init) {
    is_init = 1;
  }
  gettimeofday(&tv, NULL);
  return (size_t)(tv.tv_sec * 1000000000LL + tv.tv_usec * 1000LL);
  #endif
}

/**
 * @brief Begins a profile for a function
 * @param function_name - The name of the function to profile
 */
void profiler_start_profiling(const char *function_name) {
  size_t i;
  double start_time = _profiler_get_time();
  for(i = 0; i < emeralds_profiler.profile_index; ++i) {
    if(strcmp(emeralds_profiler.profile_data[i].function_name, function_name) ==
       0) {
      emeralds_profiler.profile_data[i].start_time = start_time;
      emeralds_profiler.profile_data[i].call_count++;
      return;
    }
  }
  emeralds_profiler.profile_data[emeralds_profiler.profile_index]
    .function_name = strdup(function_name);
  emeralds_profiler.profile_data[emeralds_profiler.profile_index].call_count =
    1;
  emeralds_profiler.profile_data[emeralds_profiler.profile_index].total_time =
    0;
  emeralds_profiler.profile_data[emeralds_profiler.profile_index].start_time =
    start_time;
  emeralds_profiler.profile_index++;
}

/**
 * @brief Stops a profile for a function
 * @param function_name - The name of the function to stop profiling
 */
void profiler_stop_profiling(const char *function_name) {
  size_t i;
  double end_time = _profiler_get_time();
  for(i = 0; i < emeralds_profiler.profile_index; ++i) {
    if(strcmp(emeralds_profiler.profile_data[i].function_name, function_name) ==
       0) {
      emeralds_profiler.profile_data[i].total_time +=
        end_time - emeralds_profiler.profile_data[i].start_time;
        emeralds_profiler.profile_data[i].total_time /= 100000000;
      return;
    }
  }
}

/** @brief Displays the all profile data */
void profiler_display(void) {
  size_t i;
  printf("%-15s %-13s %-10s\n", "Function Name", "Call Count", "Total Time");
  for(i = 0; i < emeralds_profiler.profile_index; ++i) {
    printf(
      "%-15s %-13lu %.5f sec\n",
      emeralds_profiler.profile_data[i].function_name,
      emeralds_profiler.profile_data[i].call_count,
      emeralds_profiler.profile_data[i].total_time
    );
  }
}

#else
  #define PROFILE(...)
  #define profiler_display()
#endif

#endif
