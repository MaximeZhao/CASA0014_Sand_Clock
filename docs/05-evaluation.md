# 05 — Tests, observations and unresolved issues

Do not mark a test passed merely because the sketch compiled or an illustration shows the effect.

| Test | Evidence from development | Status |
|---|---|---|
| MMA7660 three-axis readings | User-posted static, flipped and moving values | Demonstrated |
| Touch ON/OFF and simultaneous acceleration | Two_Sensors_Test logs | Demonstrated |
| MQTT to web grid | Single red pixel screenshot and later animation screenshots | Demonstrated |
| Pause stops time | Logs held LEFT_s at9.8 and4.0 | Demonstrated for the hold-to-freeze stage |
| Tilted release recovery | WAIT_POSE stall logs; code revision and synthetic test | Latest hardware confirmation needed |
| Tap toggles once, no repeated toggle on hold | Synthetic press-edge test and target compilation | Latest hardware confirmation needed |
| Flip vs shake discrimination | User reported partial improvement, later reported false disturbance | Unresolved reliability evaluation |
| Final automatic breathing | Implemented and compiled; logic test | Hardware/web video needed |
| Enclosure touch access and stable sensors | Actual assembly photos, latest fit unconfirmed | In progress |
| Classroom physical light mapping | Lamp photo only; web map established | Not verified |

## Final acceptance sheet (fill after testing)

| Test action | Expected result | Actual result | Pass/fail | Evidence |
|---|---|---|---|---|
| Start face-up | Flow begins | | | |
| Start face-down | Opposite flow begins | | | |
| Flip10times with pauses | Correct direction, minimal false shake | | | |
| Light hand jitter | No disruptive shake animation | | | |
| Deliberate horizontal shake | Obvious disturbance, no timer reset | | | |
| Tap, wait5s, tap again | Frame/time freeze then resume | | | |
| Hold touch3s | One toggle only | | | |
| Full uninterrupted transfer | Approximately30s, then breathing | | | |
| USB through enclosure opening | No cable strain or loose connectors | | | |

Record the firmware commit, Wi-Fi conditions, mounting orientation and how the controller was held. Save short videos or log excerpts, without credentials.
