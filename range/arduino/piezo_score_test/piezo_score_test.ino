/* Records inputs from a piezo knock sensor and displays cumulative hit count 
  
   Adjust the sensitivity of the hit based on the SCORE_THRESHOLD
   A weight secured to the back of the piezo increases hit response e.g. an M6 
   nut pressed on with blue tack.
*/

#include<TimerOne.h>

#define BAUD 9600
#define RESEND_TIMER 250
#define DEBOUNCE_LIMIT 20
#define SCORE_THRESHOLD 102

unsigned long last_send = 0UL;
boolean report_score = false;
volatile unsigned long last_hit = 0UL;
volatile boolean record_hit = false;
volatile long counter = 0UL;

void setup() {
  // open the USB serial port for reporting
  Serial.begin(BAUD);
  
  // open the scoreboard serial port
  Serial1.begin(BAUD);
  delay(100);
  // reset the scoreboard to zero on startup
  Serial1.println(F("Z"));
  
  // 1ms timer interrupt to check for hits
  Timer1.initialize(1000);
  Timer1.attachInterrupt(piezoCheck);
}

void loop() {
  // record a hit if registered
  if (record_hit == true) {
    counter++;
    Serial.println(F("HIT"));
    record_hit = false;
    report_score = true;
  }
  
  // update the scoreboard if changed OR on regular resend time
  if (report_score == true || millis() - last_send >= RESEND_TIMER) {
    Serial1.print(F("D"));
    Serial1.println(counter);
    report_score = false;
    last_send = millis();
  }
}

void piezoCheck() {
  if (analogRead(A0) >= SCORE_THRESHOLD && millis() - last_hit >= DEBOUNCE_LIMIT) {
    record_hit = true;
    last_hit = millis();
  }
}
