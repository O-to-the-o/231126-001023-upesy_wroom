#include <FastLED.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>


SoftwareSerial mySerial(22, 23); // RX, TX
#define PIR_PIN     13
#define PHOTON_PIN 25
#define Mosfet 2 

DFPlayerMini_Fast myMP3;


void setup() {

  Serial.begin(115200);
  mySerial.begin(9600);
  myMP3.begin(mySerial, true);

  delay(500); // 0.5 second delay for recovery

  Serial.println("awoken");

  pinMode(PIR_PIN, INPUT);  //Den PIR Sensor als Eingang deklarieren
  pinMode(PHOTON_PIN, INPUT);
  pinMode(Mosfet, OUTPUT);
  digitalWrite(Mosfet,0);
}


uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{    
  digitalWrite(Mosfet,1);
  delay(650);
  int soil_moisture_value = analogRead(34);  //put Sensor insert into soil
  int did_move = digitalRead(PIR_PIN); //Das Eingangssignal lesen

  Serial.print("soil: ");
  Serial.println(soil_moisture_value);

  if(did_move){

    // turn 5V with MOSFET on

    Serial.println("movement detected!");
    float ligth_value = analogRead(PHOTON_PIN);
    Serial.print("light value: ");
    Serial.println(ligth_value);  
    
    if(ligth_value <= 3500){ // not enough ligth, do not disturb
      if(soil_moisture_value >= 3300){ // I need water!
        int mp3_num = random(1, 3);
        Serial.println("play mp3 plaese water!");
        myMP3.playFromMP3Folder(mp3_num);
        delay(5000);
      }
      else if(ligth_value >= 3000 && ligth_value <= 3500){  // give me more light!
        int dice_roll = random(0, 100);
        Serial.print("dice roll: ");
        Serial.println(dice_roll);
        if(dice_roll > 80){
          int mp3_num = random(6, 7);
          myMP3.playFromMP3Folder(mp3_num);
          delay(5000);
        }
      }
    else if(ligth_value <= 500){  // we have super light!
        int mp3_num = random(4, 5);
        myMP3.playFromMP3Folder(mp3_num);
        delay(5000);
      }
    }
  }

  myMP3.sleep();
  digitalWrite(Mosfet,LOW);
  // shut down 5V with MOSFET here!

  if(did_move){
    esp_sleep_enable_timer_wakeup(14.4E+9); // 4 hours in microseconds
    Serial.println("4h deep sleep");
  }
  else{
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1); //1 = High, 0 = Low
    Serial.println("deep sleep, wake up on movement");
  }

esp_deep_sleep_start();
}
