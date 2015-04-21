/* control module to run all the things */

#include <Wire.h>
#include <SL018.h>

#define BAUD 9600
#define RESEND_TIMER 200
#define RUN_TIMER 30000
#define UPDATE_DELAY 10
#define RFID_DIGITS 15
#define BUTTON 2
#define RESET 3
#define LED 13
#define TARGET Serial1
#define SCOREBD Serial2
#define TIMER Serial3

//states
#define RFID_READY 1
#define START_THE_GAME 2
#define RUNNING 3
#define CLEAN_UP 4

SL018 rfid;

char player_rfid[RFID_DIGITS];
unsigned long score = 0;
int state = RFID_READY;
unsigned long start_time = 0UL;
unsigned long last_update = 0UL;

void setup() {
  Wire.begin();
  Serial.begin(BAUD);  // Computer/USB connection
  TARGET.begin(BAUD); // Target module
  SCOREBD.begin(BAUD); // Scoreboard module
  TIMER.begin(BAUD); // Timer module
  
  rfid.seekTag();
}

void loop() {

  if (state == RFID_READY) {
    //waiting for new RFID scan
    stopall();
    // hold for rfid read
    while (!rfid.available()); 
    strcpy(player_rfid, rfid.getTagString());
    sendPlayerID();
    resetall();
    while(digitalRead(BUTTON) == LOW && digitalRead(RESET) == LOW) {
      //pulse the led to show we're waiting for input
      float val = (exp(sin(millis()/1000.0*PI)) - 0.36787944)*108.0;
      analogWrite(LED, val);
    } 
    if (digitalRead(BUTTON) == HIGH) {
      state = START_THE_GAME;
    }
  } 
  
  else if (state == START_THE_GAME) {
    //set the initial conditions for play
    start();
    state = RUNNING;
  } 
  
  else if (state == RUNNING) {
    // check we're still in the allowed play time 
    if (millis() - start_time <= RUN_TIMER) {
      // check the frequency of score updates
      if (millis() - last_update >= RESEND_TIMER) {
        //update the score to the score boards
        getScore();
        last_update = millis();
      }
    } else {
      state = CLEAN_UP;
    }
  } 
  
  else if (state == CLEAN_UP) {
    //stop the target module and display the score
    stopall();
    getScore();
    //report the <rfid>,<score> back to any listening device
    Serial.print(player_rfid);
    Serial.print(',');
    Serial.print(score);
    Serial.println();
    state = RFID_READY;
  }
}

void stopall() {
  TARGET.println(F("X"));
  TIMER.println(F("X"));
  SCOREBD.println(F("X"));
}

void resetall() {
  SCOREBD.println(F("Z"));
  TIMER.println(F("R"));
  TARGET.println(F("X"));
}

void sendPlayerID() {
  //send the player rfid
  TARGET.print(F("R"));
  TARGET.print(player_rfid);
  TARGET.println(F(";"));
}

void getScore() {
  TARGET.println(F("D"));
  delay(UPDATE_DELAY); // need to wait for a reply
  if (TARGET.available()) {
    score = TARGET.parseInt();
  }
  SCOREBD.print(F("D"));
  SCOREBD.println(score);
}

void start() {
  //do the start
  //make a countdown on the scoreboard
  SCOREBD.println(F("D5"));
  delay(1000);
  SCOREBD.println(F("D4"));
  delay(1000);
  SCOREBD.println(F("D3"));
  delay(1000);
  SCOREBD.println(F("D2"));
  delay(1000);
  SCOREBD.println(F("D1"));
  delay(1000);
  SCOREBD.println(F("D0"));
  delay(100);
  //start listening to the targets
  TARGET.println(F("S"));
  //start the timer
  TIMER.println(F("S"));
  start_time = millis();
  last_update = millis() - (2 * RESEND_TIMER);
}
  
