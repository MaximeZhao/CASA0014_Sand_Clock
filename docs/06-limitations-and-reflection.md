# 07 — Evaluation and next steps

## Lessons from iteration
- A binary tilt switch was enough for basic reversal but could not provide continuous motion data.
- More visual effects did not automatically improve interaction: movement thresholds and pose guards could make the object either too sensitive or unresponsive.
- The low-range accelerometer saturates during vigorous motion; extreme readings should not be treated as precise orientation.
- An offline sensor test helped isolate hardware input from Wi-Fi/MQTT behaviour.
- Touching continuously, tapping once and changing completion effects are different interaction rules; the state logic must be updated consistently.
- Fabrication must fit real connectors and a retained breadboard, not only a rendered concept.

## Remaining work
- Complete and document the final combined hardware acceptance tests.
- Measure the built enclosure and document its final panel plan.
- Improve the motion classifier using recorded gesture sequences if false triggers remain.
- Consider nonblocking networking so input feedback is less dependent on reconnection delays.
- Verify the mapping and behaviour of the actual classroom lamp.
- Add a short final demo video and measured limitations.

## Personal reflection to write
Explain which design decisions you made, what you learned while wiring/debugging, how user feedback changed the interaction, and what you would change with more time. This section should be written in the project owner's own voice, supported by evidence.
