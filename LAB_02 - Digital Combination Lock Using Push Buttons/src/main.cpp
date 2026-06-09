#include <Arduino.h>

const int buttons[] = {9, 10, 11, 12};
const int correctOrder[] = {9, 10, 11, 12};
const int buttonCount = 4;

int enteredOrder[buttonCount];
int enteredCount = 0;

const int greenLed = 5;
const int redLed = 6;
const int statusLed = 13;

void setup() {
  for (int i = 0; i < buttonCount; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }

  pinMode(statusLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
}

void blinkLed(int ledPin) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPin, HIGH);
    delay(300);
    digitalWrite(ledPin, LOW);
    delay(300);
  }
}

bool isCorrectOrder() {
  for (int i = 0; i < buttonCount; i++) {
    if (enteredOrder[i] != correctOrder[i]) {
      return false;
    }
  }

  return true;
}

void loop() {
  digitalWrite(statusLed, HIGH);

  for (int i = 0; i < buttonCount && enteredCount < buttonCount; i++) {
    if (digitalRead(buttons[i]) == LOW) {
      delay(50);

      if (digitalRead(buttons[i]) == LOW) {
        enteredOrder[enteredCount] = buttons[i];
        enteredCount++;

        while (digitalRead(buttons[i]) == LOW) {
          continue;
        }
      }
    }
  }

  if (enteredCount == buttonCount) {
    if (isCorrectOrder()) {
      blinkLed(greenLed);
    } else {
      blinkLed(redLed);
    }

    enteredCount = 0;
  }

  digitalWrite(statusLed, LOW);
}
