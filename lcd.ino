#include <LiquidCrystal.h>

int ThermistorPin = A2;
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

volatile bool displayFlag = false; // Flag for timer interrupt

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  

  cli(); // Disable global interrupts
  TCCR1A = 0; // Set timer 1 to normal mode
  TCCR1B = 0; // Set timer 1 to normal mode
  TCNT1 = 0; // Clear the timer counter
  OCR1A = 46874; // Set the compare match value for 3 seconds at 16 MHz CPU clock frequency
  TIMSK1 = (1 << OCIE1A); // Enable CTC interrupt
  TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set prescaler to 1024

  sei(); // Enable global interrupts
}

void loop() {
  if (displayFlag) {
    Vo = analogRead(ThermistorPin);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
    T = T - 273.15;
    T = (T * 9.0) / 5.0 + 32.0;

    lcd.clear();
    lcd.print("Temp = ");
    lcd.print(T);
    lcd.print(" F");

    displayFlag = false;
  }
}

// ISR for timer compare interrupt to set the displayFlag every second
ISR(TIMER1_COMPA_vect) {
  displayFlag = true;
}
