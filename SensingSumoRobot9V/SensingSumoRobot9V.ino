#include <ZumoBuzzer.h>

#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
 
#define LED 13

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  600 // microseconds
  
// these might need to be tuned for different motor types
#define REVERSE_SPEED     170 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     250
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     200 // ms
#define LITTLE_TURN       100 // ms 

//UltraSonic Pins
//Left Passenger
#define trigPin1 1 
#define echoPin1 0

//Right Driver
#define trigPin2 A1
#define echoPin2 6

int directionToGo;
 
ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
 
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
 
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);

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

int checkUltraSonics()
{
  int duration1, distance1;           // set up variables
  int duration2, distance2;           // set up variables
  
  //Check Left UltraSonic
  digitalWrite(trigPin1, HIGH);      // start of trigger sequence
  delayMicroseconds(1000);          // delay 1ms
  digitalWrite(trigPin1, LOW);       // complete trigger sequence
  duration1 = pulseIn(echoPin1, HIGH);
  
  //Check Right UltraSonic
  digitalWrite(trigPin2, HIGH);      // start of trigger sequence
  delayMicroseconds(1000);          // delay 1ms
  digitalWrite(trigPin2, LOW);       // complete trigger sequence
  duration2 = pulseIn(echoPin2, HIGH);
  
  distance1 = (duration1/2) / 29.1;
  distance2 = (duration2/2) / 29.1;
  
  if (distance1 <= 20 && distance1 >= 0) {  //if left
  return 2; //go left
  } else if ( distance2 <= 20 && distance2 >= 0){  //else if right
  return 0; //go right
  } else {  // else forward
  return 1; //go forward
  }
  
}
 
void setup()
{
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
   
  pinMode(LED, HIGH);
  
  pinMode(trigPin1, OUTPUT);         // set up pin to trigger
  pinMode(echoPin1, INPUT);          // set up pin to receive echo
  
  pinMode(trigPin2, OUTPUT);         // set up pin to trigger
  pinMode(echoPin2, INPUT);          // set up pin to receive echo
  
  waitForButtonAndCountDown();
}

void loop()
{
  if (button.isPressed())
  {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    waitForButtonAndCountDown();
  }
  
  directionToGo = checkUltraSonics();  //Check the ultrasonics

  //Turn in the direction of the target
  if (directionToGo == 0) {
  //Turn Right 
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(LITTLE_TURN); 
  } else if (directionToGo == 2){
  //Turn Left
     motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(LITTLE_TURN);
  }
  
  //motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
   
//Determine if the zumo is at the edge of the ring
  sensors.read(sensor_values);
  
  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensor_values[5] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
   //Go Forward
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}
