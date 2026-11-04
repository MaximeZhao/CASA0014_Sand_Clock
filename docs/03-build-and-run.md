# Build and run

1. Use the [BOM](../hardware/BOM.md) and [current wiring](../hardware/wiring.md).
2. In Arduino IDE, install **Arduino SAMD Boards** and select **Arduino MKR WiFi1010**, plus the correct USB serial port.
3. Install **WiFiNINA**, **PubSubClient**, and Seeed Studio's **Accelerometer_MMA7660** library.
4. First run `sand_clock/Two_Sensors_Test/Two_Sensors_Test.ino` with Serial Monitor at 115200 baud. Check accelerometer readings and touch ON/OFF.
5. Open `SandClock_WideFlow/SandClock_WideFlow.ino` as a separate sketch.
6. Copy `SandClock_WideFlow/arduino_secrets.example.h` to `arduino_secrets.h` in that SAME folder, enter your local network and MQTT credentials, and do not upload that file to GitHub.
7. Check the assigned MQTT topic; the development snapshot uses channel29. Connect to a compatible2.4GHz Wi-Fi network.
8. Upload, select Serial Monitor at 115200 baud, wait for Wi-Fi/MQTT connection, select the matching receiver channel, and hold a face steady to start.
9. Test on the actual classroom light as well as the web simulator. A simulator test is not proof of physical mapping.

The main application is in `SandClock_WideFlow`; older sketches and diagnostics are indexed in [sand_clock](../sand_clock/README.md).

## Troubleshooting
- No USB port: disconnect power, remove external wiring, test bare board with known data cable, then restore circuits in stages.
- All-zero/absent MMA7660: check supply, SDA/D11 and SCL/D12; do not confuse D1/D2 with D11/D12.
- Redefinition errors: don't place two full sketches in one Arduino sketch folder.
- Missing secrets header: create the local file next to the current `.ino`.
- Web unchanged: check both Wi-Fi/MQTT logs and matching channel; don't infer a sensor fault from a network failure.
