#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>

int throttleValue = 127;
int steeringValue = 127;
int disconnectCounter = 0;

#define LED 13

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

void waitForButtonAndCountDown()
{
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);
   
  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);  
  delay(1000);
}

int clip(int inVal, int minVal, int maxVal)
{
  if (inVal > maxVal){
    return maxVal;
  }
  if(inVal < minVal){
    return minVal;
  }
  return inVal;  
}

//int motorScale(int inVal)
//{ 
//  if(inVal < 127)
//   {
//      return map(inVal, 0, 127, -255, 0); 
//   } 
//  else
//   {
//      return map(inVal, 127, 255, 0, 255);
//   }
//}

int motorScale(int inVal, int maxVal) 
{
  if(inVal <= 127 || inVal >= -127) 
  {
    if(inVal > 0) 
    {
      return map(inVal, 0, 127, 0, maxVal);
    }
    else 
    { 
      return map(inVal, -127, 0, -maxVal, 0);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (button.isPressed())
  {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    waitForButtonAndCountDown();
  }
  
    if(Serial.available() > 7) {
    int incomingByte = Serial.read();
    if(incomingByte == 0){
      incomingByte = Serial.read();
      if(incomingByte == 0) {
        incomingByte = Serial.read();
        if(incomingByte == 0){
          throttleValue = Serial.read();
          steeringValue = Serial.read();
          disconnectCounter = 0; //Connection Re-established. 
        }
      }
    }
  } else {
    //Bluetooth Disconnect TimeOut Counter
    if (disconnectCounter < 2000) {
      disconnectCounter++;
    } else { 
      disconnectCounter = 2001;
    }
    //When counter has exceeded 7, turn off the motors completely, lost connection
  } 
  
  if (disconnectCounter > 2000) { //BLUETOOTH DISCONNECTED
    //Disable Vehicle
    //Turn Everything off
    motors.setSpeeds(0, 0);    
      
  } else {
    
//    int scaledThrottle = 127 - throttleValue; 
//    int scaledSteering = 127 - steeringValue;   
//    
//    int leftMotor = clip(motorScale(scaledThrottle - scaledSteering), -255, 255);
//    int rightMotor = clip(motorScale(scaledThrottle + scaledSteering), -255, 255);
    
    int scaledThrottle = motorScale(127 - throttleValue, 255); 
    int scaledSteering = motorScale(127 - steeringValue, 200);   

    int leftMotor = clip(scaledThrottle - scaledSteering, -255, 255);
    int rightMotor = clip(scaledThrottle + scaledSteering, -255, 255);
    
    //int scaledThrottle = clip(127 - throttleValue, 0, 255);
    //int scaledSteering = clip(127 - steeringValue, 0, 255); 
    

    
    if(leftMotor < 50 && leftMotor > -50) {
      leftMotor = 0; 
    }
    
    if(rightMotor < 50 && rightMotor > -50) {
      rightMotor = 0; 
    }
    
    motors.setSpeeds(leftMotor, rightMotor);    
    
  }
  
}
