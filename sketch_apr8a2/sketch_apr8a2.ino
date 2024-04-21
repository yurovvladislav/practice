#include <ESP8266WiFi.h> //библиотека для работы с esp8266
#include <OneWire.h>  //для подключения датчиков
#include <DallasTemperature.h> //для работы с датчиком температуры
#include <PubSubClient.h> //для работы с MQTT сервером
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP_EEPROM.h>//
#include <ArduinoJson.h> 
#include <DNSServer.h> 
#define ONE_WIRE_BUS 2  //D4

const String topic = WiFi.macAddress();
const char* apname = "temperature_esp";
const byte DNS_PORT = 53;
//IPAddress apIP(192, 168, 1, 1);

ESP8266WebServer HTTP(80);
DNSServer dnsServer;
File fsUploadFile;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
char temperatureC[6];
WiFiServer server(80);
String jsonConfig = "{}";

WiFiClient espClient2;
PubSubClient client(espClient2);

struct Settings { // структура для временного хранения параметров подключения
  String ssid;
  String password;
  String mqtt_server;
  int port;
} setts;

void getTemperature() { // получение данных с датчика температуры
  float tempC;
  do {
    DS18B20.requestTemperatures();
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureC);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}

void reconnect() {// попытка подключения к MQTT серверу
  int tries2 = 8;
  while (!client.connected() && tries2 != 0) {
    Serial.print("Попытка подключения к MQTT серверу...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Подключено");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    tries2--;
  }
}

void ap_init(){ // переключение в режим точки доступа, запуск сервера
  FS_init();
  WiFi.mode(WIFI_AP); // Задаем настройки сети
  //WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  Serial.println("");
  Serial.println("Перевод ESP8266 в режим точки доступа:");
  Serial.println("Имя - temperature_esp");
  Serial.println("Пароль - ");
  WiFi.softAP(apname, "");
  //dnsServer.start(DNS_PORT, "192.168.1.1", apIP);
  //replay to all requests with same HTML
  // HTTP.onNotFound([]() {
  //   HTTP.on("/", [](){
  //     HTTP.send(200, "text/html", "OK");
  //   });
  // });
  HTTP_init();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  SPIFFS.begin();
  Serial.print("MAC адрес: ");
  Serial.println(WiFi.macAddress());
  loadConfig();
  byte tries = 15;
  WiFi.begin(setts.ssid.c_str(), setts.password.c_str()); 
  Serial.print("Подключение к WiFi");
  while (--tries && WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    DS18B20.begin();
    DS18B20.setResolution(12);
    Serial.println("");
    Serial.println("WiFi подключен");
    Serial.println("IP адрес: ");
    Serial.println(WiFi.localIP());
    client.setServer(setts.mqtt_server.c_str(), setts.port); 
  } else if (WiFi.status() != WL_CONNECTED) {
    ap_init();
  }
}


void loop() {
  HTTP.handleClient();
  if (WiFi.status() == WL_CONNECTED){
      if (!client.connected()){
        reconnect(); 
      }  
      if (client.connected()){
        client.loop();
        delay(2000);
        getTemperature();
        Serial.println("Температура = ");
        Serial.println(temperatureC);
        client.publish(topic.c_str(), temperatureC, true);
      }else if (!client.connected()){
        ap_init();
      }
    }
  delay(1);
}
