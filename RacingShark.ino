#include <Servo.h>
#include <XBOXRECV.h> //open-source library for the USB Host Shield

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

//instancing of the USB and the xbox receiver 
USB Usb;
XBOXRECV Xbox(&Usb);

//instancing a servo
Servo servo;

//declaring variables for the pins, used for controlling the driver
int LEN = 2; //left enable
int LPWM = 3; //left enable pwm - for controlling the speed 
int REN = 4; //right enable 
int RPWM = 5; //right enable pwm - for controlling the speed

//sets the used pins to the correct state
void setupPins(){
  pinMode(LEN,OUTPUT);
  pinMode(LPWM,OUTPUT);
  pinMode(REN,OUTPUT);
  pinMode(RPWM,OUTPUT);
}

void setup() {
  Serial.begin(115200);
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }

  setupPins(); //setuping pins

  servo.attach(9); //attaching the servo to a pin, i.e - choosing which pin to use to control it 
  servo.write(90); //rotating the servo to 90 degrees, i.e - centering it

  //setting both enables to HIGH so we can control the motors using only the pwm pins
  digitalWrite(LEN,HIGH);
  digitalWrite(REN,HIGH);
}

int angle = 0; //the angle of the servo's rotation
int car_speed = 0; //the speed of the car

void loop() {
  Usb.Task();
  if(Xbox.XboxReceiverConnected){
    //if the receiver is connected we center the servo by setting angle to 90 degrees
    angle = 90;
    if(Xbox.getButtonPress(R2,0)){
      //if the right trigger on the first (of four) controllers has been pressed
      //explanation: because one receiver allows up to 4 Xbox 360 controllers (for co-op play)
      //we need to specify which controller we want to use
      //and because we only have one controller we use the index of '0'
      //when using the getButtonPress() function
      
      car_speed = Xbox.getButtonPress(R2,0); //setting the speed of the car to a value from 0 to 255 based on how far the right trigger has been pressed
      control_speed(car_speed,0);
    }else if(Xbox.getButtonPress(L2,0)){
      //if the left trigger has been pressed, we do the same thing
      car_speed = Xbox.getButtonPress(L2,0);
      control_speed(0,car_speed);   
    }else{
      //if neither of the triggers has been pressed (the car must be idle)
      //we call the control_speed() function with both speeds (one for the speed when the motor is spinning to the right and one for the left) set to '0'
      control_speed(0,0);
    }
    steer(); //checking to see if the robot must steer
    servo.write(angle);
  }else{
    //if the receiver isn't connected, we set the car to be idle, preventing accidents
    control_speed(0,0);
    steer(); //we still want to rotate the wheels even when the car is not moving
  }
}


//the function accepts two parameters - one for the speed of the motor in each direction (right - meaning, the car will go forward, and left - the car will go backwards)
void control_speed(int RPWM_speed, int LPWM_speed){
  //setting both speeds to the PWM pins of the driver using analogWrite()
  //(because Xbox.getButtonPress() returns a value between 0 and 255, we don't need to do any further manipulation with the speed we get from the triggers)
  analogWrite(RPWM,RPWM_speed);
  analogWrite(LPWM,LPWM_speed);
}

//checks to see if the car should steer
void steer(){
  if(Xbox.getAnalogHat(LeftHatX,0)<-7500){
    //if the left analog stick of the controller is pushed to the left (-7500 is the deadzone), the car must steer left
    //then we calculate the angle that the servo must rotate by mapping the deadzone boundaries of the analog stick (from +-32767 to +-7500)
    //to the correct angle
    angle = map(Xbox.getAnalogHat(LeftHatX),-7500,-32767,90,50); //the servo can rotate from 90 (center) to 50 degrees
    //(50 degrees is the maximum value we can use in this instance based on our research)
  }else if(Xbox.getAnalogHat(LeftHatX)>7500){
    //if the left analog stick of the controller is pushed to the right (7500 deadzone), the car must steer right
    angle = map(Xbox.getAnalogHat(LeftHatX),7500,32767,90,130);
    //the angle is mapped from 90 (center) to 130 degrees (again, 130 is the tested limit that we use)
  }else{
    //if the analog stick is not pushed to either side, the car must not steer and the wheels must be straight 
    //so we center the servo (90 degrees)
    angle = 90; 
  }
}
