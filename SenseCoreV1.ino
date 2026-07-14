#include <LiquidCrystal.h>
#include <IRremote.h>
#include <EEPROM.h>

/* ===================== PIN CONFIG ===================== */
#define IR_PIN 5
#define ECHO_PIN 3
#define TRIG_PIN 4

#define LED_WARN 11
#define LED_ERROR 12
#define LED_LIGHT 10

#define BUTTON_PIN 2
#define LDR_PIN A0

#define LCD_RS A5
#define LCD_EN A4
#define LCD_D4 6
#define LCD_D5 7
#define LCD_D6 8
#define LCD_D7 9

/* ===================== SYSTEM CONSTANTS ===================== */
#define LOCK_DISTANCE_CM 20.0
#define WARNING_DISTANCE_CM 50.0
#define CM_TO_IN 0.393701

#define EEPROM_UNIT_ADDR 50

enum Unit { CM = 0, INCH = 1 };
enum LCDMode { DISTANCE_VIEW, SETTINGS_VIEW, LIGHT_VIEW };

/* ===================== GLOBAL STATE ===================== */
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
IRrecv irrecv(IR_PIN);
decode_results results;

/* Timing */
unsigned long tUltrasonic = 0;
unsigned long tLED = 0;
unsigned long tLDR = 0;

/* Intervals */
const unsigned long ULTRA_INTERVAL = 60;
const unsigned long LDR_INTERVAL = 100;

/* Sensor data */
volatile unsigned long echoStart = 0;
volatile unsigned long echoEnd = 0;
volatile bool distanceReady = false;

float distanceCM = 0;
float filteredDistance = 100;

/* System state */
bool locked = false;
Unit unit = CM;
LCDMode lcdMode = DISTANCE_VIEW;

/* ===================== UTILITY ===================== */
float readDistance()
{
  float raw = (echoEnd - echoStart) / 58.0;

  if (raw <= 0 || raw > 400) return filteredDistance;

  // Low-pass filter (smoother readings)
  filteredDistance = (filteredDistance * 0.7) + (raw * 0.3);
  return filteredDistance;
}

/* ===================== INTERRUPT ===================== */
void echoISR()
{
  if (digitalRead(ECHO_PIN)) {
    echoStart = micros();
  } else {
    echoEnd = micros();
    distanceReady = true;
  }
}

/* ===================== ULTRASONIC ===================== */
void triggerUltrasonic()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
}

/* ===================== SYSTEM CONTROL ===================== */
void lockSystem()
{
  locked = true;
  digitalWrite(LED_ERROR, LOW);
  digitalWrite(LED_WARN, LOW);
}

void unlockSystem()
{
  locked = false;
  lcd.clear();
}

/* ===================== DISPLAY ===================== */
void showDistance(float d)
{
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");

  if (unit == INCH)
    lcd.print(d * CM_TO_IN);
  else
    lcd.print(d);

  lcd.print(unit == INCH ? " in   " : " cm   ");

  lcd.setCursor(0, 1);

  if (d < WARNING_DISTANCE_CM)
    lcd.print("!! WARNING !!   ");
  else
    lcd.print("Safe            ");
}

void showLocked()
{
  lcd.setCursor(0, 0);
  lcd.print("OBSTACLE ALERT  ");
  lcd.setCursor(0, 1);
  lcd.print("Press to unlock ");
}

void showLight(int val)
{
  lcd.setCursor(0, 0);
  lcd.print("Light: ");
  lcd.print(val);
  lcd.print("     ");
}

/* ===================== LED CONTROL ===================== */
void updateLEDs(float d)
{
  if (locked) {
    digitalWrite(LED_ERROR, millis() % 400 < 200);
    digitalWrite(LED_WARN, millis() % 400 < 200);
    return;
  }

  digitalWrite(LED_WARN, millis() % max(100, (int)(d * 5)) < 50);
}

/* ===================== LDR ===================== */
void updateLight()
{
  int val = analogRead(LDR_PIN);
  int pwm = map(val, 0, 1023, 255, 0);
  analogWrite(LED_LIGHT, pwm);

  if (lcdMode == LIGHT_VIEW && !locked)
    showLight(val);
}

/* ===================== IR CONTROL ===================== */
void handleIR(long cmd)
{
  switch (cmd)
  {
    case 0xFDA05F: unlockSystem(); break;
    case 0xFD00FF:
      if (lcdMode == SETTINGS_VIEW) {
        unit = CM;
        EEPROM.write(EEPROM_UNIT_ADDR, unit);
      }
      break;

    case 0xFDB04F:
      unit = (unit == CM) ? INCH : CM;
      EEPROM.write(EEPROM_UNIT_ADDR, unit);
      break;

    case 0xFD50AF:
      lcdMode = (LCDMode)((lcdMode + 1) % 3);
      lcd.clear();
      break;
  }
}

/* ===================== SETUP ===================== */
void setup()
{
  Serial.begin(115200);

  lcd.begin(16, 2);
  irrecv.enableIRIn();

  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  pinMode(LED_WARN, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);
  pinMode(LED_LIGHT, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoISR, CHANGE);

  unit = EEPROM.read(EEPROM_UNIT_ADDR);
  if (unit > 1) unit = CM;

  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
}

/* ===================== LOOP ===================== */
void loop()
{
  unsigned long now = millis();

  /* Ultrasonic trigger */
  if (now - tUltrasonic >= ULTRA_INTERVAL) {
    tUltrasonic = now;
    triggerUltrasonic();
  }

  /* Distance processing */
  if (distanceReady) {
    distanceReady = false;

    float d = readDistance();

    if (d < LOCK_DISTANCE_CM)
      lockSystem();

    updateLEDs(d);

    if (!locked && lcdMode == DISTANCE_VIEW)
      showDistance(d);
  }

  /* IR remote */
  if (irrecv.decode(&results)) {
    handleIR(results.value);
    irrecv.resume();
  }

  /* LDR processing */
  if (now - tLDR >= LDR_INTERVAL) {
    tLDR = now;
    updateLight();
  }

  /* Locked UI */
  if (locked && lcdMode == DISTANCE_VIEW)
    showLocked();
}
