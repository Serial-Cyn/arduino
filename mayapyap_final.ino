#include <Servo.h>

// SERVOS
Servo LinearActuator;
Servo Door_1;
Servo Door_2;

// PINS
const int LINEAR_PIN = 2;
const int DOOR1_PIN = 3;
const int DOOR2_PIN = 4;
const int RELAY_PIN = 5;
const int BUTTON_PIN = 8;
const int GREEN_LED_PIN = 10;
const int RED_LED_PIN = 11;
const int RAIN_PIN = A0;
const int FAN_YELLOW_PIN = 6;

// RAIN SENSOR THRESHOLD
const int RAIN_THRESHOLD = 400;

// CONSTANTS
const int FAN_SPEED = 126; // 50%
const int ACTION_DURATION = 1000; // ms for servo run
const int POLL_INTERVAL = 200;    // ms loop delay

// STATES
bool active = true;      // System ON
bool reverse = false;    // Button toggled
bool prevButton = HIGH;  // For edge detection
bool prevClosed = false; // To detect state change

void setup() {
  LinearActuator.attach(LINEAR_PIN);
  Door_1.attach(DOOR1_PIN);
  Door_2.attach(DOOR2_PIN);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  analogWrite(FAN_YELLOW_PIN, 0); // Fan off
  digitalWrite(RELAY_PIN, LOW);    // Relay off

  Serial.begin(9600);
  delay(200);
}

void loop() {
  int rainValue = analogRead(RAIN_PIN);
  bool isRaining = rainValue < RAIN_THRESHOLD;

  // BUTTON EDGE DETECTION
  bool buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && prevButton == HIGH) {
    reverse = !reverse; // Toggle reversal
    Serial.println(reverse ? "SYSTEM REVERSED" : "SYSTEM NORMAL");
  }
  prevButton = buttonState;

  // Determine system state
  bool shouldClose = isRaining;
  if (reverse) shouldClose = !shouldClose;

  // Only act if state changed
  if (shouldClose != prevClosed) {
    prevClosed = shouldClose;
    if (shouldClose) {
      closeSystem();
    } else {
      openSystem();
    }
  }

  Serial.println(rainValue);
  delay(POLL_INTERVAL);
}

// FUNCTIONS FOR CONTINUOUS SERVOS

void openSystem() {
  // LEDs
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);

  // Doors open (full speed)
  Door_1.write(180); // one direction
  Door_2.write(0);   // opposite direction
  delay(ACTION_DURATION);
  Door_1.write(90);  // stop
  Door_2.write(90);  // stop

  // Linear actuator pushes clothes out
  LinearActuator.write(180); // push
  delay(ACTION_DURATION);
  LinearActuator.write(90);  // stop

  // Fan OFF, Relay ON
  analogWrite(FAN_YELLOW_PIN, 0);
  digitalWrite(RELAY_PIN, HIGH);

  Serial.println("SYSTEM IS OPEN");
}

void closeSystem() {
  // LEDs
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, HIGH);

  // Doors close (full speed opposite)
  Door_1.write(0);   // opposite direction
  Door_2.write(180);
  delay(ACTION_DURATION);
  Door_1.write(90);  // stop
  Door_2.write(90);  // stop

  // Linear actuator retracts clothes
  LinearActuator.write(0);   // retract
  delay(ACTION_DURATION);
  LinearActuator.write(90);  // stop

  // Fan ON, Relay OFF
  digitalWrite(RELAY_PIN, LOW);
  analogWrite(FAN_YELLOW_PIN, FAN_SPEED);

  Serial.println("SYSTEM IS CLOSED");
}
