#pragma once

#include <Servo.h>
#include <random>
#include <time.h>

enum class SkeletonState {
  START = 0,
  MOV_UP = 1,
  FLASH = 2,
  MOV_DOWN = 3,
  SLEEP = 4,
};

class Skeleton {
private:
  static const int upMicros = 1400;
  static const int downMicros = 2300;
  
  int ledPin;
  int baseDelay = 1500;
  int randDelay = 1500;

  SkeletonState state = SkeletonState::START;
  Servo servo;
  int currentMicros = 2300;
  int sleepStart = -1;
  int curDelay = baseDelay;

  static const int flashTotal = 6;
  int flashCount = 0;
  unsigned long flashStart = 0;
  bool flashOn = true;
  static const int flashDuration = 200;
  
  unsigned long lastMove;
  static const unsigned long moveIntervalMs = 50;

public:
  Skeleton() = default;
  Skeleton(int servoPin, int ledPin_) : ledPin(ledPin_) {
    servo.attach(servoPin, 500, 2500);
    pinMode(ledPin_, OUTPUT);
    servo.writeMicroseconds(currentMicros);
    srandom(time(0));
  }

  SkeletonState get_state() const {
    return state;
  }

  void move() {
    auto now = millis();
    if (now - lastMove < moveIntervalMs) {
      return;
    }
    lastMove = now;

    switch (state) {
      case SkeletonState::START:
        digitalWrite(ledPin, HIGH);
        state = SkeletonState::MOV_UP;
        break;

      case SkeletonState::MOV_UP:
        if (currentMicros > upMicros) {
          servo.writeMicroseconds(currentMicros);
          currentMicros -= 20;
        } else {
          state = SkeletonState::FLASH;
        }
        break;

      case SkeletonState::FLASH:
        if (now - flashStart >= flashDuration) {
          if (flashCount < flashTotal) {
            flashCount++;
            flashOn = !flashOn;
            digitalWrite(ledPin, flashOn);
          } else {
            flashCount = 0;
            digitalWrite(ledPin, LOW);
            state = SkeletonState::MOV_DOWN;
          }
          flashStart = now;
        } else {
          delay(50);
        }
        break;

      case SkeletonState::MOV_DOWN:
        if (currentMicros < downMicros) {
          servo.writeMicroseconds(currentMicros);
          currentMicros += 20;
        } else {
          state = SkeletonState::SLEEP;
        }
        break;

      case SkeletonState::SLEEP:
        if (sleepStart == -1) {
          sleepStart = now;
          curDelay = baseDelay + random() % randDelay;
        }
        if (now < sleepStart + curDelay) {
          delay(50);
        } else {
          sleepStart = -1;
          state = SkeletonState::START;
        }
        break;
    }
  }
};