#include <Servo.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//to do - pune pini mai frumos
//to do - cand locuri = 0 mesaj - adauga maxim locuri

Servo Servo1; 
Servo Servo2;
LiquidCrystal_I2C lcd(0x27,20,4);

//sonare intrare in parcare 
const int trigPin = 9; 
const int echoPin = 10; 

const int trigPin2 = 5;
const int echoPin2 = 6;

//servo intrare
const int servoPin = 3; 

//sonare iesire din parcare
const int trigPin3 = 2;
const int echoPin3 = 4;   //aici

const int trigPin4 = 8;
const int echoPin4 = 11;

//servo iesire
const int servoPin2 = 7;

//semafor
// const int greenLight = 4;
// const int yellowLight = 11;
// const int redLight = 12;

long duration; 
long duration2;
long duration3;
long duration4;

long distance; 
long distance2;
long distance3;
long distance4;

int open_br=0, open_bi = 0;
int parkingSpots = 50;
String command;
void setup() {
  
  // pinMode(greenLight, OUTPUT);
  // pinMode(yellowLight, OUTPUT);
  // pinMode(redLight, OUTPUT);

  Servo1.attach(servoPin); 
  Servo1.write(0);
  Servo2.attach(servoPin2);
  Servo2.write(0);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 

  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 

  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT); 

  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT); 
  

  lcd.init();
  lcd.backlight();

 // digitalWrite(redLight,HIGH);

  Serial.begin(9600); // Starts the serial communication on baud rate 9600
}

long getSensorDistance(int tP, int eP)
{
   digitalWrite(tP, LOW);
   delayMicroseconds(2); 
   digitalWrite(tP, HIGH);
   delayMicroseconds(10); 
   digitalWrite(tP, LOW);

   long time = pulseIn(eP, HIGH);                 
                                                                    
   long dist = (time/2) / 29.1;   
  
  return dist;
}

void loop() {
    if (Serial.available()) {
      command = Serial.readStringUntil('\n');
      command.trim();
    }

    distance = getSensorDistance(trigPin, echoPin);
    Serial.print("distance1: "); Serial.print(distance); Serial.println();
    delay(100);

    distance2 = getSensorDistance(trigPin2, echoPin2);
    Serial.print("distance2: "); Serial.print(distance2); Serial.println();
    delay(100);

    distance3 = getSensorDistance(trigPin3, echoPin3);
    Serial.print("distance3: "); Serial.print(distance3); Serial.println();
    delay(100);

    distance4 = getSensorDistance(trigPin4, echoPin4);
    Serial.print("distance4: "); Serial.print(distance4); Serial.println();
    delay(100);

    if(parkingSpots == 0)
    {
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Nu mai sunt locuri");
    }
    else{
      if(distance < 10 && open_br == 0) //inseamna ca este in fata barierei dar este inchisa bariera
      {
        Serial.println("Car detected at entrance");
        if(command == "Open entrance barrier"){
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("Masina intra");

          open_br = 1; //flag bariera pe 1
          Servo1.write(90);
          delay(2000);
        }
      }

      if(distance > 10 && distance2 > 10 && open_br==1)
      {
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Masina a intrat");

        open_br=0;
        Servo1.write(0);
        delay(2000);

      }

      if(distance3 <10 && open_bi == 0)
      {
        Serial.println("Car detected at exit");
        if(command == "Open exit barrier"){
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("Masina iese"); 

          open_bi=1;
          Servo2.write(90);
          delay(2000);
        }
      }

      if(distance3>10 && distance4>10 && open_bi==1)
      {
        lcd.clear();
        lcd.setCursor(3,0);
        lcd.print("Masina a iesit");

        open_bi = 0;
        Servo2.write(0);
        delay(2000); 
      }
      
    } 