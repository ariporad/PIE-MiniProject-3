#include <Adafruit_MotorShield.h>

#define STATUS_LED_PIN 13

// On the Adafruit Motor Shield
#define LEFT_MOTOR_PORT 3
#define RIGHT_MOTOR_PORT 4

#define LEFT_IR_PIN A
#define RIGHT_IR_PIN A2

#define NUM_PARAMS 8
#define SERIAL_UPDATE_INTERVAL 100

// There are 8 parameter slots than can be set, each holds an unsigned 8 bit integer
// Currently:
// 0 = base speed
// 1 = adjustment speed
// 2 = current left speed              (read only)
// 3 = current right speed             (read only)
// 4 = current left IR sensor reading  (read only)
// 5 = current right IR sensor reading (read only)
// 6 = IR cuttoff (divided by 4 for size reasons)
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

  // 0 = base speed
  // 1 = adjustment speed
  // 2 = current left speed              (read only)
  // 3 = current right speed             (read only)
  // 4 = current left IR sensor reading  (read only)
  // 5 = current right IR sensor reading (read only)
  // 6 = IR cuttoff (divided by 4 for size reasons)
  // 7 = unused

  // Read from the analog sensors
  int leftIR = parameters[4] = analogRead(LEFT_IR_PIN);
  int rightIR = parameters[5] = analogRead(RIGHT_IR_PIN);
  int cutoffIR = parameters[6];

  int leftSpeed = parameters[0];
  int rightSpeed = parameters[0];

  if (leftIR >= cutoffIR) {
    leftSpeed += parameters[1];
  } else if (rightIR >= cutoffIR) {
    rightSpeed += parameters[1];
  }

	leftMotor->run(FORWARD);
  rightMotor->run(FORWARD);
	leftMotor->setSpeed(leftSpeed);
  rightMotor->setSpeed(rightSpeed);

  parameters[2] = leftSpeed;
  parameters[3] = rightSpeed;

  send_values();
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

int last_serial_write = 0;
void send_values() {
  if (millis() - last_serial_write < SERIAL_UPDATE_INTERVAL) return;

	Serial.write(0xFF);
	Serial.write(0xAA);
  Serial.write(0xFF);
  Serial.write(0xAA);

	for (int i = 0; i < NUM_PARAMS; i++)
	{
		Serial.write(i | (1 << 8));
    Serial.write(parameters[i]);
	}

  last_serial_write = millis();
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
