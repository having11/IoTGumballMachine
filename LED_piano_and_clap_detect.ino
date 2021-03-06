/* Use a Teensy to analyze the amplitude and frequency of a piano when played. Louder = more pixels lit. Low pitch = blue, 
high pitch = red. The mic breakout board gets connected to 3.3V, GND, and pin A9. The sound detector gets connected to 3.3V, GND, 
and pin 1. The neopixels are connected to 5V, GND, and pin 0.
To change what happens when someone triggers the sound detector, just add your new code into the "clapped" function at the bottom
of the code.
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define SERVOPIN 4

#define SERVO_ZERO 0
#define SERVO_START 90
#define SERVO_END 180

#define COOLDOWNTIME 5000 //5 seconds of cooldown for clapping
#define PIN 0
#define CLAP_PIN 1

#define NUMPIXELS 30 //Number of pixels to show

// GUItool: begin automatically generated code
AudioInputAnalog         adc1(A9);           //xy=164,95
AudioAnalyzePeak         peak1;          //xy=317,123
AudioConnection          patchCord1(adc1, peak1);
AudioAnalyzeNoteFrequency notefreq1;
AudioConnection          patchCord2(adc1, notefreq1);
// GUItool: end automatically generated code

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

Servo gumballServo;

const int servo_sequence[6] = {SERVO_END, SERVO_START, SERVO_END, SERVO_ZERO, SERVO_END, SERVO_ZERO};

void clear_pixels(int pixelNum){
  for(int i=0;i<pixelNum;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
}

void setup() {
  gumballServo.attach(SERVOPIN);
  gumballServo.write(SERVO_ZERO);
  delay(100);
  gumballServo.detach();
  AudioMemory(34);
  Serial.begin(115200);
  notefreq1.begin(.10);
  pixels.begin();
  clear_pixels(30);
  pinMode(CLAP_PIN, INPUT);
  attachInterrupt(CLAP_PIN, clapped, FALLING);
}

// for best effect make your terminal/monitor a minimum of 31 chars wide and as high as you can.

elapsedMillis fps;
elapsedMillis cooldown;

void loop() {
  if (fps > 12) {
    if (peak1.available()) {
      fps = 0;
      int monoPeak = peak1.read() * 30.0;
      //pixelCounter = monoPeak;
      if (notefreq1.available()) {
        float note = notefreq1.read();
        //float prob = notefreq1.probability();
        if(note<3000){
        //Serial.printf("Note: %3.2f | Probability: %.2f | ", note, prob);Serial.println(monoPeak);
        for(int i=0;i<monoPeak;i++){
          int subVal = int(note)/8;
          pixels.setPixelColor(i,pixels.Color(0+subVal,0,255-subVal)); //Gets more red as frequency goes higher
          //pixels.show();
        }
        for(int i=monoPeak;i<NUMPIXELS;i++){
          pixels.setPixelColor(i,pixels.Color(0,0,0));
          //pixels.show();
        }
        pixels.show();
        delay(.5);
        }
    }
    }
  }
}

void clapped(){
  if(cooldown>COOLDOWNTIME){
    cooldown = 0;
    //Code goes here for when someone claps
    dispense();
  }
  
}

void dispense(){
  Serial.println("Dispense");
  gumballServo.attach(SERVOPIN);
  for(int i=0;i<6;i++){
    Serial.print("Step ");Serial.print(i);Serial.print(" Action ");Serial.println(servo_sequence[i]);
    gumballServo.write(servo_sequence[i]);
    delay(500);
  }
  gumballServo.detach();
}
