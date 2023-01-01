#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MAX31865.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);

// PT100 values
#define RREF      430.0
#define RNOMINAL  100.0

static const uint16_t NUMBER_GFX[] PROGMEM = {
    0b0000110000111111, // 0
    0b0000000000000110, // 1
    0b0000000011011011, // 2
    0b0000000010001111, // 3
    0b0000000011100110, // 4
    0b0000000001101101, // 5
    0b0000000011111101, // 6
    0b0000000000000111, // 7
    0b0000000011111111, // 8
    0b0000000011101111, // 9
};

static const uint16_t STATE_GFX[] PROGMEM = {
  0b0100110000000000, // PREHEAT
  0b0100100010000000, // SOAK
  0b0110101000000000, 0b0110101000000000, // REFLOW
  0b0110000100000000, // COOL
};

void printStatus() {
  digitalWrite(10, LOW);
  SPI.transfer(0x00);
  uint8_t conf = SPI.transfer(0x00);
  uint16_t rtd = SPI.transfer16(0x0000);
  uint16_t highThresh = SPI.transfer16(0x0000);
  uint16_t lowThresh = SPI.transfer16(0x0000);
  uint8_t fault = SPI.transfer(0x00);
  digitalWrite(10, HIGH);
  
  Serial.print("conf:"); Serial.print(conf, HEX);
  Serial.print("\trtd:"); Serial.print(rtd);
  Serial.print("\thighresh:"); Serial.print(highThresh);
  Serial.print("\tlowtresh:"); Serial.print(lowThresh);
  Serial.print("\tfault:"); Serial.print(fault, HEX);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  thermo.begin(MAX31865_3WIRE);
  alpha4.begin(0x70);
  alpha4.clear();
//  startTime = millis();
  printStatus();
  float startTemp = thermo.temperature(RNOMINAL, RREF);

  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
}

void loop() {
  float temp = thermo.temperature(RNOMINAL, RREF);

  printStatus();
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
 
  printTemp(temp);
  // printState(state);
  alpha4.writeDisplay();
  delay(100);
}

void printState(size_t s) {
  alpha4.writeDigitRaw(0, pgm_read_word(STATE_GFX + s));
}

void printTemp(float temp) {
  if (temp >= 1000) temp = 999.0;
  if (temp < 0) temp = 0;

  for (int c = 3; c >= 1; c--) {
    size_t i = ((size_t)temp) % 10;
    alpha4.writeDigitRaw(c, pgm_read_word(NUMBER_GFX + i));
    temp -= i;
    temp /= 10;
  }
}
