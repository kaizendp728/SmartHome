#include <SoftwareSerial.h>
#include <SerialCommand.h>
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
extern "C" {
  #include "user_interface.h"
}
SerialCommand sCmd;
SocketIOClient client;
const char* ssid = "Sinh Vien";
const char* password = "sinhvien1997@!";
char host[] = "192.168.1.7";
int port = 3000;
char namespace_esp8266[] = "esp8266";
extern String RID;
extern String Rfull;
unsigned long previousMillis = 0;
long interval = 2000;
void setup() {
    Serial.begin(57600);
    delay(10);
    Serial.print("Ket noi vao mang ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print('.');
    }
    Serial.println();
    Serial.println(F("Da ket noi WiFi"));
    Serial.println(F("Di chi IP cua ESP8266 (Socket Client ESP8266): "));
    Serial.println(WiFi.localIP());
 
    if (!client.connect(host, port, namespace_esp8266)) {
        Serial.println(F("Ket noi den socket server that bai!"));
        return;
    }
    else {
        client.send("Mac", "abc" , WiFi.macAddress().c_str());
        Serial.println(F("Ket noi den socket server thanh !"));
    }
    sCmd.addDefaultHandler(defaultCommand);
    Serial.println("Da san sang nhan lenh");
}
void loop() {
  if (millis() - previousMillis > interval) {
    previousMillis = millis(); 
    //gửi sự kiện "atime" là một JSON chứa tham số message có nội dung là Time please?
    client.send("atime", String(previousMillis));
  }
  if (client.monitor()) {
    Serial.print(RID);
    Serial.print('\r');
    Serial.print(Rfull);
    Serial.print('\r');
  }  
  if (!client.connected()) {
    if(client.reconnect(host, port, namespace_esp8266)) {
      client.send("Mac", "abc" , WiFi.macAddress().c_str());
    }
  }
  sCmd.readSerial();
}
void defaultCommand(String command) {
  char *json = sCmd.next();
  client.send(command, (String) json);
  Serial.print(command);
  Serial.print(' ');
  Serial.println(json);
  Serial.println("Tot lam, da gui du lieu roi, xem trong console cua Socket server di");
}




