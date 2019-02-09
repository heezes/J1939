#ifndef __GP_TIMER_H__
#define __GP_TIMER_H__

#include "J1939.h"
#include "stdint.h"

typedef uint32_t tClockTime;
/**
 * @brief Timer structure. Use Timer_Set() to set the timer.
 *
 */
struct timer {
  tClockTime start;
  tClockTime interval;
};

/** 
 * @brief This function sets a timer for a specific time.
 * The function Timer_Expired() returns true if 
 * the timer has expired.
 */
void Timer_Set(struct timer *t, tClockTime interval);

/** 
 * @brief This function resets the same interval that was
 * given to the Timer_Set() function. The starting point of the interval is 
 * the last timer value when timer expired. Using this function
 * makes the timer being stable over time.
 */
void Timer_Reset(struct timer *t);

/** 
 * @brief This function resets the same interval that was
 * given to the Timer_Set() function. The starting point of the interval is 
 * the current time.  For a stable timer over time, it is recommended to use 
 * the Timer_Reset() function. 
  */
void Timer_Restart(struct timer *t);

/** 
 * @brief This function verifies if a timer has expired. 
 */
uint8_t Timer_Expired(struct timer *t);

/** 
 * @brief This function returns the remaining time before the timer expires.
 */
tClockTime Timer_Remaining(struct timer *t);

#endif /* __GP_TIMER_H__ */
