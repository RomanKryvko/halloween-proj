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
  unsigned long stateStart = -1;
  int curDelay = baseDelay;

  static const int flashTotal = 6;
  int flashCount = 0;
  bool flashOn = true;
  static const int flashDuration = 300;
  
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

  void restart() {
    state = SkeletonState::START;
    curDelay = 0;
    stateStart = -1;
    flashCount = 0;
    flashOn = true;
  }

  void move(int upDur, int downDur, int newBaseDelay, int newRandDelay) {
    auto now = millis();
    if (now - lastMove < moveIntervalMs) {
      return;
    }
    lastMove = now;

    switch (state) {
      case SkeletonState::START: {
        digitalWrite(ledPin, HIGH);
        state = SkeletonState::MOV_UP;
        break;
      }

      case SkeletonState::MOV_UP: {
        const int stepTime = upDur / (downMicros - upMicros);
        if (currentMicros > upMicros) {
          if (now - stateStart >= stepTime) {
            servo.writeMicroseconds(currentMicros);
            currentMicros -= 20;
            stateStart = now;
          } else {
            delay(50);
          }
        } else {
          stateStart = -1;
          state = SkeletonState::FLASH;
        }
        break;
      }

      case SkeletonState::FLASH: {
        if (now - stateStart >= flashDuration) {
          if (flashCount < flashTotal) {
            flashCount++;
            flashOn = !flashOn;
            digitalWrite(ledPin, flashOn);
          } else {
            flashCount = 0;
            digitalWrite(ledPin, LOW);
            stateStart = -1;
            state = SkeletonState::MOV_DOWN;
          }
          stateStart = now;
        } else {
          delay(50);
        }
        break;
      }

      case SkeletonState::MOV_DOWN: {
        const int stepTime = downDur / (downMicros - upMicros);
        if (currentMicros < downMicros) {
           if (now - stateStart >= stepTime) {
            servo.writeMicroseconds(currentMicros);
            currentMicros += 20;
            stateStart = now;
          } else {
            delay(50);
          }
        } else {
          stateStart = -1;
          state = SkeletonState::SLEEP;
        }
        break;
      }

      case SkeletonState::SLEEP: {
        if (stateStart == -1 || newBaseDelay != baseDelay || newRandDelay != randDelay) {
          stateStart = now;
          baseDelay = newBaseDelay;
          randDelay = newRandDelay;
          curDelay = baseDelay + random() % randDelay;
        }
        if (now < stateStart + curDelay) {
          delay(50);
        } else {
          stateStart = -1;
          state = SkeletonState::START;
        }
        break;
      }
    }
  }
};