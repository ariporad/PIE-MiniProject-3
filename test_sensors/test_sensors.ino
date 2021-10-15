#define STATUS_LED_PIN 13

#define LEFT_IR_PIN A2
#define RIGHT_IR_PIN A3

void setup() {
  // Configure and turn off the panic LED, so we can see if we panic later on
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // Setup Serial
  Serial.begin(115200);
  Serial.println("0,01134"); // A hello packet

  // Configure other pins
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);
}

void loop() {
  // Read from the analog sensors
  int leftIR = analogRead(LEFT_IR_PIN);
  int rightIR = analogRead(RIGHT_IR_PIN);

  Serial.print("1,");
  Serial.print(leftIR);
  Serial.print(",");
  Serial.println(rightIR);

  delay(100);
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
