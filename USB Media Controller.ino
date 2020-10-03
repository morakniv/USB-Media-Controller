// Made by E. Selaker - elias@sealker.se 
// Based on code from wolfgang ziegler - (https://wolfgang-ziegler.com/blog/pc-volume-control) with changes made for it to work with my hardware and added functions.  
// Very few comments made by the original author so I've added some to explain what the code is doing. 
// Adding more keys to the code is quite simple as long as they aren't dependent on timing. What I did was to put them in the beginning of the loop. 
// I had a lot of trouble getting my encoder to work as it used a different pinout and method to output the pulses compared to standard rotary encoders.
// I Don't have much experience writing code, an amature at best. Send me message if you find something wrong or code that is poorly written. 


#include <ClickEncoder.h>
#include <Adafruit_NeoPixel.h>
#include <TimerOne.h>
#include <HID-Project.h>

template<typename T>
struct TimeStampedValue {
  explicit TimeStampedValue(T value) : _value(value), _timeStamp(0) {}
  void set(const T& value) { _value = value; touch(); }
  operator const T&() const { return _value; }
  void touch() { _timeStamp = millis(); }
  unsigned long getTimeStamp() const { return _timeStamp; }
  
private:
  T _value;
  unsigned long _timeStamp;
};


#define LIGHT_PIN    10 // Pin connected to NeoPixel
#define VIBRA_PIN    16 // Pin connected to vibration motor, not used in my design
#define NR_OF_PIXELS 24 // NeoPixel 24 LED Ring 
#define TIMEOUT_VIBRA_MS   30 
#define TIMEOUT_LIGHTS_MS 900


const int pinLed = LED_BUILTIN; 
const int pinButton = 2; // Pin connected to tactile switch that ctrl "Play/Pause"

// Setup of colors and configuring NeoPixel strip
Adafruit_NeoPixel strip(NR_OF_PIXELS, LIGHT_PIN, NEO_GRB + NEO_KHZ800); 
const uint32_t RED   = strip.Color(255,   0,   0);
const uint32_t GREEN = strip.Color(  0, 255,   0);
const uint32_t BLUE  = strip.Color(  0,   0, 255);
const uint32_t BLACK = 0;
const uint32_t YELLOW  = strip.Color(  255, 255, 0);


// Setup of the encoder. This can be altered to fit the type of encoder used. 
// Intensity control the level of brightness the LED output. It also determine by how much volume is changed. 
// The amount of brightness is relative to the amount of pulses the encoder registers.
ClickEncoder encoder(A1, A0, A2);
TimeStampedValue<int16_t> value(0);
int16_t current = 0;
int16_t intensity = 0;



void timerIsr() {
  encoder.service();
}

void setup() {

  Serial.begin(9600);

  strip.begin();
  strip.show();

  pinMode(VIBRA_PIN, OUTPUT);  
  pinMode(pinLed, OUTPUT); 
  pinMode(pinButton, INPUT_PULLUP);
  
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  Consumer.begin();
}

void loop() {

    // When pin 2 is pulled HIGH toggle play/pause. 
   if (!digitalRead(pinButton)) { 
    digitalWrite(pinLed, HIGH);
    Consumer.write(MEDIA_PLAY_PAUSE);
    // See HID Project documentation for more Consumer keys
    intensity = 4; 
    volumeChange(MEDIA_PLAY_PAUSE, YELLOW);

    // Simple debounce
    delay(300);
    digitalWrite(pinLed, LOW);
  
}
  current += encoder.getValue();
  auto diff = current - value;
  if (diff != 0) {
    //Serial.print("Encoder Diff: ");    
    //Serial.println(diff);

    // If the value is positive the volume increse and when negative it decrease. 
    if (diff < 0) {
      intensity = max(1, min(intensity + 1, 3));
      volumeChange(MEDIA_VOL_UP, GREEN);      
    } 
    else {
      intensity = min(-1, max(intensity - 1, -3));
      volumeChange(MEDIA_VOL_DOWN, RED);              
    }
    Serial.println(intensity);
    value.set(current);
  }

    // When encoder button is HIGH toggle mute/unmute audio. 
  ClickEncoder::Button b = encoder.getButton();
  if (b != ClickEncoder::Open) {
    Serial.println("Button");
    switch (b) {
      case ClickEncoder::Clicked:
        intensity = 3;
        volumeChange(MEDIA_VOL_MUTE, BLUE);              
        value.touch();
        break;
    }
  }
  else {
    //
    // Turn off LEDs / vibra after certain time of inactivity
    //
    if (millis() - value.getTimeStamp() > TIMEOUT_VIBRA_MS) {
      digitalWrite(VIBRA_PIN, LOW);    
    }
    if (millis() - value.getTimeStamp() > TIMEOUT_LIGHTS_MS) {
      setColor(BLACK);
      intensity = 0;
    }
  }
}

void volumeChange(uint16_t key, uint32_t color) {
  digitalWrite(VIBRA_PIN, HIGH);
  setColor(color); 
  Consumer.write(key);
}

// Addresses every LED on the strip and the level brightness it output. 
void setColor(uint32_t c) {
  strip.setBrightness(abs(intensity) * 255 / 100); 
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

 
