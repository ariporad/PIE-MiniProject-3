#include <Adafruit_MotorShield.h>

// On the Adafruit Motor Shield
#define LEFT_MOTOR_PORT 3
#define RIGHT_MOTOR_PORT 4

#define STATUS_LED_PIN 13

#define LEFT_IR_PIN A1
#define RIGHT_IR_PIN A2

Adafruit_MotorShield motorShield = Adafruit_MotorShield();
Adafruit_DCMotor *leftMotor = motorShield.getMotor(LEFT_MOTOR_PORT);
Adafruit_DCMotor *rightMotor = motorShield.getMotor(RIGHT_MOTOR_PORT);

// State Variables
int speed = 100;
int forward = true;

void setup() {
  // Configure and turn off the panic LED, so we can see if we panic later on
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // Setup Serial
  Serial.begin(115200);
  Serial.println("0,01134"); // A hello packet

  // Connect to the Motor Shield
  assert(motorShield.begin(), "Couldn't connect to motor shield!");

  // Configure other pins
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);
}

void loop() {

  Serial.print("1,");
  Serial.print(speed);
  Serial.print(",");
  Serial.print(leftIR);
  Serial.print(",");
  Serial.println(rightIR);

  if (Serial.available()) {
    int newSpeed = Serial.parseInt();
    if (newSpeed >= -255 && newSpeed <= 255) {
      if (newSpeed < 0) {
        speed = -newSpeed;
        forward = !forward;
      } else {
        speed = newSpeed;
      }
    }
  }
  
  leftMotor->run(forward ? FORWARD : BACKWARD);
  rightMotor->run(forward ? FORWARD : BACKWARD);
  leftMotor->setSpeed(speed);
  rightMotor->setSpeed(speed);
}
/**
 * Panic with message if maybe isn't true.
 */
void assert(bool maybe, const String &message) {
  if (!maybe) {
    panic(message);
  }
}

/**
 * Panic means we've encountered a fatal error.
 * 
 * It sends message over serial, turns on the status LED, and waits for the board to be reset.
 * 
 * NOTE: THIS METHOD NEVER RETURNS
 */
void panic(const String &message) {
  Serial.print("PANIC: ");
  Serial.println(message);
  digitalWrite(STATUS_LED_PIN, HIGH);

  while(1);
}
