# Wiring — OLED Display

## v0.1: OLED Fake Data (no OBD needed)

### 0.96" SSD1306 OLED (LAFVIN kit) → ESP32

Board pin order: GND VCC SCL SDA (left to right)

| OLED Pin | ESP32 Pin | Wire colour |
|---|---|---|
| GND | GND | Black |
| VCC | 3.3V | Red |
| SCL | GPIO22 | Orange |
| SDA | GPIO21 | Blue |

**Power source:** Laptop USB → ESP32 USB port. Powers everything. No external power needed.

```mermaid
graph LR
    subgraph Power
        USB[Laptop USB]
    end

    subgraph ESP32
        E_USB[USB Port]
        E_3V3[3.3V]
        E_GND[GND]
        E_21[GPIO21]
        E_22[GPIO22]
    end

    subgraph SSD1306 OLED
        O_VCC[VCC]
        O_GND[GND]
        O_SCL[SCL]
        O_SDA[SDA]
    end

    USB -->|USB cable| E_USB
    E_3V3 -->|Red| O_VCC
    E_GND -->|Black| O_GND
    E_22 -->|Orange| O_SCL
    E_21 -->|Blue| O_SDA
```

> **WARNING:** Always check labels on YOUR board before wiring — pin order varies by manufacturer.

---

## v0.2: OLED + Real OBD Data (ELM327 + CP2102)

### Full connection chain

| CP2102 Pin | ESP32 Pin | Notes |
|---|---|---|
| TXD | GPIO16 (RX2) | TX goes to RX — correct, not a mistake |
| RXD | GPIO17 (TX2) | RX goes to TX — correct, not a mistake |
| VCC | 3.3V | Power |
| GND | GND | Ground |

### OLED stays exactly the same as v0.1

```mermaid
graph LR
    subgraph BMW
        OBD[OBD Port]
    end

    subgraph ELM327
        E_OBD[OBD Plug]
        E_USB[USB-A Plug]
    end

    subgraph Adapter
        FF[USB F-F Adapter]
    end

    subgraph CP2102
        C_USB[USB Port]
        C_TXD[TXD]
        C_RXD[RXD]
        C_VCC[VCC]
        C_GND[GND]
    end

    subgraph ESP32
        ES_USB[USB Port]
        ES_3V3[3.3V]
        ES_GND[GND]
        ES_16[GPIO16 RX2]
        ES_17[GPIO17 TX2]
        ES_21[GPIO21]
        ES_22[GPIO22]
    end

    subgraph SSD1306 OLED
        O_VCC[VCC]
        O_GND[GND]
        O_SCL[SCL]
        O_SDA[SDA]
    end

    subgraph Power
        USB[Laptop USB]
    end

    OBD --- E_OBD
    E_USB --- FF
    FF --- C_USB
    C_TXD -->|TX to RX| ES_16
    C_RXD -->|RX to TX| ES_17
    C_VCC --- ES_3V3
    C_GND --- ES_GND
    USB -->|USB cable| ES_USB
    ES_3V3 -->|Red| O_VCC
    ES_GND -->|Black| O_GND
    ES_22 -->|Orange| O_SCL
    ES_21 -->|Blue| O_SDA
```

### ELM327 USB cable
- OBD plug → BMW OBD port (under dash, driver side)
- USB-A plug → USB female-female adapter → CP2102 USB port

> **WARNING:** ELM327 must be the USB/UART version — NOT Bluetooth, NOT WiFi.
