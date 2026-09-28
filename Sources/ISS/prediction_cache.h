#ifndef ISS_PREDICTION_CACHE_H
#define ISS_PREDICTION_CACHE_H

#include <CoreFoundation/CoreFoundation.h>
#include <stdbool.h>

typedef struct {
    CFMutableDictionaryRef entries;
    CFDictionaryRef topology;
} ISSPredictionCache;

// Calls stay on ISS's main event loop. Tests supply a deterministic clock.
bool iss_prediction_get(ISSPredictionCache *cache, const char *display,
                        double now, unsigned int *index);
void iss_prediction_set(ISSPredictionCache *cache, const char *display,
                        unsigned int index, double now);
void iss_prediction_reset_settled(ISSPredictionCache *cache, double now);
void iss_prediction_observe_displays(ISSPredictionCache *cache, CFArrayRef displays);
void iss_prediction_destroy(ISSPredictionCache *cache);

#endif
