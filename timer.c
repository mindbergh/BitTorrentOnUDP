#include "timer.h"


/** @brief Get a time interval between now and given start time
 *  @param start The start time point
 *  @return a double represent time interval in second.
 */
int get_time_diff(struct timeval* start) {
	struct timeval now;
	double t1 = start->tv_sec+(start->tv_usec/1000000.0);
	double t2;
	gettimeofday(&now, NULL);
	t2=now.tv_sec+(now.tv_usec/1000000.0);
	return (int)((t2 - t1)*1000);
}