#include "Adafruit-CAP1188.h"
#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define SYSTEM_NUM 0 //0 for the first, 1 for the second
#if SYSTEM_NUM == 0
    #define ACT_NAME "ACTIONBLOCK"
    #define GUM_NAME "GUMBALL"
#else
    #define ACT_NAME "ACTIONBLOCK1"
    #define GUM_NAME "GUMBALL1"
#endif

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
    Particle.publish("init",ACT_NAME,PRIVATE);
    cap.begin();
    cap.setSensitivity(6);
}

void loop() {
    poll_cap();
    if(cap_values[0]==127){ //Eggplant
        Particle.publish(ACT_NAME,"AUTO0", PRIVATE);
        delay(2000);
    }
    else if(cap_values[1]==127){ //teapot
        Particle.publish(ACT_NAME,"AUTO1", PRIVATE);
        delay(2000);
    }
    else if(cap_values[2]==127){ //penny
        Particle.publish(ACT_NAME,"LIGHTCHANGE", PRIVATE);
        delay(2000);
    }
    else if(cap_values[3]==127){ //paperclip
        Particle.publish(ACT_NAME,"PRINT", PRIVATE);
        delay(2000);
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
        Particle.publish(GUM_NAME,"DISPENSE", PRIVATE);
        Particle.publish(ACT_NAME,"TONE",PRIVATE);
        enable_gesture = false;
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        Particle.publish("DIR", "DOWN", PRIVATE);
        Particle.publish(GUM_NAME,"DISPENSE", PRIVATE);
        Particle.publish(ACT_NAME,"TONE",PRIVATE);
        enable_gesture = false;
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        Particle.publish("DIR", "LEFT", PRIVATE);
        Particle.publish(GUM_NAME,"DISPENSE", PRIVATE);
        Particle.publish(ACT_NAME,"TONE",PRIVATE);
        enable_gesture = false;
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        Particle.publish("DIR", "RIGHT", PRIVATE);
        Particle.publish(GUM_NAME,"DISPENSE", PRIVATE);
        Particle.publish(ACT_NAME,"TONE",PRIVATE);
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
