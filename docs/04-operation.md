# 04 — Interaction design

## States
- **WAIT:** waiting for a recognisable face orientation; two dim cyan cells are displayed.
- **FLOWING:** transfer sand according to the confirmed direction.
- **PAUSED:** the latest rendered frame and remaining time stay unchanged. Sensor diagnostics may still be sampled.
- **DONE_BREATHING:** the settled orange pile varies smoothly in brightness; no rapid flashing.

## Events
- A valid opposite-face orientation held for about450ms reverses the flow. Extreme acceleration is not trusted as orientation evidence.
- Side-to-side shake adds a transient displacement effect; it does not reset or change the countdown rate.
- A stable touch press while FLOWING toggles PAUSED. Another press resumes. Release alone does not resume; it rearms the input.
- After resume, animation runs in the existing direction immediately; new orientation is confirmed independently.
- Touch presses in WAIT or DONE do not toggle pause. Flip to begin another transfer after completion.
- Serial `r` restarts; `+/-` changes brightness; `a` selects automatic shake axis; `x/y` selects a manual axis.

## Timing
A full transfer is30 active seconds. Pauses stop progress. Mid-flow reversal preserves sand and therefore changes time-to-empty. Network reconnection currently blocks normal operation and pauses progress; it is not an offline design.

## Display limits
The web grid has72cells. The animation is a stylised particle representation with shared cells, not a mass-accurate physical model. Colour-channel values155 and255 are digital RGB settings, not calibrated luminous intensity.
