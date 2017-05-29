#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

RTC_PCF8523 rtc;

File myfile;
int analog = 0;
float voltage = 0;
float sum = 0.00;


int value[6500];   // variable to store the value coming from the sensor
int i=0;

int count = 0;
int int_buffer = 0;
int lastvalue = -1;
float start;
float ending;
float freq;

void setup() {
  Serial.begin( 57600 );
  while (!Serial) {
    delay(1);  // for Leonardo/Micro/Zero
  }

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
  }
  pinMode(10, OUTPUT);
  analogReference(AR_DEFAULT);
  ADC->CTRLB.reg =  0x700;

  Serial.print("Initializing card...");
  if (!SD.begin(10)) {
    Serial.println("initialization of the SD card failed!");
    return;
  }
  Serial.println("initialization of the SDcard is done.");
  myfile = SD.open("UF_Data.txt", FILE_WRITE);
}

void loop() {
  
  //Voltage Measurment
  sum = 0.00;
  for (int i = 0; i < 1000; i++) {
      analog = analogRead(0);
      voltage = analog/5.4844609;
      sum = sum + voltage;
  }
 
  //Frequency Measurement
  start = micros();
  for (i=0;i<6500;i++)
  {
   value[i]=analogRead(2);
  }
  ending = micros(); 
  delay(500);
  checkit();
  delay(500);

  //Write down Timestamp
  DateTime now = rtc.now();
  myfile.print(now.day(), DEC);
  myfile.print(".");
  myfile.print(now.month(), DEC);
  myfile.print('.');
  myfile.print(now.year(), DEC);
  myfile.print(' ');
  myfile.print(now.hour(), DEC);
  myfile.print(':');
  myfile.print(now.minute(), DEC);
  myfile.print(':');
  myfile.print(now.second(), DEC);
  myfile.print(';');
  
  //Write down Voltage in [V]
  myfile.print(sum/1000);
  Serial.println(sum/1000);
  myfile.print(';');

  //Write down Frequency in [Hz]
  myfile.print(freq,3);
  Serial.println(freq,3);
  myfile.print('\n');
  

  // close the file:
  myfile.flush();
  Serial.println("done saving.");
}

void checkit(){

  int periods = 0;
  count = 0;
  for (i=0;i<6500;i++){
    int_buffer = value[i];
    
    if (lastvalue >= 1000 && int_buffer >= 1000){
        count = count +1;
//        Serial.println(count);
        if (count == 10){
          count = 0;
          periods = periods +1;
        }
      }
      else{
        count = 0;
      }
    lastvalue = int_buffer;
  
  }
    freq = (periods*1000000)/(ending-start);
}



