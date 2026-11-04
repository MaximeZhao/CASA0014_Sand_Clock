# Earlier Sketches and Sensor Tests

This folder contains the saved early programs and diagnostics for Light Sand. The integrated application remains in [SandClock_WideFlow](../SandClock_WideFlow/SandClock_WideFlow.ino).

| Sketch | Purpose | Serial Monitor |
|---|---|---|
| [sketch_oct26b](sketch_oct26b/sketch_oct26b.ino) | Earlier Wi-Fi/MQTT RGB-frame experiment | 115200 baud |
| [Tilt_Module_Test](Tilt_Module_Test/Tilt_Module_Test.ino) | Digital tilt input and debounce | 115200 baud |
| [Light_Hourglass_Test](Light_Hourglass_Test/Light_Hourglass_Test.ino) | Early 18-second reversible transfer using a digital tilt module | 115200 baud |
| [Hourglass_Test](Hourglass_Test/Hourglass_Test.ino) | Earlier two-contact tilt-switch experiment, with `Hourglass.h` | 115200 baud |
| [MMA7660FC_Demo](MMA7660FC_Demo/MMA7660FC_Demo.ino) | Seeed three-axis accelerometer example | 9600 baud |
| [Vespera_Mapping_Test](Vespera_Mapping_Test/Vespera_Mapping_Test.ino) | Saved 18-second accelerometer/MQTT hourglass program, previously also named SandClock_Ready | 115200 baud |
| [SandClock_Colorful](SandClock_Colorful/SandClock_Colorful.ino) | Earlier 18-second colour-flow version | 115200 baud |
| [Two_Sensors_Test](Two_Sensors_Test/Two_Sensors_Test.ino) | Offline MMA7660 and Grove Touch diagnostic | 115200 baud |

## Opening a Sketch

Open one `.ino` file at a time in Arduino IDE and select Arduino MKR WiFi 1010. Each main `.ino` filename matches its enclosing folder. The Hourglass_Test program also requires its included `Hourglass.h` file.

For a networked sketch, copy its `arduino_secrets.example.h` to `arduino_secrets.h` in the same folder and enter the local Wi-Fi/MQTT credentials. The real configuration is ignored by Git. Tilt and sensor-only tests do not need Wi-Fi.

These programs belong to different development stages and do not all use the final wiring. Check each sketch before reconnecting hardware. In particular, the two-contact Hourglass_Test uses a switch between D2 and GND; the three-pin tilt-module tests use the module's output on D2. Disconnect the current touch signal from D2 before using a tilt sketch on that pin.

## Duplicate Programs

The saved SandClock_Ready code is identical to the early program in Vespera_Mapping_Test and is represented there once. The shorter Tilt_Module_Test copy differs only in comments and formatting from the retained version. The standalone duplicate folders at the repository root were removed.

Vespera_Mapping_Test is a retained development folder name: the saved program inside it is an early hourglass animation, not a separate full physical-lamp mapping test. Its main filename was aligned with the folder name for Arduino IDE.

## Build Check

The restored Two_Sensors_Test, SandClock_Colorful, Hourglass_Test and sketch_oct26b programs, plus the renamed Vespera_Mapping_Test main file, compiled for `arduino:samd:mkrwifi1010` on 4 November 2026. Networked build checks used placeholder credentials in temporary folders.
