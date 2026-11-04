# Tests and Observations

## Recorded Acceptance Results

The project's acceptance sheet records eight passed checks and one area that still needs improvement. Deliberate horizontal shaking was confirmed as PASS by the project owner on 4 November 2026. These are the owner's reported test results.

| Test action | Expected result | Recorded result |
|---|---|---|
| Start face-up | Flow begins | PASS |
| Start face-down | Opposite flow begins | PASS |
| Flip 10 times with pauses | Correct direction with minimal false shake | PASS |
| Light hand jitter | No disruptive shake animation | NEEDS IMPROVEMENT |
| Deliberate horizontal shaking | Visible disturbance without resetting the timer | PASS |
| Tap, wait 5 seconds, then tap again | Frame and countdown freeze, then resume | PASS |
| Hold touch for 3 seconds | One toggle only | PASS |
| Full uninterrupted transfer | Approximately 30 seconds, then breathing | PASS |
| USB cable through the enclosure opening | No cable strain or loose connectors | PASS |

The results confirm the listed actions under the tested conditions. They do not provide a measured timing error, a general false-trigger rate or a physical map of the classroom lamp.

## Confirmed Enclosure Measurements

| Part | Confirmed size |
|---|---|
| Overall enclosure height | 24.5 cm (245 mm) |
| Reported overall enclosure width | 8.5 cm (85 mm) |
| Central rectangle on the front face | 6 cm wide × 5 cm high (60 × 50 mm) |
| Central rectangle on the back face | 6 cm wide × 5 cm high (60 × 50 mm) |
| Breadboard footprint | 8.2 × 5.5 cm (82 × 55 mm) |

The owner confirmed that both central rectangles retained the earlier 6 × 5 cm design dimensions. Other values from earlier enclosure proposals are not treated as final measurements.

## Remaining Verification and Documentation

- **[TO IMPROVE] Light hand jitter:** small unintended movements still need better rejection. Record these movements, adjust the motion filter if necessary and retest deliberate shaking and flipping together.
- **[TO COMPLETE] Final panel plan:** overall height, overall width and both central rectangles are recorded. Overall depth, the remaining panel dimensions and measured USB/touch opening sizes still need to be added to complete the cutting plan.
- **[TO VERIFY] Physical Vespera mapping:** confirm which physical LED positions correspond to the web-grid numbering and record the actual classroom-light response.
- **[TO ADD] Final demonstration and measured limitations:** add a short real-device video and record timing deviation and remaining gesture-detection limitations. The current acceptance sheet contains pass/fail results rather than these quantitative measurements.

## Future Improvement

Nonblocking Wi-Fi/MQTT reconnection is a future software improvement. The current program can pause normal interaction during reconnection. This change has not been implemented, and it is separate from the completed interaction tests.
