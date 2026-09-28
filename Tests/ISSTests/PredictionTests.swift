import ISSInternalTestSupport
import XCTest

final class PredictionTests: XCTestCase {
    func testRapidSwitchesAndExpiry() { XCTAssertEqual(iss_test_prediction_lifetime(), 0) }
    func testSpaceAndDisplayChanges() { XCTAssertEqual(iss_test_prediction_topology(), 0) }
}
