# OTA Firmware Updates — Setup Notes

Implement OTA when switching to CAN bus (v1.0+). The device will be hardwired in the car with no USB access, so OTA is the only way to push firmware updates to customers.

## Approach: HTTP OTA

ESP32 checks a remote server for new firmware on boot (or on a timer), downloads the `.bin`, flashes itself, and reboots.

```
ESP32 boots → connects to WiFi → GET /firmware/version.txt
    → if server version > current version
        → download /firmware/bmw-dash-display.bin
        → flash to OTA partition
        → reboot into new firmware
    → if no WiFi or server unreachable
        → continue running current firmware (device works offline)
```

## Requirements

### Arduino IDE
- **Partition scheme:** set to "Minimal SPIFFS (1.9MB APP with OTA)" — ESP32 needs two app partitions for safe OTA (current + new, rolls back on failure)

### ESP32 library
- `HTTPUpdate` — built into ESP32 Arduino core, no extra install needed
- `WiFi.h` — also built in

### Firmware server
- Host two files at a public URL:
  - `version.txt` — single line, e.g. `1.0.3`
  - `bmw-dash-display.bin` — compiled firmware binary
- Options: GitHub Releases, AWS S3, own server, even a free static host

### WiFi credentials
- Store SSID + password in ESP32 flash (NVS)
- Initially: hardcode for testing
- Later: configure via companion app or captive portal on first boot

## Implementation checklist

- [ ] Add WiFi connection logic (with timeout — don't block if no WiFi)
- [ ] Add version string constant to firmware (e.g. `#define FW_VERSION "1.0.0"`)
- [ ] Fetch `version.txt` from server and compare against `FW_VERSION`
- [ ] If newer, call `httpUpdate.update()` with the `.bin` URL
- [ ] Handle update result: success (reboot), fail (log error, continue running), no update (continue)
- [ ] Test rollback — if new firmware crashes, ESP32 should boot back to previous partition
- [ ] Add OTA status to a display screen (e.g. "Checking for updates..." on boot)

## Safety considerations

- Device must always work without WiFi — CAN data display is the primary function
- OTA check should be non-blocking and have a short timeout (5-10 seconds)
- Never interrupt an in-progress update — show progress bar on TFT
- Use HTTPS if possible to prevent man-in-the-middle firmware injection
- Consider signing firmware binaries (ESP32 supports secure boot)

## When to implement

After v1.0 (CAN bus working on TFT). OTA only makes sense once the device is permanently installed in the car with no USB access.
