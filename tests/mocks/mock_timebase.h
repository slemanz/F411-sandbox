#ifndef MOCK_TIMEBASE_H
#define MOCK_TIMEBASE_H

#include <stdint.h>
#include "interface/interface.h"

/**
 * @brief Set the value returned by timebase_get()->get().
 *        Call this in your test to simulate time passing.
 */
void mock_timebase_set_ticks(uint64_t ticks);

/**
 * @brief Advance the simulated tick counter by a given amount.
 */
void mock_timebase_advance(uint64_t delta);

/**
 * @brief Reset the simulated tick counter to zero.
 */
void mock_timebase_reset(void);

/**
 * @brief Returns the mock timebase interface.
 *        Linked in place of the real timebase_get() via the Makefile.
 */
timebase_interface_t *timebase_get(void);

#endif /* MOCK_TIMEBASE_H */
