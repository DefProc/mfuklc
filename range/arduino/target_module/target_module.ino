/* Record hits on the analog pins from knock sensors, and store the results for retrieval

*/

#include<TimerOne.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

RTC_DS1307 RTC;
File datafile;

#define DEBUG 0

#define BAUD 9600
#define RESEND_TIMER 250
#define DEBOUNCE_LIMIT 20
#define SCORE_THRESHOLD 102
#define SPI_CS 53
#define PINS 16
#define BUFFER_LINES 300
#define RFID_DIGITS 9
#define FOLDER_BUFFER 50

// timer frequency for the ADC
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

class HitRecord {
  public:
    uint32_t timestamp;
    uint8_t  target;
    uint16_t bitfield;
    
    void setTarget(int aTarget) { bitfield = 1 << aTarget; };
    byte getTarget();
};

byte HitRecord::getTarget()
{
  byte ret = -1;
  // TODO: loop through to find the right bit that's set and store it in ret
  //find the number of left shifts required to set variable == 1
  for (byte shift=0; shift<PINS; shift++) {
    if (bitfield >> shift == 1) {
      ret = shift;
    }
  }
  return ret;
}

long scoremap[PINS] = { 10, 20, 
                        50, 100, 1234, 
                        5120, 
                        -100, 999, 
                        1024, 512, 256, 128, 
                        200, 865, 
                        6587, 
                        10 
                      };

volatile unsigned long last_hit[PINS] = {0};
HitRecord hit_record[BUFFER_LINES];
volatile int write_marker = -1;
int read_marker = -1;
int calc_marker = -1;
boolean is_running = false;
unsigned long start_time = 0UL;
boolean report_score = false;
boolean file_open = false;
unsigned long current_score = 0UL;
char player_rfid[RFID_DIGITS] = "12345678"; // first 8 chars for folder name
char filepath[FOLDER_BUFFER];


void setup() {
  // set up the ADC sampling speed
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  ADCSRA |= PS_16;    // set the prescaler to 16 (1MHz)

  // open the USB serial port for reporting
  Serial.begin(BAUD);
#if DEBUG > 0 
  Serial.println(F("Target Module"));
  Serial.println(F("============="));
#endif  
  // 1ms timer interrupt to check for hits
  Timer1.initialize(1000); 
  
  pinMode(53, OUTPUT);
  if (!SD.begin(SPI_CS)) {
#if DEBUG > 0 
    Serial.println("SD card failed, or not present");
#endif
    // don't do anything more:
    return;
  }
#if DEBUG > 0 
  Serial.println("SD card initialized.");
#endif
  
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
#if DEBUG > 0 
    Serial.println("RTC is NOT running!");
#endif
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop() {
  while (Serial.available() > 0) {
    char character = Serial.read();
    
    if (character == 'H' || character == 'h') {
      Serial.println(F("Accepted input"));
      Serial.println(F("  H - display this message"));
      Serial.println(F("  R<nnnn>; - set the rfid UID, 1-15 digits, ';' terminator"));
      Serial.println(F("       **** call before START ****"));
      Serial.println(F("  S - start scoring"));
      Serial.println(F("  X - stop scoring (finish this round)"));
      Serial.println(F("  D - display (report) the current score"));
    }
      
    // parse any incoming commands
    if (character == 'R' || character == 'r') {
      int digits = Serial.readBytesUntil(';', player_rfid, RFID_DIGITS-1);
      player_rfid[digits] = '\0';
      //Serial.print(F("RFID NUMBER: "));
      Serial.println(player_rfid);
    }
    
    if (character == 'S' || character == 's') {
      makeFileName();
      datafile = SD.open(filepath, FILE_WRITE);
#if DEBUG > 0 
      Serial.println(datafile);
#endif
      file_open = true;
      datafile.println(player_rfid);
      //reset all the pointers and score variables
      current_score = 0;
      write_marker = -1;
      read_marker = -1;
      calc_marker = -1;
      start_time = millis();
      for (int i=0; i<PINS; i++) {
        last_hit[i] = start_time;
      }
      //start running the hit monitoring
      is_running = true;
      Timer1.attachInterrupt(piezoCheck); 
#if DEBUG > 0 
      Serial.println(F("RUN"));
#endif
    }
      
    if (character == 'X' || character == 'x') {
      Timer1.detachInterrupt();
      is_running = false;
      // report_score = true;
    }

    if (character == 'D' || character == 'd') {
      report_score = true;
    }
  }
  
  //write the score lines to the SD.card if not up to date
  if ((write_marker - read_marker) % BUFFER_LINES > 0) {
    read_marker++;
    read_marker = read_marker % BUFFER_LINES;
    datafile.print(hit_record[read_marker].timestamp);
    datafile.print(',');
    datafile.print(hit_record[read_marker].getTarget()+1);
    datafile.println();
  } 
  
  //close the file if we're up to date, and finished running
  if ((write_marker - read_marker) % BUFFER_LINES == 0 && is_running == false && file_open == true) {
    datafile.println(current_score);
    datafile.close();
    file_open = false;
#if DEBUG > 0 
    Serial.println(F("STOP"));
#endif
  }
  
  //process the score to bring it up to date with the hits
  if ((write_marker - calc_marker) % BUFFER_LINES > 0) {
    //score the hits
    calc_marker++;
    calc_marker = calc_marker % BUFFER_LINES;
    current_score += scoremap[ hit_record[calc_marker].getTarget() ];
  }
  
  //report the score if requested
  if (report_score == true) {
    if (is_running == true) {
      // reply with the most recent (quicker is better)
      Serial.println(current_score);
      report_score = false;
    } else if ((write_marker - calc_marker) % BUFFER_LINES <= 0) {
      // reply once all the score lines have been processed
      Serial.println(current_score);
      report_score = false;
    }
  }
      
}

void piezoCheck() {
  // using the Mega analog pins for 16 inputs
  for (byte i=0; i<PINS; i++) {
    if (analogRead(i+A0) >= SCORE_THRESHOLD && millis() - last_hit[i] >= DEBOUNCE_LIMIT) {
      write_marker++;
      write_marker = write_marker % BUFFER_LINES;
      hit_record[write_marker].setTarget(i);
      last_hit[i] = millis();
      hit_record[write_marker].timestamp = last_hit[i] - start_time;
    }
  }
}

void getFolderPath() {
   DateTime now = RTC.now();
   snprintf(filepath, FOLDER_BUFFER, "%04d/%02d/%02d/%s", now.year(), now.month(), now.day(), player_rfid);
}

void makeFileName() {
  getFolderPath();
  if(!SD.exists(filepath)) {
    //make the folder
    SD.mkdir(filepath);
  }
  char filename[15];
  DateTime now = RTC.now();
  snprintf(filename, 14, "/%02d-%02d-%02d.txt", now.hour(), now.minute(), now.second());
  strncat(filepath, filename, FOLDER_BUFFER);
}
