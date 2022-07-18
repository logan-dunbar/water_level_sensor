// TODO: Overwrite with actual values

// ThingsBoard
const String tbHost = "http://12.345.678.90";
const String tbAccessToken = "AABBccbCDASDSvV";
const String tbUrl = tbHost + "/api/v1/" + tbAccessToken + "/telemetry";

// example http://12.345.678.90/api/v1/AABBccbCDASDSvV/telemetry";

// WiFi (TODO: make array/configurable) 
const char* ssid1 = "WiFi SSID";
const char* password1 = "password";

const char* ssid2 = "2nd WiFi SSID";
const char* password2 = "password";

// Configuration for NTP
const char* ntp_primary = "time.google.com";
const char* ntp_secondary = "pool.ntp.org";
