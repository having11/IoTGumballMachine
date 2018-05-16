#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define SERVOPIN 4

#define SERVO_ZERO 0
#define SERVO_START 90
#define SERVO_END 180

#define PN532_IRQ   (2) //Use these for the shield
#define PN532_RESET (3)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET); //I2C connected shield

Servo gumballServo;

const int servo_sequence[6] = {SERVO_END, SERVO_START, SERVO_END, SERVO_ZERO, SERVO_END, SERVO_ZERO};

void setup() {
  Serial.begin(115200);
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
  gumballServo.attach(SERVOPIN);
  gumballServo.write(SERVO_ZERO);
  delay(100);
  gumballServo.detach();
  
}

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.print("Found an NFC card with UID ");nfc.PrintHex(uid, uidLength);Serial.println("");
    doSomething();
    delay(3000);
  }
}

void doSomething(){ //Put what you want to do once an NFC chip is detected here
  dispense();
}

void dispense(){
  gumballServo.attach(SERVOPIN);
  for(int i=0;i<6;i++){
    gumballServo.write(servo_sequence[i]);
    delay(2000);
  }
  gumballServo.detach();
}

