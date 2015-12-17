#include <time.h>
#include <sys/time.h>
#include <sys/cdefs.h>

int
clock_gettime(int clock __attribute__ ((unused)), struct timespec *ts)
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) == -1)
		return -1;
	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = tv.tv_usec * 1000;
	return 0;
}
