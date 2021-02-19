#include "SDRunoPlugin_RemoteBridgeSerialTimeout.h"

/*!
	\brief      Constructor of the class TimeOut.
*/
// Constructor
TimeOut::TimeOut()
{}

int TimeOut::gettimeofday(struct timeval* tp, struct timezone* tzp) {
	namespace sc = std::chrono;
	sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
	sc::seconds s = sc::duration_cast<sc::seconds>(d);
	tp->tv_sec = s.count();
	tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();

	return 0;
}

/*!
	\brief      Initialise the timer. It writes the current time of the day in the structure PreviousTime.
*/
//Initialize the timer
void TimeOut::InitTimer()
{
	gettimeofday(&PreviousTime, NULL);
}

/*!
	\brief      Returns the time elapsed since initialization.  It write the current time of the day in the structure CurrentTime.
				Then it returns the difference between CurrentTime and PreviousTime.
	\return     The number of microseconds elapsed since the functions InitTimer was called.
  */
  //Return the elapsed time since initialization
unsigned long int TimeOut::ElapsedTime_ms()
{
	struct timeval CurrentTime;
	int sec, usec;
	gettimeofday(&CurrentTime, NULL);                                   // Get current time
	sec = CurrentTime.tv_sec - PreviousTime.tv_sec;                         // Compute the number of second elapsed since last call
	usec = CurrentTime.tv_usec - PreviousTime.tv_usec;                      // Compute
	if (usec < 0) {                                                       // If the previous usec is higher than the current one
		usec = 1000000 - PreviousTime.tv_usec + CurrentTime.tv_usec;          // Recompute the microseonds
		sec--;                                                          // Substract one second
	}
	return sec * 1000 + usec / 1000;
}