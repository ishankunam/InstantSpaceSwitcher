#include "switch_gesture.h"
#include "event_serialize.h"
#include "gesture_event.h"

#include <float.h>
#include <math.h>
#include <string.h>

void iss_release_prepared_gesture(ISSPreparedGesture *gesture) {
    if (!gesture) return;
    for (size_t i = 0; i < gesture->count; i++) {
        if (gesture->events[i]) CFRelease(gesture->events[i]);
    }
    memset(gesture, 0, sizeof(*gesture));
}

bool iss_prepare_switch_gesture(ISSDirection direction, double velocity,
                                bool modern, bool naturalScrolling,
                                ISSPreparedGesture *gesture) {
    if (!gesture) return false;
    memset(gesture, 0, sizeof(*gesture));
    if ((direction != ISSDirectionLeft && direction != ISSDirectionRight) ||
        !isfinite(velocity) || velocity <= 0) return false;

    const double sign = (direction == ISSDirectionRight ? 1.0 : -1.0) *
                        (modern && naturalScrolling ? -1.0 : 1.0);
    const double progress = sign * (modern ? 1.0 / 65536.0 : (double)FLT_TRUE_MIN);
    // PR #102's release-build fling. It fits the signed 16.16 payload.
    const double fling = sign * (modern ? 9999.0 : velocity);
    const uint8_t phases[] = {1, 2, 4};
    for (size_t i = 0; i < sizeof(phases); i++) {
        CGEventRef event = iss_create_dock_swipe_event(phases[i], progress, fling, modern);
        if (!event) goto failed;
        gesture->events[gesture->count++] = event;
        if (modern) {
            CGEventRef companion = CGEventCreate(NULL);
            if (!companion) goto failed;
            CGEventSetIntegerValueField(companion, (CGEventField)55, 29);
            iss_mark_synthetic_event(companion);
            gesture->events[gesture->count++] = companion;
        }
    }
    return true;
failed:
    iss_release_prepared_gesture(gesture);
    return false;
}

CGEventRef iss_copy_neutral_swipe_ending(CGEventRef event) {
    if (!event) return NULL;
    const int64_t phase = CGEventGetIntegerValueField(event, (CGEventField)132);
    if (phase != 4 && phase != 8) return NULL;
    CGEventRef copy = CGEventCreateCopy(event);
    if (!copy) return NULL;
    CGEventSetDoubleValueField(copy, (CGEventField)124, 0);
    CGEventSetDoubleValueField(copy, (CGEventField)129, 0);
    CGEventSetDoubleValueField(copy, (CGEventField)130, 0);
    // Public field edits alone can leave the old IOHID motion attached.
    CGEventRef neutral = iss_augment_dock_swipe_event(copy);
    CFRelease(copy);
    return neutral;
}
