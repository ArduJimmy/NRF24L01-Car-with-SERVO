/*
 * Ardujimmy NRF24L01 Transmitter for Arduino Car with Steering SERVO
 * https://youtu.be/l7nzjGBNXnY
 */

#include <SPI.h>
#include <RF24.h>

// --- NRF24L01 ---
RF24 radio(10, 9);  // CE, CSN  
const byte address[6] = "00001";

// --- Joystick Pins ---
#define VRX A0
#define VRY A1
#define SW 4

struct ControlData {
  int x;        // Steering
  int y;        // Throttle
  bool button;  // Optional: LED/Buzzer
} data;

void setup() {
  Serial.begin(9600);
  pinMode(SW, INPUT_PULLUP);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();  // Transmitter
}

void loop() {

  data.x = analogRead(VRX);      // 0–1023
  data.y = analogRead(VRY);      // 0–1023
  data.button = !digitalRead(SW);  // Active LOW

  radio.write(&data, sizeof(data));

  delay(10);
}
