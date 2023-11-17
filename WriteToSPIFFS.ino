#include "FS.h"
#include "SPIFFS.h"
#include <TinyGPS++.h>


TinyGPSPlus gps;  // the TinyGPS++ object

#define RXD2 2
#define TXD2 21
#define buzzer 9
String spd, lati, lngi;
String datetime;
String dataString;
long lastDoTime = 0;

const char* fileName = "/data.csv";

void setup() {
  Serial.begin(9600);
  //Serial2.begin(GPS_BAUDRATE);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 
  pinMode(buzzer, OUTPUT);
  Serial.println(F("ESP32 - GPS module"));

      if(!SPIFFS.begin()){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
}
void loop() {
  
  getData();
  //writeDataToFile();
  delay(1000);

}

void getData()
{
 while(Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      if (gps.location.isValid()) {
        Serial.print(F("- latitude: "));
        Serial.println(gps.location.lat(),6);

        lati = String(gps.location.lat(),6);

        Serial.print(F("- longitude: "));
        Serial.println(gps.location.lng());
        lngi = String(gps.location.lng(),6);

        
      } else {
        Serial.println(F("- location: INVALID"));
      }

      Serial.print(F("- speed: "));
      if (gps.speed.isValid()) {
        //Serial.print();
            
        Serial.print(gps.speed.kmph());
        Serial.println(F(" km/h"));
        spd = String(gps.speed.kmph());

          if(gps.speed.kmph() < 60){
          Serial.println("Good Speed");
          digitalWrite(buzzer, 0);
          }

          else{
          Serial.println("Too Fast");
          digitalWrite(buzzer, 1); 
            }
            
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.print(F("- GPS date&time: "));
      if (gps.date.isValid() && gps.time.isValid()) {
        Serial.print(gps.date.year());
        Serial.print(F("-"));
        Serial.print(gps.date.month());
        Serial.print(F("-"));
        Serial.print(gps.date.day());
        Serial.print(F(" "));
        Serial.print(gps.time.hour());
        Serial.print(F(":"));
        Serial.print(gps.time.minute());
        Serial.print(F(":"));
        Serial.println(gps.time.second());

        datetime = 
         String(gps.date.year())
        + String("-")
        + String(gps.date.month())
        + String("-")
        + String(gps.date.day())
        + String(" ")
        + String(gps.time.hour())
        + String(":")
        + String(gps.time.minute())
        + String(":")
        + String(gps.time.second());
      } else {
        Serial.println(F("INVALID"));
      }

      Serial.println();
    }
  // Create a CSV-formatted string with the temperature, humidity, and current time

dataString = String(datetime) + "," + String(lati) + "," + String(lngi) + "," + spd  + "," + String(gps.satellites.value())+"\n";
//break;

doThisAtEvery(5000);
//writeDataToFile();
}



if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));

  bool recebido = false;
  while (Serial2.available()) {
     char cIn = Serial2.read();
     recebido = gps.encode(cIn);
  }
   if (gps.location.isUpdated() && gps.altitude.isUpdated())
  {

Serial.print("D/M/A: ");
Serial.print(gps.date.value());
Serial.print(" | alt: ");
Serial.print(gps.altitude.feet());
Serial.print(" | satellites: ");
Serial.println(gps.satellites.value());
//delay(500);

}


 // return dataString;
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);
  
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void writeDataToFile() {

 // getData();
  //String dataString = getData();
  Serial.println(dataString);
 
  // Check if the file exists and write the header row if it doesn't
  File file = SPIFFS.open(fileName);
  if (!file) {
    writeFile(SPIFFS, fileName, "Datetime ,Latitude ,Longitude ,Speed (km/h) \n");
  } else {
    file.close();
  }

  // Write the CSV-formatted string to the file
  appendFile(SPIFFS, fileName, dataString.c_str());

}
