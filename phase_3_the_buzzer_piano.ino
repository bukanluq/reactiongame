#include <IRremote.h>

const int IRPin = 2;
const int Btns[5] = {22, 12, 24, 94, 8};
const int noteFreq[5] = {523, 587, 659, 698, 784};
const int buzzerPin = 11;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IRPin, ENABLE_LED_FEEDBACK);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  if (IrReceiver.decode()) {
    int cmd = IrReceiver.decodedIRData.command;
    for (int i = 0; i < 5; i++) {
        if (cmd == Btns[i]) {
          playCustomTone(noteFreq[i], 150);
          Serial.print("You pressed Button: ");
          Serial.println(i);
        }
    }
    IrReceiver.resume();
  }
}

void playCustomTone(int freq, int durationMs) {
  long period = 1000000L / freq;
  long halfPeriod = period / 2;        
  long cycles = (long)freq * durationMs / 1000L;
  for (long i = 0; i < cycles; i++) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(halfPeriod);
  }
}




