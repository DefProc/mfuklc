/* control module to run all the things */

#include <Wire.h>
#include <SL018.h>

#define DEBUG 0

#define BAUD 9600
#define RESEND_TIMER 50
#define RUN_TIMER 30250
#define UPDATE_DELAY 10
#define RFID_DIGITS 15
#define BUTTON 2
#define RESET 3
#define LED 13
#define RFID 4
#define TARGET Serial1
#define SCOREBD Serial2
#define TIMER Serial3

//states
#define RFID_READY 1
#define WAIT_FOR_START 2
#define START_THE_GAME 3
#define RUNNING 4
#define CLEAN_UP 5

SL018 rfid;

char player_rfid[RFID_DIGITS];
long score = 0L;
int state = RFID_READY;
unsigned long start_time = 0UL;
unsigned long last_update = 0UL;

void setup() {
  Wire.begin();
  Serial.begin(BAUD);  // Computer/USB connection
  TARGET.begin(BAUD); // Target module
  SCOREBD.begin(BAUD); // Scoreboard module
  TIMER.begin(BAUD); // Timer module
  
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(RESET, INPUT);
  pinMode(RFID, INPUT_PULLUP);
}

void loop() {

  if (state == RFID_READY) {
    //waiting for new RFID scan
    stopall();
    // hold for rfid read
    rfid.seekTag();
    while (!rfid.available());
    strcpy(player_rfid, rfid.getTagString());
#if DEBUG > 0 
    Serial.print(F("rfid: "));
    Serial.println(player_rfid);
#endif    
    state = WAIT_FOR_START;
  } else if (state == WAIT_FOR_START) {
    sendPlayerID();
    resetall();
    while(digitalRead(BUTTON) == LOW && digitalRead(RESET) == LOW) {
      //pulse the led to show we're waiting for input
      float val = (exp(sin(millis()/1000.0*PI)) - 0.36787944)*108.0;
      analogWrite(LED, val);
    } 
    digitalWrite(LED, LOW);
    if (digitalRead(BUTTON) == HIGH) {
      state = START_THE_GAME;
    } else {
      state = RFID_READY;
    }
  } 
  
  else if (state == START_THE_GAME) {
    //set the initial conditions for play
#if DEBUG > 0 
    Serial.println(F("Start the game"));
#endif    
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
    Serial.print(F("0x"));
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
  TIMER.println(F("Z"));
  SCOREBD.println(F("X"));
}

void resetall() {
  SCOREBD.println(F("D0"));
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
//  if (TARGET.available()) {
    score = TARGET.parseInt();
//  }
#if DEBUG > 1
  Serial.println(score);
#endif
  SCOREBD.print(F("D"));
  SCOREBD.println(score);
}

void start() {
  score = 0UL;
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
  delay(900);
  //start listening to the targets
  TARGET.println(F("S"));
  TARGET.println(F("D"));
  //start the timer
  TIMER.println(F("S"));
  //then display the zero
  SCOREBD.println(F("Z"));
  start_time = millis();
  last_update = millis() + RESEND_TIMER;
}  
