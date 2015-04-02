/* Records inputs from a piezo knock sensor and displays cumulative hit count */

#define BAUD 9600
#define RESEND_TIMER 100
#define DEBOUNCE_LIMIT 100
#define IN INT0

unsigned long last_send = 0UL;
volatile unsigned long last_strike = 0UL;
volatile boolean report_score = false;
volatile int counter = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(IN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial1.begin(BAUD);
  delay(100);
  
  // reset the scoreboard to zero on startup
  Serial1.println(F("Z"));
  
  attachInterrupt(IN, piezo, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (report_score == true || millis() - last_send >= RESEND_TIMER) {
    Serial1.print(F("D"));
    Serial1.println(counter);
    report_score = false;
    last_send = millis();
  }
}

void piezo() {
  if (millis() - last_strike >= DEBOUNCE_LIMIT) {
    report_score = true;
    counter++;
    last_strike = millis();
  }
}
