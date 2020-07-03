#include <NewPing.h>
//Ultrasonic sensor function library. You must install this library

//our L298N control pins
const int LeftMotorForward = 9;
const int LeftMotorBackward = 6;
const int RightMotorForward = 5;
const int RightMotorBackward = 3;


const int InfrasensorLeft = 4;
const int InfrasensorRight= 7;
int ctrBoundary=0;

//Ultrasensor pins
#define trig_pin A1 //analog input 1
#define echo_pin A2 //analog input 2

#define maximum_distance 200
int distance = 100;

//infrared sensor
const int photo_diode = 10;
int inputVal = 0;
const int photo_diode1 = 8;
int inputVal1 = 0;

char data; //Initialized variable to store recieved data from node MCU


NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function


void setup(){

  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  pinMode(InfrasensorLeft,INPUT);
  pinMode(InfrasensorRight,INPUT);
  distance = readPing();
  delay(100);
  Serial.begin(9600);

}

void loop()
{
  if (Serial.available())
  {
   data = Serial.read();
   //Serial.print("data=");
  //Serial.println(data); 
  }

inputVal= digitalRead(photo_diode);
inputVal1= digitalRead(photo_diode1);
  
  if (data=='3')
  {
    //Serial.print("lowbatt");
    lowBattery(); 
  }
 
  else if (data=='1')
  {
    //Serial.print("freenav");
    freeNavigation();  
  }
  else if(data=='2')
  {
    //Serial.print("movestop");
    moveStop();
  }
}

int readPing(){
  delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=250;
  }
  return cm;
}


void lowBattery()
{
  if (inputVal==1 && inputVal1==1){
 // Serial.print("STOP");
          moveStop();
      }
 else if (inputVal==1){
 // Serial.print("rIGHT");
          turnRight();
          delay(100);         
      }
 else if (inputVal1==1){
  //Serial.print("Left");
          turnLeft();
          delay(100);       
      }
 else if (inputVal==0 && inputVal1==0){
   //  Serial.print("Forward");
          moveForward();
      }
  
}


void freeNavigation(){

  //Boundary detection
  
if (inputVal==1){
    Serial.print("boundary");
          moveStop();
          delay(200);
          moveBackward();
         delay(100);
         pathPlanning();
         //turnAround();
         //delay(600);
      }
else if (inputVal1==1){
      Serial.print("boundary2");
           moveStop();
          delay(200);
          moveBackward();
         delay(100);
         pathPlanning();
         //turnAround();
         //delay(600);
      }
      
 //Obstacle detection
      
else if (distance <= 15)
  { 
    moveStop();
    delay(300);
    moveBackward();
    delay(300);
    moveStop();
    delay(300);

    if (digitalRead(InfrasensorLeft)==LOW){
      turnRight();
      delay(400);
      moveStop();
      delay(200);

    }
    else if (digitalRead(InfrasensorRight)==LOW){
       turnLeft();
       delay(400);
       moveStop();
       delay(200);
       }
    else{
       turnRight();
       delay(400);
       moveStop();
       delay(200);
        }
       }
else if (digitalRead(InfrasensorLeft)==LOW){
      turnRight();
      delay(400);
      moveStop();
      delay(200);
      }
else if (digitalRead(InfrasensorRight)==LOW){
       turnLeft();
       delay(400);
       moveStop();
       delay(200);      
      }

    moveForward();
    Serial.print("reached here");
    distance = readPing();
  
}


void moveStop(){
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
}

void moveForward(){
  
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
  
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW); 

}

void moveBackward(){

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
}

void turnRight(){
  
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
}

void turnLeft(){
  
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}
void turnAround(){
  
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

void pathPlanning()
{
 if(ctrBoundary%2==0)
 {
 moveStop();
  delay(200);
  moveBackward();
  delay(100);
  turnRight();
  delay(400);
  moveForward();
  delay(400);
  turnRight();
  delay(400);
  ctrBoundary++;
 } 
 else if(ctrBoundary%2==1)
 {
 moveStop();
  delay(200);
  moveBackward();
  delay(100);
  turnLeft();
  delay(400);
  moveForward();
  delay(400);
  turnLeft();
  delay(400);
  ctrBoundary++;
 }
}
