#ifndef ISS_SWITCH_GESTURE_H
#define ISS_SWITCH_GESTURE_H

#include "include/ISS.h"
#include <ApplicationServices/ApplicationServices.h>

typedef struct {
    CGEventRef events[6];
    size_t count;
} ISSPreparedGesture;

// Prepare everything before posting so allocation failure cannot leave an
// open gesture in the Dock. A successful result must be released.
bool iss_prepare_switch_gesture(ISSDirection direction, double velocity,
                                bool modern, bool naturalScrolling,
                                ISSPreparedGesture *gesture);
void iss_release_prepared_gesture(ISSPreparedGesture *gesture);
CGEventRef iss_copy_neutral_swipe_ending(CGEventRef event);

#endif
