#define LEDR D4
#define LEDG D3
#define SERVOPIN D2 //D0 and D1 can be used for I2C. Might want to keep them unused for now

#define ALL_LED 100

//servo positions
#define SERVO_ZERO 0
#define SERVO_START 90
#define SERVO_END 180

const uint8_t led_pins[] = {LEDR,LEDG};

Servo gumballServo;

void setup()
{
    int arraySize = (sizeof(led_pins)/sizeof(*led_pins));
    for(int i=0;i<arraySize;i++){ //Init the pins
        pinMode(i,OUTPUT);
    }
    gumballServo.attach(SERVOPIN);

    Particle.function("knock", knock);

    //Particle.function("yes", yes);
    Particle.function("no", no);

    Particle.function("testPasswd", testPasswd);

    //second was knockDetector
    LED_state(ALL_LED,LOW); //turn off all LEDs
    // gumballServo.attach(servoPin);
    // gumballServo.write(220);
    //was 235 most recently
    //was 360
    
    servo_mv(gumballServo,SERVO_ZERO); //was 25
    delay(100);
    gumballServo.detach();           // Detach the servo when not using it.
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

void servo_mv(Servo& servo, int position){
    servo.write(position);
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
    gumballServo.attach(SERVOPIN);
    servo_mv(gumballServo,SERVO_END); //END was 25
    delay(1000);
    servo_mv(gumballServo,SERVO_START); //START
    delay(1000);
    servo_mv(gumballServo,SERVO_END);
    delay(2000);
    servo_mv(gumballServo,SERVO_ZERO);
    delay(2000);
    servo_mv(gumballServo,SERVO_END);
    delay(2000);
    servo_mv(gumballServo,SERVO_ZERO);
    delay(2000);    
    gumballServo.detach(); // Done with the servo, so detach it.
    
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
        
        gumballServo.attach(SERVOPIN);
        servo_mv(gumballServo,235);
        //was 360
        delay(1000);
        gumballServo.detach();
        return 0;
    }
    else
    {
        return -1;
    }
}
