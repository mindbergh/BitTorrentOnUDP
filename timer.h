#ifndef _TIMER_H
#define _TIMER_H

#include <sys/time.h>

#include "bt_parse.h"
#include "queue.h"
#include "chunk.h"
#include "sha.h"
#include "job.h"

int get_time_diff(struct timeval* start);

#endif


