/*
 * Ardujimmy NRF24L01 Receiver for Arduino Car with Steering SERVO
 * https://youtu.be/l7nzjGBNXnY
 */
 
 #include <SPI.h>
#include <RF24.h>
#include <Servo.h>

// --- NRF24 ---
RF24 radio(3, 2);   // CE, CSN
const byte address[6] = "00001";

struct ControlData {
  int x;
  int y;
  bool button;
} data;

// --- Servo ---
Servo steering;
#define SERVO_PIN 10

// --- LED & Buzzer ---
#define LED_PIN 9
#define BUZZER_PIN 8

// --- L298N (NO ENA/ENB) ---
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

void setup() {
  Serial.begin(9600);

  // NRF24
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  // Pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  steering.attach(SERVO_PIN);
}

// ----------------------
// MOTOR CONTROL (NO PWM)
// ----------------------
void driveMotors(int throttle) {

  if (throttle > 50) {
    // FORWARD
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } 
  else if (throttle < -50) {
    // BACKWARD
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } 
  else {
    // STOP
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));

    // --- Steering (Servo) ---
    int servoPos = map(data.x, 0, 1023, 45, 135);
    steering.write(servoPos);

    // --- Throttle ---
    int throttle = map(data.y, 0, 1023, 512, -512);
    driveMotors(throttle);

    // --- Button → LED + Buzzer ---
    if (data.button) {
      digitalWrite(LED_PIN, HIGH);
      tone(BUZZER_PIN, 2000);
    } else {
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
    }
  }
}
