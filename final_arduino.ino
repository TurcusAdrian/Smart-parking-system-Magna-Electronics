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
const int greenLight = A3;
const int yellowLight = A2;
const int redLight = A1;

long duration; 
long duration2;
long duration3;
long duration4;

long en_distance1; 
long en_distance2;
long ex_distance1;
long ex_distance2;

int open_entrance=0, open_exit = 0;
int parkingSpots = 50;
String command;

static int is_first_time = 0;
static bool trigger_rsp_b = false;


enum EntranceState {
    WAITING_FOR_CAR,
    CAR_DETECTED,
    OPEN_BARRIER
};

enum ExitState {
    WAITING_FOR_EXIT_CAR,
    EXIT_CAR_DETECTED,
    OPEN_EXIT_BARRIER
};

EntranceState entranceState = WAITING_FOR_CAR;
ExitState exitState = WAITING_FOR_EXIT_CAR;

unsigned long carDetectedTime = 0;
unsigned long exitCarDetectedTime = 0;
const unsigned long timeoutDuration = 20000; // 10 seconds

void setup() {
  
  pinMode(greenLight, OUTPUT);
  pinMode(yellowLight, OUTPUT);
  pinMode(redLight, OUTPUT);

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

  analogWrite(redLight,HIGH);

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
    } else {
        command = "";
    }

    en_distance1 = getSensorDistance(trigPin, echoPin);
    delay(100);

    en_distance2 = getSensorDistance(trigPin2, echoPin2);
    delay(100);

    ex_distance1 = getSensorDistance(trigPin3, echoPin3);
    delay(100);

    ex_distance2 = getSensorDistance(trigPin4, echoPin4);
    delay(100);

    if (parkingSpots == 0) {
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Nu mai sunt locuri");
    } else {
        unsigned long currentTime = millis();
        // Entrance state machine
        switch (entranceState) {
            case WAITING_FOR_CAR:
                if (en_distance1 < 10 && open_entrance == 0) { // Car detected at entrance
                    Serial.println("Car detected at entrance");
                    entranceState = CAR_DETECTED;
                    carDetectedTime = currentTime;
                }
                break;

            case CAR_DETECTED:
                if (command == "Open entrance barrier") {
                    entranceState = OPEN_BARRIER;
                } else if (currentTime - carDetectedTime > timeoutDuration) {
                    // Timeout: revert to waiting for car state
                    entranceState = WAITING_FOR_CAR;
                }
                break;

            case OPEN_BARRIER:
                analogWrite(redLight, LOW);
                analogWrite(yellowLight, HIGH);
                delay(500);
                analogWrite(yellowLight, LOW);
                analogWrite(greenLight, HIGH);
                lcd.clear();
                lcd.setCursor(3, 0);
                lcd.print("Masina intra");

                open_entrance = 1;
                Servo1.write(90);
                delay(2000);

                if (en_distance1 > 10 && en_distance2 > 10 && open_entrance == 1) { // Car has entered
                    
                    analogWrite(greenLight, LOW);
                    lcd.clear();
                    lcd.setCursor(3, 0);
                    lcd.print("Masina a intrat");

                    open_entrance = 0;
                    Servo1.write(0);
                    delay(2000);

                    entranceState = WAITING_FOR_CAR;
                }
                break;
        }

        // Exit state machine
        switch (exitState) {
            case WAITING_FOR_EXIT_CAR:
                if (ex_distance1 < 10 && open_exit == 0) { // Car detected at exit
                    Serial.println("Car detected at exit");
                    exitState = EXIT_CAR_DETECTED;
                    exitCarDetectedTime = currentTime;
                }
                break;

            case EXIT_CAR_DETECTED:
                if (command == "Open exit barrier") {
                    exitState = OPEN_EXIT_BARRIER;
                } else if (currentTime - exitCarDetectedTime > timeoutDuration) {
                    // Timeout: revert to waiting for car state
                    exitState = WAITING_FOR_EXIT_CAR;
                }
                break;

            case OPEN_EXIT_BARRIER:
                lcd.clear();
                lcd.setCursor(3, 0);
                lcd.print("Masina iese");

                open_exit = 1;
                Servo2.write(90);
                delay(2000);

                if (ex_distance1 > 10 && ex_distance2 > 10 && open_exit == 1) { // Car has exited
                    lcd.clear();
                    lcd.setCursor(3, 0);
                    lcd.print("Masina a iesit");

                    open_exit = 0;
                    Servo2.write(0);
                    delay(2000);

                    exitState = WAITING_FOR_EXIT_CAR;
                }
                break;
        }
    }
}