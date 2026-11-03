# CASA0014 — Light Sand 

A medium-fidelity Arduino controller for a networked light-hourglass display. Movement and touch become falling light, disturbance, pause/resume and a breathing completion cue. The current prototype retains a breadboard and uses USB power.

## Read or run

| Goal | Entry |
|---|---|
| Run the current device |[SandClock_WideFlow](SandClock_WideFlow/SandClock_WideFlow.ino/) and [build guide](Project_Documentation.md#software-setup)|
| Understand the design journey |[12-stage iteration journal](Development Iterations.dox)|
| Reproduce the circuit |[Bill of materials](hardware/BOM.md) and [wiring](hardware/wiring.md)|
| Understand the IoT system |[Architecture](Project_Documentation.md#system-architecture)|
| Read the controls |[Interaction rules](Project_Documentation.md#using-the-hourglass)|
| Check what was actually tested |[Evaluation](Project_Documentation.md#evaluation-and-limitations)|
| Build or review the enclosure |[Enclosure](enclosure/README.md)|
| Review sources and AI assistance |[References](Project_Documentation.md#references-and-acknowledgements), [AI usage](AI_USAGE.md)|

## Prototype under construction

![Actual cardboard assembly](media/photos/cardboard-assembly.jpg)

This is an actual in-progress photograph, not a final rendered product or proof of the latest firmware's full acceptance.

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
media/        photos, screenshots, concepts and demo links
Project_Documentation.md architecture, setup, operation, tests and reflection
```

## Evidence and scope

The current sketch compiled for MKR WiFi1010 during development. Sensor logs and web-grid behaviour were demonstrated in earlier stages. The latest full hardware acceptance, final enclosure fit and Vespera physical mapping still require their own recorded tests. See the evaluation table instead of assuming all functions passed.

## Configuration

Real `arduino_secrets.h` files are excluded. Copy the supplied example inside the CURRENT sketch folder and fill it locally. The browser upload workflow requires manual checking too; `.gitignore` does not protect a secret deliberately selected for upload.

## Development and course readiness

See [CHANGELOG](CHANGELOG.md), [readiness checklist](ASSESSMENT_CHECKLIST.md) and [iteration template](templates/iteration-template.md). The reflective blog is a separate submission, not a copy of the build journal.
