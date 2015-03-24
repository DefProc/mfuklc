/* Serial controlled countdown timer for bit.ly/big-7-seg displays 
  
  Accepted input
    H - display accepted inputs
    S - start
    R - reset the time (call before Start)
    X - stop the current coutdown
    Z - zero the timer
  
*/

#define NOE 9 //pwm
#define LE 8 //latch
#define CLK 10 //clock
#define SDO 11 //serial data
#define LED 13 //indicator led on arduino

#define BAUD 9600
#define TIMER 300 // 30.0 seconds
#define TIMER_RESOLUTION 10 // 10 ms = 0.01 sec
#define HOLD_TIME 1000 // ms
#define DIGITS 3
#define DECIMAL_PLACES 1

byte segments[] = 
  {  
    0b11111100, //0
    0b01100000, //1
    0b11011010, //2
    0b11110010, //3
    0b01100110, //4
    0b10110110, //5
    0b10111110, //6
    0b11100000, //7
    0b11111110, //8
    0b11100110, //9
    0b00000001, //dot
  };
  
boolean run_flag = false;
unsigned long counter = 0UL;

void setup() {
  Serial.begin(BAUD);
  pinMode(LED,OUTPUT);
  pinMode(LE,OUTPUT);
  analogWrite(NOE,LOW);
  pinMode(CLK,OUTPUT);
  pinMode(SDO,OUTPUT);

  digitalWrite(LE,false);
  
  Serial.println(F("Coutdown Timer"));
  Serial.println(F("=============="));
  
  displayTime(0);
}

void loop() {
  // if there's any serial available, read it:
  while (Serial.available() > 0) {
    char character = Serial.read();
    
    if (character == 'h' || character == 'H') {
      Serial.println(F("Accepted input"));
      Serial.println(F("  H - display this message"));
      Serial.println(F("  S - start"));
      Serial.println(F("  R - reset the time (call before Start)"));
      Serial.println(F("  X - stop the current coutdown"));
      Serial.println(F("  Z - zero the timer"));
    }
    
    if (character == 'S' || character == 's') {
      displayTime(TIMER);
      run_flag = true;
      counter = millis();
    }
    
    if (character == 'r' || character == 'R') {
      run_flag = false;
      displayTime(TIMER);
    }
    
    if (character == 'x' || character == 'X') {
      run_flag = false;
    }

    if (character == 'z' || character == '|') {
      run_flag = false;
      displayTime(0);
    }
  }
  
  if (run_flag == true) {
    if (millis() - counter < TIMER * 100) {
      int time_to_display = TIMER - ((millis() - counter)/100);
      displayTime(time_to_display);
      //Serial.println(time_to_display);
    } else {
      displayTime(0);
      run_flag = false;
    }
  }
}

void displayTime(int number) {
  digitalWrite(LE,LOW);
  // calculate each digit for sending
  for (byte i=0; i<DIGITS; i++) {
    byte output = number % 10;
    if (i == DECIMAL_PLACES) {
      // add decimal place if specified digit
      shiftOut(SDO, CLK, LSBFIRST, segments[output] | segments[10]);
    } else {
      shiftOut(SDO, CLK, LSBFIRST, segments[output]);
    }
    number = number / 10;
  }
  digitalWrite(LE,HIGH);
}

