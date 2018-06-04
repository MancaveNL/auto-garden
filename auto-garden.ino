
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

#include <math.h>

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A3;     // Grove - Temperature Sensor connect to A3
const int pinTankSensor = 3;
const int pinPot = A0;
const int pinHumSensor = A1;
const int pinBuzz = 8;
const int pinValve =4;

long waterTimeout = 10000;
long previousWater =0;

boolean valveOpen = false;
long valveTimeout = 1000;

boolean buzz = false;
long buzzTimeout = 1000;
long previousBuzz = 0;

unsigned long currentMillis;

void setup() {
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    Serial.begin(9600);
    pinMode(pinBuzz,OUTPUT);
    pinMode(pinValve,OUTPUT);
}

void loop() {
    currentMillis = millis();

    //dislpay stuff
    int target_hum = (analogRead(pinPot)/1024.*100);
    int actual_hum = (analogRead(pinHumSensor)/1024.*100);
    int tank_level = digitalRead(pinTankSensor);
    float temp = readTemp();
    display(target_hum,actual_hum,temp);
    //check water resevoir

    //buzz if tank is empty
    alarm(tank_level);
    
    
    //Water plant if needed
    water(target_hum,actual_hum);

}

void alarm(boolean t){
    if(t==0&&buzz==false&&currentMillis>(previousBuzz+buzzTimeout)){
      digitalWrite(pinBuzz,HIGH);
      Serial.println("buzzon");
      previousBuzz = currentMillis;

      buzz = true;
    }
    if(t==0&&buzz==true&&currentMillis>(previousBuzz+buzzTimeout)){
      digitalWrite(pinBuzz,LOW);
      Serial.println("buzzoff");

      previousBuzz = currentMillis;
      buzz = false;  
    }
    if(t==1&&buzz==true){
      digitalWrite(pinBuzz,LOW);
      Serial.println("buzzoff");
    }

}

void water(int th, int ah){
    if(th>ah&&currentMillis>(previousWater+waterTimeout)&&valveOpen==false){
      digitalWrite(pinValve,HIGH);
      valveOpen = true;  
      //previousWater = currentMillis;  

      Serial.println("wateron");
    }
    if(valveOpen==true&&currentMillis>(previousWater+waterTimeout+valveTimeout)){
      digitalWrite(pinValve,LOW);
      valveOpen = false;
      previousWater = currentMillis;  
      Serial.println("wateroff");
    }
    
    

}

void display(int th,int ah,float t){
    lcd.setRGB(th, 50, 50);
    lcd.setCursor(0,0);
    lcd.print("target:");
    lcd.print(th);
    lcd.print("%");
    lcd.print(" T:");
    lcd.print(t);
    lcd.print("^C");
    lcd.setCursor(0,1);
    lcd.print("actual:");
    lcd.print(ah);
    lcd.print("%");
    lcd.display();
}

float readTemp() {
    int a = analogRead(pinTempSensor);

    float R = 1023.0/a-1.0;
    R = R0*R;

    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
    delay(200);
    return temperature;
}

