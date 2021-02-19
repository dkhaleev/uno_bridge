#pragma once
#include <sys/utime.h>
#include <chrono>

#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED



#define timerisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp,uvp,cmp)					\
		((tvp)->tv_sec cmp (uvp)->tv_sec ||		\
		 ((tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec))
#define timerclear(tvp)		(tvp)->tv_sec = (tvp)->tv_usec = 0

#endif /* _TIMEVAL_DEFINED */

class TimeOut
{
public:
	struct timeval
	{
		long tv_sec;
		long tv_usec;
	};

	// Constructor
	TimeOut();

	// Init the timer
	void                InitTimer();

	// Return the elapsed time since initialization
	unsigned long int   ElapsedTime_ms();
	int gettimeofday(struct timeval * tp, struct timezone * tzp);

private:
	struct timeval      PreviousTime;
};