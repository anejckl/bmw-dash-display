// ============================================================
// BMW Dash Display — OLED + Real OBD2 Data
// Hardware: ESP32 DevKit + 0.96" SSD1306 OLED + ELM327 USB + CP2102
// Wiring:   OLED VCC→3.3V  GND→GND  SDA→GPIO21  SCL→GPIO22
//           CP2102 TXD→GPIO16(RX2)  RXD→GPIO17(TX2)  VCC→3.3V  GND→GND
// Libraries: Adafruit SSD1306, Adafruit GFX, ELMduino
// Status:   Waiting for parts
// ============================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ELMduino.h>

void setup() {
  // TODO: Initialize Serial, OLED display, and ELM327 UART connection
  // TODO: Send AT commands to initialize ELM327
  // TODO: Display splash screen
}

void loop() {
  // TODO: Read live OBD2 data via ELM327
  // TODO: Update CarData struct with real values
  // TODO: Draw screens (reuse display layer from v0.1)
}
