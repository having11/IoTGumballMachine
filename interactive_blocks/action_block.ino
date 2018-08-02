// This #include statement was automatically added by the Particle IDE.
#include "pitches.h"

// This #include statement was automatically added by the Particle IDE.
#include "LedControl-MAX7219-MAX7221.h"

#include <Adafruit_Thermal.h>
#include <SdFat.h>

SdFat sd;
File image;
LedControl *led;
char message[64];
int phase=0, messageLength=0;

#define PANELNUM 5 //Number of 8x8 displays
#define SPEED 100
#define BRIGHTNESS 5 //1-15
#define PULSEPIN D0

Adafruit_Thermal printer;

String messageToDisplay = "    Touch the eggplant, teapot, penny, or paperclip.";
const uint8_t data = D4;
const uint8_t load = D3;
const uint8_t myclock = D2;

const int songLength = 10;
int melody[] = {NOTE_F4, NOTE_F4, NOTE_C5,NOTE_C5,NOTE_C5,NOTE_C5, NOTE_F4, NOTE_F4, NOTE_G4, NOTE_A4};
int timings[] = {4,2,8,8,8,8,4,2,4,2};

void setup() {
    Particle.subscribe("ACTIONBLOCK",handler,MY_DEVICES);
    sd.begin(A2,SPI_HALF_SPEED);
    Serial1.begin(19200);
    printer.begin(&Serial1);
    led = new LedControl(data,myclock,load,PANELNUM); //DIN,CLK,CS,HowManyDisplays
    for(int i=0;i<PANELNUM;i++){
        led->shutdown(i,false);
        led->setIntensity(i,BRIGHTNESS);
    }
}

void loop() {
    updateMatrix();
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
        
    }
    else if(String(data)=="AUTO1"){ //move automata 1
        
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
