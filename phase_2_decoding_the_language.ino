#include <IRremote.h>

const int IRPin = 2;
const int Btns[5] = {22, 12, 24, 94, 8};

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IRPin, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    int cmd = IrReceiver.decodedIRData.command;
    for (int i = 0; i < 5; i++) {
        if (cmd == Btns[i]) {
          Serial.print("You pressed Button: ");
          Serial.println(i);
        }
    }
    IrReceiver.resume();
  }
}

