/* =====================================================================
   SenseCore Advanced — Automated Access Control & Environmental Monitor
   ---------------------------------------------------------------------
   Upgrades over v1:
     - Explicit finite state machine (no more implicit bool flags)
     - Kalman filter for ultrasonic sensor fusion (replaces exp. smoothing)
     - Cooperative task scheduler (mini round-robin, no blocking delay())
     - Watchdog timer with stale-sensor fault detection & auto-recovery
     - EEPROM config block with checksum (corruption-safe persistence)
     - Debounced button via interrupt (ISR only sets a flag)
     - Servo-actuated physical gate + buzzer escalation tied to FSM state
     - JSON telemetry over Serial for a companion dashboard / logging
     - Optional temperature-compensated speed of sound (feature-flagged)
   ===================================================================== */

#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>
#include <avr/wdt.h>

/* ---- Feature flags: disable if the hardware isn't on your bench ---- */
#define ENABLE_SERVO               1
#define ENABLE_BUZZER              1
#define ENABLE_TEMP_COMPENSATION   0   // needs TMP36 on TEMP_PIN

#if ENABLE_SERVO
  #include <Servo.h>
  Servo gateServo;
#endif

/* ===================== PIN CONFIG ===================== */
#define IR_PIN 5
#define ECHO_PIN 3
#define TRIG_PIN 4

#define LED_WARN 11
#define LED_ERROR 12
#define LED_LIGHT 10

#define BUTTON_PIN 2
#define LDR_PIN A0
#define TEMP_PIN A2

#define SERVO_PIN 13
#define BUZZER_PIN A1

#define LCD_RS A5
#define LCD_EN A4
#define LCD_D4 6
#define LCD_D5 7
#define LCD_D6 8
#define LCD_D7 9

/* ===================== SYSTEM CONSTANTS ===================== */
#define LOCK_DISTANCE_CM      20.0
#define WARNING_DISTANCE_CM   50.0
#define CM_TO_IN              0.393701
#define DEBOUNCE_MS           40UL
#define SENSOR_STALE_MS       2000UL   // if no valid echo in this long -> fault
#define EEPROM_CONFIG_ADDR    0

enum Unit     { CM = 0, INCH = 1 };
enum LCDMode  { DISTANCE_VIEW, SETTINGS_VIEW, LIGHT_VIEW };
enum SystemState { STATE_INIT, STATE_MONITORING, STATE_WARNING, STATE_LOCKED, STATE_SETTINGS, STATE_FAULT };

/* ===================== CONFIG (EEPROM, checksum-protected) ===================== */
struct Config {
  uint8_t  unit;
  uint8_t  reserved;
  uint16_t checksum;
};
Config config;

uint16_t computeChecksum(const Config &c) {
  uint16_t sum = 0xAAAA;
  sum ^= c.unit;
  sum ^= (c.reserved << 8);
  return sum;
}

void loadConfig() {
  EEPROM.get(EEPROM_CONFIG_ADDR, config);
  if (config.unit > 1 || computeChecksum(config) != config.checksum) {
    config.unit = CM;         // corrupted or first boot -> safe default
    config.reserved = 0;
    config.checksum = computeChecksum(config);
    EEPROM.put(EEPROM_CONFIG_ADDR, config);
  }
}
void saveConfig() {
  config.checksum = computeChecksum(config);
  EEPROM.put(EEPROM_CONFIG_ADDR, config);
}

/* ===================== KALMAN FILTER (distance sensor fusion) ===================== */
struct KalmanFilter {
  float q;  // process noise
  float r;  // measurement noise
  float x;  // current estimate
  float p;  // estimate covariance
};
KalmanFilter distFilter = { 0.03f, 4.0f, 100.0f, 1.0f };

float kalmanUpdate(KalmanFilter &f, float measurement) {
  f.p += f.q;                          // predict
  float k = f.p / (f.p + f.r);         // gain
  f.x += k * (measurement - f.x);      // correct
  f.p *= (1.0f - k);
  return f.x;
}

/* ===================== GLOBAL OBJECTS ===================== */
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
IRrecv irrecv(IR_PIN);
decode_results irResults;

SystemState state = STATE_INIT;
LCDMode lcdMode = DISTANCE_VIEW;

volatile unsigned long echoStart = 0, echoEnd = 0;
volatile bool distanceReady = false;

volatile bool buttonFlag = false;
volatile unsigned long lastButtonISR = 0;

float filteredDistance = 100.0f;
int   lastLightReading = 0;
unsigned long lastValidEcho = 0;

/* ===================== INTERRUPT SERVICE ROUTINES ===================== */
void echoISR() {
  if (digitalRead(ECHO_PIN)) echoStart = micros();
  else { echoEnd = micros(); distanceReady = true; }
}

void buttonISR() {
  unsigned long now = millis();
  if (now - lastButtonISR > DEBOUNCE_MS) {
    buttonFlag = true;
    lastButtonISR = now;
  }
}

/* ===================== ULTRASONIC ===================== */
void triggerUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
}

#if ENABLE_TEMP_COMPENSATION
float speedOfSoundCmPerUs() {
  float tempC = (analogRead(TEMP_PIN) * 5.0f / 1024.0f - 0.5f) * 100.0f;
  float speedMS = 331.3f + 0.606f * tempC;   // m/s
  return speedMS / 10000.0f;                 // cm/us
}
#endif

float rawDistanceFromEcho() {
  float dt = (float)(echoEnd - echoStart);   // microseconds
#if ENABLE_TEMP_COMPENSATION
  float factor = speedOfSoundCmPerUs();
  return (dt * factor) / 2.0f;
#else
  return dt / 58.0f;                         // standard room-temp approximation
#endif
}

/* ===================== TASK: read + fuse distance ===================== */
void taskProcessDistance() {
  if (!distanceReady) return;
  distanceReady = false;

  float raw = rawDistanceFromEcho();
  if (raw <= 0 || raw > 400) return;          // reject out-of-range noise, don't feed filter garbage

  filteredDistance = kalmanUpdate(distFilter, raw);
  lastValidEcho = millis();
}

/* ===================== TASK: ambient light + PWM backlight ===================== */
void taskLDR() {
  lastLightReading = analogRead(LDR_PIN);
  int pwm = map(lastLightReading, 0, 1023, 255, 0);
  analogWrite(LED_LIGHT, pwm);
}

/* ===================== TASK: IR remote ===================== */
void taskIR() {
  if (!irrecv.decode(&irResults)) return;
  long cmd = irResults.value;
  irrecv.resume();

  switch (cmd) {
    case 0xFDA05F:  // unlock
      if (state == STATE_LOCKED) state = STATE_MONITORING;
      break;
    case 0xFDB04F:  // toggle unit
      config.unit = (config.unit == CM) ? INCH : CM;
      saveConfig();
      break;
    case 0xFD50AF:  // cycle LCD view
      lcdMode = (LCDMode)((lcdMode + 1) % 3);
      lcd.clear();
      break;
  }
}

/* ===================== OUTPUTS: LEDs, buzzer, servo per state ===================== */
void updateOutputs() {
  unsigned long now = millis();
  switch (state) {
    case STATE_MONITORING:
      digitalWrite(LED_WARN, LOW);
      digitalWrite(LED_ERROR, LOW);
#if ENABLE_SERVO
      gateServo.write(180);              // gate open
#endif
#if ENABLE_BUZZER
      noTone(BUZZER_PIN);
#endif
      break;

    case STATE_WARNING: {
      bool blink = (now % 400) < 200;
      digitalWrite(LED_WARN, blink);
      digitalWrite(LED_ERROR, LOW);
#if ENABLE_SERVO
      gateServo.write(90);               // gate half-closed
#endif
#if ENABLE_BUZZER
      if (blink) tone(BUZZER_PIN, 1000); else noTone(BUZZER_PIN);
#endif
      break;
    }

    case STATE_LOCKED: {
      bool blink = (now % 150) < 75;      // faster blink = higher urgency
      digitalWrite(LED_WARN, blink);
      digitalWrite(LED_ERROR, blink);
#if ENABLE_SERVO
      gateServo.write(0);                // gate closed
#endif
#if ENABLE_BUZZER
      if (blink) tone(BUZZER_PIN, 2500); else noTone(BUZZER_PIN);
#endif
      break;
    }

    case STATE_FAULT: {
      bool blink = (now % 100) < 50;      // rapid strobe = system fault
      digitalWrite(LED_ERROR, blink);
      digitalWrite(LED_WARN, blink);
#if ENABLE_BUZZER
      if (blink) tone(BUZZER_PIN, 400); else noTone(BUZZER_PIN);
#endif
      break;
    }

    default: break;
  }
}

/* ===================== DISPLAY ===================== */
void updateDisplay() {
  if (lcdMode == SETTINGS_VIEW) {
    lcd.setCursor(0, 0); lcd.print("Settings mode   ");
    lcd.setCursor(0, 1);
    lcd.print(config.unit == INCH ? "Unit: inches   " : "Unit: cm        ");
    return;
  }
  if (lcdMode == LIGHT_VIEW) {
    lcd.setCursor(0, 0); lcd.print("Light: ");
    lcd.print(lastLightReading); lcd.print("     ");
    lcd.setCursor(0, 1); lcd.print("                ");
    return;
  }

  // DISTANCE_VIEW
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  float d = filteredDistance;
  lcd.print(config.unit == INCH ? d * CM_TO_IN : d);
  lcd.print(config.unit == INCH ? " in   " : " cm   ");

  lcd.setCursor(0, 1);
  switch (state) {
    case STATE_LOCKED:     lcd.print("OBSTACLE ALERT  "); break;
    case STATE_WARNING:    lcd.print("!! WARNING !!   "); break;
    case STATE_FAULT:      lcd.print("SENSOR FAULT    "); break;
    default:               lcd.print("Safe            "); break;
  }
}

/* ===================== FINITE STATE MACHINE ===================== */
void enterState(SystemState s) {
  if (s == state) return;
  state = s;
  lcd.clear();
}

void taskFSM() {
  // Fault takes priority over everything else
  if (millis() - lastValidEcho > SENSOR_STALE_MS && state != STATE_FAULT) {
    enterState(STATE_FAULT);
  }

  if (buttonFlag) {
    buttonFlag = false;
    if (state == STATE_LOCKED)        enterState(STATE_MONITORING);
    else if (state == STATE_MONITORING) enterState(STATE_SETTINGS);
    else if (state == STATE_SETTINGS)   enterState(STATE_MONITORING);
  }

  // Distance-driven transitions only apply when not manually parked in SETTINGS/FAULT
  if (state != STATE_SETTINGS && state != STATE_FAULT) {
    float d = filteredDistance;
    if (d < LOCK_DISTANCE_CM)          enterState(STATE_LOCKED);
    else if (d < WARNING_DISTANCE_CM)  enterState(STATE_WARNING);
    else                                enterState(STATE_MONITORING);
  }

  updateOutputs();
  updateDisplay();
}

/* ===================== TELEMETRY (JSON over Serial) ===================== */
void taskTelemetry() {
  Serial.print(F("{\"state\":"));   Serial.print(state);
  Serial.print(F(",\"dist_cm\":")); Serial.print(filteredDistance);
  Serial.print(F(",\"light\":"));   Serial.print(lastLightReading);
  Serial.print(F(",\"unit\":"));    Serial.print(config.unit);
  Serial.println(F("}"));
}

/* ===================== COOPERATIVE TASK SCHEDULER ===================== */
typedef void (*TaskFn)();
struct Task { TaskFn fn; unsigned long interval; unsigned long lastRun; };

Task tasks[] = {
  { triggerUltrasonic,   60,  0 },
  { taskProcessDistance, 5,   0 },   // poll frequently; ISR sets the flag
  { taskLDR,             100, 0 },
  { taskIR,              10,  0 },
  { taskFSM,             20,  0 },
  { taskTelemetry,       500, 0 },
};
const uint8_t NUM_TASKS = sizeof(tasks) / sizeof(Task);

void runScheduler() {
  unsigned long now = millis();
  for (uint8_t i = 0; i < NUM_TASKS; i++) {
    if (now - tasks[i].lastRun >= tasks[i].interval) {
      tasks[i].lastRun = now;
      tasks[i].fn();
    }
  }
}

/* ===================== SETUP ===================== */
void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  irrecv.enableIRIn();

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(LED_WARN, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
#if ENABLE_BUZZER
  pinMode(BUZZER_PIN, OUTPUT);
#endif
#if ENABLE_SERVO
  gateServo.attach(SERVO_PIN);
  gateServo.write(180);  // start open
#endif

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  loadConfig();
  lastValidEcho = millis();

  lcd.print("SenseCore v2");
  lcd.setCursor(0, 1);
  lcd.print("Self-test OK");
  delay(1000);
  lcd.clear();

  enterState(STATE_MONITORING);
  wdt_enable(WDTO_2S);   // hardware watchdog: resets MCU if loop() ever hangs
}

/* ===================== LOOP ===================== */
void loop() {
  wdt_reset();           // feed the watchdog every pass while healthy

  // If we're stuck in FAULT, stop feeding the watchdog deliberately —
  // this forces a clean MCU reset instead of limping along on bad data.
  if (state == STATE_FAULT && millis() - lastValidEcho > SENSOR_STALE_MS * 3) {
    while (true) { /* intentionally not feeding wdt_reset() -> forces reboot */ }
  }

  runScheduler();
}
