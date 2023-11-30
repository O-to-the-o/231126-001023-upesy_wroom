#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>

//Pin Defintions
SoftwareSerial mySerial(22, 23); // RX, TX
#define PIR_PIN     13
#define PHOTON_PIN 25
#define Mosfet 2 //Switches GND for the 5V devices (DFPlayerMini and Soilmoisture Sensor)
#define Moisture_Pin 34

//Sound Files
#define waterStart 101
#define waterEnd 104
#define enoughlightStart 301
#define enoughlightEnd 304
#define moreLightStart 201
#define moreLightEnd 204

DFPlayerMini_Fast myMP3;

//Debugging with SerialPrints
#define DEBUG true
#define Serial if(DEBUG)Serial

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  myMP3.begin(mySerial, true); //Soft Serial for communication with the DF Player Module

  delay(500); // 0.5 second delay for recovery

  Serial.println("awoken");

  pinMode(PIR_PIN, INPUT);  //Den PIR Sensor als Eingang deklarieren
  pinMode(PHOTON_PIN, INPUT);
  pinMode(Mosfet, OUTPUT);
  digitalWrite(Mosfet,1); //Make sure MOSFET is off, Mosfet requires a pulldown to be in a defined state when in DeepSleep
}
 
void loop()
{    
  int did_move = digitalRead(PIR_PIN); //Das Eingangssignal lesen
  if(did_move){
    digitalWrite(Mosfet,1);     // turn 5V with MOSFET on
    delay(650);     
    int soil_moisture_value = analogRead(Moisture_Pin);  //put Sensor insert into soil
    Serial.print("soil: ");
    Serial.println(soil_moisture_value);
    Serial.println("movement detected!");
    float ligth_value = analogRead(PHOTON_PIN);
    Serial.print("light value: ");
    Serial.println(ligth_value);  
    
    if(ligth_value <= 3500){ // not enough ligth, do not disturb
      if(soil_moisture_value >= 3300){ // I need water!
        int mp3_num = random(waterStart, waterEnd);
        Serial.println("play mp3 plaese water!");
        myMP3.playFromMP3Folder(mp3_num);
      }
      else if(ligth_value >= 3000 && ligth_value <= 3500){  // give me more light!
        int dice_roll = random(0, 100);
        Serial.print("dice roll: ");
        Serial.println(dice_roll);
        if(dice_roll > 80){
          int mp3_num = random(moreLightStart, moreLightEnd);
          myMP3.playFromMP3Folder(mp3_num);
        }
      }
    else if(ligth_value <= 500){  // we have super light!
        int mp3_num = random(enoughlightStart, enoughlightEnd);
        myMP3.playFromMP3Folder(mp3_num);
      }
    }
  }

  delay(100); //Wait for the File to start Playing
  while(myMP3.isPlaying()){
    delay(1000); //To reduce Voltage Drops on the 5V rail and noise in the Speaker caused by high MCU Load
  }  //Don't shutdown while Playing

  digitalWrite(Mosfet,0);  // shut down 5V with MOSFET here 
 
  if(did_move){
    esp_sleep_enable_timer_wakeup(14.4E+9); // 4 hours in microseconds
    Serial.println("4h deep sleep");
  }
  else{
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13,1); //1 = High, 0 = Low
    Serial.println("deep sleep, wake up on movement");
 }

esp_deep_sleep_start();
}
