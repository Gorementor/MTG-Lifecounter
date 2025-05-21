#include <M5Core2.h>

// TODO: switch between split screen 2player mode / 1 player mode  if btnB pressed
// TODO: reset life totals only after btnA or btnC is pressed for 3 seconds

// Constants
const int BRIGHTNESS_FULL = 100;
const int BRIGHTNESS_DIM = 30;
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
  M5.begin();
  M5.Lcd.setRotation(1);  // Portrait
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  // Calculate layout
  lifeWidth = lifeTextSize * 6 * 3;
  lifeHeight = lifeTextSize * 8;
  lifeX = (M5.Lcd.width() - lifeWidth) / 2;
  lifeY = (M5.Lcd.height() - lifeHeight) / 2;
  deltaX = (M5.Lcd.width() - deltaWidth) / 2;

  lastInputTime = millis();
  setBrightness(BRIGHTNESS_FULL);

  drawLife();
  clearDeltaDisplay();
  drawBattery();
}

void loop() {
  M5.update();

  bool inputDetected = false;

  if (M5.BtnA.wasPressed()) {
    lifeTotal = 20;
    accumulatedDelta = 0;
    clearDeltaDisplay();
    drawLife();
    drawBattery();
    inputDetected = true;
  }

  if (M5.BtnC.wasPressed()) {
    lifeTotal = 40;
    accumulatedDelta = 0;
    clearDeltaDisplay();
    drawLife();
    drawBattery();
    inputDetected = true;
  }

  TouchPoint_t touch = M5.Touch.getPressPoint();
  static unsigned long lastTouchTime = 0;
  static bool touchHeld = false;

  if (touch.x != -1 && touch.y != -1) {
    int buttonStripHeight = 35;
    int activeHeight = M5.Lcd.height() - buttonStripHeight;

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
  M5.Lcd.fillRect(lifeX - 5, lifeY - 5, lifeWidth + 10, lifeHeight + 10, BLACK);

  String text = String(lifeTotal);
  int textW = text.length() * 6 * lifeTextSize;
  int textX = lifeX + (lifeWidth - textW) / 2;

  M5.Lcd.setTextSize(lifeTextSize);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(textX, lifeY);
  M5.Lcd.print(text);

  lastLifeTotal = lifeTotal;
}

void drawDelta(int delta) {
  int yAbove = lifeY - deltaHeight - 20;
  int yBelow = lifeY + lifeHeight + 10;

  // Format with sign manually to avoid Print issues
  String deltaStr = (delta > 0 ? "+" : "-") + String(abs(delta));

  int textW = deltaStr.length() * 6 * deltaTextSize;
  int textX = deltaX + (deltaWidth - textW) / 2;

  // Clear both delta areas first
  clearDeltaDisplay();

  M5.Lcd.setTextSize(deltaTextSize);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(textX, (delta > 0 ? yAbove : yBelow));
  M5.Lcd.print(deltaStr);
}

void clearDeltaDisplay() {
  int yAbove = lifeY - deltaHeight - 20;
  int yBelow = lifeY + lifeHeight + 10;
  M5.Lcd.fillRect(deltaX - 5, yAbove - 5, deltaWidth + 10, deltaHeight + 10, BLACK);
  M5.Lcd.fillRect(deltaX - 5, yBelow - 5, deltaWidth + 10, deltaHeight + 10, BLACK);
}

void drawBattery() {
  float voltage = M5.Axp.GetBatVoltage();
  int percent = voltageToPercent(voltage);
  bool isCharging = M5.Axp.isCharging() && M5.Axp.isVBUS();

  if (percent == lastBatteryPercent && isCharging == lastCharging) return;

  M5.Lcd.fillRect(0, 0, 120, 20, BLACK);

  int level = map(percent, 0, 100, 0, 16);
  uint16_t color = (percent > 10) ? GREEN : RED;

  M5.Lcd.drawRect(5, 5, 20, 10, WHITE);
  M5.Lcd.fillRect(25, 8, 2, 4, WHITE);
  M5.Lcd.fillRect(7, 7, level, 6, color);

  M5.Lcd.setCursor(30, 5);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("%d%%", percent);

  if (isCharging) {
    M5.Lcd.setCursor(70, 5);
    M5.Lcd.print("CHG");
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
  M5.Axp.ScreenBreath(percent);  // Sets actual backlight voltage
}
