/***************************************************
  This is a library for the CAP1188 I2C/SPI 8-chan Capacitive Sensor

  Designed specifically to work with the CAP1188 sensor from Adafruit
  ----> https://www.adafruit.com/products/1602

  These sensors use I2C/SPI to communicate, 2+ pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  **** THIS LIBRARY ONLY IMPLEMENTS IC2 PROTOCOL ****
 ****************************************************/

#include "Adafruit-CAP1188.h"

uint8_t mySPCR, SPCRback;

Adafruit_CAP1188::Adafruit_CAP1188(int8_t resetpin) {
  // I2C
  _resetpin = resetpin;
  _i2c = true;
}

Adafruit_CAP1188::Adafruit_CAP1188(int8_t cspin, int8_t resetpin) {
  // Hardware SPI
  _cs = cspin;
  _resetpin = resetpin;
  _clk = -1;
  _i2c = false;
}

Adafruit_CAP1188::Adafruit_CAP1188(int8_t clkpin, int8_t misopin,
				   int8_t mosipin,int8_t cspin,
				   int8_t resetpin) {
  // Software SPI
  _cs = cspin;
  _resetpin = resetpin;
  _clk = clkpin;
  _miso = misopin;
  _mosi = mosipin;
  _i2c = false;
}

boolean Adafruit_CAP1188::begin(uint8_t i2caddr) {
  if (_i2c) {
    Wire.begin();

    _i2caddr = i2caddr;
  }

  if (_resetpin != -1) {
    pinMode(_resetpin, OUTPUT);
    digitalWrite(_resetpin, LOW);
    delay(100);
    digitalWrite(_resetpin, HIGH);
    delay(100);
    digitalWrite(_resetpin, LOW);
    delay(100);
  }

  readRegister(CAP1188_PRODID);

  // Useful debugging info

  Serial.print("Product ID: 0x");
  Serial.println(readRegister(CAP1188_PRODID), HEX);
  Serial.print("Manuf. ID: 0x");
  Serial.println(readRegister(CAP1188_MANUID), HEX);
  Serial.print("Revision: 0x");
  Serial.println(readRegister(CAP1188_REV), HEX);

  if ( (readRegister(CAP1188_PRODID) != 0x50) ||
       (readRegister(CAP1188_MANUID) != 0x5D) ||
       (readRegister(CAP1188_REV) != 0x83)) {
    return false;
  }
  // allow multiple touches
  writeRegister(CAP1188_MTBLK,0);//
  // Have LEDs follow touches
  writeRegister(CAP1188_LEDLINK, 0xFF);
  // speed up a bit
  writeRegister(CAP1188_STANDBYCFG, 0x30);

  // we set the sensitivity
  /*writeRegister(CAP1188_SENSITIVITY, 0x2F);
  Serial.print("Sensitivity: 0x");
  Serial.println(readRegister(CAP1188_SENSITIVITY), HEX);*/

  Serial.print("MultiTouch: 0x");
  Serial.println(readRegister(CAP1188_MTBLK), HEX);
  //BIT DECODE for number of samples taken
  Serial.print("bit decode samples taken: 0x");
  Serial.println(readRegister(CAP1188_STANDBYCFG), HEX);

  return true;
}

void Adafruit_CAP1188::setSensitivity(int sensitivity){

  switch (sensitivity) {

    //least sensitive
    case 1:
      writeRegister(CAP1188_SENSITIVITY,0x7);
      break;

    case 2:
      writeRegister(CAP1188_SENSITIVITY,0x6);
      break;

    case 3:
      writeRegister(CAP1188_SENSITIVITY,0x5);
      break;

    case 4:
      writeRegister(CAP1188_SENSITIVITY,0x4);
      break;

    case 5:
      writeRegister(CAP1188_SENSITIVITY,0x3);
      break;

    case 6:
      writeRegister(CAP1188_SENSITIVITY,0x2);
      break;
    //most sensitive
    case 7:
      writeRegister(CAP1188_SENSITIVITY,0x1);
      break;
  }

  //Let's read how sensible the sensor is
  Serial.print("Sensitivity: 0x");
  Serial.println(readRegister(CAP1188_SENSITIVITY), HEX);

}

uint8_t  Adafruit_CAP1188::touched(void) {
  uint8_t t = readRegister(CAP1188_SENINPUTSTATUS);// we call the input status which will return yes and no values for all th elements
  if (t) {
    writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);
  }
  return t;
}

uint8_t Adafruit_CAP1188::touchedAnalog(byte offset){
  uint8_t t = readRegister(CAP1188_ANALOGID + offset);
  if (t) {
    writeRegister(CAP1188_MAIN, readRegister(CAP1188_MAIN) & ~CAP1188_MAIN_INT);
  }
  return t;// we make sure we don't the the 8 byte
}

void Adafruit_CAP1188::LEDpolarity(uint8_t x) {
  writeRegister(CAP1188_LEDPOL, x);
}

/*********************************************************************/

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void) {
  return Wire.read();
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x) {
  Wire.write((uint8_t)x);
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register
*/
/**************************************************************************/
uint8_t Adafruit_CAP1188::spixfer(uint8_t data) {
  if (_clk == -1) {
   //Serial.println("Hardware SPI");
    return SPI.transfer(data);
  } else {
   // Serial.println("Software SPI");
    uint8_t reply = 0;
    for (int i=7; i>=0; i--) {
      reply <<= 1;
      digitalWrite(_clk, LOW);
      digitalWrite(_mosi, data & (1<<i));
      digitalWrite(_clk, HIGH);
      if (digitalRead(_miso))
	reply |= 1;
    }
    return reply;
  }
}

uint8_t Adafruit_CAP1188::readRegister(uint8_t reg) {
  if (_i2c) {
    Wire.beginTransmission(_i2caddr);
    i2cwrite(reg);
    Wire.endTransmission();
    Wire.requestFrom(_i2caddr, 1);//requests the first value of the given address
    return (i2cread());
  }
}


/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void Adafruit_CAP1188::writeRegister(uint8_t reg, uint8_t value) {
  if (_i2c) {
    Wire.beginTransmission(_i2caddr);
    i2cwrite((uint8_t)reg);
    i2cwrite((uint8_t)(value));
    Wire.endTransmission();
  }
}
