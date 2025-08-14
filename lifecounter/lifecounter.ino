#include <M5Unified.h>

// Constants
const int BRIGHTNESS_FULL = 80;
const int BRIGHTNESS_DIM = 40;
const int MIN_LIFE = 0;
const int MAX_LIFE = 999;
const int repeatInterval = 400;
const int deltaTextSize = 2;
const int deltaHeight = 8 * deltaTextSize;
const int deltaWidth = 80;
const unsigned long deltaTimeout = 500;
const unsigned long dimTimeout = 5000;  // 5 seconds

// State
bool twoPlayerMode = false;

int lifeTotal1 = 20;
int lifeTotal2 = 20;
int lastLifeTotal1 = -1;
int lastLifeTotal2 = -1;

int accumulatedDelta1 = 0;
int accumulatedDelta2 = 0;
unsigned long lastDeltaUpdateTime1 = 0;
unsigned long lastDeltaUpdateTime2 = 0;

unsigned long lastInputTime = 0;
int lastBatteryPercent = -1;
bool lastCharging = false;
bool isDimmed = false;

// Dimensions
const int lifeTextSize = 10;
int lifeWidth, lifeHeight;
int lifeY;
int deltaX1, deltaX2;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Display.setRotation(1);  // Portrait
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);

  // Layout
  lifeWidth = lifeTextSize * 6 * 3;
  lifeHeight = lifeTextSize * 8;
  lifeY = (M5.Display.height() - lifeHeight) / 2;
  deltaX1 = (M5.Display.width() / 4) - (deltaWidth / 2);
  deltaX2 = (3 * M5.Display.width() / 4) - (deltaWidth / 2);

  lastInputTime = millis();
  setBrightness(BRIGHTNESS_FULL);

  drawAll();
}

void loop() {
  M5.update();
  bool inputDetected = false;

  // Button A: Reset Player 1
  if (M5.BtnA.pressedFor(1000)) {
    if (lifeTotal1 != 20) {
      lifeTotal1 = 20;
      accumulatedDelta1 = 0;
      drawAll();
    }
    inputDetected = true;
  }

  // Button C: Reset Player 2 (or to 40 in 1P mode)
  if (M5.BtnC.pressedFor(1000)) {
    if (twoPlayerMode) {
      if (lifeTotal2 != 20) {
        lifeTotal2 = 20;
        accumulatedDelta2 = 0;
        drawAll();
      }
    } else {
      if (lifeTotal1 != 40) {
        lifeTotal1 = 40;
        accumulatedDelta1 = 0;
        drawAll();
      }
    }
    inputDetected = true;
  }

  // Button B: Toggle mode
  if (M5.BtnB.wasPressed()) {
    twoPlayerMode = !twoPlayerMode;
    accumulatedDelta1 = accumulatedDelta2 = 0;
    M5.Display.fillScreen(BLACK);
    drawAll();
    inputDetected = true;
  }

  auto touch = M5.Touch.getDetail();
  static bool touchActive = false;
  static unsigned long touchStartTime = 0;
  static unsigned long lastTouchTime = 0;
  static bool longPressTriggered = false;

  if (touch.isPressed()) {
    if (!touchActive) {
      touchActive = true;
      touchStartTime = millis();
      longPressTriggered = false;
    }

    unsigned long heldTime = millis() - touchStartTime;
    int buttonStripHeight = 35;
    int activeHeight = M5.Display.height() - buttonStripHeight;

    int* life = &lifeTotal1;
    int* delta = &accumulatedDelta1;
    unsigned long* deltaTime = &lastDeltaUpdateTime1;
    int touchSide = 0; // 0 for left/player1, 1 for right/player2

    if (twoPlayerMode) {
      if (touch.x > M5.Display.width() / 2) {
        life = &lifeTotal2;
        delta = &accumulatedDelta2;
        deltaTime = &lastDeltaUpdateTime2;
        touchSide = 1;
      }
    }

    if (heldTime >= 1000) {
      if (!longPressTriggered || (millis() - lastTouchTime > repeatInterval)) {
        int prev = *life;
        int d = 0;
        if (touch.y < activeHeight / 2) {
          d = min(5, MAX_LIFE - *life);
        } else if (touch.y < activeHeight) {
          d = -min(5, *life - MIN_LIFE);
        }

        if (d != 0) {
          *life += d;
          *delta += d;
          *deltaTime = millis();
          drawAll();
        }

        lastTouchTime = millis();
        longPressTriggered = true;
        inputDetected = true;
      }
    }
  } else if (touchActive) {
    // Short press
    if (!longPressTriggered) {
      int* life = &lifeTotal1;
      int* delta = &accumulatedDelta1;
      unsigned long* deltaTime = &lastDeltaUpdateTime1;

      if (twoPlayerMode && touch.x > M5.Display.width() / 2) {
        life = &lifeTotal2;
        delta = &accumulatedDelta2;
        deltaTime = &lastDeltaUpdateTime2;
      }

      int prev = *life;
      int d = 0;
      int buttonStripHeight = 35;
      int activeHeight = M5.Display.height() - buttonStripHeight;

      if (touch.y < activeHeight / 2) {
        d = min(1, MAX_LIFE - *life);
      } else if (touch.y < activeHeight) {
        d = -min(1, *life - MIN_LIFE);
      }

      if (d != 0) {
        *life += d;
        *delta += d;
        *deltaTime = millis();
        drawAll();
        inputDetected = true;
      }
    }

    touchActive = false;
  }

  // Delta timeout
  if (accumulatedDelta1 != 0 && millis() - lastDeltaUpdateTime1 > deltaTimeout) {
    accumulatedDelta1 = 0;
    drawAll();
  }
  if (accumulatedDelta2 != 0 && millis() - lastDeltaUpdateTime2 > deltaTimeout) {
    accumulatedDelta2 = 0;
    drawAll();
  }

  // Auto-dim
  if (inputDetected) {
    lastInputTime = millis();
    if (isDimmed) {
      setBrightness(BRIGHTNESS_FULL);
      isDimmed = false;
    }
  } else if (!isDimmed && millis() - lastInputTime > dimTimeout) {
    setBrightness(BRIGHTNESS_DIM);
    isDimmed = true;
  }

  delay(50);
}

// Drawing helpers

void drawAll() {
  M5.Display.fillScreen(BLACK);

  // Force battery redraw by resetting last cached values
  lastBatteryPercent = -1;
  lastCharging = !M5.Power.isCharging(); // Flip it so drawBattery() always redraws

  if (twoPlayerMode) {
    drawLife(lifeTotal1, 0, accumulatedDelta1);
    drawLife(lifeTotal2, 1, accumulatedDelta2);
    
    // Draw vertical separator line
    int centerX = M5.Display.width() / 2;
    M5.Display.drawLine(centerX, 0, centerX, M5.Display.height(), WHITE);
  } else {
    drawLife(lifeTotal1, -1, accumulatedDelta1); // Full screen
  }

  drawBattery();
}


void drawLife(int value, int side, int delta) {
  int x, w;
  if (side == -1) {
    x = (M5.Display.width() - lifeWidth) / 2;
    w = lifeWidth;
  } else {
    w = M5.Display.width() / 2;
    x = (side == 0) ? (w - lifeWidth) / 2 : w + (w - lifeWidth) / 2;
  }

  String text = String(value);
  int textW = text.length() * 6 * lifeTextSize;
  int textX = x + (lifeWidth - textW) / 2;

  M5.Display.setTextSize(lifeTextSize);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setCursor(textX, lifeY);
  M5.Display.print(text);

  if (delta != 0) {
    int dy = (delta > 0) ? (lifeY - deltaHeight - 20) : (lifeY + lifeHeight + 10);
    String deltaStr = (delta > 0 ? "+" : "-") + String(abs(delta));
    int dw = deltaStr.length() * 6 * deltaTextSize;
    int dx = x + (lifeWidth - dw) / 2;

    M5.Display.setTextSize(deltaTextSize);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setCursor(dx, dy);
    M5.Display.print(deltaStr);
  }
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
