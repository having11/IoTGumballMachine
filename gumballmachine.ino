#include <Servo.h>

int ledR = D4;
int ledG = D3;
int servoPin = D0;

Servo gumballServo;

void setup()
{
    gumballServo.attach(servoPin);
    pinMode(ledR, OUTPUT);
    pinMode(ledG, OUTPUT);
    pinMode(servoPin, OUTPUT);

    Particle.function("knock", knock);

    Particle.function("yes", yes);
    Particle.function("no", no);

    Particle.function("testPasswd", testPasswd);

    //second was knockDetector

    digitalWrite(ledR, LOW);
    digitalWrite(ledG, LOW);
    // gumballServo.attach(servoPin);
    // gumballServo.write(220);
    //was 235 most recently
    //was 360

    gumballServo.attach(servoPin);   // Initialize the servo.
  gumballServo.write(0);//was 25
  delay(50);
  gumballServo.detach();           // Detach the servo when not using it.



}

void loop()
{
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

int yes(String command)
{
    return knock("knock");
}

int no(String command)
{
    return knock("no");
}

int knock(String command)
{
    //was knockDetector

    if (command == "knock")
    {

        Particle.publish("knock", "knock!", 60, PRIVATE);

        digitalWrite(ledR, LOW);
        digitalWrite(ledG, HIGH);
        gumballServo.attach(servoPin);
        //new code
 gumballServo.write(180); //END was 25
delay(1000);
gumballServo.write(90); //START

delay(1000);
gumballServo.write(180);
delay(2000);
gumballServo.write(0);
delay(2000);
gumballServo.write(180);
delay(2000);
gumballServo.write(0);
////end////

  delay(2000);    
  gumballServo.detach();             // Done with the servo, so detach it.

        return 0;
    }
    else if (command == "no")
    {
        Particle.publish("knock", "no!", 60, PRIVATE);

        digitalWrite(ledR, HIGH);
        digitalWrite(ledG, LOW);
        gumballServo.attach(servoPin);
        gumballServo.write(235);
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
