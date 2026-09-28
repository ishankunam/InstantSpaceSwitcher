#include "prediction_cache.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int index;
    double postedAt;
} Prediction;

static bool is_recent(Prediction prediction, double now) {
    // Intermediate notifications must not replace a target with lagging CGS
    // state. Expire independently for each display.
    return isfinite(now) && now >= prediction.postedAt && now - prediction.postedAt < 1.0;
}

bool iss_prediction_get(ISSPredictionCache *cache, const char *display,
                        double now, unsigned int *index) {
    if (!cache || !cache->entries || !display || !*display || !index) return false;
    CFStringRef key = CFStringCreateWithCString(NULL, display, kCFStringEncodingUTF8);
    if (!key) return false;
    CFDataRef value = CFDictionaryGetValue(cache->entries, key);
    bool found = false;
    if (value) {
        Prediction prediction;
        memcpy(&prediction, CFDataGetBytePtr(value), sizeof(prediction));
        found = is_recent(prediction, now);
        if (found) *index = prediction.index;
        else CFDictionaryRemoveValue(cache->entries, key);
    }
    CFRelease(key);
    return found;
}

void iss_prediction_set(ISSPredictionCache *cache, const char *display,
                        unsigned int index, double now) {
    if (!cache || !display || !*display || !isfinite(now)) return;
    if (!cache->entries) {
        cache->entries = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks,
                                                    &kCFTypeDictionaryValueCallBacks);
    }
    if (!cache->entries) return;
    CFStringRef key = CFStringCreateWithCString(NULL, display, kCFStringEncodingUTF8);
    Prediction prediction = {.index = index, .postedAt = now};
    CFDataRef value = CFDataCreate(NULL, (const UInt8 *)&prediction, sizeof(prediction));
    if (key && value) CFDictionarySetValue(cache->entries, key, value);
    if (key) CFRelease(key);
    if (value) CFRelease(value);
}

void iss_prediction_reset_settled(ISSPredictionCache *cache, double now) {
    if (!cache || !cache->entries) return;
    CFDictionaryRef snapshot = CFDictionaryCreateCopy(NULL, cache->entries);
    if (!snapshot) return;
    const CFIndex count = CFDictionaryGetCount(snapshot);
    const void **keys = calloc((size_t)count, sizeof(*keys));
    const void **values = calloc((size_t)count, sizeof(*values));
    if (keys && values) {
        CFDictionaryGetKeysAndValues(snapshot, keys, values);
        for (CFIndex i = 0; i < count; i++) {
            Prediction prediction;
            memcpy(&prediction, CFDataGetBytePtr(values[i]), sizeof(prediction));
            if (!is_recent(prediction, now)) CFDictionaryRemoveValue(cache->entries, keys[i]);
        }
    }
    free(keys);
    free(values);
    CFRelease(snapshot);
}

void iss_prediction_observe_displays(ISSPredictionCache *cache, CFArrayRef displays) {
    if (!cache || !displays) return;
    CFMutableDictionaryRef topology = CFDictionaryCreateMutable(NULL, 0,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    if (!topology) return;
    for (CFIndex i = 0; i < CFArrayGetCount(displays); i++) {
        CFDictionaryRef display = CFArrayGetValueAtIndex(displays, i);
        if (CFGetTypeID(display) != CFDictionaryGetTypeID()) continue;
        CFStringRef identifier = CFDictionaryGetValue(display, CFSTR("Display Identifier"));
        CFArrayRef spaces = CFDictionaryGetValue(display, CFSTR("Spaces"));
        if (!identifier || CFGetTypeID(identifier) != CFStringGetTypeID() ||
            !spaces || CFGetTypeID(spaces) != CFArrayGetTypeID()) continue;
        CFMutableArrayRef identifiers = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
        if (!identifiers) {
            CFRelease(topology);
            if (cache->entries) CFDictionaryRemoveAllValues(cache->entries);
            return;
        }
        for (CFIndex j = 0; j < CFArrayGetCount(spaces); j++) {
            CFDictionaryRef space = CFArrayGetValueAtIndex(spaces, j);
            if (CFGetTypeID(space) != CFDictionaryGetTypeID()) continue;
            CFNumberRef identifier = CFDictionaryGetValue(space, CFSTR("id64"));
            if (identifier && CFGetTypeID(identifier) == CFNumberGetTypeID()) {
                CFArrayAppendValue(identifiers, identifier);
            }
        }
        CFDictionarySetValue(topology, identifier, identifiers);
        CFRelease(identifiers);
    }
    if (!cache->topology || !CFEqual(cache->topology, topology)) {
        if (cache->entries) CFDictionaryRemoveAllValues(cache->entries);
        if (cache->topology) CFRelease(cache->topology);
        cache->topology = topology;
    } else {
        CFRelease(topology);
    }
}

void iss_prediction_destroy(ISSPredictionCache *cache) {
    if (!cache) return;
    if (cache->entries) CFRelease(cache->entries);
    if (cache->topology) CFRelease(cache->topology);
    memset(cache, 0, sizeof(*cache));
}
