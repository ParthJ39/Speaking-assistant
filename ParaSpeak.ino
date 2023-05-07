/* connections: 
 *  LCD - Pin1 - ground 
 *        Pin2 - 5V
 *        Pin3 - E1 of potentiometer
 *        Pin4 - D12
 *        Pin5 - Ground
 *        Pin6 - D11
 *        Pin11 - D5 
 *        Pin12 - D4
 *        Pin13 - D3
 *        Pin14 - D2
 *        Pin15 - 5volts
 *        Pin16 - Ground
 *  
 *  Bluetooth - RXD - D10
 *              TXD - D9
 *              GND - Ground
 *              Vcc - +5v
 *  
 *  Sound sensor - D0 - D4 
 *                 Vcc - +5volts
 *                 GND - Ground
 *                 A0 - A1
 *               
 *  Potentiometer - B1 - +5volts
 *                - E1 - Pin3 LCD
 *                - C1 - ground
 * 
 */

#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
long sum_avg = 0;
int average = 0;
int soundval = 0;
int counter = 0;
const int Sound_sensor = A1;
int preaverage = 0;
int minReading = 170;     // minimum reading needed to count
int minFluctuation = 5;
int i = 0 ;
int flag = 0;
int start_flag = 0;
int start_flag1 = 0;
int user_input = 0;
SoftwareSerial blue(9, 10); // RX | TX 
SoftwareSerial linkSerial(7, 8); // RX, TX
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
    pinMode(Sound_sensor, INPUT);
    pinMode(8,OUTPUT);
    Serial.begin(115200);
    linkSerial.begin(4800);
    while(!Serial)continue;
    blue.begin(9600);
    lcd.begin(20, 4);
    lcd.clear();
    while(user_input < 30){
        lcd.setCursor(0, 0);
        lcd.print("Please provide the");
        lcd.setCursor(4, 1);
        lcd.print("sample by");
        lcd.setCursor(0, 2);
        lcd.print("blowing air in the");
        lcd.setCursor(7,3);
        lcd.print("Sensor");
        user_input = analogRead(Sound_sensor);
    }
   while(start_flag  == 0){
    lcd.clear();
    lcd.print("Sample has been");
    lcd.setCursor(8,1);
    lcd.print("Taken");
    delay(2000);
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("Blow air to start");
    start_flag = 1;
   }
}

void loop() {
  StaticJsonDocument<200> doc; 
  while(i<150){
    soundval = analogRead(Sound_sensor);
    sum_avg = sum_avg + soundval;
    i = i + 1;
   }
   i = 0;
   average = sum_avg/150;

  int fluctuation = (average > preaverage) ? average - preaverage : preaverage - average;

  if(average > 380){
    digitalWrite(13,HIGH);
    delay(5000);
    digitalWrite(13,LOW);
  }

  if ((average > minReading) && (fluctuation > minFluctuation))
  {
    // add to count
    if(counter < 5){
    counter++;
    flag = 1;
    lcd.clear();
    doc["value"] = average;
    serializeJson(doc, linkSerial);
    delay(2000);
    }
    else{
      counter = 0;
      lcd.clear();
    }

  } if(flag == 1){
        if(counter == 1){
          lcd.setCursor(0, 0);
          lcd.print("I want to go to");
          lcd.setCursor(5, 1);
          lcd.print("toilet");
    //      Serial.println("I want to go to toilet");
          if(blue.available()){
          blue.write("I want to go to toilet");
          blue.write("\n");
          flag = 0;
          }
        }
        else if(counter == 2){
          lcd.setCursor(0, 1);
          lcd.print("I am hungry");
          if(blue.available()){
          blue.write("I am hungry");
          blue.write("\n");
          flag = 0;
          }
        }
        else if(counter == 3){
          lcd.setCursor(0, 1);
          lcd.print("Provide me water");
          if(blue.available()){
          blue.write("Provide me water");
          blue.write("\n");
          flag = 0;
          }
        }
        else if(counter == 4){
          lcd.setCursor(0, 0);
          lcd.print("Turn on or");
          lcd.setCursor(5, 1);
          lcd.print("fasten the fan");
          if(blue.available()){
          blue.write("Turn on or fasten the fan");
          blue.write("\n");
          flag = 0;
          }
        }
        else if(counter == 5){
          lcd.setCursor(0, 0);
          lcd.print("Turn off or ");
          lcd.setCursor(5, 1);
          lcd.print("slow the fan");
          if(blue.available()){
          blue.write("Turn off or slow the fan");
           blue.write("\n");
           flag = 0;
          }
        }
        else{
          lcd.setCursor(0, 0);
          lcd.print("Blow again to ");
          lcd.setCursor(5, 1);
          lcd.print("restart");
          if(blue.available()){
          blue.write("Blow again to restart");
          blue.write("\n");
          flag = 0;
            }
          }
  }
  average = 0;
  sum_avg = 0;
  preaverage = average;
  delay(300);
   
}
