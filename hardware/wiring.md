# Current wiring

| Sensor | Pin | MKR WiFi1010 |
|---|---|---|
| MMA7660 |VCC|3.3V/VCC|
| MMA7660 |GND|GND|
| MMA7660 |SDA|D11|
| MMA7660 |SCL|D12|
| Grove Touch |VCC|3.3V/VCC|
| Grove Touch |GND|GND|
| Grove Touch |SIG|D2|
| Grove Touch |NC|Unconnected|

The sensors share separate3.3V and ground rails. Check split rails and printed labels. Disconnect USB before moving wires. The existing MKR/breadboard assembly is retained. Keep the independently fixed accelerometer in the orientation tested by the code.

Use the [offline diagnostic](../firmware/diagnostics/Two_Sensors_Test/) to isolate hardware inputs from networking. Expected MMA7660 I2C address is0x4C. Physical touch ON/OFF is converted into a pause toggle by the current main sketch.
