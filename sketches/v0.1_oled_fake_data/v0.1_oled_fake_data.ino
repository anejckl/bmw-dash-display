// ============================================================
// BMW Dash Display — DISPLAY TEST (no OBD needed)
// Hardware: ESP32 DevKit + 0.96" SSD1306 OLED
// Wiring:   OLED VCC→3.3V  GND→GND  SDA→GPIO21  SCL→GPIO22
// Libraries: Adafruit SSD1306, Adafruit GFX
// ============================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ── Display config ──────────────────────────────────────────
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ── Fake car data ───────────────────────────────────────────
struct CarData {
  float rpm     = 800;
  float speed   = 0;
  float coolant = 20;
  float boost   = -0.1;
  float intake  = 18;
  float load    = 5;
  float battery = 12.4;
  float oil     = 40;
};
CarData car;

// ── Animation state ─────────────────────────────────────────
float rpmTarget    = 800;
float speedTarget  = 0;
bool  engineWarmup = true;
int   animPhase    = 0;
unsigned long lastAnimUpdate = 0;
unsigned long lastPhaseChange = 0;

// ── Screen management ────────────────────────────────────────
int currentScreen = 0;
const int NUM_SCREENS = 4;
unsigned long lastScreenChange = 0;
const int SCREEN_INTERVAL = 5000;

// ============================================================
// ANIMATION — simulates realistic engine behaviour
// ============================================================

void updateFakeData() {
  unsigned long now = millis();

  // Change driving phase every 6 seconds
  if (now - lastPhaseChange > 6000) {
    animPhase = (animPhase + 1) % 5;
    lastPhaseChange = now;

    switch (animPhase) {
      case 0: rpmTarget = 800;  speedTarget = 0;   break; // idle
      case 1: rpmTarget = 2200; speedTarget = 50;  break; // city driving
      case 2: rpmTarget = 3000; speedTarget = 90;  break; // acceleration
      case 3: rpmTarget = 1800; speedTarget = 110; break; // motorway cruise
      case 4: rpmTarget = 1200; speedTarget = 30;  break; // slowing down
    }
  }

  // Smooth interpolation toward targets
  car.rpm   += (rpmTarget  - car.rpm)   * 0.08;
  car.speed += (speedTarget - car.speed) * 0.05;

  // Coolant warms up slowly from cold
  if (car.coolant < 90) car.coolant += 0.3;

  // Oil temp follows coolant but slower
  car.oil = car.coolant * 0.85;

  // Boost follows RPM — negative at idle, positive under load
  float boostTarget = (car.rpm > 2000) ? ((car.rpm - 1500) / 4000.0) : -0.1;
  car.boost += (boostTarget - car.boost) * 0.1;

  // Engine load follows RPM
  float loadTarget = constrain((car.rpm - 800) / 28.0, 5, 95);
  car.load += (loadTarget - car.load) * 0.08;

  // Intake temp rises slightly with load
  car.intake = 18 + (car.load * 0.2);

  // Battery voltage slight fluctuation
  car.battery = 12.4 + (sin(now / 3000.0) * 0.3);
}

// ============================================================
// DISPLAY SCREENS
// ============================================================

void drawScreenDots() {
  int startX = (128 - (NUM_SCREENS * 10)) / 2;
  for (int i = 0; i < NUM_SCREENS; i++) {
    if (i == currentScreen) {
      display.fillCircle(startX + (i * 10), 62, 2, SSD1306_WHITE);
    } else {
      display.drawCircle(startX + (i * 10), 62, 2, SSD1306_WHITE);
    }
  }
}

// Screen 1: RPM + Speed
void drawScreen1() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // RPM
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("RPM");

  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print((int)car.rpm);

  // Vertical divider
  display.drawFastVLine(68, 0, 42, SSD1306_WHITE);

  // Speed
  display.setTextSize(1);
  display.setCursor(74, 0);
  display.print("km/h");

  display.setTextSize(2);
  display.setCursor(74, 10);
  display.print((int)car.speed);

  // RPM bar graph
  display.drawFastHLine(0, 43, 128, SSD1306_WHITE);
  int rpmBar = map((int)car.rpm, 0, 4500, 0, 124);
  rpmBar = constrain(rpmBar, 0, 124);
  display.fillRect(2, 45, rpmBar, 6, SSD1306_WHITE);
  display.drawRect(2, 45, 124, 6, SSD1306_WHITE);

  // Coolant temp footer
  display.setTextSize(1);
  display.setCursor(0, 53);
  display.print("CLT:");
  display.print((int)car.coolant);
  display.print("C");

  drawScreenDots();
  display.display();
}

// Screen 2: Boost + Load
void drawScreen2() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("BOOST PRESSURE");

  // Boost value large
  display.setTextSize(2);
  display.setCursor(0, 10);
  if (car.boost >= 0) display.print("+");
  display.print(car.boost, 2);
  display.setTextSize(1);
  display.print(" bar");

  // Boost bar -0.5 to +1.5 bar
  display.drawFastHLine(0, 30, 128, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 32);
  display.print("-0.5");
  display.setCursor(96, 32);
  display.print("+1.5");

  int boostBar = map((int)(car.boost * 100), -50, 150, 0, 120);
  boostBar = constrain(boostBar, 0, 120);
  display.drawRect(4, 41, 120, 8, SSD1306_WHITE);
  display.fillRect(4, 41, boostBar, 8, SSD1306_WHITE);

  // Load
  display.setCursor(0, 53);
  display.print("LOAD: ");
  display.print((int)car.load);
  display.print("%  INT:");
  display.print((int)car.intake);
  display.print("C");

  drawScreenDots();
  display.display();
}

// Screen 3: Temps
void drawScreen3() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("TEMPERATURES");
  display.drawFastHLine(0, 9, 128, SSD1306_WHITE);

  // Coolant
  display.setCursor(0, 13);
  display.print("Coolant  ");
  display.setTextSize(2);
  display.print((int)car.coolant);
  display.setTextSize(1);
  display.print(" C");

  // Oil
  display.setCursor(0, 32);
  display.print("Oil      ");
  display.setTextSize(2);
  display.print((int)car.oil);
  display.setTextSize(1);
  display.print(" C");

  // Intake
  display.setCursor(0, 51);
  display.print("Intake   ");
  display.print((int)car.intake);
  display.print(" C");

  drawScreenDots();
  display.display();
}

// Screen 4: Electrical
void drawScreen4() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ELECTRICAL");
  display.drawFastHLine(0, 9, 128, SSD1306_WHITE);

  // Battery voltage large
  display.setCursor(0, 13);
  display.print("Battery");
  display.setTextSize(2);
  display.setCursor(0, 23);
  display.print(car.battery, 1);
  display.setTextSize(1);
  display.print(" V");

  // Battery bar — 10V to 15V
  display.drawRect(0, 40, 120, 8, SSD1306_WHITE);
  int batBar = map((int)(car.battery * 10), 100, 150, 0, 118);
  batBar = constrain(batBar, 0, 118);
  display.fillRect(0, 40, batBar, 8, SSD1306_WHITE);

  // Warning if low
  display.setCursor(0, 52);
  if (car.battery < 11.5) {
    display.print("!! LOW VOLTAGE !!");
  } else if (car.battery > 14.8) {
    display.print("!! HIGH VOLTAGE !!");
  } else {
    display.print("Voltage normal");
  }

  drawScreenDots();
  display.display();
}

// Startup splash
void drawSplash() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(7, 8);
  display.print("BMW DASH");

  display.setTextSize(1);
  display.setCursor(25, 30);
  display.print("Display v0.1");

  display.setCursor(20, 44);
  display.print("[ TEST MODE ]");

  display.display();
  delay(2500);
}

// ============================================================
// SETUP & LOOP
// ============================================================

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 not found — check wiring:");
    Serial.println("VCC→3.3V  GND→GND  SDA→GPIO21  SCL→GPIO22");
    while (true);
  }

  Serial.println("Display OK — running in test mode");
  drawSplash();
}

void loop() {
  unsigned long now = millis();

  // Update fake data every 50ms
  if (now - lastAnimUpdate >= 50) {
    updateFakeData();
    lastAnimUpdate = now;
  }

  // Auto-rotate screens every 5 seconds
  if (now - lastScreenChange >= SCREEN_INTERVAL) {
    currentScreen = (currentScreen + 1) % NUM_SCREENS;
    lastScreenChange = now;
  }

  // Draw current screen
  switch (currentScreen) {
    case 0: drawScreen1(); break;
    case 1: drawScreen2(); break;
    case 2: drawScreen3(); break;
    case 3: drawScreen4(); break;
  }
}
