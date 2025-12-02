#include <M5Cardputer.h>
#include <M5Unified.h>
#include <math.h>
#include "splash.h"
float ax, ay, az;

// Double-buffer sprite
m5gfx::LGFX_Sprite sprite(&M5.Display);

// -------------------------------
// SCALING
// -------------------------------
float xScale = 1.05;
float yScale = 1.00;

// -------------------------------
// SMOOTHING
// -------------------------------
float smoothing = 0.3;  // LOWER = smoother, HIGHER = faster
float smoothPitch = 0;
float smoothRoll = 0;
float smoothBX = 0;
float smoothBY = 0;

// -------------------------------
// SAFE SCALING FUNCTIONS (accept float for higher precision)
// -------------------------------
int scaleX(float x, int cx) {
  int sx = (int)roundf(cx + (x - cx) * xScale);
  if (sx < 0) sx = 0;
  if (sx >= sprite.width()) sx = sprite.width() - 1;
  return sx;
}

int scaleY(float y, int cy) {
  int sy = (int)roundf(cy + (y - cy) * yScale);
  if (sy < 0) sy = 0;
  if (sy >= sprite.height()) sy = sprite.height() - 1;
  return sy;
}

void drawLineScaled(m5gfx::LGFX_Sprite* s,
                    float x1, float y1, float x2, float y2,
                    uint16_t c, int cx, int cy) {
  s->drawLine(scaleX(x1, cx), scaleY(y1, cy),
              scaleX(x2, cx), scaleY(y2, cy), c);
}

void drawCircleScaled(m5gfx::LGFX_Sprite* s,
                      float cx_f, float cy_f, int r, uint16_t c) {
  // iterate in degrees and draw pixel-by-pixel applying scaling
  for (int deg = 0; deg < 360; deg++) {
    float rad = deg * 0.017453292519943295f;
    float x = cx_f + r * cosf(rad);
    float y = cy_f + r * sinf(rad);
    s->drawPixel(scaleX(x, (int)roundf(cx_f)), scaleY(y, (int)roundf(cy_f)), c);
  }
}

// -------------------------------
// MARKINGS
// -------------------------------
void drawDegreeMarks(m5gfx::LGFX_Sprite* s, int cx, int cy, int radius) {
  for (int deg = 0; deg < 360; deg += 5) {

    int drawDeg = deg - 180;
    if (drawDeg < 0) drawDeg += 360;

    float rad = drawDeg * 0.017453292519943295f;

    int inner = (deg % 15 == 0) ? radius - 12 : radius - 8;
    int outer = radius;

    float x1 = cx + inner * cosf(rad);
    float y1 = cy + inner * sinf(rad);
    float x2 = cx + outer * cosf(rad);
    float y2 = cy + outer * sinf(rad);

    uint16_t color = (deg % 15 == 0) ? WHITE : DARKGREY;

    drawLineScaled(s, x1, y1, x2, y2, color, cx, cy);

    // Unscaled text
    if (deg % 90 == 0) {
      int tx = cx + (radius - 20) * cosf(rad);
      int ty = cy + (radius - 20) * sinf(rad);

      s->setTextColor(WHITE);
      s->setTextSize(1);

      switch (deg) {
        case 0:
          s->setCursor(tx - 6, ty - 5);
          s->print("0");
          break;
        case 90:
          s->setCursor(tx - 6, ty - 5);
          s->print("90");
          break;
        case 180:
          s->setCursor(tx - 10, ty - 5);
          s->print("180");
          break;
        case 270:
          s->setCursor(tx - 10, ty - 5);
          s->print("270");
          break;
      }
    }
  }
}

// SETUP ---------------------------------
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  splash();
  M5.Imu.begin();
  sprite.cp437(true);
  sprite.createSprite(M5.Display.width(), M5.Display.height());
  sprite.setTextColor(WHITE);
  sprite.setTextSize(2);
}

// LOOP ----------------------------------
void loop() {
  M5.Imu.getAccel(&ax, &ay, &az);

  float pitch = atan2f(-ay, sqrtf(ax * ax + az * az)) * 57.29577951308232f;
  float roll = atan2f(ax, sqrtf(ay * ay + az * az)) * 57.29577951308232f;

  // ANGLE SMOOTHING
  smoothPitch += (pitch - smoothPitch) * smoothing;
  smoothRoll += (roll - smoothRoll) * smoothing;

  sprite.fillScreen(BLACK);

  int cx = (sprite.width() / 2);
  int cy = sprite.height() / 2;

  int radius = 55;

  // Scaled instrument
  drawCircleScaled(&sprite, (float)cx, (float)cy, radius, WHITE);

  drawLineScaled(&sprite, cx - radius, cy, cx + radius, cy, DARKGREY, cx, cy);
  drawLineScaled(&sprite, cx, cy - radius, cx, cy + radius, DARKGREY, cx, cy);

  drawDegreeMarks(&sprite, cx, cy, radius);

  // BUBBLE TARGET (based on smoothed angles)
  float targetBX = cx + smoothRoll * 1.5f;
  float targetBY = cy + smoothPitch * 1.5f;

  // SMOOTHED BUBBLE POSITION
  smoothBX += (targetBX - smoothBX) * smoothing;
  smoothBY += (targetBY - smoothBY) * smoothing;

  // limit inside radius
  float dx = smoothBX - cx;
  float dy = smoothBY - cy;
  float dist = sqrtf(dx * dx + dy * dy);

  if (dist > radius - 7) {
    smoothBX = cx + dx * (radius - 7) / dist;
    smoothBY = cy + dy * (radius - 7) / dist;
    dx = smoothBX - cx;
    dy = smoothBY - cy;
    dist = sqrtf(dx * dx + dy * dy);
  }

  int bubbleRadius = 8;

  drawCircleScaled(&sprite, smoothBX, smoothBY, bubbleRadius, CYAN);

  drawLineScaled(&sprite, smoothBX - bubbleRadius, smoothBY,
                 smoothBX + bubbleRadius, smoothBY, CYAN, cx, cy);

  drawLineScaled(&sprite, smoothBX, smoothBY - bubbleRadius,
                 smoothBX, smoothBY + bubbleRadius, CYAN, cx, cy);

  // BUBBLE ANGLE (from smoothed position)
  float bubbleAngle = atan2f(dy, dx) * 57.29577951308232f;  // -180..180
  bubbleAngle += 180.0f;                                    // rotate 180°
  // normalize 0..360
  if (bubbleAngle < 0.0f) bubbleAngle += 360.0f;
  if (bubbleAngle >= 360.0f) bubbleAngle -= 360.0f;

  // TEXT (NOW AT 0,0) includes Angle

  sprite.setTextColor(WHITE);
  sprite.setTextSize(1);

  sprite.setCursor(184, (sprite.height() / 2) - 3);
  sprite.printf("%.1f", smoothRoll);
  sprite.setCursor((sprite.width() / 2) - 10, 0);
  sprite.printf("%.1f", smoothPitch);
  sprite.setTextSize(2);
  sprite.setCursor(0, (sprite.height() / 2) - 22);
  sprite.printf("-----\n%.1f\n-----", bubbleAngle);

  // --- calculate battery ---
  float batteryVoltage = (float)M5.Power.getBatteryVoltage() / 1000;
  int batteryPercent = (int)((batteryVoltage - 3.3) / (4.2 - 3.3) * 100);
  //batteryPercent = constrain(batteryPercent, 0, 100);

  // --- show battery ---
  if (batteryPercent == 100) {
    sprite.setCursor(166, 0);
  } else {
    sprite.setCursor(168, 0);
  }

  sprite.setTextColor(WHITE);
  sprite.setTextSize(1);
  sprite.printf("%.2f V (%d%%)", batteryVoltage, batteryPercent);
  sprite.setCursor(166, 6);
  sprite.printf("------------");

  sprite.pushSprite(0, 0);
  delay(16);
}
