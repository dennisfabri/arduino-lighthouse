#include "ArduinoSort.h"
#include "LowPower.h"

const int idleDelayMultiplicator = 3;
const int downDelayMultiplicator = 100;

const int leds[] = {3, 5, 6, 9, 10, 11};

const int pinDebug = 2;
const int pinVerbose = 12;

const int photoResistorLimit = 400;

const int length = sizeof(leds) / sizeof(int);

const int ledIncrement = 1;
const int ledDiff = 32;

bool lastStateOn = false;

void setup() {
  Serial.begin(57600);
  
  // Debug
  pinMode(2, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
  // Leds
  for (int x=0;x<length;x++) {
    pinMode(leds[x], OUTPUT);
    digitalWrite(leds[x], HIGH);
  }

  // Photoresistor
  pinMode(A0, INPUT);
}

void loop() {
  if (isDebug()) {
    debug();
  } else {
    bool stateOn = isNight();
    process(stateOn);
  }
}

bool isDebug() {
  return digitalRead(pinDebug) == LOW;
}

bool isVerbose() {
  return digitalRead(pinVerbose) == LOW;
}

void debug() {
  if (isVerbose()) {
    Serial.write('debug()');
  }
  digitalWrite(13, HIGH);
  off();
  delay(1000);
  digitalWrite(13, LOW);
}

void process(bool stateOn) {
  if (stateOn != lastStateOn) {
    if (stateOn) {
      dimUp(leds[0]);
    } else {
      dimDown(leds[0]);
    }
  }
  lastStateOn = stateOn;
  if (stateOn) {
    dimLoop();
  } else {
    off();
    sleep(false);
  }
}

bool isNight() {
  if (isVerbose()) {
    Serial.println('isNight()');
  }
  const int amount = 9;
  const int middle = amount / 2;
  int measure[amount];
  for (int x = 0; x < amount; x++) {
    measure[x] = analogRead(A0);
    delay(1);
    if (isVerbose()) {
      Serial.print(measure[x]);
      Serial.print(' ');
    }
  }
  sortArray(measure, amount);

  if (isVerbose()) {
    Serial.println();
    Serial.print(measure[middle]);
    Serial.print(' <= ');
    Serial.print(photoResistorLimit);
    Serial.println('?');
  }
  
  return measure[middle] <= photoResistorLimit;
}

void off() {
  lastStateOn = false;
  for (int x=0;x<length;x++) {
    digitalWrite(leds[x], HIGH);
  }
}

void dimLoop() {
  for (int x = 0; x < length; x++) {
    int led1 = leds[x];
    int led2 = leds[(x + 1) % length];
    dim(led1, led2);
  }  
}

void dimDown(int led) {
  for (int i = 0 ; i <= ledDiff; i += ledIncrement) {
    analogWrite(led, 1023 - ledDiff - i);
    sleep(true);
  }
}

void dimUp(int led) {
  for (int i = 0 ; i <= ledDiff; i += ledIncrement) {
    analogWrite(led, 1023 - i);
    sleep(true);
  }
}

void dim(int led1, int led2) {  
  for (int i = 0; i <= ledDiff; i += ledIncrement) {
    analogWrite(led1, 1023 - ledDiff + i);
    analogWrite(led2, 1023 - i);
    sleep(true);
  }
}

void sleep(bool isLED) {
  if (isLED) {
    for (int x = 0; x < idleDelayMultiplicator; x++) {
      LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_ON, TIMER1_ON, TIMER0_ON, SPI_OFF, USART0_OFF, TWI_OFF);
    }
  } else {
    for (int x = 0; x < downDelayMultiplicator; x++) {
      //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      delay(1);
    }
  }
}

