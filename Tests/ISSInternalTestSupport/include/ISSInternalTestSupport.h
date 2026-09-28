// Test-only access to internal headers. These are not part of ISS's public module.
#include "../../../Sources/ISS/event_serialize.h"
#include "../../../Sources/ISS/gesture_event.h"
#include "../../../Sources/ISS/switch_gesture.h"
#include "../../../Sources/ISS/prediction_cache.h"

int iss_test_fixed_point(void);
int iss_test_serialized_payload(void);
int iss_test_serialized_validation(void);
int iss_test_synthetic_identity(void);
int iss_test_prepared_gestures(void);
int iss_test_neutral_ending(void);
int iss_test_prediction_lifetime(void);
int iss_test_prediction_topology(void);
