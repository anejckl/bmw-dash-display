// ============================================================
// BMW Dash Display — TFT Display Test (no OBD needed)
// Hardware: ESP32 DevKit + 3.2" ILI9488 TFT (SPI)
// Wiring:   VCC→3.3V  GND→GND  SCK→GPIO18  MOSI→GPIO23
//           DC→GPIO2  RESET→GPIO4  CS→GPIO15
// Libraries: TFT_eSPI (configure User_Setup.h for ILI9488)
// Status:   Waiting for display
// ============================================================

#include <SPI.h>
#include <TFT_eSPI.h>

void setup() {
  // TODO: Initialize TFT display
  // TODO: Configure TFT_eSPI User_Setup.h for ILI9488 + ESP32 SPI pins
  // TODO: Display splash screen with colour graphics
}

void loop() {
  // TODO: Generate fake car data (reuse animation logic from v0.1)
  // TODO: Draw full-colour screens with gauges and bar graphs
  // TODO: Auto-rotate screens
}
