# CASA0014 — Light Sand 

A medium-fidelity Arduino controller for a networked light-hourglass display. Movement and touch become falling light, disturbance, pause/resume and a breathing completion cue. The current prototype retains a breadboard and uses USB power.

## Read or run

| Goal | Entry |
|---|---|
| Run the current device |[SandClock_WideFlow](SandClock_WideFlow/SandClock_WideFlow.ino) and [build guide](Project_Documentation.md#software-setup)|
| Understand the design journey |[12-stage iteration journal](Development%20Iterations.docx)|
| Reproduce the circuit |[Bill of materials](hardware/BOM.md) and [wiring](hardware/wiring.md)|
| Understand the IoT system |[Architecture](Project_Documentation.md#system-architecture)|
| Read the controls |[Interaction rules](Project_Documentation.md#using-the-hourglass)|
| Check what was actually tested |[Evaluation](Project_Documentation.md#evaluation-and-limitations)|
| Build or review the enclosure |[Enclosure](enclosure/README.md)|
| Review sources and AI assistance |[References](Project_Documentation.md#references-and-acknowledgements), [AI usage](AI_USAGE%2004.47.35.md)|

## Prototype under construction

The cardboard and transparent-sheet construction, confirmed measurements and remaining panel details are described in the [enclosure documentation](enclosure/README.md).

## Current behaviour

- Turn over and hold steady: reverse flow while preserving the sand state.
- Shake side to side: disturb light sand without resetting or speeding up the clock.
- Tap once: freeze frame and time. Release rearms the input; tap again to resume.
- Full transfer:30active seconds. Pauses and network reconnection stop progress.
- Falling rainbow colours are capped at155; settled orange reaches red255 at default brightness.
- At completion, the orange pile breathes automatically. Flip to run in reverse.

## Structure

```text
Development Iterations.docx chronological development, problems and improvements
hardware/     parts and actual wiring
enclosure/    construction decisions and measurements
Project_Documentation.md architecture, setup, operation, tests and reflection
```

## Evidence and scope

The latest owner-reported acceptance sheet records eight passed checks, including deliberate horizontal shaking, and one improvement item: light hand jitter. Enclosure measurements are 24.5 cm high and 8.5 cm wide, with a 6 × 5 cm central rectangle on each face. The full panel plan, physical Vespera mapping and final demonstration evidence still need completion. See the [evaluation](Project_Documentation.md#evaluation-and-limitations).

## Configuration

Configure the credential macros required by the main sketch in a local `arduino_secrets.h` beside the `.ino` file. Keep private Wi-Fi and MQTT credential values out of public uploads.

## Development and course readiness

See [Project Documentation](Project_Documentation.md) for setup, controls, reported test results and outstanding verification. [Development Iterations](Development%20Iterations.docx) records the design process. The reflective blog is a separate submission.
