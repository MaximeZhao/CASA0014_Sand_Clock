# Development Iterations

The project developed from a gesture-controlled lighting idea into a light hourglass with movement, touch, timed light flow and a breathing completion effect.

## 01 — From a Magic Wand to a Light Hourglass

### Initial idea

The project began with a magic-wand concept: rotating or waving a handheld object would change the classroom lights. I explored how physical movement could become an expressive way to control light.

### Design change

The available components influenced the direction. Rotary encoders could detect a turning knob, but could not detect the movement of the whole object. I considered other interactions, including touch, sound and folding paper, before choosing an hourglass.

### Result

The hourglass provided a clear relationship between an action and its response: turning the object over reverses the flow of light. This became the core interaction, with shaking and touch developed later.

## 02 — Testing a Tilt Switch

### Aim

The first sensor needed to distinguish between the two orientations of the hourglass.

### Implementation

I tested a digital tilt module by reading its output in the Arduino Serial Monitor. A stable change between the two states could be used to reverse the animation. The early hourglass experiments used an 18-second transfer.

### Findings

The switch provided a simple binary input, but could not measure continuous tilt or distinguish the direction and strength of a shake. Rapid changes could also come from contact bounce rather than a deliberate movement.

A pin on the module later broke. This led to replacing the tilt switch with an accelerometer, which could provide more information for the intended interaction.

## 03 — Connecting the MMA7660 Accelerometer

### Aim

I needed to detect both an upside-down orientation and movement from side to side.

### Implementation

I used a Grove three-axis digital accelerometer based on the MMA7660, with a measurement range of approximately ±1.5 g. It communicates with the MKR WiFi 1010 through I²C.

| Sensor connection | MKR WiFi 1010 connection |
|---|---|
| VCC | 3.3 V / VCC |
| GND | GND |
| SDA | D11 / SDA |
| SCL | D12 / SCL |

I installed the Seeed MMA7660 library and ran its example sketch.

### Testing and findings

The first readings were zero on all three axes. After correcting the data connections, the sensor returned changing X, Y and Z values. One stationary reading was approximately −0.10 g, −0.19 g and 1.05 g, consistent with gravity acting mainly along one axis.

Tilting and turning the sensor changed the distribution across the axes. This provided the input needed to develop orientation detection and a visual response to shaking. Strong movements could reach the sensor's measurement limit, so the project uses them to trigger disturbance rather than reconstruct exact motion.

## 04 — MQTT Connection and LED Mapping

### Aim

Before developing the animation, I needed to confirm that the Arduino could send colours to the display and identify where each LED number appeared.

### Implementation

The MKR WiFi 1010 connects to Wi-Fi and publishes RGB frames through MQTT to:

```text
student/CASA0014/luminaire/29
```

The web display contains 72 cells arranged as 12 columns and 6 rows. Its numbering follows:

```text
LED index = column × 6 + row
```

Each frame contains three colour values per LED, giving a total of 216 bytes.

### Testing and findings

A single red LED at index 0 appeared in the top-left corner of the web grid. This gave a reference for translating animation coordinates into LED numbers.

The web grid became the initial development display. Its rectangular layout does not establish how the same LED numbers are positioned on the three-dimensional Vespera lamp; that mapping requires a separate physical check.

## 05 — Building the First Falling-Light Animation

### Aim

The first animation needed to communicate the passage of time through a visible transfer from one end to the other.

### Implementation

The display was divided into an upper reservoir, a falling region and a lower reservoir. As time passed, the upper amount decreased and the lower amount increased. Reversing the object changed the flow direction.

### Findings

The initial falling region was concentrated near the centre of the display, particularly around LEDs 32, 33, 38 and 39. Although the transfer was visible, the narrow stream left much of the display inactive.

The next iteration therefore focused on colour, a wider distribution of falling particles and a clearer contrast between moving and settled light.

## 06 — Adding Colour and a Wider Flow

### Aim

I wanted more of the display to participate in the animation and a clearer visual difference between falling and settled sand.

### Changes

- Falling particles use a changing rainbow palette.
- Settled particles become orange.
- Falling paths extend across more columns instead of remaining in the central four cells.
- The animation uses 30 simulated grains, with a flight window of approximately 12 grains.
- A full transfer takes 30 seconds of active time.
- Falling colour values are capped at 155. At the default master brightness, the red channel of settled orange reaches 255.

### Result

The wider flow gives the display more movement, while the orange accumulation shows where the transferred sand has settled. The colour change connects a particle's appearance to its state.

The animation is a stylised representation. Several simulated grains can overlap in one cell, so the number of illuminated cells is not necessarily equal to the number of grains.

## 07 — Separating Shaking from Flipping

### Aim

Side-to-side shaking should disturb the light, while a normal flip should reverse the flow without producing the same disturbance effect.

### Problem

The first response was too sensitive: small movements triggered visible changes. Increasing the restrictions then made shaking less noticeable and sometimes prevented a flip from being recognised. Turning the object over could also be mistaken for shaking.

### Changes

I revised the movement detection using amplitude thresholds, back-and-forth movement checks and orientation guards. Flow reversal requires a confirmed opposite orientation rather than a single changing sample. Samples near the accelerometer's measurement limit are not sufficient on their own to confirm a flip.

### Interaction rule

Shaking changes the visual disturbance only. It does not reset the timer or make the countdown run faster. A flip changes the flow direction while preserving the amount of sand already transferred.

### Findings

Serial logs helped identify cases where the shake gate remained blocked. They also showed readings close to ±1.5 g during strong movements. These observations informed the revisions, but the sensor's limited range and the way the object is held still affect gesture recognition.

## 08 — Adding a Touch Sensor

### Aim

I added a separate intentional input so that the user could control time without relying on another movement gesture.

### Implementation

I connected a Grove Touch v1.1 module alongside the accelerometer:

| Touch connection | MKR WiFi 1010 connection |
|---|---|
| VCC | 3.3 V / VCC |
| GND | GND |
| SIG | D2 |
| NC | Unconnected |

The two modules share the power and ground connections through the breadboard. The accelerometer continues to use SDA and SCL, while touch uses a separate digital input.

### Testing and findings

I tested the two sensors together before integrating the touch interaction into the animation. The Serial Monitor showed acceleration values updating alongside `TOUCH_RAW` and the filtered touch state.

Touching the pad produced `TOUCH EVENT: ON`; releasing it produced `TOUCH EVENT: OFF`. This confirmed that the two inputs could be read together. The touch state indicates contact with the pad; the main application then interprets that contact as a pause or resume command.

## 09 — Freezing the Frame and Countdown

### Design change

Touch was initially considered as a way to make settled sand breathe. I reassigned breathing to the automatic completion state and used touch to freeze time during the transfer.

### Implementation

In the first freeze interaction, holding the touch pad paused both the animation frame and the remaining time. Releasing the pad was intended to resume the transfer.

### Problem

The resume logic included a requirement to wait for a suitable orientation. When the object was tilted, the program could remain in `WAIT_POSE`, making the response feel delayed or unresponsive.

### Revision

I removed the mandatory pose wait. Resuming continues in the existing direction, while a new flip is handled through its own orientation confirmation. This separates the touch action from the movement decision.

The next iteration changed the contact gesture itself, replacing continuous holding with a tap-to-toggle interaction.

## 10 — Tap Once to Pause, Tap Again to Resume

### Aim

Holding the pad throughout a pause was inconvenient. I wanted a short touch to leave the hourglass paused after the finger was removed.

### Implementation

Touch now acts as a toggle during an active transfer:

1. Tap once to freeze the animation and countdown.
2. Release the pad to rearm the input.
3. Tap again to resume from the frozen state.

The program filters short input changes and responds to a new press event. Keeping a finger on the pad generates only one toggle, preventing repeated pause/resume changes during a single hold.

### Result and testing

The revised firmware compiled for the MKR WiFi 1010, and a software test of the press-event logic confirmed one toggle per press. Final touch response in the assembled enclosure still needs to be checked with the actual pad position and wiring.

At the end of a transfer, the settled orange light breathes automatically. Touch is used for pausing an active transfer rather than switching the completion effect on and off.

## 11 — Choosing USB Power for the Prototype

### Aim

I explored battery power to make the object easier to carry and turn over without a computer connection.

### Trial

The available 3.7 V batteries had connectors that did not fit the board's battery socket. Suitable replacement connectors and tools were not available for completing a reliable connection.

### Decision

I retained USB power and kept the MKR WiFi 1010 mounted on the breadboard. The prototype sends its lighting commands over Wi-Fi, but still requires a USB power cable.

### Design consequence

The enclosure must provide access to the USB port and enough space for the plug and cable. Cable routing became part of the physical design because it affects how comfortably the object can be held and flipped.

## 12 — Developing the Enclosure Around the Hardware

### Aim

The enclosure needed to support the hourglass interaction while accommodating the existing electronics and USB supply.

### Construction

I used cardboard and transparent sheet material to explore the form. The MKR WiFi 1010 remains attached to an 82 × 55 mm breadboard. The accelerometer and touch module connect to it through jumper and Grove cables.

I explored a sloped internal support to reduce the space occupied by the board assembly. Later, I kept the existing base and considered closing two side panels over it to form a triangular cross-section.

### Fit and access problems

Earlier side panels did not close around the revised assembly. The shell therefore needed to be fitted around the actual board, connectors and wire loops, rather than around the board dimensions alone.

A downward-facing USB connection also interfered with placing the object on its end. I considered turning the complete breadboard assembly by 90 degrees so that the USB cable could leave through a side opening. Moving the main board does not require moving the separately mounted accelerometer.

### Next physical checks

The assembled shell needs checks for panel closure, cable clearance, secure sensor mounting and a comfortable grip. The final sensor position must remain fixed relative to the shell so that movement detection stays consistent.
