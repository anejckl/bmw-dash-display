// ============================================================
// BMW Dash Display — OLED + Real OBD2 Data
// Hardware: ESP32 DevKit + 0.96" SSD1306 OLED + Bluetooth ELM327
// Wiring:   OLED VCC→3.3V  GND→GND  SDA→GPIO21  SCL→GPIO22
//           ELM327 connects via Bluetooth — no wires needed
// Libraries: Adafruit SSD1306, Adafruit GFX, ELMduino
// ============================================================

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BluetoothSerial.h>
#include <ELMduino.h>

// ── Display config ──────────────────────────────────────────
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ── ELM327 Bluetooth config ────────────────────────────────
BluetoothSerial SerialBT;
#define ELM_NAME "OBDII"  // Change to match your dongle name (common: OBDII, OBD2, V-LINK)
ELM327 myELM327;

// ── Car data ────────────────────────────────────────────────
struct CarData {
  float rpm     = 0;
  float speed   = 0;
  float coolant = 0;
  float boost   = 0;
  float intake  = 0;
  float load    = 0;
  float battery = 0;
  float oil     = 0;
  bool  connected = false;
};
CarData car;

// ── OBD polling state machine ───────────────────────────────
enum OBD_PID {
  PID_RPM,
  PID_SPEED,
  PID_COOLANT,
  PID_BOOST,
  PID_INTAKE,
  PID_LOAD,
  PID_OIL,
  PID_BATTERY
};

// Priority schedule: RPM & speed polled most, temps least
const OBD_PID pollSchedule[] = {
  PID_RPM, PID_SPEED,
  PID_RPM, PID_SPEED, PID_BOOST,
  PID_RPM, PID_SPEED,
  PID_RPM, PID_SPEED, PID_LOAD,
  PID_COOLANT, PID_INTAKE,
  PID_RPM, PID_SPEED,
  PID_OIL, PID_BATTERY
};
const int SCHEDULE_LENGTH = sizeof(pollSchedule) / sizeof(pollSchedule[0]);
int scheduleIndex = 0;

int consecutiveErrors = 0;
const int ERROR_THRESHOLD = 20;

int oilTempFailures = 0;
bool oilTempSupported = true;

// ── Screen management ───────────────────────────────────────
int currentScreen = 0;
const int NUM_SCREENS = 4;
unsigned long lastScreenChange = 0;
const int SCREEN_INTERVAL = 5000;

// ============================================================
// ELM327 BLUETOOTH CONNECTION
// ============================================================

void connectELM327() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Searching for");
  display.setCursor(0, 10);
  display.print(ELM_NAME);
  display.print("...");
  display.display();

  SerialBT.begin("BMW_DASH", true);  // true = master mode
  Serial.println("Bluetooth started — scanning...");

  if (!SerialBT.connect(ELM_NAME)) {
    display.setCursor(0, 30);
    display.print("BT pair failed!");
    display.setCursor(0, 40);
    display.print("Check dongle is on");
    display.display();
    Serial.println("Bluetooth pairing failed");
    car.connected = false;
    delay(3000);
    return;
  }

  display.setCursor(0, 30);
  display.print("BT paired! Init...");
  display.display();

  int attempts = 0;
  const int MAX_ATTEMPTS = 5;

  while (attempts < MAX_ATTEMPTS) {
    if (myELM327.begin(SerialBT, false, 2000)) {
      car.connected = true;
      consecutiveErrors = 0;
      display.setCursor(0, 50);
      display.print("CONNECTED!");
      display.display();
      Serial.println("ELM327 connected");
      delay(1000);
      return;
    }
    attempts++;
    delay(1000);
  }

  car.connected = false;
  display.setCursor(0, 50);
  display.print("ELM327 init failed");
  display.display();
  Serial.println("ELM327 init failed");
  delay(2000);
}

// ============================================================
// OBD POLLING — non-blocking state machine
// ============================================================

void updateOBD() {
  if (!car.connected) return;

  OBD_PID currentPid = pollSchedule[scheduleIndex];

  // Skip oil temp if unsupported
  if (currentPid == PID_OIL && !oilTempSupported) {
    scheduleIndex = (scheduleIndex + 1) % SCHEDULE_LENGTH;
    return;
  }

  float val = 0;

  switch (currentPid) {
    case PID_RPM:     val = myELM327.rpm();                break;
    case PID_SPEED:   val = myELM327.kph();                break;
    case PID_COOLANT: val = myELM327.engineCoolantTemp();  break;
    case PID_BOOST:   val = myELM327.manifoldPressure();   break;
    case PID_INTAKE:  val = myELM327.intakeAirTemp();      break;
    case PID_LOAD:    val = myELM327.engineLoad();         break;
    case PID_OIL:     val = myELM327.oilTemp();            break;
    case PID_BATTERY: myELM327.batteryVoltage();           break;
  }

  if (myELM327.nb_rx_state == ELM_SUCCESS) {
    consecutiveErrors = 0;

    switch (currentPid) {
      case PID_RPM:     car.rpm     = val;                         break;
      case PID_SPEED:   car.speed   = val;                         break;
      case PID_COOLANT: car.coolant = val;                         break;
      case PID_BOOST:   car.boost   = (val / 100.0) - 1.013;      break;
      case PID_INTAKE:  car.intake  = val;                         break;
      case PID_LOAD:    car.load    = val;                         break;
      case PID_OIL:
        car.oil = val;
        oilTempFailures = 0;
        break;
      case PID_BATTERY:
        car.battery = atof(myELM327.payload);
        break;
    }

    scheduleIndex = (scheduleIndex + 1) % SCHEDULE_LENGTH;
  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG) {
    // Error — skip and move on
    consecutiveErrors++;

    if (currentPid == PID_OIL) {
      oilTempFailures++;
      if (oilTempFailures >= 3) {
        oilTempSupported = false;
        Serial.println("Oil temp PID not supported — disabling");
      }
    }

    scheduleIndex = (scheduleIndex + 1) % SCHEDULE_LENGTH;

    // Reconnect if too many errors in a row
    if (consecutiveErrors >= ERROR_THRESHOLD) {
      Serial.println("Too many OBD errors — reconnecting");
      car.connected = false;
      connectELM327();
    }
  }
  // ELM_GETTING_MSG → still waiting, just return (non-blocking)
}

// ============================================================
// DISPLAY SCREENS (identical to v0.1)
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

  // Connection indicator
  if (!car.connected) {
    display.setTextSize(1);
    display.setCursor(90, 56);
    display.print("NOLINK");
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
  display.print("Display v0.2");

  display.setCursor(22, 44);
  display.print("[ LIVE DATA ]");

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

  Serial.println("Display OK — connecting to ELM327 via Bluetooth");
  drawSplash();
  connectELM327();
}

void loop() {
  unsigned long now = millis();

  // Poll OBD (non-blocking)
  updateOBD();

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
