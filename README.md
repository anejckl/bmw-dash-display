# BMW Dash Display

Custom BMW engine data display built on ESP32. Reads data via OBD2 (ELM327) and transitions to direct CAN bus communication. Designed as a plug-and-play product for BMW owners — specifically targeting pre-2008 E-series BMWs with N47 diesel engines.

**Target vehicle:** BMW E81 118d (N47 diesel, DDE7.0 ECU, 2007)
**Target market:** BMW E-series owners (E81, E87, E90, E91, E92)
**Final product:** Flush-mounted TFT display in dashboard air vent, hardwired to CAN bus

## Development Roadmap

| Version | Description | Status |
|---|---|---|
| v0.1 | Fake animated data on 0.96" OLED | Done |
| v0.2 | Real OBD2 data via ELM327 on 0.96" OLED | Waiting for parts |
| v0.3 | Full UI on 3.2" ILI9488 TFT with fake data | Waiting for display |
| v0.4 | Real OBD2 data on ILI9488 TFT | Pending |
| v1.0 | Switch to direct CAN bus via MCP2515 | Pending |
| v1.1 | Hardwired install in car (CAN tap + fuse box power) | Pending |
| v2.0 | OTA firmware updates, companion app | Future |

## Repository Structure

```
bmw-dash-display/
├── sketches/
│   ├── v0.1_oled_fake_data/       ← ESP32 + OLED, fake animated data
│   ├── v0.2_oled_real_obd/        ← ESP32 + OLED + ELM327, live OBD2
│   ├── v0.3_tft_fake_data/        ← ESP32 + ILI9488 TFT, fake data
│   └── v0.4_tft_real_obd/         ← ESP32 + TFT + ELM327, live OBD2
├── docs/
│   ├── pid_list.md                ← OBD2 PIDs and CAN message IDs
│   ├── wiring_oled.md             ← SSD1306 OLED wiring (v0.1, v0.2)
│   ├── wiring_tft.md              ← ILI9488 TFT wiring (v0.3, v0.4)
│   └── wiring_can.md              ← MCP2515 CAN bus wiring (v1.0+)
└── README.md
```

## Hardware

### Core Components

| Part | Model | Purpose |
|---|---|---|
| Microcontroller | LAFVIN ESP32 DevKit v1 | Brain of the device |
| Main display | 3.2" SPI TFT ILI9488 with touch | Final product display |
| OBD interface | ELM327 USB cable (OBD2 to USB-A) | Reads OBD2 data from BMW |
| UART bridge | CP2102 USB to TTL UART | Connects ELM327 to ESP32 |

### CAN Bus Components (v1.0+)

| Part | Model | Purpose |
|---|---|---|
| CAN controller | MCP2515 + TJA1050 module | Direct CAN bus reading |
| Wire tap | T-tap connectors | Tap CAN High/Low wires behind dash |
| Power | MP1584EN 5V buck converter | Steps 12V car power to 5V |
| Fuse tap | Mini add-a-fuse adapter | Taps switched 12V from E81 fuse box |

## Code Architecture

All sketches use strict data/display layer separation:

```
DISPLAY LAYER — never changes between phases
  screens, UI, alerts, bar graphs, colours
        |
DATA LAYER — only this changes when switching to CAN
  getRPM() getCoolant() getBoost() etc.
        |
        |--- ELM327 via ELMduino (v0.2 to v0.4)
        |--- MCP2515 direct CAN (v1.0 onwards)
```

When switching from ELM327 to CAN — only the data functions change. Display code, warnings, and UI remain completely untouched.

## Documentation

- [PID Reference](docs/pid_list.md) — Standard PIDs, extended PIDs, CAN message IDs
- [OLED Wiring](docs/wiring_oled.md) — SSD1306 display connections (v0.1, v0.2)
- [TFT Wiring](docs/wiring_tft.md) — ILI9488 display connections (v0.3, v0.4)
- [CAN Bus Wiring](docs/wiring_can.md) — MCP2515 and power wiring (v1.0+)
- [OTA Updates](docs/ota_updates.md) — HTTP OTA firmware update setup (v2.0)
