// ============================================================
// BMW Dash Display — TFT + Real OBD2 Data
// Hardware: ESP32 DevKit + 3.2" ILI9488 TFT + Bluetooth ELM327
// Wiring:   TFT VCC→3.3V  GND→GND  SCK→GPIO18  MOSI→GPIO23
//           DC→GPIO2  RESET→GPIO4  CS→GPIO15
//           ELM327 dongle plugs into car's OBD2 port — no wires
//           to the ESP32; pairs via built-in Bluetooth Classic.
// Libraries: TFT_eSPI, BluetoothSerial, ELMduino
// Status:   Pending — combines v0.3 TFT UI with v0.2 BT data layer
// ============================================================

#include <SPI.h>
#include <TFT_eSPI.h>
#include <BluetoothSerial.h>
#include <ELMduino.h>

void setup() {
  // TODO: Initialize TFT_eSPI display
  // TODO: SerialBT.begin("BMW_DASH", true); connect to ELM327 by name
  // TODO: myELM327.begin(SerialBT, false, 2000)
  // TODO: Combine v0.3 display layer with v0.2 Bluetooth data layer
  // TODO: Display splash screen while pairing
}

void loop() {
  // TODO: Poll OBD2 values via ELMduino over Bluetooth (non-blocking)
  // TODO: Update CarData struct with real values
  // TODO: Draw full-colour TFT screens on 3.2" ILI9488
}
