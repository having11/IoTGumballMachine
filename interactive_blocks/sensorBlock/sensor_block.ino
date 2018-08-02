#include "Adafruit-CAP1188.h"
#include <Wire.h>
#include <SparkFun_APDS9960.h>

/*
3.3v VCC
GND GND
D0 SDA
D1 SCL

ADD 4.7k pullup resistors to I2C lines for APDS9960!!
*/

//Eggplant -> first automata, teapot -> second automata, penny -> change LED in gumball machine, large paperclip -> print with thermal printer

int cap_values[8] = {};
bool enable_gesture = false;

SparkFun_APDS9960 apds = SparkFun_APDS9960();
Adafruit_CAP1188 cap = Adafruit_CAP1188(); //I2C no reset pin

void setup() {
    if(apds.init()){
        Particle.publish("init", "APDS up", PRIVATE);
    }
    if(apds.enableGestureSensor(false)){
        Particle.publish("init", "Gesture up", PRIVATE);
    }
    cap.begin();
    cap.setSensitivity(6);
}

void loop() {
    poll_cap();
    if(cap_values[0]==127){ //Eggplant
        Particle.publish("ACTIONBLOCK","AUTO0", PRIVATE);
    }
    else if(cap_values[1]==127){ //teapot
        Particle.publish("ACTIONBLOCK","AUTO1", PRIVATE);
    }
    else if(cap_values[2]==127){ //penny
        Particle.publish("GUMBALL","LIGHTCHANGE", PRIVATE);
    }
    else if(cap_values[3]==127){ //paperclip
        Particle.publish("ACTIONBLOCK","PRINT", PRIVATE);
        enable_gesture = true;
    }
    if(enable_gesture){
        while(enable_gesture){
            handleGesture();
        }
        
    }
        
}

void poll_cap(){
    for(int i=0;i<8;i++){
        int8_t analogVal = cap.touchedAnalog(i);
        cap_values[i] = analogVal;
    }
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");
        Particle.publish("DIR", "UP", PRIVATE);
        Particle.publish("GUMBALL","DISPENSE", PRIVATE);
        Particle.publish("ACTIONBLOCK","TONE",PRIVATE);
        enable_gesture = false;
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        Particle.publish("DIR", "DOWN", PRIVATE);
        Particle.publish("GUMBALL","DISPENSE", PRIVATE);
        Particle.publish("ACTIONBLOCK","TONE",PRIVATE);
        enable_gesture = false;
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        Particle.publish("DIR", "LEFT", PRIVATE);
        Particle.publish("GUMBALL","DISPENSE", PRIVATE);
        Particle.publish("ACTIONBLOCK","TONE",PRIVATE);
        enable_gesture = false;
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        Particle.publish("DIR", "RIGHT", PRIVATE);
        Particle.publish("GUMBALL","DISPENSE", PRIVATE);
        Particle.publish("ACTIONBLOCK","TONE",PRIVATE);
        enable_gesture = false;
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        Particle.publish("DIR", "NEAR", PRIVATE);
        break;
      case DIR_FAR:
        Serial.println("FAR");
        Particle.publish("DIR", "FAR", PRIVATE);
        break;
      default:
        Serial.println("NONE");
        Particle.publish("DIR", "NONE", PRIVATE);
    }
  }
}
