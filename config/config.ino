#define STATUS_LED_PIN 13

#define NUM_PARAMS 8

// There are 16 parameter slots than can be set, each holds an unsigned 8 bit integer
// Currently, 0 = time on, 1 = time off, all others are unused
uint8_t parameters[NUM_PARAMS]; 

void setup() {
	// Configure and turn off the panic LED, so we can see if we panic later on
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.begin(115200);
}

void loop() {
  // Commands are 3 bytes long: 1 byte of parameter index and 2 bytes of value
	while (Serial.available() >= 2) {
    int idx = Serial.read();
    int value = Serial.read();

    assert(idx > 0 && idx < NUM_PARAMS, "illegal index!");
    
		parameters[idx] = value;
	}

	Serial.write(0xFF);
	Serial.write(0xAA);
  Serial.write(0xFF);
  Serial.write(0xAA);

	for (int i = 0; i < NUM_PARAMS; i++)
	{
		Serial.write(i);
    Serial.write(parameters[i]);
	}

//  delay(10);
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
