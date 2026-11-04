# Evaluation and Next Steps

## What I Learned

A binary tilt switch was enough to start exploring reversal, but it could not provide the continuous readings needed for shaking. The MMA7660 gave me more information, although its approximately ±1.5 g range limited what I could infer from vigorous movement.

Tuning the interaction was a balance. Increasing the movement threshold reduced unwanted effects but could also make deliberate shaking harder to detect. The current acceptance results mark flipping and deliberate shaking as passed, while light hand jitter still needs improvement.

Testing the sensors separately helped distinguish wiring and input problems from animation and networking problems. Changing touch from holding to tapping also required changing the state logic, so one touch would produce one pause or resume action.

Making the enclosure showed why I needed to work with the actual connected hardware. The breadboard, connectors, cable bends and USB plug all affected the shape and assembly.

## Completed and Documented

- Recorded the combined acceptance results: eight checks passed; light hand jitter remains an improvement item.
- Confirmed deliberate horizontal shaking, repeated flipping, tap pause/resume, a single toggle on hold, the approximately 30-second transfer and the breathing completion effect in the acceptance sheet.
- Recorded enclosure height as 24.5 cm and width as 8.5 cm.
- Confirmed the front and back central rectangles as 6 cm wide × 5 cm high each.
- Recorded the retained 8.2 × 5.5 cm breadboard and USB-powered construction.

## Remaining Work

- **[TO IMPROVE] Light hand jitter:** small unintended movements still need better rejection. Record these movements, adjust the motion filter if necessary and retest deliberate shaking and flipping together.

- **[TO ADD] Final demonstration and measured limitations:** add a short real-device video and record timing deviation and remaining gesture-detection limitations. The current acceptance sheet contains pass/fail results rather than these quantitative measurements.

## Future Software Improvement

Nonblocking Wi-Fi/MQTT reconnection is a future software improvement. The current program can pause normal interaction during reconnection. This change has not been implemented, and it is separate from the completed interaction tests.
