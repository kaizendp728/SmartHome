#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <ArduinoJson.h>
#include <Wire.h>;
#include <LiquidCrystal_I2C.h>

SerialCommand sCmd;
int ledESP = 3;
int leds[] = {4, 5, 6, 7};
int button[4] = {8, 9, 10, 11};
boolean ledStatus[4] = {0, 0, 0, 0};
boolean buttonStatus[4] = {0, 0, 0, 0};
boolean buttonLongPress[4] = {0, 0, 0, 0};// Mặc định là không có sự kiện nhấn giữ.
unsigned long waitTime = 100; //nhấn giữ 100 mili giây để hệ thống xem đó là một sự kiến nhấn giữ.
unsigned long lastChangedTime[4] = {}; //Lưu thời gian thay đổi cuối cùng.
boolean reading[4] = {} ;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
  lcd.begin();
  for (int i = 0; i < sizeof(leds) / sizeof(int); i++) {
    pinMode(leds[i], OUTPUT);
  }
  for (int i = 0; i < sizeof(button) / sizeof(int); i++) {
    pinMode(button[i], INPUT);
  }
  Serial.begin(57600);
  sCmd.addCommand("COM", funcXuLy);

  lcd.begin();
  lcd.setCursor(2, 0);
  lcd.print("Wellcome to");
  lcd.setCursor(3, 1);
  lcd.print("Smart Home");
}
void loop() {
  sCmd.readSerial();
  for (int i = 0; i < sizeof(button) / sizeof(int); i++) {
    reading[i] = digitalRead(button[i]);
    if (reading[i] != buttonStatus[i]) {
      buttonStatus[i] = reading[i]; //Cập nhập trạng thái cuối cùng.
      lastChangedTime[i] = millis(); //Cập nhập thời gian
    }
    if (millis() - lastChangedTime[i] > waitTime) {
      buttonLongPress[i] = reading[i];
      lastChangedTime[i] = millis();
    }
    if (buttonLongPress[i] == true) {
      StaticJsonBuffer<200> jsonBuffer2;
      JsonObject& root2 = jsonBuffer2.createObject();
      ledStatus[i] = !ledStatus[i];
      buttonLongPress[i] = false;
      root2["led"] = i;
      root2["sig"] = ledStatus[i];
      Serial.print("LEDSTATUS");
      Serial.print('\r');
      root2.printTo(Serial);
      Serial.print('\r');
    }
    digitalWrite(leds[i], ledStatus[i]);
  }
}

void funcXuLy() {
  char *json = sCmd.next();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  int target = root["target"];
  if (target == 1) {
    Serial.print("Vao");
    digitalWrite(ledESP, root["data"]);
  }
  if (target == 2) {
    int led = root["data"][0]; // Den
    int sig = root["data"][1]; // Tin hieu
    Serial.println(led);
    Serial.println(sig);
    digitalWrite(leds[led - 1], sig);
    ledStatus[led - 1] = sig;
  }
}


