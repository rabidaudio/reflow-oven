#include <Adafruit_MAX31865.h>
#include <Adafruit_MAX31865.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define USE_TIMER_1 true
#include "TimerInterrupt.h"

Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

#define RREF      430.0
#define RNOMINAL  100.0

uint32_t startTime;
size_t state = 1;
//float stateStart;
//float stateStartTemp;

template <size_t N>
class Regressor {
  private:
    float times[N];
    float values[N];
    size_t index = 0;
    bool seeded = false;
  public:
    bool isSeeded() {
      return seeded;
    }
  
    float step(float t, float v) {
      if (!seeded && index == 0) {
        for (size_t i = 0; i < N; i++) {
          times[i] = 0;
          values[i] = 0;
        }
      }
      double mean_t = 0, mean_v = 0;
      times[index] = t;
      values[index] = v;
      index = (index + 1) % N;
      if (!seeded && index == 0) {
        seeded = true;
      }
      for (size_t i = 0; i < N; i++) {
        mean_t += times[i];
        mean_v += values[i];
      }
      mean_t /= N;
      mean_v /= N;
      double num = 0, denum = 0;
      for (size_t i = 0; i < N; i++) {
        float delta_t = times[i] - mean_t;
        float delta_v = values[i] - mean_v;
        num += delta_t * delta_v;
        denum += delta_t * delta_t;
      }
      return num / denum;
    }
};

Regressor<10> rate;
Regressor<50> accel;

void setup() {
  Serial.begin(115200);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  thermo.begin(MAX31865_3WIRE);
  alpha4.begin(0x70);
  alpha4.clear();
  startTime = millis();

  float a; // && !(a > -0.25 && a < 0.25)
  while (!(rate.isSeeded() && accel.isSeeded())) {
    float temp = thermo.temperature(RNOMINAL, RREF);
    float t = (millis() - startTime) / 1000.0;
    float v = rate.step(t, temp);
    a = accel.step(t, v);
//    stateStart = t;
//    stateStartTemp = temp;
    delay(100);
  }

  ITimer1.init();
  if (! ITimer1.attachInterruptInterval(32, timerTick)) {
    Serial.println("Unable to initialize timer");
    while (true) ;;
  }
}

volatile bool on = false;
volatile uint8_t pwm = 255;
volatile uint8_t timerStep = 0;

void loop() {
  float temp = thermo.temperature(RNOMINAL, RREF);
  float t = (millis() - startTime) / 1000.0;
  float v = rate.step(t, temp);
  float a = accel.step(t, v);
  
  printTemp(temp);
  alpha4.writeDisplay();

//  if (state == 1) {
//    float expectedTemp = temp + (v + (a * 10)) * 10;
//    if (expectedTemp > 150.0) {
//      state = 2;
//      pwm = 102;
//    }
//  } else if (state == 2) {
//    float expectedTemp = temp + (v + (a * 10)) * 10;
//    if (expectedTemp > 180.0) {
//      state = 3;
//      pwm = 255;
//    }
//  } else if (state == 3) {
//    float expectedTemp = temp + (v + (a * 10)) * 10;
//    if (expectedTemp > 225.0) {
//      state = 4;
//      pwm = 0;
//    }
//  }
  
  Serial.print(t); Serial.print("\t");
//  Serial.print(state); Serial.print("\t");
//  Serial.print(on ? '1' : '0'); Serial.print("\t");
//  Serial.print(pwm); Serial.print("\t");
  Serial.print(temp); Serial.print("\t");
  Serial.print(v); Serial.print("\t");
  Serial.print(a); Serial.print("\t");
  Serial.println();

//  if (Serial.available()) {
//    char cmd = Serial.read();
//    if (cmd == 'y') {
//      on = true;
//      digitalWrite(9, HIGH);
//    } else if (cmd == 'n') {
//      on = false;
//      digitalWrite(9, LOW);
//    }
//  }
  delay(20);
}

void timerTick(void) {
  timerStep++;
  if (pwm != 255 && timerStep >= pwm) {
    digitalWrite(9, LOW);
    on = false;
  } else {
    digitalWrite(9, HIGH);
    on = true;
  }
}

void printTemp(float temp) {
  if (temp >= 1000) temp = 999.0;
  if (temp < 0) temp = 0;

  for (int c = 3; c >= 1; c--) {
    size_t i = ((size_t)temp) % 10;
    alpha4.writeDigitAscii(c, '0' + i);
    temp -= i;
    temp /= 10;
  }
}
