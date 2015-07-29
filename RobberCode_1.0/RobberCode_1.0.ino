#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>

#define LED 13

// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD 700 // microseconds
  
// these might need to be tuned for different motor types
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     250
#define FAST_FORW_SPEED   300 //SMOKIN'
#define REVERSE_DURATION  150 // ms
#define TURN_DURATION     125 // ms
#define LITTLE_TURN       100 // ms 

//UltraSonic Pins
//Left Passenger
#define trigPin1 A2  
#define echoPin1 A3

//Right Driver
#define trigPin2 1
#define echoPin2 0

//Rear
#define trigPin3 A0
#define echoPin3 11

//Front
#define trigPin4 A1
#define echoPin4 6

int sensorDetected = 4;

//Was bot found?
boolean botFound = false;

//Zumo Bot Spotted counter for speed if bot is lost
int botFoundCounter = 0;

//Reflectance Array
int leftpin = 4;
int rightpin = 5;
int lightPin = 2;
 
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

int checkUltraSonics()
{
  int duration1, distance1;           // set up variables
  int duration2, distance2;           // set up variables
  int duration3, distance3;           // set up variables
  int duration4, distance4;           // set up variables
  
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
  
  //Check Rear UltraSonic
  //digitalWrite(trigPin3, HIGH);      // start of trigger sequence
  //delayMicroseconds(1000);          // delay 1ms
  //digitalWrite(trigPin3, LOW);       // complete trigger sequence
  //duration3 = pulseIn(echoPin3, HIGH,5000);

  //Check Front UltraSonic
  digitalWrite(trigPin4, HIGH);      // start of trigger sequence
  delayMicroseconds(1000);          // delay 1ms
  digitalWrite(trigPin4, LOW);       // complete trigger sequence
  duration4 = pulseIn(echoPin4, HIGH);
  
  distance1 = (duration1/2) / 29.1;
  distance2 = (duration2/2) / 29.1;
  //distance3 = (duration3/2) / 29.1;
  distance4 = (duration4/2) / 29.1;
  
  if ((distance4 <= 30) && (distance4 >= 0)) { //Front
    return 0; //Turn Around
	
  //} else if ((distance3 <= 30) && (distance3 >= 0)){  //Rear
  //  return 1; //Go Fast Away

  } else if ((distance1 <= 30) && (distance1 >= 0)) { //Left
    return 2; //Go Left

  } else if ((distance2 <= 30) && (distance2 >= 0)){  //Right
    return 3; //Go Right

  } else { //Shit son you lost, head on forward slowly
    return 4;

  }
}

int readReflectanceSensor(int pin){
  //Lower numbers mean more refleacive
  //More than 3000 means nothing was reflected.
  digitalWrite(lightPin, HIGH);
  pinMode( pin, OUTPUT );
  digitalWrite( pin, HIGH );  
  delayMicroseconds(10);
  pinMode( pin, INPUT );

  long time = micros();

  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  while (digitalRead(pin) == HIGH && micros() - time < 3000); 
  int diff = micros() - time;
  digitalWrite(lightPin, LOW);
  return diff;
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
  
  pinMode(trigPin3, OUTPUT);         // set up pin to trigger
  pinMode(echoPin3, INPUT);          // set up pin to receive echo
          
  pinMode(trigPin4, OUTPUT);         // set up pin to trigger
  pinMode(echoPin4, INPUT);          // set up pin to receive echo
    
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, LOW);
  
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
  
  sensorDetected = checkUltraSonics();  //Check the ultrasonics

  //Turn Away from cop
  if (sensorDetected == 0) { //Front Sensor
  //Turn around
     motors.setSpeeds(-TURN_SPEED*2, TURN_SPEED*2);
    delay(LITTLE_TURN);
    
  } else if (sensorDetected == 2){ // Left
  //Turn Right 
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(LITTLE_TURN); 
    
  } else if (sensorDetected == 3){ // Right
  //Turn Left
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(LITTLE_TURN);
  }
  
//Determine if the zumo is at the edge of the ring
  if (readReflectanceSensor(leftpin) < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (readReflectanceSensor(rightpin) < QTR_THRESHOLD)
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
    /*
   //Go Forward
   if (sensorDetected == 1) {
     motors.setSpeeds(FAST_FORW_SPEED, FAST_FORW_SPEED);
     botFound = true;
     botFoundCounter = 5;
   } 
   */
   /*else {
     if(botFound){
       //if bot was recently found but lost, still go fucking fast, that little cunt might be there still
       botFoundCounter--;
       motors.setSpeeds(FAST_FORW_SPEED, FAST_FORW_SPEED);  
       if(botFoundCounter == 0) {
         //probably lost the bot
         botFound = false;
       }
       */
       
      //else {
       //yeah we lost him, dont waste the battery or go over the edge too fast
       motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED); 
     
   //}
   
    
  }
}
