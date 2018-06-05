// This #include statement was automatically added by the Particle IDE.
#include <AccelStepper.h>

#define LEDR D4
#define LEDG D3

#define ALL_LED 100

const uint8_t led_pins[] = {LEDR,LEDG};

#define HOLECOUNT 2 //How many holes are in gumball machine dispenser plate

#if HOLECOUNT == 1
    #define STEPSTOTAKE 200
#elif HOLECOUNT == 2
    #define STEPSTOTAKE 100 //100 steps
#endif

AccelStepper dispense_stepper(1,3,2);

void setup()
{
    int arraySize = (sizeof(led_pins)/sizeof(*led_pins));
    for(int i=0;i<arraySize;i++){ //Init the pins
        pinMode(i,OUTPUT);
    }

    Particle.function("knock", knock);

    //Particle.function("yes", yes);
    Particle.function("no", no);

    Particle.function("testPasswd", testPasswd);

    LED_state(ALL_LED,LOW); //turn off all LEDs
    dispense_stepper.setMaxSpeed(500);
    dispense_stepper.setSpeed(200);
    dispense_stepper.setAcceleration(150);
    
}

void loop()
{
}

void LED_state(int led,int state){
    int arraySize = (sizeof(led_pins)/sizeof(*led_pins));
    if(led==ALL_LED){
        for(int i=0;i<arraySize;i++){
            digitalWrite(i,state);
        }
    }
    else{
        digitalWrite(led,state);
    }
}

int testPasswd(String password)
{
    Particle.publish("passwd", password, PRIVATE);
    if (password == "Innovate")
    {
        return knock("knock");
    }
    return -1;
}

int no(String command){
    return knock("no");
}

void dispense(){
    dispense_stepper.move(STEPSTOTAKE);
    dispense_stepper.runToPosition();
}

int knock(String command)
{
    //was knockDetector

    if (command == "knock")
    {

        Particle.publish("knock", "knock!", 60, PRIVATE);
        
        LED_state(LEDR,LOW);
        LED_state(LEDG,HIGH);
        //new code
        dispense();

        return 0;
    }
    else if (command == "no")
    {
        Particle.publish("knock", "no!", 60, PRIVATE);
        
        LED_state(LEDR,HIGH);
        LED_state(LEDG,LOW);
        
        return 0;
    }
    else
    {
        return -1;
    }
}