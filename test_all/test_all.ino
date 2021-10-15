#include <Adafruit_MotorShield.h>

#define STATUS_LED_PIN 13

// On the Adafruit Motor Shield
#define LEFT_MOTOR_PORT 3
#define RIGHT_MOTOR_PORT 4

#define LEFT_IR_PIN A1
#define RIGHT_IR_PIN A2

#define NUM_PARAMS 8

// There are 16 parameter slots than can be set, each holds an unsigned 8 bit integer
// Currently:
// 0 = left speed
// 1 = right speed
// 2 = left direction (0 = forward, 1 = reverse)
// 3 = left direction (0 = forward, 1 = reverse)
// 4 = left IR sensor reading
// 5 = right IR sensor reading
// 6 = unused
// 7 = unused
uint8_t parameters[NUM_PARAMS]; 

Adafruit_MotorShield motorShield = Adafruit_MotorShield();
Adafruit_DCMotor *leftMotor = motorShield.getMotor(LEFT_MOTOR_PORT);
Adafruit_DCMotor *rightMotor = motorShield.getMotor(RIGHT_MOTOR_PORT);

void setup() {
	// Configure and turn off the panic LED, so we can see if we panic later on
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.begin(9600);

  // Connect to the Motor Shield
  assert(motorShield.begin(), "Couldn't connect to motor shield!");

  // Configure other pins
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);
}

void loop() {
  receive_commands();

  // Read from the analog sensors
  parameters[4] = analogRead(LEFT_IR_PIN);
  parameters[5] = analogRead(RIGHT_IR_PIN);

  leftMotor->run(parameters[2] ? BACKWARD : FORWARD);
  rightMotor->run(parameters[3] ? BACKWARD : FORWARD);
  leftMotor->setSpeed(parameters[0]);
  rightMotor->setSpeed(parameters[1]);

  send_values();
  delay(100);
}

void receive_commands() {
  // Commands are 3 bytes long: 1 byte of parameter index and 2 bytes of value
	while (Serial.available() >= 3) {
    if (Serial.read() != 0xA0) continue;
    int idx = Serial.read() & 0x0F;
    int value = Serial.read();
    if (Serial.read() != 0xAF) continue;

    assert(idx > 0 && idx < NUM_PARAMS, "illegal index!");
    
		parameters[idx] = value;
	}
}

void send_values() {
	Serial.write(0xFF);
	Serial.write(0xAA);
  Serial.write(0xFF);
  Serial.write(0xAA);

	for (int i = 0; i < NUM_PARAMS; i++)
	{
		Serial.write(i | (1 << 8));
    Serial.write(parameters[i]);
	}
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
//  Serial.print("PANIC: ");
//  Serial.println(message);
  for (int i = 0; i < 16; i++) {
    Serial.write(0xFF);
  }
  digitalWrite(STATUS_LED_PIN, HIGH);

  while(1);
}
