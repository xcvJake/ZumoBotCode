//Code for the QRE1113 Digital board
//Outputs via the serial terminal - Lower numbers mean more reflected
//3000 or more means nothing was reflected.

int leftpin = 4;
int rightpin = 5;
int lightPin = 2;


void setup(){
  Serial.begin(9600);
  pinMode(lightPin, OUTPUT);
  digitalWrite(lightPin, LOW);
}


void loop(){

  int Left_Value = readReflectanceSensor(leftpin);
  int Right_Value = readReflectanceSensor(rightpin);
  Serial.print("left: ");
  Serial.println(Left_Value); 
  Serial.print("right: ");
  Serial.println(Right_Value);
  
  delay(100);
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
