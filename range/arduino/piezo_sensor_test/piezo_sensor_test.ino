#define IN 2

volatile boolean piezo_flag = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(IN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  attachInterrupt(INT0, piezo, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (piezo_flag == true) {
    piezo_flag = false;
    digitalWrite(LED_BUILTIN, HIGH);
    delay(250);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
  }
}

void piezo() {
  piezo_flag = true;
}
