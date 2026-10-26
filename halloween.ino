#include <Servo.h>
#include <random>
#include <time.h>

Servo servo;
const int start = 2300; // standing
const int fin = 1400;   // lying
const int baseDelay = 1500;
const int randDelay = 1000;

void setup() {
  servo.attach(D4, 500, 2500);
  pinMode(D2, OUTPUT);
  servo.writeMicroseconds(start);
  srandom(time(0));
}

void loop() {
  digitalWrite(D2, HIGH);

  for (int i = start; i > fin; i -= 20) {
    servo.writeMicroseconds(i);
    delay(50);
  }

  for (int i = 0; i < 3; i++) {
    digitalWrite(D2, LOW);
    delay(200);
    digitalWrite(D2, HIGH);
    delay(200);
  }
  digitalWrite(D2, LOW);

  for (int i = fin; i < start; i += 20) {
    servo.writeMicroseconds(i);
    delay(50);
  }

  delay(baseDelay + random() % randDelay);
}
