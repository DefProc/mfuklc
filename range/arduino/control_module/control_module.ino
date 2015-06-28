/* control module to run all the things */

#include <Wire.h>
#include <SL018.h>
#include <SoftwareSerial.h>

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    38      // VS1053 Data/command select pin (output)
// These are common pins between breakout and shield
// Changed for the DefProc MEGA
#define CARDCS 34     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
// Changed for the DefProc MEGA
#define DREQ 18       // VS1053 Data request, ideally an Interrupt pin
// No free interrupt pin on the MFUKLC MEGA so trying number 24 

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
  // create shield-example object!
  //Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);


////


#define DEBUG 0
// set to one to get stuff
// set to 2 to get score

#define BAUD 9600
#define RESEND_TIMER 50
#define RUN_TIMER 30250
#define UPDATE_DELAY 10
#define RFID_DIGITS 15
#define BUTTON 2
#define RESET 3
#define LED 13
#define RFID 4
#define TARGET Serial2
#define SCOREBD Serial3
#define TIMER Serial4

//states
#define RFID_READY 1
#define WAIT_FOR_START 2
#define START_THE_GAME 3
#define RUNNING 4
#define CLEAN_UP 5

SL018 rfid;

boolean hurryUp = false;
boolean halfWay = false;
char player_rfid[RFID_DIGITS];
long score = 0L;
int state = RFID_READY;
unsigned long start_time = 0UL;
unsigned long last_update = 0UL;

SoftwareSerial Serial4(11, 12); //new timer serial soft

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
  
  
  ////////////////////////////////////////
  /// Adafruit VS1053                  ///
  ///                                  ///
  ////////////////////////////////////////
  
  //Serial.println("Adafruit VS1053 Library Test in MFUKLC control module");
  // initialise the music player //////////////////////////////
  if (! musicPlayer.begin()) { // initialise the music player
     //Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  //Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  //Serial.println("SD OK!");
  
  delay(250);
  // list files
  //printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(1,1);
  
  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
//  musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
  delay (250);



  //Serial.println(F("Volume Set"));
  //Serial.println(F("Playing YOUWIN.MP3"));
  musicPlayer.playFullFile("YOUWIN.MP3");
  //Serial.println("Ok We're all set! Give me your RFID!");
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
    Serial.println("Playing FullFile FANFARE.MP3");
#endif    
    musicPlayer.playFullFile("FANFARE.MP3");
    state = WAIT_FOR_START;
  } else if (state == WAIT_FOR_START) {
    //Serial.print("waiting for start");
    sendPlayerID();
    resetall();
    //Serial.print("waiting for button");
    while(digitalRead(BUTTON) == LOW && digitalRead(RESET) == LOW) {
      //Serial.print(".");
      //pulse the led to show we're waiting for input
      float val = (exp(sin(millis()/1000.0*PI)) - 0.36787944)*108.0;
      analogWrite(LED, val);
    } 
    //Serial.println("button pressed");
    digitalWrite(LED, LOW);
    if (digitalRead(BUTTON) == HIGH) {
      state = START_THE_GAME;
      //Serial.println("start");
    } else {
      state = RFID_READY;
      //Serial.println("ready");
    }
  } 
  
  else if (state == START_THE_GAME) {
    //set the initial conditions for play
#if DEBUG > 0 
    Serial.println(F("Starting the game"));
#endif    
    musicPlayer.playFullFile("FANFAR2.mp3");
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
     
   
      
      else if (millis() - start_time >= 14750 && halfWay == true){
        musicPlayer.startPlayingFile("POWERUP.WAV");
        //Serial.println("Playing on interrupt POWERUP");
        //Serial.println("halfWay = false");
        halfWay = false;
        
      }
     
        else if (millis() - start_time >= 25000 && hurryUp == true){
        musicPlayer.startPlayingFile("HURRYUP.WAV");
        //Serial.println("Playing on interrupt HURRRYUP");
        //Serial.println("halfWay = false");
        hurryUp = false;
      }
    } else {
      state = CLEAN_UP;
    }
  } else if (state == CLEAN_UP) {
    //stop the target module and display the score

    musicPlayer.playFullFile("MARPIP2.mp3");
    //Serial.println("Playing FullFile MARPIP2.MP3");


    stopall();
    getScore();
    

    
    //report the <rfid>,<score> back to any listening device
    Serial.print(F("0x"));
    Serial.print(player_rfid);
    Serial.print(',');
    Serial.print(score);
    Serial.println();
    state = RFID_READY;
    delay(2000);
    if (score < 0){
      musicPlayer.playFullFile("GAMEOVR2.MP3");
      //Serial.println("Playing FullFile GAMEOVR2.MP3");
    }
    else if (score == 0){
      musicPlayer.playFullFile("GAMEOVR3.MP3");
      //Serial.println("Playing FullFile GAMEOVR3.MP3");
    }
     else if (score < 100){
      musicPlayer.playFullFile("TWINKL.MP3");
      //Serial.println("Playing FullFile GAMEOVR3.MP3");
    }
     else if (score < 4500){
      musicPlayer.playFullFile("POWERUP.WAV");
      //Serial.println("Playing FullFile GAMEOVR3.MP3");
    } else {
      musicPlayer.playFullFile("SPOT9.MP3");
    }
  

}
}

void stopall() {

  TARGET.println(F("X"));
  TIMER.println(F("X"));
  TIMER.println(F("Z"));
  SCOREBD.println(F("X"));
}

void resetall() {
#if DEBUG > 0 
  Serial.println("reset all");
#endif
  SCOREBD.println(F("D0"));
  TIMER.println(F("R"));
  TARGET.println(F("X"));
//  // clear the serial buffer until we get zeros
//  // (max. 10 tries)
  TARGET.println(F("D"));
//  delay(UPDATE_DELAY);
//  long ret_score = TARGET.parseInt();
//  int i = 0;
//  if (i < 10 && ret_score != 0) {
//    TARGET.println(F("D"));
//    delay(UPDATE_DELAY);
//    ret_score = TARGET.parseInt();
//    i++;
//  }
}

void sendPlayerID() {
#if DEBUG > 0
  Serial.println("sending Player ID");
#endif
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
/*  
  if (score == 0){
    musicPlayer.startPlayingFile(".MP3");
    Serial.println("Playing File on Interrupt MARPIP1.MP3");
  }
*/  
}s

void start() {
  hurryUp = true;
  halfWay = true;
  //Serial.println("hurryUp = true");
  //Serial.println("halfWay = true");
  score = 0UL;
  int mario = 900;
  //do the start
  //make a countdown on the scoreboard
  musicPlayer.startPlayingFile("GETREADY.MP3");
  delay(mario);
  musicPlayer.startPlayingFile("5.MP3");
  //Serial.println("Playing File on Interrupt 5.MP3");
  SCOREBD.println(F("D5"));
  //Serial.println("5");
  delay(mario);
  musicPlayer.startPlayingFile("4.MP3");
  //Serial.println("Playing File on Interrupt 4.MP3");
  SCOREBD.println(F("D4"));
  //Serial.println("4");
  delay(mario);
  musicPlayer.startPlayingFile("MARPIP1.MP3");
  //Serial.println("Playing File on Interrupt MARPIP1.MP3");
  SCOREBD.println(F("D3"));
  //Serial.println("3");
  delay(mario);
  musicPlayer.startPlayingFile("MARPIP1.MP3");
  //Serial.println("Playing File on Interrupt MARPIP1.MP3");
  SCOREBD.println(F("D2"));
  //Serial.println("2");
  delay(mario);
  musicPlayer.startPlayingFile("MARPIP1.MP3");
  //Serial.println("Playing File on Interrupt MARPIP1.MP3");
  SCOREBD.println(F("D1"));
  //Serial.println("1");
  delay(900);
  musicPlayer.startPlayingFile("MARPIP2.MP3");
  //Serial.println("Playing File on Interrupt MARPIP2.MP3");
  //start listening to the targets
  TARGET.println(F("S"));
  TARGET.println(F("D"));
  //start the timer
  TIMER.println(F("S"));
  //then display the zero
  SCOREBD.println(F("Z"));
  start_time = millis();
  last_update = millis() + RESEND_TIMER;
  ///Serial.println("Go!");
}  


/// SD Card File listing helper
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

