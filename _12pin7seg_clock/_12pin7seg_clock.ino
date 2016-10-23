#include <Time.h>

/* Global constants */
const byte PIN_MOD = 2;
const byte PIN_INC = 3;

const byte PIN_DIG0 = 13;
const byte PIN_DIG1 = 12;
const byte PIN_DIG2 = 11;
const byte PIN_DIG3 = 10;

const byte PIN_SEGA = 9;
const byte PIN_SEGB = 8;
const byte PIN_SEGC = 7;
const byte PIN_SEGD = 6;
const byte PIN_SEGE = 5;
const byte PIN_SEGF = 4;
const byte PIN_SEGG = 1;
const byte PIN_SEGH = 0; /* DP */

const byte segment_pins[] = {
  PIN_SEGA, PIN_SEGB, PIN_SEGC, PIN_SEGD,
  PIN_SEGE, PIN_SEGF, PIN_SEGG, PIN_SEGH
};
const byte nsegment_pins = 8;

const unsigned long DEBOUNCE_DELAY = 200;

const byte MODE_DISP  = 0;
const byte MODE_SETH0 = 1;
const byte MODE_SETH1 = 2;
const byte MODE_SETM0 = 3;
const byte MODE_SETM1 = 4;

/* Global vairbales */
volatile byte mode = MODE_DISP;
volatile byte H0 = 0;
volatile byte H1 = 0;
volatile byte M0 = 0;
volatile byte M1 = 0;

void display_current_time();
void display_digit(byte /* digPin */, byte /* val */);
void switch_mode();
void increment_current();


void setup() {
  // put your setup code here, to run once:

 // Serial.begin(9600);
  //sSerial.print("test\n");

  mode = MODE_DISP;

  pinMode(PIN_MOD, INPUT);
  pinMode(PIN_INC, INPUT);

  pinMode(PIN_DIG0, OUTPUT);
  pinMode(PIN_DIG1, OUTPUT);
  pinMode(PIN_DIG2, OUTPUT);
  pinMode(PIN_DIG3, OUTPUT);

  pinMode(PIN_SEGA, OUTPUT);
  pinMode(PIN_SEGB, OUTPUT);
  pinMode(PIN_SEGC, OUTPUT);
  pinMode(PIN_SEGD, OUTPUT);
  pinMode(PIN_SEGE, OUTPUT);
  pinMode(PIN_SEGF, OUTPUT);
  pinMode(PIN_SEGG, OUTPUT);
  pinMode(PIN_SEGH, OUTPUT);

  //attachInterrupt(digitalPinToInterrupt(PIN_MOD), switch_mode, RISING);
  //attachInterrupt(digitalPinToInterrupt(PIN_INC), increment_current, RISING);
    setTime(15, 58, 0, 0, 0, 0);

}

void loop() {

  if (digitalRead(PIN_MOD) == HIGH) {
    switch_mode();
  }

  if (digitalRead(PIN_INC) == HIGH) {
    increment_current();
  }

  switch (mode) {
    case MODE_DISP:
      display_current_time();
      break;
      
    case MODE_SETH0:
      display_digit(PIN_DIG0, H0);
      break;
      
    case MODE_SETH1:
      display_digit(PIN_DIG1, H1);
      break;
      
    case MODE_SETM0:
      display_digit(PIN_DIG2, M0);
      break;
      
    case MODE_SETM1:
      display_digit(PIN_DIG3, M1);
      break;
      
    default:
      ;
  }
}

void display_current_time() {

  time_t t = now();
 // display_digit(PIN_DIG0, 1);
  //display_digit(PIN_DIG1, 2);
  //display_digit(PIN_DIG2, 3);
  //display_digit(PIN_DIG3, 4);


  display_digit(PIN_DIG0, hour(t) / 10);
  display_digit(PIN_DIG1, hour(t) % 10);
  display_colon();
  display_digit(PIN_DIG2, minute(t) / 10);
 display_digit(PIN_DIG3, minute(t) % 10);

  return;
}

/*
 * val must be a number between 0 and 9
 */
void display_digit(byte digPin, byte val) {

  /* A B C D E F G H */
  const byte num[] = {
    0x3f,0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f
  };
  /*
  const byte num[] = {
    B11111100,
    B01100000,
    B11011010,
    B11110010,
    B01100110,
    B10110110,
    B00111110,
    B11100000,
    B11111110, 
    B11100110
  };*/
  int i;
 
  

  for (i = 0; i < nsegment_pins; i++) {
    if (bitRead(num[val], i) == 1) {
      digitalWrite(segment_pins[i], HIGH);
    } else {
      digitalWrite(segment_pins[i], LOW);
    }
  }


  digitalWrite(digPin, HIGH);
  delay(1);
  digitalWrite(digPin, LOW);

  return;
}

void display_colon() {
  int i;

  /* Set all segment pins except for "H" (DP) to LOW */
  for (i = 0; i < nsegment_pins; i++) {
    digitalWrite(segment_pins[i], LOW);
  }
  digitalWrite(PIN_SEGH, HIGH);
  
  digitalWrite(PIN_DIG1, HIGH);
  delay(1);
  digitalWrite(PIN_DIG1, LOW);

  return;
}

void switch_mode() {

  static unsigned long lastTriggerTime = 0;
  time_t t = now();

  if (millis() - lastTriggerTime > DEBOUNCE_DELAY) {
    lastTriggerTime = millis();
  } else {
    return;
  }

  switch (mode) {
     case MODE_DISP:
      mode = MODE_SETH0;
      H0 =  hour(t) / 10;
      break;
      
     case MODE_SETH0:
      mode = MODE_SETH1;
      H1 = hour(t) % 10;
      break;
      
     case MODE_SETH1:
      mode = MODE_SETM0;
      M0 = minute(t) / 10;
      break;
      
     case MODE_SETM0:
      mode = MODE_SETM1;
      M1 = minute(t) % 10;
      break;
      
     case MODE_SETM1:
      mode = MODE_DISP;
      update_time();
      break;

     default:
      ; /* not reached */
  }

  ///Serial.print("MODE = ");
 // Serial.print(mode);
 // Serial.print("\n");

  return;
}

void update_time() {

  setTime(H0 * 10 + H1, M0 * 10 + M1, 0, 0, 0, 0);

  return;
}

void increment_current() {

  static unsigned long lastTriggerTime = 0;

  if (millis() - lastTriggerTime > DEBOUNCE_DELAY) {
    lastTriggerTime = millis();
  } else {
    return;
  }
  
  switch(mode) {
    case MODE_DISP:
      /* do nothing */
      break;
      
    case MODE_SETH0:
      H0 = (H0 + 1) % 10;
      break;
      
    case MODE_SETH1:
      H1 = (H1 + 1) % 10;
      break;
      
    case MODE_SETM0:
      M0 = (M0 + 1) % 10;
      break;
      
    case MODE_SETM1:
      M1 = (M1 + 1) % 10;
      break;

    default:
      ; /* not reached */
  }

  return;
}

