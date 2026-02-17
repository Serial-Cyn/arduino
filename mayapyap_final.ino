#include <Servo.h>

// ===================== SERVOS =====================
Servo linearActuator;
Servo door1;
Servo door2;

// ===================== PINS =====================
const int LINEAR_PIN      = 2;
const int DOOR1_PIN       = 3;
const int DOOR2_PIN       = 4;
const int RELAY_PIN       = 5;
const int FAN_PIN         = 6;
const int BUTTON_PIN      = 8;
const int GREEN_LED_PIN   = 10;
const int RED_LED_PIN     = 11;
const int RAIN_PIN        = A0;

// ===================== CONSTANTS =====================
const int RAIN_THRESHOLD  = 400;
const int FAN_SPEED       = 126;     // 50%
const int ACTION_DURATION = 1000;    // ms
const int POLL_INTERVAL   = 2000;    // ms
const int SERVO_STOP      = 90;

// ===================== STATES =====================
bool reverseMode = false;
bool isClose = false;
bool prevState = false;

void setup() {
  Serial.begin(9600);

  // Attach servos
  linearActuator.attach(LINEAR_PIN);
  door1.attach(DOOR1_PIN);
  door2.attach(DOOR2_PIN);

  // Pin modes
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // Default
  openSystem();

  delay(200); // Allows the system to prepare
}

void loop() {
  int rainValue = analogRead(RAIN_PIN);
  bool raining = rainValue < RAIN_THRESHOLD;

  bool currentState = digitalRead(BUTTON_PIN);
  if (currentState == HIGH) {
    Serial.println("Button pressed!");
    reverseMode = true;
  }

  if (raining) {
    isClose = true;
  } else {
    isClose = false;
  }

  if (reverseMode) {
    isClose = !isClose;
    Serial.println("Reversing System...");
  }

  if (prevState == isClose) {
    return; // Do not run the same command again and again
  }
  prevState = isClose;

  if (isClose) {
    closeSystem();
  } else {
    openSystem();
  }
}

// ========= FUNCTION =========

void openSystem() {
  Serial.println("=== OPENING SYSTEM ===");

  // Turn on Green LED to give feedback
  Serial.println("Turning LED ON...");
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  Serial.println("LED turned ON.");

  // Open the door first to avoid jams
  Serial.println("Opening doors...");
  door1.write(0);
  door2.write(180);
  delay(2000);

  // Stop the door servos
  Serial.println("Doors opened.");
  door1.write(90);
  door2.write(90);
  delay(500);

  // Push the extender to expose the clothesline
  Serial.println("Pushing Clothesline...");
  linearActuator.write(180);
  delay(4000);

  // Stop the Linear Actuator to avoid over extending
  Serial.println("Extender stopped.");
  linearActuator.write(90);
  delay(500);

  // Turn off the fan since it is not in use
  Serial.println("Fan stopped.");
  digitalWrite(RELAY_PIN, LOW);

  Serial.println("=== SYSTEM OPENED ===");
}

void closeSystem() {
  Serial.println("=== CLOSING SYSTEM ===");

  // Turn on Red LED to give feedback
  Serial.println("Turning LED ON...");
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);
  Serial.println("LED turned ON.");

  // Retract the extender to hide the clothesline
  Serial.println("Retracting Clothesline...");
  linearActuator.write(0);
  delay(4000);

  // Stop the Linear Actuator to avoid over retracting
  Serial.println("Extender stopped.");
  linearActuator.write(90);
  delay(500);

  // close the door to prevent moisture from outside
  Serial.println("Closing doors...");
  door1.write(180);
  door2.write(0);
  delay(2000);

  // Stop the door servos
  Serial.println("Doors closed.");
  door1.write(90);
  door2.write(90);
  delay(500);

  // Turn on the fan to dry the clothes
  Serial.println("Fan stopped.");
  digitalWrite(RELAY_PIN, HIGH);

  Serial.println("=== SYSTEM CLOSED ===");
}