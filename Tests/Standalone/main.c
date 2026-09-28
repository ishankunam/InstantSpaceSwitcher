#include "ISSInternalTestSupport.h"
#include <stdio.h>

int main(void) {
    int failures = iss_test_fixed_point() + iss_test_serialized_payload() +
                   iss_test_serialized_validation() + iss_test_synthetic_identity() +
                   iss_test_prepared_gestures() + iss_test_neutral_ending() +
                   iss_test_prediction_lifetime() + iss_test_prediction_topology();
    printf("Gesture regression failures: %d\n", failures);
    return failures ? 1 : 0;
}
