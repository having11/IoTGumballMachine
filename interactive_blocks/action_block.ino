// This #include statement was automatically added by the Particle IDE.
#include "pitches.h"

// This #include statement was automatically added by the Particle IDE.
#include "LedControl-MAX7219-MAX7221.h"

#include <Adafruit_Thermal.h>
#include <SdFat.h>

Servo wheel_spin_servo;
Servo detour_servo;

SdFat sd;
File image;
LedControl *led;
char message[64];
int phase=0, messageLength=0;

#define SYSTEM_NUM 0 //0 for the first, 1 for the second
#if SYSTEM_NUM == 0
    #define NAME "ACTIONBLOCK"
#else
    #define NAME "ACTIONBLOCK1"
#endif

#define WHEEL_STOP 88
#define WHEEL_RIGHT 180
#define WHEEL_SERVO_PIN D0
#define DETOUR_INITIAL 90
#define DETOUR_FINAL 280
#define DETOUR_PIN D1
#define DETOUR_DELAY 1000

#define PANELNUM 8//Number of 8x8 displays
#define SPEED 20
#define BRIGHTNESS 5 //1-15
#define PULSEPIN WKP

Adafruit_Thermal printer;

String messageToDisplay = "    Touch the eggplant, teapot, penny, or paperclip.";
const uint8_t data = D4;
const uint8_t load = D3;
const uint8_t myclock = D2;

const int songLength = 10;
int melody[] = {NOTE_F4, NOTE_F4, NOTE_C5,NOTE_C5,NOTE_C5,NOTE_C5, NOTE_F4, NOTE_F4, NOTE_G4, NOTE_A4};
int timings[] = {4,2,8,8,8,8,4,2,4,2};

void setup() {
    Particle.subscribe(NAME,handler,MY_DEVICES);
    sd.begin(A2,SPI_HALF_SPEED);
    Serial1.begin(19200);
    printer.begin(&Serial1);
    led = new LedControl(data,myclock,load,PANELNUM); //DIN,CLK,CS,HowManyDisplays
    for(int i=0;i<PANELNUM;i++){
        led->shutdown(i,false);
        led->setIntensity(i,BRIGHTNESS);
    }
    init_servos();
    pinMode(PULSEPIN,OUTPUT);
}

void loop() {
    updateMatrix();
}

void moveDetour(int oscillations){
    detour_servo.attach(DETOUR_PIN);
    detour_servo.write(DETOUR_INITIAL);
    for(int i=0;i<oscillations;i++){
        detour_servo.write(DETOUR_FINAL);
        delay(DETOUR_DELAY);
        detour_servo.write(DETOUR_INITIAL);
        delay(DETOUR_DELAY);
    }
    detour_servo.detach();
}

void moveWheel(int duration){
    wheel_spin_servo.attach(WHEEL_SERVO_PIN);
    wheel_spin_servo.write(WHEEL_RIGHT);
    delay(duration);
    wheel_spin_servo.write(WHEEL_STOP);
    wheel_spin_servo.detach();
}

void init_servos(){
    wheel_spin_servo.attach(WHEEL_SERVO_PIN);
    wheel_spin_servo.write(WHEEL_STOP);
    detour_servo.attach(DETOUR_PIN);
    detour_servo.write(DETOUR_INITIAL);
}

void updateMatrix(){
    if(phase==0){ //Message-loop starts
        sprintf(message,messageToDisplay); //update message
        messageLength = strlen(message); // ...and length
        for(int i=0;i<PANELNUM;i++){
            led->setLetter(i,message[phase+i]);
        }
       delay(SPEED);
    }

    if(phase<messageLength){ //next characters except last one
        for(int i=0;i<PANELNUM;i++){
            led->setLetter(i,message[phase+i+1]);
        }
        delay(SPEED);
        phase++;
    }else if(phase==messageLength){//last character scrolls to empty
        for(int i=0;i<PANELNUM;i++){
            led->setLetter(i,' ');
        }
        delay(SPEED);
        phase = 0; //restart message-loop
    }
}

void handler(const char *event, const char *data){
    if(String(data)=="PRINT"){
        Particle.publish("printing","printing",PRIVATE);
        print_receipt();
    }
    else if(String(data)=="AUTO0"){ //move automata 0
    Particle.publish("action","moving auto0",PRIVATE);
        moveWheel(5000); //move wheel automata for 5 secs
    }
    else if(String(data)=="AUTO1"){ //move automata 1
        moveDetour(5); //move detour automata 5 times back and forth
    }
    else if(String(data)=="TONE"){
        playMusic();
    }
}

void playMusic(){
    for(int i=0;i<songLength;i++){
        int noteDuration = 1000 / timings[i];
        tone(PULSEPIN,melody[i],noteDuration);
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(8);
    }
}

void print_receipt(){
    image = sd.open("IMAGE0.BMP",O_READ);
    printer.setSize('M');
    printer.println("Welcome to District Hall!");
    printer.feed(1);
    printer.printBitmap(128,102,dynamic_cast<Stream*>(&image));
    printer.println("Wave your hand over the wave");
    image = sd.open("WAVE.bmp",O_READ);
    printer.feed(1);
    printer.printBitmap(128,27,dynamic_cast<Stream*>(&image));
    printer.println("To get a gumball and change the color.");
    printer.feed(4);
    printer.setDefault();
}
