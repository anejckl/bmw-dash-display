// ============================================================
// BMW Dash Display — TFT + Real OBD2 Data
// Hardware: ESP32 DevKit + 3.2" ILI9488 TFT + ELM327 USB + CP2102
// Wiring:   TFT VCC→3.3V  GND→GND  SCK→GPIO18  MOSI→GPIO23
//           DC→GPIO2  RESET→GPIO4  CS→GPIO15
//           CP2102 TXD→GPIO16(RX2)  RXD→GPIO17(TX2)  VCC→3.3V  GND→GND
// Libraries: TFT_eSPI, ELMduino
// Status:   Pending
// ============================================================

#include <SPI.h>
#include <TFT_eSPI.h>
#include <ELMduino.h>

void setup() {
  // TODO: Initialize TFT display and ELM327 UART connection
  // TODO: Combine v0.3 display layer with v0.2 data layer
  // TODO: Display splash screen
}

void loop() {
  // TODO: Read live OBD2 data via ELM327
  // TODO: Update CarData struct with real values
  // TODO: Draw full-colour TFT screens
}
