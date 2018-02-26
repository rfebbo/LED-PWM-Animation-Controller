/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#define NUMLEDS 6
#define MAXBRIGHTNESS 255
#define TIMEOUT 2000
#define DELAY 30

int animationTime = 5500;

struct keyFrame_t {
public:
  int time;
  int brightness;
  float velocity;
  keyFrame_t *next;
};

class LED {
public:
  LED() {}
  void init(int p, keyFrame_t *start);
  void update(int t);
  float getBrightness() { return brightness; }
  void print();
  void logFrames();

private:
  void fade();
  void writePin();
  void checkKframe(int t);
  float brightness = 1;
  int pin;
  keyFrame_t *startFrame;
  keyFrame_t *currentKFrame;
};

void LED::init(int p, keyFrame_t *start) {
  pin = p;
  currentKFrame = start;
  startFrame = start;
  pinMode(pin, OUTPUT);
  // logFrames();
}

void LED::fade() { brightness += currentKFrame->velocity; }

void LED::writePin() {
  if (brightness < 0) {
    digitalWrite(pin, LOW);
  } else if (brightness > 255) {
    digitalWrite(pin, HIGH);
  } else {
    analogWrite(pin, brightness);
  };
}

void LED::checkKframe(int t) {
  keyFrame_t *nextFrame;
  if (currentKFrame->next == nullptr)
    nextFrame = startFrame;
  else
    nextFrame = currentKFrame->next;

  if (nextFrame->time == t) {
    brightness = nextFrame->brightness;
    currentKFrame = nextFrame;
  }
}
void LED::logFrames() {
  Serial.print("logging");
  keyFrame_t *kf;
  kf = currentKFrame;
  do {
    Serial.print("Pin: ");
    Serial.println(pin);
    Serial.print("\ttime: ");
    Serial.println(kf->time);
    Serial.print("\tbrightness: ");
    Serial.println(kf->brightness);
    Serial.print("\tvelocity: ");
    Serial.println(kf->velocity);
    kf = kf->next;
  } while (kf != nullptr);
  Serial.print("done");
}

void LED::update(int t) {
  checkKframe(t);
  fade();
  writePin();
}

void LED::print() {
  Serial.print("Pin: ");
  Serial.println(pin);
  Serial.print("\tBrightness: ");
  Serial.println(brightness);
}

keyFrame_t *popFramePointers(keyFrame_t[], int);
LED leds[NUMLEDS];

void setup() {
  Serial.begin(9600);
  // initialize LED digital pin as an output.
  int flashDist = 25;
  keyFrame_t led0frames[] = {{0, 255, 0},
                             {flashDist, 0, 0},
                             {2 * flashDist, 255, 0},
                             {3 * flashDist, 0, 0},
                             {4 * flashDist, 255, 0},
                             {5 * flashDist, 255, 0},
                             {6 * flashDist, 0, 0},
                             {7 * flashDist, 255, 0},
                             {8 * flashDist, 0, 0},
                             {9 * flashDist, 255, 0},
                             {2245, 255, -5}};

  keyFrame_t led1frames[] = {{0, 0, 0}, {flashDist * 9, 0, 1}, {2500, 255, -4}};

  keyFrame_t led2frames[] = {
      {0, 0, 0}, {(flashDist * 9) + 300, 0, 1}, {2755, 255, -3}};

  keyFrame_t led3frames[] = {{0, 0, 0},           {(flashDist * 9) + 600, 0, 1},
                             {3010, 255, -0.25f}, {3865, 255, -0.25f},
                             {4600, 200, -0.25f}, {5400, 150, -0.25f},
                             {5800, 75, -0.25f}};

  keyFrame_t led4frames[] = {{0, 0, 0}, {6810, 0, 0.5f}, {7520, 255, -0.5f}};

  keyFrame_t led5frames[] = {{0, 0, 0}, {8130, 0, 0.5f}, {9400, 255, -0.25f}};

  leds[0].init(10, popFramePointers(led0frames, 11));
  leds[1].init(9, popFramePointers(led1frames, 3));
  leds[2].init(6, popFramePointers(led2frames, 3));
  leds[3].init(11, popFramePointers(led3frames, 7));
  leds[4].init(3, popFramePointers(led4frames, 3));
  leds[5].init(5, popFramePointers(led5frames, 3));
}

void loop() {
  int t = 0;
  while (true) {
    if (t > 12000)
      t = 0;
    for (int i = 0; i < NUMLEDS; i++) {
      leds[i].update(t);
    }
    // leds[0].print();
    delay(1);
    t++;
  }
}

keyFrame_t *popFramePointers(keyFrame_t kfs[], int count) {
  keyFrame_t *previous = nullptr;
  for (int i = count - 1; i >= 0; i--) {
    keyFrame_t *kf = new keyFrame_t();
    kf->brightness = kfs[i].brightness;
    kf->velocity = kfs[i].velocity;
    kf->time = kfs[i].time;
    kf->next = previous;
    previous = kf;
    if (i == 0) {
      return kf;
    }
  }
}
