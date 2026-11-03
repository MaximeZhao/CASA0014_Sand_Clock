# System architecture

```mermaid
flowchart LR
  A[MMA7660: I2C motion data] --> C[MKR WiFi1010: interaction, clock, renderer]
  B[Grove Touch: digital input] --> C
  C -->|Wi-Fi / MQTT publish| D[MQTT broker]
  D --> E[12×6 web simulator]
  D --> F[Vespera light: channel and physical mapping must be checked]
```

The main sketch creates a216-byte RGB frame for72LEDs and publishes on the configured luminaire topic. The web grid indexes cells as `column*6+row`. Wi-Fi credentials and MQTT credentials are separate local configuration values.

The physical controller does not contain a sand display. The receiver visualises its output. A successful publish call alone does not prove broker receipt; include actual receive evidence. Reconnection currently blocks normal operation and pauses progress.
