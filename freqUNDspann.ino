#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <DS1307RTC.h>

File myfile;
int analog = 0;
float voltage = 0;

long end;
long start;
int val;
int min;
int max;
float sum;

void setup() {
  Serial.begin( 9600 );
  
  pinMode(10, OUTPUT);

  Serial.print("Initializing card...");
  if (!SD.begin(10)) {
  Serial.println("initialization of the SD card failed!");
  return;}
  Serial.println("initialization of the SDcard is done.");
  myfile = SD.open("UF_Data.txt", FILE_WRITE);
  
}

void loop() {
  //Voltage Measurment
  analogReference(INTERNAL);
  sum = 0.00;
  for(int i = 0; i<500; i++){
    if (myfile){
      analog = analogRead(0);
      voltage = analog/4.09247826;
      sum = sum + voltage;
      }  
      else{
        Serial.println("error opening the text file!");
     }
  }

  //Frequency Measurment
  analogReference(INTERNAL);
  end = micros();
  start = end;
  for (int i = 0; i < 50; i++){
      period();
      }
  end = micros();
  float freq = ((50*1000000.*0.98741)/(end - start));

  //Write down Timestamp
  tmElements_t tm;

  if (RTC.read(tm)) {
    myfile.print(tm.Day);
    myfile.print('.');
    myfile.print(tm.Month);
    myfile.print('.');
    myfile.print(tmYearToCalendar(tm.Year));
    myfile.print(" ");
    myfile.print(tm.Hour);
    myfile.print(':');
    myfile.print(tm.Minute);
    myfile.print(':');
    myfile.print(tm.Second);
    myfile.print(";");
    } 
  else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
    }
  
  //Write down Voltage in [V]
  myfile.print(sum/500);
  Serial.print(sum/500);
  myfile.print(';');
  
  //Write down Frequency in [Hz]
  myfile.print(freq,3);
  Serial.println(freq,3);
  myfile.print('\n');    

  // close the file:
  myfile.flush();
  Serial.println("done saving.");
  delay(1000);
}

void period()
{
  while ( analogReadPlus() > 50 );
  while ( analogReadPlus() < 100 );
}

int analogReadPlus() 
{
  val = analogRead(2);
  if ( val < min ){
    min = val;
  }
  if ( val > max ) {
    max = val;
  }
  return val;
}

