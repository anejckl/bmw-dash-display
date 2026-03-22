# OBD2 PID Reference — BMW E81 N47 DDE7.0

## Confirmed Standard PIDs (Mode 01)

| Parameter | PID | Formula | Unit |
|---|---|---|---|
| RPM | 0x0C | ((A*256)+B) / 4 | RPM |
| Speed | 0x0D | A | km/h |
| Coolant temp | 0x05 | A - 40 | °C |
| Intake air temp | 0x0F | A - 40 | °C |
| Boost pressure | 0x0B | ((A*256)+B) * 0.01 | bar |
| Engine load | 0x04 | A * 100/255 | % |
| Throttle position | 0x11 | A * 100/255 | % |
| MAF air flow | 0x10 | ((A*256)+B) / 100 | g/s |
| Rail pressure | 0x23 | ((A*256)+B) * 10 | kPa |
| Ambient air temp | 0x46 | A - 40 | °C |
| Battery voltage | ATRV | parse string | V |

## Verify First (confirm with Launch diagnostic tool before coding)

| Parameter | PID | Formula |
|---|---|---|
| Oil temp | 0x5C | A - 40 °C |
| Torque actual | 0x62 | A - 125 percent max |

## Needs Testing — BMW Mode 22 Extended PIDs

| Parameter | Mode | Status |
|---|---|---|
| DPF soot load | 22 | Verify with Launch tool |
| DPF regen status | 22 | Verify with Launch tool |
| EGR position | 22 | Verify with Launch tool |
| Swirl flap position | 22 | Verify with Launch tool |
| Glow plug status | 22 | Verify with Launch tool |
| Alternator load | 22 | Verify with Launch tool |

## CAN Bus Message IDs — BMW E-series (community confirmed)

| Parameter | CAN ID | Bytes | Formula |
|---|---|---|---|
| RPM | 0x0AA | 4-5 | ((B4 << 8) \| B5) / 4 |
| Speed | 0x0AA | 2-3 | ((B2 << 8) \| B3) / 16 |
| Coolant temp | 0x1D0 | 1 | B1 - 48 |
| Intake temp | 0x1D0 | 2 | B2 - 48 |
| Oil temp | 0x1D0 | 3 | B3 - 48 |
| Boost pressure | 0x130 | 3-4 | ((B3 << 8) \| B4) * 0.01 |
| MAF | 0x130 | 0-1 | ((B0 << 8) \| B1) / 100 |
| Engine load | 0x0AA | 6 | B6 * 100/255 |
| Battery voltage | 0x130 | 7 | B7 * 0.1 |
| Rail pressure | 0x0C0 | 0-1 | ((B0 << 8) \| B1) * 10 |

## CAN Bus Baud Rate

BMW E81 engine bus: **500 kbps**
