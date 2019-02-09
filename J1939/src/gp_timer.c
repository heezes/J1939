/* Includes ------------------------------------------------------------------*/

#include "gp_timer.h"

/*---------------------------------------------------------------------------*/

void Timer_Set(struct timer *t, tClockTime interval)
{
  t->interval = interval;
  t->start = HAL_GetTick();
}

void Timer_Reset(struct timer *t)
{
  t->start += t->interval;
}

void Timer_Restart(struct timer *t)
{
  t->start = HAL_GetTick();
}

uint8_t Timer_Expired(struct timer *t)
{
  tClockTime diff = (HAL_GetTick() - t->start) + 1;
  return (t->interval < diff);
}

tClockTime Timer_Remaining(struct timer *t)
{
  return (t->start + t->interval - HAL_GetTick());
}



