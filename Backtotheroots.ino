#define FASTADC 1
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//#include <LiquidCrystal.h>
//LiquidCrystal lcd(1, 2, 4, 5, 6, 7);

#include <TimeLib.h>
#include <Time.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <DS1307RTC.h>

File myfile;

int analog = 0;
float voltage = 0;
float sum = 0.00;
float right_voltage = 0.00;
int i=0;
int count = 0;
int flag;
int analogV;
float start;
float ending;
float freq;

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  //pinMode(0, INPUT);
  //pinMode(3, INPUT);
  
#if FASTADC
 // set prescale to 4
 sbi(ADCSRA,ADPS2) ;
 cbi(ADCSRA,ADPS1) ;
 cbi(ADCSRA,ADPS0) ;
#endif
    
  //lcd.begin(16,2);
  analogReference(INTERNAL); //Spannungsreferenz für die Analogeingänge: 1,1 Volt
  Serial.print("Initializing card...");
  if (!SD.begin(4)) {
    Serial.println("initialization of the SD card failed!");
    return;
  }
  Serial.println("initialization of the SDcard is done.");
  myfile = SD.open("UF_Data.txt", FILE_WRITE);
  delay(2000);
}

void loop() {
  
  //Voltage Measurment
  sum = 0.00;
  for (int i = 0; i < 2000; i++) {
      analog = analogRead(3);
      voltage = analog/2.1874828; //5.512334 for AR_DEFAULT
      sum = sum + voltage;
  }
  sum = sum/2000;
  right_voltage = sum*0.8035 + 46.168;
  //(1.3817*pow(10,-13)*pow(sum,7) - 1.4414*pow(10,-10)*pow(sum,6) + 6.0344*pow(10,-8)*pow(sum,5) - 1.2938*pow(10,-5)*pow(sum,4) + 0.0015023*pow(sum,3) - 0.091965*pow(sum,2) + 3.4747*sum + 18.322);
  //Frequency Measurment
  delay(400);
  checkf();

  //Write down Timestamp
  tmElements_t tm;
  if (RTC.read(tm)) {
  myfile.print(tm.Day);
  myfile.print(".");
  myfile.print(tm.Month);
  myfile.print('.');
  myfile.print(tmYearToCalendar(tm.Year));
  myfile.print(' ');
  myfile.print(tm.Hour);
  myfile.print(':');  
  myfile.print(tm.Minute);
  myfile.print(':');
  myfile.print(tm.Second);
  myfile.print(';');
  }
  //Write down Voltage in [V]
  myfile.print(right_voltage,2);
  Serial.println(right_voltage,2);
  myfile.print(';');

  //Write down Frequency in [Hz]
  myfile.print(freq);
  Serial.println(freq,3);
  myfile.print('\n');
  

  // close the file:
  myfile.flush();
  Serial.println("done saving.");
  /*
  lcd.print(" ");
  lcd.print(right_voltage,2);
  lcd.print("  ");
  lcd.print(freq,3);
  lcd.print(" ");
  lcd.clear();
  */
  delay(7400);
}

void checkf(){
  flag = 1;
  count = 0;
  analogV = 8000;

  while (count != 101){
    analogV = analogRead(0);
    //Serial.println(analogV);
    if (analogV <= 500 && flag == 0){
      if (count == 0) {
        start = micros();
      }
      count = count +1;
      flag = 1;
    }
    else if (flag == 1 && analogV >= 900){
      flag = 0;
      }
    }
    ending = micros();
    freq = (100000000.00/(ending - start))-0.044;
}


