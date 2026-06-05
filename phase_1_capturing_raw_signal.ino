#include <IRremote.h>

const int IRPin = 2;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IRPin, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    int cmd = IrReceiver.decodedIRData.command;
    Serial.println(cmd);
    IrReceiver.resume();
  }
}
