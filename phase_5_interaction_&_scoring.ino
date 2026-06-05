#include <IRremote.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int IRPin = 2;
const int Btns[5] = { 22, 12, 24, 94, 8 };
const int noteFreq[5] = { 523, 587, 659, 698, 784 };
const int buzzerPin = 11;
Adafruit_SSD1306 oled(128, 32, &Wire, -1);

int gameState = 0, target, score;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IRPin, ENABLE_LED_FEEDBACK);
  pinMode(buzzerPin, OUTPUT);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showScreen("press any button", "to play");
}

void loop() {
  if (IrReceiver.decode()) {
    int cmd = IrReceiver.decodedIRData.command;
    for (int i = 0; i < 5; i++) {
      if (cmd == Btns[i]) {
        //showScreen("Button:", String(i));
        //playCustomTone(noteFreq[i], 150);
        Serial.print("You pressed Button: ");
        Serial.println(i);

        if (gameState == 0) {
          startGame();
        }

        else if (gameState == 1) {
          if (i == target) {
            playCustomTone(noteFreq[i], 150);
            correctAnswer();
          } else {
            gameOver();
          }
        }
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

void showScreen(String l1, String l2) {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print(l1);
  oled.setTextSize(2);
  oled.setCursor(15, 12);
  oled.print(l2);
  oled.display();
}


void startGame() {
  score = 0;
  gameState = 1;
  nextRound();
}

void correctAnswer() {
  score++;
  showScreen("SCORE: " + String(score), "CORRECT!");
  playCustomTone(1047, 100);
  delay(500);
  nextRound();
}

void nextRound() {
  target = random(0, 4);
  showScreen("SCORE: " + String(score), "HIT " + String(target));
}

void gameOver() {
  gameState = 0;
  showScreen("GAME OVER", "SCORE: " + String(score));
  playCustomTone(200, 300);
  delay(100);
  playCustomTone(150, 500);
}
