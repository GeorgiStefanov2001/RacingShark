#include <Servo.h>
  #include <XBOXRECV.h>
  
  #ifdef dobogusinclude
  #include <spi4teensy3.h>
  #endif
  #include <SPI.h>
  
  USB Usb;
  XBOXRECV Xbox(&Usb);
  
  Servo servo;
  
  int LEN = 2;
  int LPWM = 3;
  int REN = 4;
  int RPWM = 5;
  
  void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    if (Usb.Init() == -1) {
      Serial.print(F("\r\nOSC did not start"));
      while (1); // Halt
    }
  
    pinMode(LEN,OUTPUT);
    pinMode(LPWM,OUTPUT);
    pinMode(REN,OUTPUT);
    pinMode(RPWM,OUTPUT);
   
    servo.attach(9);
    servo.write(90);
  
    digitalWrite(LEN,HIGH);
    digitalWrite(REN,HIGH);
  }
  
  int angle = 0;
  int car_speed = 0;
  
  void loop() {
    Usb.Task();
    if(Xbox.XboxReceiverConnected){
      angle = 90;
      if(Xbox.getButtonPress(R2,0)){
        car_speed = Xbox.getButtonPress(R2,0);
        control_speed(car_speed,0);
      }else if(Xbox.getButtonPress(L2,0)){
        car_speed = Xbox.getButtonPress(L2,0);
        control_speed(0,car_speed);   
      }else{
        control_speed(0,0);
      }
      turn();
      servo.write(angle);
    }else{
      control_speed(0,0);
      turn();
    }
  }
  
  
  void control_speed(int RPWM_speed, int LPWM_speed){
    analogWrite(RPWM,RPWM_speed);
    analogWrite(LPWM,LPWM_speed);
  }
  
  void turn(){
    if(Xbox.getAnalogHat(LeftHatX,0)<-7500){
        //turning left
        angle = map(Xbox.getAnalogHat(LeftHatX),-7500,-32767,90,50);
      }else if(Xbox.getAnalogHat(LeftHatX)>7500){
        //turning right
        angle = map(Xbox.getAnalogHat(LeftHatX),7500,32767,90,130);
      }else{
        angle = 90;
      }
  }
