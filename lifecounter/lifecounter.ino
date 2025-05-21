#include <M5Unified.h>

// TODO: switch between split screen 2player mode / 1 player mode  if btnB pressed
// TODO: 5 * inc / dec if touch zone is hold for 1 sec

// Constants
const int BRIGHTNESS_FULL = 80;
const int BRIGHTNESS_DIM = 10;
const int MIN_LIFE = 0;
const int MAX_LIFE = 999;
const int repeatInterval = 400;
const int deltaTextSize = 2;
const int deltaHeight = 8 * deltaTextSize;
const int deltaWidth = 80;
const unsigned long deltaTimeout = 500;
const unsigned long dimTimeout = 5000;  // 5 seconds

// State
int lifeTotal = 20;
int lastLifeTotal = -1;
int accumulatedDelta = 0;

unsigned long lastDeltaUpdateTime = 0;
unsigned long lastInputTime = 0;

int lastBatteryPercent = -1;
bool lastCharging = false;
bool isDimmed = false;

// Dimensions
const int lifeTextSize = 8;
int lifeX, lifeY, lifeWidth, lifeHeight;
int deltaX;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Display.setRotation(1);  // Portrait
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);

  // Calculate layout
  lifeWidth = lifeTextSize * 6 * 3;
  lifeHeight = lifeTextSize * 8;
  lifeX = (M5.Display.width() - lifeWidth) / 2;
  lifeY = (M5.Display.height() - lifeHeight) / 2;
  deltaX = (M5.Display.width() - deltaWidth) / 2;

  lastInputTime = millis();
  setBrightness(BRIGHTNESS_FULL);

  drawLife();
  clearDeltaDisplay();
  drawBattery();
}

void loop() {
  M5.update();

  bool inputDetected = false;

  if (M5.BtnA.pressedFor(1000)) {
    if (lifeTotal != 20) {
      lifeTotal = 20;
      accumulatedDelta = 0;
      clearDeltaDisplay();
      drawLife();
      drawBattery();
    }
    inputDetected = true;
  }

  if (M5.BtnC.pressedFor(1000)) {
    if (lifeTotal != 40) {
      lifeTotal = 40;
      accumulatedDelta = 0;
      clearDeltaDisplay();
      drawLife();
      drawBattery();
    }
    inputDetected = true;
  }

  auto touch = M5.Touch.getDetail();
  static unsigned long lastTouchTime = 0;
  static bool touchHeld = false;

  if (touch.isPressed()) {
    int buttonStripHeight = 35;
    int activeHeight = M5.Display.height() - buttonStripHeight;

    if (!touchHeld || (millis() - lastTouchTime > repeatInterval)) {
      int prev = lifeTotal;

      if (touch.y < activeHeight / 2) {
        if (lifeTotal < MAX_LIFE) lifeTotal++;
      } else if (touch.y < activeHeight) {
        if (lifeTotal > MIN_LIFE) lifeTotal--;
      }

      int delta = lifeTotal - prev;
      if (delta != 0) {
        accumulatedDelta += delta;
        lastDeltaUpdateTime = millis();
        drawLife();
        drawDelta(accumulatedDelta);
        drawBattery();
      }

      lastTouchTime = millis();
      touchHeld = true;
      inputDetected = true;
    }
  } else {
    touchHeld = false;
  }

  if (accumulatedDelta != 0 && (millis() - lastDeltaUpdateTime > deltaTimeout)) {
    clearDeltaDisplay();
    accumulatedDelta = 0;
  }

  if (inputDetected) {
    lastInputTime = millis();
    if (isDimmed) {
      setBrightness(BRIGHTNESS_FULL);
      isDimmed = false;
    }
  } else if (!isDimmed && (millis() - lastInputTime > dimTimeout)) {
    setBrightness(BRIGHTNESS_DIM);
    isDimmed = true;
  }

  delay(50);
}

void drawLife() {
  M5.Display.fillRect(lifeX - 5, lifeY - 5, lifeWidth + 10, lifeHeight + 10, BLACK);

  String text = String(lifeTotal);
  int textW = text.length() * 6 * lifeTextSize;
  int textX = lifeX + (lifeWidth - textW) / 2;

  M5.Display.setTextSize(lifeTextSize);
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(textX, lifeY);
  M5.Display.print(text);

  lastLifeTotal = lifeTotal;
}

void drawDelta(int delta) {
  int yAbove = lifeY - deltaHeight - 20;
  int yBelow = lifeY + lifeHeight + 10;

  String deltaStr = (delta > 0 ? "+" : "-") + String(abs(delta));
  int textW = deltaStr.length() * 6 * deltaTextSize;
  int textX = deltaX + (deltaWidth - textW) / 2;

  clearDeltaDisplay();

  M5.Display.setTextSize(deltaTextSize);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setCursor(textX, (delta > 0 ? yAbove : yBelow));
  M5.Display.print(deltaStr);
}

void clearDeltaDisplay() {
  int yAbove = lifeY - deltaHeight - 20;
  int yBelow = lifeY + lifeHeight + 10;
  M5.Display.fillRect(deltaX - 5, yAbove - 5, deltaWidth + 10, deltaHeight + 10, BLACK);
  M5.Display.fillRect(deltaX - 5, yBelow - 5, deltaWidth + 10, deltaHeight + 10, BLACK);
}

void drawBattery() {
  float voltage = M5.Power.getBatteryVoltage();
  int percent = voltageToPercent(voltage);
  bool isCharging = M5.Power.isCharging();

  if (percent == lastBatteryPercent && isCharging == lastCharging) return;

  M5.Display.fillRect(0, 0, 120, 20, BLACK);

  int level = map(percent, 0, 100, 0, 16);
  uint16_t color = (percent > 10) ? GREEN : RED;

  M5.Display.drawRect(5, 5, 20, 10, WHITE);
  M5.Display.fillRect(25, 8, 2, 4, WHITE);
  M5.Display.fillRect(7, 7, level, 6, color);

  M5.Display.setCursor(30, 5);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(WHITE);
  M5.Display.printf("%d%%", percent);

  if (isCharging) {
    M5.Display.setCursor(70, 5);
    M5.Display.print("CHG");
  }

  lastBatteryPercent = percent;
  lastCharging = isCharging;
}

int voltageToPercent(float voltage) {
  voltage = constrain(voltage, 3.0, 4.2);
  return int(((voltage - 3.0) / (4.2 - 3.0)) * 100);
}

void setBrightness(int percent) {
  percent = constrain(percent, 0, 100);
  M5.Display.setBrightness(percent);
}
