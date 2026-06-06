#include <IRremote.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int IRPin = 2;
const int Btns[10] = {22, 12, 24, 94, 8, 28, 90, 66, 82, 74};
const int noteFreq[10] = {523, 587, 659, 698, 784, 880, 988, 1047, 1175, 1319};
const int buzzerPin = 11;

Adafruit_SSD1306 oled(128, 32, &Wire, -1);

int gameState = 0, target, score;

// --- Timer config ---
unsigned long roundStartTime = 0;
int timeLimit = 3000;        // Starting time limit in ms
const int TIME_MIN = 800;    // Minimum time limit (ms)
const int TIME_STEP = 100;   // How much faster each round gets

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IRPin, ENABLE_LED_FEEDBACK);
  pinMode(buzzerPin, OUTPUT);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  showScreen("press any button", "to play");
}

void loop() {
  // Check timer expiry during active game
  if (gameState == 1) {
    unsigned long elapsed = millis() - roundStartTime;
    int remaining = timeLimit - (int)elapsed;

    if (remaining <= 0) {
      gameOver();  // Time ran out
      return;
    }

    // Show countdown on screen (updates ~every 100ms feels smooth enough)
    // Only refresh display here; IR check below handles input
    showTimerScreen(remaining);
  }

  if (IrReceiver.decode()) {
    int cmd = IrReceiver.decodedIRData.command;
    for (int i = 0; i < 10; i++) {
      if (cmd == Btns[i]) {
        Serial.print("You pressed Button: ");
        Serial.println(i);
        if (gameState == 0) {
          startGame();
        } else if (gameState == 1) {
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

// Shows target + countdown bar on the OLED
void showTimerScreen(int remainingMs) {
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  // Top line: score
  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.print("SCORE: ");
  oled.print(score);

  // Middle: target number
  oled.setTextSize(2);
  oled.setCursor(15, 12);
  oled.print("HIT ");
  oled.print(target);

  // Bottom: timer bar (128px wide)
  int barWidth = map(remainingMs, 0, timeLimit, 0, 124);
  barWidth = constrain(barWidth, 0, 124);
  oled.drawRect(0, 28, 128, 4, SSD1306_WHITE);   // outline
  oled.fillRect(2, 29, barWidth, 2, SSD1306_WHITE); // fill

  oled.display();
}

void startGame() {
  score = 0;
  timeLimit = 3000;   // Reset speed on new game
  gameState = 1;
  nextRound();
}

void correctAnswer() {
  score++;

  // Speed up: reduce time limit, but don't go below minimum
  timeLimit = max(TIME_MIN, timeLimit - TIME_STEP);

  showScreen("SCORE: " + String(score), "CORRECT!");
  playCustomTone(1047, 100);
  delay(500);
  nextRound();
}

void nextRound() {
  target = random(0, 10);
  roundStartTime = millis();   // Reset timer for this round
  // Initial display; loop() will keep refreshing it
  showTimerScreen(timeLimit);
}

void gameOver() {
  gameState = 0;
  showScreen("GAME OVER", "SCORE: " + String(score));
  playCustomTone(200, 300);
  delay(100);
  playCustomTone(150, 500);
}
