void HTTP_init(void) {

  HTTP.on("/configs.json", handle_ConfigJSON); // формирование configs.json страницы для передачи данных в web интерфейс
  // API для устройства
  HTTP.on("/ssid", handle_Set_Ssid);     // Установить имя и пароль роутера по запросу вида /ssid?ssid=home2&password=12345678
  HTTP.on("/mqtt", handle_Set_mqtt); // Установить имя и пароль для точки доступа по запросу вида /ssidap?ssidAP=home1&passwordAP=8765439
  HTTP.on("/restart", handle_Restart);   // Перезагрузка модуля по запросу вида /restart?device=ok
  // Запускаем HTTP сервер
  HTTP.begin();

}

// Установка параметров для подключения к внешней AP по запросу вида http://192.168.0.101/ssid?ssid=home2&password=12345678
void handle_Set_Ssid() {
  setts.ssid = HTTP.arg("ssid");            // Получаем значение ssid из запроса сохраняем в глобальной переменной  //setts
  setts.password = HTTP.arg("password");    // Получаем значение password из запроса сохраняем в глобальной переменной  //setts
  saveConfig();   
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
}

//Установка параметров внутренней точки доступа по запросу вида http://192.168.0.101/ssidap?ssidAP=home1&passwordAP=8765439
void handle_Set_mqtt() {              //
  setts.mqtt_server = HTTP.arg("mqtt_server");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной  //setts
  setts.port = atoi(HTTP.arg("port").c_str()); // Получаем значение passwordAP из запроса сохраняем в глобальной переменной //setts
  saveConfig();   
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
}

// Перезагрузка модуля по запросу вида http://192.168.0.101/restart?device=ok
void handle_Restart() {
  String restart = HTTP.arg("device");          // Получаем значение device из запроса
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
    delay(500);
    Serial.println("---Перезагрузка---");
    ESP.restart();                                // перезагружаем модуль
  }
  else {                                        // иначе
    HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
  }
}

void handle_ConfigJSON() {
  String json = "{";  // Формировать строку для отправки в браузер json формат
  // Имя mqtt
  json += "\"mqid\":\"";
  json += setts.mqtt_server;
  // Пароль mqtt
  json += "\"port\":\"";
  json += setts.port; 
  // Имя сети
  json += "\",\"ssid\":\"";
  json += setts.ssid; 
  // Пароль сети
  json += "\",\"password\":\"";
  json += setts.password;
  json += "\",\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\"}";
  HTTP.send(200, "text/json", json);
}
