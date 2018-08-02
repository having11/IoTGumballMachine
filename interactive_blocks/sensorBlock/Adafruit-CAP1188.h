#ifdef SPARK
 #include "application.h"
#endif

// The default I2C address
#define CAP1188_I2CADDR 0x29
// Wire a cable from the 3V to the AD pin in order to pass this address to the constructor
#define CAP1188_I2C2NDADDR 0x28

// Some registers we use
#define CAP1188_SENINPUTSTATUS 0x3
#define CAP1188_MTBLK 0x2A
#define CAP1188_LEDLINK 0x72
#define CAP1188_PRODID 0xFD
#define CAP1188_MANUID 0xFE
#define CAP1188_STANDBYCFG 0x41
#define CAP1188_REV 0xFF
#define CAP1188_MAIN 0x00
#define CAP1188_MAIN_INT 0x01
#define CAP1188_LEDPOL 0x73

//newly added
#define CAP1188_ANALOGID 0x10// returns analog values
#define CAP1188_SENSITIVITY 0x42// controls standby sensitivity
#define CAP188_SENSITIVYCONTROL 0x1F// controls sensitivity as well

class Adafruit_CAP1188 {
 public:
  // Software SPI
  Adafruit_CAP1188(int8_t clkpin, int8_t misopin,
       int8_t mosipin,int8_t cspin,
       int8_t resetpin);
  // Hardware SPI
  Adafruit_CAP1188(int8_t cspin, int8_t resetpin);
  // Hardware I2C
  Adafruit_CAP1188(int8_t resetpin = -1);

  boolean begin(uint8_t i2caddr = CAP1188_I2CADDR);
  uint8_t readRegister(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t value);
  uint8_t touched(void);
  uint8_t touchedAnalog(byte offset);
  void LEDpolarity(uint8_t x);
  void setSensitivity(int sensitivity);

 private:
  uint8_t spixfer(uint8_t x);
  boolean _i2c;
  int8_t _i2caddr, _resetpin, _cs, _clk, _mosi, _miso;

};
