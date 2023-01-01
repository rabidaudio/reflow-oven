
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
//      Serial.print(mean_t); Serial.print("\t");
//      Serial.print(mean_v); Serial.print("\t");
//      Serial.print(num); Serial.print("\t");
//      Serial.print(denum); Serial.print("\t");
//      Serial.print("|"); Serial.print("\t");
      return num / denum;
    }
};


Regressor<5> spd;
Regressor<20> accel;

uint32_t start;

void setup() {
  Serial.begin(115200);
  start = millis();
}

void loop() {
  float t = (millis() - start) / 1000.0;
  float r = random(-100, 100) / 100.0;
  float p = t * t * 3.0;
  p += r;
  float v = spd.step(t, p);
  float a = accel.step(t, v);
  Serial.print(t); Serial.print("\t");
  Serial.print(p); Serial.print("\t");
  Serial.print(v); Serial.print("\t");
  Serial.print(a); Serial.print("\t");
//  Serial.print(spd.isSeeded()); Serial.print("\t");
//  Serial.print(accel.isSeeded()); Serial.print("\t");
  Serial.println();
  delay(100);
}
