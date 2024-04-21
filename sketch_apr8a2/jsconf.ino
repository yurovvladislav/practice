// Загрузка данных сохраненных в файл  config.json
bool loadConfig() {
  // Открываем файл для чтения
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
  // если файл не найден  
    Serial.println("Не удалось открыть config файл");
  //  Создаем файл запиав в него данные по умолчанию
    saveConfig();
    configFile.close();
    return false;
  }
  // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Размер config файла слишком большой");
    configFile.close();
    return false;
  } 
  jsonConfig = configFile.readString();
  configFile.close();
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonConfig);
    setts.ssid = root["jsssid"].as<String>();
    setts.password = root["jspassword"].as<String>();
    setts.mqtt_server = root["jsmqtt"].as<String>();
    setts.port = root["jsport"];

  Serial.println("---------------Сохранённые параметры--------------");
  Serial.print(root["jsssid"].as<String>());
  Serial.println(" - Имя WiFi");
  Serial.print(root["jspassword"].as<String>());
  Serial.println(" - Пароль WiFi");
  Serial.print(root["jsmqtt"].as<String>());
  Serial.println(" - IP адрес MQTT сервера");
  Serial.print(root["jsport"].as<String>());
  Serial.println(" - Порт");
  Serial.println("---------------Сохранённые параметры--------------");
  return true;
}

// Запись данных в файл config.json
bool saveConfig() {
  // Резервируем памяь для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266 
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json 
  json["jsssid"] = setts.ssid;
  json["jspassword"] = setts.password;
  json["jsmqtt"] = setts.mqtt_server;
  json["jsport"] = setts.port;
  // Помещаем созданный json в глобальную переменную json.printTo(jsonConfig);
  json.printTo(jsonConfig);
  // Открываем файл для записи
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Не удалось открыть config файл для записи");
    configFile.close();
    return false;
  }
  // Записываем строку json в файл 
  json.printTo(configFile);
  Serial.println("---------------Записанные параметры--------------");
  Serial.print(json["jsssid"].as<String>());
  Serial.println(" - Имя WiFi");
  Serial.print(json["jspassword"].as<String>());
  Serial.println(" - Пароль WiFi");
  Serial.print(json["jsmqtt"].as<String>());
  Serial.println(" - IP адрес MQTT сервера");
  Serial.print(json["jsport"].as<String>());
  Serial.println(" - Порт");
  Serial.println("---------------Записанные параметры--------------");
  configFile.close();
  return true;
  }


  //{"jsssid":"","jspassword":"","jsmqtt":"","jsport":""}
