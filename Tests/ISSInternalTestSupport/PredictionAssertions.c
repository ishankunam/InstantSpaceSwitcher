#include "ISSInternalTestSupport.h"
#include <stdio.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); failures++; \
} } while (0)

int iss_test_prediction_lifetime(void) {
    int failures = 0;
    ISSPredictionCache cache = {0};
    unsigned int index = 99;
    CHECK(!iss_prediction_get(&cache, "a", 0, &index));
    iss_prediction_set(&cache, "a", 0, 10);
    iss_prediction_reset_settled(&cache, 10.1); // Notification for our first hop.
    CHECK(iss_prediction_get(&cache, "a", 10.2, &index) && index == 0);
    iss_prediction_set(&cache, "a", 1, 10.2);
    iss_prediction_reset_settled(&cache, 10.3); // Intermediate CGS index is stale.
    CHECK(iss_prediction_get(&cache, "a", 10.4, &index) && index == 1);
    iss_prediction_set(&cache, "a", 2, 10.4);
    CHECK(iss_prediction_get(&cache, "a", 11.39, &index) && index == 2);
    CHECK(!iss_prediction_get(&cache, "a", 11.4, &index)); // Resync external navigation.
    iss_prediction_set(&cache, "a", 1, 20);
    iss_prediction_set(&cache, "b", 3, 20.5);
    iss_prediction_reset_settled(&cache, 21);
    CHECK(!iss_prediction_get(&cache, "a", 21, &index));
    CHECK(iss_prediction_get(&cache, "b", 21, &index) && index == 3);
    CHECK(!iss_prediction_get(&cache, "b", 19, &index)); // Regressing clock expires.
    iss_prediction_destroy(&cache);
    CHECK(cache.entries == NULL && cache.topology == NULL);
    return failures;
}

static CFArrayRef snapshot(const int *ids, int count, int current, bool secondDisplay) {
    CFMutableArrayRef spaces = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    for (int i = 0; i < count; i++) {
        CFNumberRef number = CFNumberCreate(NULL, kCFNumberIntType, &ids[i]);
        const void *keys[] = {CFSTR("id64")};
        const void *values[] = {number};
        CFDictionaryRef space = CFDictionaryCreate(NULL, keys, values, 1,
            &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        CFArrayAppendValue(spaces, space);
        CFRelease(space);
        CFRelease(number);
    }
    CFNumberRef active = CFNumberCreate(NULL, kCFNumberIntType, &current);
    const void *keys[] = {CFSTR("Display Identifier"), CFSTR("Spaces"), CFSTR("Current Space")};
    const void *values[] = {CFSTR("a"), spaces, active};
    CFDictionaryRef display = CFDictionaryCreate(NULL, keys, values, 3,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFMutableArrayRef displays = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    CFArrayAppendValue(displays, display);
    if (secondDisplay) {
        CFMutableDictionaryRef second = CFDictionaryCreateMutableCopy(NULL, 0, display);
        CFDictionarySetValue(second, CFSTR("Display Identifier"), CFSTR("b"));
        CFArrayAppendValue(displays, second);
        CFRelease(second);
    }
    CFRelease(display);
    CFRelease(active);
    CFRelease(spaces);
    return displays;
}

int iss_test_prediction_topology(void) {
    int failures = 0;
    ISSPredictionCache cache = {0};
    const int ids[] = {101, 102, 103};
    const int reordered[] = {101, 103, 102};
    CFArrayRef initial = snapshot(ids, 3, 101, false);
    CFArrayRef ownHop = snapshot(ids, 3, 102, false);
    CFArrayRef moved = snapshot(reordered, 3, 102, false);
    CFArrayRef removed = snapshot(ids, 2, 102, false);
    CFArrayRef attached = snapshot(ids, 3, 102, true);
    unsigned int index = 99;
    iss_prediction_observe_displays(&cache, initial);
    iss_prediction_set(&cache, "a", 2, 10);
    iss_prediction_observe_displays(&cache, ownHop);
    CHECK(iss_prediction_get(&cache, "a", 10.1, &index) && index == 2);
    const CFArrayRef changed[] = {moved, removed, attached, initial};
    for (int i = 0; i < 4; i++) {
        iss_prediction_set(&cache, "a", 1, 10);
        iss_prediction_observe_displays(&cache, changed[i]);
        CHECK(!iss_prediction_get(&cache, "a", 10.1, &index));
    }
    CFRelease(initial);
    CFRelease(ownHop);
    CFRelease(moved);
    CFRelease(removed);
    CFRelease(attached);
    iss_prediction_destroy(&cache);
    return failures;
}
