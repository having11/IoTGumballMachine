// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

// This #include statement was automatically added by the Particle IDE.
#include <AccelStepper.h>

#define HOLECOUNT 2 //How many holes are in gumball machine dispenser plate

#if HOLECOUNT == 1
    #define STEPSTOTAKE 200
#elif HOLECOUNT == 2
    #define STEPSTOTAKE 100 //100 steps
#endif

#define SYSTEM_NUM 0 //0 for the first, 1 for the second
#if SYSTEM_NUM == 0
    #define NAME "GUMBALL"
#else
    #define NAME "GUMBALL1"
#endif

#define PIXEL_PIN D6
#define PIXEL_COUNT 24
#define PIXEL_TYPE WS2812

AccelStepper dispense_stepper(1,3,2);

int lights_on=0;
int setbrightness=60;
int wait=10; // delay wait ms
int wait_onther_delay=40;

int current_setting = 0; //0-12

enum EffectsMode {
  RAINBOW_MODE = 0,
  CHASE_MODE,  // you could just add other effects modes above BLACK_MODE
  BLACK_MODE,
  WHITE_MODE,
  RED_MODE,
  BLUE_MODE,
  YELLOW_MODE,
  YELLOW_HALF_MODE,
  SPOT,
  JUST_ANOTHER_RAINBOW,
  RAINBOWCYCLE,
  THEATERCHASE,
  THEATERCASERAINBOW,
  MODES_MAX // <<<<< you will see where we are going with this later in the code...
};

EffectsMode effectsMode = RAINBOW_MODE;

// ***** Object-Definition

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

void setup()
{
    Particle.function("setlight", setlight);
    Particle.function("passDis", passDispense);
    Particle.subscribe(NAME,handle_gumball,MY_DEVICES);

    dispense_stepper.setMaxSpeed(500);
    dispense_stepper.setSpeed(200);
    dispense_stepper.setAcceleration(150);
    strip.begin();
    strip.setBrightness(setbrightness);
    strip.show();
}

void loop()
{
    switch (effectsMode)
  {
    case RAINBOW_MODE:
      rainbow(40);    // case 0
      break;
    case CHASE_MODE:
      chase(10);      // case 1
      break;
    case BLACK_MODE:
      setBlack();     // case 2
      break;
    case WHITE_MODE:
      setWhite();     // case 3
      break;
    case RED_MODE:
      setRed();       // case 4
      break;
    case BLUE_MODE:
      setBlue();      // case 5
      break;
    case YELLOW_MODE:
      setYellow();    // case 6
      break;  
    case YELLOW_HALF_MODE:
      setYellow_Half();  // case 7
      break;  
    case SPOT:
      set3spot();     // case 8
      break;  
    case JUST_ANOTHER_RAINBOW:
      just_another_rainbow();     // case 9
      break;  
    case RAINBOWCYCLE:
      rainbowCycle(); // case 10
      break;  
    case THEATERCHASE:
      theaterChase();     // case 11
      break;  
    case THEATERCASERAINBOW:
      theaterChaseRainbow();     // case 12
      break;  
          
    default: 
      rainbow(10);    // if is nothing choose at start
      chase(10);
      break;
  }
}

int setlight(String command)
{
  int newMode = command.toInt();
  if (newMode < static_cast<int>(MODES_MAX) && newMode >= 0) // within the range of options
  {
    effectsMode = static_cast<EffectsMode>(newMode);
  }
  else
  {
    int incrementMode = static_cast<int>(effectsMode);  // outside the range of options
    effectsMode = static_cast<EffectsMode>(++incrementMode % MODES_MAX);
  }
  return static_cast<int>(effectsMode);
}

void handle_gumball(const char *event, const char *data){
    if(String(data)=="LIGHTCHANGE"){ //change lights
        if(current_setting<=12){
            setlight(String(current_setting));
            current_setting++;
        }
        else{
            current_setting=0;
            setlight(String(current_setting));
            current_setting++;
        }
    }
    else if(String(data)=="DISPENSE"){
        dispense();
    }
}

void dispense(){
    setBlue();
    dispense_stepper.move(STEPSTOTAKE);
    dispense_stepper.runToPosition();
    delay(2000);
}

int passDispense(String command)
{
    //was knockDetector

    if (command == "knock")
    {
        dispense();
        return 0;
    }
    else if (command == "no")
    {
        setRed();
        delay(2000);
        return 0;
    }
    else
    {
        return -1;
    }
}

void rainbow(uint8_t wait)
{
  static int lastUpdateMillis = 0;
  static int direction = -1;
  static int pixelIndex = 0;

  if (millis() - lastUpdateMillis >= wait)
  {
    if (pixelIndex <= 0 or pixelIndex >= 255)
    {
      direction *= -1;
    }
    pixelIndex += direction;
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor( i, Wheel((i + pixelIndex) & 255));  // not sure about this...
    }
    strip.show();
    lastUpdateMillis = millis();  //  Whoopsie!!
  }
}

// ***** Case 1

void chase(uint8_t wait)
{
  static int lastUpdateMillis = 0;
  static int direction = -1;
  static int pixelIndex = 0;
  if (millis() - lastUpdateMillis >= wait)
  {
    if (pixelIndex <= 0 or pixelIndex >= strip.numPixels())
    {
      direction *= -1;
    }
    pixelIndex += direction;
    for (int i = 0; i < strip.numPixels(); i++)
    {
     // if (pixelIndex = i) took this out. replaced with line below. 
     if (pixelIndex == i)  // a lot more useful in making comparisons than the single "="
      {
        strip.setPixelColor(i, 255, 255, 255);
      }
      else
      {
        strip.setPixelColor(i, 0, 0, 0);
      }
     }
    strip.show();
    lastUpdateMillis = millis();
 //   pixelIndex++; -- deleted this. 
    
  }
}

// ***** Case 2

void setBlack(void)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor( i, 0, 0, 0);
  }
  strip.show();
}

// ***** Case 3

void setWhite(void)
{
for (int i = 0; i < strip.numPixels(); i++)
{
strip.setPixelColor( i, 255, 255, 255);
}
strip.show();
}

// ***** Case 4

void setRed(void)
{
for (int i = 0; i < strip.numPixels(); i++)
{
strip.setPixelColor( i, 255, 0, 0);
}
strip.show();
}

// ***** Case 5

void setBlue(void)
{
for (int i = 0; i < strip.numPixels(); i++)
{
strip.setPixelColor( i, 0, 0, 255);
}
strip.show();
}

// ***** Case 6

void setYellow(void)
{
for (int i = 0; i < strip.numPixels(); i++)
{
strip.setPixelColor( i, 255, 255, 0);
}
strip.show();
}

// ***** Case 7

void setYellow_Half(void)
{
for (int i = 0; i < strip.numPixels()/2; i++)
{
strip.setPixelColor( i, 255, 255, 0);
}
strip.show();
}

// ***** Case 8

void set3spot(void)
{
      // define which pixel, R, G, B
strip.setPixelColor( 4, 128, 255, 0);
strip.setPixelColor( 7, 100, 20, 5);
strip.setPixelColor( 10, 80, 74, 9);
strip.show();
}

// ***** Case 9

// https://www.tweaking4all.com/hardware/arduino/arduino-ws2812-led/#adafruit_neopixel

void just_another_rainbow(void) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// ***** Case 10

// Slightly different, this makes the rainbow equally distributed throughout

void rainbowCycle(void) {
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// ***** Case 11

//Theatre-style crawling lights.

void theaterChase(void) {
    int c = 200;
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait_onther_delay);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// ***** Case 12

//Theatre-style crawling lights with rainbow effect

void theaterChaseRainbow(void) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
        delay(wait_onther_delay);
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.

uint32_t Wheel(byte WheelPos) {

  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

