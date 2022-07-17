#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "WiFiClient.h"
#include <time.h>
#include "config.h";

//void setup() {
//  Serial.begin(9600);
//  while (!Serial);
//  Serial.print("READY");
//  
////  if (Serial.find("START")) {
////    String water_level = Serial.readStringUntil('\n');
//    String water_level = "12345.00";
//    String data = getDataString(water_level);
//    espPrintln(String("[data] ") + data);
//
//    setupWifi();
//
//    httpPost(tbUrl.c_str(), data);
//    Serial.println("END");
////  }
//
//  ESP.deepSleep(0);
//}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.print("READY");
  
  if (Serial.find("START")) {
    String water_level = Serial.readStringUntil('\n');
    String data = getDataString(water_level);
    espPrintln(String("[data] ") + data);

    setupWifi();

    httpPost(tbUrl.c_str(), data);
    Serial.println("END");
  }

  ESP.deepSleep(0);
}

void loop() { }

// ##############
// HTTP stuff
// ##############
String getDataString(String water_level) {
  String data = "{\"water_level\":";
  data += water_level;
  return data + "}";
}

void httpGet(String url) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    httpPrintln("begin...");
    if (http.begin(client, url)) {
      httpPrintln("GET...");
      int httpCode = http.GET();
      handleHttpResponse(&http, httpCode);
    } else {
      httpPrintln("unable to connect");
    }
  }
}

void httpPost(String url, String data) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    httpPrintln("begin...");
    if (http.begin(client, url)) {
      httpPrintln("POST...");
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(data);
      handleHttpResponse(&http, httpCode);
    } else {
      httpPrintln("unable to connect");
    }
  }
}

void handleHttpResponse(HTTPClient *http, int httpCode) {
  // httpCode will be negative on Arduino error
  httpPrintln(String("response code: ") + httpCode);
  if (httpCode > 0) {
    // call success, normal http codes apply from here
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      // file found at server
      String payload = http->getString();
      httpPrintln("payload:");
      Serial.println(payload);
      httpPrintln("payload end:");
    }
  } else {
    //    Serial.printf("[HTTP] GET... failed, error: %s\n", http->errorToString(httpCode).c_str());
    httpPrintln(String("failed with error: ") + http->errorToString(httpCode));
  }

  http->end();
}

// ##############
// WiFi stuff
// ##############
static void setupWifi() {
  connectWifi();
  timeSync();
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifiPrintln(String("connecting to WiFi: ") + ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  wifiPrintln("connected!");
}

void timeSync() {
  configTime(0, 0, ntp_primary, ntp_secondary);
  wifiPrintln("waiting on time sync...");
  while (time(nullptr) < 1510644967) {
    delay(10);
  }

  wifiPrintln("synced!");
}

// ##############
// String helpers
// ##############
void espPrintln(String str) {
  Serial.println(String("[esp] ") + str);
}

void espPrint(String str) {
  Serial.print(String("[esp] ") + str);
}

void httpPrintln(String str) {
  espPrintln(String("[http] ") + str);
}

void httpPrint(String str) {
  espPrint(String("[http] ") + str);
}

void wifiPrintln(String str) {
  espPrintln(String("[wifi] ") + str);
}

void wifiPrint(String str) {
  espPrint(String("[wifi] ") + str);
}

//void setup() {
//  Serial.begin(115200);
//  setupWifi();
//  //  httpGet(tbUrl);
//}
//
//void loop() {
//  //  ESP.wdtDisable();
//  String data = "{\"water_level\":";
//  data += random(5000, 25000);
//  data += "}";
//  Serial.println("data: " + data);
//  httpPost(tbUrl, data);
////  httpPost(tbUrl, "{\"water_level\":12345}");
//  //  ESP.wdtEnable(0);
//  //  httpGet("http://example.phpoc.com/");
//  delay(1e4);
//}

//  String str = Serial.readStringUntil('\n');
//  int idx = str.indexOf("water_level=");
//  if (idx >= 0) {
//    String water_level = str.substring(idx + 12);
//
//    setupWifi();
//
//    Serial.println("data: " + data);
//    httpPost(tbUrl, data);
//    Serial.print("END\n");
//  } else {
//    Serial.println("water_level not found!");
//  }
