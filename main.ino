#include <Servo.h>

Servo servo;
int pos = 135;
const int fin = 225;

void setup() {
  servo.attach(D4, 500, 2500);
  pinMode(D2, OUTPUT);
}

void loop() {
  digitalWrite(D2, HIGH);
  servo.writeMicroseconds(1400);
  delay(1500);
  for (int i = 0; i < 3; i++) {
    digitalWrite(D2, LOW);
    delay(200);
    digitalWrite(D2, HIGH);
    delay(200);
  }
  digitalWrite(D2, LOW);
  servo.writeMicroseconds(2400);
  delay(1500);
}
