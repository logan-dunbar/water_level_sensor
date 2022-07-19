#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "WiFiClient.h"
#include <time.h>
#include "config.h";
//#include "WiFiAutoSelector.h"

#define WIFI_CONNECT_TIMEOUT 10000

const unsigned long wifiTimeout = 5000L;
const unsigned long timeTimeout = 10000L;

//WiFiAutoSelector wifiAutoSelector(WIFI_CONNECT_TIMEOUT);

//void setup() { }
//void loop() { }

//void setup() {
////  wifiAutoSelector.add(ssid1, password1);
////  wifiAutoSelector.add(ssid2, password2);
////  wifiAutoSelector.add(ssid3, password3);
//
//  Serial.begin(9600);
////  while (!Serial);
//
//  connectWifi(ssid1, password1, 10000L);
//  Serial.println("Testing");
//}
//
//float dummy_level = 100.0;
//void loop() {
//  if (connectWifi(ssid1, password1, 10000L)) {
//    delay(100);
//
//    Serial.println("***Success***");
//
//    String water_level = String(dummy_level, 2);
//    String data = getDataString(water_level);
//    espPrintln(String("[data] ") + data);
//
//    Serial.println(String("free memory: ") + ESP.getFreeHeap());
//
//    httpPost(tbUrl, data);
//    Serial.println("END");
//
//  } else {
//    Serial.println("***Failed***");
//  }
//
////  String water_level = String(dummy_level, 2);
////
////  if (setupWifi()) {
////    String data = getDataString(water_level);
////    espPrintln(String("[data] ") + data);
////
////    httpPost(tbUrl.c_str(), data);
////    Serial.println("END");
////  }
////
//  dummy_level += random(10, 100);
//  WiFi.disconnect();
//  delay(10000L);
//}

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.begin(9600);
  Serial.setTimeout(5000);
  while (!Serial);
  Serial.print("READY");
  delay(10);

  if (Serial.find("START")) {
    String water_level_reading = Serial.readStringUntil('\n');
    double water_level = water_level_reading.toDouble();
    if (water_level > 0) {
      String data = getDataString(String(water_level, 2));
      espPrintln(String("[data] ") + data);

      if (setupWifi()) {
        delay(10);
        httpPost(tbUrl, data);
        Serial.println("END");
      }
    } else {
      espPrintln("no/bad reading received");
    }
  } else {
    espPrintln("timed out waiting for START");
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
  //  data += ",\"ssid\":\"";
  //  data += ssid + "\"";
  //  data += ",\"rssi\": ";
  //  data += rssi;
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
      handleHttpResponse(http, httpCode);
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
      handleHttpResponse(http, httpCode);
    } else {
      httpPrintln("unable to connect");
    }
  }
}

void handleHttpResponse(HTTPClient &http, int httpCode) {
  // httpCode will be negative on Arduino error
  httpPrintln(String("response code: ") + httpCode);
  if (httpCode > 0) {
    // call success, normal http codes apply from here
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      // file found at server
      String payload = http.getString();
      httpPrintln("payload:");
      Serial.println(payload);
      httpPrintln("payload end:");
    }
  } else {
    httpPrintln(String("failed with error: ") + http.errorToString(httpCode));
  }

  http.end();
}

// ##############
// WiFi stuff
// ##############
bool setupWifi() {
  //  if (WiFi.status() != WL_CONNECTED) {
  //    wifiPrintln("connecting to WiFi");
  //    if (-1 < wifiAutoSelector.scanAndConnect()) {
  //      int connectedIndex = wifiAutoSelector.getConnectedIndex();
  //      wifiPrintln(String("connected to ") + wifiAutoSelector.getSSID(connectedIndex));
  //      WiFi.setAutoReconnect(false);
  //      WiFi.persistent(false);
  //    } else {
  //      wifiPrintln("connection failed");
  //      return false;
  //    }
  //  } else {
  //    wifiPrintln("already connected");
  //  }
  //
  //  return true;

  bool conn = WiFi.status() == WL_CONNECTED;
  conn = !conn && connectWifi(ssid2, password2, wifiTimeout);
  conn = !conn && connectWifi(ssid1, password1, wifiTimeout);
  //  if (conn) {
  //    timeSync();
  //  }

  return conn;
}

bool connectWifi(const char* ssid, const char* password, unsigned long timeout) {
  unsigned long now = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifiPrintln(String("connecting to WiFi: ") + ssid);
  bool conn;
  while (!(conn = WiFi.status() == WL_CONNECTED) && millis() < now + timeout) {
    delay(100);
  }

  wifiPrintln(conn ? String(ssid) + " connected! " + WiFi.RSSI() : String(ssid) + " timed out...");
  return conn;
}

void timeSync() {
  unsigned long now = millis();
  configTime(0, 0, ntp_primary, ntp_secondary);
  wifiPrintln("waiting on time sync...");
  while (time(nullptr) < 1510644967 && millis() < now + timeTimeout) {
    delay(10);
  }

  wifiPrintln(time(nullptr) < 1510644967 ? "sync timed out..." : "synced!");
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
