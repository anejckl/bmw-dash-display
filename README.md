# BMW Dash Display

**Custom engine-data display for pre-2008 BMW E-series, built on ESP32.**
**Live OBD2 today, direct CAN bus tomorrow, flush-mounted in the dash where the air vent used to be.**

[![Live site](https://img.shields.io/badge/esp.anej.dev-live-b48820?style=flat-square)](https://esp.anej.dev)
[![Platform](https://img.shields.io/badge/platform-ESP32-00a6c2?style=flat-square)](https://www.espressif.com/en/products/socs/esp32)
[![Display](https://img.shields.io/badge/display-ILI9488-a855f7?style=flat-square)](https://esp.anej.dev/docs/wiring-tft/)
[![Bus](https://img.shields.io/badge/bus-CAN%20%2F%20OBD2-58c060?style=flat-square)](https://esp.anej.dev/docs/wiring-can/)
[![Phase](https://img.shields.io/badge/phase-v0.2-fbbf24?style=flat-square)](#development-roadmap)

> ESP32 reads engine telemetry from the car — over OBD2 today, directly off the CAN bus tomorrow — and renders it on a flush-mounted TFT where the air-vent used to be. Fully open hardware, fully documented, built one version at a time.

| | |
|---|---|
| **Target vehicle** | BMW E81 118d (N47 diesel, DDE7.0 ECU, 2007) |
| **Target market** | Owners of pre-2008 E-series BMWs — E81, E87, E90, E91, E92 |
| **Final form** | Flush-mounted 3.2" TFT in the dashboard air vent, hardwired to the CAN bus |
| **Project site** | [**esp.anej.dev**](https://esp.anej.dev) |

## Explore

- **[esp.anej.dev](https://esp.anej.dev)** — the full project site: docs, sketches with source, roadmap
- **[esp.anej.dev/animation](https://esp.anej.dev/animation)** — full-screen interactive 3D view of the wiring
- **[esp.anej.dev/roadmap](https://esp.anej.dev/roadmap)** — phase-by-phase development timeline
- **[Docs](https://esp.anej.dev/docs/)** with rendered Mermaid diagrams · **[Sketches](https://esp.anej.dev/sketches/)** with syntax-highlighted source

## Development roadmap

| Version | Description | Status |
|---|---|---|
| **v0.1** | Fake animated data on 0.96" OLED | Done |
| **v0.2** | Real OBD2 data via Bluetooth ELM327 on OLED | Waiting on BT dongle |
| **v0.3** | Full UI on 3.2" ILI9488 TFT with fake data | Waiting on display |
| **v0.4** | Real OBD2 data (Bluetooth ELM327) on ILI9488 TFT | Pending |
| **v1.0** | Switch to direct CAN bus via MCP2515 | Pending |
| **v1.1** | Hardwired install in car (CAN tap + fuse-box power) | Pending |
| **v2.0** | OTA firmware updates + companion app | Future |

Each phase swaps exactly one thing from the previous — **display** or **data source** — so neither change has to touch both sides of the code at once.

## Hardware

### Core components

| Part | Model | Purpose |
|---|---|---|
| Microcontroller | LAFVIN ESP32 DevKit v1 | Brain of the device — built-in Bluetooth Classic |
| Main display | 3.2" SPI TFT ILI9488 w/ touch | Final product display |
| OBD interface | **Bluetooth ELM327 dongle** (Bluetooth Classic / SPP) | Reads OBD2 data from the BMW wirelessly |

> [!WARNING]
> **Don't buy a USB-style ELM327 cable or a CP2102 USB-UART bridge for this project.**
> The ESP32 dev board cannot act as a USB host — USB ELM327 cables don't work, and a female-to-female USB adapter won't make them work either.
> Use a **Bluetooth ELM327** dongle instead. It plugs straight into the OBD2 port and pairs with the ESP32 over Bluetooth Classic SPP — no soldering, no UART tap, no voltage divider.

### CAN bus components (v1.0 onwards)

| Part | Model | Purpose |
|---|---|---|
| CAN controller | MCP2515 + TJA1050 module | Direct CAN bus reading |
| Wire taps | T-tap connectors | Tap CAN-H / CAN-L behind the dash |
| Power | MP1584EN 5V buck converter | Drops 12V car power to 5V |
| Fuse tap | Mini add-a-fuse adapter | Taps switched 12V from the E81 glovebox fuse box |

## Code architecture

Every sketch uses strict **data / display layer** separation:

```
┌─────────────────────────────────────────────────────────┐
│  DISPLAY LAYER  — never changes between phases          │
│  Gauges, screens, alerts, bar graphs, colours           │
└───────────────────────────┬─────────────────────────────┘
                            │
┌───────────────────────────┴─────────────────────────────┐
│  DATA LAYER  — only this changes when swapping sources  │
│  getRPM()   getCoolant()   getBoost()   ...             │
└──┬───────────────────────────────────────────────────┬──┘
   │                                                   │
   ├─ ELMduino over BluetoothSerial   (v0.2 – v0.4)    │
   └─ MCP2515 direct CAN              (v1.0 onwards)  ─┘
```

When the data source swaps from Bluetooth ELM327 to direct CAN, only the data functions change. The display code, warning logic, and UI stay completely untouched.

## Repository layout

```
bmw-dash-display/
├── sketches/
│   ├── v0.1_oled_fake_data/    ESP32 + OLED, fake animated data
│   ├── v0.2_oled_real_obd/     ESP32 + OLED + Bluetooth ELM327
│   ├── v0.3_tft_fake_data/     ESP32 + ILI9488 TFT, fake data
│   └── v0.4_tft_real_obd/      ESP32 + TFT + Bluetooth ELM327
├── docs/
│   ├── pid_list.md             OBD2 PIDs + CAN message IDs
│   ├── wiring_oled.md          SSD1306 wiring + BT pairing
│   ├── wiring_tft.md           ILI9488 SPI pins + TFT_eSPI config
│   ├── wiring_can.md           MCP2515 + T-tap + buck converter
│   └── ota_updates.md          HTTP OTA firmware update plan
└── README.md
```

## Documentation

| Doc | Covers | Rendered |
|---|---|---|
| [PID Reference](docs/pid_list.md) | Standard Mode-01 PIDs, Mode-22 extended PIDs, CAN message IDs, 500 kbps baud | [on the site](https://esp.anej.dev/docs/pid-list/) |
| [OLED Wiring](docs/wiring_oled.md) | SSD1306 I²C wiring + Bluetooth ELM327 pairing | [on the site](https://esp.anej.dev/docs/wiring-oled/) |
| [TFT Wiring](docs/wiring_tft.md) | ILI9488 SPI pin map + library setup | [on the site](https://esp.anej.dev/docs/wiring-tft/) |
| [CAN Bus Wiring](docs/wiring_can.md) | MCP2515 + TJA1050 + T-tap + buck converter + fuse-box power | [on the site](https://esp.anej.dev/docs/wiring-can/) |
| [OTA Updates](docs/ota_updates.md) | HTTP OTA firmware update architecture for v2.0 | [on the site](https://esp.anej.dev/docs/ota-updates/) |

The rendered versions on [esp.anej.dev](https://esp.anej.dev) have live Mermaid diagrams and syntax-highlighted sketch source.

## Disclaimer

This project involves tapping into a vehicle's electrical system — the OBD2 port in early phases, the CAN bus and fuse box directly in later phases. **Always measure voltages with a multimeter before connecting anything**, never reverse polarity, and use correctly rated fuses. Modifications to your car are your responsibility. No warranty is expressed or implied.
