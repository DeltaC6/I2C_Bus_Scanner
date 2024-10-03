#include <Arduino.h>
#include <Wire.h>

static const uint8_t PIN_LED_HEARTBEAT  = 2;
static hw_timer_t *timer                = NULL;
static portMUX_TYPE timerMux            = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimerTick(void) {
  portENTER_CRITICAL_ISR(&timerMux);

  // Heartbeat LED
  static uint32_t ledCounter = 0;
  ledCounter++;
  if(ledCounter >= 1000) {
    ledCounter = 0;
    digitalWrite(PIN_LED_HEARTBEAT, !digitalRead(PIN_LED_HEARTBEAT));
  }
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  // Heartbeat LED
  pinMode(PIN_LED_HEARTBEAT, OUTPUT);

  // Timer
  timer = timerBegin(0, 80, true);
  timerAlarmWrite(timer, 1000, true);
  timerAttachInterrupt(timer, onTimerTick, true);
  timerAlarmEnable(timer);

  // Serial
  Serial.begin(115200);
  while(!Serial);

  // I2C
  Wire.begin();
  delay(1000);

  Serial.println();
  Serial.println("I2C Bus Scanner Application");
  Serial.println("=====");

  int frame[8][16];
  int ack = -1;
  int add = 0;

  // Scnning I2C bus from 0x00 to 0x7F
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 16; j++) {
      add = (i * 16) + j;
      
      Wire.beginTransmission(add);
      ack = Wire.endTransmission();
      
      if(ack == 0) {
        frame[i][j] = add;
      } else {
        frame[i][j] = 0;
      }
    }
  }

  // Print frame legends
  Serial.print("X\t");
  for(int i = 0; i < 16; i++) {
    Serial.print(i, HEX);
    Serial.print("\t");
  }
  Serial.println();
  Serial.println();

  // Print frame
  for(int x = 0; x < 8; x++) {
    Serial.print(x, HEX);
    Serial.print("\t");
    for(int y = 0; y < 16; y++) {
      Serial.print(frame[x][y], HEX);
      Serial.print("\t");
    }
    Serial.println();
  }

  Serial.println("=====");
}

void loop() {
}
