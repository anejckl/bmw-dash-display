# Wiring — Direct CAN Bus (MCP2515)

## v1.0: Replaces ELM327 + CP2102 entirely

### MCP2515 → ESP32

| MCP2515 Pin | ESP32 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| CS | GPIO5 |
| SCK | GPIO18 (shared with TFT) |
| SI (MOSI) | GPIO23 (shared with TFT) |
| SO (MISO) | GPIO19 |
| INT | GPIO21 |

```mermaid
graph LR
    subgraph BMW E81
        CAN_H[CAN High\ngreen/white]
        CAN_L[CAN Low\ngreen/yellow]
        FUSE[Fuse Box 12V]
        CHASSIS[Chassis Ground]
    end

    subgraph T-Tap Connectors
        TH[CAN High Tap]
        TL[CAN Low Tap]
    end

    subgraph MCP2515 + TJA1050
        M_CANH[CAN H]
        M_CANL[CAN L]
        M_VCC[VCC]
        M_GND[GND]
        M_CS[CS]
        M_SCK[SCK]
        M_MOSI[SI / MOSI]
        M_MISO[SO / MISO]
        M_INT[INT]
    end

    subgraph MP1584 Buck Converter
        B_IN_P[IN+]
        B_IN_N[IN-]
        B_OUT_P[OUT+ 5V]
        B_OUT_N[OUT-]
    end

    subgraph ESP32
        E_VIN[VIN]
        E_3V3[3.3V]
        E_GND[GND]
        E_5[GPIO5]
        E_18[GPIO18]
        E_23[GPIO23]
        E_19[GPIO19]
        E_21[GPIO21]
    end

    subgraph ILI9488 TFT
        TFT[Display]
    end

    CAN_H --- TH --> M_CANH
    CAN_L --- TL --> M_CANL
    M_VCC --- E_3V3
    M_GND --- E_GND
    M_CS --- E_5
    M_SCK -->|shared| E_18
    M_MOSI -->|shared| E_23
    M_MISO --- E_19
    M_INT --- E_21

    FUSE -->|Add-a-fuse| B_IN_P
    CHASSIS --- B_IN_N
    B_OUT_P -->|5V verified| E_VIN
    B_OUT_N --- E_GND

    E_18 --- TFT
    E_23 --- TFT
```

### CAN bus wire colours in BMW E81

- **CAN High** — green/white wire
- **CAN Low** — green/yellow wire
- **Location:** behind instrument cluster, or along door sill wiring loom

### Power wiring (v1.1 hardwired install)

1. Pull existing mini fuse from E81 glovebox fuse box
2. Insert add-a-fuse — original fuse in top slot, new 5A fuse in bottom slot
3. Red wire from add-a-fuse → MP1584 IN+
4. Black wire from chassis ground → MP1584 IN-
5. Adjust MP1584 trimmer pot until OUT+ reads exactly 5.0V (measure with multimeter)
6. MP1584 OUT+ → ESP32 VIN pin
7. MP1584 OUT- → ESP32 GND

> **WARNING:** Always verify buck converter output voltage with a multimeter BEFORE connecting to ESP32.
