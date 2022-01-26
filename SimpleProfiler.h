/*

		 #####################################
		 #####################################
			      Profiler.h
		 #####################################
		 #####################################

*/
#ifndef PROFILER_H
#define PROFILER_H

/*

USAGE(bies):

TimeState state;
profiler_start(&state);
// do smth we want profile
profiler_end(&state);

state.Result; <- time in ns
i64 ms = profiler_get_microseconds(&state);

*/

#define PROFILER_NS_TO_S(ns)   (ns / (1000 * 1000 * 1000))
#define PROFILER_NS_TO_MS(ns)  (ns / (1000 * 1000))
#define PROFILER_NS_TO_MCS(ns) (ns / (1000))

typedef enum ProfilerTimeType
{
    PROFILER_TIME_NS = 0,
    PROFILER_TIME_MCS,
    PROFILER_TIME_MS,
    PROFILER_TIME_S,
} ProfilerTimeType;

#ifdef LINUX_PLATFORM

#include <time.h>
static i64 g_DeltaInNanoseconds;
static struct timespec g_StartTime;
static struct timespec g_EndTime;

typedef struct TimeState
{
    struct timespec Start;
    struct timespec End;
    i64 Result;
} TimeState;

force_inline void
profiler_start(TimeState* state)
{
    clock_gettime(CLOCK_REALTIME, &state->Start);
}

force_inline void
profiler_end(TimeState* state)
{
    clock_gettime(CLOCK_REALTIME, &state->End);
    state->Result = (1000 * 1000 * 1000 * (state->End.tv_sec - state->Start.tv_sec)) + (state->End.tv_nsec - state->Start.tv_nsec);
}


#elif WINDOWS_PLATFORM

// NOTE: check if delta in nanoseconds
static i64 g_StartTime;
static i64 g_EndTime;
#include <profileapi.h>

typedef struct TimeState
{
    LARGE_INTEGER Start;
    LARGE_INTEGER End;
    LARGE_INTEGER Result;
} TimeState;

force_inline void
profiler_start(TimeState* state)
{
    QueryPerformanceCounter(&state->Start);
}

force_inline void
profiler_end(TimeState* state)
{
    QueryPerformanceCounter(&state->End);
    state->Result = state->End - state->Start;
}

#endif

force_inline ProfilerTimeType
profiler_get_time_type(TimeState* state)
{
    if (PROFILER_NS_TO_S(state->Result))
    {
	return PROFILER_TIME_S;
    }
    else if (PROFILER_NS_TO_MS(state->Result))
    {
	return PROFILER_TIME_MS;
    }
    else if (PROFILER_NS_TO_MCS(state->Result))
    {
	return PROFILER_TIME_MCS;
    }
    else if (state->Result)
    {
	return PROFILER_TIME_NS;
    }

    assert(0 && "");
    return PROFILER_TIME_NS;
}

force_inline i64
profiler_get_nanoseconds(TimeState* state)
{
    return state->Result;
}

force_inline i64
profiler_get_microseconds(TimeState* state)
{
    return state->Result / 1000;
}

force_inline i64
profiler_get_milliseconds(TimeState* state)
{
    return state->Result / (1000 * 1000);
}

force_inline i64
profiler_get_seconds(TimeState* state)
{
    return state->Result / (1000 * 1000 * 1000);
}

force_inline f64
profiler_get_microseconds_as_float(TimeState* state)
{
    return ((f64)state->Result) / 1000;
}

force_inline f64
profiler_get_milliseconds_as_float(TimeState* state)
{
    return ((f64)state->Result) / (1000 * 1000);
}

force_inline f64
profiler_get_seconds_as_float(TimeState* state)
{
    return ((f64)state->Result) / (1000 * 1000 * 1000);
}

force_inline void
profiler_print(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);
    switch (timeType)
    {
    case PROFILER_TIME_NS:
	printf("%ld %s\n", profiler_get_nanoseconds(state), "ns");
	break;
    case PROFILER_TIME_MCS:
	printf("%ld %s\n", profiler_get_microseconds(state), "mcs");
	break;
    case PROFILER_TIME_MS:
	printf("%ld %s\n", profiler_get_milliseconds(state), "ms");
	break;
    case PROFILER_TIME_S:
	printf("%ld %s\n", profiler_get_seconds(state), "s");
	break;
    default:
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
}

force_inline void
profiler_print_as_float(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);

    switch (timeType)
    {
    case PROFILER_TIME_NS:
	printf("%ld %s\n", profiler_get_nanoseconds(state), "ns");
	break;
    case PROFILER_TIME_MCS:
	printf("%.4f %s\n", profiler_get_microseconds_as_float(state), "mcs");
	break;
    case PROFILER_TIME_MS:
	printf("%.4f %s\n", profiler_get_milliseconds_as_float(state), "ms");
	break;
    case PROFILER_TIME_S:
	printf("%.4f %s\n", profiler_get_seconds_as_float(state), "s");
	break;
    default:
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
}

static char g_TimeString[512];

force_inline char*
profiler_get_string(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);

    switch (timeType)
    {
    case PROFILER_TIME_NS:
	string_format(g_TimeString, "%ld %s", profiler_get_nanoseconds(state), "ns");
	break;
    case PROFILER_TIME_MCS:
	string_format(g_TimeString, "%ld %s", profiler_get_microseconds(state), "mcs");
	break;
    case PROFILER_TIME_MS:
	string_format(g_TimeString, "%ld %s", profiler_get_milliseconds(state), "ms");
	break;
    case PROFILER_TIME_S:
	string_format(g_TimeString, "%ld %s", profiler_get_seconds(state), "s");
	break;
    default:
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }

    return (char*)g_TimeString;
}

force_inline char*
profiler_get_string_as_float(TimeState* state)
{
    ProfilerTimeType timeType = profiler_get_time_type(state);

    switch (timeType)
    {
    case PROFILER_TIME_NS:
    {
	f64 temp = profiler_get_nanoseconds(state);
	string_format(g_TimeString, "%f %s\n", temp, "ns");
	break;
    }
    case PROFILER_TIME_MCS:
    {
	f64 temp = profiler_get_microseconds_as_float(state);
	string_format(g_TimeString, "%f %s\n", temp, "mcs");
	break;
    }
    case PROFILER_TIME_MS:
    {
	f64 temp = profiler_get_milliseconds_as_float(state);
	string_format(g_TimeString, "%f %s\n", temp, "ms");
	break;
    }
    case PROFILER_TIME_S:
    {
	f64 temp = profiler_get_seconds_as_float(state);
	string_format(g_TimeString, "%f %s\n", temp, "s");
	break;
    }
    default:
    {
	assert(0 && "Just a thing to delete compiler warning message, this code never ever ll be executed!");
	break;
    }
    }

    return (char*)g_TimeString;
}


#endif
