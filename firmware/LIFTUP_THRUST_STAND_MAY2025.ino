#include <Servo.h>             // Libreria per controllare ESC tramite PWM
#include "HX711.h"             // Libreria per leggere le celle di carico HX711
#include <Wire.h>              // I2C standard
#include <INA226_WE.h>         // Libreria per sensore INA226 (tensione/corrente)

// === DEFINIZIONE PIN ===
#define LED_ARMED   8          // LED che indica sistema armato
#define LED_SAFE    10         // LED che indica sistema sicuro/disarmato
#define ARM_BUTTON_PIN    9    // Pulsante per armare/disarmare
#define START_BUTTON_PIN  12   // Pulsante per avviare il test
#define ESC_PIN     11         // Pin PWM per ESC (motore)

#define DT_LEFT     3          // Pin DT cella Torque Left
#define SCK_LEFT    2
#define DT_RIGHT    7          // Pin DT cella Torque Right
#define SCK_RIGHT   6
#define DT_THRUST   5          // Pin DT cella Thrust
#define SCK_THRUST  4

#define VIN         A0         // Pin analogico per ACS758
#define I2C_ADDRESS 0x40       // Indirizzo I2C del sensore INA226

// === COSTANTI ===
const float VCC = 5.0;         // Tensione di riferimento per analogRead
const int acsModel = 0;        // Modello ACS758 (0 → 50A bidirezionale)
float sensitivity[] = {40.0, 60.0, 20.0, 40.0, 13.3, 16.7, 10.0, 20.0};
const float FACTOR = sensitivity[acsModel] / 1000.0; // V per Ampere
const float calibrationMultiplier = 3.11; // Fattore empirico per calibrazione ACS
const float currentOffset = 0.550;        // Offset a riposo del sensore
const float busVoltageScalingFactor = 15.16; // Fattore di scaling per INA226

// Fattori di calibrazione per le 3 celle
const float CALIB_FACTOR_LEFT   = 330000.00;
const float CALIB_FACTOR_RIGHT  = -346708.00;
const float CALIB_FACTOR_THRUST = -213499.00;

// Range PWM dell’ESC
const int pwm_min = 1100;
const int pwm_max = 2000;

// Parametri test (standard: 10, 10, 100. per un test che va da 0% a 100% con uno scalino del 10% alla volta) 
const int test_start_percent = 10;                 // % DI AVVIO TEST
const int test_step_percent  = 10;                 // INCREMENTO TRA STEP
const int test_end_percent   = 100;                // % DI FINE TEST

const unsigned long stabilizationTime = 2000;      // Tempo per stabilizzare il motore tra uno step e un altro
const unsigned long measurementDuration = 5000;    // Durata di uno step in ms

const float CURRENT_LIMIT = 38.0; // NON MODIFICARE! PERICOLO! Soglia massima corrente media per step 
const float DISTANCE_FROM_CENTER_M = 0.03; // Distanza delle celle dal centro (in metri)
const float KG_TO_G = 1000.0;

// === OGGETTI ===
Servo esc;
HX711 scaleLeft, scaleRight, scaleThrust;
INA226_WE ina226(I2C_ADDRESS);

// === VARIABILI DI STATO ===
bool is_armed = false;
bool is_test_running = false;
bool lastArmButtonState = HIGH;
bool lastStartButtonState = HIGH;
float baselineVoltage = 0;

// === PROTOTIPI FUNZIONI ===
void configureScale(HX711 &scale, int dt, int sck, float cal, const char *label);
void calibrateBaseline();
void armSystem();
void disarmSystem();
bool tareBalances();
void runTest();
void flashLedFast(int pin, int durationMs);

// === SETUP ===
void setup() {
  Serial.begin(9600);
  Serial.println("Avvio sistema...");

  pinMode(LED_ARMED, OUTPUT);
  pinMode(LED_SAFE, OUTPUT);
  pinMode(ARM_BUTTON_PIN, INPUT_PULLUP);
  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  lastArmButtonState = digitalRead(ARM_BUTTON_PIN);
  lastStartButtonState = digitalRead(START_BUTTON_PIN);
  digitalWrite(LED_ARMED, LOW);
  digitalWrite(LED_SAFE, HIGH);

  esc.attach(ESC_PIN);
  esc.writeMicroseconds(1000);
  delay(3000);
  Serial.println("ESC configurato.");

  configureScale(scaleLeft, DT_LEFT, SCK_LEFT, CALIB_FACTOR_LEFT, "Torque Left");
  configureScale(scaleRight, DT_RIGHT, SCK_RIGHT, CALIB_FACTOR_RIGHT, "Torque Right");
  configureScale(scaleThrust, DT_THRUST, SCK_THRUST, CALIB_FACTOR_THRUST, "Thrust");

  Serial.println("Calibrazione baseline ACS758...");
  calibrateBaseline();
  Serial.println("Baseline completata.");

  Wire.begin();
  ina226.init();
}

// === LOOP ===
void loop() {
  // Lettura pulsante armamento
  bool currentArm = digitalRead(ARM_BUTTON_PIN);
  if (lastArmButtonState == HIGH && currentArm == LOW) {
    if (!is_armed) {
      armSystem();
    } else {
      disarmSystem();
    }
    delay(300);
  }
  lastArmButtonState = currentArm;

  // Lettura pulsante avvio test
  bool currentStart = digitalRead(START_BUTTON_PIN);
  if (lastStartButtonState == HIGH && currentStart == LOW) {
    delay(30); // debounce
    if (digitalRead(START_BUTTON_PIN) == LOW) {
      if (is_armed) {
        if (tareBalances()) {
          is_test_running = true;
        } else {
          Serial.println("Errore: celle non pronte.");
        }
      } else {
        Serial.println("❌ Errore: tentativo di avvio test con sistema disarmato.");
        flashLedFast(LED_ARMED, 1000);
      }
    }
  }
  lastStartButtonState = currentStart;

  if (is_test_running) {
    runTest();
    is_test_running = false;
    disarmSystem();
  }

  delay(50);
}

// === CONFIGURA CELLA DI CARICO ===
void configureScale(HX711 &scale, int dt, int sck, float calibration_factor, const char *label) {
  scale.begin(dt, sck);
  Serial.print("Configurazione cella ");
  Serial.println(label);
  while (!scale.is_ready()) {
    Serial.print("Cella ");
    Serial.print(label);
    Serial.println(" non pronta...");
    delay(500);
  }
  scale.set_scale(calibration_factor);
  scale.tare();
  Serial.print("Cella ");
  Serial.print(label);
  Serial.println(" pronta.");
}

// === CALIBRA BASELINE CORRENTE ===
void calibrateBaseline() {
  long sumADC = 0;
  for (int i = 0; i < 50; i++) {
    sumADC += analogRead(VIN);
    delay(20);
  }
  float avgADC = sumADC / 50.0;
  baselineVoltage = (avgADC / 1023.0) * VCC;
  Serial.print("Baseline ACS758: ");
  Serial.print(baselineVoltage, 3);
  Serial.println(" V");
}

// === ARM E DISARM ===
void armSystem() {
  is_armed = true;
  digitalWrite(LED_ARMED, HIGH);
  digitalWrite(LED_SAFE, LOW);
  Serial.println("✅ Sistema armato.");
}

void disarmSystem() {
  is_armed = false;
  digitalWrite(LED_ARMED, LOW);
  digitalWrite(LED_SAFE, HIGH);
  Serial.println("🔒 Sistema disarmato.");
}

// === TARA TUTTE LE CELLE ===
bool tareBalances() {
  Serial.println("Taratura celle...");
  bool l = false, r = false, t = false;

  for (int i = 0; i < 10; i++) {
    if (scaleLeft.is_ready())   { scaleLeft.tare();   l = true; }
    if (scaleRight.is_ready())  { scaleRight.tare();  r = true; }
    if (scaleThrust.is_ready()) { scaleThrust.tare(); t = true; }
    if (l && r && t) {
      Serial.println("✅ Taratura completata.");
      return true;
    }
    delay(500);
  }

  return false;
}

// === ESECUZIONE TEST ===
void runTest() {
  Serial.println("🚀 Avvio test...");

  for (int percent = test_start_percent; percent <= test_end_percent; percent += test_step_percent) {
    int pwm_value = map(percent, 0, 100, pwm_min, pwm_max);
    esc.writeMicroseconds(pwm_value);
    delay(stabilizationTime);

    float sum_thrust = 0, sum_torqueNm = 0, sum_voltage = 0, sum_current = 0;
    int samples = 0;

    unsigned long start = millis();
    while (millis() - start < measurementDuration) {
      float leftGr  = scaleLeft.get_units() * KG_TO_G;
      float rightGr = scaleRight.get_units() * KG_TO_G;
      float thrustGr = -scaleThrust.get_units() * KG_TO_G;
      float torqueNm = (leftGr - rightGr) / 1000.0 * DISTANCE_FROM_CENTER_M;

      float vRaw = (VCC / 1023.0) * analogRead(VIN);
      float iValue = ((vRaw - baselineVoltage + 0.007) / FACTOR) * calibrationMultiplier - currentOffset;
      float vBus = ina226.getBusVoltage_V() * busVoltageScalingFactor;
      if (vBus < 1.0) vBus = 0.0;

      sum_thrust += thrustGr;
      sum_torqueNm += torqueNm;
      sum_voltage += vBus;
      sum_current += iValue;
      samples++;

      // LED lampeggiano alternati
      if ((millis() / 250) % 2 == 0) {
        digitalWrite(LED_ARMED, HIGH);
        digitalWrite(LED_SAFE, LOW);
      } else {
        digitalWrite(LED_ARMED, LOW);
        digitalWrite(LED_SAFE, HIGH);
      }

      delay(20);
    }

    float avg_thrust = sum_thrust / samples;
    float avg_torqueNm = sum_torqueNm / samples;
    float avg_voltage = sum_voltage / samples;
    float avg_current = sum_current / samples;
    float avg_power = avg_voltage * avg_current;

    Serial.print("PWM ");
    Serial.print(percent);
    Serial.print("% → THRUST: ");
    Serial.print(avg_thrust, 0);
    Serial.print("g | TORQUE: ");
    Serial.print(avg_torqueNm, 4);
    Serial.print(" Nm | VOLT: ");
    Serial.print(avg_voltage, 2);
    Serial.print("V | CURR: ");
    Serial.print(avg_current, 2);
    Serial.print("A | POWER: ");
    Serial.print(avg_power, 2);
    Serial.println("W");

    if (!isnan(avg_current) && avg_current >= CURRENT_LIMIT) {
      Serial.println("⚠️ Corrente media troppo alta! Test interrotto.");
      break;
    }

    delay(1000);
  }

  esc.writeMicroseconds(1000);
  Serial.println("✅ Test terminato.");
  digitalWrite(LED_ARMED, LOW);
  digitalWrite(LED_SAFE, HIGH);
}

// === LAMPEGGIO RAPIDO LED PER ERRORE ===
void flashLedFast(int pin, int durationMs) {
  unsigned long start = millis();
  while (millis() - start < durationMs) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
  }
}